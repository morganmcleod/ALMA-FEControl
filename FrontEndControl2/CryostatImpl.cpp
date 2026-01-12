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

#include "CryostatImpl.h"
#include "logger.h"
#include "stringConvert.h"
#include "FEMCEventQueue.h"
#include "CONFIG/FrontEndDatabase.h"
#include "OPTIMIZE/CryostatPumping.h"
using namespace std;
#include <cmath>

CryostatImpl::CryostatImpl(unsigned long channel, 
                           unsigned long nodeAddress,
                           const std::string &ESN)
  : CryostatImplBase("Cryostat"),
    dbObject_mp(NULL),
    headerId_m(FEICDataBase::ID_T()),
    recordCoolDownPlot_m(false),
    cryoPumpMonitor_mp(NULL)
{
    setESN(ESN);
    CryostatImplBase::initialize(channel, nodeAddress);
}

CryostatImpl::~CryostatImpl() {
    if (cryoPumpMonitor_mp) {
        cryoPumpMonitor_mp -> stop();        
        delete cryoPumpMonitor_mp;
    }
}

bool CryostatImpl::cryoPumpingEnable(bool val) {
    if (val) {
	    if (!backingPumpEnable()) {
	        string msg("Cryostat pumping process: Backing pump is not enabled.");
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << msg << endl;
	        return false;
	    }
	    LOG(LM_INFO) << "Cryostat pumping process: Starting..." << endl;
		if (!cryoPumpMonitor_mp)
			cryoPumpMonitor_mp = new CryostatPumping(*this);
		cryoPumpMonitor_mp -> start();

	} else if (cryoPumpMonitor_mp) {
	    LOG(LM_INFO) << "Cryostat pumping process: Stopping..." << endl;
	    cryoPumpMonitor_mp -> stop();
	}
	return true;
}

void CryostatImpl::setCryostatConfig(const FEConfig::CryostatConfig &params) {
    LOG(LM_INFO) << "Cryostat setting coefficients:" << endl << params.tvoCoeff_m << endl;

    for (auto& elem : params.tvoCoeff_m) {
        auto sensor = elem.first;
        auto data = elem.second;
        unsigned char se = (unsigned char)(sensor - 1);
        if (se > 8 || se < 0) continue;
        for (unsigned char co = 0; co < 7; co++) {
            auto val = data.get(co);
            setTVOCoefficient(se, co, val);
        }
    }
}

void CryostatImpl::startCooldownPlot(FrontEndDatabase &dbObject, const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus)
{
    dbObject_mp = &dbObject;
    if (dbObject_mp -> startCryostatCooldownPlot(headerId_m, feConfig, dataStatus))
        recordCoolDownPlot_m = true;
}

void CryostatImpl::stopCooldownPlot() {
    headerId_m = FEICDataBase::ID_T();
    recordCoolDownPlot_m = false;
}

bool CryostatImpl::getMonitorCryostat(Cryostat_t &target) const {
    impl_getMonitorCryostat(target);
    return true;
}

void CryostatImpl::appendThermalLog(std::string &target) const {
    Cryostat_t state;
    impl_getMonitorCryostat(state);
    target += "\t" + to_string(state.backingPumpEnable_value ? 1 : 0);
    target += "\t" + to_string(state.turboPumpEnable_value ? 1 : 0);
    target += "\t" + to_string(state.turboPumpErrorState_value ? 1 : 0);
    target += "\t" + to_string(state.turboPumpHighSpeed_value ? 1 : 0);
    target += "\t" + to_string((int) state.gateValveState_value);
    target += "\t" + to_string((int) state.solenoidValveState_value);
    target += "\t" + to_string(state.supplyCurrent230V_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature0_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature1_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature2_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature3_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature4_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature5_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature6_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature7_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature8_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature9_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature10_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature11_value, std::fixed, 2);
    target += "\t" + to_string(state.cryostatTemperature12_value, std::fixed, 2);
    target += "\t" + to_string(state.vacuumCryostatPressure_value, std::scientific, 4);
    target += "\t" + to_string(state.vacuumPortPressure_value, std::scientific, 4);
    if (recordCoolDownPlot_m && dbObject_mp)
        dbObject_mp -> insertCryostatCooldownData(headerId_m, state);
}

void CryostatImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tbacking pump\tturbo pump\tturbo pump error\tturbo pump speed"
              "\tgate valve\tsolenoid valve\t230V current"
              "\t4K stage\t4K link 1\t4K link 2\t4K far side 1\t4K far side 2"
              "\t15K stage\t15K link\t15K far side\t15K shield"
              "\t110K stage\t110K link\t110K far side\t110K shield"
              "\tcryo pressure\tport pressure";
}

void CryostatImpl::impl_getMonitorCryostat(Cryostat_t &target) const {
    memset(&target, 0, sizeof(target));
    target.cryostatTemperature0_value = checkCryostatTemperature(cryostatTemperature0_value);
    target.cryostatTemperature1_value = checkCryostatTemperature(cryostatTemperature1_value);
    target.cryostatTemperature2_value = checkCryostatTemperature(cryostatTemperature2_value);
    target.cryostatTemperature3_value = checkCryostatTemperature(cryostatTemperature3_value);
    target.cryostatTemperature4_value = checkCryostatTemperature(cryostatTemperature4_value);
    target.cryostatTemperature5_value = checkCryostatTemperature(cryostatTemperature5_value);
    target.cryostatTemperature6_value = checkCryostatTemperature(cryostatTemperature6_value);
    target.cryostatTemperature7_value = checkCryostatTemperature(cryostatTemperature7_value);
    target.cryostatTemperature8_value = checkCryostatTemperature(cryostatTemperature8_value);
    target.cryostatTemperature9_value = checkCryostatTemperature(cryostatTemperature9_value);
    target.cryostatTemperature10_value = checkCryostatTemperature(cryostatTemperature10_value);
    target.cryostatTemperature11_value = checkCryostatTemperature(cryostatTemperature11_value);
    target.cryostatTemperature12_value = checkCryostatTemperature(cryostatTemperature12_value);
    target.backingPumpEnable_value = backingPumpEnable_value;
    target.turboPumpEnable_value = turboPumpEnable_value;
    target.turboPumpErrorState_value = turboPumpErrorState_value;
    target.turboPumpHighSpeed_value = turboPumpHighSpeed_value;
    target.gateValveState_value = gateValveState_value;
    target.solenoidValveState_value = solenoidValveState_value;
    target.vacuumCryostatPressure_value = vacuumCryostatPressure_value;
    target.vacuumPortPressure_value = vacuumPortPressure_value;
    target.vacuumGaugeEnable_value = vacuumGaugeEnable_value;
    target.vacuumGaugeErrorState_value = vacuumGaugeErrorState_value;
    target.supplyCurrent230V_value = supplyCurrent230V_value;
}

float CryostatImpl::checkCryostatTemperature(float value) const {
    const float maxTemp = 350.0;
    const float minTemp = 0.0;
    if (isnan(value))
        return -1;
    if (value <= minTemp)
        return -1;
    if (value >= maxTemp)
        return -1;
    return value;
}
