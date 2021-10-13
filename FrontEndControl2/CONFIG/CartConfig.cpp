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

#include "CartConfig.h"
#include "logger.h"
#include <iomanip>
using namespace std;

namespace FEConfig {
    
static const bool DEBUG_CONFIG_PARAMS = true;

void ColdCartConfig::reset(unsigned band, unsigned keyColdCart) {
    band_m = band;
    keyColdCart_m = keyColdCart;
    SN_m = ESN_m = description_m = "";        
    mixerParams_m.clear();
    magnetParams_m.clear(); 
    preampParams_m[0].clear();
    preampParams_m[1].clear();
    preampParams_m[2].clear();
    preampParams_m[3].clear(); 
}

void ColdCartConfig::streamOut(std::ostream& out) const {
    out << "ColdCartConfig(" << keyColdCart_m << "): ";
    if (!keyColdCart_m) {
        out << "empty" << endl;
    } else {
        out << "band=" << band_m << " SN='" << SN_m << "' ESN='" << ESN_m << "' '" << description_m << "'" << endl;
        if (DEBUG_CONFIG_PARAMS) {
            out << " - MixerParams:" << endl << mixerParams_m;
            out << " - MagnetParams:" << endl << magnetParams_m;
            out << " - PreampParamsPol0Sb1:" << endl << getPreampParams(0, 1);
            out << " - PreampParamsPol0Sb2:" << endl << getPreampParams(0, 2);
            out << " - PreampParamsPol1Sb1:" << endl << getPreampParams(1, 1);
            out << " - PreampParamsPol1Sb2:" << endl << getPreampParams(1, 2);
        }
    }
}
    
void WCAConfig::reset(unsigned band, unsigned keyWCA) {
    band_m = band;
    keyWCA_m = keyWCA;
    SN_m = ESN_m = "";
    FLOYIG_m = FHIYIG_m = 0.0;      
    PAParams_m.clear(); 
    loopBW_m = LOOPBW_DEFAULT;
    lockingStrategy_m = LOCK_NORMAL;
}

void WCAConfig::streamOut(std::ostream& out) const {
    out << "WCAConfig(" << keyWCA_m << "): ";
    if (!keyWCA_m) {
        out << "empty" << endl;
    } else {
        string loopBW;
        switch (loopBW_m) {
            case LOOPBW_NORMAL:
                loopBW = "override_NORMAL";
                break;
            case LOOPBW_ALT:
                loopBW = "override_ALT";
                break;
            case LOOPBW_DEFAULT:
            default:
                loopBW = "band_default";
                break;
        }
        out  << "band=" << band_m <<" SN='" << SN_m << "' ESN='" << ESN_m << "'"
             << setprecision(4) << " FLOYIG=" << FLOYIG_m << " FHIYIG=" << FHIYIG_m
             << " loopBW=" << loopBW << " lockingStrategy=" << lockingStrategy_m
             << " '" << description_m << "'" << endl;
        if (DEBUG_CONFIG_PARAMS) {
            out << " - PowerAmpParams:" << PAParams_m << endl;
        }
    }
}

void CartAssemblyID::streamOut(std::ostream& out) const {
    out << "CartAssemblyID( port=" << port_m << ", band=" << band_m
        << ", WCABand=" << WCABand_m << ", WCAId=" << WCAId_m
        << ", CCABand=" << CCABand_m << ", CCAId=" << CCAId_m << " )";
}

void CartAssemblyConfig::reset() {
    Id_m.reset();
    coldCart_m.reset();
    WCA_m.reset(); 
    description_m.clear();
} 

void CartAssemblyConfig::streamOut(std::ostream& out) const {
    out << Id_m << " '" << description_m << "'" << endl
        << coldCart_m << WCA_m;
}


}; // namespace FEConfig;

