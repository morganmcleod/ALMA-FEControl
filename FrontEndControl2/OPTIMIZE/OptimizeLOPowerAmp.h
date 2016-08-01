#ifndef OPTIMIZELOPOWERAMP_H_
#define OPTIMIZELOPOWERAMP_H_
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
/// \brief An optimization procedure which adjusts the LO PA drain voltages to attain target junciton currents.

#include "OptimizeBase.h"
#include "CONFIG/LookupTables.h"
using namespace FEConfig;
class CartAssembly;

class OptimizeLOPowerAmp : public OptimizeBase {
public:    
    OptimizeLOPowerAmp(CartAssembly &CA)
      : OptimizeBase("OptimizeLOPowerAmp"),
        cartAssembly_m(CA)
        { reset(); }

    virtual ~OptimizeLOPowerAmp()
      {}

    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(double freqLO, 
               float VD0, float targetIJ01, 
               float VD1, float targetIJ11,
               int repeatCount);
    
    const PowerAmpParams &getResult() const
      { return paParams_m; }
    
protected:    
    virtual void optimizeAction();
   
    CartAssembly &cartAssembly_m;

    double freqLO_m;
    float VD0_m;
    float VD1_m;
    float targetIJ01_m;
    float targetIJ11_m;
    int repeatCount_m;
    
    PowerAmpParams paParams_m;
};

#endif /*OPTIMIZELOPOWERAMP_H_*/
