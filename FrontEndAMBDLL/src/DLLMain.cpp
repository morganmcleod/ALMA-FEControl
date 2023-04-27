/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2022
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
*
*/

#include "DLLMain.h"

#include "SWVersion.h"
#include "FrontEndAMB/NICANBusInterface.h"
#include "FrontEndAMB/NodeList.h"
#include "FrontEndAMB/ambDefs.h"
#include "inifile.h"
#include "logger.h"
#include "setTimeStamp.h"
#include "splitPath.h"
#include "stringConvert.h"
#include "exchndl.h"
#include <string>
#include <cstring>
#include <string.h>
#include <windows.h>

#include "AMBDevice.h"
using namespace std;

namespace FrontEndAMBDLL {
    // Software objects we create:
    FILE *logStream = NULL;             ///< our logger stream
    const AmbInterface *ambItf;         ///< interface to the low level CAN bus machinery
    CANBusInterface *canBus = NULL;     ///< interface to the higher level command queuing and logging
    AMBDevice *ambDevice;               ///< This object handles packaging and recieving of messages

    // Configuration loading:
    CIniFile *configINI = NULL;         ///< ini file access object
    string iniFileName;                 ///< the top-level FrontEndControlDLL.ini
    string logDir("");                  ///< output logs are created here
    logLevel reportingLevel(LM_DEBUG);   ///< global logging level
    bool CAN_noTransmit = false;        ///< Normally false: ignore CAN connection failure and suppress all CAN messages
    AmbDataMem_t FEMode = 0;            ///< Normally 0=Operational, 1=Troubleshooting, 2=Maintenance, 3=Simulate

    // Library status:
    AmbChannel CANChannel = 0;          ///< Which CAN channel to use
    AmbNodeAddr nodeAddress = 0x13;     ///< The default node address
    bool isValid = false;
};
using namespace FrontEndAMBDLL;


extern "C" BOOL WINAPI DllMain (
    HINSTANCE const instance,  // handle to DLL module
    DWORD     const reason,    // reason for calling function
    LPVOID    const reserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (reason) {
    case DLL_PROCESS_ATTACH: {
            // Get the path to FrontEndControlDLL.ini from the environment or assume it in the working directory:
            char *fn=getenv("FRONTENDAMBDLL.INI");
            iniFileName = (fn) ? fn : "FrontEndAmbDLL.ini";
            try {
                configINI = new CIniFile(iniFileName);
                configINI -> ReadFile();
            } catch (...) {
                delete configINI;
                configINI = NULL;
                return FALSE;
            }
        }
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        shutdown();
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

DLL_API short DLL_CALL getSoftwareVersion(char *versionString) {
    if (!versionString)
        return -1;
    strcpy(versionString, FRONTENDAMB_SW_VERSION_STRING.c_str());
    return 0;
}

DLL_API int DLL_CALL initialize() {
    // check that we loaded our config:
    if (!configINI)
        return -1;

    // initialize exception handling library:
    ExcHndlInit();

    // initialize logDir to the Windows temporary path:
    TCHAR lpTempPathBuffer[MAX_PATH];
    DWORD dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
    if (dwRetVal > 0 && dwRetVal <= MAX_PATH) {
        logDir = lpTempPathBuffer;
    }
    try {
        std::string iniPath, logDir, tmp;

        // get the path where the FrontendControlDLL.ini file is located:
        splitPath(iniFileName, iniPath, tmp);
        if (iniPath.empty())
            iniPath = ".";

        // load a user specified logDir:
        string temp = configINI -> GetValue("logger", "logDir");
        if (!temp.empty())
            logDir = temp;

        // Compose the log file and excHndl file names:
        string logFile("");
        string excHndlFile("");
        if (!logDir.empty()) {
            if (logDir[logDir.length() - 1] != '\\' && logDir[logDir.length() - 1] != '/')
                logDir += "/";

            Time ts;
            setTimeStamp(&ts);
            timestampToText(&ts, tmp, true);

            logFile = logDir + "DLLLOG-" + tmp + ".txt";
            logStream = fopen(logFile.c_str(), "w");
            StreamOutput::setStream(logStream);
            StreamLogger::setReportingLevel(reportingLevel);

            excHndlFile = logDir + "ExcHndl-" + tmp  + ".txt";
            ExcHndlSetLogFileNameA(excHndlFile.c_str());
        }

        // Report configuration so far:

        LOG(LM_INFO) << "FrontEndAMB.DLL version " << FRONTENDAMB_SW_VERSION_STRING << endl;

        LOG(LM_INFO) << "Using configuration file '" << iniFileName << "'" << endl;
        if (!logDir.empty())
            LOG(LM_INFO) << "Using log directory '" << logDir << "'" << endl;
        if (!logFile.empty())
            LOG(LM_INFO) << "Using log file '" << logFile << "'" << endl;
        if (!excHndlFile.empty())
            LOG(LM_INFO) << "Using ExcHndl file '" << excHndlFile << "'" << endl;

        // CAN_debugStdout = if true, every low-level CAN frame will be logged to stdout:
        tmp = configINI -> GetValue("debug", "CAN_debugStdout");
        if (!tmp.empty())
            CANBusInterface::enableDebug_m = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "CAN debug to stdout=" << CANBusInterface::enableDebug_m << endl;

        // CAN_noTransmit = if true, don't actually send CAN messages.  Useful for testing when no FE available:
        tmp = configINI -> GetValue("debug", "CAN_noTransmit");
        if (!tmp.empty()) {
            CAN_noTransmit = from_string<unsigned long>(tmp);
            CANBusInterface::noTransmit_m = CAN_noTransmit;
        }
        LOG(LM_INFO) << "CAN debug no transmit=" << CANBusInterface::noTransmit_m << endl;

        tmp = configINI -> GetValue("debug", "FEMode");
        if (!tmp.empty())
            FEMode = from_string<AmbDataMem_t>(tmp);
        LOG(LM_INFO) << "debug:FEMode=" << FEMode << endl;
        
        // CAN_monitorTimeout = milliseconds timeout for monitor messages:
        //  2ms typical;  Increase when debugging FE firmware.
        tmp = configINI -> GetValue("debug", "CAN_monitorTimeout");
        if (!tmp.empty())
            CANBusInterface::monitorTimeout_m = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "CAN monitor timeout=" << CANBusInterface::monitorTimeout_m << endl;

        // Configured CAN channel
        tmp = configINI -> GetValue("connection", "channel");
        if (!tmp.empty())
            CANChannel = from_string<unsigned long>(tmp);

        // Configured default CAN node address
        tmp  = configINI -> GetValue("connection", "nodeAddress");
        if (!tmp.empty()) {
            if (tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X'))
                tmp = tmp.substr(2);
            nodeAddress = from_string<unsigned long>(tmp, std::hex);
        }
        LOG(LM_INFO) << "Using CAN" << CANChannel
            << " at nodeAddress=" << uppercase << hex << setw(2) << setfill('0') << nodeAddress << dec << setw(0) << endl;

    } catch (...) {
        LOG(LM_ERROR) << "FrontEndAMB.DLL exception loading configuration file." << endl;
        return -1;
    }

    ambItf = AmbInterface::getInstance();
    canBus = new NICANBusInterface();
    if (ambItf && canBus)
        ambItf -> setBus(canBus);
    else {
        LOG(LM_ERROR) << "FrontEndAMB.DLL AmbInterface::getInstance() or memory allocation failed." << endl;
        return -1;
    }
    ambDevice = new AMBDevice(CANChannel, nodeAddress, ambItf);
    if (!ambDevice) {
        LOG(LM_ERROR) << "FrontEndAMB.DLL memory allocation failed." << endl;
        return -1;
    }

    isValid = true;
    // Set FEMode:
    AmbDataMem_t data[8] = {FEMode, 0, 0, 0, 0, 0, 0, 0};
    command(nodeAddress, 0x2000E, 1, data);
    return 0;
};

DLL_API int DLL_CALL shutdown() {
    if (!isValid)
        return 0;
    isValid = false;

    canBus -> shutdown();
    delete canBus;
    canBus = NULL;
    LOG(LM_INFO) << "FrontEndAMB.DLL: CANBusInterface destroyed" << endl;

    SLEEP(100);

    ambItf -> setBus(NULL);
    AmbInterface::deleteInstance();
    ambItf = NULL;
    LOG(LM_INFO) << "FrontEndAMB.DLL: AmbInterface destroyed" << endl;

    if (logStream) {
        fflush(logStream);
        fclose(logStream);
        logStream = NULL;
    }
    StreamOutput::setStream(NULL);

    delete configINI;
    configINI = NULL;

    return 0;
}

DLL_API int DLL_CALL setTimeout(unsigned long timeout) {
    CANBusInterface::monitorTimeout_m = timeout;
    LOG(LM_INFO) << "CAN monitor timeout=" << CANBusInterface::monitorTimeout_m << endl;
    return 0;
}

DLL_API int DLL_CALL findNodes(unsigned short *numFound, unsigned char *nodeAddrs, unsigned char **serialNums, unsigned short maxLen) {
    if (!isValid || !numFound || !nodeAddrs || !serialNums || !maxLen) {
        LOG(LM_ERROR) << "FrontEndAMB.DLL findNodes: bad state or params." << endl;
        return -1;
    }
    const nodeList_t *nodes = canBus -> findNodes(CANChannel);
    if (!nodes)
        return -1;
    *numFound = 0;
    nodeList_t::const_iterator it = nodes -> begin();
    if (it != nodes -> end())
        LOG(LM_INFO) << "FrontEndAMB.DLL found nodes:" << endl;
    while (it != nodes -> end() and (*numFound) < maxLen) {
        LOG(LM_DEBUG) << uppercase << hex << setfill('0') << setw(2) << it -> node
                    << ":" << (unsigned)(it -> serialNumber[0])
                    << " " << (unsigned)(it -> serialNumber[1])
                    << " " << (unsigned)(it -> serialNumber[2])
                    << " " << (unsigned)(it -> serialNumber[3])
                    << " " << (unsigned)(it -> serialNumber[4])
                    << " " << (unsigned)(it -> serialNumber[5])
                    << " " << (unsigned)(it -> serialNumber[6])
                    << " " << (unsigned)(it -> serialNumber[7]) << dec << setw(0) << endl;
        nodeAddrs[*numFound] = it -> node;
        memcpy(serialNums[*numFound], it -> serialNumber, 8);
        (*numFound)++;
        it++;
    }
    return 0;
}

DLL_API int DLL_CALL command(unsigned char nodeAddr, unsigned long RCA, unsigned short dataLength, const unsigned char *data) {
    if (!isValid || !nodeAddr || !RCA) {
        LOG(LM_ERROR) << "FrontEndAMB.DLL command: bad state or params." << endl;
        return -1;
    }
    LOG(LM_DEBUG) << "FrontEndAMB.DLL command(" << uppercase << hex << setfill('0') << setw(2) << (unsigned) nodeAddr << "):"
                  << RCA << endl;
    return ambDevice -> command(nodeAddr, RCA, dataLength, data);
}

DLL_API int DLL_CALL monitor(unsigned char nodeAddr, unsigned long RCA, unsigned short *dataLength, unsigned char *data) {
    if (!isValid || !dataLength || !data) {
        LOG(LM_ERROR) << "FrontEndAMB.DLL monitor: bad state or params." << endl;
        return -1;
    }
    LOG(LM_DEBUG) << "FrontEndAMB.DLL monitor(" << uppercase << hex << setfill('0') << setw(2) << (unsigned) nodeAddr << "):"
                  << RCA << endl;
    return ambDevice -> monitor(nodeAddr, RCA, *dataLength, data);
}

DLL_API int DLL_CALL runSequence(unsigned char nodeAddr, Message *sequence, unsigned long maxLen) {
    for (unsigned long i = 0; i < maxLen; i++) {
        bool command = false;
        if (sequence[i].dataLength == 0) {
            ambDevice -> monitor(nodeAddr, sequence[i].RCA, sequence[i].dataLength, sequence[i].data);
        } else {
            command = true;
            ambDevice -> command(nodeAddr, sequence[i].RCA, sequence[i].dataLength, sequence[i].data);
        }
        LOG(LM_DEBUG) << (command ? "command 0x" : "monitor 0x")
                        << uppercase << hex << setfill('0') << setw(5)
                        << (unsigned) sequence[i].RCA << " "
                        << dec << sequence[i].dataLength << ": "
                        << uppercase << hex << setfill('0')
                        << setw(2) << unsigned(sequence[i].data[0]) << " "
                        << setw(2) << unsigned(sequence[i].data[1]) << " "
                        << setw(2) << unsigned(sequence[i].data[2]) << " "
                        << setw(2) << unsigned(sequence[i].data[3]) << " "
                        << setw(2) << unsigned(sequence[i].data[4]) << " "
                        << setw(2) << unsigned(sequence[i].data[5]) << " "
                        << setw(2) << unsigned(sequence[i].data[6]) << " "
                        << setw(2) << unsigned(sequence[i].data[7]) << endl;

        setTimeStamp(&(sequence[i].timestamp));
    }
    return 0;
}
