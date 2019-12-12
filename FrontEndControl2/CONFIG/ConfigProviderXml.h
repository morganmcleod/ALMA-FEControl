#ifndef CONFIG_CONFIGPROVIDERXML_H_
#define CONFIG_CONFIGPROVIDERXML_H_
/*******************************************************************************
 * ALMA - Atacama Large Millimeter Array
 * (c) Associated Universities Inc., 2019
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
/// \brief declaration of class FEConfig::ConfigProviderIniFile.

#include "ConfigProvider.h"
#include <string>

namespace FEConfig {

    class ConfigProviderXml: public FEConfig::ConfigProvider {
    public:
        ConfigProviderXml(const std::string& xmlFilesDirectory = std::string());
        virtual ~ConfigProviderXml();

        virtual bool exists(unsigned configId) const;
        ///< return true if the config specified by configId exists in the database, false otherwise.

        virtual void setESNList(const StringSet &toCopy);
        ///< Set the list of ESNs for which to search for configuration

        virtual bool getConfiguration(unsigned configId, Configuration::Record &target) const;
        ///< get the top-level configuration record specified by configId.
        ///< returns false if the config does not exist or on error.

        virtual bool getAllConfigurations(std::vector<Configuration::Record> &target) const;
        ///< get all configurations for this provider.

        virtual bool getFrontEndConfig(unsigned keyFrontEnd, FrontEndConfig &target) const;
        ///< get the FrontEndConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.

        virtual bool getCryostatConfig(unsigned keyCryostat, CryostatConfig &target) const;
        ///< get the CryostatConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.

        virtual bool getLPRConfig(unsigned keyLPR, LPRConfig &target) const;
        ///< get the LPRConfig portion of the specified config.
        ///< returns false if that portion is not available or on error.

        virtual bool getCartAssemblyConfig(CartAssemblyID id, CartAssemblyConfig &target) const;
        ///< load the configuration for the specified CartAssembly.
        ///< returns false if the configuration is not available or on error.

        virtual bool getColdCartConfig(unsigned CCABand, unsigned keyColdCart, ColdCartConfig &target) const;
        ///< get the ColdCartConfig portion of the specified config for the specified band.
        ///< returns false if that portion is not available or on error.

        virtual bool getWCAConfig(unsigned WCABand, unsigned keyWCA, WCAConfig &target) const;
        ///< get the WCAConfig portion of the specified config for the specified band.
        ///< returns false if that portion is not available or on error.

    private:
        ConfigProviderXml(const ConfigProviderXml &other);
        ///< copy construct is private and forbidden.
        ConfigProviderXml &operator =(const ConfigProviderXml &other);
        ///< assignment is private and forbidden.

    };

}; // namespace FEConfig

#endif /* CONFIG_CONFIGPROVIDERXML_H_ */
