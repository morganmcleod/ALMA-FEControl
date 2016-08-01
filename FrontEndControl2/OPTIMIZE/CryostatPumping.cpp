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

#include "CryostatPumping.h"
#include "logger.h"
#include "stringConvert.h"
#include "CryostatImpl.h"
#include <iomanip>
using namespace std;

void CryostatPumping::reset() {
    switchTemperature_m = 0;
    cycleTimeSec_m = timeoutSec_m = elapsedSec_m = 0;
    enable_m = false;   
}

bool CryostatPumping::start(float switchTemperature, int cycleTimeSec, int timeoutSec) {
    switchTemperature_m = switchTemperature;
    cycleTimeSec_m = cycleTimeSec;
    timeoutSec_m = timeoutSec;
    elapsedSec_m = 0;
    enable_m = true;
    string msg("CryostatPumping: process started.");
    setStatusMessage(true, msg);
    return OptimizeBase::startWorkerThread();
}

void CryostatPumping::stop() {
    enable_m = false;
}

void CryostatPumping::optimizeAction() {    
    if (!enable_m) {
        string msg("CryostatPumping: process was stopped by user.");
        LOG(LM_INFO) << msg << endl;
        setStatusMessage(true, msg);
        setFinished(false);
        return;
    }    
    bool targetReached = false;
    CryostatImpl::Cryostat_t cryoTemps;
    if (Cryostat_m.getMonitorCryostat(cryoTemps)) {
        // Temperature is reached if the 4K stage temperature is below the target switchTemperature_m
        if (cryoTemps.cryostatTemperature0_value < switchTemperature_m) {
            // ... AND the 15K stage is below the target.
            if (cryoTemps.cryostatTemperature5_value < switchTemperature_m)
                targetReached = true;
            // TODO: for each stage, remove the highest and lowest and then average the rest.
        }
        LOG(LM_INFO) << fixed << setprecision(2) << "CryostatPumping: 4Kstage=" << cryoTemps.cryostatTemperature0_value
            << " 15Kstage=" << cryoTemps.cryostatTemperature5_value << endl;
    }
    if (!targetReached) {
        if (elapsedSec_m > timeoutSec_m) {
            string msg("CryostatPumping: Timed out after ");
            msg += to_string(elapsedSec_m);
            msg += " seconds.";
            LOG(LM_ERROR) << msg << endl;
            setStatusMessage(false, msg);
            setFinished(false);
        } else {
            for (int t=0; t < cycleTimeSec_m && enable_m; ++t) {
                SLEEP(1000);
                ++elapsedSec_m;
            }
        }
        return;
    }
    {
        string msg("CryostatPumping: Target temperature reached.  Closing gate valve...");
        LOG(LM_INFO) << msg << endl;
        setStatusMessage(true, msg);
    }    
    Cryostat_m.CryostatImplBase::gateValveState(0);

    int retries = 20;
    while (retries > 0 && Cryostat_m.gateValveState() != 0)
        SLEEP(1000);

    bool doStopBackingPump = true;
    if (Cryostat_m.gateValveState() != 0) {
        string msg("CryostatPumping ERROR: Gate valve did not close.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        // temporary: leave backing pump on so that operator can monitor the final state:
        doStopBackingPump = false;
        //setFinished(false);
        //return;
    }
    LOG(LM_INFO) << "CryostatPumping: Closing solenoid valve..." << endl;
    Cryostat_m.CryostatImplBase::solenoidValveState(0);

    retries = 10;
    while (retries > 0 && Cryostat_m.solenoidValveState() != 0)
        SLEEP(1000);

    if (Cryostat_m.gateValveState() != 0) {
        string msg("CryostatPumping ERROR: Solenoid valve did not close.  Stopping.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        setFinished(false);
        return;
    }
    LOG(LM_INFO) << "CryostatPumping: Stopping turbo pump..." << endl;
    Cryostat_m.CryostatImplBase::turboPumpEnable(false);
    setEvent(FEMCEventQueue::EVENT_PROGRESS, 0, -1, 0, 50);
    LOG(LM_INFO) << "CryostatPumping: Pausing for " << cycleTimeSec_m << " seconds..." << endl;
    for (int t=0; t < cycleTimeSec_m && enable_m; ++t)
        SLEEP(1000);
    
    if (!enable_m)
        LOG(LM_INFO) << "CryostatPumping: final pause was stopped by user." << endl;
    if (doStopBackingPump) {
        LOG(LM_INFO) << "CryostatPumping: Stopping backing pump..." << endl;
        Cryostat_m.CryostatImplBase::backingPumpEnable(false);
        {
            string msg("CryostatPumping: finished successfully.");
            LOG(LM_INFO) << msg << endl;
            setStatusMessage(true, msg);
        }
    } else {
        // temporary, not stopping backing pump when gate valve failed:
        LOG(LM_INFO) << "CryostatPumping ERROR: Not stopping backing pump due to gate valve failure." << endl;
        string msg("CryostatPumping: finished; Gate valve ERROR.");
        LOG(LM_INFO) << msg << endl;
        setStatusMessage(true, msg);
    }
    setEvent(FEMCEventQueue::EVENT_PROGRESS, 0, -1, 0, 100);
    setEvent(FEMCEventQueue::EVENT_CRYO_PUMP_DONE, 0, -1, 0, 100, "Cryostat pumping process finished.");
    setEvent(FEMCEventQueue::EVENT_PROGRESS, 0, -1, 0, 0);
    setFinished(doStopBackingPump);
}

