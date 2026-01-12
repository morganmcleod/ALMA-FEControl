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

#include "XMLParser.h"
#include "stringConvert.h"
#include <map>
#include <string>
using namespace std;
using namespace expatmm;

namespace FEConfig {

XMLParser::XMLParser(std::string filename)
 : ExpatXMLFileParser(filename),
   targetType_m(TYPE_UNDEFINED)
{
    target_m.cca_p = NULL;
    initAttrStrings();
}


bool XMLParser::getColdCartConfig(ColdCartConfig &target) {
    target_m.cca_p = &target;
    targetType_m = TYPE_COLDCART;
    return Parse();
}


bool XMLParser::getWCAConfig(WCAConfig &target) {
    target_m.wca_p = &target;
    targetType_m = TYPE_WCA;
    return Parse();
}

bool XMLParser::getCryostatConfig(CryostatConfig &target) {
    target_m.cryostat_p = &target;
    targetType_m = TYPE_CRYOSTAT;
    return Parse();
}


enum attrStrings {
    MixerParams,
    FreqLO,
    VJ01,
    VJ02,
    VJ11,
    VJ12,
    IJ01,
    IJ02,
    IJ11,
    IJ12,
    MagnetParams,
    IMag01,
    IMag02,
    IMag11,
    IMag12,
    PreampParamsPol0Sb1,
    PreampParamsPol0Sb2,
    PreampParamsPol1Sb1,
    PreampParamsPol1Sb2,
    VD0,
    VD1,
    VD2,
    VD3,
    ID1,
    ID2,
    ID3,
    VG0,
    VG1,
    VG2,
    VG3,
    FLOYIG,
    FHIYIG,
    PowerAmp,
    TVOCoefficients,
    Se,
    A0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6
};

static std::map<std::string, attrStrings> s_mapStrings;


void XMLParser::StartElement(const XML_Char *name, const XML_Char **attrs) {

    string attrName;
    string attrValue;
    double freqLO;
    double fValue;

    if (attrs[0]) {
        attrName = attrs[0];
        if (attrs[1])
            attrValue = attrs[1];
    }

    switch (targetType_m) {
        case TYPE_COLDCART:
            switch (s_mapStrings[name]) {
                case MixerParams:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            case VJ01:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::VJ01, fValue);
                                break;
                            case VJ02:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::VJ02, fValue);
                                break;
                            case VJ11:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::VJ11, fValue);
                                break;
                            case VJ12:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::VJ12, fValue);
                                break;
                            case IJ01:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::IJ01, fValue);
                                break;
                            case IJ02:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::IJ02, fValue);
                                break;
                            case IJ11:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::IJ11, fValue);
                                break;
                            case IJ12:
                                target_m.cca_p -> mixerParams_m.set(freqLO, MixerParams::IJ12, fValue);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case MagnetParams:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            case IMag01:
                                target_m.cca_p -> magnetParams_m.set(freqLO, MagnetParams::IMag01, fValue);
                                break;
                            case IMag02:
                                target_m.cca_p -> magnetParams_m.set(freqLO, MagnetParams::IMag02, fValue);
                                break;
                            case IMag11:
                                target_m.cca_p -> magnetParams_m.set(freqLO, MagnetParams::IMag11, fValue);
                                break;
                            case IMag12:
                                target_m.cca_p -> magnetParams_m.set(freqLO, MagnetParams::IMag12, fValue);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case PreampParamsPol0Sb1:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            default:
                                assignPreampParam(0, 1, freqLO, attrName, fValue);
                                break;
                        }
                    }
                    break;
                case PreampParamsPol0Sb2:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            default:
                                assignPreampParam(0, 2, freqLO, attrName, fValue);
                                break;
                        }
                    }
                    break;
                case PreampParamsPol1Sb1:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            default:
                                assignPreampParam(1, 1, freqLO, attrName, fValue);
                                break;
                        }
                    }
                    break;
                case PreampParamsPol1Sb2:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            default:
                                assignPreampParam(1, 2, freqLO, attrName, fValue);
                                break;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case TYPE_WCA:
            switch (s_mapStrings[name]) {
                case FLOYIG:
                    target_m.wca_p -> FLOYIG_m = from_string<double>(attrValue) / 1.0E9;
                    break;
                case FHIYIG:
                    target_m.wca_p -> FHIYIG_m = from_string<double>(attrValue) / 1.0E9;
                    break;
                case PowerAmp:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case FreqLO:
                                freqLO = fValue / 1.0E9;
                                break;
                            case VD0:
                                target_m.wca_p -> PAParams_m.set(freqLO, PowerAmpParams::VD0, fValue);
                                break;
                            case VD1:
                                target_m.wca_p -> PAParams_m.set(freqLO, PowerAmpParams::VD1, fValue);
                                break;
                            case VG0:
                                target_m.wca_p -> PAParams_m.set(freqLO, PowerAmpParams::VG0, fValue);
                                break;
                            case VG1:
                                target_m.wca_p -> PAParams_m.set(freqLO, PowerAmpParams::VG1, fValue);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case TYPE_CRYOSTAT:
            switch (s_mapStrings[name]) {
                case TVOCoefficients:
                    for (int i = 0; attrs[i]; i += 2) {
                        attrName = attrs[i];
                        attrValue = attrs[i + 1];
                        fValue = from_string<double>(attrValue);
                        switch (s_mapStrings[attrName]) {
                            case Se:
                                freqLO = fValue + 1;
                                break;
                            case A0:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff0, fValue);
                                break;
                            case A1:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff1, fValue);
                                break;
                            case A2:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff2, fValue);
                                break;
                            case A3:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff3, fValue);
                                break;
                            case A4:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff4, fValue);
                                break;
                            case A5:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff5, fValue);
                                break;
                            case A6:
                                target_m.cryostat_p -> tvoCoeff_m.set(freqLO, TVOCoeffParams::Coeff6, fValue);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case TYPE_UNDEFINED:
        default:
            break;
    }
}

void XMLParser::EndElement(const XML_Char *name) {

}

void XMLParser::assignPreampParam(unsigned pol, unsigned sis, double freqLO, const std::string &paramName, double fValue) {
    switch (s_mapStrings[paramName]) {
        case VD1:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::VD1, fValue);
            break;
        case VD2:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::VD2, fValue);
            break;
        case VD3:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::VD3, fValue);
            break;
        case ID1:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::ID1, fValue);
            break;
        case ID2:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::ID2, fValue);
            break;
        case ID3:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::ID3, fValue);
            break;
        case VG1:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::VG1, fValue);
            break;
        case VG2:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::VG2, fValue);
            break;
        case VG3:
            target_m.cca_p -> usePreampParams(pol, sis).set(freqLO, PreampParams::VG3, fValue);
            break;
        default:
            break;
    }
}

void XMLParser::initAttrStrings() const {
    if (!s_mapStrings.empty())
        return;

    s_mapStrings["MixerParams"] = MixerParams;
    s_mapStrings["FreqLO"] = FreqLO;
    s_mapStrings["VJ01"] = VJ01;
    s_mapStrings["VJ02"] = VJ02;
    s_mapStrings["VJ11"] = VJ11;
    s_mapStrings["VJ12"] = VJ12;
    s_mapStrings["IJ01"] = IJ01;
    s_mapStrings["IJ02"] = IJ02;
    s_mapStrings["IJ11"] = IJ11;
    s_mapStrings["IJ12"] = IJ12;
    s_mapStrings["MagnetParams"] = MagnetParams;
    s_mapStrings["IMag01"] = IMag01;
    s_mapStrings["IMag02"] = IMag02;
    s_mapStrings["IMag11"] = IMag11;
    s_mapStrings["IMag12"] = IMag12;
    s_mapStrings["PreampParamsPol0Sb1"] = PreampParamsPol0Sb1;
    s_mapStrings["PreampParamsPol0Sb2"] = PreampParamsPol0Sb2;
    s_mapStrings["PreampParamsPol1Sb1"] = PreampParamsPol1Sb1;
    s_mapStrings["PreampParamsPol1Sb2"] = PreampParamsPol1Sb2;
    s_mapStrings["VD0"] = VD0;
    s_mapStrings["VD1"] = VD1;
    s_mapStrings["VD2"] = VD2;
    s_mapStrings["VD3"] = VD3;
    s_mapStrings["ID1"] = ID1;
    s_mapStrings["ID2"] = ID2;
    s_mapStrings["ID3"] = ID3;
    s_mapStrings["VG0"] = VG0;
    s_mapStrings["VG1"] = VG1;
    s_mapStrings["VG2"] = VG2;
    s_mapStrings["VG3"] = VG3;
    s_mapStrings["FLOYIG"] = FLOYIG;
    s_mapStrings["FHIYIG"] = FHIYIG;
    s_mapStrings["PowerAmp"] = PowerAmp;
    s_mapStrings["TVOCoefficients"] = TVOCoefficients;
    s_mapStrings["Se"] = Se;
    s_mapStrings["A0"] = A0;
    s_mapStrings["A1"] = A1;
    s_mapStrings["A2"] = A2;
    s_mapStrings["A3"] = A3;
    s_mapStrings["A4"] = A4;
    s_mapStrings["A5"] = A5;
    s_mapStrings["A6"] = A6;
}

} // namespace FEConfig
