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

#include "Configuration.h"
#include "logger.h"
#include "ConfigProvider.h"
#include "CartConfig.h"
#include "FrontEndConfig.h"
#include "SubsysConfig.h"
using namespace std;

namespace FEConfig {

Configuration::Configuration(unsigned configId)
  : configId_m(configId),
    FE_mp(NULL),
    cartAssy_mp(NULL)
    {}

void Configuration::reset(unsigned configId) {
    if (configId > 0)
        configId_m = configId;  
    config_m.reset();
    delete FE_mp;
    FE_mp = NULL;
    delete cartAssy_mp;
    cartAssy_mp = NULL;
}

bool Configuration::load(const ConfigProvider &source) {
    reset();

    if (!configId_m) {
    	LOG(LM_INFO) << "Configuration::load(" <<  configId_m << ") not a valid configId." << endl;
        return false;
    }
    
    if (!source.exists(configId_m)) {
    	LOG(LM_INFO) << "Configuration::load(" <<  configId_m << ") does not exist." << endl;
        return false;
    }
    
    if (!source.getConfiguration(configId_m, config_m)) {
    	LOG(LM_INFO) << "Configuration::load(" <<  configId_m << ") FAILED." << endl;
        reset();
        return false;
    }

    LOG(LM_INFO) << "Configuration::load(" <<  configId_m << ") '" << config_m.description_m << "'" << endl;

    FE_mp = new FrontEndConfig;
    if (!source.getFrontEndConfig(config_m.keyFrontEnd_m, *FE_mp)) {
        delete FE_mp;
        FE_mp = NULL;    
    } else {
        LOG(LM_INFO) << "\t" << *FE_mp;
    }

    if (!FE_mp) {
        cartAssy_mp = new CartAssemblyConfig;
        if (!source.getCartAssemblyConfig(config_m.CAId_m, *cartAssy_mp)) {
            delete cartAssy_mp;
            cartAssy_mp = NULL;
        } else {
            LOG(LM_INFO) << "\t" << *cartAssy_mp;
        }
    }
    if (!FE_mp && !cartAssy_mp) {
        LOG(LM_INFO) << "\tConfiguration is empty." << endl;
    }
    return true;
}

}; // namespace FEConfig
