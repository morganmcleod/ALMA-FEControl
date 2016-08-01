#ifndef MEASUREIVCURVE_H_
#define MEASUREIVCURVE_H_
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
/// \brief Worker thread object to measure the juction current while sweeping the junction voltage.

#include "OptimizeBase.h"
#include <iostream>
class XYPlotArray;
class ColdCartImpl;

class MeasureIVCurve : public OptimizeBase {
public:    
    MeasureIVCurve(ColdCartImpl &ColdCart, XYPlotArray &target)
      : OptimizeBase("MeasureIVCurve"),
        coldCart_m(ColdCart),
        data_m(target),
        dataFile_mp(NULL)
        { reset(); }

    virtual ~MeasureIVCurve()
      { delete dataFile_mp; }

    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(int pol, int sb, float VJlow, float VJhigh, float VJstep, int repeatCount = 1);
    ///< Start an I-V curve measurement

    virtual void requestStop();

protected:    
    virtual void optimizeAction();

    virtual void exitAction(bool success);

private:

    bool actionImpl(); 

    ColdCartImpl &coldCart_m;

    int pol_m;
    int sb_m;
    float VJlow_m;
    float VJhigh_m;
    float VJstep_m;
    int repeatCount_m;
    
    XYPlotArray &data_m; 
    std::ostream *dataFile_mp;
};

#endif /*MEASUREIVCURVE_H_*/
