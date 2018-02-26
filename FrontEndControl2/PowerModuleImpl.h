#ifndef POWERMODULEIMPL_H_
#define POWERMODULEIMPL_H_
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

#include "FEBASE/PowerModuleImplBase.h"

/// implementation class for the Power Distribution module

class PowerModuleImpl : public PowerModuleImplBase {
public:
    PowerModuleImpl(unsigned long channel, 
                    unsigned long nodeAddress,
                    const std::string &name,
                    int port);
    virtual ~PowerModuleImpl()
      {}

    bool getEnable() const
      { return enabled_m; }

    using PowerModuleImplBase::enableModule;
    virtual void enableModule(bool val);
    virtual void standby2Module(bool val);

    bool setEnable();
    void clearEnable();

    struct PowerModule_t {
        float voltageP6V_value;
        float currentP6V_value;
        float voltageN6V_value;
        float currentN6V_value;
        float voltageP15V_value;
        float currentP15V_value;
        float voltageN15V_value;
        float currentN15V_value;
        float voltageP24V_value;
        float currentP24V_value;
        float voltageP8V_value;
        float currentP8V_value;
        bool enableModule_value;
    };
    bool getMonitorPowerModule(PowerModule_t &target) const;
    
private:
    PowerModuleImpl();
    PowerModuleImpl(const PowerModuleImpl &other);
    PowerModuleImpl &operator =(const PowerModuleImpl &other);
    
    bool enabled_m;
};
    
#endif /*POWERMODULEIMPL_H_*/
