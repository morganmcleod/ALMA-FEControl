#include "FEHardwareDevice.h"
#include "logger.h"
#include "setTimeStamp.h"
#include <string>
#include <iomanip>
using namespace std;

bool FEHardwareDevice::randomizeAnalogMonitors_m(false); 
bool FEHardwareDevice::logMonitors_m(false);
bool FEHardwareDevice::logAmbErrors_m(true);
FEHardwareDevice::LogInterface FEHardwareDevice::defaultLogger_m;
FEHardwareDevice::LogInterface *FEHardwareDevice::logger_mp(NULL);

FEHardwareDevice::FEHardwareDevice(const std::string &name)
  : AmbDeviceImpl(name),
    ESN_m(),
    running(false),
    minimalMonitoring_m(false),
    stopped(true),
    paused(false),
    errorCount_m(0),
    maxErrorCount_m(0)
    {} 
    
FEHardwareDevice::~FEHardwareDevice() {
    stopMonitor();
}    

// monitor thread operations:    

void FEHardwareDevice::startMonitor() {
    if (running)
        return;
    LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): starting monitor thread..." << endl;
    pthread_create(&thread_m, NULL, reinterpret_cast<void*(*)(void*)>(monitorThread), this); 
    pthread_detach(thread_m);
    running = true;
    errorCount_m = 0;
}

void FEHardwareDevice::stopMonitor() {
    if (!running)
        return;
    LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): stopping monitor thread..." << endl;
    running = false;
    int retry = 250;
    while (!stopped && retry--)
        SLEEP(100);
    int elapsed = (250 - retry) / 10;
    if (!stopped)
        LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): NOT stopped after " << elapsed << " seconds." << endl;
}

void FEHardwareDevice::pauseMonitor(bool pause, const char *reason) {
    if (paused != pause) {
        paused = pause;
        LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): " << ((paused) ? "paused monitoring. " : "resumed monitoring. ")
                     << (reason ? reason : "") << endl; 
        if (!paused)
            errorCount_m = 0;
    }
}

void FEHardwareDevice::minimalMonitor(bool minimal, const char *reason) {
    if (minimalMonitoring_m != minimal) {
        minimalMonitoring_m = minimal;
        LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): " << ((minimalMonitoring_m) ? "minimal monitoring. " : "normal monitoring. ")
                     << (reason ? reason : "") << endl; 
    }
}

// logging helpers and operations:

const char *FEHardwareDevice::TransactionText[14] = {
    "000",
    "   ",
    "   ",
    "   ",
    "   ",
    "   ",
    "   ",
    "   ",
    "   ",
    "   ",
    "CHK",
    "MON",
    "CMD",
    "RBK"
};

// Pack and unpack functions for unsigned long:
//TODO:  move pack/unpack routines into FrontEndAMBLib:


typedef union { // A union for unsigned long/can conversion
    unsigned char chr_val[4];
    unsigned long long_val;
} AmbULConv;

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(unsigned long &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1 || dataLength == 5)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 4)
        ret = FEMC_UNPACK_ERROR;
    else {
        for (int index = 0; index < dataLength; ++index) {
            target *= 0xFF;
            target += data[index];
        }
    }
    return ret;
}

void FEHardwareDevice::pack(unsigned long value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    AmbULConv conv;
    conv.long_val = value;
    dataLength = 4;
    data[0] = conv.chr_val[3];
    data[1] = conv.chr_val[2];
    data[2] = conv.chr_val[1];
    data[3] = conv.chr_val[0];
}

// Pack and unpack functions for unsigned short:

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(unsigned short &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1 || dataLength == 3)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 2)
        ret = FEMC_UNPACK_ERROR;
    else {
        target = (data[0] & 0xff) << 8;
        target += data[1];
    }
    return ret;
}

void FEHardwareDevice::pack(unsigned short value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    dataLength = 2;
    data[0] = (value >> 8) & 0xff;  //shift and set upper byte
    data[1] = value & 0xff;         //set lower byte
}

// Pack and unpack functions for signed short:

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(short &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1 || dataLength == 3)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 2)
        ret = FEMC_UNPACK_ERROR;
    else {
        unsigned short temp = data[0] & 0xFF;
        temp <<= 8;
        temp += data[1];
        if (data[0] & 0x80) {
            // negative 2s complement number:
            temp -= 1;
            temp ^= 0xFFFF;
            target = (short) -temp;
        } else
            target = (short) temp;
        LOG(LM_DEBUG) << "unpack(short &target) data=" 
                      << uppercase << hex << setw(2) << setfill('0') << (int) data[0] << " " << (int) data[1] << dec << setw(0)
                      << " temp=" << temp << " target=" << target << endl;
    }
    return ret;
}

void FEHardwareDevice::pack(short value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    dataLength = 2;
    unsigned short temp = (unsigned short) (value);
    if (value < 0) {
        // Convert to negative 2s compliment:
        temp = (unsigned short) (-value);
        temp ^= 0xFFFF;
        temp += 1;
    }
    data[0] = (temp >> 8) & 0xff;  //shift and set upper byte
    data[1] = temp & 0xff;         //set lower byte
}


// Pack and unpack functions for unsigned char:

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(unsigned char &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1) {
        // Ambiguous whether this is just the requested data or just the status byte.
        // Treat it as a status byte if it matches the range of FEMC_ERROR, otherwise as data:
        target = data[0];
        signed char retC((signed char) target);
        if (retC < 0 && retC >= FEMC_LAST_ERROR_CODE) {
            target = 0;
            ret = (FEMC_ERROR) retC;
        }
    } else if (dataLength == 2) {
        ret = (FEMC_ERROR) ((signed char) data[dataLength - 1]);
        if (ret == FEMC_NO_ERROR)
            target = data[0];
    } else
        ret = FEMC_UNPACK_ERROR;
    return ret;
}

void FEHardwareDevice::pack(unsigned char value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    dataLength = 1;
    data[0] = value;
}

// Pack and unpack functions for float:

typedef union { // A union for float/can conversion
    unsigned char chr_val[4];
    float flt_val;
} AmbFloatConv;

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(float &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0.0;
    if (dataLength == 1 || dataLength == 5)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 4)
        ret = FEMC_UNPACK_ERROR;
    else {
        // Convert to float the incoming CAN message:
        AmbFloatConv conv;
        conv.chr_val[0] = data[3];
        conv.chr_val[1] = data[2];
        conv.chr_val[2] = data[1];
        conv.chr_val[3] = data[0];
        target = conv.flt_val;
    }
    return ret;
}

void FEHardwareDevice::pack(float value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    /* Set a float value to the CAN message*/
    AmbFloatConv conv;
    conv.flt_val = value;
    
    dataLength = 4;
    data[0] = conv.chr_val[3];
    data[1] = conv.chr_val[2];
    data[2] = conv.chr_val[1];
    data[3] = conv.chr_val[0];
}

// unpack function for std::string.  No pack is required for this type:
FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(std::string &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    target = "";
    for (int index = 0; index < dataLength; ++index)
        target += (char) data[index]; 
    return FEMC_NO_ERROR;
}

// unpack function to copy a CAN payload into an array of bytes. No pack is required for this type:  
FEHardwareDevice::FEMC_ERROR FEHardwareDevice::unpack(AmbDataMem_t *target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    if (!target)
        return FEMC_UNPACK_ERROR;
    memcpy(target, data, dataLength);
    return FEMC_NO_ERROR;
}

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::syncMonitorThreeByteRevLevel(AmbRelativeAddr RCA, std::string &target) {
    target.erase();
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(RCA, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status == AMBERR_NOERR) {
        char buf[20];
        sprintf(buf, "%d.%d.%d", (int) data[0], (int) data[1], (int) data[2]);
        target = buf;
        return FEMC_NO_ERROR;
    } else
        return FEMC_UNPACK_ERROR;
}

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::syncMonitorEightByteESN(AmbRelativeAddr RCA, std::string &target, bool reverseBytes) {
    target.erase();
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(RCA, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status == AMBERR_NOERR) {
        char buf[20];
        if (reverseBytes) {
            sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X", data[7], data[6], data[5], data[4],
                                                             data[3], data[2], data[1], data[0]);
        } else {
            sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X", data[0], data[1], data[2], data[3],
                                                             data[4], data[5], data[6], data[7]);
        }
        target = buf;
        return FEMC_NO_ERROR;
    } else
        return FEMC_UNPACK_ERROR;
}

FEHardwareDevice::FEMC_ERROR FEHardwareDevice::syncMonitorAverage(AmbRelativeAddr RCA, float &target, int average) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    sem_t synchLock;
    sem_init(&synchLock, 0, 0);
    if (average < 1)
        average = 1;
    int count(average);
    double sum(0.0);
    float val(0.0);
    bool done(false);
    while (!done) {
        ret = syncMonitor(RCA, val, synchLock);
        --count;
        // if AMB error, quit immediately:
        if (ret == FEMC_AMB_ERROR)
            done = true;
        // accumulate sum until requested number of readings done:
        else {
            sum += val;
            if (count == 0)
                done = true;
        }
    }
    sem_destroy(&synchLock);
    // calculate average:
    target = (float) (sum / (double) average);
    return ret;
}

// implement the monitor thread:
void *FEHardwareDevice::monitorThread(FEHardwareDevice *dev) {
    // refuse to run if not passed an owner pointer:
    if (!dev)
        return NULL;

    LOG(LM_TRACE) << "FEHardwareDevice(" << dev -> name_m << "): in monitor thread." << endl;
    
	while (true) {
        // signal back to the owner whether running or stopped:
        dev -> stopped = (!dev -> running);

        if (dev -> stopped) {
            // If stopped, worker thread dies:
            LOG(LM_TRACE) << "FEHardwareDevice(" << dev -> name_m << "): exiting monitor thread." << endl;
            pthread_exit(NULL);
        
        } else if (!dev -> paused) {
            // If not stopped, call the derived class' monitorAction:
    		Time timestamp;
    		setTimeStamp(&timestamp);
            dev -> monitorAction(&timestamp);
        }
        // check for too many errors meaning monitoring should be paused:
        if (dev -> exceededErrorCount())
            dev -> pauseMonitor(true, "Too many AMB/FEMC errors.");
        
        // pause between calls to monitorAction:
        SLEEP(5);
    }
	return NULL;
}

