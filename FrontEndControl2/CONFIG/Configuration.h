#ifndef FECONFIG_CONFIG_H_
#define FECONFIG_CONFIG_H_
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

/// \file
/// \brief declaration of class FEConfig::Configuration.
#include <FrontEndAMB/ambDefs.h>
#include "CartConfig.h"

namespace FEConfig {
    
    class ConfigProvider;
    class FrontEndConfig;
    class CartAssemblyConfig;

    /// Uses the abstract ConfigProvider interface to load a Configuration record into memory.
    /// The record contans config items for a whole FrontEnd, or for a single CartAssembly
    class Configuration {
    public:
        Configuration(unsigned configId = 0);
        ///< Construct with the primary key configId
        ///< which uniquely identify a Configuration record in the config DB.
        ///< These may be changed later with reset().
        
        ~Configuration()
          { reset(); }
    
        void reset(unsigned configId = 0);
        ///< reset all data to defaults.
        ///< Use any nonzero values provided for the key.  Otherwise retain the existing value.
    
        void getConfigID(unsigned &configId) const
          { configId = configId_m; }
        ///< get the current value of configId.

        const std::string &getDescription() const
          { return config_m.description_m; }
        ///< get the description for the currently loaded configuration.
        ///< will be empty if no configuration is loaded.           
    
        bool load(const ConfigProvider &source);
        ///< load the configuration for the current configId from the given provider.
        ///< returns true if the configuration was found and loaded.        

        const FrontEndConfig *getFrontEndConfig() const
          { return FE_mp; }
        ///< get the FrontEnd configuration item, if any.

        const CartAssemblyConfig *getCartAssemblyConfig() const
          { return cartAssy_mp; }
        ///< get the CartAssembly config item, if this configuration is for a single cartridge only.
    
        /// A structure for the elements in the top-level Configuration record.
        /// bundled together for convenient interaction with the ConfigProvider.
        struct Record {
            unsigned configId_m;        ///< the configuration ID for this record.
            unsigned keyFrontEnd_m;     ///< a foreign key into the FrontEnds table.
            CartAssemblyID CAId_m;      ///< identifying information for a CartAssembly.
            std::string description_m;  ///< the description of the configuration.
            
            Record(unsigned configId = 0,
                   unsigned keyFrontEnd = 0,
                   CartAssemblyID CAId = CartAssemblyID(),
                   const std::string &description = std::string())
              : configId_m(configId),
                keyFrontEnd_m(keyFrontEnd),
                CAId_m(CAId),
                description_m(description)
                {}

            void reset(unsigned configId = 0)
              { configId_m = configId;
                keyFrontEnd_m = 0;
                CAId_m.reset();
                description_m = ""; }

            bool isValid() const
              { return (configId_m && (keyFrontEnd_m || CAId_m.isValid())); }
        };

    private:
        Configuration(const Configuration &other);
        ///< copy construct is private and forbidden.
        Configuration &operator =(const Configuration &other);
        ///< assignment is private and forbidden.

        unsigned configId_m;                ///< uniquely identifies a Configuration.
    
        Record              config_m;       ///< the top-level configuration info currently loaded.
    
        FrontEndConfig      *FE_mp;         ///< holds the FrontEnd config, if any.
        CartAssemblyConfig  *cartAssy_mp;   ///< holds the CartAssembly config, if any.
    };

}; // namespace FEConfig

#endif /*FECONFIG_CONFIG_H_*/
