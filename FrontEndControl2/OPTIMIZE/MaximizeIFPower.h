#ifndef MAXIMIZEIFPOWER_H_
#define MAXIMIZEIFPOWER_H_
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
/// \brief An optimization procedure which finds the maximum IF power while adjusting the LO power or mixer voltage.

#include "OptimizeBase.h"
#include "Maximizer.h"
#include "CONFIG/LookupTables.h"
using namespace FEConfig;
#include <iostream>
class ColdCartImpl;
class WCAImpl;

class MaximizeIFPower : public OptimizeBase, public Maximizer {
public:    
    MaximizeIFPower(ColdCartImpl &ColdCart, WCAImpl &WCA)
      : OptimizeBase("MaximizeIFPower"),
        Maximizer(),
        coldCart_m(ColdCart),
        WCA_m(WCA),
        dataFile_mp(NULL)
        { reset(); } 
    
    virtual ~MaximizeIFPower()
      { delete dataFile_mp; }
    
    virtual void reset();
    ///< reset all state to initial/just constructed EXCEPT for MixerParams, PowerAmpParams results
    ///<   -- those are cleared using resetParamsResult().
    
    bool start(double freqLO,        ///< for which frequency to store the results. 
               bool doPol0,          ///< true means optimize pol0
               bool doPol1,          ///< true means optimize pol1
               float startVJ1,       ///< initial setting for SIS1 junction voltage
               float startVJ2,       ///< initial setting for SIS2 junction voltage
               float startVD);       ///< initial setting for PA drain voltage.
    ///< start the optimization procedure.

    virtual void requestStop()
      { stopMaximizer();
        OptimizeBase::requestStop(); }
    ///< abort the measurement

    bool setIFPower(int pol, float powerSb1, float powerSb2);
    ///< called by main thread to feed IF power readings into the optimization process.
    
    void resetParamsResult()
      { mixerParams_m.clear();
        paParams_m.clear(); }
    ///< clear the optimized parameters results.

    const MixerParams &getMixerParamsResult() const
      { return mixerParams_m; }
    ///< get the resulting optimized mixer parameters vs. LO frequency.

    const PowerAmpParams &getPowerAmpParamsResult() const
      { return paParams_m; }
    ///< get the resulting optimized PA parameters vs. LO frequency.
    
protected:
    virtual bool setControlValue(float control);
    ///< Called by the Maximizer base class to set the independent control variable.
    ///< If implementation returns false, the maximize funtion returns false.  
    
    virtual bool requestDependentValue(float &depVal);
    ///< Called by the Maximizer base class to request the value of the dependent variable.
    ///< Implementation may provide a result right away and return true 
    ///< or return false and provide a value later using setDependentValue().
    
    virtual void optimizeAction();
    ///< called by the OptimizeBase worker thread.

    virtual void exitAction(bool success);
    ///< called by OptimizeBase prior to worker thread exit.

private:
    ColdCartImpl &coldCart_m;   ///< the cold cartridge to operate
    WCAImpl &WCA_m;             ///< the WCA to operate.
    double freqLO_m;            ///< what LO frequency applies to the optimization in-progress. 
    bool doPol0_m;              ///< true means optimize pol0
    bool doPol1_m;              ///< true means optimize pol1
    int currentPol_m;           ///< polarization currently being optimized.
    float startVJ1_m;           ///< starting value for SIS1 junction voltage
    float startVJ2_m;           ///< starting value for SIS2 junction voltage
    float startVD_m;            ///< starting value for PA drain voltage
    float VJ1_m;                ///< the current setting for SIS1 junction voltage
    float VJ2_m;                ///< the current setting for SIS2 junction voltage
    float VD_m;                 ///< the current setting for PA drain voltage
    float VJ01opt_m;
    float VJ02opt_m;
    float VJ11opt_m;
    float VJ12opt_m;
    float VD0opt_m;
    float VD1opt_m;
    float powerSb1_m;           ///< most recent SB1 power level
    float powerSb2_m;           ///< most recent SB2 power level
    short progress_m;           ///< local variable for progress bar percentage.

    static const short maxIter_m; ///< maximum iterations for the optimizer loop.

    enum OperationPhases {
        /// phases of the maximization process:
        //[3]  Phase 1A: Coarse LO Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as LO varies. Vd search step = 0.03V, Direction: Positive. Power Meter Averaging = 32. Log Values.
        //[4]  Phase 1B: Coarse SIS2 Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as Vj varies. Vj search step = 0.1V, Direction: Positive. Power Meter Averaging = 32.  Log Values.
        //[5]  Phase 1C: Coarse SIS1 Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as Vj varies. Vj search step = 0.1V, Direction: Positive. Power Meter Averaging = 32.  Log Values.
        //[6]  Phase 2A: Medium LO Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as LO varies. Vd search step = 0.01V, Direction: Negative. Power Meter Averaging = 32.  Log Values.
        //[7]  Phase 2B: Medium SIS2 Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as Vj varies. Vj search step = 0.04V, Direction: Negative. Power Meter Averaging = 32.  Log Values.
        //[8]  Phase 2C: Medium SIS1 Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as Vj varies. Vj search step = 0.04V, Direction: Negative. Power Meter Averaging = 32.  Log Values.
        //[9]  Phase 3A: Medium LO Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as LO varies. Vd search step = 0.01V, Direction: Negative. Power Meter Averaging = 32.  Log Values.
        //[10] Phase 3B: Fine SIS2 Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as Vj varies. Vj search step = 0.02V, Direction: Negative. Power Meter Averaging = 32.  Log Values.
        //[11] Phase 3C: Fine SIS1 Search: Maximize Sum of Pif(SIS1) and Pif(SIS2) as Vj varies. Vj search step = 0.02V, Direction: Negative. Power Meter Averaging = 32.  Log Values.
        OP_PHASE_NONE,      ///< startup/reset phase.
        OP_PHASE_1A,
        OP_PHASE_1B,
        OP_PHASE_1C,
        OP_PHASE_2A,
        OP_PHASE_2B,
        OP_PHASE_2C,
        OP_PHASE_3A,
        OP_PHASE_3B,
        OP_PHASE_3C,
        OP_PHASE_POL_DONE   ///< final state for logging results for a single pol
    } opPhase_m, failPhase_m;

    bool optimizeSinglePol(int pol);
    ///< performs the optimization of a single pol.

    bool performPhase(OperationPhases phase, float step, bool directionUp, int averaging);
    ///< perform the specified phase of the optimization.
    
    void logOutput(bool printHeader = false);
    ///< add a line to the optimization output log.

    MixerParams mixerParams_m;  ///< where to store the resulting mixer parameters vs. frequency.
    PowerAmpParams paParams_m;  ///< where to store the resulting PA parameters vs. frequency.
    std::ostream *dataFile_mp;
};



#endif /*MAXIMIZEIFPOWER_H_*/
