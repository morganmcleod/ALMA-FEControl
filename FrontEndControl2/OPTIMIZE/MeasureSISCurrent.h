#ifndef MEASUREIJVSSISMEGNET_H_
#define MEASUREIJVSSISMEGNET_H_
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
/// \brief Worker thread object to measure the junction current while sweeping the SIS magnet current.

#include "OptimizeBase.h"
#include <iostream>
class XYPlotArray;
class ColdCartImpl;

class MeasureSISCurrent : public OptimizeBase {
public:    
    MeasureSISCurrent(ColdCartImpl &ColdCart, XYPlotArray &target)
      : OptimizeBase("MeasureSISCurrent"),
        coldCart_m(ColdCart),
        data_m(target),
        dataFile_mp(NULL)
        { reset(); }

    virtual ~MeasureSISCurrent()
      { delete dataFile_mp; }
    
    void reset();
    ///< reset all state to initial/just constructed.
    
    bool start(int pol, int sbMixer,
               float IMagStart, float IMagStop, float IMagStep,
               float VJLow, float VJHigh, float VJStep, int repeatCount = 1); 
    
protected:    
    virtual void optimizeAction();

private:
    void readData(int VJSteps, bool printHeader = false);
    
    ColdCartImpl &coldCart_m;

    int pol_m;              ///< which polarization to measure: 0 or 1.
    int sbMagnet_m;         ///< which SIS magnet to sweep: 1 or 2 indicating SIS1/SIS2 or -1 indicating both (or band-specific modes.)
    int sbMixer_m;          ///< which SIS mixer to measure: 1 or 2 indicating SIS1/SIS2 or -1 indicating both.
    float VJ1Nom_m;         ///< cached SIS1 voltage to return to after the measurement.
    float VJ2Nom_m;         ///< cached SIS2 voltage.
    float IMag1Nom_m;       ///< cached SIS1 magnet current to return to after the measurement.
    float IMag2Nom_m;       ///< cached SIS2 magnet current.
    float IMagStart_m;      ///< start of magnet current range to sweep
    float IMagStop_m;       ///< end of magnet range
    float IMagStep_m;       ///< magnet current step size
    float IMagSet_m;        ///< instantaneous magnet current setting in force
    float VJLow_m;          ///< start of SIS voltage range to sweep
    float VJHigh_m;         ///< end of SIS range
    float VJStep_m;         ///< SIS voltage step size
    int repeatCount_m;      ///< how many times to repeat the measurement
    float progress_m;       ///< value between 0 and 100 for the progress bar.

    XYPlotArray &data_m;    ///< plot data to return
    std::ostream *dataFile_mp;  ///< output data file
};

#endif /*MEASUREIJVSSISMEGNET_H_*/
