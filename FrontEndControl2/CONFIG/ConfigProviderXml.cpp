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

#include "ConfigProviderXml.h"

namespace FEConfig {

ConfigProviderXml::ConfigProviderXml(const std::string& xmlFilesDirectory) {
    // TODO Auto-generated constructor stub
}

ConfigProviderXml::~ConfigProviderXml() {
    // TODO Auto-generated destructor stub
}

bool ConfigProviderXml::exists(unsigned configId) const {
    return false;
}

void ConfigProviderXml::setESNList(const StringSet &toCopy) {
}

bool ConfigProviderXml::getConfiguration(unsigned configId, Configuration::Record &target) const {
    return false;
}

bool ConfigProviderXml::getAllConfigurations(std::vector<Configuration::Record> &target) const {
    return false;
}

bool ConfigProviderXml::getFrontEndConfig(unsigned keyFrontEnd, FrontEndConfig &target) const {
    return false;
}

bool ConfigProviderXml::getCryostatConfig(unsigned keyCryostat, CryostatConfig &target) const {
    return false;
}

bool ConfigProviderXml::getLPRConfig(unsigned keyLPR, LPRConfig &target) const {
    return false;
}

bool ConfigProviderXml::getCartAssemblyConfig(CartAssemblyID id, CartAssemblyConfig &target) const {
    return false;
}

bool ConfigProviderXml::getColdCartConfig(unsigned CCABand, unsigned keyColdCart, ColdCartConfig &target) const {
    return false;
}

bool ConfigProviderXml::getWCAConfig(unsigned WCABand, unsigned keyWCA, WCAConfig &target) const {
    return false;
}

}; // namespace FEConfig
