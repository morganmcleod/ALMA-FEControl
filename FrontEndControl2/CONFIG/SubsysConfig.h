#ifndef SUBSYSCONFIG_H_
#define SUBSYSCONFIG_H_

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
/// \brief declares data structures for front end subsystems configuration items in namespace FEConfig.  
#include "LookupTables.h"
#include <iostream>

namespace FEConfig {

    /// configuration data for the cryostat.
    class CryostatConfig {
    public:
        CryostatConfig(unsigned keyCryostat = 0)
          : keyCryostat_m(keyCryostat)
            {}
        ///< construct with the primary keyCryostat
        
        ~CryostatConfig()
          {}
    
        void reset(unsigned keyCryostat = 0);
        ///< reset all data to defaults, optionally assigning a new key

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
        
    public:
        unsigned keyCryostat_m;     ///< uniquely identifies a record in the Cryostats table.
        std::string SN_m;           ///< The cryostat's assigned serial number.
        std::string ESN_m;          ///< The cryostat's electronic serial number.
        TVOCoeffParams tvoCoeff_m;  ///< contains the mixer bias values which vary with LO frequency.
    };

    inline std::ostream &operator << (std::ostream& out, const CryostatConfig &config)
      { config.streamOut(out); return out; }
    ///< stream output for debugging CryostatConfig    

    /// configuration data for the LPR.
    class LPRConfig {
    public:
        LPRConfig(unsigned keyLPR = 0)
          : keyLPR_m(keyLPR)
            {}
        ///< construct with the primary keyLPR
        
        ~LPRConfig()
          {}
    
        void reset(unsigned keyLPR = 0);
        ///< reset all data to defaults

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 

    public:
        unsigned keyLPR_m;          ///< along with provider, uniquely identifies a record in the LPRs table.
        std::string SN_m;           ///< The LPR's assigned serial number.
        std::string ESN_m;          ///< The LPR's electronic serial number.
    };

    inline std::ostream &operator << (std::ostream& out, const LPRConfig &config)
      { config.streamOut(out); return out; }
    ///< stream output for debugging LPRConfig    

}; // namespace FEConfig;

#endif /*SUBSYSCONFIG_H_*/
