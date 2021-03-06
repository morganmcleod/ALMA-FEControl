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
#include "CartAssembly.h"
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
    ca_m.useColdCart() -> stopIVCurve();
    OptimizeBase::requestStop();
}


void MeasureIVCurve::optimizeAction() {
    LOG(LM_INFO) << "MeasureIVCurve::optimizeAction pol=" << pol_m << " sb=" << sb_m << fixed << setprecision(3) 
        << " VJlow=" << VJlow_m << " VJhigh=" << VJhigh_m << " VJstep=" << VJstep_m << " repeatCount=" << repeatCount_m << endl;
    

    bool success = true;

    ca_m.pauseMonitor(false, true, "Measure I-V curve.");

    int iter = 0;
    while (iter < repeatCount_m && !stopRequested()) {
        ++iter;
        if (pol_m == 0 || pol_m == -1) {
        	if (sb_m == 1 || sb_m == -1)
        		success &= actionImpl(0, 1);

        	if (sb_m == 2  || sb_m == -1)
        		success &= actionImpl(0, 2);
        }
        if (pol_m == 1 || pol_m == -1) {
			if (sb_m == 1 || sb_m == -1)
				success &= actionImpl(1, 1);

			if (sb_m == 2  || sb_m == -1)
				success &= actionImpl(1, 2);
		}
	}
    ca_m.pauseMonitor(false, false);
    setFinished(success);
}

bool MeasureIVCurve::actionImpl(int pol, int sb) {
    bool measError = false;
    bool fileError = false;

    ColdCartImpl *cca = ca_m.useColdCart();
    setProgress(0);

	if (!ca_m.measureIVCurveSingleSynchronous(pol, sb, VJlow_m, VJhigh_m, VJstep_m))
		measError = true;
	else {
		SLEEP(3000);    // TODO:  this sleep is a hack to allow the GUI to grab the trace data.

		// save the I-V curve text data file:
		if (!cca -> saveIVCurveData(ca_m.getXYData(), logDir_m, pol, sb))
			fileError = true;
	}
	return !measError && !fileError;
}

void MeasureIVCurve::exitAction(bool success) {
    if (success)
        setStatusMessage(true, "MeasureIVCurve: finished successfully.");
    else if (stopRequested())
        setStatusMessage(false, "MeasureIVCurve: process stopped.");
    else
        setStatusMessage(false, "MeasureIVCurve: failed.");
}

