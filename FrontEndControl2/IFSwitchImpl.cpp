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

#include "IFSwitchImpl.h"
#include <stdio.h>      // for sprintf()
using namespace std;

IFSwitchImpl::IFSwitchImpl(unsigned long channel, 
                           unsigned long nodeAddress)
  : IFSwitchImplBase("IFSwitch"),
    observingBand_m(0)
{ 
    IFSwitchImplBase::initialize(channel, nodeAddress);
}

bool IFSwitchImpl::setObservingBand(int band) {
    if (band < 1 || band > 10)
        return false;
    switchCartridge((unsigned char) (band - 1));
    observingBand_m = band;
    return true;
}

bool IFSwitchImpl::getMonitorIFSwitch(IFSwitch_t &target) {
    memset(&target, 0, sizeof(target));
    target.pol0Sb1Attenuation_value = pol0Sb1Attenuation_value;
    target.pol0Sb2Attenuation_value = pol0Sb2Attenuation_value;
    target.pol1Sb1Attenuation_value = pol1Sb1Attenuation_value;
    target.pol1Sb2Attenuation_value = pol1Sb2Attenuation_value;
    target.pol0Sb1AssemblyTemp_value = pol0Sb1AssemblyTemp_value;
    target.pol0Sb2AssemblyTemp_value = pol0Sb2AssemblyTemp_value;
    target.pol1Sb1AssemblyTemp_value = pol1Sb1AssemblyTemp_value;
    target.pol1Sb2AssemblyTemp_value = pol1Sb2AssemblyTemp_value;
    target.pol0Sb1TempServoEnable_value = pol0Sb1TempServoEnable_value;
    target.pol0Sb2TempServoEnable_value = pol0Sb2TempServoEnable_value;
    target.pol1Sb1TempServoEnable_value = pol1Sb1TempServoEnable_value;
    target.pol1Sb2TempServoEnable_value = pol1Sb2TempServoEnable_value;
    target.switchCartridge_value = switchCartridge() + 1;
    return true;
}


void IFSwitchImpl::appendThermalLog(std::string &target) const {
    char buf[20];    
    sprintf(buf, "\t%.2f", pol0Sb1AssemblyTemp_value);
    target += buf;
    sprintf(buf, "\t%.2f", pol0Sb2AssemblyTemp_value);
    target += buf;
    sprintf(buf, "\t%.2f", pol1Sb1AssemblyTemp_value);
    target += buf;
    sprintf(buf, "\t%.2f", pol1Sb2AssemblyTemp_value);
    target += buf;
}

void IFSwitchImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tIFSW pol0 sb1\tIFSW pol0 sb2\tIFSW pol1 sb1\tIFSW pol1 sb2";
}




