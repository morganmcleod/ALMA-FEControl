#ifndef CARTRIDGESCONTAINER_H_
#define CARTRIDGESCONTAINER_H_
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

#include "CartAssembly.h"
#include "OPTIMIZE/ThermalLoggable.h"
#include <string>

//TODO: make the array private and internal.  Make it delete CartAssembly objects.

class CartridgesContainer : public ThermalLoggable{
public:    
    CartridgesContainer();
    ~CartridgesContainer();

// Functions to create and delete cartridges:    
    void reset();
    bool addCartridge(int port, WCAImpl &WCA, ColdCartImpl &coldCart);
    bool addCartridge(int port, WCAImpl &WCA);
    bool addCartridge(int port, ColdCartImpl &coldCart);
    bool deleteCartridge(int port);

// query subsystem status of powermodules and cartridges:
    void queryCartridgeState();
    void measureSISVoltageError(int port);
    
// Access to the contained CartAssembly, ColdCart, and WCA objects:
    bool existsCartAssembly(int port) const;
    bool existsColdCart(int port) const;
    bool existsWCA(int port) const;
    
    CartAssembly *getCartAssembly(int port);
    const CartAssembly *getCartAssembly(int port) const;
    const ColdCartImpl *getColdCart(int port) const;
    const WCAImpl *getWCA(int port) const;

// Monitoring state control:
    void startMonitor();
    void stopMonitor();
    bool pauseMonitor(int port, bool pauseWCA, bool pauseCC);
    
// Functions to manage the enabled/standby/observing status of the cartridges:    
    bool setObserving(int port);
    void clearObserving();
    int getObserving() const 
      { return observingPort_m; }
    bool setEnable(int port);
    bool clearEnable(int port);    
    bool getEnable(int port) const;

// Debug/utility functions:
    void print(const std::string &callerName, int port = -1) const;

    void appendThermalLog(std::string &target) const;

    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string

private:
    class Impl;
    Impl *carts_mp;
    int numEnabled_m;
    int observingPort_m;
};



#endif /*CARTRIDGESCONTAINER_H_*/
