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

#include "ConfigManager.h"
#include "Configuration.h"
#include "FrontEndImpl.h"
#include "SignalSourceImpl.h"
#include "logger.h"
using namespace std;

namespace FEConfig {
    
bool ConfigManager::configure(const Configuration &config, FrontEndImpl &target) const {
    
    const FrontEndConfig *feConfig = config.getFrontEndConfig();
    if (feConfig) {
        target.setFrontEndSN(feConfig -> SN_m);

        const CryostatConfig *cryoConfig = feConfig -> getCryostatConfig();
        if (cryoConfig) {
            configureCryostat(*cryoConfig, target);
        }

        unsigned fetimConfig = feConfig -> getFETIMConfig();
        if (fetimConfig) {
            LOG(LM_INFO) << "ConfigManager: Configured FETIM " << fetimConfig << "." <<  endl;
        }

        unsigned ifSwitchConfig = feConfig -> getIFSwitchConfig();
        if (ifSwitchConfig) {
            LOG(LM_INFO) << "ConfigManager: Configured IF switch " << ifSwitchConfig << "." <<  endl;
        }
        
        const LPRConfig *lprConfig = feConfig -> getLPRConfig();
        if (lprConfig) {
            configureLPR(*lprConfig, target);
        }

        unsigned powerDistConfig = feConfig -> getPowerDistConfig();
        if (powerDistConfig) {
            LOG(LM_INFO) << "ConfigManager: Configured CPDS " << ifSwitchConfig << "." <<  endl;
        }

        for (int pos = 1; pos <= 10; ++pos) {
            const CartAssemblyConfig *cartAssy = feConfig -> getCartridgeConfig(pos);
            if (cartAssy)
                configureCartAssembly(*cartAssy, target);
        }
        LOG(LM_INFO) << "ConfigManager: Configured Front End (" << feConfig -> keyFrontEnd_m << ")." << endl;
        return true;
    }

    const CartAssemblyConfig *cartAssy = config.getCartAssemblyConfig();
    if (cartAssy) {
        if (configureCartAssembly(*cartAssy, target)) {
            LOG(LM_INFO) << "ConfigManager: Configured Cartridge Assembly ( port=" << cartAssy -> Id_m.port_m << ", band=" << cartAssy -> Id_m.band_m << " )." << endl;
            return true;
        }
    }
    return false;
}
    

bool ConfigManager::configure(unsigned configId, FrontEndImpl &target) const {

    Configuration config(configId);
    if (!config.load(provider_m))
        return false;

    LOG(LM_INFO) << "ConfigManager: Loaded configuration (" << configId << ") '" << config.getDescription() << "'." << endl;
    return configure(config, target);
}

bool ConfigManager::configure(const Configuration &config, SignalSourceImpl &target) const {
// TODO: refactor configurable things.  SignalSource should be similar enough to FrontEndImpl that these duplicate functions aren't needed.
    const CartAssemblyConfig *cartAssy = config.getCartAssemblyConfig();
    if (cartAssy) {
        if (configureCartAssembly(*cartAssy, target)) {
            LOG(LM_INFO) << "ConfigManager: Configured Cartridge Assembly ( port=" << cartAssy -> Id_m.port_m << ", band=" << cartAssy -> Id_m.band_m << " )." << endl;
            return true;
        }
    }
    return false;
}

bool ConfigManager::configure(unsigned configId, SignalSourceImpl &target) const {
// TODO: refactor configurable things.  SignalSource should be similar enough to FrontEndImpl that these duplicate functions aren't needed.
    Configuration config(configId);
    if (!config.load(provider_m))
        return false;

    LOG(LM_INFO) << "ConfigManager: Loaded configuration (" << configId << ") '" << config.getDescription() << "'." << endl;
    return configure(config, target);
}    
    
bool ConfigManager::configureCryostat(const CryostatConfig &cryo, FrontEndImpl &target) const {
    // TODO: insert cryostat config into target.
    LOG(LM_INFO) << "ConfigManager: Configured Cryostat (" << cryo.keyCryostat_m << ")." << endl;
    return true;
}

bool ConfigManager::configureLPR(const LPRConfig &lpr, FrontEndImpl &target) const {
    // TODO: insert LPR config into target.
    LOG(LM_INFO) << "ConfigManager: Configured LPR (" << lpr.keyLPR_m << ")." << endl;
    return true;
}
 
bool ConfigManager::configureCartAssembly(const CartAssemblyConfig &cartAssy, FrontEndImpl &target) const {
    bool wcaOk = configureWCA(cartAssy.Id_m.port_m, cartAssy.WCA_m, target);
    bool ccOk = configureColdCart(cartAssy.Id_m.port_m, cartAssy.coldCart_m, target);
    if (wcaOk || ccOk) {
        LOG(LM_INFO) << "ConfigManager: Configured cartridge at port=" << cartAssy.Id_m.port_m << "." << endl;
        return true;
    } else {
        LOG(LM_ERROR) << "ConfigManager ERROR: Bad configuration for cartridge at port=" << cartAssy.Id_m.port_m << "." << endl;
        return false;
    }
}

bool ConfigManager::configureCartAssembly(const CartAssemblyConfig &cartAssy, SignalSourceImpl &target) const {
    // TODO: refactor configurable things.  SignalSource should be similar enough to FrontEndImpl that these duplicate functions aren't needed.
    bool wcaOk = configureWCA(cartAssy.Id_m.port_m, cartAssy.WCA_m, target);
    if (wcaOk) {
        LOG(LM_INFO) << "ConfigManager: Configured signal source at port=" << cartAssy.Id_m.port_m << "." << endl;
        return true;
    } else {
        LOG(LM_ERROR) << "ConfigManager ERROR: Bad configuration for  signal source at port=" << cartAssy.Id_m.port_m << "." << endl;
        return false;
    }
}

bool ConfigManager::configureColdCart(unsigned port, const ColdCartConfig &ccConfig, FrontEndImpl &target) const {
    return target.setColdCartConfig(port, ccConfig);
}

bool ConfigManager::configureWCA(unsigned port, const WCAConfig &wcaConfig, FrontEndImpl &target) const {
    return target.setWCAConfig(port, wcaConfig);
}

bool ConfigManager::configureWCA(unsigned port, const WCAConfig &wcaConfig, SignalSourceImpl &target) const {
    // TODO: refactor configurable things.  SignalSource should be similar enough to FrontEndImpl that these duplicate functions aren't needed.
    return target.setWCAConfig(wcaConfig);
}

}; // namespace FEConfig;


