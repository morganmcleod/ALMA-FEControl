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

#include "CartridgesContainer.h"
#include "logger.h"
#include <vector>
using namespace std;

class CartridgesContainer::Impl : public vector<CartAssembly*> {
public:
    Impl(int size = 10)
      : vector<CartAssembly*>(size)
        { fill(begin(), end(), value_type(NULL)); }
    
    ~Impl()
      { reset(); }

    void whack(int port) {
        if (port >= 1 && port <= 10) {
            reference item = operator[](port - 1);
            delete item;
            item = NULL;
        }
    }

    void reset() {
        for (iterator it = begin(); it != end(); ++it) {
            delete (*it);
            (*it) = NULL;
        }
    }
    
    reference use(int port) {
        if (port < 1 || port > 10)
            return nullReference;
        else        
            return operator[](port - 1);
    }
    
    const_reference get(int port) const {
        if (port < 1 || port > 10)
            return nullReference;
        else        
            return operator[](port - 1);
    }
    
    static CartAssembly *nullReference;
};

CartAssembly *CartridgesContainer::Impl::nullReference(NULL);

//-----------------------------------------------------------------------------

CartridgesContainer::CartridgesContainer()
  : carts_mp(new Impl(10)),
    numEnabled_m(0),
    observingPort_m(-1)
    {}  

CartridgesContainer::~CartridgesContainer() {
    reset(); 
    delete carts_mp;
}

void CartridgesContainer::reset() {
    carts_mp -> reset();
    numEnabled_m = 0;
    observingPort_m = -1;
    print("CartridgesContainer::reset");
}

bool CartridgesContainer::addCartridge(int port, WCAImpl &WCA, ColdCartImpl &coldCart) {
    int coldCartBand = coldCart.getBand();
    int WCABand = WCA.getBand();
    if (WCABand != coldCartBand) {
        LOG(LM_INFO) << "CartridgesContainer::addCartridge: Cannot pair WCA band " << WCABand
            << " with ColdCart band " << coldCartBand << endl;
        return false;
    }
    if (port < 1 || port > 10) {
        LOG(LM_INFO) << "CartridgesContainer::addCartridge: Cartridge port " << port << " out of range." << endl;
        return false;
    }
    char name[20];
    sprintf(name, "CartAssembly%d", port);
    carts_mp -> whack(port);
    carts_mp -> use(port) = new CartAssembly(name, &WCA, &coldCart);
    print("CartridgesContainer::addCartridge", port);
    return true;
}

bool CartridgesContainer::addCartridge(int port, WCAImpl &WCA) {
    if (port < 1 || port > 10) {
        LOG(LM_INFO) << "CartridgesContainer::addCartridge: Cartridge port " << port << " out of range." << endl;
        return false;
    }
    char name[20];
    sprintf(name, "CartAssembly%d", port);
    carts_mp -> whack(port);
    carts_mp -> use(port) = new CartAssembly(name, &WCA, NULL);
    print("CartridgesContainer::addCartridge", port);
    return true;
}

bool CartridgesContainer::addCartridge(int port, ColdCartImpl &coldCart) {
    if (port < 1 || port > 10) {
        LOG(LM_INFO) << "CartridgesContainer::addCartridge: Cartridge port " << port << " out of range." << endl;
        return false;
    }
    char name[20];
    sprintf(name, "CartAssembly%d", port);
    carts_mp -> whack(port);
    carts_mp -> use(port) = new CartAssembly(name, NULL, &coldCart);
    print("CartridgesContainer::addCartridge", port);
    return true;
}

bool CartridgesContainer::deleteCartridge(int port) {
    if (!existsCartAssembly(port))
        return false;
    clearEnable(port);
    carts_mp -> whack(port);
    return true;
}

void CartridgesContainer::queryCartridgeState() {
    CartAssembly *ca;
    for (int port = 1; port <= 10; ++port) {
        ca = carts_mp -> use(port);
        if (ca)
            ca -> queryCartridgeState();
    }
}

void CartridgesContainer::measureSISVoltageError(int port, bool measureOnMainThread) {
    CartAssembly *ca = carts_mp -> use(port);
    if (ca && ca -> getEnable())
        ca -> measureSISVoltageError(measureOnMainThread);
}

bool CartridgesContainer::existsCartAssembly(int port) const {
    if (port < 1 || port > 10)
        return false;
    return (carts_mp -> get(port) != NULL);
}

bool CartridgesContainer::existsColdCart(int port) const {
    const CartAssembly *ca = carts_mp -> get(port);
    if (ca)
        return ca -> existsColdCart();
    else
        return false;       
}

bool CartridgesContainer::existsWCA(int port) const {
    const CartAssembly *ca = carts_mp -> get(port);
    if (ca)
        return ca -> existsWCA();
    else
        return false;       
}

CartAssembly *CartridgesContainer::getCartAssembly(int port) {
    return carts_mp -> use(port);
}

const CartAssembly *CartridgesContainer::getCartAssembly(int port) const {
    return carts_mp -> get(port);
}

const ColdCartImpl *CartridgesContainer::getColdCart(int port) const {
    const CartAssembly *ca = carts_mp -> get(port);
    if (ca)
        return ca -> getColdCart();
    else
        return NULL;
}

const WCAImpl *CartridgesContainer::getWCA(int port) const {
    const CartAssembly *ca = carts_mp -> get(port);
    if (ca)
        return ca -> getWCA();
    else
        return NULL;
}

void CartridgesContainer::startMonitor() {
    CartAssembly *ca;
    for (int port = 1; port <= 10; ++port) {
        ca = getCartAssembly(port);
        if (ca)
            ca -> startMonitor();
    }      
}

void CartridgesContainer::stopMonitor() {
    CartAssembly *ca;
    for (int port = 1; port <= 10; ++port) {
        ca = carts_mp -> use(port);
        if (ca)
            ca -> stopMonitor();
    }
}

bool CartridgesContainer::pauseMonitor(int port, bool pauseWCA, bool pauseCC) {
    CartAssembly *ca = carts_mp -> use(port);
    if (ca) {
        ca -> pauseMonitor(pauseWCA, pauseCC);
        return true;
    }
    return false;
}

bool CartridgesContainer::setObserving(int port) {
    if (!existsCartAssembly(port))
        return false;
    if (!getEnable(port))
        return false;
    clearObserving();
    observingPort_m = port;
    print("CartridgesContainer::setObserving", port);
    carts_mp -> use(observingPort_m) -> setObserving();
    return true;
}

void CartridgesContainer::clearObserving() {
    if (observingPort_m > 0 && existsCartAssembly(observingPort_m)) {
        print("CartridgesContainer::clearObserving", observingPort_m);
        carts_mp -> use(observingPort_m) -> clearObserving();
    }
    observingPort_m = -1;
}

bool CartridgesContainer::setEnable(int port) {
    CartAssembly *ca = carts_mp -> use(port);
    if (!ca)
        return false;
    bool wasEnabled = ca -> getEnable();
    ca -> setEnable();
    if (!wasEnabled)
        ++numEnabled_m;
    print("CartridgesContainer::setEnable", port);
    return true;    
}

bool CartridgesContainer::clearEnable(int port) {
    CartAssembly *ca = carts_mp -> use(port);
    if (!ca)
        return false;
    bool wasEnabled = ca -> getEnable();
    ca -> clearEnable();
    if (port == observingPort_m)
        clearObserving();   
    if (wasEnabled)
        --numEnabled_m;
    print("CartridgesContainer::clearEnable", port);
    return true;
}

bool CartridgesContainer::getEnable(int port) const {
    const CartAssembly *ca = carts_mp -> get(port);
    if (ca)
        return ca -> getEnable();
    else
        return false;
}

void CartridgesContainer::print(const string &callerName, int port) const {
    char buf[20];
    if (!existsCartAssembly(port))
        sprintf(buf, "()");
    else
        sprintf(buf, "(%d)", port);
    string text(callerName);
    text += buf;
    int len = 20 - text.length();
    if (len < 1)
        len = 1;
    text.append(len, ' ');
    
    sprintf(buf, " (en=%d ob=%02d) ", numEnabled_m, observingPort_m);
    text += buf;
    
    string portTxt;
    const CartAssembly *ca = carts_mp -> get(port);
    if (ca)
        text += ca -> asString(portTxt);
    LOG(LM_INFO) << text << endl;
}

void CartridgesContainer::appendThermalLog(std::string &target) const {
    const CartAssembly *ca;
    for (int port = 1; port <= 10; ++port) {
        ca = carts_mp -> get(port);
        if (ca)
            ca -> appendThermalLog(target);
    }
}

void CartridgesContainer::appendThermalLogHeader(std::string &target) const {
    CartAssembly::appendThermalLogHeaderImpl(target);
    target += "\t...";
}


