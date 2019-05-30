/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
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
*
*/

/************************************************************************
 * implementation class for the Warm Cartridge Assembly
 *----------------------------------------------------------------------
 */
 
#include "WCAImpl.h"
#include "FEMCEventQueue.h"
#include "logger.h"
#include "stringConvert.h"
using namespace FEConfig;
#include <math.h>
using namespace std;

namespace FrontEndLVWrapper {
    bool tryAlternateLockMethod = false;
    bool defeatNormalLockDetect = false;
};
using namespace FrontEndLVWrapper;

WCAImpl::WCAImpl(unsigned long channel, 
                 unsigned long nodeAddress,
                 const std::string &name,
                 int port, 
                 int band, 
                 const std::string &ESN)
  : WCAImplBase(name, port),
    band_m(band)
{ 
    reset();
    setESN(ESN);
    WCAImplBase::initialize(channel, nodeAddress);
}

const std::string &WCAImpl::getBandText(std::string &toFill) const {
    toFill = to_string(band_m);
    toFill = "band" + toFill;
    return toFill;
}

void WCAImpl::reset() {
    ytoCoarseTune_m = 0;
    pllSidebandLockSelect_m = false;
    pllNullLoopIntegrator_m = false;
    paEnable_m = false;
    LOEnable_m = true;
    photomixerEnable_m = false;
    paDrainVoltage_m[0] = paDrainVoltage_m[1] = 0.0;
    paGateVoltage_m[0] = paGateVoltage_m[1] = 0.0;
    amcDrainEVoltage_m = amcGateEVoltage_m = 0.0;
    isLockedLO_m = false;
    LockDetectStrategy_m = LOCK_DETECT_VOLTAGE;
    resetPllCorrectionVoltageStats();
}

void WCAImpl::queryCartridgeState() {
    int status;
    ytoCoarseTune_m = ytoCoarseTune();
    pllSidebandLockSelect_m = pllSidebandLockSelect();
    pllNullLoopIntegrator_m = pllNullLoopIntegrator();
    status = syncMonitorWithRetry(paPol0DrainVoltage_RCA + 0x10000, paDrainVoltage_m[0]);
    status = syncMonitorWithRetry(paPol1DrainVoltage_RCA + 0x10000, paDrainVoltage_m[1]);
    status = syncMonitorWithRetry(paPol0GateVoltage_RCA + 0x10000, paGateVoltage_m[0]);
    status = syncMonitorWithRetry(paPol1GateVoltage_RCA + 0x10000, paGateVoltage_m[1]);
    paEnable_m = (paDrainVoltage_m[0] > 0.2 || paDrainVoltage_m[1] > 0.2);
    photomixerEnable_m = photomixerEnable();
    
    // Check whether locked:
    isLockedLO_m = testNormalLockDetect();
    LockDetectStrategy_m = LOCK_DETECT_VOLTAGE;

    LOG(LM_INFO) << "WCAImpl::queryCartridgeState: port=" << port_m << " band=" << band_m << " YTO=" << ytoCoarseTune_m
        << " sbLock=" <<  ((pllSidebandLockSelect_m == 1) ? "1-aboveRef" : "0-belowRef") 
        << " isLocked=" << isLockedLO_m
        << " nullInt=" << pllNullLoopIntegrator_m << " paEnable=" << paEnable_m << " photomixerEnable=" << photomixerEnable_m
        << " VD0=" << paDrainVoltage_m[0] << " VD1=" << paDrainVoltage_m[1]
        << " VG0=" << paGateVoltage_m[0] << " VG1=" << paGateVoltage_m[1] << endl;
}

//-------------------------------------------------------------------------------------------------
// YTO and PLL control settings

void WCAImpl::ytoCoarseTune(unsigned short val) {
    ytoCoarseTune_m = val;
    WCAImplBase::ytoCoarseTune(val);
    resetPllCorrectionVoltageStats();
}

void WCAImpl::ytoCoarseTuneWithTrace(unsigned short coarseYIG, const char *traceText) {
    ytoCoarseTune(coarseYIG);
    LOG(LM_DEBUG) << ((traceText) ? traceText : "") << " set COARSE_YIG tuning to " << coarseYIG << endl;
}

void WCAImpl::pllSidebandLockSelect(bool val) {
    pllSidebandLockSelect_m = val;
    WCAImplBase::pllSidebandLockSelect(val);
}

void WCAImpl::pllNullLoopIntegrator(bool val) {
    pllNullLoopIntegrator_m = val;
    WCAImplBase::pllNullLoopIntegrator(val);
} 

//-------------------------------------------------------------------------------------------------
// Lock detection and monitoring

const float WCAImpl::maxCVVariation(0.5);
const float WCAImpl::lowCVRange(-6.5);
const float WCAImpl::highCVRange(6.5);
const float WCAImpl::lowLockDetectVoltage(3.0);
const float WCAImpl::lowIFTotalPower(0.5);
const float WCAImpl::lowRefTotalPower(0.5);

float WCAImpl::pllLockDetectVoltage() {
    float val = WCAImplBase::pllLockDetectVoltage();
    if (FrontEndLVWrapper::defeatNormalLockDetect)
        return 0.0;
    else
        return val;
}

float WCAImpl::pllCorrectionVoltage() {
    float val = WCAImplBase::pllCorrectionVoltage();
    pllCorrectionVoltage_count++;
    if (val < pllCorrectionVoltage_min)
        pllCorrectionVoltage_min = val;
    if (val > pllCorrectionVoltage_max)
        pllCorrectionVoltage_max = val;
    return val;
}

void WCAImpl::resetPllCorrectionVoltageStats() {
    pllCorrectionVoltage_count = 0;
    pllCorrectionVoltage_min = 99.0;
    pllCorrectionVoltage_max = -99.0;
}

bool WCAImpl::testPLLCorrectionVoltageStats() const {
    return ((pllCorrectionVoltage_max - pllCorrectionVoltage_min) < maxCVVariation);
}

bool WCAImpl::testPLLUnlockDetect() {
    return WCAImplBase::pllUnlockDetectLatch();
}

void WCAImpl::clearUnlockDetect() {
    WCAImplBase::pllClearUnlockDetectLatch();
}

bool WCAImpl::testPLLCVInRange(const float &cv) const {
    return (lowCVRange <= cv && cv <= highCVRange);
}

bool WCAImpl::testPLLPowerLevels() {
    return (fabsf(pllRefTotalPower()) > lowRefTotalPower
          && fabsf(pllIfTotalPower()) > lowIFTotalPower);
}

bool WCAImpl::testPLLLockDetectVoltage() {
    return (pllLockDetectVoltage() > lowLockDetectVoltage);
}

bool WCAImpl::testNormalLockDetect() {
    return (testPLLLockDetectVoltage() && testPLLPowerLevels());
}

bool WCAImpl::testPLLLockDetectVoltage2X() {
    bool lock = testPLLLockDetectVoltage();
    SLEEP(3);
    lock &= testPLLLockDetectVoltage();
    return lock;
}

bool WCAImpl::test2XVoltageLockDetect() {
    return (testPLLLockDetectVoltage2X() && testPLLPowerLevels());
}

bool WCAImpl::testCorrVoltageLockDetect() {
    if (band_m < 9) {
        LOG(LM_ERROR) << "WCAImpl::testCorrVoltageLockDetect: only supported for band 9 and 10." << endl;
        return false;
    }
    if (!testPLLPowerLevels()) {
        LOG(LM_DEBUG) << "WCAImpl::testCorrVoltageLockDetect: failed testPLLPowerLevels" << endl;
        return false;
    }
    short yto = getYTOCoarseTuneSetting();
    float cv0 = pllCorrectionVoltage();
    float cv1, cv2;
    bool stepsOk(false), statsOk(false);
    // move the YTO to +2 steps, then +1, measuring CV at each step:
    if (testPLLCVInRange(cv0)) {
        LOG(LM_INFO) << "WCAImpl::testCorrVoltageLockDetect: yto=" << yto << " cv0=" << cv0 << endl;
        ytoCoarseTune(yto + 2);
        SLEEP(1);
        cv2 = pllCorrectionVoltage();
        ytoCoarseTune(yto + 1);
        SLEEP(1);
        cv1 = pllCorrectionVoltage();
        // Move back to starting YTO:
        ytoCoarseTune(yto);
        // When locked, the correction voltage decreases as the YTO tuning increases:
        stepsOk = (cv0 > cv1 && cv1 > cv2);
    }
    if (stepsOk) {
        // read several more times to collect stats on the variance of the CV:
        SLEEP(1);
        pllCorrectionVoltage();
        SLEEP(1);
        pllCorrectionVoltage();
        SLEEP(1);
        pllCorrectionVoltage();
        SLEEP(1);
        pllCorrectionVoltage();
        SLEEP(1);
        pllCorrectionVoltage();
        statsOk = testPLLCorrectionVoltageStats();
    }
    if (stepsOk) {
        LOG(LM_INFO) << "WCAImpl::testCorrVoltageLockDetect: cv1=" << cv1 << " cv2=" << cv2
                     << " success=" << (stepsOk && statsOk)
                     << " cvSize=" << pllCorrectionVoltage_count
                     << " cvMin=" << pllCorrectionVoltage_min
                     << " cvMax=" << pllCorrectionVoltage_max
                     << endl;
    }
    return stepsOk && statsOk;
}

bool WCAImpl::interrogateLock() {
    switch (LockDetectStrategy_m) {
        case LOCK_DETECT_CORR_V:
            return (testPLLCorrectionVoltageStats()
                         && !pllNullLoopIntegrator_m
                         && testPLLPowerLevels()
                         && testPLLCVInRange(pllCorrectionVoltage()));

        case LOCK_DETECT_2XVOLTAGE:
            return test2XVoltageLockDetect();

        case LOCK_DETECT_VOLTAGE:
        default:
            return testNormalLockDetect();
    }
}

bool WCAImpl::monitorLockForDisplay() const {
    if (fabsf(pllRefTotalPower_value) < lowRefTotalPower)
        return false;   // the ref total power is now too low.
    if (fabsf(pllIfTotalPower_value) < lowIFTotalPower)
        return false;   // the IF total power is now too low.

    switch (LockDetectStrategy_m) {
        case LOCK_DETECT_CORR_V:
            // previous lock was in the 'CORR_V' way.
            if (pllNullLoopIntegrator_value)
                return false;   // ...but now the PLL is defeated so it can't be considered locked.
            if (!testPLLCVInRange(pllCorrectionVoltage_value))
                return false;   // ...but now the PLL cv is out of range.
            if (!testPLLCorrectionVoltageStats()) {
                LOG(LM_DEBUG) << "WCAImpl::monitorLockForDisplay: no lock"
                             << " cvSize=" << pllCorrectionVoltage_count
                             << " cvMin=" << pllCorrectionVoltage_min
                             << " cvMax=" << pllCorrectionVoltage_max << endl;
                return false;   // ...but now the correction voltage variance is too large.
            }
            break;

        case LOCK_DETECT_VOLTAGE:
        case LOCK_DETECT_2XVOLTAGE:
        default:
            // we were previously locked in the 'normal' or '2XVOLTAGE' way.
            if (pllLockDetectVoltage_value < lowLockDetectVoltage)
                return false;   // ...but lock detect voltage has dropped below threshold.
            break;
    }
    // appears we are still locked:
    return true;
}

//-------------------------------------------------------------------------------------------------
// Locking procedure

bool WCAImpl::searchForLock(int &coarseYIG, int windowSteps, int stepSize) {
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

    isLockedLO_m = false;
    LockDetectStrategy_m = LOCK_DETECT_VOLTAGE;
    if (band_m == 1 || band_m == 2) {
        // Use the 2XVOLTAGE lock detect for bands 1 and 2:
        LockDetectStrategy_m = LOCK_DETECT_2XVOLTAGE;
        // Band1 seems to have a slower YTO:
        stepSleep = 10;
    }

    // outer loop tries normal lock search and then alternateLockTest for bands 9 & 10:
    while (!lock && !error) {
        ytoCoarseTuneWithTrace(coarseYIG, "WCAImpl::searchForLock initial");
        // give the YTO time to slew:
        SLEEP(stepSleep);
        LOG(LM_INFO) << "Searching for Lock with strategy " << LockDetectStrategy_m << "..." << endl;

        // initialize stepping size and direction:
        int step = stepSize;

        // inner loop searches for lock using standard or alternateLockTest strategy:
        while (!lock && !error) {
            switch (LockDetectStrategy_m) {
                case LOCK_DETECT_CORR_V:
                    lock = testCorrVoltageLockDetect();
                    break;

                case LOCK_DETECT_2XVOLTAGE:
                    lock = test2XVoltageLockDetect();
                    break;

                case LOCK_DETECT_VOLTAGE:
                default:
                    lock = testPLLLockDetectVoltage();
                    break;
            }

            if (!lock) {
                // no lock so move the YTO to the next try frequency:
                tryCoarseYIG = coarseYIG + step;
                if (tryCoarseYIG >= loLimit && tryCoarseYIG <= hiLimit) {
                    pllNullLoopIntegrator(true);
                    ytoCoarseTuneWithTrace(tryCoarseYIG, "WCAImpl::searchForLock");
                    SLEEP(stepSleep);
                    pllNullLoopIntegrator(false);
                    SLEEP(stepSleep);   // was clearUnlockDetect() but that shouldn't be needed inside the loop.
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
        // can we try again with the correction voltage strategy?
        if (!lock && LockDetectStrategy_m == LOCK_DETECT_VOLTAGE
                  && tryAlternateLockMethod)
        {
            // retry inner loop again:
            LockDetectStrategy_m = LOCK_DETECT_CORR_V;
            error = false;
        }
    }
    if (lock)
        coarseYIG = tryCoarseYIG;
    isLockedLO_m = lock;
    return isLockedLO_m;
}

bool WCAImpl::adjustPLL(float targetCorrVoltage) {
    LOG(LM_INFO) << "WCAImpl::adjustPLL targetCV=" << targetCorrVoltage << endl;

    int maxDist(50);
    int dist(0);
    double window(0.25);
    double hiThreshold = targetCorrVoltage + window;
    double loThreshold = targetCorrVoltage - window;

    float correctionVoltage = pllCorrectionVoltage();
    float voltageError = correctionVoltage - targetCorrVoltage;

    unsigned short coarseYIG = ytoCoarseTune();
    int tryCoarseYIG = coarseYIG;

    LOG(LM_INFO) << "WCAImpl::adjustPLL CV=" << correctionVoltage << " vError=" << voltageError << " coarseYIG=" << tryCoarseYIG << endl;

    if (tryCoarseYIG >= 0)
        ytoCoarseTuneWithTrace(tryCoarseYIG, "WCAImpl::adjustPLL initial");
    else
        return false;

    bool done = false;
    bool error = false;
    int retries = 50;
    while (!done && !error) {
        correctionVoltage = pllCorrectionVoltage();
        if (correctionVoltage >= loThreshold && correctionVoltage <= hiThreshold)
            done = true;
        else if (--retries <= 0) {
            done = true;
            LOG(LM_INFO) << "WCAImpl::adjustPLL too many retries." << endl;
        } else {
            voltageError = correctionVoltage - targetCorrVoltage;
            tryCoarseYIG += (voltageError > 0) ? 1 : -1;
            dist = tryCoarseYIG - coarseYIG;
            if (dist < 0)
                dist = -dist;
            if (dist > maxDist)
                error = true;
            else if (tryCoarseYIG >= 0 && tryCoarseYIG <= 4095)
                ytoCoarseTuneWithTrace(tryCoarseYIG, "WCAImpl::adjustPLL");
        }
    }

    if (!interrogateLock()) {
        string msg("WCAImpl ERROR: band ");
        msg += to_string(band_m);
        msg += " lost the lock while adjusting the PLL.";
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        error = true;
    } else
        // clear the unlock detect latch so we will see if lock is lost hereafter:
        clearUnlockDetect();
    return done && !error;
}

//-------------------------------------------------------------------------------------------------
// LO PA monitor and control:

const float WCAImpl::minSettingPAVD(0.0);
const float WCAImpl::maxSettingPAVD(2.5);
const float WCAImpl::minSettingPAVG(-0.85);
const float WCAImpl::maxSettingPAVG(0.15);

void WCAImpl::paPol0DrainVoltage(float val) {
    if (val < minSettingPAVD || val > maxSettingPAVD) {
        val = 0;
        string msg("WCAImpl ERROR: PA Pol0 drain voltage out of range.  Setting to zero.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
    }
    paDrainVoltage_m[0] = val;
    WCAImplBase::paPol0DrainVoltage(val);
}

void WCAImpl::paPol1DrainVoltage(float val) {
    if (val < minSettingPAVD || val > maxSettingPAVD) {
        val = 0;
        string msg("WCAImpl ERROR: PA Pol1 drain voltage out of range.  Setting to zero.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
    }
    paDrainVoltage_m[1] = val;
    WCAImplBase::paPol1DrainVoltage(val);
}

void WCAImpl::paPol0GateVoltage(float val) {
    if (val < minSettingPAVG || val > maxSettingPAVG) {
        val = 0;
        string msg("WCAImpl ERROR: PA Pol0 gate voltage out of range.  Setting to zero.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
    }
    paGateVoltage_m[0] = val;
    WCAImplBase::paPol0GateVoltage(val);
}

void WCAImpl::paPol1GateVoltage(float val) {
    if (val < minSettingPAVG || val > maxSettingPAVG) {
        val = 0;
        string msg("WCAImpl ERROR: PA Pol1 gate voltage out of range.  Setting to zero.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
    }
    paGateVoltage_m[1] = val;
    WCAImplBase::paPol1GateVoltage(val);
}

void WCAImpl::setPAEnable(bool val) {
    paEnable_m = val;
    if (!paEnable_m) {
        LOG(LM_INFO) << "WCAImpl::setPAEnable(false): Switching LO PA off for port=" << port_m << " band=" << band_m << endl;
        // turn everything off:
        paPol0DrainVoltage(0.0);
        paPol1DrainVoltage(0.0);
        paPol0GateVoltage(0.0);
        paPol1GateVoltage(0.0);
    }
}

void WCAImpl::setEnableLO(bool enable, bool setAMC, bool detune) {
    paEnable_m = LOEnable_m = enable;
    if (!enable) {
        // turn off the LO PA:
        WCAImplBase::paPol0DrainVoltage(0.0);
        WCAImplBase::paPol1DrainVoltage(0.0);
        // Set the null integrator so we can return to lock on enable:
        WCAImplBase::pllNullLoopIntegrator(true);
        if (detune) {
            // since there still might be leakage, detune the YTO to the far end of the band:
            WCAImplBase::ytoCoarseTune((unsigned short) (ytoCoarseTune_m < 2048) ? 4095 : 0);
        }
        if (setAMC && band_m == 3) {
            // turn off the AMC stage E:
            WCAImpl::setAMC(0.0, amcGateEVoltage_m);
        }
    } else {
        // set the AMC back where it was:
        if (setAMC && band_m == 3) {
            // turn back on the AMC stage E with the prior settings:
            WCAImpl::setAMC(amcDrainEVoltage_m, amcGateEVoltage_m);
        }
        // Set the YTO back where it was:
        WCAImplBase::ytoCoarseTune(ytoCoarseTune_m);
        // Reset the null integrator to its previous state.  Should re-lock if was locked before:
        WCAImplBase::pllNullLoopIntegrator(pllNullLoopIntegrator_m);
        // Turn on the LO PA:
        WCAImplBase::paPol0DrainVoltage(paDrainVoltage_m[0]);
        WCAImplBase::paPol1DrainVoltage(paDrainVoltage_m[1]);
        // Clear the unlock detect latch:
        clearUnlockDetect();
    }
}

void WCAImpl::setLOPower(int pol, float percent) {
    if (percent < 0.0)
        percent = 0;
    else if (percent > 100.0)
        percent = 100.0;
    if (pol == 0)    
        paPol0DrainVoltage(2.5 * percent / 100);
    else if (pol == 1)
        paPol1DrainVoltage(2.5 * percent / 100);
}

//-------------------------------------------------------------------------------------------------
// AMC control:

bool WCAImpl::setAMC(float VDE, float VGE) {
    if (band_m != 3) {
        LOG(LM_ERROR) << "WCAImpl: setAMC not allowed for band=" << band_m << endl;
        return false;
    }
    LOG(LM_INFO) << "WCAImpl::setAMC VDE=" << VDE << " VGE=" << VGE << " for port=" << port_m << " band=" << band_m << endl;
    amcDrainEVoltage_m = VDE;
    amcGateEVoltage_m = VGE;
    WCAImplBase::amcDrainEVoltage(amcDrainEVoltage_m);
    WCAImplBase::amcGateEVoltage(amcGateEVoltage_m);
    return true;
}

//-------------------------------------------------------------------------------------------------
// Photomixer monitor and control:

void WCAImpl::photomixerEnable(bool val) {
    photomixerEnable_m = val;
    LOG(LM_INFO) << "WCAImpl: photomixerEnable=" << photomixerEnable_m << " for port=" << port_m << " band=" << band_m << endl;
    WCAImplBase::photomixerEnable(val);
}

//-------------------------------------------------------------------------------------------------
// retrieve monitor values:

bool WCAImpl::getMonitorYTO(YTO_t &target) const {
    memset(&target, 0, sizeof(target));
    target.ytoCoarseTune_value = ytoCoarseTune_value;
    return true;
}
    
bool WCAImpl::getMonitorPhotomixer(Photomixer_t &target) const {
    memset(&target, 0, sizeof(target));
    target.photomixerEnable_value = photomixerEnable_value;
    target.photomixerVoltage_value = photomixerVoltage_value;
    target.photomixerCurrent_value = photomixerCurrent_value;
    return true;
};

bool WCAImpl::getMonitorPLL(PLL_t &target) const {
    memset(&target, 0, sizeof(target));
    
    if (minimalMonitoring_m) {
        // if in minimal monitoring mode, just return the latest values.
        target.pllLockDetectVoltage_value = pllLockDetectVoltage_value;
        target.pllCorrectionVoltage_value = pllCorrectionVoltage_value;
        target.pllRefTotalPower_value = pllRefTotalPower_value;
        target.pllIfTotalPower_value = pllIfTotalPower_value;
        target.pllUnlockDetectLatch_value = pllUnlockDetectLatch_value;
    } else {
        // else, make a non-const copy of this so we can call real-time monitors directly.
        // TODO: fix these constness hacks.
        WCAImpl *nonConstThis = const_cast<WCAImpl *>(this);    
        target.pllLockDetectVoltage_value = nonConstThis -> pllLockDetectVoltage();
        target.pllCorrectionVoltage_value = nonConstThis -> pllCorrectionVoltage();
        target.pllRefTotalPower_value = nonConstThis -> pllRefTotalPower();
        target.pllIfTotalPower_value = nonConstThis -> pllIfTotalPower();
        target.pllUnlockDetectLatch_value = nonConstThis -> pllUnlockDetectLatch();
    }
    target.pllLock_value = monitorLockForDisplay();

    target.pllAssemblyTemp_value = pllAssemblyTemp_value;
    target.pllYTOHeaterCurrent_value = pllYTOHeaterCurrent_value;
    target.pllLoopBandwidthSelect_value = pllLoopBandwidthSelect_value;
    target.pllSidebandLockSelect_value = pllSidebandLockSelect_value;
    target.pllNullLoopIntegrator_value = pllNullLoopIntegrator_value;
    return true;
};
    
bool WCAImpl::getMonitorAMC(AMC_t &target) const {
    memset(&target, 0, sizeof(target));
    target.amcGateAVoltage_value = amcGateAVoltage_value;
    target.amcDrainAVoltage_value = amcDrainAVoltage_value;
    target.amcDrainACurrent_value = amcDrainACurrent_value;
    target.amcGateBVoltage_value = amcGateBVoltage_value;
    target.amcDrainBVoltage_value = amcDrainBVoltage_value;
    target.amcDrainBCurrent_value = amcDrainBCurrent_value;
    target.amcGateEVoltage_value = amcGateEVoltage_value;
    target.amcDrainEVoltage_value = amcDrainEVoltage_value;
    target.amcDrainECurrent_value = amcDrainECurrent_value;
    target.amcMultiplierDCounts_value = amcMultiplierDCounts_value;
    target.amcMultiplierDCurrent_value = amcMultiplierDCurrent_value;
    target.amcSupplyVoltage5V_value = amcSupplyVoltage5V_value;
    return true;
};

bool WCAImpl::getMonitorPA(PA_t &target) const {
    memset(&target, 0, sizeof(target));
    target.paPol0GateVoltage_value = paPol0GateVoltage_value;
    target.paPol0DrainVoltage_value = paPol0DrainVoltage_value;
    target.paPol0DrainCurrent_value = paPol0DrainCurrent_value;
    target.paPol1GateVoltage_value = paPol1GateVoltage_value;
    target.paPol1DrainVoltage_value = paPol1DrainVoltage_value;
    target.paPol1DrainCurrent_value = paPol1DrainCurrent_value;
    target.paSupplyVoltage3V_value = paSupplyVoltage3V_value;
    target.paSupplyVoltage5V_value = paSupplyVoltage5V_value;
    return true;
};

//-------------------------------------------------------------------------------------------------
// Thermal Log interface:

void WCAImpl::appendThermalLogHeaderImpl(std::string &target) {
    target += "\tPLL";
}

void WCAImpl::appendThermalLog(std::string &target) const {
    char buf[20];    
    sprintf(buf, "\t%.2f", pllAssemblyTemp_value);
    target += buf;
}

void WCAImpl::appendThermalLogPlaceholder(std::string &target) {
    target += "\t0";
}
