#ifndef MeasureIFPower_H_
#define MeasureIFPower_H_
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
/// \brief Worker thread object to measure the IF output power while sweeping other parameters.

#include "OptimizeBase.h"
#include "XYPlotArray.h"
#include <iostream>
class ColdCartImpl;
class WCAImpl;

class MeasureIFPower : public OptimizeBase {
public:    
    MeasureIFPower(ColdCartImpl &ColdCart, WCAImpl &WCA, XYPlotArray &target)
      : OptimizeBase("MeasureIFPower"),
        coldCart_m(ColdCart),
        WCA_m(WCA),
        data_m(target),
        dataFile_mp(NULL)
        { reset(); }

    virtual ~MeasureIFPower()
      { delete dataFile_mp; }
    
    void reset();
    ///< reset all state to initial/just constructed.
    
    bool startVsVJ(int pol, int sb, const std::string &description,
                   float VJStart, float VJStop, float VJStep,
                   bool doYFactor, int repeatCount = 1); 
    ///< start a measurment of IF Power vs SIS junction voltage.
    
    bool startVsVD(int pol, const std::string &description,
                   float VDStart, float VDStop, float VDStep,
                   bool doYFactor, int repeatCount = 1);
    ///< start a measurment of IF Power vs LO PA drain voltage.

    bool setIFPower(int pol, float powerSb1, float powerSb2);
    ///< called by main thread to feed IF power readings into the optimization process.

protected:    
    virtual void optimizeAction();
    ///< worker thread calls this function repeatedly until done.

    virtual void exitAction(bool success);
    ///< called by OptimizeBase prior to worker thread exit.

private:
    void readData(bool printHeader = false);
    
    ColdCartImpl &coldCart_m;   ///< the cold cartridge to operate
    WCAImpl &WCA_m;             ///< the WCA to operate.
    enum { MODE_NONE, MODE_VJ, MODE_VD } mode_m;
                                ///< which type of measurement is being performed.
    int pol_m;                  ///< Polarization to measure
    int sb_m;                   ///< Sideband (1,2) to vary voltage on.  -1 means do both.
    bool b6SweepMode_m;         ///< For band 6 sweep SIS1 voltage to the negative of the polarity of SIS2.
    float VJNom1_m;             ///< SIS1 voltage prior to starting measurement.
    float VJNom2_m;             ///< SIS2 voltage prior to starting measurement.
    float VDNom_m;              ///< PA drain voltage prior to starting measurement.
    float VStart_m;             ///< One end of range for parameter to sweep.  
    float VStop_m;              ///< Other end of range for parameter to sweep.
    float VStep_m;              ///< Step size for parameter to sweep.
    float V_m;                  ///< Current value of parameter to sweep.
    float powerSb1_m;           ///< most recent SB1 power level
    float powerSb2_m;           ///< most recent SB2 power level
    bool doYFactor_m;           ///< if true measure with both the hot and cold load and compute Y factor.
    int repeatCount_m;          ///< how many times to repeat the measurement.
    int iter_m;                 ///< which repetition is currently running.
    bool coldHot_m;             ///< which chopper position is currently measuring.  false=cold.
    int noReadingCount_m;       ///< This increments while waiting for power meter readings.
                                ///< If it reaches a large value the process is aborted.
    bool waitForPowerReading_m; ///< True if we are waiting for the next power meter readings.
    float progress_m;           ///< progress bar value.
    float progressIncrement_m;  ///< how much to increment the progress bar at each step.
    XYPlotArray yFactorData_m;  ///< place to cache pCold values when measuring Y-factor
    int dataIndex_m;            ///< current index into the yFactorData array.
    XYPlotArray &data_m;        ///< the output data array
    std::ostream *dataFile_mp;  ///< the output log file.
};

#endif /*MeasureIFPower_H_*/
