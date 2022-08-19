#include "FEHardwareDevice.h"
#include "logger.h"
#include "setTimeStamp.h"
#include <string>
#include <iomanip>
using namespace std;

bool FEHardwareDevice::logMonitors_m(false);
bool FEHardwareDevice::logAmbErrors_m(true);
FEHardwareDevice::LogInterface FEHardwareDevice::defaultLogger_m;
FEHardwareDevice::LogInterface *FEHardwareDevice::logger_mp(NULL);

FEHardwareDevice::FEHardwareDevice(const std::string &name)
  : AmbDeviceImpl(name),
    ESN_m(),
    running_m(false),
    minimalMonitoring_m(false),
    stopped_m(true),
    paused_m(false),
    exceededErrorCount_m(false),
    errorCount_m(0),
    maxErrorCount_m(0)
    {} 
    
FEHardwareDevice::~FEHardwareDevice() {
    stopMonitor();
}    

// monitor thread operations:    

void FEHardwareDevice::startMonitor() {
    if (running_m)
        return;
    stopped_m = false;
    LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): starting monitor thread..." << endl;
    pthread_create(&thread_m, NULL, reinterpret_cast<void*(*)(void*)>(monitorThread), this); 
    pthread_detach(thread_m);
    running_m = true;
    resetErrorCount();
}

void FEHardwareDevice::stopMonitor() {
    if (!running_m)
        return;
    LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): stopping monitor thread..." << endl;
    running_m = false;
    int retry = 250;
    while (!stopped_m && retry--)
        SLEEP(100);
    int elapsed = (250 - retry) / 10;
    if (!stopped_m)
        LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): NOT stopped after " << elapsed << " seconds." << endl;
}

void FEHardwareDevice::pauseMonitor(bool pause, const char *reason) {
    if (paused_m != pause) {
        paused_m = pause;
        LOG(LM_INFO) << "FEHardwareDevice(" << name_m << "): " << ((paused_m) ? "paused monitoring. " : "resumed monitoring. ")
                     << (reason ? reason : "") << endl; 
        if (!paused_m)
            resetErrorCount();
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


FEMC_ERROR FEHardwareDevice::syncMonitorThreeByteRevLevel(AmbRelativeAddr RCA, std::string &target) {
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
        postMonitorHook(RCA);
        char buf[20];
        sprintf(buf, "%d.%d.%d", (int) data[0], (int) data[1], (int) data[2]);
        target = buf;
        return FEMC_NO_ERROR;
    } else
        return FEMC_UNPACK_ERROR;
}

FEMC_ERROR FEHardwareDevice::syncMonitorEightByteESN(AmbRelativeAddr RCA, std::string &target, bool reverseBytes) {
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
        postMonitorHook(RCA);
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

FEMC_ERROR FEHardwareDevice::syncMonitorAverage(AmbRelativeAddr RCA, float &target, int average) {
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

void FEHardwareDevice::checkExceededErrorCount() {
    if (!exceededErrorCount_m && maxErrorCount_m > 0 && errorCount_m > maxErrorCount_m) {
        exceededErrorCount_m = true;
        pauseMonitor(true, "Too many AMB/FEMC errors.");
    }
}

// implement the monitor thread:
void *FEHardwareDevice::monitorThread(FEHardwareDevice *dev) {
    // refuse to run if not passed an owner pointer:
    if (!dev)
        return NULL;

    LOG(LM_TRACE) << "FEHardwareDevice(" << dev -> name_m << "): in monitor thread." << endl;
    
	while (true) {
        // signal back to the owner whether running or stopped:
        dev -> stopped_m = (!dev -> running_m);

        if (dev -> stopped_m) {
            // If stopped, worker thread dies:
            LOG(LM_TRACE) << "FEHardwareDevice(" << dev -> name_m << "): exiting monitor thread." << endl;
            pthread_exit(NULL);
        
        } else if (!dev -> paused_m) {
            // If not paused, call the derived class' monitorAction:
    		Time timestamp;
    		setTimeStamp(&timestamp);
            dev -> monitorAction(&timestamp);
        }
        // check if we should stop because of too many errors:
        dev -> checkExceededErrorCount();

        // pause between calls to monitorAction:
        SLEEP(5);
    }
	return NULL;
}




