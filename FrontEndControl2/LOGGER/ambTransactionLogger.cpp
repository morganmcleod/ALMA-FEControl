/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*/

#include "AmbTransactionLogger.h"
#include "logger.h"
#include "setTimeStamp.h"
#include "feAddressMeta.h"

#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

/// define the structure for items waiting to be logged.
struct AmbTransactionLogger::LogEntry {
    Time ts;
    FEHardwareDevice::Transaction_t trans;
    string text;
    AmbRelativeAddr RCA;
    int FEStatus;
    unsigned long iValue;
    float fValue;
    
    LogEntry(Time _ts = 0,
             FEHardwareDevice::Transaction_t _trans = FEHardwareDevice::FEMC_LOG_NONE,
             const char *_text = "",
             AmbRelativeAddr _RCA = 0,
             int _FEStatus = FEMC_NO_ERROR,
             unsigned long _iValue = 0,
             float _fValue = 0.0)
      : ts(_ts),
        trans(_trans),
        text(_text),
        RCA(_RCA),
        FEStatus(_FEStatus),
        iValue(_iValue),
        fValue(_fValue)
        {}
};

typedef std::vector<AmbTransactionLogger::LogEntry *> LogBuffer_t;
///< type for circular buffer and for passing sets of entries.

//----------------------------------------------------------------------------

static const int BUFFER_SIZE = 4000;

/// Define the class for the log buffer, a circular buffer of pointers to LogEntry with a mutex to protect the queue.
class AmbTransactionLogger::LogBuffer {
public:
    LogBuffer(int bufferSize = BUFFER_SIZE);
    ///< construct with the size (number of messages) in the circular buffer.

    ~LogBuffer();
    ///< destructor.  deletes all entries by calling reset().
    
    void insertEntry(LogEntry *entry);
    ///< insert the entry to be logged.  LogBuffer takes ownership of the pointer.
    
    int getEntriesToLog(LogBuffer_t &toLog);
    ///< get the sequence of entries to be logged since last call to getEntriesToLog().
    ///< items are copied into toLog and must be deleted by the caller.
    ///< Returns the number of items found. 
        
private:
    LogBuffer(const AmbTransactionLogger::LogBuffer &other);
    
    LogBuffer &operator =(const AmbTransactionLogger::LogBuffer &other);
    
    void reset();
    ///< delete all entries and set pointers to NULL.
    ///< handles locking/unlocking the mutex.
        
    int getNextInsertIndex();
    ///< get the index of the next available insert position.
    ///< handles locking/unlocking the mutex.

    int getNextLogIndex(int &nextLogBegin, int &nextLogEnd);
    ///< get the range of indexes to be logged.  If begin==end then nothing to log.
    ///< handles locking/unlocking the mutex.
    ///< returns the maximum number of items to log.
    
    void setNextLogIndex(int nextLogBegin, int numItemsLogged);
    ///< set the actual next item to be logged.  
    ///< May be different from nextLogEnd returned by getNextLogIndex if a NULL item was found before reaching nextLogEnd.

    pthread_mutex_t mutex;  ///< mutex to protect the buffer.
    int bufferSize_m;       ///< size of the circular buffer.
    int nextInsertPos_m;    ///< next available position for an entry to be inserted.
    int nextLogPos_m;       ///< next item waiting to be logged.
    int numItems_m;         ///< number of items waiting to be logged.
    LogBuffer_t buffer_m;   ///< the circular buffer of entries to log.
};

AmbTransactionLogger::LogBuffer::LogBuffer(int bufferSize)
  : bufferSize_m(bufferSize),
    nextInsertPos_m(0),
    nextLogPos_m(0),
    numItems_m(0),
    buffer_m(bufferSize_m, (LogEntry *)(NULL))
{ 
    LOG(LM_TRACE) << "AmbTransactionLogger::LogBuffer constructor" << endl;
    pthread_mutex_init(&mutex, NULL);
}

AmbTransactionLogger::LogBuffer::~LogBuffer() {
    reset(); 
    pthread_mutex_destroy(&mutex);
    LOG(LM_TRACE) << "AmbTransactionLogger::LogBuffer destructor" << endl;
}

void AmbTransactionLogger::LogBuffer::insertEntry(LogEntry *entry) {
    if (!entry)
        return;
    // get the next insert position (takes care of locking the mutex):
    int pos = getNextInsertIndex();
    if (pos < 0) {
        LOG(LM_WARNING) << "AmbTransactionLogger::LogBuffer::insertEntry:  Buffer is full." << endl;
        delete entry;
    } else {
        // and save the pointer to the new entry:
        buffer_m[pos] = entry;
    }
}
    
int AmbTransactionLogger::LogBuffer::getEntriesToLog(LogBuffer_t &toLog) {
    // get the range of indexes to return:
    int nextLogBegin, nextLogEnd, maxCount;
    maxCount = getNextLogIndex(nextLogBegin, nextLogEnd);
    // quit if nothing to log:
    if (maxCount == 0)
        return 0;
    // prepare the output buffer:    
    toLog.clear();
    toLog.reserve(maxCount);
    // move up to maxCount items from buffer_m into toLog:
    // Stop early if a NULL pointer is encountered.
    int pos(nextLogBegin), count(0);
    while (count < maxCount && buffer_m[pos] != NULL) {        
        ++count;
        // ownership of the pointer is transferred over to toLog:
        toLog.push_back(buffer_m[pos]);
        buffer_m[pos] = NULL;
        ++pos;
        if (pos == bufferSize_m)
            pos = 0;
    }
    // Save the next starting log index:
    setNextLogIndex(pos, count);
    // return the number of items actually moved:
    return count;
}
    
// private:

void AmbTransactionLogger::LogBuffer::reset() {
    pthread_mutex_lock(&mutex);
    // delete and set all entries to NULL:
    for (int index = 0; index < bufferSize_m; ++index) {
        delete buffer_m[index];
        buffer_m[index] = NULL;
    }
    nextInsertPos_m = 0,
    nextLogPos_m = 0;    
    pthread_mutex_unlock(&mutex);
}
    
int AmbTransactionLogger::LogBuffer::getNextInsertIndex() {
    pthread_mutex_lock(&mutex);
    // get the next insert position:
//    int preInsertPos(nextInsertPos_m);
//    int preLogPos(nextLogPos_m);
//    int preNumItems(numItems_m);
    int pos(nextInsertPos_m);
    // if an entry already exists then the buffer is full; return -1:
    if (buffer_m[pos] != NULL)
        pos = -1;
    // otherwise, increment and account for circular buffer wraparound: 
    else {
        ++numItems_m;
        ++nextInsertPos_m;
        if (nextInsertPos_m == bufferSize_m)
            nextInsertPos_m = 0;
    }
//    int postInsertPos(nextInsertPos_m);
//    int postNumItems(numItems_m);
    // unlock and return the saved position:
    pthread_mutex_unlock(&mutex);
    //LOG(LM_DEBUG) << "LogBuffer::getNextInsertIndex preInsertPos=" << preInsertPos 
    //              << " preNumItems=" << preNumItems << " preLogPos=" << preLogPos 
    //              << " postInsertPos=" << postInsertPos
    //              << " postNumItems=" << postNumItems << " pos=" << pos << endl;
    return pos;
}

int AmbTransactionLogger::LogBuffer::getNextLogIndex(int &nextLogBegin, int &nextLogEnd) {        
    pthread_mutex_lock(&mutex);
//    int preNumItems(numItems_m);
    // set nextLogBegin to the first available item:
    nextLogBegin = nextLogPos_m;
    // set nextLogEnd one past the last available item, = the next insert position:    
    nextLogEnd = nextInsertPos_m;
    pthread_mutex_unlock(&mutex);
    // compute and return the number of items in the range:
    int count = numItems_m; // nextLogEnd - nextLogBegin;
    //LOG(LM_DEBUG) << "LogBuffer::getNextLogIndex nextLogBegin=" << nextLogBegin << " nextLogEnd=" << nextLogEnd 
    //              << " preNumItems=" << preNumItems << " count=" << count << endl;
    return count;
}

void AmbTransactionLogger::LogBuffer::setNextLogIndex(int nextLogBegin, int numItemsLogged) {
    pthread_mutex_lock(&mutex);
//    int preLogPos(nextLogPos_m);
//    int preNumItems(numItems_m);
    nextLogPos_m = nextLogBegin;
    numItems_m -= numItemsLogged;
//    int postNumItems(numItems_m);
    pthread_mutex_unlock(&mutex);
    //LOG(LM_DEBUG) << "LogBuffer::setNextLogIndex preLogPos=" << preLogPos << " preNumItems=" << preNumItems
    //              << " nextLogPos_m=" << nextLogPos_m << " postNumItems=" << postNumItems << endl;
}    

//-----------------------------------------------------------------------------

AmbTransactionLogger::AmbTransactionLogger(bool useStream)
  : buffer_mp(new LogBuffer),
    useStream_m(useStream),
    do_shutdown(false),
    shutdown_done(false)
{
    // Create the worker thread, passing this as the thread argument:
    LOG(LM_INFO) << "AmbTransactionLogger starting logger thread..." << endl;
    pthread_create(&thread_m, NULL, reinterpret_cast<void*(*)(void*)> (loggerThread), this);
    pthread_detach(thread_m);
}

AmbTransactionLogger::~AmbTransactionLogger() {
    shutdown();
    delete buffer_mp;
}

void AmbTransactionLogger::setUseStream(bool useStream) {
	useStream_m = useStream;
}

void AmbTransactionLogger::shutdown() {
	LOG(LM_INFO) << "AmbTransactionLogger stopping logger thread..." << endl;
    do_shutdown = true;

    int retry = 250;
    while (!shutdown_done && retry--)
        SLEEP(100);

    if (shutdown_done)
    	LOG(LM_INFO) << "AmbTransactionLogger logger thread is stopped." << endl;
    else
    	LOG(LM_INFO) << "AmbTransactionLogger logger thread is NOT stopped." << endl;
}

void AmbTransactionLogger::checkPoint(const char *text) {
    LogEntry *entry = new LogEntry(0, FEHardwareDevice::FEMC_LOG_CHECKPOINT, text);
    setTimeStamp(&(entry -> ts));
    buffer_mp -> insertEntry(entry);
}    

void AmbTransactionLogger::log(FEHardwareDevice::Transaction_t trans,
                               const char *text,
                               AmbRelativeAddr RCA,
                               int FEStatus,
                               unsigned long iValue,
                               float fValue)
{
    LogEntry *entry = new LogEntry(0, trans, text, RCA, FEStatus, iValue, fValue);
    setTimeStamp(&(entry -> ts));
    buffer_mp -> insertEntry(entry);
}                               


void AmbTransactionLogger::logSingleItem(const LogEntry *entry) const {
    if (entry && useStream_m) {
        
        const char *transText = FEHardwareDevice::TransactionText[entry -> trans];

        FrontEnd::FESelector_t selector;
        selector.decode(entry -> RCA);
        std::ostringstream os;
        os << selector;       
    
        switch (entry -> trans) {
            case FEHardwareDevice::FEMC_LOG_CHECKPOINT:
            	LOGT(LM_INFO, &(entry -> ts)) << entry -> text << endl;  
                break;
            case FEHardwareDevice::FEMC_LOG_MONITOR:
            case FEHardwareDevice::FEMC_LOG_COMMAND:
            	LOGT(LM_INFO, &(entry -> ts)) << transText
            	    << " 0x" << uppercase << hex << setw(6) << setfill('0') << (unsigned long) entry -> RCA << dec << setw(0)            	
            	    << " '" << entry -> text << (os.str().length() ? " " : "") << os.str() << "'"
            	    << " i=" << entry -> iValue << " f=" << entry -> fValue << " stat=" << entry -> FEStatus << endl;
                break;
            default:
                break;
        }
    }
}

void *AmbTransactionLogger::loggerThread(AmbTransactionLogger *owner) {
    if (!owner)
        pthread_exit(NULL);

    LogBuffer_t toLog;
    LogEntry *entry;
        
    while (true) {
        if (owner -> buffer_mp -> getEntriesToLog(toLog)) {
            for (LogBuffer_t::iterator it = toLog.begin(); it != toLog.end(); ++it) {
                entry = *it;
                if (!entry)
                    LOG(LM_WARNING) << "AmbTransactionLogger: Bad entry in loggerThread!" << endl;
                else {
                    owner -> logSingleItem(entry);
                    delete entry;
                }
            }
        }
        if (owner -> do_shutdown) {    
            owner -> shutdown_done = true;
            pthread_exit(NULL);
        } else
            SLEEP(2000);
    }
}

const string &AmbTransactionLogger::statusToText(const AmbErrorCode_t *status, string &statusText) {

    statusText = "??";
    if (status) {
        switch (*status) {
            case AMBERR_NOERR:
                statusText = "OK";
                break;
            case AMBERR_BADCMD:
            case AMBERR_BADCHAN:
            case AMBERR_UNKNOWNDEV:
            case AMBERR_INITFAILED:
            case AMBERR_WRITEERR:
            case AMBERR_READERR:
                statusText = "ER";
                break;
            
            case AMBERR_TIMEOUT:
                statusText = "TO";
                break;
    
            default:
                break;
        }
    }
    return statusText;
}

