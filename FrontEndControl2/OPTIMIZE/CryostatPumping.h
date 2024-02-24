#ifndef CRYOSTATPUMPING_H_
#define CRYOSTATPUMPING_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
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
/// \brief Procedure which runs while pumping down the cryostat.  Closes the gate valve and turns off pumps when cold enough.

#include "OptimizeBase.h"
class CryostatImpl;

class CryostatPumping : public OptimizeBase {
public:    
    CryostatPumping(CryostatImpl &cryo)
      : OptimizeBase("CryostatPumping"),
        Cryostat_m(cryo)
        { reset(); }

    virtual ~CryostatPumping()
      {}

    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(
        float switchTemperature4k = 5,	    // below what temperatures to close the gate valve, etc. 
        float switchTemperature15k = 30,    // ""
        int cycleTimeSec = 20,              // how often to check.
        int timeoutSec = 172800);           // give up after this much time (172800 sec = 2 days).
    ///< start the cryostat pumping process
    
    virtual void stop();
    ///< stop the cryostat pumping process
    
protected:    
    virtual void optimizeAction();

private:

    CryostatImpl &Cryostat_m;

    float switchTemperature4k_m;
    float switchTemperature15k_m;
    int cycleTimeSec_m;
    int timeoutSec_m;
    int elapsedSec_m;
    bool enable_m;
};

#endif /*CRYOSTATPUMPING_H_*/
