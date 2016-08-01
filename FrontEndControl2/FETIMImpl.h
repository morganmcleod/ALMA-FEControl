#ifndef FETIMIMPL_H_
#define FETIMIMPL_H_
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

#include "FEBASE/FETIMImplBase.h"
#include "OPTIMIZE/ThermalLoggable.h"

/// implementation class for the FETIM

class FETIMImpl : public FETIMImplBase, public ThermalLoggable {
public:
    FETIMImpl(unsigned long channel,
              unsigned long nodeAddress,
              const std::string &ESN = "");
    virtual ~FETIMImpl()
      {}

    // Structure for monitoring all FETIM parameters:
    struct FETIM_t {
        float internalTemperature1_value;
        float internalTemperature2_value;
        float internalTemperature3_value;
        float internalTemperature4_value;
        float internalTemperature5_value;
        bool internalTemperatureOOR_value;
        float externalTemperature1_value;
        float externalTemperature2_value;
        bool externalTemperatureOOR1_value;
        bool externalTemperatureOOR2_value;
        float getAirflowSensor1_value;
        float getAirflowSensor2_value;
        bool airflowSensorOOR_value;
        float heliumBufferPressure_value;
        bool heliumBufferPressureOOR_value;
        bool sensorSingleFailed_value;
        bool sensorMultiFailed_value;
        float glitchValue_value;
        bool glitchCounterTriggered_value;
        bool delayShutdownTriggered_value;
        bool finalShutdownTriggered_value;
        bool getCompressorStatus_value;
        bool getCompressorInterlockStatus_value;
        bool getCompressorCableStatus_value;
    };
    bool getMonitorFETIM(FETIM_t &target);

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string

    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string

private:
};


#endif /* FETIMIMPL_H_ */
