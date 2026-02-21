#include <ICT_19283_LOCK_CV_Points.h>
#include "CartAssembly.h"
#include "WCAImpl.h"
#include "LPRImpl.h"

using namespace std;
#include <cmath>

bool ICT_19283_LOCK_CV_Points::lockPLL(CartAssembly &CA) const {

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

    CA.pauseMonitor(true, true, "Searching for lock.");

    LOG(LM_INFO) << "lockPLL points=" << numPoints_m << " interval=" << interval_m << "..." << endl;

    int coldMultiplier = ColdCartImpl::getMultiplier(CA.getBand());
    int warmMultiplier = WCAImpl::getMultiplier(CA.getBand());
    
    if (allowEdfaAdjust_m) {
        const double targetPmxCurrent_mA = 1.1;   //mA    We want this much LO photomixer current when testing the lock
        const double lprStep_V = 0.1;             //V     Adjust in this step size
        // Set the LPR EDFA to a starting value:
        setLPRVoltageForPmxCurrent(WCA_p, targetPmxCurrent_mA, lprStep_V);
    }

    bool isLocked = WCA_p -> getLocked();

    // Target final settings for IFTP
    double targetIFTP_V = 2.0;          //V     When finally locked, adjust LPR EDFA to get this IFTP voltage
    double toleranceIFTP_V = 0.3;       //V     ... within this tolerance
    double pdPowerLimit_mW = 7.0;       //mW    But not exceeding this much photodetector power in the EDFA

    // If already locked, zero the CV:
    if (isLocked) {
        WCA_p -> adjustPLL(0.0);
        isLocked = WCA_p -> getLocked();

        // If still locked, adjust the LPR EDFA for target IFTP:
        if (isLocked && allowEdfaAdjust_m) {
            const double lprStep_V = 0.02;
            setLPRVoltageForIFTP(WCA_p, targetIFTP_V, toleranceIFTP_V, lprStep_V, pdPowerLimit_mW);
        }

    } else {
        // Gather variables:
        double hiYig = CA.getWCAConfig().FHIYIG_m;
        double loYig = CA.getWCAConfig().FLOYIG_m;
        double currentYig = freqLO / warmMultiplier / coldMultiplier;
        int firstGuessInt = (int)(4095.0 * (currentYig - loYig) / (hiYig - loYig));

        LOG(LM_INFO) << "lockPLL: points-1: "
                     << "first guess: coarseTune=" << firstGuessInt
                     << ", ytoFreq:"        << currentYig
                     << ", freqLO:"                << freqLO << endl;

        // Declare container: PLL voltage info:
        struct PllV {
            double corrV;
            double lockV;
            double coarseTune;
        };
        std::vector <PllV> pllVList;

        // search for points where lock is achieved:
        for (int i = 0; i < numPoints_m; i++) {
            int offset = (int)(interval_m * (i - int(numPoints_m / 2.0)));
            int thisTune = firstGuessInt + offset;

            // Border control
            if (thisTune > 4095)
                thisTune = 4095;
            if (thisTune < 0)
                thisTune = 0;

            // Disable the PLL and reset the integrator
            WCA_p -> pllNullLoopIntegrator(true);

            // Set coarse tune
            WCA_p -> ytoCoarseTuneWithTrace(thisTune, "lockPLL: initial");
            SLEEP(10);

            // Reactivate the PLL
            WCA_p -> pllNullLoopIntegrator(false);
            SLEEP(10);

            // Gather data
            PllV thisPllV;
            thisPllV.lockV = WCA_p -> pllLockDetectVoltage();
            thisPllV.corrV = WCA_p -> pllCorrectionVoltage();
            thisPllV.coarseTune = WCA_p -> ytoCoarseTune();
            LOG(LM_INFO) << "lockPLL: "
                         << "coarseTune:"   << thisPllV.coarseTune
                         << ", lockV:"        << thisPllV.lockV
                         << ", corrV:"      << thisPllV.corrV << endl;

            // Keep it if the lock detect is on:
            if (thisPllV.lockV > 3.0)
                pllVList.push_back(thisPllV);
        }

        // at this point the list contains only those points where the locking voltage was high = the PLL is able to lock
        // there must be at least two points, because the locking range is ~ 20 counts wide in B1

        // check for found only one point:
        if (pllVList.size() == 1) {
            WCA_p -> ytoCoarseTuneWithTrace(pllVList.front().coarseTune, "Lock at our one and only");
            SLEEP(10);
            // Zero the CV from here:
            WCA_p -> adjustPLL(0.0);
            // log one point found:
            double lockV = WCA_p -> pllLockDetectVoltage();
            double corrV = WCA_p -> pllCorrectionVoltage();
            double coarse = WCA_p -> ytoCoarseTune();
            LOG(LM_INFO) << "lockPLL: found only one good tuning: "
                         << "lockV:"    << lockV
                         << ", corrV:"  << corrV
                         << ", coarse:" << coarse << endl;

        } else if (pllVList.size() == 0) {
            // we could NOT find at least two locking points
            // something went WRONG
            LOG(LM_ERROR) << "lockPLL: points: FAILED TO FIND AT ANY POINTS!" << endl;
        } else {
            // we have found two or more locking points
            // Calculate slope:
            PllV firstPllV = pllVList.front();
            PllV lastPllV = pllVList.back();
            double y1 = lastPllV.corrV;
            double y0 = firstPllV.corrV;
            double x1 = lastPllV.coarseTune;
            double x0 = firstPllV.coarseTune;

            // guard divide by zero:
            if (x1 == x0) {
                LOG(LM_ERROR) << "lockPLL: calculate slope would divide by zero:"
                              << " x0=" << x0
                              << " x1=" << x1
                              << " y0=" << y0
                              << " y1=" << y1 << endl;
                return false;
            }

            double slope = (y1 - y0) / (x1 - x0);
            int tuneZero;
            // If we have a decreasing slope (good)
            if (slope <= -0.001) {
                // Get tune zero following the slope until y=0
                tuneZero = (int) ( -y0/slope  + x0 );
            // Else, maybe in simulation
            } else {
                // Get the mean point
                tuneZero = (int) ( (x0 + x1) / 2 );
                // Warn
                LOG(LM_WARNING) << "lockPLL: points: seems to be in simulation, set YIG coarse tune to the mean: " << tuneZero << endl;
            }

            LOG(LM_INFO) << "lockPLL: "
                         << "tuneZero:" << tuneZero
                         << " ,x0:" << x0
                         << " ,x1:" << x1
                         << " ,y0:" << y0
                         << " ,y1:" << y1
                         << " ,slope:" << slope << endl;

            // Disable the PLL and reset the integrator
            WCA_p -> pllNullLoopIntegrator(true);

            // Lock at tune_zero
            WCA_p -> ytoCoarseTuneWithTrace(tuneZero, "Lock at tuneZero");
            SLEEP(10);

            // Reactivate the PLL
            WCA_p -> pllNullLoopIntegrator(false);
            SLEEP(10);
            
            // Clear unlock detect:
            WCA_p -> pllClearUnlockDetectLatch();

            // check if still locked:
            double lockV = WCA_p -> pllLockDetectVoltage();
            double corrV = WCA_p -> pllCorrectionVoltage();
            double coarse = WCA_p -> ytoCoarseTune();
            if (lockV > 3.0) {
                LOG(LM_INFO) << "lockPLL: lock succeeded with: "
                            << "lockV:"    << lockV
                            << ", corrV:"  << corrV
                            << ", coarse:" << coarse << endl;
            } else {
                LOG(LM_ERROR) << "PLL NO LOCK" << endl;
            }
        }
        // log final state:
        isLocked = WCA_p -> getLocked();

        // Adjust LPR EDFA for target IFTP
        if (isLocked && allowEdfaAdjust_m) {
            float lprStep_V = 0.02;
            setLPRVoltageForIFTP(WCA_p, targetIFTP_V, toleranceIFTP_V, lprStep_V, pdPowerLimit_mW);
        }
    }
    CA.pauseMonitor(false, false);
    return WCA_p -> getLocked();
}

void ICT_19283_LOCK_CV_Points::setLPRVoltageForPmxCurrent(WCAImpl *WCA_p,
                                                          double targetPmxCurrent_mA,
                                                          double lprStep_V) const
// Set the LPR EDFA to the 'knee' voltage and then adjust upward until the target photomixer current is reached
{
    if (!WCA_p) {
       LOG(LM_ERROR) << "setLPRVoltageForPmxCurrent: WCA_p is NULL" << endl;
       return;
   }

   if (!LPR_p) {
       LOG(LM_ERROR) << "setLPRVoltageForPmxCurrent: LPR_p is NULL" << endl;
       return;
   }

   LOG(LM_DEBUG) << "setLPRVoltageForPmxCurrent: targetPmxCurrent_mA=" << targetPmxCurrent_mA
                 << " lprStep_V=" << lprStep_V << endl;

   float knee = 1.2;    // TODO: actually get from measured/stored!
   LPR_p -> EDFAModulationInput(knee);
   LOG(LM_DEBUG) << "setLPRVoltageForPmxCurrent: set EDFA voltage to knee value: " << knee << endl;

   double lprEDFA_V = LPR_p -> EDFAModulationInput();
   double pmCurrent_mA = fabs(WCA_p -> photomixerCurrent());
   double cycleTime = 0.0;
   double cycleStop = 1.5;
   // ACS::Time startf = ::getTimeStamp();
   while (pmCurrent_mA < targetPmxCurrent_mA && cycleTime < cycleStop) {
       lprEDFA_V += lprStep_V;
       LPR_p -> EDFAModulationInput(lprEDFA_V);
       SLEEP(10);
       pmCurrent_mA = fabs(WCA_p -> photomixerCurrent());
       LOG(LM_DEBUG) << "Modulation voltage: " << lprEDFA_V
                     << " Photomixer Current: " << pmCurrent_mA << endl;
       cycleTime += 0.01;   // TODO: use timestamps instead of this fakery.
   }

   LOG(LM_INFO) << "setLPRVoltageForPmxCurrent target Pmx current=" << targetPmxCurrent_mA
                << " actual=" << pmCurrent_mA << " mA" << " cycleTime=" << cycleTime << endl;
}

void ICT_19283_LOCK_CV_Points::setLPRVoltageForIFTP(WCAImpl *WCA_p,
                                                    double targetIFTP_V,
                                                    double toleranceIFTP_V,
                                                    double lprStep_V,
                                                    double pdPowerLimit_mW) const
// When locked: adjust the LPR EDFA to attain the requested IFTP detector voltage and tolerance
// Stopping if the LPR photodetector power limit is exceeded.
{
    if (!WCA_p) {
       LOG(LM_ERROR) << "setLPRVoltageForIFTP: WCA_p is NULL" << endl;
       return;
   }

   if (!LPR_p) {
       LOG(LM_ERROR) << "setLPRVoltageForIFTP: LPR_p is NULL" << endl;
       return;
   }

   targetIFTP_V = fabs(targetIFTP_V);
   toleranceIFTP_V = fabs(toleranceIFTP_V);
   lprStep_V = fabs(lprStep_V);

   LOG(LM_DEBUG) << "setLPRVoltageForIFTP: targetIFTP_V=" << targetIFTP_V << " tol=" << toleranceIFTP_V
                 << " lprStep_V=" << lprStep_V << " pdPowerLimit_mW=" << pdPowerLimit_mW << endl;

   double lprEDFA_V = LPR_p -> EDFAModulationInput();
   double pllIFTP_V = fabs(WCA_p -> pllIfTotalPower());
   double photoPower = LPR_p -> EDFAPhotoDetectPower();
   double cycleTime = 0.0;
   double cycleStop = 1.5;
//        ACS::Time startf = ::getTimeStamp();
   while ((pllIFTP_V < (targetIFTP_V - toleranceIFTP_V) ||
           pllIFTP_V > (targetIFTP_V + toleranceIFTP_V))  &&
          photoPower < pdPowerLimit_mW &&
          cycleTime < cycleStop)
   {
       lprEDFA_V += lprStep_V * ((pllIFTP_V <= targetIFTP_V) ? 1.0 : -1.0);

       LPR_p -> EDFAModulationInput(lprEDFA_V);
       SLEEP(10);
       pllIFTP_V = fabs(WCA_p -> pllIfTotalPower());
       photoPower = LPR_p -> EDFAPhotoDetectPower();
       LOG(LM_DEBUG) << "Modulation voltage: " << lprEDFA_V
                     << " IFTP voltage: " << pllIFTP_V
                     << " PhotoDetect power: " << photoPower << endl;
       cycleTime += 0.01;   // TODO: use timestamps instead of this fakery.
   }

   LOG(LM_INFO) << "setLPRVoltageForIFTP: target IFTP voltage=" << targetIFTP_V << " tol=" << toleranceIFTP_V
                << " actual=" << pllIFTP_V << " V cycleTime=" << cycleTime << endl;
}
