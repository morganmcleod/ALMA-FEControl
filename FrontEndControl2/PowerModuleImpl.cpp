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

/************************************************************************
 * implementation class for the Power Distribution Module
 *----------------------------------------------------------------------
 */
 
#include "PowerModuleImpl.h"
#include "logger.h"
#include <string>
using namespace std;


PowerModuleImpl::PowerModuleImpl(unsigned long channel, 
                                 unsigned long nodeAddress,
                                 const std::string &name,
                                 int port)
  : PowerModuleImplBase(name, port),
    enable_m(false)
{ 
    PowerModuleImplBase::initialize(channel, nodeAddress);
    enable_m = PowerModuleImplBase::enableModule();
}

void PowerModuleImpl::setEnable() {
    LOG(LM_INFO) << "PowerModuleImpl::setEnable port=" << port_m << endl;
    enable_m = true;
    PowerModuleImplBase::enableModule(true);
}
        
void PowerModuleImpl::clearEnable() {
    LOG(LM_INFO) << "PowerModuleImpl::clearEnable port=" << port_m << endl;
    enable_m = false;
    PowerModuleImplBase::enableModule(false);
}

bool PowerModuleImpl::getMonitorPowerModule(PowerModule_t &target) const {
    memset(&target, 0, sizeof(target));
    target.voltageP6V_value = voltageP6V_value; 
    target.currentP6V_value = currentP6V_value;
    target.voltageN6V_value = voltageN6V_value;
    target.currentN6V_value = currentN6V_value;
    target.voltageP15V_value = voltageP15V_value;
    target.currentP15V_value = currentP15V_value;
    target.voltageN15V_value = voltageN15V_value;
    target.currentN15V_value = currentN15V_value;
    target.voltageP24V_value = voltageP24V_value;
    target.currentP24V_value = currentP24V_value;
    target.voltageP8V_value = voltageP8V_value;
    target.currentP8V_value = currentP8V_value;
    target.enableModule_value = enableModule_value;
    return true;
}

