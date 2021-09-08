/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2005
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

#include "lv_wrapper.h"
#include "lv_structs.h"
#include "exchndl.h"
#include "iniFile.h"
#include "logger.h"
#include "setTimeStamp.h"
#include "stringConvert.h"
#include "splitPath.h"
#include "FrontEndAMB/NICANBusInterface.h"
#include "FrontEndAMB/SocketClientBusInterface.h"

#include "FEBASE/FEHardwareDevice.h"
#include "LOGGER/AmbTransactionLogger.h"
#include "LOGGER/logDir.h"
#include "OPTIMIZE/OptimizeBase.h"
#include "FEMCEventQueue.h"
#include "DLL/SWVersion.h"

#include <algorithm>
using namespace std;

/// Shared infrastructure for configuration, logging, database connection
///  used by the LabVIEW wrappers for the Front End and the LORTM.

namespace FrontEndLVWrapper {
    // Configuration loading:
    std::string iniFileName;            ///< the top-level FrontEndControlDLL.ini
    std::string FrontEndIni("");        ///< the FE specific ini file to use, if different from FrontEndControlDLL.ini

    // Library init and lifecycle:
    static bool LVWrapperValid = false; ///< true if all objects here were created and configured
    static int connectedModules = 0;    ///< reference count number of callers to LVWrapperInit()
    pthread_mutex_t LVWrapperLock;      ///< mutex to protect the above two vars

    // Debug options:
    std::string logDir("");             ///< output logs are created here
    bool logTransactions = false;       ///< Normally false: log all low-level CAN transactions to FELog
    bool debugLVStructures = false;     ///< Normally false: dump all monitor data results to the log
    bool CAN_noTransmit = false;        ///< Normally false: ignore CAN connection failure and suppress all CAN messages
    unsigned int thermalLogInterval = 30; ///< Seconds between rows in the thermal log file

    // Software objects we create:
    FILE *logStream = NULL;
    static const AmbInterface *ambItf;
    static CANBusInterface *canBus = NULL;
    static AmbTransactionLogger *logger = NULL;
};
using namespace FrontEndLVWrapper;

short LVWrapperInit() {
    // If first client, initialize the shared data mutex:
    if (!connectedModules)
        pthread_mutex_init(&LVWrapperLock, NULL);

    bool valid;
    int connected;

    // Increment the number of DLL clients:
    pthread_mutex_lock(&LVWrapperLock);
    ++connectedModules;
    // Copy the mutex protected vars:
    connected = connectedModules;
    valid = LVWrapperValid;
    pthread_mutex_unlock(&LVWrapperLock);

    // If the DLL is already initialized, return success:
    if (valid) {
        LOG(LM_INFO) << "LVWrapperInit: connectedModules=" << connected << endl;
        return 0;
    }

    // initialize exception handling library:
    ExcHndlInit();

    // Get the path to FrontEndControlDLL.ini from the environment or assume it in the working directory:
    char *fn=getenv("FRONTENDCONTROL.INI");
    iniFileName = (fn) ? fn : "FrontendControlDLL.ini";
    
    try {
        // Open and read the FrontEndControlDLL.ini file:
        CIniFile configINI(iniFileName);
        configINI.ReadFile();

        std::string iniPath, logDir, tmp;

        // get the path where the FrontendControlDLL.ini file is located:
        splitPath(iniFileName, iniPath, tmp);
        if (iniPath.empty())
            iniPath = ".";

        // look for the item specifying a separate file for Front End configuration:
        tmp = configINI.GetValue("configFiles", "FrontEnd");
        if (tmp.empty()) {
            // not found.  Continue using this file:
            FrontEndIni = iniFileName;
            // And don't override logDir:
            logDir = "";
        } else {
            // Found: use the current ini file path plus the specified name:
            FrontEndIni = iniPath + "/" + tmp;
            // Override logDir with the same directory:
            splitPath(FrontEndIni, logDir, tmp);
        }

        // If logDir not overridden above, load it from the setting in FrontEndControlDLL.ini:
        if (logDir.empty())
            logDir = configINI.GetValue("logger", "logDir");

        // Compose the log file and excHndl file names:
        string logFile("");
        string excHndlFile("");
        if (!logDir.empty()) {
            if (logDir[logDir.length() - 1] != '\\' && logDir[logDir.length() - 1] != '/')
                logDir += "/";
            FEConfig::setLogDir(logDir);

            Time ts;
            setTimeStamp(&ts);
            timestampToText(&ts, tmp, true);

            logFile = logDir + "FELOG-" + tmp + ".txt";
            logStream = fopen(logFile.c_str(), "w");
            StreamOutput::setStream(logStream);
            StreamLogger::setReportingLevel(LM_INFO);

            excHndlFile = logDir + "ExcHndl-" + tmp  + ".txt";
            ExcHndlSetLogFileNameA(excHndlFile.c_str());
        }

        // Report configuration so far:
        LOG(LM_INFO) << "LVWrapperInit: connectedModules=" << connectedModules << endl;
        
        LOG(LM_INFO) << "FrontEndControl library version " << FECONTROL_SW_VERSION_STRING << endl;
        
        LOG(LM_INFO) << "Using configuration file '" << iniFileName << "'" << endl;
        if (!logDir.empty())
            LOG(LM_INFO) << "Using log directory '" << logDir << "'" << endl;
        if (!logFile.empty())
            LOG(LM_INFO) << "Using log file '" << logFile << "'" << endl;
        if (!excHndlFile.empty())
            LOG(LM_INFO) << "Using ExcHndl file '" << excHndlFile << "'" << endl;

        // logTransactions = if true, every CAN message will be logged.  HUGE log file!
        tmp = configINI.GetValue("logger", "logTransactions");
        if (!tmp.empty())
            logTransactions = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "Logging FE transactions=" << logTransactions << endl;

        // CAN_debugStdout = if true, every low-level CAN frame will be logged to stdout:
        tmp = configINI.GetValue("debug", "CAN_debugStdout");
        if (!tmp.empty())
            CANBusInterface::enableDebug_m = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "CAN debug to stdout=" << CANBusInterface::enableDebug_m << endl;

        // CAN_noTransmit = if true, don't actually send CAN messages.  Useful for testing when no FE available:
        tmp = configINI.GetValue("debug", "CAN_noTransmit");
        if (!tmp.empty()) {
            CAN_noTransmit = from_string<unsigned long>(tmp);
            CANBusInterface::noTransmit_m = CAN_noTransmit;
        }
        LOG(LM_INFO) << "CAN debug no transmit=" << CANBusInterface::noTransmit_m << endl;

        // CAN_maxChannels = how many CAN queues to allocate:
        tmp = configINI.GetValue("debug", "CAN_maxChannels");
        if (!tmp.empty())
            CANBusInterface::maxChannels_m = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "CAN max channels=" << CANBusInterface::maxChannels_m << endl;

        // CAN_monitorTimeout = milliseconds timeout for monitor messages:
        //  2ms typical;  Increase when debugging FE firmware.
        tmp = configINI.GetValue("debug", "CAN_monitorTimeout");
        if (!tmp.empty())
            CANBusInterface::monitorTimeout_m = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "CAN monitor timeout=" << CANBusInterface::monitorTimeout_m << endl;
        
        // debugLVStructures = if true, dump all monitor data results to the log:
        tmp = configINI.GetValue("debug", "debugLVStructures");
        if (!tmp.empty())
            debugLVStructures = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "Debug LV Structures=" << debugLVStructures << endl;
        
        // thermalLogInterval = seconds; how often to update the thermalLog:
        tmp = configINI.GetValue("logger", "thermalLogInterval");
        if (!tmp.empty())
            thermalLogInterval = from_string<unsigned int>(tmp);
        LOG(LM_INFO) << "Thermal log interval=" << thermalLogInterval << endl;
        if (thermalLogInterval == 0)
            LOG(LM_INFO) << "Thermal logging DISABLED." << endl;

    } catch (...) {
        LOG(LM_ERROR) << "LVWrapperInit exception loading configuration file." << endl;
        pthread_mutex_unlock(&LVWrapperLock);
        return -1;
    }
    
    // Create the CAN interface:
    WHACK(canBus);
    ambItf = AmbInterface::getInstance();
    canBus = new NICANBusInterface(); 
    //canBus = new SocketClientBusInterface("ste-acc.cv.nrao.edu", 2000);
    if (ambItf)
        ambItf -> setBus(canBus);

    // Create the CAN transaction logging queue:
    WHACK(logger);
    logger = new AmbTransactionLogger(logTransactions);
    FEHardwareDevice::setLogger(*logger);

    // Create the FEMC event queue:
    FEMCEventQueue::createInstance();

    // Now the DLL is properly initialized:
    pthread_mutex_lock(&LVWrapperLock);
    LVWrapperValid = true;
    pthread_mutex_unlock(&LVWrapperLock);

    return 0;
}

short LVWrapperShutdown() {
    bool valid = false;
    pthread_mutex_lock(&LVWrapperLock);
    valid = LVWrapperValid;
    pthread_mutex_unlock(&LVWrapperLock);

    if (!valid)
        return -1;

    int connected;
    pthread_mutex_lock(&LVWrapperLock);
    --connectedModules;
    connected = connectedModules;
    pthread_mutex_unlock(&LVWrapperLock);

    LOG(LM_INFO) << "LVWrapperShutdown: connectedModules=" << connected << endl;
    if (connected <= 0) {

        pthread_mutex_lock(&LVWrapperLock);
        LVWrapperValid = false;
        pthread_mutex_unlock(&LVWrapperLock);
        pthread_mutex_destroy(&LVWrapperLock);

        FEHardwareDevice::clearLogger();
        WHACK(logger);
        LOG(LM_INFO) << "LVWrapperShutdown: logger destroyed" << endl;

        FEMCEventQueue::destroyInstance();
        LOG(LM_INFO) << "LVWrapperShutdown: eventQueue destroyed" << endl;

        AmbInterface::deleteInstance();
        ambItf = NULL;
        LOG(LM_INFO) << "LVWrapperShutdown: AmbInterface destroyed" << endl;

        WHACK(canBus);
        LOG(LM_INFO) << "LVWrapperShutdown: CANBusInterface destroyed" << endl;

        if (logStream) {
            fflush(logStream);
            fclose(logStream);
        }
        StreamOutput::setStream(NULL);
        logStream = NULL;
    }
    return 0;
}

DLLEXPORT short getSoftwareVersion(char *versionString) {
    if (!versionString)
        return -1;
    strcpy(versionString, FECONTROL_SW_VERSION_STRING.c_str());
    return 0;
}

DLLEXPORT short getDataPath(char *pathString) {
    if (!pathString)
        return -1;
    string logDir(FEConfig::getLogDir());
    if (logDir.empty()) {
        pathString[0] = '\0';
        return -1;
    }
    // replace all forward slashes with backslashes:
    replace(logDir.begin(), logDir.end(), '/','\\');
    strcpy(pathString, logDir.c_str());
    return 0;
}

DLLEXPORT short subscribeForEvents(short doSubscribe) {
    pthread_mutex_lock(&LVWrapperLock);
    bool valid = LVWrapperValid;
    pthread_mutex_unlock(&LVWrapperLock);

    if (!valid)
        return -1;
    FEMCEventQueue::subscribe(doSubscribe != 0);
    return 0;
}

DLLEXPORT short getNextEvent(unsigned long *seq,
                             short *eventCode,
                             short *band,
                             short *pol,
                             short *param,
                             short *progress,
                             short messageLen,
                             char *message)
{                             
    pthread_mutex_lock(&LVWrapperLock);
    bool valid = LVWrapperValid;
    pthread_mutex_unlock(&LVWrapperLock);

    if (!valid || !seq || !eventCode || !band || !pol || !param || !progress || !message)
        return -1;
    FEMCEventQueue::Event ev;
    bool ret = FEMCEventQueue::getNextEvent(ev);
    *seq = ev.seq_m;
    *eventCode = ev.eventCode_m;
    *band = ev.band_m;
    *pol = ev.pol_m;
    *param = ev.param_m;
    *progress = ev.progress_m;
    int len = ev.message_m.length();
    if (len > messageLen)
        ev.message_m[messageLen - 1] = '\0';
    strcpy(message, ev.message_m.c_str());
    return (ret) ? 0 : -1;
}

DLLEXPORT short getNextEventSeqNo(unsigned long *seq) {
    if (!seq)
        return -1;
    bool ret = FEMCEventQueue::getNextEventSeqNo(*seq);
    return (ret) ? 0 : -1;
}

//----------------------------------------------------------------------------

DLLEXPORT short TestSocketClient() {
    CANBusInterface::enableDebug_m = true;
    SocketClientBusInterface itf("ste-acc.cv.nrao.edu", 2000);
    AmbMessage_t req;
    AmbDataMem_t data[8];
    AmbDataLength_t dataLen;
    Time timestamp;
    AmbErrorCode_t status;
    sem_t synchLock;
    sem_init(&synchLock, 0, 0);

    req.address = createAMBAddr(0x13, 0xD000);
    req.channel = 1;
    req.completion_p = new AmbCompletion_t;
    req.completion_p->dataLength_p = &dataLen;
    req.completion_p->data_p       = &(data[0]);
    req.completion_p->channel_p    = NULL;
    req.completion_p->address_p    = NULL;
    req.completion_p->timestamp_p  = &timestamp;
    req.completion_p->status_p     = &status;
    req.completion_p->synchLock_p  = &synchLock;
    req.completion_p->contLock_p   = NULL;
    req.completion_p->type_p       = NULL;

    memset(req.data, 0, 8);
    req.dataLen = 0;
    req.requestType = AMB_MONITOR;
    req.targetTE = 0;
    itf.sendMessage(req);
    sem_wait(&synchLock);
    
    
    return 0;
}



