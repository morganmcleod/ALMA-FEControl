#ifndef MIXERDEFLUX_H_
#define MIXERDEFLUX_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2009
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
/// \brief Procedure to demagnetize the mixer magnet coil and deflux the SIS mixer.

#include "OptimizeBase.h"
#include <iostream>
class XYPlotArray;
class CartAssembly;
class ColdCartImpl;
class WCAImpl;

class MixerDeflux : public OptimizeBase {
public:    
	MixerDeflux(CartAssembly &ca, XYPlotArray &target)
      : OptimizeBase("MixerDeflux"),
        ca_m(ca),
        cca_m(NULL),
        wca_m(NULL),
        data_m(target),
        dataFile_mp(NULL)
        { reset(); }

    virtual ~MixerDeflux()
      {}

    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(int pol);

protected:    
    virtual void optimizeAction();

private:
    void demagnetize(int pol, int sb);
    void goToMagnetCurrent(int pol, int sb, float IMag);
    void mixerHeating(int pol, int sb);
    void readData(int pol, int sb, bool printHeader = false);
    
    CartAssembly &ca_m; ///< cart assembly to demagnetize
    ColdCartImpl *cca_m;///< cold cartridge in the cart assembly
    WCAImpl *wca_m;     ///< optional WCA in the cart assembly
    int pol_m;          ///< which pol to demagnetize.  -1 means both pols
    float IMagMax_m;    ///< maximum magnet current for the demag operating
    float IMag_m;       ///< instantaneous magnet current
    unsigned stepSleep_m;   ///< wait this long between setting and reading the magnet current.
    float stepSize_m;   ///< how much less we swing the magnet current at each step.
    bool band5sweepMode_m;  ///< true for band 5, special sweeping mode.
    float VDNom0_m;     ///< Cache the prior/nominal LO PA drain voltage settings
    float VDNom1_m;
    float VJNom01_m;    ///< Cache the prior/nominal junction voltage settings
    float VJNom02_m;
    float VJNom11_m;
    float VJNom12_m;
    float IMagNom01_m;  ///< Cache the prior/nominal magnet current settings
    float IMagNom02_m;
    float IMagNom11_m;
    float IMagNom12_m;
    float progress_m;   ///< Progress bar value varies from 0 to 100%
    float progressIncrement_m;  ///< Step size for progress bar
    unsigned long timerStart_m; ///< Timer when operation started, for logging
    XYPlotArray &data_m;        ///< Output data array
    std::ostream *dataFile_mp;  ///< output data log file
};

#endif /*MIXERDEFLUX_H_*/
