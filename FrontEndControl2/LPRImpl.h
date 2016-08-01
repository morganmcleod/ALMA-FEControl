#ifndef LPRIMPL_H_
#define LPRIMPL_H_
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

#include "FEBASE/LPRImplBase.h"
#include "OPTIMIZE/ThermalLoggable.h"

/// implementation class for the IF Switch module

class LPRImpl : public LPRImplBase, public ThermalLoggable {
public:
    LPRImpl(unsigned long channel, 
            unsigned long nodeAddress, 
            const std::string &ESN = "");
    virtual ~LPRImpl()
      {}

// Functions to manage the observing status of the cartridge bands:    
    bool setObservingBand(int band);
    int getObservingBand() const
      { return observingBand_m; }

// Structure for monitoring all LPR parameters:
    struct LPR_t {
        float LPRTemperature0_value;
        float LPRTemperature1_value;
        unsigned char opticalSwitchPort_value;
        unsigned char opticalSwitchShutter_value;
        unsigned char opticalSwitchState_value;
        unsigned char opticalSwitchBusy_value;
        float EDFALaserPumpTemperature_value;
        float EDFALaserDriveCurrent_value;
        float EDFALaserPhotoDetectCurrent_value;
        float EDFAPhotoDetectCurrent_value;
        float EDFAPhotoDetectPower_value;
        float EDFAModulationInput_value;
    };
    bool getMonitorLPR(LPR_t &target);

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string

    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string
    
private:
    int observingBand_m;
};

#endif /*LPRIMPL_H_*/
