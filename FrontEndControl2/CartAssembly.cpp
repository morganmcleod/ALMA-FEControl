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
 * The class representing a generic cartridge assembly, including
 * operations for which a CartAssembly is responsible.
 * 
 *----------------------------------------------------------------------
 */

#include "CartAssembly.h"
#include "FEMCEventQueue.h"
#include "iniFile.h"
#include "logger.h"
#include "stringConvert.h"
#include "OPTIMIZE/CartHealthCheck.h"
#include "OPTIMIZE/OptimizeLOPowerAmp.h"
#include "OPTIMIZE/MaximizeIFPower.h"
#include "OPTIMIZE/MeasureFineLOSweep.h"
#include "OPTIMIZE/MeasureIVCurve.h"
#include "OPTIMIZE/MeasureIFPower.h"
#include "OPTIMIZE/MeasureSISCurrent.h"
#include "OPTIMIZE/MeasureSISVoltageError.h"
#include "OPTIMIZE/MixerDeflux.h"
#include "OPTIMIZE/MixerHeating.h"
#include "OPTIMIZE/XYPlotArray.h"

using namespace FEConfig;
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

std::string CartAssembly::FineLoSweepIni_m = std::string();

CartAssembly::CartAssembly(const std::string &name, WCAImpl *WCA, ColdCartImpl *coldCart)
  : WCA_mp(WCA),
    coldCart_mp(coldCart),
	measureSISVoltageErr_mp(NULL),
    cartHealthCheck_mp(NULL),
    optimizerPA_mp(NULL),
    measureLOSweep_mp(NULL),
    measureIV_mp(NULL),
    measureSISCurrent_mp(NULL),
    measureIFPower_mp(NULL),
    optimizerIFPower_mp(NULL),
    mixerDefluxer_mp(NULL),
    mixerHeater_mp(NULL),
    XYData_mp(new XYPlotArray),
    band_m(0)
{
    int WCABand((WCA_mp) ? WCA_mp -> getBand() : 0); 
    int CCBand((coldCart_mp) ? coldCart_mp -> getBand() : 0);
    reset();
    if (WCA_mp) {
        if (coldCart_mp) {
            band_m = CCBand; // if a coldcart is present, use its band as the CartAssembly's.
            if (WCABand != CCBand)
                LOG(LM_INFO) << "CartAssembly(" << band_m << ") WARNING: WCA band " << WCABand << " is paired with ColdCart band " << WCABand << "." << endl;
        } else {
            band_m = WCABand; // Else only a WCA is present, use its band as the CartAssembly's.
        }
    } else {
        if (coldCart_mp) {
            band_m = CCBand; // if a coldcart is present, use its band as the CartAssembly's.
            LOG(LM_INFO) << "CartAssembly(" << band_m << ") WARNING: ColdCart band " << coldCart_mp -> getBand() << " has no WCA." << endl;
        } else {
            string msg("CartAssembly ERROR: no WCA or ColdCart was provided.");
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << msg << endl;
        }
    }
}

CartAssembly::~CartAssembly() {
	delete measureSISVoltageErr_mp;
    delete cartHealthCheck_mp;
    delete optimizerPA_mp;
    delete measureLOSweep_mp;
    delete measureIV_mp;
    delete measureSISCurrent_mp;
    delete optimizerIFPower_mp;
    delete mixerDefluxer_mp;
    delete mixerHeater_mp;
    delete XYData_mp;
}

void CartAssembly::reset() {
    if (WCA_mp)
        WCA_mp -> reset();
    if (coldCart_mp)
        coldCart_mp -> reset();       
    multAMC_m = multCold_m = 0;
    enable_m = observing_m = false;
    freqLO_m = freqFLOOG_m = freqREF_m = 0;
    isTunedLO_m = false;
}

const std::string &CartAssembly::getBandText(std::string &toFill) const {
    toFill = to_string(band_m);
    toFill = "band" + toFill;
    return toFill;
}

void CartAssembly::queryCartridgeState() {
    if (!getEnable())
        return;
    if (WCA_mp) {
        WCA_mp -> queryCartridgeState();

        // get the cold and warm multipliers:
        int CCBand((coldCart_mp) ? coldCart_mp -> getBand() : band_m);
        multCold_m = ColdCartImpl::getMultiplier(CCBand);
        multAMC_m = WCAImpl::getMultiplier(WCA_mp -> getBand());
        bool sbLock = WCA_mp -> getPLLSidebandLockSelectSetting();

        // get the LO frequency, rounded to the nearest 100 MHz:
        double freqFLOOG((freqFLOOG_m > 0.0) ? freqFLOOG_m : 0.0315);  // Assume 31.5 MHz unless told otherwise.
        double freqWCA = multAMC_m * YIGCourseToFreq(WCA_mp -> getYTOCoarseTuneSetting());
        freqLO_m = multCold_m * freqWCA;
        freqLO_m = (floor(freqLO_m * 10 + 0.5) / 10);
        freqREF_m = freqWCA + ((sbLock == 0) ? freqFLOOG : -freqFLOOG);
        isTunedLO_m = true;
    }
    if (coldCart_mp)
        coldCart_mp -> queryCartridgeState();
}

void CartAssembly::measureSISVoltageError() {
    if (band_m <= 2) {
        // No SIS mixers in band 1 and 2.  Nothing to do.
        return;
    }
    if (!coldCart_mp) {
    	// No cold cartridge configured.  Nothing to do.
    	return;
    }
    if (!measureSISVoltageErr_mp)
    	measureSISVoltageErr_mp = new MeasureSISVoltageError(*this);

    measureSISVoltageErr_mp -> start();
}

void CartAssembly::setEnable() {
    if (getEnable())
        return;
    reset();
    enable_m = true;
    onEnableTasks();
}

void CartAssembly::clearEnable() {
    if (!getEnable())
        return;
    onDisableTasks();
    reset();
    enable_m = false;
}

void CartAssembly::onEnableTasks() {
    // setup tasks to be done immediately after a cartridge is powered on. 
    if (WCA_mp)
        WCA_mp -> startMonitor();
    if (coldCart_mp)
        coldCart_mp -> startMonitor();
}

void CartAssembly::onDisableTasks() {
    // shutdown tasks to be done before a cartridge is powered off.
    if (WCA_mp) {
        WCA_mp -> setPAEnable(false);
        WCA_mp -> photomixerEnable(false);
        WCA_mp -> pllNullLoopIntegrator(false);
        WCA_mp -> stopMonitor();
    }
    if (coldCart_mp) {
        coldCart_mp -> setSISEnable(false);
        coldCart_mp -> setSISMagnetEnable(false);
        coldCart_mp -> setLNAEnable(false);
        coldCart_mp -> stopMonitor();
    }        
}    

void CartAssembly::setObserving() {
    if (!getEnable())
        return;
    observing_m = true;
}

void CartAssembly::clearObserving() {
    observing_m = false;
}

void CartAssembly::startMonitor() {
    if (WCA_mp)
        WCA_mp -> startMonitor();
    if (coldCart_mp)
        coldCart_mp -> startMonitor();
}

void CartAssembly::stopMonitor() {
    if (WCA_mp)
        WCA_mp -> stopMonitor();
    if (coldCart_mp)
        coldCart_mp -> stopMonitor();
}

void CartAssembly::pauseMonitor(bool pauseWCA, bool pauseCC, const char *reason) {
    if (WCA_mp)
        WCA_mp -> minimalMonitor(pauseWCA, reason);
    if (coldCart_mp)
        coldCart_mp -> pauseMonitor(pauseCC, reason);
}

void CartAssembly::setColdCartConfig(const ColdCartConfig &params) { 
    if (checkColdCart("CartAssembly::setColdCartConfig")) {
        config_m.Id_m.band_m = params.band_m;
        config_m.coldCart_m = params;
        LOG(LM_INFO) << config_m.coldCart_m;
    }    
}

void CartAssembly::setWCAConfig(const WCAConfig &params) {
    if (checkWCA("CartAssembly::setWCAConfig")) {
        config_m.Id_m.band_m = params.band_m;
        config_m.WCA_m = params;
        LOG(LM_INFO) << config_m.WCA_m;
    }
}

void CartAssembly::setYIGLimits(double FLOYIG, double FHIYIG) { 
    if (checkWCA("CartAssembly::setYIGLimits")) {
        config_m.WCA_m.FLOYIG_m = FLOYIG;
        config_m.WCA_m.FHIYIG_m = FHIYIG;
        LOG(LM_INFO) << "CartAssembly::setYIGLimits band=" << band_m 
            << setprecision(4) << " FLOYIG=" << FLOYIG << " FHIYIG=" << FHIYIG << endl;
    }
}        

bool CartAssembly::setLOFrequency(double freqLO, double freqFLOOG, int sbLock) {
    freqLO_m = 0;
    freqFLOOG_m = 0;
    freqREF_m = 0;
    isTunedLO_m = false;

    if (!checkWCA("CartAssembly::setLOFrequency"))
        return false;

    int CCBand((coldCart_mp) ? coldCart_mp -> getBand() : band_m);
    multCold_m = ColdCartImpl::getMultiplier(CCBand);
    multAMC_m = WCAImpl::getMultiplier(WCA_mp -> getBand());
    
    // Get the default WCA loop bandwidth for this band:
    bool altLoopBW = WCAImpl::getAltLoopBW(WCA_mp -> getBand());

    // Get whether it is overridden in the WCAConfig:
    switch (config_m.WCA_m.loopBW_m) {
        case WCAConfig::LOOPBW_NORMAL:
            altLoopBW = false;
            break;
        case WCAConfig::LOOPBW_ALT:
            altLoopBW = true;
            break;
        case WCAConfig::LOOPBW_DEFAULT:
        default:
            break;
    }
    LOG(LM_DEBUG) << "CartAssembly: Setting loop BW = " << (altLoopBW ? "ALTERNATE" : "NORMAL") << endl;
    WCA_mp -> pllLoopBandwidthSelect(altLoopBW);

    if (sbLock < 0)
        sbLock = 0;
    else if (sbLock > 1)
        sbLock = 1;

    double freqYIG = 0;
    double freqREF = 0;
    int coarseYIG = getCoarseYIG(freqYIG, freqREF, freqLO, freqFLOOG, sbLock);
    if (coarseYIG < 0) {
        coarseYIG = 0;     
        string msg("CartAssembly ERROR: band ");
        msg += to_string(band_m);
        msg += " LO frequency setting is out of range.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    freqLO_m = freqLO;
    freqFLOOG_m = freqFLOOG;
    freqREF_m = freqREF; 
   
    LOG(LM_DEBUG) << "CartAssembly: Setting PLL_LOCK_POL = " << ((sbLock == 1) ? "1 Above Ref." : "0 Below Ref.") << endl;
    WCA_mp -> pllSidebandLockSelect(sbLock);

    bool lastNull = WCA_mp -> pllNullLoopIntegrator(); 
    nullPLLIntegrator(true);
    WCA_mp -> ytoCoarseTuneWithTrace(coarseYIG, "CartAssembly::setLOFrequency");
    nullPLLIntegrator(lastNull);

    isTunedLO_m = true;
    return true;
};

bool CartAssembly::overrideLoopBW(bool altLoopBW) {
    LOG(LM_INFO) << "CartAssembly: overrideLoopBW = " << (altLoopBW ? "ALTERNATE" : "NORMAL") << endl;
    WCA_mp -> pllLoopBandwidthSelect(altLoopBW);
    return true;
}

bool CartAssembly::getMonitorYTO(WCAImpl::YTO_t &target) const {
    target.reset();
    if (WCA_mp && WCA_mp -> getMonitorYTO(target)) {
        target.ytoFrequency_value = YIGCourseToFreq(target.ytoCoarseTune_value);
        return true;
    }
    return false;
}

bool CartAssembly::lockPLL() {
    if (!checkWCA("CartAssembly::lockPLL"))
        return false;

    if (freqLO_m == 0.0)
        return false;

    LOG(LM_INFO) << "Locking PLL...." << endl;

    double freqYIG = 0;
    double freqREF = 0;
    int sbLock = WCA_mp -> getPLLSidebandLockSelectSetting();
    int coarseYIG = getCoarseYIG(freqYIG, freqREF, freqLO_m, freqFLOOG_m, sbLock);
    if (coarseYIG < 0) {  
        coarseYIG = 0;
        return false;
    }

    // lock search window is +/- 50 MHz:
    const double window(0.05);

    // lock search step size is 3 MHz:
    const double step(0.003);
    
    // compute YIG step size:
    double stepYIG = (config_m.WCA_m.FHIYIG_m - config_m.WCA_m.FLOYIG_m) / 4095.0;

    // lock search window in YIG steps: 
    int windowSteps = (int) (window / stepYIG) + 1;
     
    // lock step size in YIG steps:
    int stepSize = (int) (step / stepYIG);
    if (stepSize == 0)
        stepSize = 1;
    
    pauseMonitor(true, true, "Searching for lock.");

    bool success = WCA_mp -> searchForLock(coarseYIG, windowSteps, stepSize);
    if (success) {
        // clear the unlock detect latch so we will see if lock is lost from here on:
        WCA_mp -> clearUnlockDetect();
        success = WCA_mp -> adjustPLL(0.0);
    }
    pauseMonitor(false, false);
    return success;
}

bool CartAssembly::adjustPLL(float targetCorrVoltage) {
    pauseMonitor(true, true, "Adjusting PLL.");
    bool ret = WCA_mp -> adjustPLL(targetCorrVoltage);
    pauseMonitor(false, false);
    return ret;
}     

bool CartAssembly::adjustYTO(int steps) {
    int coarseYIG = WCA_mp -> ytoCoarseTune();    
    coarseYIG += steps;
    if (coarseYIG < 0)
        coarseYIG = 0;
    if (coarseYIG > 4095)
        coarseYIG = 4095;
    WCA_mp -> ytoCoarseTuneWithTrace(coarseYIG, "CartAssembly::adjustYTO");
    return true;   
}

int CartAssembly::getCoarseYIG(double &freqYIG, double &freqREF,
                               double freqLO, double freqFLOOG, int sbLock) const
{
    double freqWCA = freqLO / multCold_m;
        ///< the frequency at the WCA LO output ports.

    freqREF = freqWCA + ((sbLock == 0) ? freqFLOOG : -freqFLOOG);
        ///< the required frequency at the WCA reference input port.

    freqYIG = freqWCA / multAMC_m;
        ///< the target YTO frequency.

    int coarseYIG = YIGFreqToCoarse(freqYIG);

    LOG(LM_DEBUG) << "getCoarseYIG: band=" << band_m << fixed << setprecision(6)
        << " freqLO=" << freqLO << " freqFLOOG=" << freqFLOOG << " freqREF=" << freqREF << " freqYIG=" << freqYIG
        << " coarseYIG=" << coarseYIG << " lock=" << ((sbLock == 1) ? "above ref" : "below ref") << endl;
    return coarseYIG;
}

int CartAssembly::YIGFreqToCoarse(double freqYIG) const { 
    if ((freqYIG < config_m.WCA_m.FLOYIG_m) || (freqYIG > config_m.WCA_m.FHIYIG_m)) {
        LOG(LM_INFO) << "freqYIG out of range. Parameter error." << endl;
        return -1;
    }
    return (unsigned short) ((freqYIG - config_m.WCA_m.FLOYIG_m) / 
                             (config_m.WCA_m.FHIYIG_m - config_m.WCA_m.FLOYIG_m) * 4095.0);
}

double CartAssembly::YIGCourseToFreq(int coarseYIG) const {
    double span = config_m.WCA_m.FHIYIG_m - config_m.WCA_m.FLOYIG_m;
    double offset = (span * coarseYIG) / 4095.0;
    return config_m.WCA_m.FLOYIG_m + offset;     
}

bool CartAssembly::nullPLLIntegrator(bool enable) {
    if (!checkWCA("CartAssembly::nullPLLIntegrator"))
        return false;
    LOG(LM_DEBUG) << "Setting PLL_NULL_INTEGRATOR=" <<  (enable ? "1" : "0") << endl;
    WCA_mp -> pllNullLoopIntegrator(enable);
    return true;    
}

bool CartAssembly::getNullPLLIntegrator() const {
    if (!checkWCA(NULL))
        return false;
    return WCA_mp -> getPLLNullLoopIntegratorSetting();
}

bool CartAssembly::setLNABias(int pol, int sb, 
                              const float *_VD1, 
                              const float *_ID1,
                              const float *_VD2, 
                              const float *_ID2,
                              const float *_VD3, 
                              const float *_ID3)
{
    if (!checkColdCart("CartAssembly::setLNABias"))
        return false;

    bool setAll = (pol < 0 || sb < 1);

    float VD1 = (_VD1) ? *_VD1 : 0;
    float VD2 = (_VD2) ? *_VD2 : 0;
    float VD3 = (_VD3) ? *_VD3 : 0;
    float ID1 = (_ID1) ? *_ID1 : 0;
    float ID2 = (_ID2) ? *_ID2 : 0;
    float ID3 = (_ID3) ? *_ID3 : 0;

    if (setAll || (pol == 0 && sb == 1)) {
        PreampParams &params = config_m.coldCart_m.usePreampParams(0, 1);
        ParamTableRow row;
        bool success = params.get(freqLO_m, row);
        if (success) {
            if (!_VD1)
                VD1 = row[PreampParams::VD1];
            if (!_VD2)
                VD2 = row[PreampParams::VD2];
            if (!_VD3)
                VD3 = row[PreampParams::VD3];
            if (!_ID1)
                ID1 = row[PreampParams::ID1];
            if (!_ID2)
                ID2 = row[PreampParams::ID2];
            if (!_ID3)
                ID3 = row[PreampParams::ID3];
        }
        if (!success) {
            LOG(LM_INFO) << "Getting LNA parameters failed" << endl;
            return false;
        }
        LOG(LM_DEBUG) << fixed << setprecision(2)
            << "Setting LNA_POL0_SB1_VD1 to " << VD1 << endl
            << "Setting LNA_POL0_SB1_VD2 to " << VD2 << endl
            << "Setting LNA_POL0_SB1_VD3 to " << VD3 << endl
            << "Setting LNA_POL0_SB1_ID1 to " << ID1 << endl
            << "Setting LNA_POL0_SB1_ID2 to " << ID2 << endl
            << "Setting LNA_POL0_SB1_ID3 to " << ID3 << endl;

        coldCart_mp -> lnaPol0Sb1St1DrainVoltage(VD1);
        coldCart_mp -> lnaPol0Sb1St2DrainVoltage(VD2);
        coldCart_mp -> lnaPol0Sb1St3DrainVoltage(VD3);
        coldCart_mp -> lnaPol0Sb1St1DrainCurrent(ID1);
        coldCart_mp -> lnaPol0Sb1St2DrainCurrent(ID2);
        coldCart_mp -> lnaPol0Sb1St3DrainCurrent(ID3);
    }
    if (coldCart_mp -> hasSb2() && (setAll || (pol == 0 && sb == 2))) {
        PreampParams &params = config_m.coldCart_m.usePreampParams(0, 2);
        ParamTableRow row;
        bool success = params.get(freqLO_m, row);
        if (success) {
            if (!_VD1)
                VD1 = row[PreampParams::VD1];
            if (!_VD2)
                VD2 = row[PreampParams::VD2];
            if (!_VD3)
                VD3 = row[PreampParams::VD3];
            if (!_ID1)
                ID1 = row[PreampParams::ID1];
            if (!_ID2)
                ID2 = row[PreampParams::ID2];
            if (!_ID3)
                ID3 = row[PreampParams::ID3];
        }
        if (!success) {
            LOG(LM_INFO) << "Getting LNA parameters failed" << endl;
            return false;
        }
        LOG(LM_DEBUG) << fixed << setprecision(2)
            << "Setting LNA_POL0_SB2_VD1 to " << VD1 << endl
            << "Setting LNA_POL0_SB2_VD2 to " << VD2 << endl
            << "Setting LNA_POL0_SB2_VD3 to " << VD3 << endl
            << "Setting LNA_POL0_SB2_ID1 to " << ID1 << endl
            << "Setting LNA_POL0_SB2_ID2 to " << ID2 << endl
            << "Setting LNA_POL0_SB2_ID3 to " << ID3 << endl;

        coldCart_mp -> lnaPol0Sb2St1DrainVoltage(VD1);
        coldCart_mp -> lnaPol0Sb2St2DrainVoltage(VD2);
        coldCart_mp -> lnaPol0Sb2St3DrainVoltage(VD3);
        coldCart_mp -> lnaPol0Sb2St1DrainCurrent(ID1);
        coldCart_mp -> lnaPol0Sb2St2DrainCurrent(ID2);
        coldCart_mp -> lnaPol0Sb2St3DrainCurrent(ID3);
    }
    if (setAll || (pol == 1 && sb == 1)) {
        PreampParams &params = config_m.coldCart_m.usePreampParams(1, 1);
        ParamTableRow row;
        bool success = params.get(freqLO_m, row);
        if (success) {
            if (!_VD1)
                VD1 = row[PreampParams::VD1];
            if (!_VD2)
                VD2 = row[PreampParams::VD2];
            if (!_VD3)
                VD3 = row[PreampParams::VD3];
            if (!_ID1)
                ID1 = row[PreampParams::ID1];
            if (!_ID2)
                ID2 = row[PreampParams::ID2];
            if (!_ID3)
                ID3 = row[PreampParams::ID3];
        }
        if (!success) {
            LOG(LM_INFO) << "Getting LNA parameters failed" << endl;
            return false;
        }
        LOG(LM_DEBUG) << fixed << setprecision(2)
            << "Setting LNA_POL1_SB1_VD1 to " << VD1 << endl
            << "Setting LNA_POL1_SB1_VD2 to " << VD2 << endl
            << "Setting LNA_POL1_SB1_VD3 to " << VD3 << endl
            << "Setting LNA_POL1_SB1_ID1 to " << ID1 << endl
            << "Setting LNA_POL1_SB1_ID2 to " << ID2 << endl
            << "Setting LNA_POL1_SB1_ID3 to " << ID3 << endl;

        coldCart_mp -> lnaPol1Sb1St1DrainVoltage(VD1);
        coldCart_mp -> lnaPol1Sb1St2DrainVoltage(VD2);
        coldCart_mp -> lnaPol1Sb1St3DrainVoltage(VD3);
        coldCart_mp -> lnaPol1Sb1St1DrainCurrent(ID1);
        coldCart_mp -> lnaPol1Sb1St2DrainCurrent(ID2);
        coldCart_mp -> lnaPol1Sb1St3DrainCurrent(ID3);
    }
    if (coldCart_mp -> hasSb2() && (setAll || (pol == 1 && sb == 2))) {
        PreampParams &params = config_m.coldCart_m.usePreampParams(1, 2);
        ParamTableRow row;
        bool success = params.get(freqLO_m, row);
        if (success) {
            if (!_VD1)
                VD1 = row[PreampParams::VD1];
            if (!_VD2)
                VD2 = row[PreampParams::VD2];
            if (!_VD3)
                VD3 = row[PreampParams::VD3];
            if (!_ID1)
                ID1 = row[PreampParams::ID1];
            if (!_ID2)
                ID2 = row[PreampParams::ID2];
            if (!_ID3)
                ID3 = row[PreampParams::ID3];
        }
        if (!success) {
            LOG(LM_INFO) << "Getting LNA parameters failed" << endl;
            return false;
        }
        LOG(LM_DEBUG) << fixed << setprecision(2)
            << "Setting LNA_POL1_SB2_VD1 to " << VD1 << endl
            << "Setting LNA_POL1_SB2_VD2 to " << VD2 << endl
            << "Setting LNA_POL1_SB2_VD3 to " << VD3 << endl
            << "Setting LNA_POL1_SB2_ID1 to " << ID1 << endl
            << "Setting LNA_POL1_SB2_ID2 to " << ID2 << endl
            << "Setting LNA_POL1_SB2_ID3 to " << ID3 << endl;

        coldCart_mp -> lnaPol1Sb2St1DrainVoltage(VD1);
        coldCart_mp -> lnaPol1Sb2St2DrainVoltage(VD2);
        coldCart_mp -> lnaPol1Sb2St3DrainVoltage(VD3);
        coldCart_mp -> lnaPol1Sb2St1DrainCurrent(ID1);
        coldCart_mp -> lnaPol1Sb2St2DrainCurrent(ID2);
        coldCart_mp -> lnaPol1Sb2St3DrainCurrent(ID3);
    }
    return true;
}

bool CartAssembly::setEnableLNABias(bool enable, int pol, int sb) {
    if (!checkColdCart("CartAssembly::setEnableLNABias"))
        return false;

    bool setAll = (pol < 0 || sb < 1);
    if (setAll)
        return coldCart_mp -> setLNAEnable(enable);
    else
        return coldCart_mp -> setLNAEnable(pol, sb, enable);
}

bool CartAssembly::getEnableLNABias() const {
    if (!checkColdCart(NULL))
        return false;
    return coldCart_mp -> getLNAEnableSetting();
}

bool CartAssembly::setSISBias(bool enable, int pol, int sb, const float *_VJ, int _openLoop) {

    if (!checkColdCart("CartAssembly::setSISBias"))
        return false;

    bool setAll = (pol < 0 || sb < 1);

    float VJ = (_VJ) ? *_VJ : 0.0;
    bool openLoop = (_openLoop == 1); 

    // Check whether this band has SIS mixers:
    if (!coldCart_mp -> hasSIS()) {
        // If not and attempting to operate them, its an error:
        if (enable && VJ != 0.0) {
            string msg("setSISBias ERROR: enabling SIS mixers where no SIS in band ");
            msg += to_string(band_m);
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
            return false;

        } else {
            // Return no error:
            return true;
        }
    }

    if (!coldCart_mp -> setSISEnable(enable))
        return false;

    if (enable) {    
        ParamTableRow row;
        bool success = config_m.coldCart_m.mixerParams_m.get(freqLO_m, row);
        if (!success) {
            LOG(LM_INFO) << "Getting SIS parameters failed" << endl;
            return false;
        }

        if (setAll || (pol == 0 && sb == 1)) {
            if (!_VJ)
                VJ = row[MixerParams::VJ01];
            LOG(LM_DEBUG) << "Setting SIS_POL0_SB1_VOLTAGE to " << VJ << endl;
            coldCart_mp -> setSISVoltage(0, 1, VJ);
            if (_openLoop >= 0) {
                LOG(LM_DEBUG) << "Setting SIS_POL0_SB1 mode to " << ((openLoop) ? "open loop" : "closed loop") << endl;
                coldCart_mp -> sisPol0Sb1OpenLoop(openLoop);                      
            }
        }
        if (coldCart_mp -> hasSb2() && (setAll || (pol == 0 && sb == 2))) {
            if (!_VJ)
                VJ = row[MixerParams::VJ02];
            LOG(LM_DEBUG) << "Setting SIS_POL0_SB2_VOLTAGE to " << VJ << endl;
            coldCart_mp -> setSISVoltage(0, 2, VJ);
            if (_openLoop >= 0) {
                LOG(LM_DEBUG) << "Setting SIS_POL0_SB2 mode to " << ((openLoop) ? "open loop" : "closed loop") << endl;
                coldCart_mp -> sisPol0Sb2OpenLoop(openLoop);                      
            }
        }
        if (setAll || (pol == 1 && sb == 1)) {
            if (!_VJ)
                VJ = row[MixerParams::VJ11];
            LOG(LM_DEBUG) << "Setting SIS_POL1_SB1_VOLTAGE to " << VJ << endl;
            coldCart_mp -> setSISVoltage(1, 1, VJ);
            if (_openLoop >= 0) {
                LOG(LM_DEBUG) << "Setting SIS_POL1_SB1 mode to " << ((openLoop) ? "open loop" : "closed loop") << endl;
                coldCart_mp -> sisPol1Sb1OpenLoop(openLoop);                      
            }
        }
        if (coldCart_mp -> hasSb2() && (setAll || (pol == 1 && sb == 2))) {
            if (!_VJ)
                VJ = row[MixerParams::VJ12];
            LOG(LM_DEBUG) << "Setting SIS_POL1_SB2_VOLTAGE to " << VJ << endl;
            coldCart_mp -> setSISVoltage(1, 2, VJ);
            if (_openLoop >= 0) {
                LOG(LM_DEBUG) << "Setting SIS_POL1_SB2 mode to " << ((openLoop) ? "open loop" : "closed loop") << endl;
                coldCart_mp -> sisPol1Sb2OpenLoop(openLoop);                      
            }
        }
    }
    return true;
}

bool CartAssembly::getEnableSISBias() const {
    if (!checkColdCart(NULL))
        return false;
    return coldCart_mp -> getSISEnableSetting();
}

bool CartAssembly::setSISMagnet(bool enable, int pol, int sb, const float *_IMag) {
    if (!checkColdCart("CartAssembly::setSISMagnet"))
        return false;

    bool setAll = (pol < 0 || sb < 1);

    float IMag01(_IMag ? *_IMag : 0.0);
    float IMag02(_IMag ? *_IMag : 0.0);
    float IMag11(_IMag ? *_IMag : 0.0);
    float IMag12(_IMag ? *_IMag : 0.0);

    // Check whether this band has SIS mixers:
    if (!coldCart_mp -> hasSIS()) {
        // If so and attempting to operate the SIS magnets, its an error:
        if (enable && (IMag01 != 0.0 || IMag02 != 0.0 || IMag11 != 0.0 || IMag12 != 0.0)) {
            string msg("setSISMagnet ERROR: operating SIS magnets where no SIS in band ");
            msg += to_string(band_m);
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
            return false;

        } else {
            // Return no error:
            return true;
        }
    }

    if (!coldCart_mp -> setSISMagnetEnable(enable))
        return false;

    if (enable) {
        if (!_IMag)
            if (!getSISMagnetNominal(IMag01, IMag02, IMag11, IMag12))
                return false;

        if (setAll || (pol == 0 && sb == 1)) {
            LOG(LM_DEBUG) << "Setting SIS_POL0_SB1_MAGNET_CURRENT to " << IMag01 << endl;
            coldCart_mp -> setSISMagnetCurrent(0, 1, IMag01);
        }
        if (coldCart_mp -> hasSb2() && (setAll || (pol == 0 && sb == 2))) {
            LOG(LM_DEBUG) << "Setting SIS_POL0_SB2_MAGNET_CURRENT to " << IMag02 << endl;
            coldCart_mp -> setSISMagnetCurrent(0, 2, IMag02);
        }
        if (setAll || (pol == 1 && sb == 1)) {
            LOG(LM_DEBUG) << "Setting SIS_POL1_SB1_MAGNET_CURRENT to " << IMag11 << endl;
            coldCart_mp -> setSISMagnetCurrent(1, 1, IMag11);
        }
        if (coldCart_mp -> hasSb2() && (setAll || (pol == 1 && sb == 2))) {
            LOG(LM_DEBUG) << "Setting SIS_POL1_SB2_MAGNET_CURRENT to " << IMag12 << endl;
            coldCart_mp -> setSISMagnetCurrent(1, 2, IMag12);
        }
    }
    return true;       
}

bool CartAssembly::getEnableSISMagnet() const {
    if (!checkColdCart(NULL))
        return false;
    return coldCart_mp -> getSISMagnetEnableSetting();
}

bool CartAssembly::getSISMagnet(int pol, int sb, float *IMag) {
    if (pol < 0)
        pol = 0;
    else if (pol > 1)
        pol = 1;
    if (sb < 1)
        sb = 1;
    else if (sb > 2)
        sb = 2;
    if (coldCart_mp && IMag) {
        *IMag = coldCart_mp -> getSISMagnetCurrentSetting(pol, sb);
        return true;
    }
    return false;
}

bool CartAssembly::getSISMagnetNominal(float &IMag01, float &IMag02, float &IMag11, float &IMag12) const {
    ParamTableRow row;
    double freq(freqLO_m);
    bool success = config_m.coldCart_m.magnetParams_m.get(freq, row);
    if (!success) {
        if (coldCart_mp -> getBand() > 4)   // band 4 doesn't have electromagnets.  Bands 5-10 do.
            LOG(LM_ERROR) << "Getting SIS magnet parameters failed" << endl;
        IMag01 = IMag02 = IMag11 = IMag12 = 0.0;
        return false;
    }
    IMag01 = row[MagnetParams::IMag01];
    IMag02 = row[MagnetParams::IMag02];
    IMag11 = row[MagnetParams::IMag11];
    IMag12 = row[MagnetParams::IMag12];
    return true;
}

bool CartAssembly::setLOPowerAmps(bool enable,
                                  const float *_VDP0, 
                                  const float *_VGP0,
                                  const float *_VDP1, 
                                  const float *_VGP1)
{
    if (!checkWCA("CartAssembly::setLOPowerAmp"))
        return false;

    float VDP0 = (_VDP0) ? *_VDP0 : 0;
    float VGP0 = (_VGP0) ? *_VGP0 : 0;
    float VDP1 = (_VDP1) ? *_VDP1 : 0;
    float VGP1 = (_VGP1) ? *_VGP1 : 0;

    WCA_mp -> setPAEnable(enable);
    
    if (enable) {
        ParamTableRow row;
        bool success = config_m.WCA_m.PAParams_m.get(freqLO_m, row);
        if (!success) {
            LOG(LM_INFO) << "Getting LO_PA parameters failed" << endl;
            return false;
        }
        if (!_VDP0)
            VDP0 = row[PowerAmpParams::VD0];    
        if (!_VDP1)
            VDP1 = row[PowerAmpParams::VD1];    
        if (!_VGP0)
            VGP0 = row[PowerAmpParams::VG0];    
        if (!_VGP1)
            VGP1 = row[PowerAmpParams::VG1];    
        LOG(LM_DEBUG) << "Setting LO_PA_POL0_DRAIN_VOLTAGE to " << VDP0 << endl;
        WCA_mp -> paPol0DrainVoltage(VDP0);
        LOG(LM_DEBUG) << "Setting LO_PA_POL1_DRAIN_VOLTAGE to " << VDP1 << endl;
        WCA_mp -> paPol1DrainVoltage(VDP1);
        LOG(LM_DEBUG) << "Setting LO_PA_POL0_GATE_VOLTAGE to " << VGP0 << endl;
        WCA_mp -> paPol0GateVoltage(VGP0);
        LOG(LM_DEBUG) << "Setting LO_PA_POL1_GATE_VOLTAGE to " << VGP1 << endl;
        WCA_mp -> paPol1GateVoltage(VGP1);
    }    
    return true;
}

bool CartAssembly::getLOPowerAmpsEnable() const {
    if (!checkWCA(NULL))
        return false;
    return WCA_mp -> getPAEnableSetting();
}

bool CartAssembly::adjustLOPowerAmps(int repeatCount) {

    if (!checkWCA("CartAssembly::adjustLOPowerAmps"))
        return false;
    if (!checkColdCart("CartAssembly::adjustLOPowerAmps"))
        return false;

    if (!isTunedLO_m) {
        LOG(LM_INFO) << "CartAssembly::adjustLOPowerAmps error LO is not tuned." << endl;
        return false;
    }
    if (!WCA_mp -> getPAEnableSetting()) {
        LOG(LM_INFO) << "CartAssembly::adjustLOPowerAmps error LO PA is not enabled." << endl;
        return false;
    }

    if (!coldCart_mp -> hasSIS()) {
        // For bands with no SIS, there is nothing to adjust.  Return no error:
        return true;
    }

    if (!coldCart_mp -> getSISEnableSetting()) {
        LOG(LM_INFO) << "CartAssembly::adjustLOPowerAmps error SIS is not enabled." << endl;
        return false;
    }

    if (!optimizerPA_mp)
        optimizerPA_mp = new OptimizeLOPowerAmp(*this);

    ParamTableRow SISRow, PARow; 
    bool success = config_m.coldCart_m.mixerParams_m.get(freqLO_m, SISRow);
    if (success) 
        success = config_m.WCA_m.PAParams_m.get(freqLO_m, PARow);
    if (!success) {
        LOG(LM_INFO) << "CartAssembly::adjustLOPowerAmps: Getting SIS or PA parameters failed" << endl;
        return false;
    }

    float VD0 = PARow[PowerAmpParams::VD0];
    float VD1 = PARow[PowerAmpParams::VD1];

    float targetIJ01 = SISRow[MixerParams::IJ01];
    float targetIJ11 = SISRow[MixerParams::IJ11];
    
    bool ret = optimizerPA_mp -> start(freqLO_m, VD0, targetIJ01, VD1, targetIJ11, repeatCount);
    if (!ret)
        LOG(LM_INFO) << "CartAssembly::adjustLOPowerAmps: optimize failed." << endl;
    return ret;
}

bool CartAssembly::adjustLOPowerAmpsIsBusy() const {
    if (!optimizerPA_mp)
        return false;
    else
        return (optimizerPA_mp -> busy());
}


#define READ_ISIS_A(pol) (fabs(coldCart_mp -> getSISCurrent(pol, 1, 10) / 1E3))

// adjust the LO power amplifier drain voltage setting to achieve a target junction current.
// 'pol' controls which polarization to optimize
// 'VD' is the drain voltage to adjust.
// 'targetIJ1' is the optimal junction current for SIS1.  SIS2 is allowed to fall where it may.
bool CartAssembly::adjustLOPASinglePol(int pol, float &VD, float targetIJ1, bool verboseLogging) {
    if (!checkWCA("CartAssembly::adjustLOPASinglePol"))
        return false;
    if (!checkColdCart("CartAssembly::adjustLOPASinglePol"))
        return false;

    if (!coldCart_mp -> getSISEnableSetting()) {
        string msg("OptimizeLOPowerAmp ERROR: SIS mixer is not enabled.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    if (!WCA_mp -> getPAEnableSetting()) {
        string msg("OptimizeLOPowerAmp ERROR: Power amplifier is not enabled.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }

    char buf[500];
    int timeStart = GETTIMER();  // get the timer value in ms.
    int iter = 0;
    bool error = false;
    bool foundMin = false;      // true if a minimum has been seen


//#define FECONTROL2_5
//#define TRUNK_20151228
//#define CCAOPT_PY
//#define B2015_08_ICT_6284_B
#define MMPROPOSED

#ifdef MMPROPOSED

    //Sleep time
    int ts = 0; //ms

    float ij = fabs(targetIJ1 / 1E6);               // targetIJ1 in A

    sprintf(buf, "Pol%d: Target ij=%.4f[uA]", pol,ij*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    const float pa_default = 1.0;
    const float pa_window = 0.25;
    const float pa_max = 2.50;

    // get the Vset and ISIS from prev. setting:
    float u = WCA_mp -> getPADrainVoltageSetting(pol);
    if (u < pa_window || u > (pa_max - pa_window)) {
        u = pa_default;
        WCA_mp -> setPADrainVoltage(pol, u);
        SLEEP(ts);
    }
    float iavg = READ_ISIS_A(pol);

    //Get the Vset to ISIS conversion slope
    float u_low = u - pa_window;
    float u_high = u + pa_window;

    WCA_mp -> setPADrainVoltage(pol, u_low);
    SLEEP(ts);
    float isis_low = READ_ISIS_A(pol);

    WCA_mp -> setPADrainVoltage(pol, u_high);
    SLEEP(ts);
    float isis_high = READ_ISIS_A(pol);

    float slope = fabs((isis_high - isis_low) / (u_high - u_low));   // |A/V|

    if (verboseLogging) {
        sprintf(buf, "Pol%d: u_low=%.4f[V] u_high=%.4f[V] isis_low=%.4f[uA] isis_high=%.4f[uA] slope=%.4f[uA/V]", pol, u_low, u_high, isis_low*1.0E6, isis_high*1e6, slope*1E6);
        LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;
    }

    // Initial error
    float e1 = (ij - iavg);
    float e0 = e1;
    //Step size and sign to get there in four steps:
    float u_step = (e1 / slope) / 4.0;
    float sign = (u_step < 0) ? -1.0 : 1.0;

    //Guard against flat-ish slope leading to overlarge V step:
    const float max_step = 0.25;
    if (fabs(u_step) > max_step)
        u_step = max_step * sign;

    if (verboseLogging) {
        sprintf(buf, "Pol%d: vd=%.4f[V] iavg=%.4f[uA] e0=%.4f[uA] e1=%.4f[uA] step=%.4f[V]", pol, u, iavg*1.0E6, e0*1e6, e1*1E6, u_step);
        LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol  start: " << buf << endl;
    }

    //Loop until the error change the sign (this mean to cross over the target)
    bool abort = false;
    int j = 0;
    while (((e0 < 0.0) == (e1 < 0.0)) && j++ < 40 && !abort) {
        //Course step
        u += u_step;

        //Output limit:
        if (u < 0.0) {
            u = 0;
            abort = true;
        } else if (u > 2.5) {
            u = 2.5;
            abort = true;
        }

        //Set LO-PA bias voltage
        WCA_mp -> setPADrainVoltage(pol, u);
        SLEEP(ts);

        //Read the new error
        iavg = READ_ISIS_A(pol);
        e0 = e1;
        e1 = ij - iavg;

        if (verboseLogging) {
            sprintf(buf, "Pol%d: vd=%.4f[V] iavg=%.4f[uA] e0=%.4f[uA] e1=%.4f[uA] iter=%d", pol, u, iavg*1.0E6, e0*1e6, e1*1E6, j);
            LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol coarse: " << buf << endl;
        }
        iter++;
    }

    //Early exit if we happened to reach the target exactly or already failed:
    if(e1 != 0.0 && !abort) {

        //Fine adjustment
        e0 = e1;
        j = 0;
        while (((e0 < 0.0) == (e1 < 0.0)) && j++ < 100 && !abort) {
            if (e1 < 0.0)
                u -= 10.0e-3;
            else
                u += 10.0e-3;

            //Output limit:
            if (u < 0.0) {
                u = 0;
                abort = true;
            } else if (u > 2.5) {
                u = 2.5;
                abort = true;
            }
            //Set LO-PA bias voltage
            WCA_mp -> setPADrainVoltage(pol, u);
            SLEEP(ts);

            //Read the new error
            iavg = READ_ISIS_A(pol);
            e0 = e1;
            e1 = ij - iavg;

            if (verboseLogging) {
                sprintf(buf, "Pol%d: vd=%.4f[V] iavg=%.4f[uA] e0=%.4f[uA] e1=%.4f[uA] iter=%d", pol, u, iavg*1.0E6, e0*1e6, e1*1E6, j);
                LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol   fine: " << buf << endl;
            }
            iter++;
        }

        //Very Fine Adjustment
        if (fabs(e0) < fabs(e1) && !abort) {
            j++;
            if (e1 < 0.0)
                u -= 5.0e-3;
            else
                u += 5.0e-3;

            //Set LO-PA bias voltage
            WCA_mp -> setPADrainVoltage(pol, u);

            SLEEP(ts);
            //Read the new error
            iavg = READ_ISIS_A(pol);
            e0 = e1;
            e1 = ij - iavg;

            if (verboseLogging) {
                sprintf(buf, "Pol%d: vd=%.4f[V] iavg=%.4f[uA] e0=%.4f[uA] e1=%.4f[uA] iter=%d", pol, u, iavg*1.0E6, e0*1e6, e1*1E6, j);
                LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol v.fine: " << buf << endl;
            }
            iter++;
        }
    }

    VD = u;
    error = abort;
    foundMin = !abort;

#endif

#ifdef B2015_08_ICT_6284_B

    // seems to be in [Vget/A] which can't possibly be right for all WCAs of a given band.
    int kp_bands[10] = {4000, 4000, 5000, 4000, 6000, 6000, 4000, 4000, 7000, 4000};

    //Sleep time
    int ts = 5; //ms

    //Kp = Proportional constant (band dependent)
    int kp = kp_bands[band_m - 1];

    float ij = fabs(targetIJ1 / 1E6);               // targetIJ1 in A

    sprintf(buf, "Pol%d: Target ij=%.4f[uA]", pol,ij*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;


    //Get the Drain Voltage Convertion Constant
    float pa_low = 0, pa_high = 1.0;
    float u, u_00, u_10;

    u = WCA_mp -> getPADrainVoltage(pol);   // Vget from prev. setting

    WCA_mp -> setPADrainVoltage(pol, pa_low);       // Vset
    u_00 = WCA_mp -> getPADrainVoltage(pol);        // Vget

    WCA_mp -> setPADrainVoltage(pol, pa_high);      // Vset
    u_10 = WCA_mp -> getPADrainVoltage(pol);        // Vget

    //Calculate the convertion constant mu
    float mu = (u_00 - u_10) / (pa_low - pa_high);  // slope [-Vget/-Vset]
    float nu = u_10 - mu * pa_high;                 // y-intercept [Vget]

    //Re-scale the previous Vget u (round to 10mV)
    u = int(((u - nu) / mu) * 100.0) / 100.0;

    if (u <= 0.4 || u > 2.5) {
        u = 1.0;
    }

    WCA_mp -> setPADrainVoltage(pol, u);

    // Initial error
    float iavg = READ_ISIS_A(pol);  // A
    float e1 = ij - iavg;     // error A
    float e0 = e1;

    sprintf(buf, "Pol%d: vd=%.4f[V] yavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    //2.5v / 256 = 9.765mV = ~10 mV
    float roundTo = 10E-3;


    //Keep until the error change the sign (this mean to cross over the target)
    size_t j = 0;
    while ((e0 < 0.0) == (e1 < 0.0) && j++ < 40) {
        LOG(LM_INFO) << "Coarse adjustment ongoing, iteration " << j << endl;

        //Translate the error to the Vd_set space
        // VsetErr = A * [Vget/A] / [Vget/Vset]
        float ue = e1 * kp / mu;

        //Round to 10.0mV (the minimum voltage step)
        float counts = ceil(fabs(ue) / roundTo);
        if (ue < 0.0)
            ue = counts * roundTo * -1;
        else
            ue = counts * roundTo;

        //Accumulated error
        u += ue;

        //Output limit
        if (u < 0.0)
            u = 0;
        else if (u > 2.5)
            u = 2.5;

        //Set LO-PA bias voltage
        WCA_mp -> setPADrainVoltage(pol, u);

        SLEEP(ts);

        //Read the new error
        iavg = READ_ISIS_A(pol);
        e0 = e1;
        e1 = ij - iavg;

        sprintf(buf, "Pol%d: vd=%.4f[V] yavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
        LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;
        iter++;
    }

    if(((e0 < 0.0) == (e1 < 0.0)) || (e1 == 0.0)) {
        VD = u;

    //Fine adjustment
    } else {
        j = 0;
        while ((e0 < 0.0) == (e1 < 0.0) && j++ < 100) {
            if (e1 < 0.0)
                u -= 10.0e-3;
            else
                u += 10.0e-3;

            //Set LO-PA bias voltage
            WCA_mp -> setPADrainVoltage(pol, u);

            SLEEP(ts);

            //Read the new error
            iavg = READ_ISIS_A(pol);
            e0 = e1;
            e1 = ij - iavg;

            iter++;
        }

        //Very Fine Adjustment -

        if (fabs(e0) < fabs(e1)) {

            if (e1 < 0.0)
                u -= 5.0e-3;
            else
                u += 5.0e-3;

            //Set LO-PA bias voltage
            WCA_mp -> setPADrainVoltage(pol, u);

            SLEEP(ts);
            //Read the new error
            iavg = READ_ISIS_A(pol);
            e0 = e1;
            e1 = ij - iavg;

            iter++;
        }
    }

    sprintf(buf, "Pol%d: yavg=%.4f[uA] e1=%.4f[uA] vd=%.4f[V])\n", pol, iavg * 1.0e6, (ij - iavg)*1e6, u);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    VD = u;
    foundMin = true;

#endif

#ifdef CCAOPT_PY

    // seems to be in [Vget/A] which can't possibly be right for all WCAs of a given band.
    int kp_bands[10] = {4000, 4000, 5000, 4000, 6000, 6000, 4000, 4000, 7000, 4000};

    //Sleep time
    int ts = 48; //ms

    //Kp = Proportional constant (band dependent)
    int kp = kp_bands[band_m - 1];

    float ij = fabs(targetIJ1 / 1E6);               // targetIJ1 in A

    sprintf(buf, "Pol%d: Target ij=%.4f[uA]", pol,ij*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;


    //Get the Drain Voltage Convertion Constant
    float u, u_00, u_10;
    {
        u = WCA_mp -> getPADrainVoltage(pol);       // Vget from prev. setting

        WCA_mp -> setPADrainVoltage(pol, 0.0);      // Vset
        u_00 = WCA_mp -> getPADrainVoltage(pol);    // Vget

        WCA_mp -> setPADrainVoltage(pol, 1.0);      // Vset
        u_10 = WCA_mp -> getPADrainVoltage(pol);    // Vget
    }

    //Calculate the convertion constant mu
    float mu = (u_00 - u_10) / (0.0 - 1.0);               // slope [-Vget/-Vset]
    float nu = u_10 - mu * 1.0;                           // y-intercept [Vget]

    //Re-scale the previous Vget u (round to 10mV)
    u = int(((u - nu) / mu) * 100.0) / 100.0;

    // except (in case of div/0 mu)
    //    nu = 0.0
    //    mu = 1.0
    //    u = 0.0

    //Guess the initial voltage when is out of range (usually is near to 1.0)
    if (u <= 0.4 or u > 2.5) {
        u = 1.0;

        if (band_m == 5)
            u = 2.0;
        else
            u = 1.0;
    }

    // so now the rescaled/corrected GET value is being used as the SET value.   This seems wrong.
    WCA_mp -> setPADrainVoltage(pol, u);

    // Initial error
    float iavg = READ_ISIS_A(pol);  // A
    float e1 = ij - iavg;     // error A
    float e0 = e1;

    sprintf(buf, "Pol%d: vd=%.4f[V] iavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    //2.5v / 256 = 9.765mV = ~10 mV
    float roundTo = 10E-3;

    //Coarse Adjustment -
    LOG(LM_INFO) << "Coarse Adjustment" << endl;

    //Keep until the error change the sign (this mean to cross over the target)
    int j = 0;
    float ue;
    while ((e0 < 0.0) == (e1 < 0.0) && j < 40) {

        //Translate the error to the Vd_set space
        // VsetErr = A * [Vget/A] / [Vget/Vset]
        ue = e1 * kp / mu;

        //Round to 10.0mV (the minimum voltage step)
        float counts = ceil(fabs(ue) / roundTo);
        if (ue < 0.0)
            ue = counts*-roundTo;
        else
            ue = counts*roundTo;

        //Accumulated error
        u += ue;

        //Output limit
        if (u < 0.0)
            u = 0;
        if (u > 2.5)
            u = 2.5;

        //Set LO-PA bias voltage
        WCA_mp -> setPADrainVoltage(pol, u);

        SLEEP(ts);

        //Read the new error
        iavg = READ_ISIS_A(pol);
        e0 = e1;
        e1 = ij - iavg;

        sprintf(buf, "Pol%d: vd=%.4f[V] yavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
        LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;
        j++;
        iter++;
    }

    if ((e0 < 0.0) == (e1 < 0.0)) {
//        self.iOptT[str(pol)] = 0.0;
//        return u
        u;

    } else if (e1 == 0.0) {
//        self.iOptT[str(pol)] = 0.0;
//        return u
        u;

    } else {
        e0 = e1;
        //Fine Adjustment -
        LOG(LM_INFO) << "Fine Adjustment" << endl;

        //Adjust in step of 10mV

        j = 0;
        while ((e0 < 0.0) == (e1 < 0.0) && j < 100) {
            if (e1 < 0.0)
                u = u - 10.0e-3;
            else
                u = u + 10.0e-3;

            //Set LO-PA bias voltage
            WCA_mp -> setPADrainVoltage(pol, u);

            SLEEP(ts);

            //Read the new error
            iavg = READ_ISIS_A(pol);
            e0 = e1;
            e1 = ij - iavg;

            sprintf(buf, "Pol%d: vd=%.4f[V] yavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
            LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;
            j++;
            iter++;
        }

        //Very Fine Adjustment -

        if (fabs(e0) < fabs(e1)) {
            LOG(LM_INFO) << "Very Fine Adjustment" << endl;

            if (e1 < 0.0)
                u = u - 5.0e-3;
            else
                u = u + 5.0e-3;

            //Set LO-PA bias voltage
            WCA_mp -> setPADrainVoltage(pol, u);

            SLEEP(ts);
            //Read the new error
            iavg = READ_ISIS_A(pol);
            e0 = e1;
            e1 = ij - iavg;

            sprintf(buf, "Pol%d: vd=%.4f[V] yavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
            LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;
        }
    }

    sprintf(buf, "Pol%d: iavg=%.4f[uA] e1=%.4f[uA] (Set value= %.4f[V])\n", pol, iavg * 1.0e6, (ij - iavg)*1e6, u);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    VD = u;
    foundMin = true;

#endif

#ifdef TRUNK_20151228

    // seems to be in [Vget/A] which can't possibly be right for all WCAs of a given band.
    int kp_bands[10] = {4000, 4000, 5000, 4000, 6000, 6000, 4000, 4000, 7000, 4000};

    //Sleep time
    int ts = 5; //ms

    //Kp = Proportional constant (band dependent)
    int kp = kp_bands[band_m - 1];

    float ij = fabs(targetIJ1 / 1E6);               // targetIJ1 in A

    sprintf(buf, "Pol%d: Target ij=%.4f[uA]", pol,ij*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    //Get the Drain Voltage Convertion Constant
    float u, ijc, u_00, ij00, u_01, ij01;
    {
        u = WCA_mp -> getPADrainVoltage(pol);       // Vget from prev. setting
        ijc = READ_ISIS_A(pol);                     // A from prev.

        WCA_mp -> setPADrainVoltage(pol, 0.75);     // Vset
        u_00 = WCA_mp -> getPADrainVoltage(pol);    // Vget
        ij00 = READ_ISIS_A(pol);                    // A

        WCA_mp -> setPADrainVoltage(pol, 1.5);      // Vset
        u_01 = WCA_mp -> getPADrainVoltage(pol);    // Vget
        ij01 = READ_ISIS_A(pol);                    // A
    }

    //Calculate the conversion constant mu
    float mu = (u_01 - u_00)/(1.5 - 0.75);          // slope [Vget/Vset]
    float nu = u_01-1.5*mu;                         // y-intercept [Vget]

    //Re-scale the previous Vget u (round to 10mV)
    u = ((int)(((u - nu) / mu) * 100.0)) / 100.0;

    //Calculate the conversion constant my
    float my = (ij01 - ij00)/(1.5 - 0.75);          // SIS current slope [A/Vset]
    float ny = ij01-1.5*my;                         // y-intercept [A]

    //Calc the SET voltage corresponding to the target current ij (round to 10mV)
    float u0 = ((int)(((ij - ny) / my) * 100.0)) / 100.0;

    sprintf(buf, "Pol%d: ij=%.4f[uA], my=%.4f[uA/Vset], ny=%.4f[uA], ij01=%.4f[uA]", pol, ij*1E6, my*1E6, ny*1E6, ij01*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    sprintf(buf, "Pol%d: ijc=%.4f[uA], ij00=%.4f[uA],ij01=%.4f[uA]", pol, ijc*1E6, ij00*1E6, ij01*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    sprintf(buf, "Pol%d: Initial Guess u0 = %.4f[V]", pol, u0);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    sprintf(buf, "Pol%d: Prev setting u = %.4f[V]", pol, u);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    //Guess the initial voltage when it is out of range (usually is near 1.0)
    // is (previous current A - target A) > 25% of target?
    if (fabs(ijc-ij) >= 0.25*ij) {
        // Force initial guess to be in [1.5...2.1]
        if (u0 > 2.1)
            u = 2.1;
        else if (u0 < 1.5)
            u = 1.5;
        else
            u = u0;
    }

    // so now the rescaled/corrected GET value is being used as the SET value.   This seems wrong.

    sprintf(buf, "Pol%d: Set value = %.4f[V]", pol, u);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    //Restore the initial voltage
    WCA_mp -> setPADrainVoltage(pol, u);

    //Initial error
    float iavg = READ_ISIS_A(pol);  // A
    float e1 = ij - iavg;           // error A
    float e0 = e1;

    sprintf(buf, "Pol%d: vd%d=%.4f[V] iavg=%.4f[uA] e1=%.4f[uA]", pol, u, iavg*1.0E6, e1*1E6);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    //2.5v / 256 = 9.765mV = ~10 mV
    float roundTo = 10E-3;

    //Coarse Adjustment
    //Keep until the error change the sign (this mean to cross over the target)
    int j = 0;
    bool hitMax = false;
    std::vector<float> iavgList;
    std::vector<float> uList;
    float u_r;
    while (((e0 < 0.0) == (e1 < 0.0)) && j < 20) {
        sprintf(buf, "Loop ongoing j=%d", j);
        LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

        //Translate the error to the Vd_set space
        // VsetErr = A * [Vget/A] / [Vget/Vset]
        float ue = e1*kp / mu;

        //Round to 10.0mV (the minimum voltage step)
        float counts = ceil(fabs(ue) / roundTo);
        if (ue < 0.0)
            ue = counts*-roundTo;
        else
            ue = counts*roundTo;

        //Accumulated error
        u += ue;

        //Set LO-PA bias voltage
        WCA_mp -> setPADrainVoltage(pol, u);
        u_r = WCA_mp -> getPADrainVoltage(pol);

        //Print!
        SLEEP(ts);
        //Read the new error
        iavg = READ_ISIS_A(pol);
        e0 = e1;
        e1 = ij - iavg;
        iavgList.push_back(iavg);
        uList.push_back(u);

        //Output limit
        if (u < -0.1 || u_r > 3.5 || j > 18) {
            hitMax = true;
            std::vector<float>::iterator max = std::max_element(iavgList.begin(), iavgList.end());
            int index = std::distance(iavgList.begin(), max);
            float um = uList[index];

            sprintf(buf, "Escape from Coarse Adjustment: vd%d=%.4f[V] (Set value= %.4f[V])", pol, u_r, u);
            LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

            if (um < u - 0.2) {
                WCA_mp -> setPADrainVoltage(pol, um);
                u_r = WCA_mp -> getPADrainVoltage(pol);

                sprintf(buf, "Pol%d, SIS Current hit a peak around Set value vd%d=%.4f[V])", pol, pol, um);
                LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

                u = um;
            }
            break;
        }
        //print
        //print
        j++;
        iter++;
    }
    if (!hitMax) {
        if (e1 != 0.0) {
            e0 = e1;
            //Fine adjustment
            j = 0;
            while ((e0 < 0.0) == (e1 < 0.0) && j < 20) {
                float ue = 20.0E-3;
                if (e1 < 0.0)
                u = u - ue;
                else
                u = u + ue;
                //Set LO-PA bias voltage
                WCA_mp -> setPADrainVoltage(pol, u);
                u_r = WCA_mp -> getPADrainVoltage(pol);
                SLEEP(ts);

                //Read the new error
                iavg = READ_ISIS_A(pol);
                e0 = e1;
                e1 = ij - iavg;
                //print
                j++;
                iter++;
            }
            //Very fine adjustment (minimum error)
            //Adjust in step of (9.756mV /2) ~ 5mV
            if (fabs(e1) >= ij*0.05) {
                //print
                e0 = e1;
                j = 0;
                while (fabs(e1) > ij*0.05 && j < 5) {
                    if (e1 < 0.0)
                    u = u - 5.0E-3;
                    else
                    u = u + 5.0E-3;
                    //Set LO-PA bias voltage
                    WCA_mp -> setPADrainVoltage(pol, u);
                    u_r = WCA_mp -> getPADrainVoltage(pol);
                    SLEEP(ts);
                    iavg = READ_ISIS_A(pol);
                    e0 = e1;
                    e1 = ij - iavg;
                    j++;
                    iter++;
                }
                //print
            }
        }
    }

    sprintf(buf, "Pol%d: iavg=%.4f[uA] e1=%.4f[uA] vd%d=%.4f[V] (Set value= %.4f[V])\n", pol, iavg * 1.0e6, (ij - iavg)*1e6, pol, u_r, u);
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol: " << buf << endl;

    VD = u;
    foundMin = true;

#endif

#ifdef FECONTROL2_5

    // Constants governing the range of possible settings:
    const float VDmax(2.5);         // V
    const float VDstep(VDmax / 255);
    float window = fabsf(targetIJ1 * 0.05); // +/- 5% of target in uA
    float startVD(VD);

    // Set a maximum iteration count to guard against pathological inputs:    
    const int iterLimit = 260;

    if (verboseLogging) {
        LOG(LM_INFO) << "OptimizeLOPowerAmp: pol=" << pol << fixed << setprecision(4)
                     << " startVD=" << startVD << " VDstep=" << VDstep << " targetIJ1=" << targetIJ1 << " +/- " << window << " uA" << endl;
    }

    // Check for targetIJ1==0 and just use starting VD if so:
    if (targetIJ1 == 0.0) {
        WCA_mp -> setPADrainVoltage(pol, VD);
        LOG(LM_ERROR) << "OptimizeLOPowerAmp: targetIJ1 is zero so setting VD, can't optimize." << endl;
        return true;
    }
    
    // Define the window of IJ values near the target.  We'll try to move IJ1 within these limits:
    float loThreshold = fabsf(targetIJ1) - window;
    if (loThreshold < 0)
        loThreshold = 0;        
    float hiThreshold = fabsf(targetIJ1) + window;
        
    // Initialize variables used within the loop:
    bool done = false;
    float minErrorIJ1 = 999;    // starting smallest errorIJ1 values seen
    float minErrorVD1 = 0;      // VD setting where smallest errorIJ1 was seen
    float VJ1;                  // instantaneous VJ reading 
    float IJ1;                  // instantaneous IJ reading
    float errorIJ1;             // IJ error = abs(IJ1 - targetIJ1)
    int searchDirection;        // inverse of the sign of the error
    vector<float> VDHistory;    // array to collect recent VD settings to prevent oscillating around the target.
        
    while (!done && !error) {
        
        // Check for maximum iterations:
        if (iter++ > iterLimit) {
            string msg("OptimizeLOPowerAmp ERROR: Max iterations exceeded.");
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << fixed << "VD=" << setprecision(4) << VD << " IJ1=" << IJ1 << " iter=" << iter << " Max iterations exceeded, error stop." << endl;
            error = true;
        
        // Check for VD out of range:
        } else if (VD < 0 || VD > VDmax) {
            string msg("OptimizeLOPowerAmp ERROR: VD out of range.");
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << fixed << "VD=" << setprecision(4) << VD << " IJ1=" << IJ1 << " iter=" << iter << " VD out of range, error stop." << endl;
            error = true;
        
        } else {
            // Set the new (or starting) VD and get the instantaneous VJ & IJ reading:
            WCA_mp -> setPADrainVoltage(pol, VD);
            VJ1 = coldCart_mp -> getSISVoltage(pol, 1);
            IJ1 = 1000 * fabsf(coldCart_mp -> getSISCurrent(pol, 1, 10));
            // Save the drain voltage setting into the history array:
            VDHistory.push_back(VD);
            // Limit the history array to four elements:
            if (VDHistory.size() == 5)
                VDHistory.erase(VDHistory.begin());
            std::stringstream historyStream;

            if (verboseLogging) {
                historyStream << fixed << setprecision(4);
                for (int index = VDHistory.size() - 1; index >= 0; --index)
                    historyStream << " " << VDHistory[index];
            }
            
            if (IJ1 >= loThreshold && IJ1 <= hiThreshold) {
                // We are within the window.
                if (verboseLogging)
                    LOG(LM_INFO) << fixed << "VD=" << setprecision(4) << historyStream.str() << " IJ1=" << IJ1 << " Within window." << endl;

                // Compute the IJ error and sign:
                // We pretend zero error is positive to simplify.
                errorIJ1 = IJ1 - targetIJ1;
                if (errorIJ1 >= 0)
                    searchDirection = -1;
                else {
                    searchDirection = 1;
                    errorIJ1 = -errorIJ1;
                }
                   
                // Find the minimum error...
                
                // If the absolute error is smaller than the smallest seen,
                //  save the new smallest and the corresponding VD:
                if (!foundMin) {
                    if (errorIJ1 <= minErrorIJ1) {
                        minErrorIJ1 = errorIJ1;
                        minErrorVD1 = VD;
                    // Otherwise the error has started to increase; stop here:
                    } else {
                        VD = minErrorVD1;
                        if (verboseLogging)
                            LOG(LM_INFO) << fixed << "VD=" << setprecision(4) << historyStream.str() << " errorIJ1=" << minErrorIJ1 << " Found minimum, stopping." << endl;
                        done = foundMin = true;
                    }
                }
                if (!foundMin)
                    VD += searchDirection * VDstep;
            
            // Check for oscillations, defined as two iterations through the same two values:
            } else if (VDHistory.size() == 4 && VDHistory[0] == VDHistory[2] && VDHistory[1] == VDHistory[3]) {
                // take the larger of the two values:
                if (VDHistory[0] > VD) {
                    VD = VDHistory[0];
                    // Set it to the hardware:
                    WCA_mp -> setPADrainVoltage(pol, VD);
                    VJ1 = coldCart_mp -> getSISVoltage(pol, 1);
                    IJ1 = 1000 * fabsf(coldCart_mp -> getSISCurrent(pol, 1, 10));
                }
                // log a message and exit:
                if (verboseLogging)
                    LOG(LM_INFO) << fixed << "VD=" << setprecision(4) << historyStream.str() << " IJ1=" << IJ1 << " Oscillation detected, stopping." << endl;
                done = foundMin = true;
            
            // If IJ is below the low threshold, step up:        
            } else if (IJ1 < loThreshold) {
                if (verboseLogging)
                    LOG(LM_INFO) << fixed << "VD=" << setprecision(4) << historyStream.str() << " IJ1=" << IJ1 << " Stepping up." << endl;
                VD += VDstep;
                    
            // If it is above the high threshold step down:            
            } else if (IJ1 > hiThreshold) {
                if (verboseLogging)
                    LOG(LM_INFO) << fixed << "VD=" << setprecision(4) << historyStream.str() << " IJ1=" << IJ1 << " Stepping down." << endl;
                VD -= VDstep;
            }
        }
    }
    // Set the final VD:
    if (error)
        VD = startVD;

#endif

    WCA_mp -> setPADrainVoltage(pol, VD);

    int timeEnd = GETTIMER();
    LOG(LM_INFO) << "CartAssembly::adjustLOPASinglePol(pol=" << pol << ", VD=" << VD << ", targetIJ1=" << targetIJ1
                 << "): iter=" << iter << ", timeElapsed=" << (timeEnd - timeStart) << " ms, error=" << error << endl;

    return foundMin;   
}

bool CartAssembly::optimizeIFPower(bool doPol0, bool doPol1, float VDstart0, float VDstart1) {
    if (!checkWCA("CartAssembly::optimizeIFPower"))
        return false;
    if (!checkColdCart("CartAssembly::optimizeIFPower"))
        return false;

    if (!observing_m) {
        string msg("optimizeIFPower ERROR: this is not the observing band.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
        return false;
    }
    if (!isTunedLO_m) {
        string msg("optimizeIFPower ERROR: LO is not tuned");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
        return false;
    }
    if (!coldCart_mp -> getSISEnableSetting()) {
        string msg("optimizeIFPower ERROR: SIS is not enabled.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
        return false;
    }
    if (!WCA_mp -> getPAEnableSetting()) {
        string msg("optimizeIFPower ERROR: PA is not enabled.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
        return false;
    }
    
    if (!optimizerIFPower_mp)
        optimizerIFPower_mp = new MaximizeIFPower(*coldCart_mp, *WCA_mp);
    
    ParamTableRow SISRow, PARow;
    bool success = config_m.coldCart_m.mixerParams_m.get(freqLO_m, SISRow);
    if (success) 
        success = config_m.WCA_m.PAParams_m.get(freqLO_m, PARow);
    if (!success) {
        string msg("optimizeIFPower ERROR: Getting SIS or PA parameters failed.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
        return false;
    }

    // get the starting SIS voltages from the configuration database:
    float startVJ1 = coldCart_mp -> getSISVoltageSetting(0, 1);
    float startVJ2 = coldCart_mp -> getSISVoltageSetting(0, 2);
    float startVD0 = 0.8;
    float startVD1 = 0.8;
    if (VDstart0 > 0.0 && VDstart0 <= 2.5)
        startVD0 = VDstart0;
    if (VDstart1 > 0.0 && VDstart1 <= 2.5)
        startVD1 = VDstart1;

    // but use fixed values for bands 3 and 4:
    if (band_m == 3)
        startVJ1 = startVJ2 = 10.0;
    else if (band_m == 4)
        startVJ1 = startVJ2 = 5.0;

    bool ret = optimizerIFPower_mp -> start(freqLO_m, doPol0, doPol1, startVJ1, startVJ2, startVD0, startVD1);
    if (!ret) {
        string msg("optimizeIFPower ERROR: optimize failed.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "CartAssembly::" << msg << endl;
        return false;
    }
    return ret;
}

bool CartAssembly::clearOptimizedResult() {
    if (!optimizerIFPower_mp)
        return false;
    optimizerIFPower_mp -> resetParamsResult();
    return true;
}

bool CartAssembly::getOptimizedResult(std::string &mixerParamsText) {
    mixerParamsText.clear();

    if (!optimizerIFPower_mp)
        return false;

    const MixerParams &mp = optimizerIFPower_mp -> getMixerParamsResult();

    std::stringstream tmp;
    mp.streamOut(tmp, true);
    mixerParamsText = tmp.str();

    LOG(LM_INFO) << "CartAssembly::getOptimizedResult band=" << band_m << ":" << endl;
    LOG(LM_INFO) << mixerParamsText << endl;
    return true;
}

bool CartAssembly::setIFPower(int pol, float powerSB1, float powerSB2) {
//    LOG(LM_INFO) << "CartAssembly::setIFPower: pol=" << pol << setprecision(4) << scientific
//                 << " powerSB1=" << powerSB1 << " powerSB2=" << powerSB2 << fixed << endl;
    if (measureIFPower_mp)
        measureIFPower_mp -> setIFPower(pol, powerSB1, powerSB2);
    if (optimizerIFPower_mp)
        optimizerIFPower_mp -> setIFPower(pol, powerSB1, powerSB2);
    return true;
}


bool CartAssembly::setVJVD(int pol, float *VJ1, float *VJ2, float *VD)
{
    if (pol < 0)
        pol = 0;
    else if (pol > 1)
        pol = 1;

    if (pol == 0) {
        if (coldCart_mp) {
            if (VJ1) {
                LOG(LM_DEBUG) << "Setting SIS_POL0_SB1_VOLTAGE to " << fixed << setprecision(2) << *VJ1 << endl;
                coldCart_mp -> setSISVoltage(0, 1, *VJ1);
            }
            if (VJ2) {
                LOG(LM_DEBUG) << "Setting SIS_POL0_SB2_VOLTAGE to " << fixed << setprecision(2) << *VJ2 << endl;
                coldCart_mp -> setSISVoltage(0, 2, *VJ2);
            }
        }
        if (WCA_mp && VD) {
            LOG(LM_DEBUG) << "Setting LO_PA_POL0_DRAIN_VOLTAGE to " << fixed << setprecision(3) << *VD << endl;
            WCA_mp -> paPol0DrainVoltage(*VD);
        }
    
    } else {
        if (coldCart_mp) {
            if (VJ2) {
                LOG(LM_DEBUG) << "Setting SIS_POL1_SB1_VOLTAGE to " << fixed << setprecision(2) << *VJ2 << endl;
                coldCart_mp -> setSISVoltage(1, 1, *VJ1);
            }
            if (VJ2) {
                LOG(LM_DEBUG) << "Setting SIS_POL1_SB2_VOLTAGE to " << fixed << setprecision(2) << *VJ2 << endl;
                coldCart_mp -> setSISVoltage(1, 2, *VJ2);
            }
        }
        if (WCA_mp && VD) {
            LOG(LM_DEBUG) << "Setting LO_PA_POL1_DRAIN_VOLTAGE to " << fixed << setprecision(3) << *VD << endl;
            WCA_mp -> paPol1DrainVoltage(*VD);
        }
    }        
    return true;
}


bool CartAssembly::getVJVD(int pol, float *VJ1, float *VJ2, float *VD)
{
    if (pol < 0)
        pol = 0;
    else if (pol > 1)
        pol = 1;

    if (coldCart_mp && VJ1)
        *VJ1 = coldCart_mp -> getSISVoltageSetting(pol, 1);
    if (coldCart_mp && VJ2)
        *VJ2 = coldCart_mp -> getSISVoltageSetting(pol, 2);
    if (WCA_mp && VD)
        *VD = WCA_mp -> getPADrainVoltageSetting(pol); 
    return true;
}

bool CartAssembly::setAMC(const float *VDE_p, const float *VGE_p) {
    if (!checkWCA("CartAssembly::setAMC"))
        return false;

    float VDE = (VDE_p) ? *VDE_p : 1.0;
    float VGE = (VGE_p) ? *VGE_p : -0.2;
    WCA_mp -> setAMC(VDE, VGE);
    return true;
}

bool CartAssembly::measureFineLOSweep(FrontEndDatabase &dbObject, const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus, float tiltAngle,
        int pol, const float *VJ_p, const float *IJ_p, const float *fixedVD_p, const double *LOStart_p, const double *LOStop_p, const double *LOStep_p, int repeatCount)
{
    if (!checkColdCart("CartAssembly::measureFineLOSweep")) {
        string msg("MeasureFineLOSweep ERROR: this cartridge assembly has no cold cartridge.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
    if (!checkWCA("CartAssembly::measureFineLOSweep")) {
        string msg("MeasureFineLOSweep ERROR: this cartridge assembly has no WCA.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
    if (!coldCart_mp -> getSISEnableSetting()) {
        string msg("MeasureFineLOSweep ERROR: error SIS is not enabled.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
    if (!WCA_mp -> getPAEnableSetting()) {
        string msg("MeasureFineLOSweep ERROR: LO PA is not enabled.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    // For any parameters not provided, read the default values from the ini file:
    CIniFile configINI(FineLoSweepIni_m);
    configINI.ReadFile();
    string tmp, sectionName("Band");

    sectionName += to_string(band_m);

    float VJ(0.0), IJ(0.0);
    double LOStart(0.0), LOStop(0.0), LOStep(0.1);

    if (VJ_p)
        VJ = *VJ_p;
    else {
        tmp = configINI.GetValue(sectionName, "SISVoltage");
        if (!tmp.empty())
            VJ = from_string<float>(tmp);
    }
    if (IJ_p)
        IJ = *IJ_p;
    else {
        tmp = configINI.GetValue(sectionName, "SISCurrent");
        if (!tmp.empty())
            IJ = from_string<float>(tmp);
    }
    if (LOStart_p)
        LOStart = *LOStart_p;
    else {
        tmp = configINI.GetValue(sectionName, "LOStart");
        if (!tmp.empty())
            LOStart = from_string<double>(tmp);
    }
    if (LOStop_p)
        LOStop = *LOStop_p;
    else {
        tmp = configINI.GetValue(sectionName, "LOStop");
        if (!tmp.empty())
            LOStop = from_string<double>(tmp);
    }
    if (LOStep_p)
        LOStep = *LOStep_p;
    else {
        tmp = configINI.GetValue(sectionName, "LOStep");
        if (!tmp.empty())
            LOStep = from_string<double>(tmp);
    }

    if (VJ == 0.0 || IJ == 0.0) {
        string msg("MeasureFineLOSweep ERROR: Invalid settings for SISVoltage or SISCurrent.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    if (LOStart == 0.0 || LOStop == 0.0 || LOStep == 0.0) {
        string msg("MeasureFineLOSweep ERROR: Invalid settings for LO stepping.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    LOG(LM_INFO) << fixed << setprecision(3) << "CartAssembly::measureFineLOSweep VJ=" << VJ << " IJ=" << IJ
                 << " LOStart=" << LOStart << " LOStop=" << LOStop << " LOStep=" << LOStep << endl;

    if (!measureLOSweep_mp)
        measureLOSweep_mp = new MeasureFineLOSweep(dbObject, *this, *XYData_mp);
    
    if (measureLOSweep_mp -> start(feConfig, dataStatus, tiltAngle, pol, VJ, IJ, fixedVD_p, LOStart, LOStop, LOStep, repeatCount))
        return true;

    LOG(LM_ERROR) << "CartAssembly::measureFineLOSweep failed." << endl;
    return false;
}

bool CartAssembly::measureFineLOSweepSingleSynchronous(int pol, float VJ, float IJ, float fixedVD, float VDNom, double LOStart, double LOStop, double LOStep,
        XYPlotArray &sisCurrents, XYPlotArray &loPaVoltages)
{
    // check whether we've already been told to stop:
    if (measureLOSweep_mp && measureLOSweep_mp -> stopRequested()) {
        string msg("MeasureFineLOSweep: process stopped.");
        LOG(LM_ERROR) << msg << endl;
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    // setup stepping parameters:
    if (LOStop < LOStart) {
        float t(LOStart);
        LOStart = LOStop;
        LOStop = t;
    }
    double span = LOStop - LOStart;
    span = fabs(span);
    LOStep = fabs(LOStep);

    if (LOStep == 0)
        LOStep = 0.1;
    else if (LOStep > span)
        LOStep = span;

    int steps = (int) (span / LOStep) + 1;

    // Clear the result data set and reserve space:
    sisCurrents.clear();
    sisCurrents.reserve(steps);
    loPaVoltages.clear();
    loPaVoltages.reserve(steps);

    // Set the SIS voltages:
    coldCart_mp ->setSISVoltage(pol, 1, VJ);
    coldCart_mp ->setSISVoltage(pol, 2, VJ);

    // Set the PLL zero integrator switch:
    nullPLLIntegrator(true);

    // Get the existing floog and lock sideband settings:
    int sbLock;
    double freqFLOOG;
    getLOLockSettings(freqFLOOG, sbLock);

    // Set up the progress bar variables:
    FEMCEventQueue::addProgressEvent(0);
    float progress = 0.0;
    float progressIncrement = 100.0 / steps;

    // loop on LO frequency:
    bool error = false;
    double freqLO = LOStart;
    float VD = VDNom;
    bool hasSb2 = coldCart_mp -> hasSb2();

    for (int step = 0; !error && step < steps; ++step) {

        if (measureLOSweep_mp && measureLOSweep_mp -> stopRequested()) {
            string msg("MeasureFineLOSweep: process stopped.");
            LOG(LM_ERROR) << msg << endl;
            FEMCEventQueue::addStatusMessage(false, msg);
            error = true;

        // Set the LO frequency:
        } else if (!setLOFrequency(freqLO, freqFLOOG, sbLock)) {
            string msg("MeasureFineLOSweep ERROR: failed setting LO frequency to ");
            msg += to_string(freqLO);
            msg += " GHz.  Frequency out of range?";
            LOG(LM_ERROR) << msg << endl;
            FEMCEventQueue::addStatusMessage(false, msg);
            error = true;

        // If measuring at fixed drain voltage, set it:
        } else if (fixedVD >= 0) {
            WCA_mp -> setPADrainVoltage(pol, fixedVD);

            VD = fixedVD;

        // Adjust the LO PA to get the target junction current:
        } else if (!adjustLOPASinglePol(pol, VD, IJ, false)) {
            string msg("MeasureFineLOSweep ERROR: search for PA drain voltage failed at LO frequency ");
            msg += to_string(freqLO);
            msg += " GHz.";
            LOG(LM_ERROR) << msg << endl;
            FEMCEventQueue::addStatusMessage(false, msg);

            VD = VDNom;
        }
        if (!error) {
            // Store the latest settings and readings:
            float IJ1 = 1000.0 * coldCart_mp -> getSISCurrent(pol, 1, 3);
            float IJ2 = (hasSb2) ? (1000.0 * coldCart_mp -> getSISCurrent(pol, 2, 3)) : 0.0;
            float VDRead = WCA_mp -> getPADrainVoltage(pol);
            sisCurrents.push_back(XYPlotPoint(freqLO, IJ1, IJ2));
            loPaVoltages.push_back(XYPlotPoint(freqLO, VD, VDRead));
        }
        // Report progress:
        progress += progressIncrement;
        FEMCEventQueue::addProgressEvent((int) progress);

        // Go to the next LO step:
        freqLO += LOStep;
    }
    return !error;
}

bool CartAssembly::measureIVCurve(int pol, int sb, const float *VJlow_p, const float *VJhigh_p, const float *VJstep_p, int repeatCount) {
    if (!coldCart_mp -> getSISEnableSetting()) {
        LOG(LM_ERROR) << "CartAssembly::measureIVCurve error SIS is not enabled." << endl;
        return false;
    }
    
    if (pol < -1 || pol > 1)			// -1 means measure both pols.
    	return false;

    if (sb < -1 || sb == 0 || sb > 2)	// -1 means measure both sbs.  0 is an illegal value.
    	return false;

    float VJLow_default = 0;
    float VJHigh_default = 0;
    float VJStep_default = 0;

    getIVCurveDefaults(pol, sb, &VJLow_default, &VJHigh_default, &VJStep_default);
    
    float VJlow = (VJlow_p) ? *VJlow_p : VJLow_default;
    float VJhigh = (VJhigh_p) ? *VJhigh_p : VJHigh_default;
    float VJstep = (VJstep_p) ? *VJstep_p : VJStep_default;

    if (!measureIV_mp)
        measureIV_mp = new MeasureIVCurve(*this, *XYData_mp);
    
    if (measureIV_mp -> start(pol, sb, VJlow, VJhigh, VJstep, repeatCount))
        return true;
    
    LOG(LM_ERROR) << "CartAssembly::measureIVCurve failed." << endl;
    return false;
}

bool CartAssembly::measureIVCurveSingleSynchronous(int pol, int sb, float VJlow, float VJhigh, float VJstep) {
    if (coldCart_mp -> measureIVCurve(*XYData_mp, pol, sb, VJlow, VJhigh, VJstep)) {
		FEMCEventQueue::addEvent(FEMCEventQueue::Event(FEMCEventQueue::EVENT_IVCURVE_DONE, band_m, -1, 0, 100));
		return true;
    }
    return false;
}

bool CartAssembly::getIVCurveDefaults(int pol, int sb, float *VJlow_p, float *VJhigh_p, float *VJstep_p) {

    float VJMax = 3.0;
    switch (band_m) {
        case 4:
            VJMax = 6.5;
            break;

        case 3:
        case 6:
            VJMax = 12.0;
            break;

        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
        default:
            VJMax = 3.0;
            break;
    }
    if (VJlow_p)
        *VJlow_p = -VJMax;
    if (VJhigh_p)
        *VJhigh_p = VJMax;
    if (VJstep_p) {
        *VJstep_p = (2 * VJMax) / 401;
        if (*VJstep_p < 0.001)
            *VJstep_p = 0.001;
    }
    return true;
}

bool CartAssembly::measureIJvsSISMagnet(int pol, int sbMixer,
                                        float IMagStart, float IMagStop, float IMagStep,
                                        float VJLow, float VJHigh, float VJStep,
                                        int repeatCount)
{
    if (!checkColdCart("CartAssembly::measureIJvsSISMagnet"))
        return false;

    if (!measureSISCurrent_mp)
        measureSISCurrent_mp = new MeasureSISCurrent(*coldCart_mp, *XYData_mp);

    if (measureSISCurrent_mp -> start(pol, sbMixer, IMagStart, IMagStop, IMagStep, VJLow, VJHigh, VJStep, repeatCount))
        return true;
    
    LOG(LM_ERROR) << "CartAssembly::measureIJvsSISMagnet failed." << endl;
    return false;
}

bool CartAssembly::measureIFPowerVsVJ(int pol, int sb, const std::string &description,
                                      float VJStart, float VJStop, float VJStep,
                                      bool doYFactor, int repeatCount)
{
    if (!checkColdCart("CartAssembly::measureIFPowerVsVJ"))
        return false;
    if (!checkWCA("CartAssembly::measureIFPowerVsVJ"))
        return false;
    
    if (!measureIFPower_mp)
        measureIFPower_mp = new MeasureIFPower(*coldCart_mp, *WCA_mp, *XYData_mp);

    if (measureIFPower_mp -> startVsVJ(pol, sb, description, VJStart, VJStop, VJStep, doYFactor, repeatCount))
        return true;
    
    LOG(LM_ERROR) << "CartAssembly::measureIFPowerVsVJ failed." << endl;
    return false;
}

bool CartAssembly::measureIFPowerVsVD(int pol, const std::string &description,
                                      float VDStart, float VDStop, float VDStep, 
                                      bool doYFactor, int repeatCount)
{
    if (!checkColdCart("CartAssembly::measureIFPowerVsVD"))
        return false;
    if (!checkWCA("CartAssembly::measureIFPowerVsVD"))
        return false;

    if (!measureIFPower_mp)
        measureIFPower_mp = new MeasureIFPower(*coldCart_mp, *WCA_mp, *XYData_mp);

    if (measureIFPower_mp -> startVsVD(pol, description, VDStart, VDStop, VDStep, doYFactor, repeatCount))
        return true;
    
    LOG(LM_ERROR) << "CartAssembly::measureIFPowerVsVD failed." << endl;
    return false;
}

bool CartAssembly::mixerDeflux(int pol, int sb, int IMagMax) {
    if (!checkColdCart("CartAssembly::mixerDeflux"))
        return false;
    int caBand = coldCart_mp -> getBand();

    if (caBand == 4) {
        LOG(LM_INFO) << "CartAssembly::mixerDeflux: only heating allowed for band 4.  Calling MixerHeating process..." << endl;
        return setEnableSISHeaters(true, pol);

    } else if (caBand < 5 || caBand > 10) {
        LOG(LM_INFO) << "CartAssembly::mixerDeflux not allowed for band " << caBand << endl;
        return false;
    }

    if (!mixerDefluxer_mp)
        mixerDefluxer_mp = new MixerDeflux(*this, *XYData_mp);
    
    if (mixerDefluxer_mp -> start(pol))
        return true;
    
    LOG(LM_INFO) << "CartAssembly::mixerDeflux failed." << endl;
    return false;
}

bool CartAssembly::prepareHealthCheck(FrontEndDatabase &dbObject, const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus,
        double &freqLOret, bool receiverIsCold, int warmUpTimeSeconds, bool includeIFPower)
{
    if (!cartHealthCheck_mp)
        cartHealthCheck_mp = new CartHealthCheck(dbObject, *this, *XYData_mp);

    freqLOret = 0.0;
    return cartHealthCheck_mp -> prepare(feConfig, dataStatus, freqLOret, receiverIsCold, warmUpTimeSeconds, includeIFPower);
}

bool CartAssembly::startHealthCheck() {
    if (!cartHealthCheck_mp)
        return false;

    return cartHealthCheck_mp -> start();
}

void CartAssembly::abortMeasurement() {
    if (optimizerPA_mp)
        optimizerPA_mp -> requestStop();
    if (measureLOSweep_mp)
        measureLOSweep_mp -> requestStop();
    if (measureIV_mp)
        measureIV_mp -> requestStop();
    if (measureSISCurrent_mp)
        measureSISCurrent_mp -> requestStop();
    if (measureIFPower_mp)
        measureIFPower_mp -> requestStop();
    if (optimizerIFPower_mp)
        optimizerIFPower_mp -> requestStop();
    if (mixerDefluxer_mp)
        mixerDefluxer_mp -> requestStop();
    if (mixerHeater_mp)
        mixerHeater_mp -> requestStop();
}

bool CartAssembly::setEnableLNALEDs(bool enable) {
    if (!checkColdCart("CartAssembly::enableLNALEDs"))
        return false;
    if (!enable) {
        LOG(LM_INFO) << "Disabling LNA_LED_POL0." << endl;
        coldCart_mp -> lnaLedPol0Enable(false);
        LOG(LM_INFO) << "Disabling LNA_LED_POL1." << endl;
        coldCart_mp -> lnaLedPol1Enable(false);
    } else {
        LOG(LM_INFO) << "Enabling LNA_LED_POL0." << endl;
        coldCart_mp -> lnaLedPol0Enable(true);
        LOG(LM_INFO) << "Enabling LNA_LED_POL1." << endl;
        coldCart_mp -> lnaLedPol1Enable(true);
    }
    return true;
}

bool CartAssembly::setEnableSISHeaters(bool enable, int pol, float targetTemp, int timeout) {
    if (!checkColdCart("CartAssembly::setEnableSISHeaters"))
        return false;
    int band = coldCart_mp -> getBand();
    // bands 3-10 have heaters. (3 & 4 don't have magnets):
    if (!ColdCartImpl::hasSISHeater(band)) {
        if (enable == true) {
            string msg("setEnableSISHeaters ERROR: no SIS heaters in band ");
            msg += to_string(band);
            FEMCEventQueue::addStatusMessage(false, msg);
        }
        return false;
    }
    if (pol < -1 || pol > 1)
        return false;

    if (!enable) {
        if (pol == 0 || pol == -1) {
            LOG(LM_INFO) << "Disabling SIS_HEATER_POL0." << endl;
            coldCart_mp -> sisHeaterPol0Enable(false);
        }
        if (pol == 1 || pol == -1) {
            LOG(LM_INFO) << "Disabling SIS_HEATER_POL1." << endl;
            coldCart_mp -> sisHeaterPol1Enable(false);
        }
        if (mixerHeater_mp)
            mixerHeater_mp -> requestStop();
        
    } else {
        if (!mixerHeater_mp)
            mixerHeater_mp = new MixerHeating(*coldCart_mp);
    
        if (!mixerHeater_mp -> start(pol, targetTemp, timeout))
            return false;
    }
    return true;
}

bool CartAssembly::setEnablePhotomixer(bool enable) {
    if (!checkWCA("CartAssembly::setEnablePhotomixer"))
        return false;
    if (!enable)
        LOG(LM_INFO) << "Disabling PHOTOMIXER." << endl;
    else
        LOG(LM_INFO) << "Enabling PHOTOMIXER." << endl;
    WCA_mp -> photomixerEnable(enable);
    return true;
}

bool CartAssembly::getEnablePhotomixer() const {
    if (!checkWCA(NULL))
        return false;
    return WCA_mp -> getPhotomixerEnableSetting(); 
}

//----------------------------------------------------------------------------

const std::string &CartAssembly::asString(std::string &target) const {
    if (band_m <= 0)
        target.erase();
    else {
        char buf[10];
        sprintf(buf, "(%d:", band_m);
        target = buf;        
        if (coldCart_mp)
            target += coldCart_mp -> getESN();
        target += " ";
        if (WCA_mp)
            target += WCA_mp -> getESN();
        target += " ";
        sprintf(buf, "%s%s) ", (enable_m) ? "E" : " ", (observing_m) ? "*" : " ");
        target += buf;
    }
    return target;                             
}

void CartAssembly::appendThermalLogHeaderImpl(std::string &target) {
    target += "\tcartridge band";
    WCAImpl::appendThermalLogHeaderImpl(target);
    ColdCartImpl::appendThermalLogHeaderImpl(target);
}

void CartAssembly::appendThermalLog(std::string &target) const {
    if (!getEnable())
        return;
    else {
        char buf[10];
        sprintf(buf, "\t%d", band_m);
        target += buf;
        if (WCA_mp)
            WCA_mp -> appendThermalLog(target);
        else
            WCAImpl::appendThermalLogPlaceholder(target);
        if (coldCart_mp)
            coldCart_mp -> appendThermalLog(target);
        else
            ColdCartImpl::appendThermalLogPlaceholder(target);
    }
}  

bool CartAssembly::checkWCA(const char *caller_name) const {
    if (!WCA_mp) {
        if (caller_name)
            LOG(LM_INFO) << "Band " << band_m << ": " << string(caller_name) << " called without a valid WCA." << endl;
        return false;
    }
    return true;
}

bool CartAssembly::checkColdCart(const char *caller_name) const {
    if (!coldCart_mp) {
        if (caller_name)
            LOG(LM_INFO) << "Band " << band_m << ": " << string(caller_name) << " called without a valid coldCart." << endl;
        return false;
    }
    return true;
}
