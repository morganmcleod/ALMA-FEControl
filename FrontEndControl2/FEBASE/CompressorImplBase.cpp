/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
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
 * base class for the Compressor M&C module
 *----------------------------------------------------------------------
 */

#include "CompressorImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

CompressorImplBase::CompressorImplBase(const std::string &name)
  : AMBSIHardwareDevice(name),
    getTemp1_value(0),
    getTemp2_value(0),
    getTemp3_value(0),
    getTemp4_value(0),
    getReturnLinePressure_value(0),
    getAuxTransducer_value(0),
    getSupplyPressure_value(0),
    getPressureAlarm_value(false),
    getTempAlarm_value(false),
    getDriveIndicator_value(false),
    getICCUStatusError_value(false),
    getICCUCableError_value(false),
    getFETIMStatusError_value(false),
    getFETIMCableError_value(false),
    getInterlockOverride_value(false),
    getECUType_value(0),
    getFaultStatusError_value(false),
    getSWRevisionLevel_value(),
    getTimeSinceLastPowerOn_value(0),
    getTimeSinceLastPowerOff_value(0),
    lastMonitorTime(0),
    monitorPhase(0)
{
}

void CompressorImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void CompressorImplBase::shutdown() {
}

float CompressorImplBase::getTemp1() {
    SYNCMON2_LOG_FLOAT(GET_TEMP_1, "COMPRESSOR:GET_TEMP_1")
}

float CompressorImplBase::getTemp2() {
    SYNCMON2_LOG_FLOAT(GET_TEMP_2, "COMPRESSOR:GET_TEMP_2")
}

float CompressorImplBase::getTemp3() {
    SYNCMON2_LOG_FLOAT(GET_TEMP_3, "COMPRESSOR:GET_TEMP_3")
}

float CompressorImplBase::getTemp4() {
    SYNCMON2_LOG_FLOAT(GET_TEMP_4, "COMPRESSOR:GET_TEMP_4")
}

float CompressorImplBase::getReturnLinePressure() {
    SYNCMON2_LOG_FLOAT(GET_RET_PRESSURE, "COMPRESSOR:GET_RET_PRESSURE")
}

float CompressorImplBase::getAuxTransducer() {
    SYNCMON2_LOG_FLOAT(GET_AUX_2, "COMPRESSOR:GET_AUX_2")
}

float CompressorImplBase::getSupplyPressure() {
    SYNCMON2_LOG_FLOAT(GET_SUPPLY_PRESSURE, "COMPRESSOR:GET_SUPPLY_PRESSURE")
}

bool CompressorImplBase::getPressureAlarm() {
    SYNCMON2_LOG_BOOL(GET_PRESSURE_ALARM, "COMPRESSOR:GET_PRESSURE_ALARM")
}

bool CompressorImplBase::getTempAlarm() {
    SYNCMON2_LOG_BOOL(GET_TEMP_ALARM, "COMPRESSOR:GET_TEMP_ALARM")
}

bool CompressorImplBase::getDriveIndicator() {
    SYNCMON2_LOG_BOOL(GET_DRIVE_INDICATION, "COMPRESSOR:GET_DRIVE_INDICATION")
}

bool CompressorImplBase::getICCUStatusError() {
    SYNCMON2_LOG_BOOL(GET_ICCU_STATUS_ERROR, "COMPRESSOR:GET_ICCU_STATUS_ERROR")
}

bool CompressorImplBase::getICCUCableError() {
    SYNCMON2_LOG_BOOL(GET_ICCU_CABLE_DETECT_ERROR, "COMPRESSOR:GET_ICCU_CABLE_DETECT_ERROR")
}

bool CompressorImplBase::getFETIMStatusError() {
    SYNCMON2_LOG_BOOL(GET_FETIM_STATUS_ERROR, "COMPRESSOR:GET_FETIM_STATUS_ERROR")
}

bool CompressorImplBase::getFETIMCableError() {
    SYNCMON2_LOG_BOOL(GET_FETIM_CABLE_ERROR, "COMPRESSOR:GET_FETIM_CABLE_ERROR")
}

bool CompressorImplBase::getInterlockOverride() {
    SYNCMON2_LOG_BOOL(GET_INTERLOCK_OVERRIDE, "COMPRESSOR:GET_INTERLOCK_OVERRIDE")
}

unsigned char CompressorImplBase::getECUType() {
    SYNCMON2_LOG_INT(GET_ECU_TYPE, unsigned char, "COMPRESSOR:GET_ECU_TYPE")
}

bool CompressorImplBase::getFaultStatusError() {
    SYNCMON2_LOG_BOOL(GET_FAULT_STATUS_ERROR, "COMPRESSOR:GET_FAULT_STATUS_ERROR")
}

std::string CompressorImplBase::getSWRevisionLevel() {
    FEHardwareDevice::syncMonitorThreeByteRevLevel(GET_SW_REVISION_LEVEL, getSWRevisionLevel_value);
    return getSWRevisionLevel_value;
}

unsigned long CompressorImplBase::getTimeSinceLastPowerOn() {
    unsigned long target;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    AmbRelativeAddr RCA(GET_TIME_SINCE_LAST_POWER_ON);
    SYNCMON2(RCA, feStatus, target);
    if (target >= 100000) // Compressor M&C module returns huge bogus value if the timer is not counting up anymore.
        target = 99999;  // sanitize.
    LOG2_INT(FEMC_LOG_MONITOR, RCA, "COMPRESSOR:GET_TIME_SINCE_LAST_POWER_ON", feStatus, target);
    return target;
}

unsigned long CompressorImplBase::getTimeSinceLastPowerOff() {
    unsigned long target;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    AmbRelativeAddr RCA(GET_TIME_SINCE_LAST_POWER_OFF);
    SYNCMON2(RCA, feStatus, target);
    if (target >= 100000) // Compressor M&C module returns huge bogus value if the timer is not counting up anymore.
        target = 99999;  // sanitize.
    LOG2_INT(FEMC_LOG_MONITOR, RCA, "COMPRESSOR:GET_TIME_SINCE_LAST_POWER_OFF", feStatus, target);
    return target;
}

void CompressorImplBase::setRemoteDrive(bool val) {
    unsigned char cVal(val ? 1 : 0);
    SYNCCMD2_LOG_BYTE(SET_REMOTE_DRIVE, cVal, "COMPRESSOR:SET_REMOTE_DRIVE")
}

void CompressorImplBase::setRemoteReset() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SET_REMOTE_RESET, dummy, "COMPRESSOR:SET_REMOTE_RESET")
}

void CompressorImplBase::setFaultLatchReset() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SET_FAULT_LATCH_RESET, dummy, "COMPRESSOR:SET_FAULT_LATCH_RESET")
}

void CompressorImplBase::monitorAction(Time *timestamp_p) {

    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 10000 * milliseconds;
    bool doMonitor = false;

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    static bool monitorOnce(true);
    if (monitorOnce) {
        monitorOnce = false;
        getECUType_value = getECUType();
        getSWRevisionLevel_value = getSWRevisionLevel();
    }

    if (doMonitor) {
        LOG(LM_TRACE) << "CompressorImplBase::monitorAction" << endl;
        getTemp1_value = getTemp1();
        getTemp2_value = getTemp2();
        getTemp3_value = getTemp3();
        getTemp4_value = getTemp4();
        getReturnLinePressure_value = getReturnLinePressure();
        getAuxTransducer_value = getAuxTransducer();
        getSupplyPressure_value = getSupplyPressure();
        getPressureAlarm_value = getPressureAlarm();
        getTempAlarm_value = getTempAlarm();
        getDriveIndicator_value = getDriveIndicator();
        getICCUStatusError_value = getICCUStatusError();
        getICCUCableError_value = getICCUCableError();
        getFETIMStatusError_value = getFETIMStatusError();
        getFETIMCableError_value = getFETIMCableError();
        getInterlockOverride_value = getInterlockOverride();
        getFaultStatusError_value = getFaultStatusError();
        getTimeSinceLastPowerOn_value = getTimeSinceLastPowerOn();
        getTimeSinceLastPowerOff_value = getTimeSinceLastPowerOff();
    }
}


