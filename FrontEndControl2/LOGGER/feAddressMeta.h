#ifndef _FRONTEND_DEFS_H_
#define _FRONTEND_DEFS_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2005
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

/************************************************************************
 * Defines some constants specific to the FE control software.
 * 
 *----------------------------------------------------------------------
 */

#include <FrontEndAMB/ambDefs.h>
#include <iostream>

/// namespace for constants and support classes for the FE and its subsystems.
namespace FrontEnd {

    /// Define the subsystems in the front end.
    enum SubSystem_t {
        SUBSYS_NONE             = -1,   ///< Used as a negative return value when decoding.
        SUBSYS_AMBSI            = 0,
        SUBSYS_AMBSI_SN,
        SUBSYS_SPECIAL_MONITOR,
        SUBSYS_SPECIAL_CONTROL,
        SUBSYS_CARTRIDGE_BIAS,
        SUBSYS_CARTRIDGE_LO,
        SUBSYS_CARTRIDGE_TEMP,
        SUBSYS_POWERDIST,
        SUBSYS_POWERDIST_CHANNEL,
        SUBSYS_IFSWITCH,
        SUBSYS_CRYOSTAT,
        SUBSYS_LPR
    };

    /// Base addresses specify large blocks of RCAs for front end subsystems.
    /// They are tested for exact match, in the order given here.
    enum BaseAddress_t {
        AMBSI_BASE          = 0x30000UL,    ///< standard AMBSI monitor points
        SPECIAL_CONTROL     = 0x21000UL,    ///< special commands to initialize the FE firmware
        SPECIAL_MONITOR     = 0x20000UL,    ///< special monitor points for the FE firmware
        CONTROL_BASE        = 0x10000UL,    ///< all FE control commands
        MONITOR_BASE        = 0x00000UL     ///< all FE monitor commands
    };
    
    /// Bitmasks are used to decode subsystems withing the MONITOR_BASE and CONTROL_BASE blocks.
    /// They are tested for exact match, in the order given here.
    enum Bitmask_t {
        BITMASK_POWERDIST       = 0xA000,   ///< match for cartridge power distribution module
        BITMASK_POWERDIST_CART  = 0x00F0,   ///< mask for power distribution cartridge number
        BITMASK_IFSWITCH        = 0xB000,   ///< match for IF switch module
        BITMASK_CRYOSTAT        = 0xC000,   ///< match for cryostat module 
        BITMASK_LPR             = 0xD000,   ///< match for LPR module
        BITMASK_E_NOT_IMPL      = 0xE000,   ///< match for subsystem not yet implemented
        BITMASK_F_NOT_IMPL      = 0xF000,   ///< match for subsystem not yet implemented
        BITMASK_CARTRIDGE       = 0xF000,   ///< mask for cartridge number in the high bits
        BITMASK_CARTRIDGE_TEMP  = 0x0880,   ///< match for cartridge temperature sensors
        BITMASK_CARTRIDGE_LO    = 0x0800,   ///< match for WCA LO module
        BITMASK_CARTRIDGE_BIAS  = 0x0800    ///< this bit must be zero for cartridge bias module
    };
    
    /// Bitshifts specify how many places to shift masked selector values.
    enum BitShift_t {
        BITSHIFT_POWERDIST_CART = 4,        ///< bit shift for power distribution cartridge number
        BITSHIFT_CARTRIDGE      = 12,       ///< bit shift for cartridge number in the high bits
    };


    /// This structure is populated when a monitor or control command is parsed.
    struct FESelector_t {
        AmbRelativeAddr base;
        SubSystem_t subsys;
        AmbRelativeAddr offset;
        std::string description;
        bool isCommand;
        bool isMonitor() const { return !isCommand; }
        bool isReadBack;
        int cartridge;
        
        FESelector_t()
          { reset(); }          
        unsigned getCartridge() const
          { return (cartridge >= 0) ? (unsigned) cartridge : 0; }

        void reset();
        bool decode(AmbRelativeAddr RCA);

        void streamOut(std::ostream& out) const;
        ///< stream output for debugging 

        inline static bool match(AmbRelativeAddr RCA, AmbRelativeAddr mask)
          { return ((RCA & mask) == mask); }
        ///< test whether set bits of mask are exactly matched in the RCA.
    };

    inline std::ostream &operator << (std::ostream& out, const FESelector_t &sel)
      { sel.streamOut(out); return out; }
    ///< stream output for debugging FESelector_t 
    
};
  
#endif /*_FRONTEND_DEFS_H_*/
