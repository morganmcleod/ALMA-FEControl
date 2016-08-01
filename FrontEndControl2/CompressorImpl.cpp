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


#include "CompressorImpl.h"
#include "logger.h"
#include "stringConvert.h"
using namespace std;

CompressorImpl::CompressorImpl(unsigned long channel,
                               unsigned long nodeAddress)
  : CompressorImplBase("Compressor"),
    thermalLogger_mp(new ThermalLogger(*this, "Compressor")),
    thermaLogInterval_m(30)
{
    CompressorImplBase::initialize(channel, nodeAddress);
}

CompressorImpl::~CompressorImpl() {
    delete thermalLogger_mp;
}

void CompressorImpl::startMonitor() {
    CompressorImplBase::startMonitor();
    if (thermalLogger_mp)
        thermalLogger_mp -> start(thermaLogInterval_m);
}

void CompressorImpl::stopMonitor() {
    if (thermalLogger_mp)
        thermalLogger_mp -> stop();
    CompressorImplBase::stopMonitor();
}

void CompressorImpl::setThermalLogInterval(unsigned int interval) {
    thermaLogInterval_m = interval;
    LOG(LM_INFO) << "CompressorImpl: setThermalLogInterval=" << thermaLogInterval_m << endl;
    if (thermalLogger_mp)
        thermalLogger_mp -> setLoggingInterval(thermaLogInterval_m);
}

bool CompressorImpl::getMonitorCompressor(Compressor_t &target) {
    memset(&target, 0, sizeof(target));
    target.getTemp1_value = getTemp1_value;
    target.getTemp2_value = getTemp2_value;
    target.getTemp3_value = getTemp3_value;
    target.getTemp4_value = getTemp4_value;
    target.getReturnLinePressure_value = getReturnLinePressure_value;
    target.getAuxTransducer_value = getAuxTransducer_value;
    target.getSupplyPressure_value = getSupplyPressure_value;
    target.getPressureAlarm_value = getPressureAlarm_value;
    target.getTempAlarm_value = getTempAlarm_value;
    target.getDriveIndicator_value = getDriveIndicator_value;
    target.getICCUStatusError_value = getICCUStatusError_value;
    target.getICCUCableError_value = getICCUCableError_value;
    target.getFETIMStatusError_value = getFETIMStatusError_value;
    target.getFETIMCableError_value = getFETIMCableError_value;
    target.getInterlockOverride_value = getInterlockOverride_value;
    target.getFaultStatusError_value = getFaultStatusError_value;
    target.getTimeSinceLastPowerOn_value = getTimeSinceLastPowerOn_value;
    target.getTimeSinceLastPowerOff_value = getTimeSinceLastPowerOff_value;
    return true;
}

void CompressorImpl::appendThermalLog(std::string &target) const {
    target += "\t" + to_string(getTemp1_value, std::fixed, 2);
    target += "\t" + to_string(getTemp2_value, std::fixed, 2);
    target += "\t" + to_string(getTemp3_value, std::fixed, 2);
    target += "\t" + to_string(getTemp4_value, std::fixed, 2);

    target += "\t" + to_string(getReturnLinePressure_value, std::fixed, 2);
    target += "\t" + to_string(getSupplyPressure_value, std::fixed, 2);
    target += "\t" + to_string(getAuxTransducer_value, std::fixed, 2);

    target += "\t" + to_string(getTempAlarm_value ? 1 : 0);
    target += "\t" + to_string(getPressureAlarm_value ? 1 : 0);
    target += "\t" + to_string(getDriveIndicator_value ? 1 : 0);

    target += "\t" + to_string(getICCUStatusError_value ? 1 : 0);
    target += "\t" + to_string(getICCUCableError_value ? 1 : 0);
    target += "\t" + to_string(getFETIMStatusError_value ? 1 : 0);
    target += "\t" + to_string(getFETIMCableError_value ? 1 : 0);

    target += "\t" + to_string(getInterlockOverride_value ? 1 : 0);
    target += "\t" + to_string(getTimeSinceLastPowerOn_value);
    target += "\t" + to_string(getTimeSinceLastPowerOff_value);
}

void CompressorImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tcompTemp1\tcompTemp2\tcompTemp3\tcompTemp4"
              "\tcompReturnPres\tcompSuppyPres\tcompAux"
              "\tcompTempAlarm\tcompPresAlarm\tcompDriveInd"
              "\tICCUStatusError\tICCUCableError\tFETIMStatusError\tFETIMCableError"
              "\tinterlockOverride\ttimeSincePowerOn\ttimeSincePowerOff";
}




