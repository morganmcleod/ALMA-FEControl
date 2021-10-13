#include "LOCK_ONLINE_2021JUL_B.h"
#include "CartAssembly.h"
#include "WCAImpl.h"
#include "LPRImpl.h"

using namespace std;
#include <cmath>

bool LOCK_ONLINE_2021JUL_B::lockPLL(CartAssembly &CA) const {
    WCAImpl *WCA_p = CA.useWCA();
    if (!WCA_p) {
        LOG(LM_ERROR) << "lockPLL: WCA is null" << endl;
        return false;
    }

    double freqLO(0.0), freqREF(0.0);

    if (!CA.getLOFrequency(freqLO, freqREF) && freqLO > 0.0) {
        LOG(LM_ERROR) << "lockPLL: LO frequency is 0" << endl;
        return false;
    }

    double freqFLOOG(0.0);
    int sbLock(-1);

    if (!CA.getLOLockSettings(freqFLOOG, sbLock)) {
        LOG(LM_ERROR) << "lockPLL: failed getLOLockSettings" << endl;
        return false;
    }

    LOG(LM_INFO) << "--> LOCK_ONLINE_2021JUL_B..." << endl;

    int warmMultiplier = WCAImpl::getMultiplier(CA.getBand());

    CA.pauseMonitor(true, true, "Searching for lock.");

    //parkStandbyBands();

    // 200 MHz at photoRefFreq:
    double spanFreq = 0.200;  //GHz
    // ICT-19283: reduce the span for band1
    // -- The +/-200 MHz search span which reduces to +/-66 MHz or 33 MHz at the YIG oscillator for other bands.
    // -- This not do so for band 1 where the warm multiplication factor is 1.
    // -- This makes it much more likely to get a false IFTP value at the first step of the search.
    if (CA.getBand() == 1) {
        spanFreq = 50e6;  //GHz
    }

    double hiYig = CA.getWCAConfig().FHIYIG_m;
    double loYig = CA.getWCAConfig().FLOYIG_m;

    // Really ugly way to compute 200 or 50 MHz photoRefFreq span equivalent in YIG steps:
    int yigSpan = (int)((spanFreq * 4095) / ((hiYig - loYig) * warmMultiplier)) + 1;

    LOG(LM_INFO) << "lockPLL: freqLO=" << freqLO << " freqREF=" << freqREF
                 << " freqFLOOG=" << freqFLOOG << " sbLock=" << sbLock << " yigSpan=" << yigSpan << endl;

    // Set the bit in the hardware:
    //   Done previously by setLOFrequency()

    // Set the coarse tune that corresponds to the linear interpolation of the yig frequency.
    double freqYIG;
    int coarseYIG = CA.getCoarseYIG(freqYIG, freqREF, freqLO, freqFLOOG, sbLock);

    WCA_p -> ytoCoarseTuneWithTrace(coarseYIG, "lockPLL: initial");

    // ((sbLock == 1) ? "1 Above Ref." : "0 Below Ref.")
    // Calculate the reference frequency per count.
    double ref_f_count = (hiYig - loYig) / 4095.0 * warmMultiplier;
    double counts = freqFLOOG/ref_f_count;
    int offset = (int)std::floor(counts + 0.5);

    LOG(LM_DEBUG) << "lockPLL: ref_f_count=" << ref_f_count << " counts=" << counts << " offset=" << offset << endl;

    // subtract if (apply XOR LSB).  Subtract for USB; add for LSB:
    //wca->offsetCoarseByFloog(sideband, currentSubScanInfo.floogFreq, apply = true);
    WCA_p -> ytoCoarseTuneWithTrace(coarseYIG + offset * (sbLock == 1 ? -1 : 1), "lockPLL: first offset");

    //First optimization of the LPR voltage.
    //Pump the voltage until IFTOTAL power is greater than limit target1_pll_if_pmW
    double target1_pll_if_pmW = 0.25; //V
    double lprStep1 = 0.01; //V
    double pplimit1 = 1.0;  //mW

    // sets EDFA to knee to start search
    // linear search up by lprStep1 until PLL IF power > target
    //   OR  the photodetector power exceeds pplimit.
    optimizeLPRVoltage(WCA_p, target1_pll_if_pmW, lprStep1, pplimit1, true);

    // Essentially this is a binary search of current coarse tuning +/- yigSpan steps.
    // Where IFTP is the fitness function.
    // This could break if currentCoarse and (currentCoarse + span) were both in a noisy region.
    maximizeIFTP(WCA_p, yigSpan);  // details BELOW

    // subtract if (apply XOR LSB).  Subtract for LSB; add for USB:
    //wca->offsetCoarseByFloog(sideband, currentSubScanInfo.floogFreq, apply = false);
    WCA_p -> ytoCoarseTuneWithTrace(coarseYIG + offset * (sbLock == 1 ? 1 : -1), "lockPLL: second offset");

    double target2_pll_if_pmW = 2.0; //V
    double lprStep2 = 0.02; //V
    double pplimit2 = 7.0; //mW

    // Same as first pass but with higher target, step, limit.  Start from prev optimization:
    optimizeLPRVoltage(WCA_p, target2_pll_if_pmW, lprStep2, pplimit2, false);

    // toggle the integrator once or twice:
    WCA_p -> pllNullLoopIntegrator(false);
    if (!WCA_p ->getLocked()) {
        WCA_p -> pllNullLoopIntegrator(true);
        WCA_p -> pllNullLoopIntegrator(false);
    }

    // Linear search down from previous optimization if IFTP is too high:
    double target3_pll_if_pmW = 4.5; //V
    lowerLPRVoltage(WCA_p, target3_pll_if_pmW, lprStep2);

    // assuming we are locked now, zero the PLL CV:
    WCA_p -> adjustPLL(0.0);
    CA.pauseMonitor(false, false);
    return WCA_p ->getLocked();
}

void LOCK_ONLINE_2021JUL_B::maximizeIFTP(WCAImpl *WCA_p, const int yigSpan) const {
    int coarseSpan = yigSpan;   // = 200 MHz at at photoRefFreq / warmMultiplier
    int initialCoarse = WCA_p -> ytoCoarseTune();
    int optimalCoarse = 0;
    int highCoarse = 0;
    int ctSign =1;
    double optimalIFTP = 0.0;
    double highIFTP = 0.0;

    // optimialCourse is where we expect the final tuning to be near
    // because of the first call to offsetCoarseByFloog() above:
    optimalCoarse = initialCoarse - coarseSpan;
    if (optimalCoarse < 0) {
        optimalCoarse = 0;
    }
    // BUT: previous to this in the calling code we may have offset by - or + depending on sideband.
    //      So this could be wrong here, and be the cause of subsequent failure to find the right tuning.

    // set course tuning yigSpan steps below current.
    WCA_p -> ytoCoarseTuneWithTrace(optimalCoarse, "maximizeIFTP: initial");

    SLEEP(48);

    // get the IFTP at the (initial - span) tuning:
    optimalIFTP = WCA_p -> pllIfTotalPower();


    // pre-calculate (initial + span) tuning:
    highCoarse = initialCoarse + coarseSpan;

    // So the search space is actually initialCoarse +/- coarseSpan.

    // Loop while reducing coarseSpan...
    //  Jump up by 'span' and then compare IFTP
    //  Change directions if new IFTP is better
    //  Reduce search space by half
    //  Effectively a binary search where IFTP is the fitness function.
    LOG(LM_INFO) << "maximizeIFTP: coarseSpan, highCoarse, highIFTP, ctSign, optimalIFTP, optimalCoarse" << endl;
    while(coarseSpan > 0) {  //  && cycleTime < cycleStop) {
        // highCoarse gets moved each iteration.  Keep it within limits:
        if (highCoarse > 4095) {
            highCoarse = 4095;
        }
        if (highCoarse < 0) {
            highCoarse = 0;
        }
        // jump to new 'high' tuning which is actually higher counts for first pass but is higher IFTP thereafter:
        WCA_p -> ytoCoarseTune(highCoarse);

        SLEEP(48);

        // Get the IFTP at this tuning:
        highIFTP = WCA_p -> pllIfTotalPower();

        // Set our next step direction  IF  this step was in the wrong direction:
        if (highCoarse > optimalCoarse) {
            ctSign = 1;
        } else {
            ctSign = -1;
        }

        // Did this step find a higher IFTP?
        if (std::fabs(highIFTP) > std::fabs(optimalIFTP)) {
            // Yes, make it the new gues for 'optimal':
            optimalIFTP = highIFTP;
            optimalCoarse = highCoarse;
            // And plan to step in the opposite direction next iter:
            highCoarse = optimalCoarse - coarseSpan*ctSign;
        } else {
            // No, plan to step in the same direction next iter:
            highCoarse = optimalCoarse + coarseSpan*ctSign;
        }
        // Binary divide search space:
        coarseSpan /= 2;
        //cycleTime = (::getTimeStamp() - startf) * 100 * 1E-9;
        LOG(LM_INFO) << "maximizeIFTP: " << coarseSpan << ", " << highCoarse
                      << ", " << highIFTP << ", " << ctSign << ", " << optimalIFTP
                      << ", " << optimalCoarse << endl;
    }
    //  Set the final coarse tuning:
    WCA_p -> ytoCoarseTuneWithTrace(optimalCoarse, "maximizeIFTP: final");
}


void LOCK_ONLINE_2021JUL_B::optimizeLPRVoltage(WCAImpl *WCA_p, double target_pll_if_pmW,
                                         double lpr_step, double pplimit,
                                         bool set_mod_input) const
{
    if (!WCA_p) {
        LOG(LM_ERROR) << "optimizeLPRVoltage: WCA_p is NULL" << endl;
        return;
    }

    if (!allowEdfaAdjust_m) {
        LOG(LM_ERROR) << "optimizeLPRVoltage: allowEdfaAdjust_m is false. Can't continue" << endl;
        return;
    }

    if (!LPR_p) {
        LOG(LM_ERROR) << "optimizeLPRVoltage: LPR_p is NULL" << endl;
        return;
    }

    if (set_mod_input) {
        float knee = 1.0;
        LPR_p -> EDFAModulationInput(knee);
        LOG(LM_DEBUG) << "optimizeLPRVoltage: Knee value: " << knee << endl;
    }

    LOG(LM_DEBUG) << "optimizeLPRVoltage: target_pll_if_pmW=" << target_pll_if_pmW
                 << " lpr_step=" << lpr_step << " pplimit=" << pplimit << endl;

    double pll_if_pmW = fabs(WCA_p -> pllIfTotalPower());
    double lprCurrentValue = LPR_p -> EDFAModulationInput();
    double photoPower = LPR_p -> EDFAPhotoDetectPower();
    double cycleTime = 0.0;
    double cycleStop = 1.5;
//        ACS::Time startf = ::getTimeStamp();
    while(pll_if_pmW < target_pll_if_pmW &&
          photoPower < pplimit &&
          cycleTime < cycleStop) {
        LPR_p -> EDFAModulationInput(lprCurrentValue + lpr_step);
        pll_if_pmW = fabs(WCA_p -> pllIfTotalPower());
        lprCurrentValue = LPR_p -> EDFAModulationInput();
        photoPower = LPR_p -> EDFAPhotoDetectPower();
        double pmCurrent = fabs(WCA_p -> photomixerCurrent());
        LOG(LM_DEBUG) << "Modulation voltage: " << lprCurrentValue + lpr_step
                     << " IFTotal Power: " << pll_if_pmW << " LPR power: "
                     << photoPower << " Photomixer Current: "<< pmCurrent << endl;

        cycleTime += 0.01;
    }

    LOG(LM_INFO) << "optimizeLPRVoltage target IFTP=" << target_pll_if_pmW << " actual="
                 << pll_if_pmW << " photoPower=" << photoPower << " mW" << endl;
}

void LOCK_ONLINE_2021JUL_B::lowerLPRVoltage(WCAImpl *WCA_p,
                                      double target_pll_if_pmW,
                                      double lpr_step) const
{
    if (!WCA_p) {
        LOG(LM_ERROR) << "lowerLPRVoltage: WCA_p is NULL" << endl;
        return;
    }

    if (!allowEdfaAdjust_m) {
        LOG(LM_ERROR) << "lowerLPRVoltage: allowEdfaAdjust_m is false. Can't continue" << endl;
        return;
    }

    if (!LPR_p) {
        LOG(LM_ERROR) << "lowerLPRVoltage: LPR_p is NULL" << endl;
        return;
    }

    double pll_if_pmW = fabs(WCA_p -> pllIfTotalPower());
    double lprCurrentValue = LPR_p -> EDFAModulationInput();
    double cycleTime = 0.0;
    double cycleStop = 1.5;

    //ACS::Time startf = ::getTimeStamp();
    while (pll_if_pmW > target_pll_if_pmW && cycleTime < cycleStop) {
        LPR_p -> EDFAModulationInput(lprCurrentValue + lpr_step);
        pll_if_pmW = fabs(WCA_p -> pllIfTotalPower());
        lprCurrentValue = LPR_p -> EDFAModulationInput();
        LOG(LM_DEBUG) << "lowerLPRVoltage: Modulation voltage: " << lprCurrentValue + lpr_step
                     << " IFTotal Power: " << pll_if_pmW;
        cycleTime += 0.01;
    }
}
