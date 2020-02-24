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
/// \brief definition of class FEConfig::ConfigProviderIniFile. 

#include "ConfigProviderIniFile.h"
#include "FrontEndConfig.h"
#include "XMLParser.h"
#include "iniFile.h"
#include "listDir.h"
#include "setTimeStamp.h"
#include "stringConvert.h"
#include "splitPath.h"
#include "trimStr.h"
#include "logger.h"
#include <stdlib.h>         // for atof()
#include <stdio.h>          // for sprintf()
using namespace std;

namespace FEConfig {

ConfigProviderIniFile::ConfigProviderIniFile(const std::string& filename)
  : iniFile_mp(new CIniFile(filename)),
    xmlFiles_m()
{
    iniFile_mp -> ReadFile();
}

ConfigProviderIniFile::~ConfigProviderIniFile() {
    delete iniFile_mp;
}

bool ConfigProviderIniFile::exists(unsigned configId) const {
    if (!configId)
        return false;
    
    Configuration::Record rec;
    return getConfiguration(configId, rec);
}

bool ConfigProviderIniFile::getConfiguration(unsigned configId, Configuration::Record &target) const {
    target.reset(configId);
    
    if (!configId)
        return false;

    char sectionName[30];
    sprintf(sectionName, "~Configuration40-%u", configId);

    target.description_m = iniFile_mp -> GetValue(sectionName, "Description");

    int endPos(0);
    string tmp;
    unsigned tmpU;
    tmp = iniFile_mp -> GetValue(sectionName, "FrontEnd");
    if (!tmp.empty()) {
        parseUnsigned(tmp, tmpU, endPos); // parse the vestigial facility code
        parseUnsigned(tmp, target.keyFrontEnd_m, endPos);
    }
    tmp = iniFile_mp -> GetValue(sectionName, "CartAssembly");
    if (!tmp.empty()) {
        parseCartRow(tmp, target.CAId_m);
    }
    return target.isValid();
}

bool ConfigProviderIniFile::getAllConfigurations(std::vector<Configuration::Record> &target) const {
    target.clear();

    char toFind[30];
    sprintf(toFind, "~Configuration");

    string sectionName, description;
    unsigned numSections = iniFile_mp -> NumKeys();
    unsigned configId, keyFrontEnd, keyCartAssy;
    Configuration::Record rec;

    for (unsigned index = 0; index < numSections; ++index) {
        sectionName = iniFile_mp -> KeyName(index);
        if (sectionName.find(toFind, 0) != string::npos) {
            int dashPos = sectionName.find("-", 0);
            configId = from_string<unsigned>(sectionName.substr(dashPos + 1));
            rec.reset();
            getConfiguration(configId, rec);
            target.push_back(rec);
        }
    }
    return !target.empty();
}

bool ConfigProviderIniFile::getFrontEndConfig(unsigned keyFrontEnd, FrontEndConfig &target) const {
    target.reset(keyFrontEnd);
    
    if (!keyFrontEnd)
        return false;
    
    string tmp;
    char sectionName[30];
    char itemName[30];
    sprintf(sectionName, "~FrontEnd40-%u", keyFrontEnd);
    
    unsigned fkCryostat(0), fkLPR(0), numCarts(0);

    target.SN_m = iniFile_mp -> GetValue(sectionName, "SN");
    target.ESN_m = iniFile_mp -> GetValue(sectionName, "ESN");

    tmp = iniFile_mp -> GetValue(sectionName, "Cryostat");
    if (!tmp.empty())
        fkCryostat = from_string<unsigned>(tmp);

    if (fkCryostat) {
        CryostatConfig cryoConfig;
        if (getCryostatConfig(fkCryostat, cryoConfig))
            target.setCryostatConfig(cryoConfig);
    }
        
    // FETIM: default to 1 unless explicitly set to 0:
    tmp = iniFile_mp -> GetValue(sectionName, "FETIM");
    if (!tmp.empty())
        target.fkFETIM_m = from_string<unsigned>(tmp);
    else
        target.fkFETIM_m = 1;

    tmp = iniFile_mp -> GetValue(sectionName, "IFSwitch");
    if (!tmp.empty())
        target.fkIFSwitch_m = from_string<unsigned>(tmp);

    tmp = iniFile_mp -> GetValue(sectionName, "LPR");
    if (!tmp.empty())
        fkLPR = from_string<unsigned>(tmp);

    if (fkLPR) {
        LPRConfig lprConfig;
        if (getLPRConfig(fkLPR, lprConfig))
            target.setLPRConfig(lprConfig);
    }
        
    tmp = iniFile_mp -> GetValue(sectionName, "PowerDist");
    if (!tmp.empty())
        target.fkPowerDist_m = from_string<unsigned>(tmp);
    
    tmp = iniFile_mp -> GetValue(sectionName, "Carts");
    if (!tmp.empty())
        numCarts = from_string<unsigned>(tmp);

    CartAssemblyConfig caConfig;
    
    for (int cart = 1; cart <= numCarts; ++cart) {
        sprintf(itemName, "Cart%02d", cart);
        tmp = iniFile_mp -> GetValue(sectionName, itemName);
        if (!tmp.empty()) {
            caConfig.reset();
            if (parseCartRow(tmp, caConfig.Id_m)) {
                caConfig.coldCart_m.reset(caConfig.Id_m.CCABand_m, caConfig.Id_m.CCAId_m);
                caConfig.WCA_m.reset(caConfig.Id_m.WCABand_m, caConfig.Id_m.WCAId_m);
                if (caConfig.Id_m.CCAValid())
                    getColdCartConfig(caConfig.Id_m.CCABand_m, caConfig.Id_m.CCAId_m, caConfig.coldCart_m);
                if (caConfig.Id_m.WCAValid())
                    getWCAConfig(caConfig.Id_m.WCABand_m, caConfig.Id_m.WCAId_m, caConfig.WCA_m);
                target.setCartridgeConfig(caConfig.Id_m.port_m, caConfig);
            }
        }
    }
    
    return true;
}

bool ConfigProviderIniFile::getCryostatConfig(unsigned keyCryostat, CryostatConfig &target) const {
    target.reset(keyCryostat);

    if (!keyCryostat)
        return false;

    char sectionName[30];
    sprintf(sectionName, "~Cryostat40-%u", keyCryostat);
    
    target.SN_m = iniFile_mp -> GetValue(sectionName, "SN");
    target.ESN_m = iniFile_mp -> GetValue(sectionName, "ESN");

    return true;
}

bool ConfigProviderIniFile::getLPRConfig(unsigned keyLPR, LPRConfig &target) const {
    target.reset(keyLPR);

    if (!keyLPR)
        return false;

    char sectionName[30];
    sprintf(sectionName, "~LPR40-%u", keyLPR);
    
    target.SN_m = iniFile_mp -> GetValue(sectionName, "SN");
    target.ESN_m = iniFile_mp -> GetValue(sectionName, "ESN");

    return true;
}

bool ConfigProviderIniFile::getCartAssemblyConfig(CartAssemblyID CAId, CartAssemblyConfig &target) const {
    target.reset();

    if (!CAId.isValid())
        return false;
    
    target.Id_m = CAId;
    
    bool found;
    found = getColdCartConfig(CAId.CCABand_m, CAId.CCAId_m, target.coldCart_m);
    if (!found)
        target.coldCart_m.reset();

    found = getWCAConfig(CAId.WCABand_m, CAId.WCAId_m, target.WCA_m);
    if (!found)
        target.WCA_m.reset();
        
    return found;
}

bool ConfigProviderIniFile::getColdCartConfig(unsigned CCABand, unsigned keyColdCart, ColdCartConfig &target) const {
    target.reset(CCABand, keyColdCart);
    
    if (!CCABand || !keyColdCart)
        return false;

    unsigned cartBand(0), numParams(0); 
    string tmp;

    char sectionName[30];
    char itemName[30];
    sprintf(sectionName, "~ColdCart%u-%u", CCABand, keyColdCart);

    tmp = iniFile_mp -> GetValue(sectionName, "Band");
    if (!tmp.empty())
        cartBand = from_string<unsigned>(tmp);
    target.band_m = cartBand;

    target.SN_m = iniFile_mp -> GetValue(sectionName, "SN");
    target.ESN_m = iniFile_mp -> GetValue(sectionName, "ESN");
    target.description_m = iniFile_mp -> GetValue(sectionName, "Description");
    
    // If the XML key is defined, load the configuration from the XML file instead:
    tmp = iniFile_mp -> GetValue(sectionName, "XML");
    if (!tmp.empty()) {
        string xmlFile(tmp);
        string iniFile(iniFile_mp -> Path());
        string iniPath;

        // get the path where the FrontendControlDLL.ini file is located:
        splitPath(iniFile, iniPath, tmp);
        if (iniPath.empty())
            iniPath = ".";

        xmlFile = iniPath + "/" + xmlFile;

        XMLParser xmlParser(xmlFile);
        return xmlParser.getColdCartConfig(target);
    }

    tmp = iniFile_mp -> GetValue(sectionName, "MixerParams");
    if (!tmp.empty())
        numParams = from_string<unsigned>(tmp);
        
    if (numParams) {       
        double freqLO;
        float VJ01, VJ02, VJ11, VJ12, IJ01, IJ02, IJ11, IJ12;

        for (int param = 1; param <= numParams; ++param) {
            sprintf(itemName, "MixerParam%02d", param);
            tmp = iniFile_mp -> GetValue(sectionName, itemName);
            if (parseMixerParamRow(tmp, freqLO, VJ01, VJ02, VJ11, VJ12, IJ01, IJ02, IJ11, IJ12)) {
                target.mixerParams_m.set(freqLO, MixerParams::VJ01, VJ01);
                target.mixerParams_m.set(freqLO, MixerParams::VJ02, VJ02);
                target.mixerParams_m.set(freqLO, MixerParams::VJ11, VJ11);
                target.mixerParams_m.set(freqLO, MixerParams::VJ12, VJ12);
                target.mixerParams_m.set(freqLO, MixerParams::IJ01, IJ01);
                target.mixerParams_m.set(freqLO, MixerParams::IJ02, IJ02);
                target.mixerParams_m.set(freqLO, MixerParams::IJ11, IJ11);
                target.mixerParams_m.set(freqLO, MixerParams::IJ12, IJ12);
            }
        }
    }
    
    tmp = iniFile_mp -> GetValue(sectionName, "MagnetParams");
    if (!tmp.empty())
        numParams = from_string<unsigned>(tmp);
        
    if (numParams) {       
        double freqLO;
        float IMag01, IMag02, IMag11, IMag12;

        for (int param = 1; param <= numParams; ++param) {
            sprintf(itemName, "MagnetParam%02d", param);
            tmp = iniFile_mp -> GetValue(sectionName, itemName);
            if (parseMagnetParamRow(tmp, freqLO, IMag01, IMag02, IMag11, IMag12)) {
                target.magnetParams_m.set(freqLO, MagnetParams::IMag01, IMag01);
                target.magnetParams_m.set(freqLO, MagnetParams::IMag02, IMag02);
                target.magnetParams_m.set(freqLO, MagnetParams::IMag11, IMag11);
                target.magnetParams_m.set(freqLO, MagnetParams::IMag12, IMag12);
            }
        }
    }
    
    tmp = iniFile_mp -> GetValue(sectionName, "PreampParams");
    if (!tmp.empty())
        numParams = from_string<unsigned>(tmp);
        
    if (numParams) {       
        double freqLO;
        unsigned pol, sis;
        float VD1, VD2, VD3, ID1, ID2, ID3, VG1, VG2, VG3;

        for (int param = 1; param <= numParams; ++param) {
            sprintf(itemName, "PreampParam%02d", param);
            tmp = iniFile_mp -> GetValue(sectionName, itemName);
            if (parsePreampParamRow(tmp, freqLO, pol, sis, VD1, VD2, VD3, ID1, ID2, ID3, VG1, VG2, VG3)) {
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::VD1, VD1);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::VD2, VD2);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::VD3, VD3);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::ID1, ID1);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::ID2, ID2);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::ID3, ID3);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::VG1, VG1);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::VG2, VG2);
                target.usePreampParams(pol, sis).set(freqLO, PreampParams::VG3, VG3);
            }
        }
    }
    
    //TODO:  get tempsensor params.
    return true;
}


bool ConfigProviderIniFile::getWCAConfig(unsigned WCABand, unsigned keyWCA, WCAConfig &target) const {
    target.reset(WCABand, keyWCA);
    
    if (!WCABand || !keyWCA)
        return false;

    unsigned cartBand(0), numParams(0), x(0); 
    string tmp;

    char sectionName[30];
    char itemName[30];
    sprintf(sectionName, "~WCA%u-%u", WCABand, keyWCA);
    
    tmp = iniFile_mp -> GetValue(sectionName, "Band");
    if (!tmp.empty())
        cartBand = from_string<unsigned>(tmp);
    target.band_m = cartBand;
    
    target.SN_m = iniFile_mp -> GetValue(sectionName, "SN");
    target.ESN_m = iniFile_mp -> GetValue(sectionName, "ESN");
    target.description_m = iniFile_mp -> GetValue(sectionName, "Description");
    
    // If the XML key is defined, load the configuration from the XML file instead:
    tmp = iniFile_mp -> GetValue(sectionName, "XML");
    if (!tmp.empty()) {
        string xmlFile(tmp);
        string iniFile(iniFile_mp -> Path());
        string iniPath;

        // get the path where the FrontendControlDLL.ini file is located:
        splitPath(iniFile, iniPath, tmp);
        if (iniPath.empty())
            iniPath = ".";

        xmlFile = iniPath + "/" + xmlFile;

        XMLParser xmlParser(xmlFile);
        return xmlParser.getWCAConfig(target);
    }

    tmp = iniFile_mp -> GetValue(sectionName, "FLOYIG");
    if (!tmp.empty())
        target.FLOYIG_m = (float) atof(tmp.c_str());
        
    tmp = iniFile_mp -> GetValue(sectionName, "FHIYIG");
    if (!tmp.empty())
        target.FHIYIG_m = (float) atof(tmp.c_str());
    
    tmp = iniFile_mp -> GetValue(sectionName, "LOParams");
    if (!tmp.empty())
        numParams = from_string<unsigned>(tmp);

    tmp = iniFile_mp -> GetValue(sectionName, "LoopBW");
    if (!tmp.empty())
        target.loopBW_m = static_cast<WCAConfig::LOOPBW_OPTS>(from_string<int>(tmp));

    if (numParams) {       
        double freqLO;
        float VD0, VD1, VG0, VG1;
        
        for (int param = 1; param <= numParams; ++param) {
            sprintf(itemName, "LOParam%02d", param);
            tmp = iniFile_mp -> GetValue(sectionName, itemName);
            if (parseLOParamRow(tmp, freqLO, VD0, VD1, VG0, VG1)) {
                target.PAParams_m.set(freqLO, PowerAmpParams::VD0, VD0);
                target.PAParams_m.set(freqLO, PowerAmpParams::VD1, VD1);
                target.PAParams_m.set(freqLO, PowerAmpParams::VG0, VG0);
                target.PAParams_m.set(freqLO, PowerAmpParams::VG1, VG1);
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//private:

bool ConfigProviderIniFile::parseCartSectionName(const string &src, unsigned &provider, unsigned &id) {
    provider = id = 0;
    int p2 = src.rfind("-");
    int len = src.length();
    if (p2 == string::npos)
        return false;
    
    id = from_string<unsigned>(src.substr(p2 + 1, string::npos));
    if (!id)
        return false;
    
    int p1 = src.find_last_not_of("0123456789", p2-1);
    if (p1 == string::npos)
        return false;
    if (p1 + 1 >= p2 - 1)
        return false;
    
    provider = from_string<unsigned>(src.substr(p1 + 1, p2 - p1 - 1)); 
    if (!provider)
        return false;
        
    return true;
}

bool ConfigProviderIniFile::parseCartRow(const std::string &src, CartAssemblyID &target)
{
    //each Cart row is like:
    //cartPos,cartBand,ccFacility,ccId,wcaFacility,wcaId
    target.reset();

    if (src.empty())
        return false;
    
    int nextPos(0);
    if (!parseUnsigned(src, target.port_m, nextPos))
        return false;
    if (!parseUnsigned(src, target.band_m, nextPos))
        return false;
    if (!parseUnsigned(src, target.CCABand_m, nextPos))
        return false;
    if (!parseUnsigned(src, target.CCAId_m, nextPos))
        return false;
    if (!parseUnsigned(src, target.WCABand_m, nextPos))
        return false;
    if (!parseUnsigned(src, target.WCAId_m, nextPos))
        return false;
    return true;
}

bool ConfigProviderIniFile::parseMixerParamRow(const string &src, double &freqLO, 
                                               float &VJ01, float &VJ02, float &VJ11, float &VJ12, 
                                               float &IJ01, float &IJ02, float &IJ11, float &IJ12)
{
    //each NixerParam row is like:
    //freqLO,VJ01,VJ02,VJ11,VJ12,IJ01,IJ02,IJ11,IJ12
    freqLO = 0;
    VJ01 = VJ02 = VJ11 = VJ12 = IJ01 = IJ02 = IJ11 = IJ12 = 0;
    
    if (src.empty())
        return false;
    
    int nextPos(0);
    if (!parseDouble(src, freqLO, nextPos))
        return false;
    if (!parseFloat(src, VJ01, nextPos))
        return false;
    if (!parseFloat(src, VJ02, nextPos))
        return false;
    if (!parseFloat(src, VJ11, nextPos))
        return false;
    if (!parseFloat(src, VJ12, nextPos))
        return false;
    if (!parseFloat(src, IJ01, nextPos))
        return false;
    if (!parseFloat(src, IJ02, nextPos))
        return false;
    if (!parseFloat(src, IJ11, nextPos))
        return false;
    if (!parseFloat(src, IJ12, nextPos))
        return false;
    return true;
}

bool ConfigProviderIniFile::parseMagnetParamRow(const string &src, double &freqLO, 
                                                float &IMag01, float &IMag02, float &IMag11, float &IMag12)
{
    //each MagnetParam row is like:
    //freqLO,IMag01,IMag02,IMag11,IMag12
    freqLO = 0;
    IMag01 = IMag02 = IMag11 = IMag12 = 0;
  
    if (src.empty())
        return false;
    
    int nextPos(0);
    if (!parseDouble(src, freqLO, nextPos))
        return false;
    if (!parseFloat(src, IMag01, nextPos))
        return false;
    if (!parseFloat(src, IMag02, nextPos))
        return false;
    if (!parseFloat(src, IMag11, nextPos))
        return false;
    if (!parseFloat(src, IMag12, nextPos))
        return false;
    
    return true;
}
        
bool ConfigProviderIniFile::parsePreampParamRow(const string &src, double &freqLO, 
                                                unsigned &pol, unsigned &sis, 
                                                float &VD1, float &VD2, float &VD3, 
                                                float &ID1, float &ID2, float &ID3, 
                                                float &VG1, float &VG2, float &VG3)
{
    //each PreampParam row is like:
    //freqLO,Pol,SIS,VD1,VD2,VD3,ID1,ID2,ID3,VG1,VG2,VG3
    freqLO = 0;
    VD1 = VD2 = VD3 = ID1 = ID2 = ID3 = VG1 = VG2 = VG3 = 0;
  
    if (src.empty())
        return false;
    
    int nextPos(0);
    if (!parseDouble(src, freqLO, nextPos))
        return false;
    if (!parseUnsigned(src, pol, nextPos))
        return false;
    if (!parseUnsigned(src, sis, nextPos))
        return false;
    if (!parseFloat(src, VD1, nextPos))
        return false;
    if (!parseFloat(src, VD2, nextPos))
        return false;
    if (!parseFloat(src, VD3, nextPos))
        return false;
    if (!parseFloat(src, ID1, nextPos))
        return false;
    if (!parseFloat(src, ID2, nextPos))
        return false;
    if (!parseFloat(src, ID3, nextPos))
        return false;
    if (!parseFloat(src, VG1, nextPos))
        return false;
    if (!parseFloat(src, VG2, nextPos))
        return false;
    if (!parseFloat(src, VG3, nextPos))
        return false;
    return true;
}

bool ConfigProviderIniFile::parseLOParamRow(const string &src, double &freqLO, 
                                            float &VD0, float &VD1, float &VG0, float &VG1) 
{
    //each LOParam row is like:
    //freqLO,VD0,VD1,VG0,VG1
    freqLO = 0;
    VD0 = VD1 = VG0 = VG1 = 0;
  
    if (src.empty())
        return false;
    
    int nextPos(0);
    if (!parseDouble(src, freqLO, nextPos))
        return false;
    if (!parseFloat(src, VD0, nextPos))
        return false;
    if (!parseFloat(src, VD1, nextPos))
        return false;
    if (!parseFloat(src, VG0, nextPos))
        return false;
    if (!parseFloat(src, VG1, nextPos))
        return false;
    return true;
}

bool ConfigProviderIniFile::parseUnsigned(const string &src, unsigned &target, int &nextPos) {
    target = 0;
    int p1(nextPos), p2;
    string tmp("");
        
    // look for the next comma:
    p2 = src.find(",", p1);
    if (p2 == string::npos) {
        // no comma found.  Use the rest of the string:
        tmp = TrimStr(src.substr(p1, string::npos));
    } else {
        // found comma.  Get the value:    
        tmp = TrimStr(src.substr(p1, p2 - p1));
        // Advance nextPos past the comma:
        nextPos = p2 + 1;
    }
    if (!tmp.empty()) {
        target = from_string<unsigned>(tmp);
        return true;
    }
    return false;
}

bool ConfigProviderIniFile::parseDouble(const string &src, double &target, int &nextPos) {
    target = 0.0;
    int p1(nextPos), p2;
    string tmp("");
        
    // look for the next comma:
    p2 = src.find(",", p1);
    if (p2 == string::npos) {
        // no comma found.  Use the rest of the string:
        tmp = TrimStr(src.substr(p1, string::npos));
    } else {
        // found comma.  Get the value:    
        tmp = TrimStr(src.substr(p1, p2 - p1));
        // Advance nextPos past the comma:
        nextPos = p2 + 1;
    }
    if (!tmp.empty()) {
        target = atof(tmp.c_str());
        return true;
    }
    return false;
}

bool ConfigProviderIniFile::parseFloat(const string &src, float &target, int &nextPos) {
    double dTarget;
    if (parseDouble(src, dTarget, nextPos)) {
        target = (float) dTarget;
        return true;
    }
    return false;
}

}; // namespace FEConfig


