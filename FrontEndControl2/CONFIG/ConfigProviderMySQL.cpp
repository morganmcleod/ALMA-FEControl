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
*/

/// \file
/// \brief definition of class FEConfig::ConfigProviderMySQL. 

#include "ConfigProviderMySQL.h"
#include "FrontEndDatabase.h"
#include "FrontEndConfig.h"
using namespace std;

namespace FEConfig {

ConfigProviderMySQL::ConfigProviderMySQL()
  : dbObject_mp(new FrontEndDatabase())
    {}

ConfigProviderMySQL::ConfigProviderMySQL(const std::string &host,
                                         const std::string &user,
                                         const std::string &password,
                                         const std::string &dbName)
  : dbObject_mp(new FrontEndDatabase(host, user, password, dbName))
    {}

ConfigProviderMySQL::~ConfigProviderMySQL() {
    delete dbObject_mp;
}

bool ConfigProviderMySQL::exists(unsigned keyFacility, unsigned configId) const {
    if (!dbObject_mp -> isConnected())
        return false;
    if (!keyFacility || !configId)
        return false;
    return dbObject_mp -> isValidConfigId(FEICDataBase::ID_T(keyFacility, configId));
}

bool ConfigProviderMySQL::getConfiguration(unsigned keyFacility, unsigned configId, Configuration::Record &target) const {
    target.reset();
    
    if (!dbObject_mp -> isConnected())
        return false;
    if (!keyFacility || !configId)
        return false;
    return dbObject_mp -> getConfigurationRecord(FEICDataBase::ID_T(keyFacility, configId), target);
}

bool ConfigProviderMySQL::getFrontEndConfig(unsigned keyFacility, unsigned keyFrontEnd, FrontEndConfig &target) const {
    target.reset(keyFacility, keyFrontEnd);
    
    if (!dbObject_mp -> isConnected())
        return false;
    if (!keyFacility || !keyFrontEnd)
        return false;


    // lookup the SN and ESN from the Front_Ends table:
    FEICDataBase::ID_T frontEndId(keyFacility, keyFrontEnd);

    if (!dbObject_mp -> getFrontEndRecord(frontEndId, &target.SN_m, &target.ESN_m))
        return false;

    // lookup the latest configuration ID from the FE_Config table:
    FEICDataBase::ID_T configId = dbObject_mp -> getConfigId(frontEndId);

    FEICDataBase::ID_T componentId;
    string SN;

    if (dbObject_mp -> getConfigCryostat(configId, componentId, SN)) {
        // get the cryostat configuration:
        CryostatConfig cryoConfig;
        if (getCryostatConfig(componentId.keyFacility, componentId.keyId, cryoConfig))
            target.setCryostatConfig(cryoConfig);
    }

    if (dbObject_mp -> getConfigLPR(configId, componentId, SN)) {
        // get the LPR configuration:
        LPRConfig lprConfig;
        if (getLPRConfig(componentId.keyFacility, componentId.keyId, lprConfig))
            target.setLPRConfig(lprConfig);
    }

    // get the FETIM component ID:
    if (dbObject_mp -> getConfigFETIM(configId, componentId, SN)) {
        target.fkFETIM_m = componentId.keyId;
    }

    // get the IF Switch Assembly or the IF Switch Subrack component ID:
    if (dbObject_mp -> getConfigIFSwitchAssy(configId, componentId, SN)) {
        target.fkIFSwitch_m = componentId.keyId;
    }

    // get the CPDS Assembly or the CPDS Subrack component ID:
    if (dbObject_mp -> getConfigCPDSAssy(configId, componentId, SN)) {
        target.fkPowerDist_m = componentId.keyId;
    }

    // loop on band number from 1 to 10:
    CartAssemblyConfig caConfig;
    bool saveConfig;

    for (int band = 1; band <= 10; ++band) {
        caConfig.reset();
        saveConfig = false;

        // save the band number as both the port and band for the cartAssembly configuration:
        caConfig.Id_m = CartAssemblyID(band, band);

        // load the CCA component for the band:
        if (dbObject_mp -> getConfigCCA(configId, band, componentId, SN)) {
            // if found, save its component ID in caConfig.Id_m:
            caConfig.Id_m.CCAFacility_m = componentId.keyFacility;
            caConfig.Id_m.CCAId_m = componentId.keyId;

            // also save it as the ID portion of caConfig.coldCart_m:
            caConfig.coldCart_m.reset(caConfig.Id_m.CCAFacility_m, caConfig.Id_m.CCAId_m);

            // if that looks valid, load the associated coldCart configuration:
            if (caConfig.Id_m.CCAValid()) {
                if (getColdCartConfig(caConfig.Id_m.CCAFacility_m, caConfig.Id_m.CCAId_m, caConfig.coldCart_m))
                    saveConfig = true;  // record that we got at least one valid component for the band.
            }
        }

        // load the WCA component for the band:
        if (dbObject_mp -> getConfigWCA(configId, band, componentId, SN)) {
            // if found, save its component ID in caConfig.Id_m:
            caConfig.Id_m.WCAFacility_m = componentId.keyFacility;
            caConfig.Id_m.WCAId_m = componentId.keyId;

            // also save it as the ID portion of caConfig.WCA_m:
            caConfig.WCA_m.reset(caConfig.Id_m.WCAFacility_m, caConfig.Id_m.WCAId_m);

            if (caConfig.Id_m.WCAValid()) {
                // if that looks valid, load the associated WCA configuration:
                if (getWCAConfig(caConfig.Id_m.WCAFacility_m, caConfig.Id_m.WCAId_m, caConfig.WCA_m))
                    saveConfig = true;  // record that we got at least one valid component for the band.
            }
        }

        // if valid configuration was found for this band, apply it to the target FrontEndConfig:
        if (saveConfig)
            target.setCartridgeConfig(band, caConfig);
    }
    return true;
}

bool ConfigProviderMySQL::getCryostatConfig(unsigned keyFacility, unsigned keyCryostat, CryostatConfig &target) const {
    target.reset(keyFacility, keyCryostat);

    if (!dbObject_mp -> isConnected())
        return false;   
    if (!keyFacility || !keyCryostat)
        return false;

    FEICDataBase::FEComponent comp;
    if (!dbObject_mp -> getComponentRecord(FEICDataBase::ID_T(keyFacility, keyCryostat), comp))
        return false;

    target.SN_m = comp.SN_m;
    target.ESN_m = comp.ESN1_m;
    return true;
}

bool ConfigProviderMySQL::getLPRConfig(unsigned keyFacility, unsigned keyLPR, LPRConfig &target) const {
    target.reset(keyFacility, keyLPR);

    if (!dbObject_mp -> isConnected())
        return false;   
    if (!keyFacility || !keyLPR)
        return false;

    FEICDataBase::FEComponent comp;
    if (!dbObject_mp -> getComponentRecord(FEICDataBase::ID_T(keyFacility, keyLPR), comp))
        return false;

    target.SN_m = comp.SN_m;
    target.ESN_m = comp.ESN1_m;
    return true;
}

bool ConfigProviderMySQL::getCartAssemblyConfig(CartAssemblyID CAId, CartAssemblyConfig &target) const {

    // TODO:  this function is duplicated here and in ConfigProviderIniFile.  Move to base or refactor away somehow?

    target.reset();

    if (!CAId.isValid())
        return false;

    target.Id_m = CAId;

    bool found;
    found = getColdCartConfig(CAId.CCAFacility_m, CAId.CCAId_m, target.coldCart_m);
    if (!found)
        target.coldCart_m.reset();

    found = getWCAConfig(CAId.WCAFacility_m, CAId.WCAId_m, target.WCA_m);
    if (!found)
        target.WCA_m.reset();

    return found;
}

bool ConfigProviderMySQL::getColdCartConfig(unsigned keyFacility, unsigned keyColdCart, ColdCartConfig &target) const {
    static const string context("ConfigProviderMySQL::getColdCartConfig");

    target.reset(keyFacility, keyColdCart);
    
    if (!dbObject_mp -> isConnected())
        return false;  
    if (!keyFacility || !keyColdCart)
        return false;

    FEICDataBase::ID_T componentId(keyFacility, keyColdCart);

    FEICDataBase::FEComponent comp;
    if (!dbObject_mp -> getComponentRecord(componentId, comp))
        return false;

    target.band_m = comp.band_m;
    target.SN_m = comp.SN_m;
    target.ESN_m = comp.ESN1_m;
    target.description_m = comp.desc_m;

    bool success = true;

    if (!dbObject_mp -> getMixerParams(componentId, target.mixerParams_m, target.magnetParams_m)) {
        LOG(LM_ERROR) << context << ": failed getMixerParams for componentId=" << componentId << endl;
        success = false;
    }

    if (!dbObject_mp -> getPreampParams(componentId, target)) {
        LOG(LM_ERROR) << context << ": failed getPreampParams for componentId=" << componentId << endl;
        success = false;
    }

    //TODO:  get tempsensor params.

    return success;
}

bool ConfigProviderMySQL::getWCAConfig(unsigned keyFacility, unsigned keyWCA, WCAConfig &target) const {
    static const string context("ConfigProviderMySQL::getWCAConfig");

    target.reset(keyFacility, keyWCA);
    
    if (!dbObject_mp -> isConnected())
        return false;   
    if (!keyFacility || !keyWCA)
        return false;

    FEICDataBase::ID_T componentId(keyFacility, keyWCA);

    FEICDataBase::FEComponent comp;
    if (!dbObject_mp -> getComponentRecord(componentId, comp))
        return false;

    target.band_m = comp.band_m;
    target.SN_m = comp.SN_m;
    target.ESN_m = comp.ESN1_m;
    target.description_m = comp.desc_m;

    // TODO since 2.7.0:  load LoopBW option!

    bool success = true;

    if (!dbObject_mp -> getWCAYIGLimits(componentId, target.FLOYIG_m, target.FHIYIG_m)) {
        LOG(LM_ERROR) << context << ": failed getWCAYIGLimits for componentId=" << componentId << endl;
        success = false;
    }

    if (!dbObject_mp -> getWCALOParams(componentId, target.PAParams_m)) {
        LOG(LM_ERROR) << context << ": failed getWCALOParams for componentId=" << componentId << endl;
        success = false;
    }
    return success;
}

}; // namespace FEConfig


