#include "feAddressMeta.h"

namespace FrontEnd {

void FESelector_t::reset() {
    base = offset = 0;
    isCommand = isReadBack = false;
    subsys = SUBSYS_NONE;
    cartridge = -1;
}

bool FESelector_t::decode(AmbRelativeAddr RCA) {
    AmbRelativeAddr cartBits;
    // reset all the variables this function may set:
    reset();
    // Test for generic AMBSI M&C commands:
    if (FESelector_t::match(RCA, AMBSI_BASE)) {        
        isCommand = false;
        base = AMBSI_BASE;
        offset = RCA - base;
        subsys = SUBSYS_AMBSI;
        return true;
    }
    // Test for special FEMC commands:
    if (FESelector_t::match(RCA, SPECIAL_CONTROL)) {        
        isCommand = true;
        base = SPECIAL_CONTROL;
        offset = RCA - base;
        subsys = SUBSYS_SPECIAL_CONTROL;
        return true;
    }
    // Test for special FEMC monitor points:
    if (FESelector_t::match(RCA, SPECIAL_MONITOR)) {        
        isCommand = false;
        base = SPECIAL_MONITOR;
        offset = RCA - base;
        subsys = SUBSYS_SPECIAL_MONITOR;
        return true;
    }
    // Test for special AMBSI serial number request at RCA=0:
    if (RCA == 0) {
        base = 0;
        offset = 0;
        subsys = SUBSYS_AMBSI_SN;
        return true;
    }
    // At this point it is assumed to be a FE monitor or command RCA.
    // If it is command, mask off the CONTROL_BASE bit:   
    if (FESelector_t::match(RCA, CONTROL_BASE)) {
        isCommand = true;
        base = CONTROL_BASE;
        RCA -= base;
    } else {
        base = MONITOR_BASE;
    }
    // Test for power distribution module:
    if ((RCA & BITMASK_CARTRIDGE) == BITMASK_POWERDIST) {
        offset = RCA - BITMASK_POWERDIST;
        cartBits = (offset & BITMASK_POWERDIST_CART);
        cartridge = cartBits >> BITSHIFT_POWERDIST_CART;                
        // If the bits in the cartridge position are in range then this RCA is for a power channel:
        if (cartridge >= 0 && cartridge <= 9) {                      
            offset -= cartBits;
            subsys = SUBSYS_POWERDIST_CHANNEL;
        // otherwise, it must be another RCA within the power dist module:                    
        } else {
            cartridge = -1;
            subsys = SUBSYS_POWERDIST;    
        }
        offset = RCA;
        return true;
    }
    
    // Test for IF switch:
    if ((RCA & BITMASK_CARTRIDGE) == BITMASK_IFSWITCH) {
        offset = RCA - BITMASK_IFSWITCH;
        subsys = SUBSYS_IFSWITCH;
        return true;
    }
    // Test for cryostat:
    if ((RCA & BITMASK_CARTRIDGE) == BITMASK_CRYOSTAT) {
        offset = RCA - BITMASK_CRYOSTAT;
        subsys = SUBSYS_CRYOSTAT;
        return true;
    }        
    // Test for LPR:
    if ((RCA & BITMASK_LPR) == BITMASK_LPR) {
        offset = RCA - BITMASK_LPR;
        subsys = SUBSYS_LPR;
        return true;
    }
    // Test for subsystems not yet implemented:
    if ((RCA & BITMASK_CARTRIDGE) == BITMASK_E_NOT_IMPL ||
        (RCA & BITMASK_CARTRIDGE) == BITMASK_F_NOT_IMPL)
        return false;
        
    // All that is left now are cartridge and LO subsystems.  
    // Mask off the cartridge selector bits:
    cartBits = (RCA & BITMASK_CARTRIDGE); 
    RCA -= cartBits;
    cartridge = cartBits >> BITSHIFT_CARTRIDGE;
        
    // Test for cartridge temperature sensors:
    if (FESelector_t::match(RCA, BITMASK_CARTRIDGE_TEMP)) {
        offset = RCA - BITMASK_CARTRIDGE_TEMP;
        subsys = SUBSYS_CARTRIDGE_TEMP;
        return true;
    }
    // Test for cartridge LO:
    if (FESelector_t::match(RCA, BITMASK_CARTRIDGE_LO)) {
        offset = RCA - BITMASK_CARTRIDGE_LO;
        subsys = SUBSYS_CARTRIDGE_LO;
        return true;
    }
    // Negative-match test for cold cartridge bias:
    if ((RCA & BITMASK_CARTRIDGE_BIAS) == 0) {
        offset = RCA;
        subsys = SUBSYS_CARTRIDGE_BIAS;
        return true;
    }
    // Cannot decode the RCA:                
    return false;
}

void FESelector_t::streamOut(std::ostream& out) const {
    if (cartridge >= 0)
        out << "Ca=" << (cartridge + 1);
} 
    
};  // namespace FrontEnd
