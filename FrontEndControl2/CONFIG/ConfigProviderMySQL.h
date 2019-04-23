#ifndef CONFIGPROVIDERMYSQL_H_
#define CONFIGPROVIDERMYSQL_H_
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
/// \brief declaration of class FEConfig::ConfigProviderMySQL. 

#include "ConfigProvider.h"
#include <string>
class FrontEndDatabase;

namespace FEConfig {
    /// Loads FE configuration data from a MySQL database connection.
    class ConfigProviderMySQL : public ConfigProvider {
    public:
        ConfigProviderMySQL();
        ///< construct the provider to find connection info from the environment.

        ConfigProviderMySQL(const std::string &host,
                            const std::string &user,
                            const std::string &password,
                            const std::string &dbName);
        ///< construct the provider with connection info for the MySQL database server.                                     
        
        virtual ~ConfigProviderMySQL();
    
        virtual bool exists(unsigned keyFacility, unsigned configId) const;
        ///< return true if the config specified by (keyFacility, configId) exists in the database, false otherwise.
    
        virtual bool getConfiguration(unsigned keyFacility, unsigned configId,
                                      Configuration::Record &target) const;
        ///< get the top-level configuration record specified by (keyFacility, configId).
        ///< returns false if the config does not exist or on error.

        virtual bool getFrontEndConfig(unsigned keyFacility, unsigned keyFrontEnd, FrontEndConfig &target) const;
        ///< get the FrontEndConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getCryostatConfig(unsigned keyFacility, unsigned keyCryostat, CryostatConfig &target) const;
        ///< get the CryostatConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getLPRConfig(unsigned keyFacility, unsigned keyLPR, LPRConfig &target) const;
        ///< get the LPRConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getCartAssemblyConfig(CartAssemblyID CAId, CartAssemblyConfig &target) const;
        ///< load the configuration for the specified CartAssembly.
        ///< returns false if the configuration is not available or on error.
    
        virtual bool getColdCartConfig(unsigned keyFacility, unsigned keyColdCart, ColdCartConfig &target) const;
        ///< get the ColdCartConfig portion of the specified config for the specified band.
        ///< returns false if that portion is not available or on error.
    
        virtual bool getWCAConfig(unsigned keyFacility, unsigned keyWCA, WCAConfig &target) const;
        ///< get the WCAConfig portion of the specified config for the specified band.
        ///< returns false if that portion is not available or on error.
    
    private:
        FrontEndDatabase *dbObject_mp; ///< connection to the MySQL server.
    };

}; // namespace FEConfig

#endif /*CONFIGPROVIDERMYSQL_H_*/
