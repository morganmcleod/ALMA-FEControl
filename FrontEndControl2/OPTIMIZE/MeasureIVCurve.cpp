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

#include "MeasureIVCurve.h"
#include "XYPlotArray.h"
#include "logger.h"
#include "setTimeStamp.h"
#include "stringConvert.h"
#include "ColdCartImpl.h"
#include <fstream>
#include <iomanip>
using namespace std;

void MeasureIVCurve::reset() {
    pol_m = sb_m = -1;
    VJlow_m = VJhigh_m = VJstep_m = 0;
    repeatCount_m = 0;
    data_m.clear(); 
    if (dataFile_mp) {
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
}

bool MeasureIVCurve::start(int pol, int sb, float VJlow, float VJhigh, float VJstep, int repeatCount) {
    pol_m = pol;
    sb_m = sb;
    VJlow_m = VJlow;
    VJhigh_m = VJhigh;
    VJstep_m = VJstep;
    repeatCount_m = repeatCount;
    string msg("MeasureIVCurve: process started.");
    setStatusMessage(true, msg);
    return OptimizeBase::startWorkerThread();
} 

void MeasureIVCurve::requestStop() {
    coldCart_m.stopIVCurve();
    OptimizeBase::requestStop();
}


void MeasureIVCurve::optimizeAction() {
    LOG(LM_INFO) << "MeasureIVCurve::optimizeAction pol=" << pol_m << " sb=" << sb_m << fixed << setprecision(3) 
        << " VJlow=" << VJlow_m << " VJhigh=" << VJhigh_m << " VJstep=" << VJstep_m << " repeatCount=" << repeatCount_m << endl;
    
    bool success = true;
    int iter = 0;
    while (iter < repeatCount_m && !stopRequested()) {
        ++iter;
        setProgress(0);
        coldCart_m.pauseMonitor(true, "Measure I-V curve.");
        success = coldCart_m.measureIVCurve(data_m, pol_m, sb_m, VJlow_m, VJhigh_m, VJstep_m);
        coldCart_m.pauseMonitor(false);
    }
    setFinished(success);
}

void MeasureIVCurve::exitAction(bool success) {
    if (success)
        success = coldCart_m.saveIVCurveData(data_m, logDir_m, pol_m, sb_m);
    setEvent(FEMCEventQueue::EVENT_IVCURVE_DONE, coldCart_m.getBand(), -1, 0, 100);
    if (success)
        setStatusMessage(true, "MeasureIVCurve: finished successfully.");
    else if (stopRequested())
        setStatusMessage(false, "MeasureIVCurve: process stopped.");
    else
        setStatusMessage(false, "MeasureIVCurve: failed.");
}

