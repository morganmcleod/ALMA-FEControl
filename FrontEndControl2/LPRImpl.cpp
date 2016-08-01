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

#include "LPRImpl.h"
#include <stdio.h>  // for sprintf()
using namespace std;

LPRImpl::LPRImpl(unsigned long channel, 
                 unsigned long nodeAddress, 
                 const std::string &ESN)
  : LPRImplBase("LPR"),
    observingBand_m(0)
{ 
    setESN(ESN);
    LPRImplBase::initialize(channel, nodeAddress);
}

bool LPRImpl::setObservingBand(int band) {
    if (band < 1 || band > 10)
        return false;
    opticalSwitchPort((unsigned char) (band - 1));
    observingBand_m = band;
    unsigned long beginTime = GETTIMER();
    unsigned long elapsed;
    bool timeout = false;
    while (!timeout && opticalSwitchBusy()) {
        SLEEP(5);
        elapsed = GETTIMER() - beginTime;
        if (elapsed > 2000)
            timeout = true;
    }
    return !timeout;
}

bool LPRImpl::getMonitorLPR(LPR_t &target) {
    memset(&target, 0, sizeof(target));
    target.LPRTemperature0_value = LPRTemperature0_value;
    target.LPRTemperature1_value = LPRTemperature1_value;
    target.opticalSwitchPort_value = opticalSwitchPort() + 1;
    target.opticalSwitchShutter_value = opticalSwitchShutter_value;
    target.opticalSwitchState_value = opticalSwitchState_value;
    target.opticalSwitchBusy_value = opticalSwitchBusy_value;
    target.EDFALaserPumpTemperature_value = EDFALaserPumpTemperature_value;
    target.EDFALaserDriveCurrent_value = EDFALaserDriveCurrent_value;
    target.EDFALaserPhotoDetectCurrent_value = EDFALaserPhotoDetectCurrent_value;
    target.EDFAPhotoDetectCurrent_value = EDFAPhotoDetectCurrent_value;
    target.EDFAPhotoDetectPower_value = EDFAPhotoDetectPower_value;
    target.EDFAModulationInput_value = EDFAModulationInput_value;
    return true;
}


void LPRImpl::appendThermalLog(std::string &target) const {
    char buf[20];    
    sprintf(buf, "\t%.2f", LPRTemperature0_value);
    target += buf;
    sprintf(buf, "\t%.2f", LPRTemperature1_value);
    target += buf;
    sprintf(buf, "\t%.2f", EDFALaserPumpTemperature_value);
    target += buf;
}

void LPRImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tLPR 0\tLPR 1\tLPR laser pump";
}

