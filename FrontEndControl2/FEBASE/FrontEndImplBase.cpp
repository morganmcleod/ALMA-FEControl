/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
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

/************************************************************************
 * base class for the FrontEnd monitor and control module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "FrontEndImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
#include <FrontEndAMB/ds1820.h>
using namespace std;

FrontEndImplBase::FrontEndImplBase(const std::string &name)
  : AMBSIHardwareDevice(name),
    specialGetSetupInfo_value(0xFF),
    numEnabledModules_value(0),
    numEnabledModules_status(FEMC_NOT_CONNECTED),
    numErrors_value(0),
    numErrors_status(FEMC_NOT_CONNECTED),
    nextError_value(0),
    nextError_status(FEMC_NOT_CONNECTED),
    FEMode_value(0),
    FEMode_status(FEMC_NOT_CONNECTED),
    numEnabledModules_RCA(POWER_NUM_MODULES_ENABLED),
    powerEnableModule_RCA(POWER_ENABLE_MODULE),
    numErrors_RCA(SPECIAL_MONITOR + GET_ERRORS_NUMBER),
    nextError_RCA(SPECIAL_MONITOR + GET_NEXT_ERROR),
    lastMonitorTime(0),
    monitorPhase(0)
{
}

void FrontEndImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void FrontEndImplBase::shutdown() {
}

std::string FrontEndImplBase::AMBSILibraryVersion() {
    FEHardwareDevice::syncMonitorThreeByteRevLevel(SPECIAL_MONITOR + GET_AMBSI_VERSION_INFO, AMBSILibraryVersion_value);
    return AMBSILibraryVersion_value;
}

std::string FrontEndImplBase::FEMCFirmwareVersion() {
    FEHardwareDevice::syncMonitorThreeByteRevLevel(SPECIAL_MONITOR + GET_VERSION_INFO, FEMCFirmwareVersion_value);
    return FEMCFirmwareVersion_value;
}

std::string FrontEndImplBase::FEMCFPGAVersion() {
    FEHardwareDevice::syncMonitorThreeByteRevLevel(SPECIAL_MONITOR + GET_FPGA_VERSION_INFO, FEMCFPGAVersion_value);
    return FEMCFPGAVersion_value;
}

unsigned char FrontEndImplBase::FEMCGetESNsFound() {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock; 
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(SPECIAL_MONITOR + GET_ESNS_FOUND, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        FEMCGetESNsFound_value = 0;
    else
        FEMCGetESNsFound_value = data[0];
    return FEMCGetESNsFound_value;
}

std::string FrontEndImplBase::FEMCGetNextESN(bool reverseBytes) {
    std::string FEMCGetNextESN_value;
    FEHardwareDevice::syncMonitorEightByteESN(SPECIAL_MONITOR + GET_ESNS, FEMCGetNextESN_value, reverseBytes);
    return FEMCGetNextESN_value;
}

unsigned char FrontEndImplBase::specialGetSetupInfo() {
    AmbDataLength_t dataLength; AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(SPECIAL_MONITOR + GET_SETUP_INFO, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        specialGetSetupInfo_value = 0xFF;
    else
        unpack(specialGetSetupInfo_value, dataLength, data);
    return specialGetSetupInfo_value;
}


unsigned char FrontEndImplBase::numEnabledModules() {
    SYNCMON_LOG_BYTE(numEnabledModules, "POWER_NUM_ENABLED_MODULES")
}

unsigned char FrontEndImplBase::powerEnableModule(int port) {
    unsigned char val(0);
    if (port >= 1 && port <= 10) {
        AmbRelativeAddr RCA(powerEnableModule_RCA);
        RCA |= ((port - 1) << 4);
        sem_t synchLock;
        sem_init(&synchLock, 0, 0);
        FEMC_ERROR status = syncMonitor(RCA, val, synchLock);
        sem_destroy(&synchLock);
        getLogger().log(FEMC_LOG_MONITOR, "POWER_ENABLE_MODULE", RCA, (signed char) status, val, 0.0);
    }
    return val;
}

unsigned short FrontEndImplBase::getNumErrors() {
    SYNCMON_LOG_INT(numErrors, unsigned short, "NUM_ERRORS")
}
unsigned short FrontEndImplBase::getNextError() {
    SYNCMON_LOG_INT(nextError, unsigned short, "NEXT_ERROR")
}

unsigned char FrontEndImplBase::getFEMode() {
    SYNCMON_LOG_INT(FEMode, unsigned char, "FE_MODE")
}

void FrontEndImplBase::powerEnableModule(int port, unsigned char val) {
    if (port >= 1 && port <= 10) {    
        AmbRelativeAddr RCA(powerEnableModule_RCA);
        RCA |= ((port - 1) << 4);
        syncCommand(RCA + 0x10000, val);
        int status(0);
        getLogger().log(FEMC_LOG_COMMAND, "POWER_ENABLE_MODULE", RCA, (signed char) status, val ? 1 : 0, 0.0);
    }
}

void FrontEndImplBase::specialExitProgram(bool val) {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    unsigned char wireVal = val;
    pack(wireVal, dataLength, data);
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    command(SPECIAL_CONTROL + SET_EXIT_PROGRAM, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);    
}

void FrontEndImplBase::specialReadESNs(bool val) {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    unsigned char wireVal = val;
    pack(wireVal, dataLength, data);
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    command(SPECIAL_CONTROL + SET_READ_ESN, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
}

void FrontEndImplBase::setFEMode(unsigned char val) {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    pack(val, dataLength, data);
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    command(SPECIAL_CONTROL + SET_FE_MODE, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
}

void FrontEndImplBase::getMonitorTimers(unsigned short &monTimer1,
                                        unsigned short &monTimer2,
                                        unsigned short &monTimer3,
                                        unsigned short &monTimer4,
                                        unsigned short &monTimer5,
                                        unsigned short &monTimer6,
                                        unsigned short &monTimer7,
                                        unsigned short &maxTimerValue)
{
    // initialize all results to 0:
    monTimer1 = monTimer2 = monTimer3 = monTimer4 = monTimer5 = monTimer6 = monTimer7 = maxTimerValue = 0;

    // these monitor points were added in version 1.1.0:
    if (AMBSIFirmwareRev_value == "1.0.0" || AMBSIFirmwareRev_value == "1.0.1")
        return;

    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);

    // Get the first set of timers:
    monitor(SPECIAL_MONITOR + GET_MON_TIMERS1, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    if (status == AMBERR_NOERR) {
        monTimer1 = ((data[0] & 0xff) << 8) + data[1];
        monTimer2 = ((data[2] & 0xff) << 8) + data[3];
        monTimer3 = ((data[4] & 0xff) << 8) + data[5];
        monTimer4 = ((data[6] & 0xff) << 8) + data[7];
    }

    // Get the second set of timers and the timer start value:
    monitor(SPECIAL_MONITOR + GET_MON_TIMERS2, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    if (status == AMBERR_NOERR) {
       monTimer5 = ((data[0] & 0xff) << 8) + data[1];
       monTimer6 = ((data[2] & 0xff) << 8) + data[3];
       monTimer7 = ((data[4] & 0xff) << 8) + data[5];
       maxTimerValue = ((data[6] & 0xff) << 8) + data[7];
    }

    sem_destroy(&synchLock);
}

void FrontEndImplBase::monitorAction(Time *timestamp_p) {
    
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 5000 * milliseconds;   

    bool doMonitor = false;

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        LOG(LM_TRACE) << "FrontEndImplBase::monitorAction" << endl;
        AMBSINumErrors();
        AMBSINumTransactions();
        AMBSITemperature();
        numEnabledModules_value = numEnabledModules();
    }
}

