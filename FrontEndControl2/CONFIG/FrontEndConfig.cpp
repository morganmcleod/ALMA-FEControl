#include "FrontEndConfig.h"

namespace FEConfig {

FrontEndConfig::FrontEndConfig(unsigned keyFrontEnd)
  : keyFrontEnd_m(keyFrontEnd),
    fkFETIM_m(0),
    fkIFSwitch_m(0),
    fkPowerDist_m(0),
    cryo_mp(NULL),
    lpr_mp(NULL),
    cartridges_m(10)
{ 
    for (int index = 0; index < 10; ++index)
        cartridges_m[index] = NULL; 
}

void FrontEndConfig::reset(unsigned keyFrontEnd) {
    keyFrontEnd_m = keyFrontEnd;
    SN_m = ESN_m = "";
    fkFETIM_m = fkIFSwitch_m = fkPowerDist_m = 0;
    delete cryo_mp;
    cryo_mp = NULL; 
    delete lpr_mp;
    lpr_mp = NULL;
    for (int index = 0; index < 10; ++index) {
        delete cartridges_m[index];
        cartridges_m[index] = NULL;
    }
}

const CartAssemblyConfig *FrontEndConfig::getCartridgeConfig(unsigned band) const {
    if (--band < 10) return cartridges_m[band];
    else return NULL; 
}

bool FrontEndConfig::setCartridgeConfig(unsigned band, CartAssemblyConfig &cartAssy) {
    if (--band < 10) {        
        delete cartridges_m[band];
        cartridges_m[band] = new CartAssemblyConfig(cartAssy);
        return true;
    } else 
        return false; 
}

void FrontEndConfig::streamOut(std::ostream& out) const {
    out << "FrontEndConfig(" << keyFrontEnd_m << "): "
        << "SN='" << SN_m << "' ESN='" << ESN_m << "'"
        << " cryostat=" << ((cryo_mp) ? cryo_mp -> keyCryostat_m : 0)
        << " FETIM=" << fkFETIM_m
        << " IFSwitch=" << fkIFSwitch_m
        << " LPR=" << ((lpr_mp) ? lpr_mp -> keyLPR_m : 0)
        << " CPDS=" << fkPowerDist_m << std::endl;
}

}; // namespace FEConfig;


