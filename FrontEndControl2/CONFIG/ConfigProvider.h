#ifndef CONFIGPROVIDER_H_
#define CONFIGPROVIDER_H_
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
*/

/// \file
/// \brief declaration of class FEConfig::ConfigProvider. 

#include <FrontEndAMB/ambDefs.h>
#include "Configuration.h"
#include <vector>

namespace FEConfig {
    // Forward-declare classes passed by-reference to this interface:
    class FrontEndConfig;
    class CryostatConfig;
    class CartAssemblyConfig;
    class ColdCartConfig;
    class LPRConfig;
    class WCAConfig;

    // Forward declare classes passed by-reference to descendant interfaces:
    class MixerParams;
    class MagnetParams;
    class PreampParams;
    class PowerAmpParams;

    /// A ConfigProver loads FE configuration data from a database.
    /// The abstract base class declares the interface that a real provider must implement.
    class ConfigProvider {
    public:
        ConfigProvider()
          {}
        virtual ~ConfigProvider()
          {}
    
        virtual bool exists(unsigned keyFacility, unsigned configId) const = 0;
        ///< return true if the configuration specified by (keyFacility, configId) exists in the database, false otherwise.
    
        virtual bool getConfiguration(unsigned keyFacility, unsigned configId,
                                      Configuration::Record &target) const = 0;
        ///< get the top-level configuration record specified by (keyFacility, configId).
        ///< returns false if the config does not exist or on error.

        virtual bool getAllConfigurations(unsigned keyFacility, std::vector<Configuration::Record> &target) const
          { return false; }
        ///< get all configurations for the given provider.

        virtual bool saveConfiguration(const Configuration &source)
          { return false; }
        ///< write out the entire configuration to the ini file.
        
        virtual bool getFrontEndConfig(unsigned keyFacility, unsigned keyFrontEnd, FrontEndConfig &target) const = 0;
        ///< get the FrontEndConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getCryostatConfig(unsigned keyFacility, unsigned keyCryostat, CryostatConfig &target) const = 0;
        ///< get the CryostatConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.

        virtual bool getLPRConfig(unsigned keyFacility, unsigned keyLPR, LPRConfig &target) const = 0;
        ///< get the LPRConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getCartAssemblyConfig(CartAssemblyID id, CartAssemblyConfig &target) const = 0;
        ///< load the configuration for the specified CartAssembly.
    
        virtual bool getColdCartConfig(unsigned keyFacility, unsigned keyColdCart, ColdCartConfig &target) const = 0;
        ///< get the ColdCartConfig portion of the specified config for the specified band.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getWCAConfig(unsigned keyFacility, unsigned keyWCA, WCAConfig &target) const = 0;
        ///< get the WCAConfig portion of the specified config for the specified band.
        ///< returns false if that portion is not available or on error.


    private:
        ConfigProvider(const ConfigProvider &other);
        ///< copy construct is private and forbidden.
        ConfigProvider &operator =(const ConfigProvider &other);
        ///< assignment is private and forbidden.
    };

}; // namespace FEConfig

#endif /*CONFIGPROVIDER_H_*/
