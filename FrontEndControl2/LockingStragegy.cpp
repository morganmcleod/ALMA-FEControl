/*
 * LockingStragegy.cpp
 *
 *  Created on: Oct 20, 2021
 *      Author: mmcleod
 */

#include "LockingStrategy.h"
#include "CartAssembly.h"
#include "WCAImpl.h"
#include "logger.h"
#include <cmath>
using namespace std;

LPRImpl *LockingStrategy::LPR_p(NULL);

bool LOCK_Normal::lockPLL(CartAssembly &CA) const {
    WCAImpl *WCA_p = CA.useWCA();
    if (!WCA_p) {
        LOG(LM_ERROR) << "LOCK_Normal::lockPLL: WCA is null" << endl;
        return false;
    }

    double freqLO(0.0), freqREF(0.0);

    if (!CA.getLOFrequency(freqLO, freqREF) && freqLO > 0.0) {
        LOG(LM_ERROR) << "LOCK_Normal::lockPLL: LO frequency is 0" << endl;
        return false;
    }

    double freqFLOOG(0.0);
    int sbLock(-1);

    if (!CA.getLOLockSettings(freqFLOOG, sbLock)) {
        LOG(LM_ERROR) << "LOCK_Normal::lockPLL: failed getLOLockSettings" << endl;
        return false;
    }

    LOG(LM_INFO) << "--> LOCK_Normal..." << endl;

    double freqYIG(0.0);
    int coarseYIG = CA.getCoarseYIG(freqYIG, freqREF, freqLO, freqFLOOG, sbLock);

    LOG(LM_DEBUG) << "LOCK_Normal::lockPLL:"
                  << " freqLO=" << freqLO << " freqREF=" << freqREF
                  << " freqFLOOG=" << freqFLOOG << " sbLock=" << sbLock
                  << " coarseYIG=" << coarseYIG << endl;

    if (coarseYIG < 0) {
        LOG(LM_ERROR) << "LOCK_Normal::lockPLL: getCoarseYIG() < 0" << endl;
        return false;
    }

    // lock search window is +/- 50 MHz:
    const double window(0.05);

    // lock search step size is 3 MHz:
    const double step(0.003);

    // compute YIG step size:
    double stepYIG = (CA.getWCAConfig().FHIYIG_m - CA.getWCAConfig().FLOYIG_m) / 4095.0;

    // lock search window in YIG steps:
    int windowSteps = (int) (window / stepYIG) + 1;

    // lock step size in YIG steps:
    int stepSize = (int) (step / stepYIG);
    if (stepSize == 0)
        stepSize = 1;

    CA.pauseMonitor(true, true, "Searching for lock.");

//  bool success = WCA_p -> pllLOCK_NORMAL(coarseYIG, windowSteps, stepSize, false);
    int loLimit = coarseYIG - windowSteps;
    if (loLimit < 0)
        loLimit = 0;
    int hiLimit = coarseYIG + windowSteps;
    if (hiLimit > 4095)
        hiLimit = 4095;
    int tryCoarseYIG = coarseYIG;
    int stepSleep = 2;  // ms - YTO moves to within 10% of commanded value after 1 ms.
    bool lock = false;
    bool error = false;

    while (!lock && !error) {
        WCA_p -> ytoCoarseTuneWithTrace(coarseYIG, "LOCK_Normal::lockPLL: initial");
        // give the YTO time to slew:
        SLEEP(stepSleep);

        // initialize stepping size and direction:
        int step = stepSize;

        // inner loop searches for lock using standard or alternateLockTest strategy:
        while (!lock && !error) {
            lock = WCA_p -> testPLLLockDetectVoltage();
            if (!lock) {
                // no lock so move the YTO to the next try frequency:
                tryCoarseYIG = coarseYIG + step;
                if (tryCoarseYIG >= loLimit && tryCoarseYIG <= hiLimit) {
                    WCA_p -> pllNullLoopIntegrator(true);
                    WCA_p -> ytoCoarseTuneWithTrace(tryCoarseYIG, "WCAImpl::pllLOCK_NORMAL");
                    SLEEP(stepSleep);
                    WCA_p -> pllNullLoopIntegrator(false);
                    SLEEP(stepSleep);
                }
                // step increases by stepsize or alternates sign each iteration
                //  to expand the search outward from the starting value:
                if (step > 0)
                    step = -step;
                else {
                    step = -step + stepSize;
                    if (step > windowSteps)
                        error = true;
                }
            }
        }
    }
    if (lock)
        coarseYIG = tryCoarseYIG;

    if (lock) {
        // clear the unlock detect latch so we will see if lock is lost from here on:
        WCA_p -> pllClearUnlockDetectLatch();
        lock = WCA_p -> adjustPLL(0.0);
    }
    CA.pauseMonitor(false, false);
    return lock;
}

