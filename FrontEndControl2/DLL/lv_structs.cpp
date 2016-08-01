#include "lv_structs.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

const string &canDataAsText(string &target, const unsigned char *data, int len) {
	target.clear();
	char buf[3];
	for (int index = 0; index < len; ++index) {
		sprintf(buf, "%02X", (unsigned) data[index]);
		target += buf;
	}
	return target;
}

//----------------------------------------------------------------------------

void CartStateData_t::streamOut(std::ostream& out) const {
    string tmp;
    canDataAsText(tmp, data, sizeof(data));
    ostringstream ost(ostringstream::out);
    
    ost << "CartStateData_t: enabled =" << (getBool(CART_ENABLED) ? "true" : "false")
        << " observing=" << (getBool(CART_OBSERVING) ? "true" : "false") << endl;
            
    ost << "                 LNAEnabled=" << (getBool(CART_LNA_ENABLED) ? "true" : "false")
        << " SISEnabled=" << (getBool(CART_SIS_ENABLED) ? "true" : "false")
        << " SISMagnet=" << (getBool(CART_SIS_MAGNET_ENABLED) ? "true" : "false") << endl;

    ost << "                 SISHeater=" << (getBool(CART_SIS_HEATER_ENABLED) ? "true" : "false")
        << " LNALED=" << (getBool(CART_HEMT_LED_ENABLED) ? "true" : "false") << endl;
    
    ost << "                 LOPAEnabled=" << (getBool(CART_LO_PA_ENABLED) ? "true" : "false")
        << " photomixerEnabled=" << (getBool(CART_PHOTOMIXER_ENABLED) ? "true" : "false")
        << " zeroIntegrator=" << (getBool(CART_ZERO_INTEGRATOR_ENABLED) ? "true" : "false") << endl;

    ost << tmp << endl; 
    out << ost.str();
}

const int CartStateData_t::offsets[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//----------------------------------------------------------------------------

void CartIFPowerData_t::streamOut(std::ostream& out) const {
    ostringstream ost(ostringstream::out);

    ost << "CartIFPowerData_t: PHOT0_IF0=" << getFloat(PHOT0_IF0) << " PHOT0_IF1=" << getFloat(PHOT0_IF1)
        << " PHOT0_IF2=" << getFloat(PHOT0_IF2) << " PHOT0_IF3=" << getFloat(PHOT0_IF3) << endl;

    ost << "                   PHOT15_IF0=" << getFloat(PHOT15_IF0) << " PHOT15_IF1=" << getFloat(PHOT15_IF1)
        << " PHOT15_IF2=" << getFloat(PHOT15_IF2) << " PHOT15_IF3=" << getFloat(PHOT15_IF3) << endl;

    ost << "                   PCOLD_IF0=" << getFloat(PCOLD_IF0) << " PCOLD_IF1=" << getFloat(PCOLD_IF1)
        << " PCOLD_IF2=" << getFloat(PCOLD_IF2) << " PCOLD_IF3=" << getFloat(PCOLD_IF3) << endl;

    ost << "                   YFACTOR_IF0=" << getFloat(YFACTOR_IF0) << " YFACTOR_IF1=" << getFloat(YFACTOR_IF1)
        << " YFACTOR_IF2=" << getFloat(YFACTOR_IF2) << " YFACTOR_IF3=" << getFloat(YFACTOR_IF3) << endl;

    out << ost.str();
}

const int CartIFPowerData_t::offsets[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};

//----------------------------------------------------------------------------


void CartYTOData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);
	
    ost << "CartYTOData_t: coarseTune=" << getCoarseTune() 
        << " frequency=" << fixed << setprecision(6) << getFrequency() << " GHz" << endl;

	ost << tmp << endl;	
	out << ost.str();
}

const int CartYTOData_t::offsets[] = {0, 2};

//----------------------------------------------------------------------------

void CartPhotomixerData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

	ost << "CartPhotomixer_t: enabled=" << (getEnable() ? "true" : "false")
    	<< " voltage=" << getFloat(PHOTOMIXER_VOLTAGE)
    	<< " current=" << getFloat(PHOTOMIXER_CURRENT) << endl;        
	
	ost << tmp << endl;	
	out << ost.str();
}

const int CartPhotomixerData_t::offsets[] = {0, 1, 5};

//----------------------------------------------------------------------------

void CartPLLData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CartPLLData_t: lock=" << (getBool(PLL_LOCK) ? "true" : "false") 
        << " unlockDetect=" << (getBool(PLL_UNLOCK_DETECT_LATCH) ? "true" : "false") << endl;
    
    ost << "               lockVoltage=" << getFloat(PLL_LOCK_DETECT_VOLTAGE) 
        << " corrVoltage=" << getFloat(PLL_CORRECTION_VOLTAGE) 
        << " refPower=" << getFloat(PLL_REF_TOTAL_POWER) 
        << " ifPower=" << getFloat(PLL_IF_TOTAL_POWER) << endl;
    
    ost << "               loopBW=" << (getBool(PLL_LOOP_BANDWIDTH_SELECT) ? "alt" : "dflt") 
        << " sidebandLock=" << (getBool(PLL_SIDEBAND_LOCK_POL_SELECT) ? "above" : "below") 
        << " nullIntegrator=" << (getBool(PLL_NULL_LOOP_INTEGRATOR) ? "true" : "false") << endl;
    
    ost << "               temperature=" << getFloat(PLL_ASSEMBLY_TEMP) 
        << " YTOHeaterCurrent=" << getFloat(PLL_YTO_HEATER_CURRENT) << endl;

	ost << tmp << endl;	
	out << ost.str();
}

const int CartPLLData_t::offsets[] = {
    0,  //PLL_LOCK,                       // 1 byte unsigned char  
    1,  //PLL_UNLOCK_DETECT_LATCH,        // 1 byte unsigned char
    2,  //PLL_LOCK_DETECT_VOLTAGE,        // 4 byte float 
    6,  //PLL_CORRECTION_VOLTAGE,         // 4 byte float
    10, //PLL_REF_TOTAL_POWER,            // 4 byte float
    14, //PLL_IF_TOTAL_POWER,             // 4 byte float
    18, //PLL_LOOP_BANDWIDTH_SELECT,      // 1 byte unsigned char
    19, //PLL_SIDEBAND_LOCK_POL_SELECT,   // 1 byte unsigned char
    20, //PLL_NULL_LOOP_INTEGRATOR        // 1 byte unsigned char            
    21, //PLL_ASSEMBLY_TEMP,              // 4 byte float
    25  //PLL_YTO_HEATER_CURRENT          // 4 byte float
};

//----------------------------------------------------------------------------

void CartAMCData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

	ost << "CartAMCData_t: AMC A Vd=" << getFloat(AMC_DRAIN_A_VOLTAGE) 
	    << " Id=" << getFloat(AMC_DRAIN_A_CURRENT) 
	    << " Vg=" << getFloat(AMC_GATE_A_VOLTAGE) << endl;
	
	ost << "               AMC B Vd=" << getFloat(AMC_DRAIN_B_VOLTAGE) 
	    << " Id=" << getFloat(AMC_DRAIN_B_CURRENT) 
	    << " Vg=" << getFloat(AMC_GATE_B_VOLTAGE) << endl;
	
	ost << "               AMC E Vd=" << getFloat(AMC_DRAIN_E_VOLTAGE) 
		<< " Id=" << getFloat(AMC_DRAIN_E_CURRENT) 
		<< " Vg=" << getFloat(AMC_GATE_E_VOLTAGE) << endl;
	
	ost << "               multD voltage=" << getByte(AMC_MULTIPLIER_D_VOLTAGE) 
		<< " current=" << getFloat(AMC_MULTIPLIER_D_CURRENT) << endl;
	
	ost << "               5VSupply=" << getFloat(AMC_SUPPLY_VOLTAGE_5V) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

// All fields except the last one are 4-byte float:
const int CartAMCData_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44 };

//----------------------------------------------------------------------------

void CartPAData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CartPAData_t: Pol0 Vd=" << getFloat(PA_POL0_DRAIN_VOLTAGE) 
     	<< " Id=" << getFloat(PA_POL0_DRAIN_CURRENT) 
     	<< " Vg=" << getFloat(PA_POL0_GATE_VOLTAGE) << endl;
    
    ost << "              Pol1 Vd=" << getFloat(PA_POL1_DRAIN_VOLTAGE) 
    	<< " Id=" << getFloat(PA_POL1_DRAIN_CURRENT) 
    	<< " Vg=" << getFloat(PA_POL1_GATE_VOLTAGE) << endl;
    
    ost << "              3VSupply=" << getFloat(PA_SUPPLY_VOLTAGE_3V) 
    	<< " 5VSupply=" << getFloat(PA_SUPPLY_VOLTAGE_5V) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

// All fields are 4-byte float:
const int CartPAData_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28 };

//----------------------------------------------------------------------------

void CartTempData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CartTempData_t: T0=" << getFloat(CARTRIDGE_TEMP0) 
    	<< " T1=" << getFloat(CARTRIDGE_TEMP1) 
    	<< " T2=" << getFloat(CARTRIDGE_TEMP2) 
    	<< " T3=" << getFloat(CARTRIDGE_TEMP3) 
    	<< " T4=" << getFloat(CARTRIDGE_TEMP4) 
    	<< " T5=" << getFloat(CARTRIDGE_TEMP5) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

// All fields are 4-byte float:
const int CartTempData_t::offsets[] = { 0, 4, 8, 12, 16, 20 };

//----------------------------------------------------------------------------

void CartSISData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CartSISData_t: state=" << (getOpenLoop() ? "OpenLoop" : "ClosedLoop") 
    	<< " Vj=" << getFloat(SIS_VOLTAGE) 
    	<< " Ij=" << getFloat(SIS_CURRENT) 
    	<< " Vmag=" << getFloat(SIS_MAGNET_VOLTAGE) 
    	<< " Imag=" << getFloat(SIS_MAGNET_CURRENT) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

const int CartSISData_t::offsets[] = {
    0,  //SIS_OPEN_LOOP,      // 1 byte unsigned char
    1,  //SIS_VOLTAGE,        // 4 byte float
    5,  //SIS_CURRENT,        // 4 byte float
    9,  //SIS_MAGNET_VOLTAGE, // 4 byte float
    13  //SIS_MAGNET_CURRENT  // 4 byte float
};

//----------------------------------------------------------------------------

void CartLNAData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CartLNAData_t: enable=" << (getEnable() ? "true" : "false") << endl;
    
    ost << "               Stage 1 Vd=" << getFloat(LNA1_DRAIN_VOLTAGE) 
    	<< " Id=" << getFloat(LNA1_DRAIN_CURRENT) 
    	<< " Vg=" << getFloat(LNA1_GATE_VOLTAGE) << endl;

    ost << "               Stage 2 Vd=" << getFloat(LNA2_DRAIN_VOLTAGE) 
    	<< " Id=" << getFloat(LNA2_DRAIN_CURRENT) 
    	<< " Vg=" << getFloat(LNA2_GATE_VOLTAGE) << endl;
    
    ost << "               Stage 3 Vd=" << getFloat(LNA3_DRAIN_VOLTAGE) 
    	<< " Id=" << getFloat(LNA3_DRAIN_CURRENT) 
    	<< " Vg=" << getFloat(LNA3_GATE_VOLTAGE) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

const int CartLNAData_t::offsets[] = {
    0,  //LNA_ENABLE,         // 1 byte unsigned char
    1,  //LNA1_DRAIN_VOLTAGE, // 4 byte float
    5,  //LNA1_DRAIN_CURRENT, // 4 byte float
    9,  //LNA1_GATE_VOLTAGE,  // 4 byte float
    13, //LNA2_DRAIN_VOLTAGE, // 4 byte float
    17, //LNA2_DRAIN_CURRENT, // 4 byte float
    21, //LNA2_GATE_VOLTAGE,  // 4 byte float
    25, //LNA3_DRAIN_VOLTAGE, // 4 byte float
    29, //LNA3_DRAIN_CURRENT, // 4 byte float
    33  //LNA3_GATE_VOLTAGE   // 4 byte float
};

//----------------------------------------------------------------------------

void CartAuxData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CartAuxData_t: heaterCurrent=" << getHeaterCurrent() 
    	<< " LED=" << (getLEDEnable() ? "enabled" : "disabled") << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

const int CartAuxData_t::offsets[] = { 0, 4 };

//----------------------------------------------------------------------------

void PowerModuleData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "PowerModuleData_t: " << (getEnable() ? "enabled" : "disabled") << endl;
    ost << "                    +6V=" << getFloat(VOLTAGE_P6V) << " I=" << getFloat(CURRENT_P6V) << endl;
    ost << "                    -6V=" << getFloat(VOLTAGE_N6V) << " I=" << getFloat(CURRENT_N6V) << endl;
    ost << "                   +15V=" << getFloat(VOLTAGE_P15V) << " I=" << getFloat(CURRENT_P15V) << endl;
    ost << "                   -15V=" << getFloat(VOLTAGE_N15V) << " I=" << getFloat(CURRENT_N15V) << endl;
    ost << "                   +24V=" << getFloat(VOLTAGE_P24V) << " I=" << getFloat(CURRENT_P24V) << endl;
    ost << "                    +8V=" << getFloat(VOLTAGE_P8V) << " I=" << getFloat(CURRENT_P8V) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

const int PowerModuleData_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48 };

//----------------------------------------------------------------------------

void IFSwitchData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "IFSwitchData_t: observing=" << ((int) getByte(OBSERVING_BAND)) << endl;
    
    ost << "                temperature=" << getFloat(TEMPERATURE_POL0SB1) 
    	<< " " << getFloat(TEMPERATURE_POL0SB2) 
    	<< " " << getFloat(TEMPERATURE_POL1SB1) 
    	<< " " << getFloat(TEMPERATURE_POL1SB2) << endl; 
    
    ost << "                attenuation=" << ((int) getByte(ATTENUATION_POL0SB1)) 
    	<< " " << ((int) getByte(ATTENUATION_POL0SB2)) 
    	<< " " << ((int) getByte(ATTENUATION_POL1SB1)) 
    	<< " " << ((int) getByte(ATTENUATION_POL1SB2)) << endl;
    
    ost << "                tempServo=" << ((int) getByte(TEMPSERVO_POL0SB1))
    	<< " " << ((int) getByte(TEMPSERVO_POL0SB2)) 
    	<< " " << ((int) getByte(TEMPSERVO_POL1SB1)) 
    	<< " " << ((int) getByte(TEMPSERVO_POL1SB2)) << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

const int IFSwitchData_t::offsets[] = { 0, 1, 2, 3, 4, 8, 12, 16, 20, 21, 22, 23, 24 };

//----------------------------------------------------------------------------
void CryostatData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "CryostatData_t: temperatures=" << getFloat(CRYOSTAT_TEMP_0) 
    	<< " " << getFloat(CRYOSTAT_TEMP_1) 
    	<< " " << getFloat(CRYOSTAT_TEMP_2) 
    	<< " " << getFloat(CRYOSTAT_TEMP_3) 
    	<< " " << getFloat(CRYOSTAT_TEMP_4) << endl;
    
    ost << "                            " << getFloat(CRYOSTAT_TEMP_5) 
    	<< " " << getFloat(CRYOSTAT_TEMP_6)
    	<< " " << getFloat(CRYOSTAT_TEMP_7) 
    	<< " " << getFloat(CRYOSTAT_TEMP_8) 
    	<< " " << getFloat(CRYOSTAT_TEMP_9) << endl;
    
    ost << "                            " << getFloat(CRYOSTAT_TEMP_10) 
    	<< " " << getFloat(CRYOSTAT_TEMP_11)
    	<< " " << getFloat(CRYOSTAT_TEMP_12) << endl;
    
    ost << "                230V current=" << getFloat(SUPPLY_CURRENT_230V) << " A" << endl;
    
    ost << "                backingPump=" << (getByte(BACKING_PUMP_ENABLE) ? "ON(1)" : "OFF(0)") << endl;
    
    ost << "                turboPump=" << (getByte(TURBO_PUMP_ENABLE) ? "ON(1)" : "OFF(0)") 
    	<< " state=" << (getByte(TURBO_PUMP_STATE) ? "ERROR(1)" : "OK(0)") 
    	<< " speed=" << (getByte(TURBO_PUMP_SPEED) ? "LOW(1)" : "HIGH(0)") << endl;
    
    ost << "                gateValve=" << ((int) getByte(GATE_VALVE_STATE))
    	<< " solenoidValve=" << ((int) getByte(SOLENOID_VALVE_STATE)) << endl;
    
    ost << "                pressures=" << getFloat(VACUUM_CRYOSTAT_PRES) << " " << getFloat(VACUUM_PORT_PRES) << endl;
    
    ost << "                vacuumControl=" << (getByte(VACUUM_GAUGE_ENABLE) ? "ON(1)" : "OFF(0)") 
    	<< " " << (getByte(VACUUM_GAUGE_STATE) ? "ERROR(1)" : "OK(0)") << "" << endl;
	
	ost << tmp << endl;	
	out << ost.str();
}

const int CryostatData_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 
                                        36, 40, 44, 48, 52, 56, 60, 64,
                                        65, 66, 67, 68, 69, 70, 71 };

//----------------------------------------------------------------------------
   
void LPRData_t::streamOut(std::ostream& out) const {
	string tmp;
	canDataAsText(tmp, data, sizeof(data));
	ostringstream ost(ostringstream::out);

    ost << "LPRData_t: port=" << ((int) getByte(OPTICAL_SWITCH_PORT)) 
    	<< " shutter=" << ((int) getByte(OPTICAL_SWITCH_SHUTTER))
    	<< " state=" << ((int) getByte(OPTICAL_SWITCH_STATE))
    	<< " busy=" << ((int) getByte(OPTICAL_SWITCH_BUSY)) << endl;
    
    ost << "           LPR temperatures=" << getFloat(LPR_TEMPERATURE_0) << " " << getFloat(LPR_TEMPERATURE_1) << endl;
    
    ost << "           EDFA laser temp=" << getFloat(EDFA_LASER_PUMP_TEMPERATURE) 
    	<< " current=" << getFloat(EDFA_LASER_DRIVE_CURRENT) 
    	<< " photodetect current=" << getFloat(EDFA_LASER_PHOTODETECT_CURRENT) << endl;
    
    ost << "           EDFA photodetect current=" << getFloat(EDFA_PHOTODETECT_CURRENT) 
    	<< " photodetect power=" << getFloat(EDFA_PHOTODETECT_POWER) 
    	<< " mod input=" << getFloat(EDFA_MODULATION_INPUT) << endl;

	ost << tmp << endl;	
	out << ost.str();
}

const int LPRData_t::offsets[] = { 0, 1, 2, 3, 4, 8, 12, 16, 20, 24, 28, 32 };

//----------------------------------------------------------------------------

void LORTMStatus_t::streamOut(std::ostream& out) const {
    string tmp;
    canDataAsText(tmp, data, sizeof(data));
    ostringstream ost(ostringstream::out);

    ost << "LORTMStatus_t: status=" << (int) (getByte(SYSTEM_STATE))
        << " warning=" << (getBool(SYSTEM_WARNING) ? 1 : 0)
        << " error=" << (getBool(SYSTEM_ERROR) ? 1 : 0) << endl;
    
    ost << "               opPending=" << (getBool(OPERATION_PENDING) ? 1 : 0) 
        << " interlock=" << (getBool(INTERLOCK_STATUS) ? 1 : 0) << endl;
    
    ost << "               warnCode=" << getShort(SYSTEM_WARNING_CODE)
        << " errorCode=" << getShort(SYSTEM_ERROR_CODE) << endl;

    ost << "               phaseLocked=" << (getBool(PHASELOCK_LOCKED) ? 1 : 0)
        << " tuningReady=" << (getBool(PHASELOCK_TUNING_READY) ? 1 : 0)
        << " RFInputReady=" << (getBool(PHASELOCK_RF_INPUT_READY) ? 1 : 0) << endl;
    
    ost << "               selectedLaser=" << (int) (getByte(SELECTED_LASER))
        << " selectedBand=" << (int) (getByte(SELECTED_BAND)) << endl;
    ost << tmp << endl;
    out << ost.str();
}

const int LORTMStatus_t::offsets[] = { 0, 1, 2, 3, 4, 5, 7, 9, 10, 11, 12, 13 };

//----------------------------------------------------------------------------

void LORTMLasers_t::streamOut(std::ostream& out) const {
    string tmp;
    canDataAsText(tmp, data, sizeof(data));
    ostringstream ost(ostringstream::out);

    ost << "LORTMLasers_t: status1=" << (int) (getByte(LASER_STATUS1))
        << " bias1=" << getFloat(LASER_BIAS_MON1)
        << " temp1=" << getFloat(LASER_TEMP_MON1) << endl;

    ost << "               status2=" << (int) (getByte(LASER_STATUS2))
        << " bias2=" << getFloat(LASER_BIAS_MON2)
        << " temp2=" << getFloat(LASER_TEMP_MON2) << endl;

    ost << "               status3=" << (int) (getByte(LASER_STATUS3))
        << " bias3=" << getFloat(LASER_BIAS_MON3)
        << " temp3=" << getFloat(LASER_TEMP_MON3) << endl;

    ost << "               status4=" << (int) (getByte(LASER_STATUS4))
        << " bias4=" << getFloat(LASER_BIAS_MON4)
        << " temp4=" << getFloat(LASER_TEMP_MON4) << endl;

    ost << "               slowCorr=" << getFloat(LASER_SLOW_CORR_MON) << endl;

    ost << tmp << endl;
    out << ost.str();
}

const int LORTMLasers_t::offsets[] = { 0, 1, 2, 3, 4, 8, 12, 16, 20, 24, 28, 32, 36 };

//----------------------------------------------------------------------------

void LORTMOther_t::streamOut(std::ostream& out) const {
    string tmp;
    canDataAsText(tmp, data, sizeof(data));
    ostringstream ost(ostringstream::out);

    ost << "LORTMOther_t: laserTECI1=" << getFloat(LASER_TEC_I_MON1)
        << " power1=" << getFloat(LASER_POW_MON1)
        << " phmixBias1" << getFloat(PHMIX_BIAS_MON1) << endl;

    ost << "              laserTECI2=" << getFloat(LASER_TEC_I_MON2)
        << " power2=" << getFloat(LASER_POW_MON2)
        << " phmixBias2" << getFloat(PHMIX_BIAS_MON2) << endl;

    ost << "              laserTECI3=" << getFloat(LASER_TEC_I_MON3)
        << " power3=" << getFloat(LASER_POW_MON3)
        << " phmixBias3" << getFloat(PHMIX_BIAS_MON3) << endl;

    ost << "              laserTECI4=" << getFloat(LASER_TEC_I_MON4)
        << " power4=" << getFloat(LASER_POW_MON4)
        << " phmixBias4" << getFloat(PHMIX_BIAS_MON4) << endl;

    ost << "              RFAGCGain=" << getFloat(RF_AGC_GAIN_MON)
        << " RFPow34dB=" << getFloat(RF_POW_MON_34DB)
        << " externTherm=" << getFloat(EXTERN_THERM_MON)
        << " internTherm=" << getFloat(INTERN_THERM_MON) << endl;

    ost << tmp << endl;
    out << ost.str();
}

const int LORTMOther_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60 };

//----------------------------------------------------------------------------

void FETIMData_t::streamOut(std::ostream& out) const {
    string tmp;
    canDataAsText(tmp, data, sizeof(data));
    ostringstream ost(ostringstream::out);

    ost << "FETIMData_t: internalTemp1=" << getFloat(INTERNAL_TEMP1)
        << " internalTemp2=" << getFloat(INTERNAL_TEMP2)
        << " internalTemp3=" << getFloat(INTERNAL_TEMP3)
        << " internalTemp4=" << getFloat(INTERNAL_TEMP4)
        << " internalTemp5=" << getFloat(INTERNAL_TEMP5) << endl;

    ost << "             externalTemp1=" << getFloat(EXTERNAL_TEMP1)
        << " externalTemp2=" << getFloat(EXTERNAL_TEMP2)
        << " airflow1=" << getFloat(AIRFLOW1)
        << " airflow2=" << getFloat(AIRFLOW2) << endl;

    ost << "             HE2Pressure=" << getFloat(HE2_PRESSURE)
        << " glitchValue=" << getFloat(GLITCH_VALUE) << endl;

    ost << "             internalTempOOR=" << (getBool(INTERNAL_TEMP_OOR) ? 1 : 0)
        << " externalTempOOR1=" << (getBool(EXTERNAL_TEMP1_OOR) ? 1 : 0)
        << " externalTempOOR2=" << (getBool(EXTERNAL_TEMP2_OOR) ? 1 : 0)
        << " airflowOOR=" << (getBool(AIRFLOW_OOR) ? 1 : 0)
        << " HE2PressureOOR=" << (getBool(HE2_PRESSURE_OOR) ? 1 : 0) << endl;

    ost << "             sensorSingleFail=" << (getBool(SENSOR_SINGLE_FAIL) ? 1 : 0)
        << " sensorMultiFail=" << (getBool(SENSOR_MULTI_FAIL) ? 1 : 0)
        << " glitchCounterTrig= " << (getBool(GLITCH_COUNTER_TRIG) ? 1 : 0)
        << " delayShutdownTrig= " << (getBool(DELAY_SHUTDOWN_TRIG) ? 1 : 0)
        << " finalShutdownTrig= " << (getBool(FINAL_SHUTDOWN_TRIG) ? 1 : 0) << endl;

    ost << "             compStatus=" << (getBool(COMP_STATUS) ? 1 : 0)
        << " compInterlockStatus=" << (getBool(COMP_INTERLOCK_STATUS) ? 1 : 0)
        << " compCableStatus=" << (getBool(COMP_CABLE_STATUS) ? 1 : 0) << endl;

    ost << tmp << endl;
    out << ost.str();
}

const int FETIMData_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56 };

//----------------------------------------------------------------------------

void CompressorData_t::streamOut(std::ostream& out) const {
    string tmp;
    canDataAsText(tmp, data, sizeof(data));
    ostringstream ost(ostringstream::out);

    ost << "CompressorData_t: temp1=" << getFloat(TEMP1)
        << " temp2=" << getFloat(TEMP2)
        << " temp3=" << getFloat(TEMP3)
        << " temp4=" << getFloat(TEMP4) << endl;

    ost << "                  returnPres=" << getFloat(RETURN_PRESSURE)
        << " supplyPres=" << getFloat(SUPPLY_PRESSURE)
        << " auxTransducer=" << getFloat(AUX_TRANSDUCER) << endl;

    ost << "                  timeSincePowerOn=" << getULong(TIME_SINCE_POWER_ON)
        << " timeSincePowerOff=" << getULong(TIME_SINCE_POWER_OFF) << endl;

    ost << "                  tempAlarm=" << (getBool(TEMP_ALARM) ? 1 : 0)
        << " presAlarm=" << (getBool(PRESSURE_ALARM) ? 1 : 0)
        << " driveIndicator=" << (getBool(DRIVE_INDICATOR) ? 1 : 0)
        << " faultStatus=" << (getBool(FAULT_STATUS) ? 1 : 0) << endl;

    ost << "                  ICCUStatus=" << (getBool(ICCU_STATUS) ? 1 : 0)
        << " ICCUCableError=" << (getBool(ICCU_CABLE_ERROR) ? 1 : 0)
        << " FETIMStatus=" << (getBool(FETIM_STATUS) ? 1 : 0)
        << " FETIMCableError=" << (getBool(FETIM_CABLE_ERROR) ? 1 : 0)
        << " InterlockOverride=" << (getBool(INTERLOCK_OVERRIDE) ? 1 : 0)
        << " FaultStatus=" << (getBool(FAULT_STATUS) ? 1 : 0) << endl;

    ost << tmp << endl;
    out << ost.str();
}

const int CompressorData_t::offsets[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 37, 38, 39, 40, 41, 42, 43, 44 };


