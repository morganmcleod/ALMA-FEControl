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

#include "PowerModulesContainer.h"
#include "logger.h"
#include <vector>
using namespace std;

class PowerModulesContainer::Impl : public vector<PowerModuleImpl *> {
public:
    Impl(int size = 10)
      : vector<PowerModuleImpl*>(size)
        { fill(begin(), end(), value_type(NULL)); }

    ~Impl()
      { reset(); }

    void whack(int port) {
        reference item = operator[](port - 1);
        item = NULL;
    }

    void reset() {
        for (iterator it = begin(); it != end(); ++it)
            (*it) = NULL;
    }
    
    reference use(int port) {
        return operator[](port - 1);
    }
    
    const_reference get(int port) const {
        return operator[](port - 1);
    }
};


PowerModulesContainer::PowerModulesContainer()
  : modules_m(new Impl(10)), 
    numEnabled_m(0)
    {}

PowerModulesContainer::~PowerModulesContainer() {
    reset();
    delete modules_m; 
}

bool PowerModulesContainer::getMonitorPowerModule(int port, PowerModuleImpl::PowerModule_t &target) const {
    if (!limitsCheck(port))
        return false;
    const PowerModuleImpl *mod = modules_m -> get(port);
    if (mod)
        return mod -> getMonitorPowerModule(target);
    else
        return false;
}

void PowerModulesContainer::reset() {
    modules_m -> reset();
    numEnabled_m = 0;
}

bool PowerModulesContainer::addPowerModule(PowerModuleImpl &mod) {
    int port = mod.getPort();
    if (port < 1 || port > 10) {
        LOG(LM_INFO) << "FrontEndImpl: Power Module port " << port << " out of range." << endl;
        return false;
    }
    modules_m -> use(port) = &mod;
    return true;
}

bool PowerModulesContainer::deletePowerModule(int port) {
    if (!limitsCheck(port))
        return false;
    clearEnable(port);
    modules_m -> whack(port);
    return true;
}

void PowerModulesContainer::startMonitor() {
    PowerModuleImpl *mod;
    for (int port = 1; port <= 10; ++port) {
        mod = modules_m -> use(port);
        if (mod)
            mod -> startMonitor();
    }      
}

void PowerModulesContainer::stopMonitor() {
    PowerModuleImpl *mod;
    for (int port = 1; port <= 10; ++port) {
        mod = modules_m -> use(port);
        if (mod)
            mod -> stopMonitor();
    }
}

bool PowerModulesContainer::setEnable(int port) {
    if (!limitsCheck(port))
        return false;
    PowerModuleImpl *mod = modules_m -> use(port);
    if (mod && mod -> getEnable())
        return true;
    if (mod -> setEnable()) {
        ++numEnabled_m;
        return true;
    }
    return false;
}

bool PowerModulesContainer::clearEnable(int port) {
    if (!limitsCheck(port))
        return false;
    PowerModuleImpl *mod = modules_m -> use(port);
    if (mod && mod -> getEnable()) {
        mod -> clearEnable();
        --numEnabled_m;
    }
    return true;
}

bool PowerModulesContainer::getEnable(int port) const {
    if (!limitsCheck(port))
        return false;
    const PowerModuleImpl *mod = modules_m -> get(port);
    return (mod && mod -> getEnable());
}

bool PowerModulesContainer::queryEnable(int port) {
    if (!limitsCheck(port))
        return false;
    PowerModuleImpl *mod = modules_m -> use(port);
    return (mod && mod -> enableModule());
}

bool PowerModulesContainer::setStandby2(int port, bool val) {
    if (!limitsCheck(port))
        return false;
    PowerModuleImpl *mod = modules_m -> use(port);
    mod -> standby2Module(val);
    return true;
}

bool PowerModulesContainer::limitsCheck(int port) const {
    if (port < 1 || port > 10)
        return false;
    const PowerModuleImpl *mod = modules_m -> get(port);
    return (mod && mod -> getPort() > 0);
}

