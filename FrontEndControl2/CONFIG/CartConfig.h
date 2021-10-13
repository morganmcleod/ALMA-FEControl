#ifndef CARTCONFIG_H_
#define CARTCONFIG_H_
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
*/

/// \file
/// \brief declares data structures for cartridge configuration items as-loaded from the config database.

#include <FrontEndAMB/ambDefs.h>
#include "LookupTables.h"
#include <iostream>

namespace FEConfig {

    /// holds the configuration for a cold cartridge
    class ColdCartConfig {
    public:
        ColdCartConfig(unsigned keyColdCart = 0)
          : keyColdCart_m(keyColdCart),
            band_m(0)
            {}
        ///< construct with the primary keyColdCart
        ~ColdCartConfig()
          {}
    
        void reset(unsigned band = 0, unsigned keyColdCart = 0);
        ///< reset all data to defaults, optionally assigning new key values
        
        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
    
    public:
        unsigned keyColdCart_m;     ///< uniquely identifies a record in ColdCarts table.
        unsigned band_m;            ///< the cartridge band.
        std::string SN_m;           ///< the assigned serial number for the cartridge.
        std::string ESN_m;          ///< the electronic serial number for the cartridge.
        MixerParams mixerParams_m;  ///< contains the mixer bias values which vary with LO frequency.
        MagnetParams magnetParams_m;///< contains the SIS magnet values which vary with LO frequency.
        std::string description_m;  ///< description of the coldCart

        /// non-const access to preampParams
        PreampParams &usePreampParams(unsigned pol, unsigned sb)
          { MixerParams::normalizePolSb(pol, sb);
            return preampParams_m[pol * 2 + (sb - 1)]; }

        /// const access to preampParams
        const PreampParams &getPreampParams(unsigned pol, unsigned sb) const
          { return const_cast<ColdCartConfig*>(this) -> usePreampParams(pol, sb); }
            
    private:
        PreampParams preampParams_m[4]; ///< contains the LNA bias values for two or four channels.
    };
    
    inline std::ostream &operator << (std::ostream& out, const ColdCartConfig &config)
      { config.streamOut(out); return out; }
    ///< stream output for debugging ColdCartConfig    

//----------------------------------------------------------------------------

    /// Holds the configuration for a warm cartridge assembly and the associated first LO.
    class WCAConfig {
    public:
        WCAConfig(unsigned keyWCA = 0)
          : keyWCA_m(keyWCA),
            band_m(0),
            FLOYIG_m(0.0),
            FHIYIG_m(0.0),
            loopBW_m(LOOPBW_DEFAULT),
            lockingStrategy_m(LOCK_NORMAL)
            {}
        ///< construct with the primary keyWCA
        ~WCAConfig()
          {}
    
        void reset(unsigned band = 0, unsigned keyWCA = 0);
        ///< reset all data to defaults, optionally assigning new key values
    
        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
    
    public:
        enum LOOPBW_OPTS {
            LOOPBW_DEFAULT  = -1,   ///< use the default loop bandwidth for the band's normal warm multiplier.
            LOOPBW_NORMAL   = 0,    ///< override to use the "normal" loop BW:   7.5MHz/V (Band 4,8,9)
            LOOPBW_ALT      = 1     ///< override to use the "alternate" loop BW: 15MHz/V (Band 3,5,6,7,10)
        };
        enum LOCK_STRATEGY_OPTS {
            LOCK_NORMAL             = 0,
            LOCK_ONLINE_2021JUL_B   = 1,
            LOCK_5_POINTS           = 2,
            LOCK_9_POINTS           = 3
        };
        unsigned keyWCA_m;          ///< along with provider, uniquely identifies a record in ColdCarts table.
        unsigned band_m;            ///< the cartridge band.
        std::string SN_m;           ///< the assigned serial number for the WCA.
        std::string ESN_m;          ///< the electronic serial number for the WCA.
        double FLOYIG_m;            ///< the lower YiG oscillator limit, in GHz.
        double FHIYIG_m;            ///< the upper YiG oscillator limit, in GHz.
        PowerAmpParams PAParams_m;  ///< contains the LO PA bias values which vary with LO frequency.
        std::string description_m;  ///< description of the WCA
        LOOPBW_OPTS loopBW_m;       ///< loop bandwidth option.
        LOCK_STRATEGY_OPTS lockingStrategy_m;  ///< locking strategy to use.
    };

    inline std::ostream &operator << (std::ostream& out, const WCAConfig &config)
      { config.streamOut(out); return out; }
    ///< stream output for debugging WCAConfig

//----------------------------------------------------------------------------

    /// Holds the keys of a CCA and a WCA paired as a CartAssembly
    class CartAssemblyID {
    public:
        unsigned port_m;            ///< what port the cartridge is connected to.  Usually same as band_m.
        unsigned band_m;            ///< the cartridge band.
        unsigned WCABand_m;         ///< the WCA band which usually is the same as the cartridge band.
        unsigned WCAId_m;           ///< uniquely identifies a record in WCAs table.
        unsigned CCABand_m;         ///< the CCA band which usually is the same as the cartridge band.
        unsigned CCAId_m;           ///< uniquely identifies a record in ColdCarts table.

        CartAssemblyID(unsigned port = 0,
                       unsigned band = 0,
                       unsigned WCABand = 0,
                       unsigned WCAId = 0,
                       unsigned CCABand = 0,
                       unsigned CCAId = 0)
          : port_m(port),
            band_m(band),
            WCABand_m(WCABand),
            WCAId_m(WCAId),
            CCABand_m(CCABand),
            CCAId_m(CCAId)
            {}

        void reset()
          { port_m = band_m = WCABand_m = WCAId_m = CCABand_m = CCAId_m = 0; }
        ///< reset all data to defaults.

        bool WCAValid() const
          { return WCABand_m && WCAId_m; }

        bool CCAValid() const
          { return CCABand_m && CCAId_m; }

        bool isValid() const
          { return port_m && band_m && (WCAValid() || CCAValid()); }

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging
    };
    inline std::ostream &operator << (std::ostream& out, const CartAssemblyID &id)
      { id.streamOut(out); return out; }

    /// holds the configuration for a CartAssembly
    class CartAssemblyConfig {
    public:
        CartAssemblyConfig()
          {}
        ///< constructor
        ~CartAssemblyConfig()
          {}
    
        void reset();
        ///< reset all data to defaults.
    
        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 
    
    public:
        CartAssemblyID Id_m;        ///< id Info
        ColdCartConfig coldCart_m;  ///< cold cartridge config goes here.
        WCAConfig WCA_m;            ///< WCA config goes here.
        std::string description_m;  ///< description of the CartAssembly
    };

    inline std::ostream &operator << (std::ostream& out, const CartAssemblyConfig &config)
      { config.streamOut(out); return out; }
    ///< stream output for debugging CartAssemblyConfig    

}; // namespace FEConfig;

#endif /*CARTCONFIG_H_*/
