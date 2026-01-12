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

#include "SubsysConfig.h"

namespace FEConfig {
    
void CryostatConfig::reset(unsigned keyCryostat) {
    keyCryostat_m = keyCryostat;
    SN_m = ESN_m = ""; 
}

void CryostatConfig::streamOut(std::ostream& out) const {
    out << "CryostatConfig(" << keyCryostat_m << "): "
        << "SN='" << SN_m << "' ESN='" << ESN_m << "'" << std::endl;
    if (!keyCryostat_m) {
        out << "empty" << std::endl;
    } else {
        out << " - TVO Coefficients:" << std::endl << tvoCoeff_m;
    }
}

void LPRConfig::reset(unsigned keyLPR) {
    keyLPR_m = keyLPR;
    SN_m = ESN_m = "";
}

void LPRConfig::streamOut(std::ostream& out) const {
    out << "LPRConfig(" << keyLPR_m << "): "
        << "SN='" << SN_m << "' ESN='" << ESN_m << "'" << std::endl;
}
    
}; //namespace FEConfig
