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
 * base class for the FrontEnd IF switch module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "IFSwitchImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

IFSwitchImplBase::IFSwitchImplBase(const std::string &name)
  : FEHardwareDevice(name),
    switchCartridge_value(0),
    pol0Sb1Attenuation_value(0),
    pol0Sb2Attenuation_value(0),
    pol1Sb1Attenuation_value(0),
    pol1Sb2Attenuation_value(0),
    pol0Sb1AssemblyTemp_value(0),
    pol0Sb2AssemblyTemp_value(0),
    pol1Sb1AssemblyTemp_value(0),
    pol1Sb2AssemblyTemp_value(0),
    pol0Sb1TempServoEnable_value(0),
    pol0Sb2TempServoEnable_value(0),
    pol1Sb1TempServoEnable_value(0),
    pol1Sb2TempServoEnable_value(0),
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = 0xB000;
    AmbRelativeAddr pol1 = 0x0004;
    AmbRelativeAddr sb2  = 0x0008;
    
    switchCartridge_RCA         = baseRCA + SWITCH_CARTRIDGE;
    pol0Sb1Attenuation_RCA      = baseRCA + ATTENUATION;
    pol0Sb2Attenuation_RCA      = baseRCA + ATTENUATION + sb2;
    pol1Sb1Attenuation_RCA      = baseRCA + ATTENUATION + pol1;
    pol1Sb2Attenuation_RCA      = baseRCA + ATTENUATION + pol1 + sb2;
    pol0Sb1AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP;
    pol0Sb2AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP + sb2;
    pol1Sb1AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP + pol1;
    pol1Sb2AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP + pol1 + sb2;
    pol0Sb1TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE;
    pol0Sb2TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE + sb2;
    pol1Sb1TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE + pol1;
    pol1Sb2TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE + pol1 + sb2;
    
    addMon(&pol0Sb1AssemblyTemp_value, &IFSwitchImplBase::pol0Sb1AssemblyTemp);
    addMon(&pol0Sb2AssemblyTemp_value, &IFSwitchImplBase::pol0Sb2AssemblyTemp);
    addMon(&pol1Sb1AssemblyTemp_value, &IFSwitchImplBase::pol1Sb1AssemblyTemp);
    addMon(&pol1Sb2AssemblyTemp_value, &IFSwitchImplBase::pol1Sb2AssemblyTemp);

    nextMon = monitorRegistry.begin();
}
    

void IFSwitchImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void IFSwitchImplBase::shutdown() {
}

unsigned char IFSwitchImplBase::switchCartridge() {
    SYNCMON_LOG_INT(switchCartridge, unsigned char, "IF_SWITCH_CARTRIDGE")
}

unsigned char IFSwitchImplBase::pol0Sb1Attenuation() {
    SYNCMON_LOG_INT(pol0Sb1Attenuation, unsigned char, "IF_ATTENUATION Po=0 Sb=1")
}

unsigned char IFSwitchImplBase::pol0Sb2Attenuation() {
    SYNCMON_LOG_INT(pol0Sb2Attenuation, unsigned char, "IF_ATTENUATION Po=0 Sb=2")
}

unsigned char IFSwitchImplBase::pol1Sb1Attenuation() {
    SYNCMON_LOG_INT(pol1Sb1Attenuation, unsigned char, "IF_ATTENUATION Po=1 Sb=1")
}

unsigned char IFSwitchImplBase::pol1Sb2Attenuation() {
    SYNCMON_LOG_INT(pol1Sb2Attenuation, unsigned char, "IF_ATTENUATION Po=1 Sb=2")
}

float IFSwitchImplBase::pol0Sb1AssemblyTemp() {
    if (!pol0Sb1TempServoEnable_value) {
        MONRETURN(pol0Sb1AssemblyTemp, float, 0);
    } else {            
        SYNCMON_LOG_FLOAT(pol0Sb1AssemblyTemp, "IF_SWITCH_TEMP Po=0 Sb=1")
    }
}

float IFSwitchImplBase::pol0Sb2AssemblyTemp() {
    if (!pol0Sb2TempServoEnable_value) {
        MONRETURN(pol0Sb2AssemblyTemp, float, 0);
    } else {            
        SYNCMON_LOG_FLOAT(pol0Sb2AssemblyTemp, "IF_SWITCH_TEMP Po=0 Sb=2")
    }
}

float IFSwitchImplBase::pol1Sb1AssemblyTemp() {
    if (!pol1Sb1TempServoEnable_value) {
        MONRETURN(pol1Sb1AssemblyTemp, float, 0);
    } else {            
        SYNCMON_LOG_FLOAT(pol1Sb1AssemblyTemp, "IF_SWITCH_TEMP Po=1 Sb=1")
    }
}

float IFSwitchImplBase::pol1Sb2AssemblyTemp() {
    if (!pol1Sb2TempServoEnable_value) {
        MONRETURN(pol1Sb2AssemblyTemp, float, 0);
    } else {            
        SYNCMON_LOG_FLOAT(pol1Sb2AssemblyTemp, "IF_SWITCH_TEMP Po=1 Sb=2")
    }
}

bool IFSwitchImplBase::pol0Sb1TempServoEnable() {
    SYNCMON_LOG_BOOL(pol0Sb1TempServoEnable, "IF_SWITCH_TEMP_SERVO_ENABLE Po=0 Sb=1")
}

bool IFSwitchImplBase::pol0Sb2TempServoEnable() {
    SYNCMON_LOG_BOOL(pol0Sb2TempServoEnable, "IF_SWITCH_TEMP_SERVO_ENABLE Po=0 Sb=2")
}
   
bool IFSwitchImplBase::pol1Sb1TempServoEnable() {
    SYNCMON_LOG_BOOL(pol1Sb1TempServoEnable, "IF_SWITCH_TEMP_SERVO_ENABLE Po=1 Sb=1")
}

bool IFSwitchImplBase::pol1Sb2TempServoEnable() {
    SYNCMON_LOG_BOOL(pol1Sb2TempServoEnable, "IF_SWITCH_TEMP_SERVO_ENABLE Po=1 Sb=2")
}

void IFSwitchImplBase::switchCartridge(unsigned char val) {
    SYNCCMD_LOG_INT(switchCartridge, val, "IF_SWITCH_CARTRIDGE")    
}

void IFSwitchImplBase::pol0Sb1Attenuation(unsigned char val) {
    SYNCCMD_LOG_INT(pol0Sb1Attenuation, val, "IF_ATTENUATION Po=0 Sb=1")    
}

void IFSwitchImplBase::pol0Sb2Attenuation(unsigned char val) {
    SYNCCMD_LOG_INT(pol0Sb2Attenuation, val, "IF_ATTENUATION Po=0 Sb=2")    
}

void IFSwitchImplBase::pol1Sb1Attenuation(unsigned char val) {
    SYNCCMD_LOG_INT(pol1Sb1Attenuation, val, "IF_ATTENUATION Po=1 Sb=1")    
}

void IFSwitchImplBase::pol1Sb2Attenuation(unsigned char val) {
    SYNCCMD_LOG_INT(pol1Sb2Attenuation, val, "IF_ATTENUATION Po=1 Sb=2")    
}

void IFSwitchImplBase::pol0Sb1TempServoEnable(bool val) {
    SYNCCMD_LOG_BOOL(pol0Sb1TempServoEnable, val, "IF_SWITCH_TEMP_SERVO_ENABLE Po=0 Sb=1")    
}

void IFSwitchImplBase::pol0Sb2TempServoEnable(bool val) {
    SYNCCMD_LOG_BOOL(pol0Sb2TempServoEnable, val, "IF_SWITCH_TEMP_SERVO_ENABLE Po=0 Sb=2")    
}

void IFSwitchImplBase::pol1Sb1TempServoEnable(bool val) {
    SYNCCMD_LOG_BOOL(pol1Sb1TempServoEnable, val, "IF_SWITCH_TEMP_SERVO_ENABLE Po=1 Sb=1")    
}

void IFSwitchImplBase::pol1Sb2TempServoEnable(bool val) {
    SYNCCMD_LOG_BOOL(pol1Sb2TempServoEnable, val, "IF_SWITCH_TEMP_SERVO_ENABLE Po=1 Sb=2")    
}

void IFSwitchImplBase::monitorAction(Time *timestamp_p) {
    
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 2000 * milliseconds;   

    bool doMonitor = false;

    if (lastMonitorTime == 0 && logMonitors_m)
        logMon(true);

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        switchCartridge_value = switchCartridge();
        pol0Sb1Attenuation_value = pol0Sb1Attenuation();
        pol0Sb2Attenuation_value = pol0Sb2Attenuation();
        pol1Sb1Attenuation_value = pol1Sb1Attenuation();
        pol1Sb2Attenuation_value = pol1Sb2Attenuation();
        pol0Sb1TempServoEnable_value = pol0Sb1TempServoEnable();
        pol0Sb2TempServoEnable_value = pol0Sb2TempServoEnable();
        pol1Sb1TempServoEnable_value = pol1Sb1TempServoEnable();
        pol1Sb2TempServoEnable_value = pol1Sb2TempServoEnable();        
        while (executeNextMon()) ;
        if (logMonitors_m)
           logMon();
    }        
}

DEFINE_MONITORS_REGISTRY(IFSwitchImplBase)

void IFSwitchImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:IFSwitch: randomized,"
                         "observingBand,pol0Sb1Attenuation,pol0Sb2Attenuation,pol1Sb1Attenuation,pol1Sb2Attenuation,"
                         "pol0Sb1TempServo,pol0Sb2TempServo,pol1Sb1TempServo,pol1Sb2TempServo"
                         "pol0Sb1Temp,pol0Sb2Temp,pol1Sb1Temp,pol1Sb2Temp" << endl;
    } else {
        LOG(LM_INFO)  << "AllMonitors:IFSwitch: "
                      << (switchCartridge_value + 1) << "," << (int) pol0Sb1Attenuation_value << "," << (int) pol0Sb2Attenuation_value << "," << (int) pol1Sb1Attenuation_value << "," << (int) pol1Sb2Attenuation_value << ","
                      << (pol0Sb1TempServoEnable_value ? 1 : 0) << "," << (pol0Sb2TempServoEnable_value ? 1 : 0) << "," << (pol1Sb1TempServoEnable_value ? 1 : 0) << "," << (pol1Sb2TempServoEnable_value ? 1 : 0) << ","
                      << pol0Sb1AssemblyTemp_value << "," << pol0Sb2AssemblyTemp_value << "," << pol1Sb1AssemblyTemp_value << "," << pol1Sb2AssemblyTemp_value << endl;
    }
}

