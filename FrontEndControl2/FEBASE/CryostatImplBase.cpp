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
 * base class for the FrontEnd Cryostat M&C module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "CryostatImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

CryostatImplBase::CryostatImplBase(const std::string &name)
  : FEHardwareDevice(name),
    cryostatTemperature0_value(0),
    cryostatTemperature1_value(0),
    cryostatTemperature2_value(0),
    cryostatTemperature3_value(0),
    cryostatTemperature4_value(0),
    cryostatTemperature5_value(0),
    cryostatTemperature6_value(0),
    cryostatTemperature7_value(0),
    cryostatTemperature8_value(0),
    cryostatTemperature9_value(0),
    cryostatTemperature10_value(0),
    cryostatTemperature11_value(0),
    cryostatTemperature12_value(0),    
    backingPumpEnable_value(false),
    turboPumpEnable_value(false),
    turboPumpErrorState_value(0),
    turboPumpHighSpeed_value(0),
    gateValveState_value(2),        //unknown state
    solenoidValveState_value(2),    //unknown state
    vacuumCryostatPressure_value(0),
    vacuumPortPressure_value(0),
    vacuumGaugeEnable_value(false),
    vacuumGaugeErrorState_value(0),
    supplyCurrent230V_value(0),
    cryostatTemperature0_status(FEMC_NOT_CONNECTED),
    cryostatTemperature1_status(FEMC_NOT_CONNECTED),
    cryostatTemperature2_status(FEMC_NOT_CONNECTED),
    cryostatTemperature3_status(FEMC_NOT_CONNECTED),
    cryostatTemperature4_status(FEMC_NOT_CONNECTED),
    cryostatTemperature5_status(FEMC_NOT_CONNECTED),
    cryostatTemperature6_status(FEMC_NOT_CONNECTED),
    cryostatTemperature7_status(FEMC_NOT_CONNECTED),
    cryostatTemperature8_status(FEMC_NOT_CONNECTED),
    cryostatTemperature9_status(FEMC_NOT_CONNECTED),
    cryostatTemperature10_status(FEMC_NOT_CONNECTED),
    cryostatTemperature11_status(FEMC_NOT_CONNECTED),
    cryostatTemperature12_status(FEMC_NOT_CONNECTED),    
    backingPumpEnable_status(FEMC_NOT_CONNECTED),
    supplyCurrent230V_status(FEMC_NOT_CONNECTED),
    turboPumpEnable_status(FEMC_NOT_CONNECTED),
    turboPumpErrorState_status(FEMC_NOT_CONNECTED),
    turboPumpHighSpeed_status(FEMC_NOT_CONNECTED),
    gateValveState_status(FEMC_NOT_CONNECTED),
    solenoidValveState_status(FEMC_NOT_CONNECTED),
    vacuumCryostatPressure_status(FEMC_NOT_CONNECTED),
    vacuumPortPressure_status(FEMC_NOT_CONNECTED),
    vacuumGaugeEnable_status(FEMC_NOT_CONNECTED),
    vacuumGaugeErrorState_status(FEMC_NOT_CONNECTED),
    setTVOCoefficient_status(FEMC_NOT_CONNECTED),
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = 0xC000;    
    cryostatTemperature0_RCA    = baseRCA + CRYOSTAT_TEMP;
    cryostatTemperature1_RCA    = baseRCA + CRYOSTAT_TEMP + 0x04;
    cryostatTemperature2_RCA    = baseRCA + CRYOSTAT_TEMP + 0x08;
    cryostatTemperature3_RCA    = baseRCA + CRYOSTAT_TEMP + 0x0C;
    cryostatTemperature4_RCA    = baseRCA + CRYOSTAT_TEMP + 0x10;
    cryostatTemperature5_RCA    = baseRCA + CRYOSTAT_TEMP + 0x14;
    cryostatTemperature6_RCA    = baseRCA + CRYOSTAT_TEMP + 0x18;
    cryostatTemperature7_RCA    = baseRCA + CRYOSTAT_TEMP + 0x1C;
    cryostatTemperature8_RCA    = baseRCA + CRYOSTAT_TEMP + 0x20;
    cryostatTemperature9_RCA    = baseRCA + CRYOSTAT_TEMP + 0x24;
    cryostatTemperature10_RCA   = baseRCA + CRYOSTAT_TEMP + 0x28;
    cryostatTemperature11_RCA   = baseRCA + CRYOSTAT_TEMP + 0x2C;
    cryostatTemperature12_RCA   = baseRCA + CRYOSTAT_TEMP + 0x30;

    backingPumpEnable_RCA       = baseRCA + BACKING_PUMP_ENABLE;
    turboPumpEnable_RCA         = baseRCA + TURBO_PUMP_ENABLE;
    turboPumpErrorState_RCA     = baseRCA + TURBO_PUMP_STATE;
    turboPumpHighSpeed_RCA      = baseRCA + TURBO_PUMP_SPEED;
    gateValveState_RCA          = baseRCA + GATE_VALVE_STATE;
    solenoidValveState_RCA      = baseRCA + SOLENOID_VALVE_STATE;
    vacuumCryostatPressure_RCA  = baseRCA + VACUUM_CRYOSTAT_PRES;
    vacuumPortPressure_RCA      = baseRCA + VACUUM_PORT_PRES;
    vacuumGaugeEnable_RCA       = baseRCA + VACUUM_GAUGE_ENABLE;
    vacuumGaugeErrorState_RCA   = baseRCA + VACUUM_GAUGE_STATE;
    setTVOCoefficient_RCA       = baseRCA + TVO_COEFF_SET + 0x10000;
    supplyCurrent230V_RCA       = baseRCA + SUPPLY_CURRENT_230V;

    // Add all the analog monitor points to the registry:
    
    addMon(&cryostatTemperature0_value, &CryostatImplBase::cryostatTemperature0);
    addMon(&cryostatTemperature1_value, &CryostatImplBase::cryostatTemperature1);
    addMon(&cryostatTemperature2_value, &CryostatImplBase::cryostatTemperature2);
    addMon(&cryostatTemperature3_value, &CryostatImplBase::cryostatTemperature3);
    addMon(&cryostatTemperature4_value, &CryostatImplBase::cryostatTemperature4);
    addMon(&cryostatTemperature5_value, &CryostatImplBase::cryostatTemperature5);
    addMon(&cryostatTemperature6_value, &CryostatImplBase::cryostatTemperature6);
    addMon(&cryostatTemperature7_value, &CryostatImplBase::cryostatTemperature7);
    addMon(&cryostatTemperature8_value, &CryostatImplBase::cryostatTemperature8);
    addMon(&cryostatTemperature9_value, &CryostatImplBase::cryostatTemperature9);
    addMon(&cryostatTemperature10_value, &CryostatImplBase::cryostatTemperature10);
    addMon(&cryostatTemperature11_value, &CryostatImplBase::cryostatTemperature11);
    addMon(&cryostatTemperature12_value, &CryostatImplBase::cryostatTemperature12);
    addMon(&vacuumCryostatPressure_value, &CryostatImplBase::vacuumCryostatPressure);
    addMon(&vacuumPortPressure_value, &CryostatImplBase::vacuumPortPressure);
    addMon(&supplyCurrent230V_value, &CryostatImplBase::supplyCurrent230V);

    nextMon = monitorRegistry.begin();
}

void CryostatImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void CryostatImplBase::shutdown() {
}

FEMC_ERROR CryostatImplBase::CryostatMonitorTempsensor(AmbRelativeAddr RCA, float &target) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    sem_t synchLock;
    sem_init(&synchLock, 0, 0);
    ret = syncMonitor(RCA, target, synchLock);
    // if AMB error, quit without retrying:
    if (ret == FEMC_AMB_ERROR)
        return ret;
    // no AMB error.  Check for retry required:
    else if (ret == FEMC_HARDW_RETRY_WARN)
        ret = FEMC_NO_ERROR; // Proceed to 2nd measurement.
    SLEEP(25);
    syncMonitor(RCA, target, synchLock);
    sem_destroy(&synchLock);
    return ret;
}

#define DELAY_TEMPSENSOR_READING 1

#ifdef DELAY_TEMPSENSOR_READING

#define SYNCMON_LOG_TEMPSENSOR(NAME, TEXT) { \
    float target; \
    NAME##_status = CryostatMonitorTempsensor(NAME##_RCA, target); \
    LOG_FLOAT(FEMC_LOG_MONITOR, NAME, TEXT, target); \
    return target; }

#else

#define SYNCMON_LOG_TEMPSENSOR(NAME, TEXT) \
    SYNCMON_LOG_FLOAT(NAME, TEXT)

#endif



float CryostatImplBase::cryostatTemperature0() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature0, "CRYOSTAT_TEMP Te=0")
}

float CryostatImplBase::cryostatTemperature1() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature1, "CRYOSTAT_TEMP Te=1")
}

float CryostatImplBase::cryostatTemperature2() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature2, "CRYOSTAT_TEMP Te=2")
}

float CryostatImplBase::cryostatTemperature3() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature3, "CRYOSTAT_TEMP Te=3")
}

float CryostatImplBase::cryostatTemperature4() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature4, "CRYOSTAT_TEMP Te=4")
}

float CryostatImplBase::cryostatTemperature5() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature5, "CRYOSTAT_TEMP Te=5")
}

float CryostatImplBase::cryostatTemperature6() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature6, "CRYOSTAT_TEMP Te=6")
}

float CryostatImplBase::cryostatTemperature7() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature7, "CRYOSTAT_TEMP Te=7")
}

float CryostatImplBase::cryostatTemperature8() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature8, "CRYOSTAT_TEMP Te=8")
}

float CryostatImplBase::cryostatTemperature9() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature9, "CRYOSTAT_TEMP Te=9")
}

float CryostatImplBase::cryostatTemperature10() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature10, "CRYOSTAT_TEMP Te=10")
}

float CryostatImplBase::cryostatTemperature11() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature11, "CRYOSTAT_TEMP Te=11")
}

float CryostatImplBase::cryostatTemperature12() {
    SYNCMON_LOG_TEMPSENSOR(cryostatTemperature12, "CRYOSTAT_TEMP Te=12")
}

bool CryostatImplBase::backingPumpEnable() {
    SYNCMON_LOG_BOOL(backingPumpEnable, "CRYO_BACKING_PUMP_ENABLE")
}

float CryostatImplBase::supplyCurrent230V() {
    if (!backingPumpEnable_value)
        MONRETURN(supplyCurrent230V, float, 0);
    SYNCMON_LOG_FLOAT(supplyCurrent230V, "CRYO_SUPPLY_CURRENT_230V")
}

bool CryostatImplBase::turboPumpEnable() {
    if (!backingPumpEnable_value)
        MONRETURN(turboPumpEnable, unsigned char, 0);
    SYNCMON_LOG_BOOL(turboPumpEnable, "CRYO_TURBO_PUMP_ENABLE")
}

bool CryostatImplBase::turboPumpErrorState() {
    if (!backingPumpEnable_value)
        MONRETURN(turboPumpErrorState, unsigned char, 0);
    SYNCMON_LOG_BOOL(turboPumpErrorState, "CRYO_TURBO_PUMP_ERROR_STATE")
}

bool CryostatImplBase::turboPumpHighSpeed() {
    if (!backingPumpEnable_value)
        MONRETURN(turboPumpHighSpeed, unsigned char, 0);
    SYNCMON_LOG_BOOL(turboPumpHighSpeed, "CRYO_TURBO_PUMP_HIGH_SPEED")
}

unsigned char CryostatImplBase::gateValveState() {
    SYNCMON_LOG_BYTE(gateValveState, "CRYO_GATE_VALVE_STATE")
}

unsigned char CryostatImplBase::solenoidValveState() {
    if (!backingPumpEnable_value)
        MONRETURN(solenoidValveState, unsigned char, solenoidValveState_value);
    SYNCMON_LOG_BYTE(solenoidValveState, "CRYO_SOLENOID_VALVE_STATE")
}

float CryostatImplBase::vacuumCryostatPressure() {
	SYNCMON_LOG_FLOAT(vacuumCryostatPressure, "CRYO_VACUUM_CRYOSTAT_PRESSURE")
}

float CryostatImplBase::vacuumPortPressure() {
    SYNCMON_LOG_FLOAT(vacuumPortPressure, "CRYO_VACUUM_PORT_PRESSURE")
}

bool CryostatImplBase::vacuumGaugeEnable() {
    SYNCMON_LOG_BOOL(vacuumGaugeEnable, "CRYO_VACUUM_GAUGE_ENABLE")
}

bool CryostatImplBase::vacuumGaugeErrorState() {
    SYNCMON_LOG_BOOL(vacuumGaugeErrorState, "CRYO_VACUUM_GAUGE_ERROR_STATE")
}

void CryostatImplBase::backingPumpEnable(bool val) {
    SYNCCMD_LOG_BOOL(backingPumpEnable, val, "CRYO_BACKING_PUMP_ENABLE") 
}

void CryostatImplBase::turboPumpEnable(bool val) {
    SYNCCMD_LOG_BOOL(turboPumpEnable, val, "CRYO_TURBO_PUMP_ENABLE") 
}

void CryostatImplBase::gateValveState(unsigned char val) {
    SYNCCMD_LOG_INT(gateValveState, val, "CRYO_GATE_VALVE_STATE") 
}

void CryostatImplBase::solenoidValveState(unsigned char val) {
    SYNCCMD_LOG_INT(solenoidValveState, val, "CRYO_SOLENOID_VALVE_STATE") 
}

void CryostatImplBase::vacuumGaugeEnable(bool val) {
    SYNCCMD_LOG_BOOL(vacuumGaugeEnable, val, "CRYO_VACUUM_GAUGE_ENABLE") 
}

void CryostatImplBase::setTVOCoefficient(unsigned char se, unsigned char co, float val) {
    SYNCCMD2_LOG_FLOAT(setTVOCoefficient_RCA + se * 0x0008 + co, val, "CRYO_SET_TVO_COEFFICIENT")
}

void CryostatImplBase::monitorAction(Time *timestamp_p) {
    
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 50 * milliseconds;

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
                backingPumpEnable_value = backingPumpEnable();
                turboPumpEnable_value = turboPumpEnable();
                turboPumpErrorState_value = turboPumpErrorState();
                turboPumpHighSpeed_value = turboPumpHighSpeed();
                monitorPhase = 2;
            case 2:
                gateValveState_value = gateValveState();
                solenoidValveState_value = solenoidValveState();
                vacuumGaugeEnable_value = vacuumGaugeEnable();
                vacuumGaugeErrorState_value = vacuumGaugeErrorState();
                monitorPhase = 3;
                break;
            case 3:
            default:
                if (logMonitors_m)
                    logMon();
                monitorPhase = 0;
                break;
        };
    }
}

DEFINE_MONITORS_REGISTRY(CryostatImplBase)

void CryostatImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:Cryostat: randomized,"
                         "backingPumpEnable,turboPumpEnable,turboPumpErrorState,turboPumpHighSpeed,"
                         "gateValveState,solenoidValveState,vacuumGaugeEnable,vacuumGaugeErrorState,"        
                         "Temp0,Temp1,Temp2,Temp3,"
                         "Temp4,Temp5,Temp6,Temp7,"
                         "Temp8,Temp9,Temp10,Temp11,"
                         "Temp12,CryostatPressure,PortPressure,Isupply230V" << endl;
    
    } else {
        LOG(LM_INFO)  << "AllMonitors:Cryostat: "
                      << (backingPumpEnable_value ? 1 : 0) << "," << (turboPumpEnable_value ? 1 : 0) << "," << (turboPumpErrorState_value ? 1 : 0) << "," << (turboPumpHighSpeed_value ? 1 : 0) << ","
                      << (int) gateValveState_value << "," << (int) solenoidValveState_value << "," << (vacuumGaugeEnable_value ? 1 : 0) << "," << (vacuumGaugeErrorState_value ? 1 : 0) << ","
                      << cryostatTemperature0_value << "," << cryostatTemperature1_value << "," << cryostatTemperature2_value << "," << cryostatTemperature3_value << ","
                      << cryostatTemperature4_value << "," << cryostatTemperature5_value << "," << cryostatTemperature6_value << "," << cryostatTemperature7_value << ","
                      << cryostatTemperature8_value << "," << cryostatTemperature9_value << "," << cryostatTemperature10_value << "," << cryostatTemperature11_value << ","
                      << cryostatTemperature12_value << "," << vacuumCryostatPressure_value << "," << vacuumPortPressure_value << "," << supplyCurrent230V_value << endl;
    }
}

