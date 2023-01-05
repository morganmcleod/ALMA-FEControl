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
#include "OPTIMIZE/CryostatPumping.h"
using namespace std;
#include <cmath>

CryostatImpl::CryostatImpl(unsigned long channel, 
                           unsigned long nodeAddress,
                           const std::string &ESN)
  : CryostatImplBase("Cryostat"),
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

bool CryostatImpl::getMonitorCryostat(Cryostat_t &target) const {
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
    return true;
}


void CryostatImpl::appendThermalLog(std::string &target) const {
    target += "\t" + to_string(backingPumpEnable_value ? 1 : 0);
    target += "\t" + to_string(turboPumpEnable_value ? 1 : 0);
    target += "\t" + to_string(turboPumpErrorState_value ? 1 : 0);
    target += "\t" + to_string(turboPumpHighSpeed_value ? 1 : 0);
    target += "\t" + to_string((int) gateValveState_value);
    target += "\t" + to_string((int) solenoidValveState_value);
    target += "\t" + to_string(supplyCurrent230V_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature0_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature1_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature2_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature3_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature4_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature5_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature6_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature7_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature8_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature9_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature10_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature11_value, std::fixed, 2);
    target += "\t" + to_string(cryostatTemperature12_value, std::fixed, 2);
    target += "\t" + to_string(vacuumCryostatPressure_value, std::scientific, 4);
    target += "\t" + to_string(vacuumPortPressure_value, std::scientific, 4);
}

void CryostatImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tbacking pump\tturbo pump\tturbo pump error\tturbo pump speed"
              "\tgate valve\tsolenoid valve\t230V current"
              "\t4K stage\t4K link 1\t4K link 2\t4K far side 1\t4K far side 2"
              "\t15K stage\t15K link\t15K far side\t15K shield"
              "\t110K stage\t110K link\t110K far side\t110K shield"
              "\tcryo pressure\tport pressure";
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
