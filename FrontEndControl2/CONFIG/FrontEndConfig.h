#ifndef FECONFIG_H_
#define FECONFIG_H_
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
/// \brief declaration of class FEConfig::FrontEndConfig. 


#include "SubsysConfig.h"
#include "CartConfig.h"
#include <iostream>
#include <vector>
 
/// namespace for all items relating to Front End configuration
namespace FEConfig {

    /// FrontEndConfig is an object with space for the config parameters front end and all of its subsystems.
    class FrontEndConfig {
    public:
        FrontEndConfig(unsigned keyFacility = 0, unsigned keyFrontEnd = 0);
        ///< construct with the primary key values keyFacility and keyFrontEnd
            
        ~FrontEndConfig()
          { reset(); }
          
        void reset(unsigned keyFacility = 0, unsigned keyFrontEnd = 0);
        ///< reset all data to defaults, optionally assigning new values to the keys.
        
        const CryostatConfig *getCryostatConfig() const
          { return cryo_mp; }
        ///< get the cryostat config, if any.
    
        void setCryostatConfig(const CryostatConfig &cryo)
          { delete cryo_mp;
            cryo_mp = new CryostatConfig(cryo); }
        ///< set the cryostat config.

        unsigned getFETIMConfig() const
          { return fkFETIM_m; }
        ///< get the IF switch configuration (just a number for now.)

        unsigned getIFSwitchConfig() const
          { return fkIFSwitch_m; }
        ///< get the IF switch configuration (just a number for now.)
        
        const LPRConfig *getLPRConfig() const
          { return lpr_mp; }
        ///< get the LPR config, if any.
    
        void setLPRConfig(const LPRConfig &lpr)
          { delete lpr_mp;
            lpr_mp = new LPRConfig(lpr); }
        ///< set the LPR config.

        unsigned getPowerDistConfig() const
          { return fkPowerDist_m; }
        ///< get the power distribution configuration (just a number for now.)
        
        const CartAssemblyConfig *getCartridgeConfig(unsigned band) const;
        ///< get the cart assembly config for the given band, if any.
    
        bool setCartridgeConfig(unsigned band, CartAssemblyConfig &cartAssy);
        ///< get the cart assembly config for the given band.
    
        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
         
    public:
        unsigned keyFacility_m; ///< the provider portion of the primary key.
        unsigned keyFrontEnd_m; ///< along with provider, uniquely identifies a record in the FrontEnds table.
        unsigned fkFETIM_m;     ///< non-zero if a FETIM is installed.
        unsigned fkIFSwitch_m;  ///< non-zero if an IF switch is installed.
        unsigned fkPowerDist_m; ///< non-zero if the power distribution module is installed.
        std::string SN_m;       ///< the assigned serial number for the FrontEnd.
        std::string ESN_m;      ///< the electronic serial number for the FrontEnd.
        
    private:
        FrontEndConfig(const FrontEndConfig &other);
        FrontEndConfig &operator =(const FrontEndConfig &other);
    
        CryostatConfig *cryo_mp;
        LPRConfig *lpr_mp;
        std::vector<CartAssemblyConfig *> cartridges_m;
    };

    inline std::ostream &operator << (std::ostream& out, const FrontEndConfig &config)
      { config.streamOut(out); return out; }
    ///< stream output for debugging FrontEndConfig    

}; // namespace FEConfig;
    
#endif /*FECONFIG_H_*/
