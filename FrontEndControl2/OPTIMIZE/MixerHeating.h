#ifndef MixerHeating_H_
#define MixerHeating_H_
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
class ColdCartImpl;


class MixerHeating : public OptimizeBase {
public:    
	MixerHeating(ColdCartImpl &coldCart)
      : OptimizeBase("MixerHeating"),
        coldCart_m(coldCart)
        { reset(); }

    virtual ~MixerHeating()
      {}

    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(int pol, float targetTemp, int timeout);
    ///< start the mixer heating process.
    ///< If pol == -1, heat both polarizations.  Else pol must be 0 or 1.
    ///< If targetTemp >= 4, retrigger the the heater for up to timeout seconds to attain the target temperature.
    ///< Else, retrigger the heater for exactly timeout seconds.
    ///< For band 9, timeout may be no longer than 2 seconds.
    ///< For bands 678, timeout may be up to 2 minutes.
    
    virtual void requestStop();
    ///< force the heater process to stop.

protected:    
    virtual void optimizeAction();

private:
    ColdCartImpl &coldCart_m;
    int pol_m;
    float targetTemp_m;
    int timeout_m;
};

#endif /*MixerHeating_H_*/
