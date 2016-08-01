#ifndef IFSWITCHIMPL_H_
#define IFSWITCHIMPL_H_
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

#include "FEBASE/IFSwitchImplBase.h"
#include "OPTIMIZE/ThermalLoggable.h"

/// implementation class for the IF Switch module

class IFSwitchImpl : public IFSwitchImplBase, public ThermalLoggable {
public:
    IFSwitchImpl(unsigned long channel, 
                 unsigned long nodeAddress);
    virtual ~IFSwitchImpl()
      {}

// Functions to manage the observing status of the cartridge bands:    
    bool setObservingBand(int band);
    int getObservingBand() const
      { return observingBand_m; }

// Structure for monitoring all IF switch parameters:
    struct IFSwitch_t {
        unsigned char pol0Sb1Attenuation_value;
        unsigned char pol0Sb2Attenuation_value;
        unsigned char pol1Sb1Attenuation_value;
        unsigned char pol1Sb2Attenuation_value;
        float pol0Sb1AssemblyTemp_value;
        float pol0Sb2AssemblyTemp_value;
        float pol1Sb1AssemblyTemp_value;
        float pol1Sb2AssemblyTemp_value;
        bool pol0Sb1TempServoEnable_value;
        bool pol0Sb2TempServoEnable_value;
        bool pol1Sb1TempServoEnable_value;
        bool pol1Sb2TempServoEnable_value;
        unsigned char switchCartridge_value;
    };
    bool getMonitorIFSwitch(IFSwitch_t &target);

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string
    
    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string
    
private:
    int observingBand_m;
};

#endif /*IFSWITCHIMPL_H_*/
