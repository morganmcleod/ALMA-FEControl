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

#include "OptimizeLOPowerAmp.h"
#include "logger.h"
#include "stringConvert.h"
#include "CartAssembly.h"

#include <math.h>
#include <iomanip>
using namespace std;

void OptimizeLOPowerAmp::reset() {
    freqLO_m = 0;
    VD0_m = VD1_m = targetIJ01_m = targetIJ11_m = 0;
    repeatCount_m = 0;
    paParams_m.clear();
}

bool OptimizeLOPowerAmp::start(double freqLO, float VD0, float targetIJ01, float VD1, float targetIJ11, int repeatCount) {
    freqLO_m = freqLO;
    VD0_m = VD0;
    targetIJ01_m = targetIJ01;
    VD1_m = VD1;
    targetIJ11_m = targetIJ11;
    repeatCount_m = repeatCount;
    string msg("OptimizeLOPowerAmp: process started.");
    setStatusMessage(true, msg);
    return OptimizeBase::startWorkerThread();
}

void OptimizeLOPowerAmp::optimizeAction() {
    bool success = true;
    const bool verboseLogging = true;
    cartAssembly_m.pauseMonitor(true, true, "Optimize LO power amp.");
    float VD0_temp, VD1_temp;
    int iter = 0;
    while (iter < repeatCount_m && !stopRequested()) {
        ++iter;
        setProgress(0);
        VD0_temp = VD0_m;

        if (!cartAssembly_m.adjustLOPASinglePol(0, VD0_temp, targetIJ01_m, verboseLogging))
            success = false;

        setProgress(50);
        VD1_temp = VD1_m;

        if (!cartAssembly_m.adjustLOPASinglePol(1, VD1_temp, targetIJ11_m, verboseLogging))
            success = false;

        setProgress(100);
    }
    if (success && !stopRequested()) {
        VD0_m = VD0_temp;  
        VD1_m = VD1_temp;
    }
    cartAssembly_m.pauseMonitor(false, false);
    setEvent(FEMCEventQueue::EVENT_PA_ADJUST_DONE, cartAssembly_m.getBand(), -1, 0, 100);
    if (success & !stopRequested()) {
        string msg("OptimizeLOPowerAmp: finished successfully.");
        setStatusMessage(true, msg);
    } else {
        string msg("OptimizeLOPowerAmp: process stopped.");
        setStatusMessage(false, msg);
        success = false;
    }
    setFinished(success);
}


