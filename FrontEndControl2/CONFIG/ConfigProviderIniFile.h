#ifndef CONFIGPROVIDERINIFILE_H_
#define CONFIGPROVIDERINIFILE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
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
/// \brief declaration of class FEConfig::ConfigProviderIniFile. 

#include "ConfigProvider.h"
#include <string>

// Forward-declare implementation class for INI file parser:
class CIniFile;

namespace FEConfig {

    /// Loads FE configuration data from an ini file.   Also loads some content from XML files in TMCDB format.
    class ConfigProviderIniFile : public ConfigProvider {
    public:
        ConfigProviderIniFile(const std::string& filename);
        ///< construct the provider with filename and behavior info for the ini file.                                     
        
        virtual ~ConfigProviderIniFile();
    
        virtual bool exists(unsigned configId) const;
        ///< return true if the config specified by configId exists in the database, false otherwise.

        virtual bool getConfiguration(unsigned configId, Configuration::Record &target) const;
        ///< get the top-level configuration record specified by configId.
        ///< returns false if the config does not exist or on error.

        virtual bool getAllConfigurations(std::vector<Configuration::Record> &target) const;
        ///< get all configurations for the given provider.

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
        ConfigProviderIniFile(const ConfigProvider &other);
        ///< copy construct is private and forbidden.
        ConfigProviderIniFile &operator =(const ConfigProvider &other);
        ///< assignment is private and forbidden.

        bool getConfigurationRecord(const char *sectionName, Configuration::Record &target) const;
        ///< get the items which basically define a configuration.
        
        static bool parseCartSectionName(const std::string &src, unsigned &provider, unsigned &id);
        ///< private helper to extract the provider and id from a section name formatted like "~WCA2-3"
        ///< where provider=2 and id=3.
        
        static bool parseCartRow(const std::string &src, CartAssemblyID &target);
        ///< private helper to parse a cartridge assembly description row formatted like "cartPos,cartBand,WCAFacility,WCAId,CCAFacility,CCAId"
                
        static bool parseMixerParamRow(const std::string &src, double &FreqLO, 
                                       float &VJ01, float &VJ02, float &VJ11, float &VJ12, 
                                       float &IJ01, float &IJ02, float &IJ11, float &IJ12);
        ///< private helper to parse a MixerParam row formatted like "FreqLO,VJ01,VJ02,VJ11,VJ12,IJ01,IJ02,IJ11,IJ12"
        
        static bool parseMagnetParamRow(const std::string &src, double &FreqLO, 
                                        float &IMag01, float &IMag02, float &IMag11, float &IMag12);
        ///< private helper to parse a MagnetParam row formatted like "FreqLO,IMag01,IMag02,IMag11,IMag12"
                
        static bool parsePreampParamRow(const std::string &src, double &FreqLO,
                                        unsigned &pol, unsigned &sis, 
                                        float &VD1, float &VD2, float &VD3, 
                                        float &ID1, float &ID2, float &ID3, 
                                        float &VG1, float &VG2, float &VG3);
        ///< private helper to parse a PreampParam row formatted like "FreqLO,Pol,SIS,VD1,VD2,VD3,ID1,ID2,ID3,VG1,VG2,VG3"

        static bool parseTVOCoeffRow(const std::string &src, 
                                    float &X0, float &X1, float &X2, float &X3,
                                    float &X4, float &X5, float &X6);

        static bool parseLOParamRow(const std::string &src, double &FreqLO, 
                                    float &VD0, float &VD1, float &VG0, float &VG1);
        ///< private helper to parse an LOParam row formatted like "FreqLO,VD0,VD1,VG0,VG1"

        static bool parseUnsigned(const std::string &src, unsigned &target, int &nextPos);
        ///< private helper to extract an unsigned from the given string formatted as one of the above.
        ///< extraction starts at nextPos.  Also nextPos returns the position of the next item in the string.

        static bool parseDouble(const std::string &src, double &target, int &nextPos);
        ///< private helper to extract a double from the given string formatted as one of the above.
        ///< extraction starts at nextPos.  Also nextPos returns the position of the next item in the string.
        
        static bool parseFloat(const std::string &src, float &target, int &nextPos);
        ///< private helper to extract a double from the given string formatted as one of the above.
        ///< extraction starts at nextPos.  Also nextPos returns the position of the next item in the string.
        
        unsigned providerCode_m;    ///< vestigial, for loading like [~Configuration49-9999]
        CIniFile *iniFile_mp;       ///< ini file to use
        StringSet xmlFiles_m;       ///< XML files found in the INI file directory
    };

}; // namespace FEConfig

#endif /*CONFIGPROVIDERINIFILE_H_*/
