#ifndef POWERMODULESCONTAINER_H_
#define POWERMODULESCONTAINER_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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

#include "PowerModuleImpl.h"

class PowerModulesContainer {
public:    
    PowerModulesContainer();
    ~PowerModulesContainer();

// Functions to create and manage the enabled/standby/observing status of the cartridges:    
    void reset();
    bool addPowerModule(PowerModuleImpl &module);
    bool deletePowerModule(int port);

// State control:
    void startMonitor();
    void stopMonitor();
    
    bool setEnable(int port);
    bool clearEnable(int port);    
    bool getEnable(int port) const;
    
    bool queryEnable(int port);
    ///< force a query of the enabled state of the underlying hardware.

    bool setStandby2(int port, bool val);

// Get monitor data:
    bool getMonitorPowerModule(int port, PowerModuleImpl::PowerModule_t &target) const;
    
// Utility functions:
    bool limitsCheck(int port) const;

private:
    class Impl;
    Impl *modules_m;
    int numEnabled_m;
};

#endif /*POWERMODULESCONTAINER_H_*/
