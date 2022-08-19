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
 * base class for the FrontEnd LO Photonic Receiver module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */

#include "FETIMImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

FETIMImplBase::FETIMImplBase(const std::string &name)
  : FEHardwareDevice(name),
    internalTemperature1_value(0),
    internalTemperature2_value(0),
    internalTemperature3_value(0),
    internalTemperature4_value(0),
    internalTemperature5_value(0),
    internalTemperatureOOR_value(true),
    externalTemperature1_value(0),
    externalTemperature2_value(0),
    externalTemperatureOOR1_value(true),
    externalTemperatureOOR2_value(true),
    getAirflowSensor1_value(0),
    getAirflowSensor2_value(0),
    airflowSensorOOR_value(true),
    heliumBufferPressure_value(0),
    heliumBufferPressureOOR_value(true),
    sensorSingleFailed_value(true),
    sensorMultiFailed_value(true),
    glitchValue_value(0),
    glitchCounterTriggered_value(true),
    delayShutdownTriggered_value(false),
    finalShutdownTriggered_value(false),
    getCompressorStatus_value(false),
    getCompressorInterlockStatus_value(true),
    getCompressorCableStatus_value(true),
    internalTemperature1_status(FEMC_NOT_CONNECTED),
    internalTemperature2_status(FEMC_NOT_CONNECTED),
    internalTemperature3_status(FEMC_NOT_CONNECTED),
    internalTemperature4_status(FEMC_NOT_CONNECTED),
    internalTemperature5_status(FEMC_NOT_CONNECTED),
    internalTemperatureOOR_status(FEMC_NOT_CONNECTED),
    externalTemperature1_status(FEMC_NOT_CONNECTED),
    externalTemperature2_status(FEMC_NOT_CONNECTED),
    externalTemperatureOOR1_status(FEMC_NOT_CONNECTED),
    externalTemperatureOOR2_status(FEMC_NOT_CONNECTED),
    getAirflowSensor1_status(FEMC_NOT_CONNECTED),
    getAirflowSensor2_status(FEMC_NOT_CONNECTED),
    airflowSensorOOR_status(FEMC_NOT_CONNECTED),
    heliumBufferPressure_status(FEMC_NOT_CONNECTED),
    heliumBufferPressureOOR_status(FEMC_NOT_CONNECTED),
    sensorSingleFailed_status(FEMC_NOT_CONNECTED),
    sensorMultiFailed_status(FEMC_NOT_CONNECTED),
    glitchValue_status(FEMC_NOT_CONNECTED),
    glitchCounterTriggered_status(FEMC_NOT_CONNECTED),
    delayShutdownTriggered_status(FEMC_NOT_CONNECTED),
    finalShutdownTriggered_status(FEMC_NOT_CONNECTED),
    getCompressorStatus_status(FEMC_NOT_CONNECTED),
    getCompressorInterlockStatus_status(FEMC_NOT_CONNECTED),
    getCompressorCableStatus_status(FEMC_NOT_CONNECTED),
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = 0xE000;
    internalTemperature1_RCA            = baseRCA + FETIM_INTRLK_SENS_INT_TEMP + 0x0;
    internalTemperature2_RCA            = baseRCA + FETIM_INTRLK_SENS_INT_TEMP + 0x1;
    internalTemperature3_RCA            = baseRCA + FETIM_INTRLK_SENS_INT_TEMP + 0x2;
    internalTemperature4_RCA            = baseRCA + FETIM_INTRLK_SENS_INT_TEMP + 0x3;
    internalTemperature5_RCA            = baseRCA + FETIM_INTRLK_SENS_INT_TEMP + 0x4;
    internalTemperatureOOR_RCA          = baseRCA + FETIM_INTRLK_SENS_INT_TEMP_OOR;
    externalTemperature1_RCA            = baseRCA + FETIM_COMP_EXT_TEMP + 0x0;
    externalTemperature2_RCA            = baseRCA + FETIM_COMP_EXT_TEMP + 0x8;
    externalTemperatureOOR1_RCA         = baseRCA + FETIM_COMP_EXT_TEMP_OOR + 0x0;
    externalTemperatureOOR2_RCA         = baseRCA + FETIM_COMP_EXT_TEMP_OOR + 0x8;
    getAirflowSensor1_RCA               = baseRCA + FETIM_INTRLK_SENS_FLOW_SENS + 0x0;
    getAirflowSensor2_RCA               = baseRCA + FETIM_INTRLK_SENS_FLOW_SENS + 0x4;
    airflowSensorOOR_RCA                = baseRCA + FETIM_INTRLK_SENS_FLOW_OOR;
    heliumBufferPressure_RCA            = baseRCA + FETIM_COMP_HE2_PRESS;
    heliumBufferPressureOOR_RCA         = baseRCA + FETIM_COMP_HE2_PRESS_OOR;
    sensorSingleFailed_RCA              = baseRCA + FETIM_INTRLK_SENS_SINGLE_FAIL;
    sensorMultiFailed_RCA               = baseRCA + FETIM_INTRLK_SENS_MULT_FAIL;
    glitchValue_RCA                     = baseRCA + FETIM_INTRLK_GLITCH_VALUE;
    glitchCounterTriggered_RCA          = baseRCA + FETIM_INTRLK_GLITCH_CNT_TRIG;
    delayShutdownTriggered_RCA          = baseRCA + FETIM_INTRLK_DELAY_TRIG;
    finalShutdownTriggered_RCA          = baseRCA + FETIM_INTRLK_SHUTDOWN_TRIG;
    getCompressorStatus_RCA             = baseRCA + FETIM_COMP_FE_STATUS;
    getCompressorInterlockStatus_RCA    = baseRCA + FETIM_COMP_INTRLK_STATUS;
    getCompressorCableStatus_RCA        = baseRCA + FETIM_COMP_CABLE_STATUS;
    setTriggerDewarN2Fill_RCA           = baseRCA + FETIM_COMP_DEWAR_N2_FILL;

    addMon(&internalTemperature1_value, &FETIMImplBase::internalTemperature1);
    addMon(&internalTemperature2_value, &FETIMImplBase::internalTemperature2);
    addMon(&internalTemperature3_value, &FETIMImplBase::internalTemperature3);
    addMon(&internalTemperature4_value, &FETIMImplBase::internalTemperature4);
    addMon(&internalTemperature5_value, &FETIMImplBase::internalTemperature5);
    addMon(&externalTemperature1_value, &FETIMImplBase::externalTemperature1);
    addMon(&externalTemperature2_value, &FETIMImplBase::externalTemperature2);
    addMon(&getAirflowSensor1_value, &FETIMImplBase::getAirflowSensor1);
    addMon(&getAirflowSensor2_value, &FETIMImplBase::getAirflowSensor2);
    addMon(&heliumBufferPressure_value, &FETIMImplBase::heliumBufferPressure);
    addMon(&glitchValue_value, &FETIMImplBase::glitchValue);

    nextMon = monitorRegistry.begin();
}

void FETIMImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void FETIMImplBase::shutdown() {
}

float FETIMImplBase::internalTemperature1() {
    SYNCMON_LOG_FLOAT(internalTemperature1, "FETIM_INTRLK_SENS_INT_TEMP1")
}

float FETIMImplBase::internalTemperature2() {
    SYNCMON_LOG_FLOAT(internalTemperature2, "FETIM_INTRLK_SENS_INT_TEMP2")
}

float FETIMImplBase::internalTemperature3() {
    SYNCMON_LOG_FLOAT(internalTemperature3, "FETIM_INTRLK_SENS_INT_TEMP3")
}

float FETIMImplBase::internalTemperature4() {
    SYNCMON_LOG_FLOAT(internalTemperature4, "FETIM_INTRLK_SENS_INT_TEMP4")
}

float FETIMImplBase::internalTemperature5() {
    SYNCMON_LOG_FLOAT(internalTemperature5, "FETIM_INTRLK_SENS_INT_TEMP5")
}

bool FETIMImplBase::internalTemperatureOOR() {
    SYNCMON_LOG_BOOL(internalTemperatureOOR, "FETIM_INTRLK_SENS_INT_TEMP_OOR")
}

float FETIMImplBase::externalTemperature1() {
    SYNCMON_LOG_FLOAT(externalTemperature1, "FETIM_COMP_EXT_TEMP1")
}

float FETIMImplBase::externalTemperature2() {
    SYNCMON_LOG_FLOAT(externalTemperature2, "FETIM_COMP_EXT_TEMP2")
}

bool FETIMImplBase::externalTemperatureOOR1() {
    SYNCMON_LOG_BOOL(externalTemperatureOOR1, "FETIM_COMP_EXT_TEMP_OOR1")
}

bool FETIMImplBase::externalTemperatureOOR2() {
    SYNCMON_LOG_BOOL(externalTemperatureOOR2, "FETIM_COMP_EXT_TEMP_OOR2")
}

float FETIMImplBase::getAirflowSensor1() {
    SYNCMON_LOG_FLOAT(getAirflowSensor1, "FETIM_INTRLK_SENS_FLOW_SENS1")
}

float FETIMImplBase::getAirflowSensor2() {
    SYNCMON_LOG_FLOAT(getAirflowSensor2, "FETIM_INTRLK_SENS_FLOW_SENS2")
}

bool FETIMImplBase::airflowSensorOOR() {
    SYNCMON_LOG_BOOL(airflowSensorOOR, "FETIM_INTRLK_SENS_FLOW_OOR")
}

float FETIMImplBase::heliumBufferPressure() {
    SYNCMON_LOG_FLOAT(heliumBufferPressure, "FETIM_COMP_HE2_PRESS")
}

bool FETIMImplBase::heliumBufferPressureOOR() {
    SYNCMON_LOG_BOOL(heliumBufferPressureOOR, "FETIM_COMP_HE2_PRESS_OOR")
}

bool FETIMImplBase::sensorSingleFailed() {
    SYNCMON_LOG_BOOL(sensorSingleFailed, "FETIM_INTRLK_SENS_SINGLE_FAIL")
}

bool FETIMImplBase::sensorMultiFailed() {
    SYNCMON_LOG_BOOL(sensorMultiFailed, "FETIM_INTRLK_SENS_MULT_FAIL")
}

float FETIMImplBase::glitchValue() {
    SYNCMON_LOG_FLOAT(glitchValue, "FETIM_INTRLK_GLITCH_VALUE")
}

bool FETIMImplBase::glitchCounterTriggered() {
    SYNCMON_LOG_BOOL(glitchCounterTriggered, "FETIM_INTRLK_GLITCH_CNT_TRIG")
}

bool FETIMImplBase::delayShutdownTriggered() {
    SYNCMON_LOG_BOOL(delayShutdownTriggered, "FETIM_INTRLK_DELAY_TRIG")
}

bool FETIMImplBase::finalShutdownTriggered() {
    SYNCMON_LOG_BOOL(finalShutdownTriggered, "FETIM_INTRLK_SHUTDOWN_TRIG")
}

bool FETIMImplBase::getCompressorStatus() {
    SYNCMON_LOG_BOOL(getCompressorStatus, "FETIM_COMP_FE_STATUS")
}

bool FETIMImplBase::getCompressorInterlockStatus() {
    SYNCMON_LOG_BOOL(getCompressorInterlockStatus, "FETIM_COMP_INTRLK_STATUS")
}

bool FETIMImplBase::getCompressorCableStatus() {
    SYNCMON_LOG_BOOL(getCompressorCableStatus, "FETIM_COMP_CABLE_STATUS")
}

void FETIMImplBase::setTriggerDewarN2Fill(bool val) {
    SYNCCMD_LOG_BOOL(setTriggerDewarN2Fill, val, "FETIM_COMP_DEWAR_N2_FILL")
}

void FETIMImplBase::monitorAction(Time *timestamp_p) {

    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 20 * milliseconds;

    bool doMonitor = false;

    if (lastMonitorTime == 0 && logMonitors_m)
        logMon(true);

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        switch (monitorPhase) {
            case 0:
                if (!executeNextMon())
                    monitorPhase = 1;
                break;
            case 1:
                internalTemperatureOOR_value = internalTemperatureOOR();
                ++monitorPhase;
                break;
            case 2:
                externalTemperatureOOR1_value = externalTemperatureOOR1();
                ++monitorPhase;
                break;
            case 3:
                externalTemperatureOOR2_value = externalTemperatureOOR2();
                ++monitorPhase;
                break;
            case 4:
                airflowSensorOOR_value = airflowSensorOOR();
                ++monitorPhase;
                break;
            case 5:
                heliumBufferPressureOOR_value = heliumBufferPressureOOR();
                ++monitorPhase;
                break;
            case 6:
                sensorSingleFailed_value = sensorSingleFailed();
                ++monitorPhase;
                break;
            case 7:
                sensorMultiFailed_value = sensorMultiFailed();
                ++monitorPhase;
                break;
            case 8:
                glitchCounterTriggered_value = glitchCounterTriggered();
                ++monitorPhase;
                break;
            case 9:
                delayShutdownTriggered_value = delayShutdownTriggered();
                ++monitorPhase;
                break;
            case 10:
                finalShutdownTriggered_value = finalShutdownTriggered();
                ++monitorPhase;
                break;
            case 11:
                getCompressorStatus_value = getCompressorStatus();
                ++monitorPhase;
                break;
            case 12:
                getCompressorInterlockStatus_value = getCompressorInterlockStatus();
                ++monitorPhase;
                break;
            case 13:
                getCompressorCableStatus_value = getCompressorCableStatus();
                ++monitorPhase;
                // no break;
            default:
                if (logMonitors_m)
                    logMon();
                monitorPhase = 0;
                break;
        };
    }
}

DEFINE_MONITORS_REGISTRY(FETIMImplBase)

void FETIMImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:FETIM: randomized,"
                         "internalTemperature1,internalTemperature2,internalTemperature3,internalTemperature4,internalTemperature5,internalTemperatureOOR,"
                         "externalTemperature1,externalTemperature2,externalTemperatureOOR1,externalTemperatureOOR2,"
                         "getAirflowSensor1,getAirflowSensor2,airflowSensorOOR,heliumBufferPressure,heliumBufferPressureOOR,"
                         "sensorSingleFailed,sensorMultiFailed,glitchValue,glitchCounterTriggered,"
                         "delayShutdownTriggered,finalShutdownTriggered,getCompressorStatus,getCompressorInterlockStatus,getCompressorCableStatus" << endl;
    } else {
        LOG(LM_INFO)  << "AllMonitors:LPR: "
                      << internalTemperature1_value << "," << internalTemperature2_value << "," << internalTemperature3_value << "," << internalTemperature4_value << ","
                      << internalTemperature5_value << "," << internalTemperatureOOR_value << "," << externalTemperature1_value << "," << externalTemperature2_value << ","
                      << externalTemperatureOOR1_value << "," << externalTemperatureOOR2_value << "," << getAirflowSensor1_value << "," << getAirflowSensor2_value << ","
                      << airflowSensorOOR_value << "," << heliumBufferPressure_value << "," << heliumBufferPressureOOR_value << "," << sensorSingleFailed_value << ","
                      << sensorMultiFailed_value << "," << glitchValue_value << "," << glitchCounterTriggered_value << "," << delayShutdownTriggered_value << ","
                      << finalShutdownTriggered_value << "," << getCompressorStatus_value << "," << getCompressorInterlockStatus_value << "," << getCompressorCableStatus_value << endl;
    }
}


