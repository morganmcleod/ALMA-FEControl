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
*
*/

/************************************************************************
 * base class for the FrontEnd LO Photonic Receiver module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "LPRImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

LPRImplBase::LPRImplBase(const std::string &name)
  : FEHardwareDevice(name),
    LPRTemperature0_value(0),
    LPRTemperature1_value(0),
    opticalSwitchPort_value(0),
    opticalSwitchShutter_value(false),
    opticalSwitchState_value(false),
    opticalSwitchBusy_value(false),
    EDFALaserPumpTemperature_value(0),
    EDFALaserDriveCurrent_value(0),
    EDFALaserPhotoDetectCurrent_value(0),
    EDFAPhotoDetectCurrent_value(0),
    EDFAPhotoDetectPower_value(0),
    EDFAModulationInput_value(0),    
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = 0xD000;
    LPRTemperature0_RCA             = baseRCA + LPR_TEMPERATURE_0;
    LPRTemperature1_RCA             = baseRCA + LPR_TEMPERATURE_1;
    opticalSwitchPort_RCA           = baseRCA + OPTICAL_SWITCH_PORT;
    opticalSwitchShutter_RCA        = baseRCA + OPTICAL_SWITCH_SHUTTER;
    opticalSwitchForceShutter_RCA   = baseRCA + OPTICAL_SWITCH_FORCE_SHUTTER;
    opticalSwitchState_RCA          = baseRCA + OPTICAL_SWITCH_STATE;
    opticalSwitchBusy_RCA           = baseRCA + OPTICAL_SWITCH_BUSY;
    EDFALaserPumpTemperature_RCA    = baseRCA + EDFA_LASER_PUMP_TEMPERATURE;
    EDFALaserDriveCurrent_RCA       = baseRCA + EDFA_LASER_DRIVE_CURRENT;
    EDFALaserPhotoDetectCurrent_RCA = baseRCA + EDFA_LASER_PHOTODETECT_CURRENT;
    EDFAPhotoDetectCurrent_RCA      = baseRCA + EDFA_PHOTODETECT_CURRENT;
    EDFAPhotoDetectPower_RCA        = baseRCA + EDFA_PHOTODETECT_POWER;
    EDFAModulationInput_RCA         = baseRCA + EDFA_MODULATION_INPUT;

    addMon(&LPRTemperature0_value, &LPRImplBase::LPRTemperature0);
    addMon(&LPRTemperature1_value, &LPRImplBase::LPRTemperature1);
    addMon(&EDFALaserPumpTemperature_value, &LPRImplBase::EDFALaserPumpTemperature);
    addMon(&EDFALaserDriveCurrent_value, &LPRImplBase::EDFALaserDriveCurrent);
    addMon(&EDFALaserPhotoDetectCurrent_value, &LPRImplBase::EDFALaserPhotoDetectCurrent);
    addMon(&EDFAPhotoDetectCurrent_value, &LPRImplBase::EDFAPhotoDetectCurrent);
    addMon(&EDFAPhotoDetectPower_value, &LPRImplBase::EDFAPhotoDetectPower);
    addMon(&EDFAModulationInput_value, &LPRImplBase::EDFAModulationInput);
    
    nextMon = monitorRegistry.begin();
}

void LPRImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void LPRImplBase::shutdown() {
}

float LPRImplBase::LPRTemperature0() {
    SYNCMON_LOG_FLOAT(LPRTemperature0, "LPR_TEMPERATURE_0")
}

float LPRImplBase::LPRTemperature1() {
    SYNCMON_LOG_FLOAT(LPRTemperature1, "LPR_TEMPERATURE_1")
}

unsigned char LPRImplBase::opticalSwitchPort() {
    SYNCMON_LOG_INT(opticalSwitchPort, unsigned char, "LPR_OPTICAL_SWITCH_PORT")
}

bool LPRImplBase::opticalSwitchShutter() {
    SYNCMON_LOG_BOOL(opticalSwitchShutter, "LPR_OPTICAL_SWITCH_SHUTTER")
}

bool LPRImplBase::opticalSwitchState() {
    SYNCMON_LOG_BOOL(opticalSwitchState, "LPR_OPTICAL_SWITCH_ERROR_STATE")
}

bool LPRImplBase::opticalSwitchBusy() {
    SYNCMON_LOG_BOOL(opticalSwitchBusy, "LPR_OPTICAL_SWITCH_BUSY")
}

float LPRImplBase::EDFALaserPumpTemperature() {
    SYNCMON_LOG_FLOAT(EDFALaserPumpTemperature, "LPR_EDFA_LASER_PUMP_TEMPERATURE")
}

float LPRImplBase::EDFALaserDriveCurrent() {
    SYNCMON_LOG_FLOAT(EDFALaserDriveCurrent, "LPR_EDFA_LASER_DRIVE_CURRENT")
}

float LPRImplBase::EDFALaserPhotoDetectCurrent() {
    SYNCMON_LOG_FLOAT(EDFALaserPhotoDetectCurrent, "LPR_EDFA_LASER_PHOTODETECT_CURRENT")
}

float LPRImplBase::EDFAPhotoDetectCurrent() {
    SYNCMON_LOG_FLOAT(EDFAPhotoDetectCurrent, "LPR_EDFA_PHOTODETECT_CURRENT")
}

float LPRImplBase::EDFAPhotoDetectPower() {
    SYNCMON_LOG_FLOAT(EDFAPhotoDetectPower, "LPR_EDFA_PHOTODETECT_POWER")
}

float LPRImplBase::EDFAModulationInput() {
    SYNCMON_LOG_FLOAT(EDFAModulationInput, "LPR_EDFA_MODULATION_INPUT")
}

void LPRImplBase::opticalSwitchPort(unsigned char val) {
    SYNCCMD_LOG_INT(opticalSwitchPort, val, "LPR_OPTICAL_SWITCH_PORT")    
}

void LPRImplBase::opticalSwitchShutter(bool val) {
    SYNCCMD_LOG_BOOL(opticalSwitchShutter, val, "LPR_OPTICAL_SWITCH_SHUTTER")    
}

void LPRImplBase::opticalSwitchForceShutter(bool val) {
    SYNCCMD_LOG_BOOL(opticalSwitchForceShutter, val, "LPR_OPTICAL_SWITCH_FORCE_SHUTTER")    
}

void LPRImplBase::EDFAModulationInput(float val) {
    SYNCCMD_LOG_FLOAT(EDFAModulationInput, val, "LPR_EDFA_MODULATION_INPUT")    
}

void LPRImplBase::monitorAction(Time *timestamp_p) {
    
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 150 * milliseconds; 

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
                opticalSwitchPort_value = opticalSwitchPort();
                opticalSwitchShutter_value = opticalSwitchShutter();
                opticalSwitchState_value = opticalSwitchState();
                opticalSwitchBusy_value = opticalSwitchBusy();
                monitorPhase = 2;
                break;
            case 2:
            default:
                if (logMonitors_m)
                    logMon();
                monitorPhase = 0;
                break;
        };
    }
}

DEFINE_MONITORS_REGISTRY(LPRImplBase)

void LPRImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:LPR: randomized,"
                         "observingBand,opticalSwitchShutter,opticalSwitchState,opticalSwitchBusy,"
                         "Temp0,Temp1,EDFALaserPumpTemp,EDFALaserDriveCurrent,"
                         "EDFALaserPhotoDetectCurrent,EDFAPhotoDetectCurrent,EDFAPhotoDetectPower,EDFAModulationInput" << endl;
    } else {
        LOG(LM_INFO)  << "AllMonitors:LPR: "
                      << ((int) opticalSwitchPort_value + 1) << "," << (opticalSwitchShutter_value ? 1 : 0) << "," << (opticalSwitchState_value ? 1 : 0) << "," << (opticalSwitchBusy_value ? 1 : 0) << "," 
                      << LPRTemperature0_value << "," << LPRTemperature1_value << "," << EDFALaserPumpTemperature_value << "," << EDFALaserDriveCurrent_value << "," 
                      << EDFALaserPhotoDetectCurrent_value << "," << EDFAPhotoDetectCurrent_value << "," << EDFAPhotoDetectPower_value << "," << EDFAModulationInput_value << endl;
    }
}


