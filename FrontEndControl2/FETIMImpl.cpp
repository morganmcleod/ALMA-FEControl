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

#include "FETIMImpl.h"
#include "logger.h"
#include "stringConvert.h"
using namespace std;

FETIMImpl::FETIMImpl(unsigned long channel,
                     unsigned long nodeAddress,
                     const std::string &ESN)
  : FETIMImplBase("FETIM")
{
    setESN(ESN);
    FETIMImplBase::initialize(channel, nodeAddress);
    FEHardwareDevice::maxErrorCount_m = 1;
}

bool FETIMImpl::getMonitorFETIM(FETIM_t &target) {
    if (FEHardwareDevice::exceededErrorCount())
        return false;

    memset(&target, 0, sizeof(target));
    target.internalTemperature1_value = internalTemperature1_value;
    target.internalTemperature2_value = internalTemperature2_value;
    target.internalTemperature3_value = internalTemperature3_value;
    target.internalTemperature4_value = internalTemperature4_value;
    target.internalTemperature5_value = internalTemperature5_value;
    target.internalTemperatureOOR_value = internalTemperatureOOR_value;

    target.externalTemperature1_value = externalTemperature1_value;
    target.externalTemperature2_value = externalTemperature2_value;
    target.externalTemperatureOOR1_value = externalTemperatureOOR1_value;
    target.externalTemperatureOOR2_value = externalTemperatureOOR2_value;

    target.getAirflowSensor1_value = getAirflowSensor1_value;
    target.getAirflowSensor2_value = getAirflowSensor2_value;
    target.airflowSensorOOR_value = airflowSensorOOR_value;

    target.heliumBufferPressure_value = heliumBufferPressure_value;
    target.heliumBufferPressureOOR_value = heliumBufferPressureOOR_value;

    target.sensorSingleFailed_value = sensorSingleFailed_value;
    target.sensorMultiFailed_value = sensorMultiFailed_value;
    target.glitchValue_value = glitchValue_value;
    target.glitchCounterTriggered_value = glitchCounterTriggered_value;

    target.delayShutdownTriggered_value = delayShutdownTriggered_value;
    target.finalShutdownTriggered_value = finalShutdownTriggered_value;

    target.getCompressorStatus_value = getCompressorStatus_value;
    target.getCompressorInterlockStatus_value = getCompressorInterlockStatus_value;
    target.getCompressorCableStatus_value = getCompressorCableStatus_value;
    return true;
}

void FETIMImpl::appendThermalLog(std::string &target) const {
    target += "\t" + to_string(internalTemperature1_value, std::fixed, 2);
    target += "\t" + to_string(internalTemperature2_value, std::fixed, 2);
    target += "\t" + to_string(internalTemperature3_value, std::fixed, 2);
    target += "\t" + to_string(internalTemperature4_value, std::fixed, 2);
    target += "\t" + to_string(internalTemperature5_value, std::fixed, 2);
    target += "\t" + to_string(internalTemperatureOOR_value ? 1 : 0);

    target += "\t" + to_string(externalTemperature1_value, std::fixed, 2);
    target += "\t" + to_string(externalTemperature2_value, std::fixed, 2);
    target += "\t" + to_string(externalTemperatureOOR1_value ? 1 : 0);
    target += "\t" + to_string(externalTemperatureOOR2_value ? 1 : 0);

    target += "\t" + to_string(getAirflowSensor1_value, std::fixed, 2);
    target += "\t" + to_string(getAirflowSensor2_value, std::fixed, 2);
    target += "\t" + to_string(airflowSensorOOR_value ? 1 : 0);

    target += "\t" + to_string(heliumBufferPressure_value, std::fixed, 2);
    target += "\t" + to_string(heliumBufferPressureOOR_value ? 1 : 0);

    target += "\t" + to_string(sensorSingleFailed_value ? 1 : 0);
    target += "\t" + to_string(sensorMultiFailed_value ? 1 : 0);
    target += "\t" + to_string(glitchValue_value, std::fixed, 2);
    target += "\t" + to_string(glitchCounterTriggered_value ? 1 : 0);

    target += "\t" + to_string(delayShutdownTriggered_value ? 1 : 0);
    target += "\t" + to_string(finalShutdownTriggered_value ? 1 : 0);

    target += "\t" + to_string(getCompressorStatus_value ? 1 : 0);
    target += "\t" + to_string(getCompressorInterlockStatus_value ? 1 : 0);
    target += "\t" + to_string(getCompressorCableStatus_value ? 1 : 0);

}

void FETIMImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tint_temp1\tint_temp2\tint_temp3\tint_temp4\tint_temp5\tint_temp_OOR"
              "\text_temp1\text_temp2\text_temp1_OOR\text_temp2_OOR"
              "\tairflow1\tairflow2\tairflow_OOR\tHE2_pres\tHE2_pres_OOR"
              "\tsingle_failed\tmulti_failed\tglitch_value\tglitch_triggered"
              "\tdelay_trig\tshutdown_trig"
              "\tcomp_FE_status\tcomp_interlock_status\tcomp_cable_status";
}

