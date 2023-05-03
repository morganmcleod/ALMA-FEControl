#ifndef FEHARDWAREDEVICE_H_
#define FEHARDWAREDEVICE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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
 
#include <FrontEndAMB/ambDeviceImpl.h>
#include <FrontEndAMB/femcDefs.h>
#include <FrontEndAMB/messagePackUnpack.h>
#include "logger.h"
#include <tuple>
#include <list>
#include <algorithm>
#include <iomanip>

/// FEHardwareDevice is a base class common to all front end ImplBase classes.
/// It provides CAN message packing and unpacking services, synchronous monitor 
/// and control, and a monitor thread for each device.  
class FEHardwareDevice : public AmbDeviceImpl, public MessagePackUnpack {
public:
    FEHardwareDevice(const std::string &name);
    ///< construct with the device name to pass down to AmbDeviceImpl.
    virtual ~FEHardwareDevice();
    
    void setESN(const std::string &esn)
      { ESN_m = esn; }
    ///< set the Electronic Serial Number 

    const std::string &getESN() const
      { return ESN_m; }
    ///< get the Electronic Serial Number


    inline bool monitorIgnorableError(FEMC_ERROR &err)
      { return (err == FEMC_NO_ERROR || err == FEMC_HARDW_UPDATE_WARN || err == FEMC_HARDW_RETRY_WARN); }
    ///< returns true if the data associated with the error can perhaps be used anyway.
    
// monitor thread operations:    

    virtual void startMonitor();
    ///< start the monitor thread.
    virtual void stopMonitor();
    ///< stop the monitor thread.
    void pauseMonitor(bool pause, const char *reason = NULL);
    ///< pause/resume the monitor thread.
    void minimalMonitor(bool minimal, const char *reason = NULL);
    ///< reduce monitoring to the minimum if true.
    static void logMonitors(bool doLog)
      { logMonitors_m = doLog; }
    ///< enable/disable continuous logging of monitor points for all devices.
    static void logAmbErrors(bool doLog)
      { logAmbErrors_m = doLog; }
    ///< enable/disable logging of CAN bus error messages.
    bool isErrorStop() const
      { return exceededErrorCount_m; }
    ///< return whether paused, either by user or measurement process or too many errors.
    void setMaxErrorCount(unsigned maxErrorCount)
      { maxErrorCount_m = maxErrorCount; }
    ///< set the max communication errors which can happen before monitoring will automatically pause.
    void resetErrorCount()
      { errorCount_m = 0; exceededErrorCount_m = false; }
    ///< reset the error counter

// logging helpers and operations:

    /// types of device transactions which may be logged.
    typedef enum {
        FEMC_LOG_NONE = 0,
        FEMC_LOG_CHECKPOINT = 10,
        FEMC_LOG_MONITOR = 11,
        FEMC_LOG_COMMAND = 12,
        FEMC_LOG_COMMAND_READBACK = 13
    } Transaction_t;

    /// a string corresponding to each TRANSACTION_TYPE.
    static const char *TransactionText[];

    /// declare an interface for CAN transaction logging.     
    class LogInterface {
    public:
        LogInterface()
          {}
        virtual ~LogInterface()
          {}

        virtual void checkPoint(const char *text)
          {}
        ///< log a checkpoint transaction.
        ///< Default logger does nothing. 
        
        virtual void log(Transaction_t trans,       // a transaction type, from above
                         const char *text,          // monitor/control property name
                         AmbRelativeAddr RCA,       // the RCA of the transaction
                         int FEStatus,              // status code returned by the FEMC
                         unsigned long iValue,      // integer value if applicable.
                         float fValue)              // float value if applicable.
          {}                         
        ///< Log an actual transaction. 
        ///< Any of the monitor/control types can be adapted into text, fValue, or iValue.                           
        ///< Default logger does nothing. 
    };

    static void setLogger(LogInterface &logger)
      { logger_mp = &logger; }
    ///< Set the logger object to use

    static void clearLogger()
      { logger_mp = NULL; }
    ///< Set the logger object to use

    LogInterface &getLogger()
      { return (logger_mp) ? *logger_mp : defaultLogger_m; }
    ///< get a reference to the current logger   

protected:


// Monitor and control operations:
    
    /// This hook is called immediately after each completed monitor transaction:
    virtual void postMonitorHook(const AmbRelativeAddr &RCA)
      {}

    /// Monitor a three-byte software version number and return a string.
    FEMC_ERROR syncMonitorThreeByteRevLevel(AmbRelativeAddr RCA, std::string &target);

    /// Monitor an eight-byte ESN and return a string.
    FEMC_ERROR syncMonitorEightByteESN(AmbRelativeAddr RCA, std::string &target, bool reverseBytes = false);

    /// Synchronous monitor of any type supported with an unpack() function.
    /// synchLock must be initialized before and destroyed after calling.
    template<typename T>
    FEMC_ERROR syncMonitor(AmbRelativeAddr RCA, T &target, sem_t &synchLock) {
        FEMC_ERROR ret(FEMC_NO_ERROR);
        AmbErrorCode_t status(AMBERR_NOERR);
        AmbDataLength_t dataLength;
        AmbDataMem_t data[8];
        Time timestamp;
        // send monitor request with the semaphore:
        monitor(RCA, dataLength, data, &synchLock, &timestamp, &status);
        // wait on the semaphore:
        sem_wait(&synchLock);
        // check for errors and return result:
        if (status == AMBERR_NOERR) {
            postMonitorHook(RCA);
            ret = unpack(target, dataLength, data);
        } else {
            ret = FEMC_AMB_ERROR;
            if (logAmbErrors_m) {
                LOG(LM_ERROR) << "FEHardwareDevice(0x" << std::uppercase << std::hex << m_nodeAddress << "): AMB error=" << status << " RCA=" << " 0x" << std::uppercase << std::hex << std::setw(6) << std::setfill('0') << RCA << std::endl;
            }
        }
        if (!monitorIgnorableError(ret))
            ++errorCount_m;   // increment error counter.
        return ret;
    }

    /// Synchronous monitor with retries of any type supported with an unpack() function.
    template<typename T>
    FEMC_ERROR syncMonitorWithRetry(AmbRelativeAddr RCA, T &target, int retries = 12) {
        FEMC_ERROR ret(FEMC_NO_ERROR);
        sem_t synchLock;
        sem_init(&synchLock, 0, 0);
        bool done = false;
        while (!done && retries > 0) {
            ret = syncMonitor(RCA, target, synchLock);
            // if AMB error, quit without retrying:
            if (ret == FEMC_AMB_ERROR)
                done = true;
            // no AMB error.  Check for retry required:
            else if (ret == FEMC_HARDW_RETRY_WARN)
                --retries;
            // No error, or some other error than what we checked for - return:   
            else
                done = true;
        }
        sem_destroy(&synchLock);
        return ret;
    }
    
    /// Synchronous monitor a float with averaging.
    FEMC_ERROR syncMonitorAverage(AmbRelativeAddr RCA, float &target, int average);
    
    /// Template function to perform a pair of command/readback transactions at the given RCA.
    /// The 'value' is sent as the payload.
    /// FEStatus receives the status byte from the readback.
    /// pack(T, AmbDataLength_t&, AmbDataMem_t*) must be defined for T.
    template<typename T>
    FEMC_ERROR syncCommand(AmbRelativeAddr RCA, T value) {
        FEMC_ERROR ret(FEMC_NO_ERROR);
        AmbErrorCode_t status(AMBERR_NOERR);
        AmbDataLength_t dataLength;
        AmbDataMem_t data[8];
        pack(value, dataLength, data);
        Time timestamp;
        // send command with a semaphore:
        sem_t synchLock;
        sem_init(&synchLock, 0, 0);
        command(RCA, dataLength, data, &synchLock, &timestamp, &status);
        // wait on the semaphore (hence synchronous):
        sem_wait(&synchLock);
        // send a monitor request to the same RCA with the semaphore and wait:
        monitor(RCA, dataLength, data, &synchLock, NULL, NULL);
        sem_wait(&synchLock);
        sem_destroy(&synchLock);
        T temp;
        ret = unpack(temp, dataLength, data);
        return ret;
    }

    virtual void monitorAction(Time *timestamp_p) = 0;
    ///< derived classes must declare a monitorAction method for the monitor thread to call.
    
protected:
    bool minimalMonitoring_m;   ///< true if only the bare minimum monitoring should be performed.
    FEMC_ERROR lastFemcError_m; ///< store error/status of last operation.
    static bool logMonitors_m;  ///< true if analog monitor points should be continuously dumped to the log.
    static bool logAmbErrors_m; ///< true if AMB errors should be logged.
    
private:
    bool running_m;             ///< true if the monitor thread should continue to run.
    bool paused_m;              ///< true if the monitor thread is paused.
    bool exceededErrorCount_m;  ///< true if the monitor thread stopped because of too many errors.
    
    unsigned errorCount_m;      ///< count of errors seen by the monitor thread since started/unpaused.
    unsigned maxErrorCount_m;   ///< maximum error count before the thread is stopped.

    void checkExceededErrorCount();
    //< private helper to check whether maxErrorCount_m is newly exceeded.

    FEHardwareDevice(const FEHardwareDevice &other);
    ///< forbid copy construct.

    std::string ESN_m;
    ///< Storage for the device ESN.

    static LogInterface defaultLogger_m;
    ///< a do-nothing transaction logger which dumps all messages.

    static LogInterface *logger_mp;
    ///< the interface to use for all transaction logging.
    
    pthread_t thread_m; ///< the monitor thread handle.
    bool stopped_m;       ///< true when the monitor thread has stopped and may be destroyed.

    static void *monitorThread(FEHardwareDevice *dev);
    ///< the monitor thread runner function.  Calls derived class' monitorAction().
};

// These macros support declaring and defining a MONITORS_REGISTRY in a derived class having the interface:
// addMon(float *target, (pointer to member function) pf), bool isTemporary;
//  - add a monitor point and its corresponding function to the registry.
//  - if isTemporary insert it at the front and reset the iterator to service it next.
// executeNextMon()
//  - call a monitor function from the registry and put its value in the target float variable.
//  - if isTemporary, delete it after servicing.

#define DECLARE_MONITORS_REGISTRY(CLASS)                                    \
    typedef float (CLASS:: *MonFuncPtr) (void);                             \
    typedef std::tuple<float *, MonFuncPtr, bool> RegEntry;                 \
    std::list<RegEntry> monitorRegistry;                                    \
    std::list<RegEntry>::iterator nextMon;                                  \
    void addMon(float *target, MonFuncPtr pf, bool isTemporary = false) {   \
        if (isTemporary) {                                                  \
            monitorRegistry.push_front(RegEntry(target, pf, true));         \
            nextMon = monitorRegistry.begin();                              \
        } else monitorRegistry.push_back(RegEntry(target, pf, false)); }    \
    bool executeNextMon();

#define DEFINE_MONITORS_REGISTRY(CLASS)                                 \
    bool CLASS ::executeNextMon() {                                     \
        if (nextMon == monitorRegistry.end()) {                         \
            nextMon = monitorRegistry.begin();                          \
            return false;                                               \
        } else {                                                        \
            float *target; MonFuncPtr pf; bool isTemporary;             \
            std::tie(target, pf, isTemporary) = *nextMon;               \
            float val = (this ->* pf)();                                \
            if (target) *(target) = val;                                \
            if (isTemporary) nextMon = monitorRegistry.erase(nextMon);  \
            else ++nextMon;                                             \
			return true;                                                \
    }}



#endif /*FEHARDWAREDEVICE_H_*/
