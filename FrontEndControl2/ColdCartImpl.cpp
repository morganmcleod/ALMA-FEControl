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
 * implementation class for the Cold Cartridge
 *----------------------------------------------------------------------
 */

#include "ColdCartImpl.h"
#include "FEMCEventQueue.h"
#include "logger.h"
#include "setTimeStamp.h"
#include "stringConvert.h"
#include "LOGGER/logDir.h"
#include "OPTIMIZE/XYPlotArray.h"
using namespace FEConfig;
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <vector>
using namespace std;

ColdCartImpl::ColdCartImpl(unsigned long channel, 
                           unsigned long nodeAddress,
                           const std::string &name,
                           int port, 
                           int band, 
                           const std::string &ESN)
  : ColdCartImplBase(name, port),
    band_m(band),
    mixerHeatingdataFile_mp(NULL)
{ 
    reset();
    setESN(ESN);
    ColdCartImplBase::initialize(channel, nodeAddress); 

    // Add all the analog monitor points to the registry:
    // EXCEPT SIS current monitoring which we handle separately to do averaging.
    //addMon(&sisPol0Sb1Current_value, &ColdCartImplBase::sisPol0Sb1Current);
    //addMon(&sisPol0Sb2Current_value, &ColdCartImplBase::sisPol0Sb2Current);
    //addMon(&sisPol1Sb1Current_value, &ColdCartImplBase::sisPol1Sb1Current);
    //addMon(&sisPol1Sb2Current_value, &ColdCartImplBase::sisPol1Sb2Current);
    addMon(&sisPol0Sb1Voltage_value, &ColdCartImplBase::sisPol0Sb1Voltage);
    addMon(&sisPol0Sb2Voltage_value, &ColdCartImplBase::sisPol0Sb2Voltage);
    addMon(&sisPol1Sb1Voltage_value, &ColdCartImplBase::sisPol1Sb1Voltage);
    addMon(&sisPol1Sb2Voltage_value, &ColdCartImplBase::sisPol1Sb2Voltage);
    // SIS magnet monitoring we override in this class:
    addMon(&sisMagnetPol0Sb1Voltage_value, &sisMagnetPol0Sb1Voltage);
    addMon(&sisMagnetPol0Sb1Current_value, &sisMagnetPol0Sb1Current);
    addMon(&sisMagnetPol0Sb2Voltage_value, &sisMagnetPol0Sb2Voltage);
    addMon(&sisMagnetPol0Sb2Current_value, &sisMagnetPol0Sb2Current);
    addMon(&sisMagnetPol1Sb1Voltage_value, &sisMagnetPol1Sb1Voltage);
    addMon(&sisMagnetPol1Sb1Current_value, &sisMagnetPol1Sb1Current);
    addMon(&sisMagnetPol1Sb2Voltage_value, &sisMagnetPol1Sb2Voltage);
    addMon(&sisMagnetPol1Sb2Current_value, &sisMagnetPol1Sb2Current);
    // LNA monitoring handled by base class:
    addMon(&lnaPol0Sb1St1DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb1St1DrainVoltage);
    addMon(&lnaPol0Sb1St1DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb1St1DrainCurrent);
    addMon(&lnaPol0Sb1St1GateVoltage_value, &ColdCartImplBase::lnaPol0Sb1St1GateVoltage);
    addMon(&lnaPol0Sb1St2DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb1St2DrainVoltage);
    addMon(&lnaPol0Sb1St2DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb1St2DrainCurrent);
    addMon(&lnaPol0Sb1St2GateVoltage_value, &ColdCartImplBase::lnaPol0Sb1St2GateVoltage);
    addMon(&lnaPol0Sb1St3DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb1St3DrainVoltage);
    addMon(&lnaPol0Sb1St3DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb1St3DrainCurrent);
    addMon(&lnaPol0Sb1St3GateVoltage_value, &ColdCartImplBase::lnaPol0Sb1St3GateVoltage);
    addMon(&lnaPol0Sb2St1DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb2St1DrainVoltage);
    addMon(&lnaPol0Sb2St1DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb2St1DrainCurrent);
    addMon(&lnaPol0Sb2St1GateVoltage_value, &ColdCartImplBase::lnaPol0Sb2St1GateVoltage);
    addMon(&lnaPol0Sb2St2DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb2St2DrainVoltage);
    addMon(&lnaPol0Sb2St2DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb2St2DrainCurrent);
    addMon(&lnaPol0Sb2St2GateVoltage_value, &ColdCartImplBase::lnaPol0Sb2St2GateVoltage);
    addMon(&lnaPol0Sb2St3DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb2St3DrainVoltage);
    addMon(&lnaPol0Sb2St3DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb2St3DrainCurrent);
    addMon(&lnaPol0Sb2St3GateVoltage_value, &ColdCartImplBase::lnaPol0Sb2St3GateVoltage);
    addMon(&lnaPol1Sb1St1DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb1St1DrainVoltage);
    addMon(&lnaPol1Sb1St1DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb1St1DrainCurrent);
    addMon(&lnaPol1Sb1St1GateVoltage_value, &ColdCartImplBase::lnaPol1Sb1St1GateVoltage);
    addMon(&lnaPol1Sb1St2DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb1St2DrainVoltage);
    addMon(&lnaPol1Sb1St2DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb1St2DrainCurrent);
    addMon(&lnaPol1Sb1St2GateVoltage_value, &ColdCartImplBase::lnaPol1Sb1St2GateVoltage);
    addMon(&lnaPol1Sb1St3DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb1St3DrainVoltage);
    addMon(&lnaPol1Sb1St3DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb1St3DrainCurrent);
    addMon(&lnaPol1Sb1St3GateVoltage_value, &ColdCartImplBase::lnaPol1Sb1St3GateVoltage);
    addMon(&lnaPol1Sb2St1DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb2St1DrainVoltage);
    addMon(&lnaPol1Sb2St1DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb2St1DrainCurrent);
    addMon(&lnaPol1Sb2St1GateVoltage_value, &ColdCartImplBase::lnaPol1Sb2St1GateVoltage);
    addMon(&lnaPol1Sb2St2DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb2St2DrainVoltage);
    addMon(&lnaPol1Sb2St2DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb2St2DrainCurrent);
    addMon(&lnaPol1Sb2St2GateVoltage_value, &ColdCartImplBase::lnaPol1Sb2St2GateVoltage);
    addMon(&lnaPol1Sb2St3DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb2St3DrainVoltage);
    addMon(&lnaPol1Sb2St3DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb2St3DrainCurrent);
    addMon(&lnaPol1Sb2St3GateVoltage_value, &ColdCartImplBase::lnaPol1Sb2St3GateVoltage);
    addMon(&sisHeaterPol0Current_value, &ColdCartImplBase::sisHeaterPol0Current);
    addMon(&sisHeaterPol1Current_value, &ColdCartImplBase::sisHeaterPol1Current);
    addMon(&cartridgeTemperature0_value, &ColdCartImplBase::cartridgeTemperature0);
    addMon(&cartridgeTemperature1_value, &ColdCartImplBase::cartridgeTemperature1);
    addMon(&cartridgeTemperature2_value, &ColdCartImplBase::cartridgeTemperature2);
    addMon(&cartridgeTemperature3_value, &ColdCartImplBase::cartridgeTemperature3);
    addMon(&cartridgeTemperature4_value, &ColdCartImplBase::cartridgeTemperature4);
    addMon(&cartridgeTemperature5_value, &ColdCartImplBase::cartridgeTemperature5);

    nextMon = monitorRegistry.begin();
}

const std::string &ColdCartImpl::getBandText(std::string &toFill) const {
    toFill = to_string(band_m);
    toFill = "band" + toFill;
    return toFill;
}

void ColdCartImpl::reset() {
    sisEnableSet_m = false;
    sisMagnetEnableSet_m = false;
    sisVoltageSet_m[0] = sisVoltageSet_m[1] = sisVoltageSet_m[2] = sisVoltageSet_m[3] = 0.0;
    sisVoltageError_m[0] = sisVoltageError_m[1] = sisVoltageError_m[2] = sisVoltageError_m[3] = 0.0;
    sisMagnetCurrentSet_m[0] = sisMagnetCurrentSet_m[1] = sisMagnetCurrentSet_m[2] = sisMagnetCurrentSet_m[3] = 0.0;
    lnaEnableSet_m[0] = lnaEnableSet_m[1] = lnaEnableSet_m[2] = lnaEnableSet_m[3] = false;
    resetMixerHeating();
}

void ColdCartImpl::queryCartridgeState() {
    int status;
    // Query the FEMC firmware for the SIS and magnet settings:
    if (hasSIS()) {
        // SIS1 voltage settings:
        status = syncMonitorWithRetry(sisPol0Sb1Voltage_RCA + 0x10000, sisVoltageSet_m[0]);
        status = syncMonitorWithRetry(sisPol1Sb1Voltage_RCA + 0x10000, sisVoltageSet_m[2]);
        // SIS enabled:
        sisEnableSet_m = (fabsf(sisVoltageSet_m[0]) > 0.2 || fabsf(sisVoltageSet_m[2]) > 0.2);
        // SIS1 magnet current settings:
        status = syncMonitorWithRetry(sisMagnetPol0Sb1Current_RCA + 0x10000, sisMagnetCurrentSet_m[0]);
        status = syncMonitorWithRetry(sisMagnetPol1Sb1Current_RCA + 0x10000, sisMagnetCurrentSet_m[2]);
        // SIS1 magnet enabled:
        sisMagnetEnableSet_m = (fabsf(sisMagnetCurrentSet_m[0]) > 0.2 || fabsf(sisMagnetCurrentSet_m[2]) > 0.2);

        if (hasSb2()) {
            // SIS2 voltage settings:
            status = syncMonitorWithRetry(sisPol0Sb2Voltage_RCA + 0x10000, sisVoltageSet_m[1]);
            status = syncMonitorWithRetry(sisPol1Sb2Voltage_RCA + 0x10000, sisVoltageSet_m[3]);
            // SIS2 magnet current settings:
            status = syncMonitorWithRetry(sisMagnetPol0Sb2Current_RCA + 0x10000, sisMagnetCurrentSet_m[1]);
            status = syncMonitorWithRetry(sisMagnetPol1Sb2Current_RCA + 0x10000, sisMagnetCurrentSet_m[3]);

        }
    }
    // Query for the LNA settings:
    if (hasLNA()) {
        // LNA1s enabled:
        status = syncMonitorWithRetry(lnaPol0Sb1Enable_RCA + 0x10000, lnaEnableSet_m[0]);
        status = syncMonitorWithRetry(lnaPol1Sb1Enable_RCA + 0x10000, lnaEnableSet_m[2]);
        if (hasSb2()) {
            // LNA2s enabled:
            status = syncMonitorWithRetry(lnaPol0Sb2Enable_RCA + 0x10000, lnaEnableSet_m[1]);
            status = syncMonitorWithRetry(lnaPol1Sb2Enable_RCA + 0x10000, lnaEnableSet_m[3]);
        }
    }

    LOG(LM_INFO) << "ColdCartImpl::queryCartridgeState: port=" << port_m << " band=" << band_m 
        << " sisEnabled=" << sisEnableSet_m 
        << " sisMagnetEnabled=" << sisMagnetEnableSet_m << endl 
        << "    lna01=" << lnaEnableSet_m[0] << " lna02=" << lnaEnableSet_m[1] 
        << " lna11=" << lnaEnableSet_m[2] << " lna12=" << lnaEnableSet_m[3] <<  endl
        << "    sisV01=" << sisVoltageSet_m[0] << " sisV02=" << sisVoltageSet_m[1] 
        << " sisV11=" << sisVoltageSet_m[2] << " sisV12=" << sisVoltageSet_m[3] <<  endl
        << "    Imag01=" << sisMagnetCurrentSet_m[0] << " Imag02=" << sisMagnetCurrentSet_m[1] 
        << " Imag11=" << sisMagnetCurrentSet_m[2] << " Imag12=" << sisMagnetCurrentSet_m[3] <<  endl;
}

void ColdCartImpl::sisPol0Sb1Voltage(float val) {
    if (hasSIS()) {
        sisVoltageSet_m[0] = val;
        val -= sisVoltageError_m[0];
        ColdCartImplBase::sisPol0Sb1Voltage(val);
    }
}

void ColdCartImpl::sisPol0Sb2Voltage(float val) {
    if (hasSb2() && hasSIS()) {
        sisVoltageSet_m[1] = val;
        val -= sisVoltageError_m[1];
        ColdCartImplBase::sisPol0Sb2Voltage(val);
    }    
}

void ColdCartImpl::sisPol1Sb1Voltage(float val) {
    if (hasSIS()) {
        sisVoltageSet_m[2] = val;
        val -= sisVoltageError_m[2];
        ColdCartImplBase::sisPol1Sb1Voltage(val);
    }
}

void ColdCartImpl::sisPol1Sb2Voltage(float val) {
    if (hasSb2() && hasSIS()) {
        sisVoltageSet_m[3] = val;
        val -= sisVoltageError_m[3];
        ColdCartImplBase::sisPol1Sb2Voltage(val);
    }    
}

void ColdCartImpl::setSISVoltage(int pol, int sb, float val, bool sweep) {
    // ignore if no SIS:
    if (!hasSIS())
        return;

    // ignore sweep parameter if this is band 6 or below:
    if (band_m <= 6)
        sweep = false;
    
    // get the prior value as valNow:
    float valNow = getSISVoltageSetting(pol, sb);
    
    // decide whether to sweep from the current value to the new value:    
    float span = val - valNow;
    if (sweep && span != 0.0) {
        float neg = (span < 0);
        float step = (neg) ? -0.05 : 0.05;
        // loop from valNow to the final value:
        bool done = false;
        while (!done) {
            if ((neg && valNow <= val) || (!neg && valNow >= val)) 
                done = true;
            else {
                // set the junction voltage:
                if (pol == 0) {
                    if (sb == 1)
                        ColdCartImpl::sisPol0Sb1Voltage(valNow);
                    else if (sb == 2)
                        ColdCartImpl::sisPol0Sb2Voltage(valNow);
                } else if (pol == 1) {
                    if (sb == 1)
                        ColdCartImpl::sisPol1Sb1Voltage(valNow);
                    else if (sb == 2)
                        ColdCartImpl::sisPol1Sb2Voltage(valNow);
                }
                valNow += step;
            }
        }
    }
    // set the final value:
    if (pol == 0) {
        if (sb == 1)
            ColdCartImpl::sisPol0Sb1Voltage(val);
        else if (sb == 2)
            ColdCartImpl::sisPol0Sb2Voltage(val);
    } else if (pol == 1) {
        if (sb == 1)
            ColdCartImpl::sisPol1Sb1Voltage(val);
        else if (sb == 2)
            ColdCartImpl::sisPol1Sb2Voltage(val);
    }
}    

void ColdCartImpl::measureSISVoltageError(std::string *resultStr) {
    // ignore if no SIS:
    if (!hasSIS())
        return;

    // reset the offsets to zero:
    sisVoltageError_m[0] = sisVoltageError_m[1] = sisVoltageError_m[2] = sisVoltageError_m[3] = 0.0;
    float e0 = measureSISVoltageErrorImpl(0, 1);
    float e1 = measureSISVoltageErrorImpl(0, 2);
    float e2 = measureSISVoltageErrorImpl(1, 1);
    float e3 = measureSISVoltageErrorImpl(1, 2);
    string result = "(";
    result += to_string(e0);
    result += ", ";
    result += to_string(e1);
    result += ", ";
    result += to_string(e2);
    result += ", ";
    result += to_string(e3);
    result += ")";
    if (resultStr)
    	*resultStr = result;

    LOG(LM_INFO) << "ColdCartImpl: SIS voltage errors are " << result << endl;
}

float ColdCartImpl::measureSISVoltageErrorImpl(int pol, int sb) {
    if (!hasSIS())
        return 0.0;
    if (!checkPolSb(pol, sb)) 
        return 0.0;
    if (sb == 2 && !hasSb2())
        return 0.0;
    
    // do gentle voltage sweeps if band 7 or above:
    bool sweep = (band_m >= 7);
    int average = 100;
    float testVoltage;
    switch (band_m) {
        case 3:
            testVoltage = 10.0;
            break;
        case 4:
            testVoltage = 4.8;
            break;        
        case 5:
            testVoltage = 2.5;
            break;
        case 6:
            testVoltage = 9.0;
            break;
        case 7:
        case 8:
            testVoltage = 2.2;
            break;
        case 9:
            testVoltage = 2.3;
            break;
        case 10:
            testVoltage = 2.1;
            break;
        default:
            testVoltage = 2.2;
            break;
    }

    bool print = false; // Set to true to enable printing ALL of the individual readings.
    
    if (print)
        LOG(LM_INFO) << "measureSISVoltageErrorImpl pol=" << pol << " sis=" << sb << "..." << endl; 
    
    // clear the previous correction:
    sisVoltageError_m[pol * 2 + sb - 1] = 0.0;

    // cache the prior value as VJprior:
    float VJprior = getSISVoltageSetting(pol, sb);    
    
    // set the SIS voltage and pause:
    setSISVoltage(pol, sb, testVoltage, sweep);
    SLEEP(10);
    
    // measure the average readback voltage value:
    float VJerror = getSISVoltage(pol, sb, average, print) - testVoltage;

    // restore the prior value:
    setSISVoltage(pol, sb, VJprior, sweep);
    
    // store the correction offset to use:
    sisVoltageError_m[pol * 2 + sb - 1] = VJerror;
    return VJerror;
}

bool ColdCartImpl::measureIVCurve(XYPlotArray &target, int pol, int sb, float VJ1, float VJ2, float VJstep) {
    // clear the output data array:
    target.clear();

    if (!hasSIS()) {
        string msg("measureIVCurve ERROR: can't measure SIS for band ");
        msg += to_string(band_m);
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
        return false;
    }
    if (!getSISEnableSetting()) {
        string msg("measureIVCurve ERROR: the SIS is not enabled.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
        return false;
    }

    // clear the stop flag:
    ivCurveStop_m = false;

    // store the voltage setting in effect now:
    float VJnom = getSISVoltageSetting(pol, sb);

    // sort the VJ1, VJ2 inputs into min and max:
    if (VJ2 < VJ1)
        std::swap(VJ2, VJ2);

    // make VJstep positive for now:
    VJstep = fabsf(VJstep);

    // preallocate space in the output array:
    int steps = (int) ((VJ2 - VJ1) / VJstep) + 1;
    target.reserve(2 * steps + 1);

    // Sweep one or two ranges:
    bool VJ1Negative(VJ1 < 0);
    bool VJ2Positive(VJ2 > 0);
    bool zeroCrossing(VJ1Negative && VJ2Positive);

    // progress bar:
    int progress(0), progressStop(100);

    // Sweep first range from negative towards zero:
    if (VJ1Negative) {
        float endpt = (zeroCrossing) ? 0 : VJ2;

        // How much progress bar for first range?
        if (zeroCrossing)
            progressStop = int((fabsf(VJ1) / (VJ2 - VJ1)) * 100);

        if (!measureIVCurveInnerLoop(target, pol, sb, VJ1, endpt, VJstep, progress, progressStop)) {
            string msg("measureIVCurve ERROR in sweep first range.");
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
            return false;
        }
        progress = progressStop;
    }

    // Sweep second range from positive towards zero:
    if (VJ2Positive) {
        float endpt = (zeroCrossing) ? 0 : VJ1;

        // allocate a separate results array so we can reverse it below.
        XYPlotArray target2;
        int steps2 = (int) ((VJ2 - endpt) / VJstep) + 1;
        target2.reserve(steps2);

        if (!measureIVCurveInnerLoop(target2, pol, sb, VJ2, endpt, -VJstep, progress, 100)) {
            string msg("measureIVCurve ERROR in sweep second range.");
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
            return false;
        }
        // reverse the array:
        reverse(target2.begin(), target2.end());
        // and append it to the results:
        target.insert(target.end(), target2.begin(), target2.end());
    }

    // sweep the SIS voltage back to where it was:
    setSISVoltage(pol, sb, VJnom, true);

    FEMCEventQueue::addProgressEvent(100);
    return (!ivCurveStop_m);
}

bool ColdCartImpl::measureIVCurveInnerLoop(XYPlotArray &target, int pol, int sb,
                                           float VJstart, float VJstop, float VJstep,
                                           int progressStart, int progressEnd)
{
    // Inner loop measures in one direction only.
    // NOTE: no provision to ensure exactly VJstop is measured.

    // Sanity check inputs:
    // Range is at least 1 step
    float range = VJstop - VJstart;

    if (fabsf(range) < fabsf(VJstep))
        return false;

    // Sign of range is same as step
    bool range_neg(range < 0.0);
    bool step_neg(VJstep < 0.0);

    if (range_neg != step_neg)
        return false;

    // Progress bar variables:
    if (progressEnd <= progressStart)
        return false;

    // Setup timing and averaging
    const int stepSleep = 1;    // ms
    const int step0Sleep = 10;  // ms
    const int voltAveraging = 3;
    const int currAveraging = 3;

    // move slowly to the first point:
    float VJset = VJstart;
    setSISVoltage(pol, sb, VJset, true);
    SLEEP(step0Sleep);
    FEMCEventQueue::addProgressEvent(progressStart);

    // loop vars:
    float VJ, IJ;
    float progress(progressStart);
    float progressSpan(progressEnd - progressStart);
    float progressStep((fabsf(VJstep) / fabsf(range)) * progressSpan);
    int lastProgress(-1);
    int thisProgress;
    bool done = false;

    while (!done) {
        // move quickly from point to point:
        setSISVoltage(pol, sb, VJset, false);
        SLEEP(stepSleep);
        // read back the instantaneous voltage and current:
        VJ = getSISVoltage(pol, sb, voltAveraging);
        IJ = getSISCurrent(pol, sb, currAveraging) * 1000.0;  // convert mA to uA
        target.push_back(XYPlotPoint(VJset, VJ, IJ));

        // increment and loop end condition:
        VJset += VJstep;

        // report progress:
        progress += progressStep;
        thisProgress = int(progress);
        // avoid reporting any progress step more than once due to round-down:
        if (thisProgress > lastProgress) {
            lastProgress = thisProgress;
            if ((thisProgress % 5) == 0)
                FEMCEventQueue::addProgressEvent(thisProgress);
        }

        // check loop end condition:
        if (ivCurveStop_m)
            done = true;
        else if (step_neg) {
            if (VJset <= VJstop)
                done = true;
        } else {
            if (VJset >= VJstop)
                done = true;
        }
    }
    return true;
}

bool ColdCartImpl::saveIVCurveData(const XYPlotArray &source, const std::string logDir, int pol, int sb) const {
    if (logDir.empty()) {
        LOG(LM_ERROR) << "ColdCartImpl::saveIVCurveData: logDir is empty." << endl;
        return false;
    }

    // format the file name with the band, pol, mixer number, and timestamp:
    string fileName, bandTxt, polText, sbText;
    Time ts;
    setTimeStamp(&ts);
    timestampToText(&ts, fileName, true);
    getBandText(bandTxt);
    polText = to_string(pol);
    sbText = to_string(sb);
    fileName = logDir + "IVCurve-" + bandTxt + "-pol" + polText + "-sis" + sbText + "-" + fileName + ".txt";

    // attempt to open the file:
    ofstream dataFile(fileName.c_str(), ios_base::trunc);
    if (!dataFile.is_open()) {
        LOG(LM_ERROR) << "ColdCartImpl::saveIVCurveData: could not open dataFile for output: '" << fileName << "'." << endl;
        return false;
    }

    // write the file header row:
    dataFile << "VJ (set mV)\tVJ (mV)\tIJ (uA)" << endl;

    // write out the file data:
    for (XYPlotArray::const_iterator it = source.begin(); it != source.end(); ++it) {
        dataFile << (*it).X << "\t" << (*it).Y << "\t" << (*it).Y2 << endl;
    }
    dataFile.flush();
    dataFile.close();
    return true;
}

float ColdCartImpl::getSISVoltage(int pol, int sb, int average, bool print) {
    if (!hasSIS())
        return 0;
    if (!checkPolSb(pol, sb)) 
        return 0;
    if (average < 1)
        return 0;    
    
    // compute the average:
    double sum = 0;
    float val =  0;
    int i;
    vector<float> *data(print ? new vector<float>(average, 0.0) : NULL);
    for (i = 0; i < average; ++i) {
        if (pol == 0) {
            if (sb == 1)
                val = ColdCartImpl::sisPol0Sb1Voltage();
            else if (sb == 2)
                val = ColdCartImpl::sisPol0Sb2Voltage();
        } else if (pol == 1) {
            if (sb == 1)
                val = ColdCartImpl::sisPol1Sb1Voltage();
            else if (sb == 2)
                val = ColdCartImpl::sisPol1Sb2Voltage();
        }
        if (print)
            (*data)[i] = val;
        sum += val;
    }
    if (average > 1)
        sum /= (double) average;
    if (print) {
        for (i = 0; i < average; ++i)
            LOG(LM_INFO) << (*data)[i] << endl;
        delete data;
    }
    return (float) sum;
}

float ColdCartImpl::getSISCurrent(int pol, int sb, int average) {
    if (!hasSIS())
        return 0;
    if (!checkPolSb(pol, sb)) 
        return 0;
    if (average < 1)
        return 0;    

    // Call the averaging version of the base class function:
    if (pol == 0) {
        if (sb == 1)
            return avgSisPol0Sb1Current(average);
        else if (sb == 2)
            return avgSisPol0Sb2Current(average);
    } else if (pol == 1) {
        if (sb == 1)
            return avgSisPol1Sb1Current(average);
        else if (sb == 2)
            return avgSisPol1Sb2Current(average);
    }
    return 0;
}

bool ColdCartImpl::setSISEnable(bool val) {
    if (!hasSIS()) {
        if (val == true) {
            string msg("setSISEnable ERROR: no SIS mixers in band ");
            msg += to_string(band_m);
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
        }
        return false;
    }
    if (val == sisEnableSet_m)
        return true; // no change.
    sisEnableSet_m = val;
    if (!sisEnableSet_m) {
        // turn everything off:
        setSISVoltage(0, 1, 0.0);
        setSISVoltage(0, 2, 0.0);
        setSISVoltage(1, 1, 0.0);
        setSISVoltage(1, 2, 0.0);
    }
    return true;
}

void ColdCartImpl::sisMagnetPol0Sb1Current(float val) {
    if (hasMagnet()) {
        sisMagnetCurrentSet_m[0] = val;
        ColdCartImplBase::sisMagnetPol0Sb1Current(val);
    }
}

void ColdCartImpl::sisMagnetPol0Sb2Current(float val) {
    if (hasSb2() && hasMagnet()) {
        sisMagnetCurrentSet_m[1] = val;
        ColdCartImplBase::sisMagnetPol0Sb2Current(val);    
    }
}

void ColdCartImpl::sisMagnetPol1Sb1Current(float val) {
    if (hasMagnet()) {
        sisMagnetCurrentSet_m[2] = val;
        ColdCartImplBase::sisMagnetPol1Sb1Current(val);
    }
}

void ColdCartImpl::sisMagnetPol1Sb2Current(float val) {
    if (hasSb2() && hasMagnet()) {
        sisMagnetCurrentSet_m[3] = val;
        ColdCartImplBase::sisMagnetPol1Sb2Current(val);    
    }
}

void ColdCartImpl::setSISMagnetCurrent(int pol, int sb, float val, bool sweep, float sweepStep, unsigned sweepDwell) {
    if (!hasMagnet())
        return;
    if (!checkPolSb(pol, sb)) 
        return;

    // set a reasonable lower-limit on the sweep step-size:
    if (sweepStep < 0.001)
        sweepStep = 0.001;

    // cache the prior setting value as valNow:
    float valNow = getSISMagnetCurrentSetting(pol, sb);
    
    // decide whether to sweep from the current value to the new value:    
    float span = val - valNow;
    if (sweep && span != 0.0) {
        bool neg = (span < 0);
        float step = (neg) ? -sweepStep : sweepStep;
        // loop from valNow to the final value:
        bool done = false;
        while (!done) {
            if ((neg && valNow <= val) || (!neg && valNow >= val)) 
                done = true;
            else {
                // set the magnet current:
                if (pol == 0) {
                    if (sb == 1)
                        ColdCartImpl::sisMagnetPol0Sb1Current(valNow);
                    else if (sb == 2)
                        ColdCartImpl::sisMagnetPol0Sb2Current(valNow);
                } else if (pol == 1) {
                    if (sb == 1)
                        ColdCartImpl::sisMagnetPol1Sb1Current(valNow);
                    else if (sb == 2)
                        ColdCartImpl::sisMagnetPol1Sb2Current(valNow);
                }
                // sleep for the specified dwell time:
                SLEEP(sweepDwell);
                // and move to the next step:
                valNow += step;
            }
        }
    }
    // set the final value:
    if (pol == 0) {
        if (sb == 1)
            ColdCartImpl::sisMagnetPol0Sb1Current(val);
        else if (sb == 2)
            ColdCartImpl::sisMagnetPol0Sb2Current(val);
    } else if (pol == 1) {
        if (sb == 1)
            ColdCartImpl::sisMagnetPol1Sb1Current(val);
        else if (sb == 2)
            ColdCartImpl::sisMagnetPol1Sb2Current(val);
    }
}

float ColdCartImpl::getSISMagnetCurrent(int pol, int sb) {
    if (!hasMagnet())
        return 0;
    if (!checkPolSb(pol, sb)) 
        return 0;
    if (pol == 0) {
        if (sb == 1)
            return sisMagnetPol0Sb1Current();
        else if (sb == 2)
            return sisMagnetPol0Sb2Current();
    } else if (pol == 1) {
        if (sb == 1)
            return sisMagnetPol1Sb1Current();
        else if (sb == 2)
            return sisMagnetPol1Sb2Current();
    }
    return -1.0;
}

bool ColdCartImpl::setSISMagnetEnable(bool val) {
    if (!hasMagnet()) {
        if (val == true) {
            string msg("setSISMagnetEnable ERROR: no SIS magnets in band ");
            msg += to_string(band_m);
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
        }
        return false; // no SIS magnets.
    }
    if (val == sisMagnetEnableSet_m)
        return true; // no change.
    sisMagnetEnableSet_m = val;
    if (!sisMagnetEnableSet_m) {
        // turn everything off:
        LOG(LM_INFO) << "Setting ALL SIS_MAGNET_CURRENT to 0." << endl;
        setSISMagnetCurrent(0, 1, 0.0);
        setSISMagnetCurrent(0, 2, 0.0);
        setSISMagnetCurrent(1, 1, 0.0);
        setSISMagnetCurrent(1, 2, 0.0);
    }
    return true;
}

//-------------------------------------------------------------------------------------------------
// SIS heater operation

const float ColdCartImpl::mixerHeatingMinTargetTemp_m(4.0);
const int ColdCartImpl::mixerHeatingMaxTimeout_m(120);
const int ColdCartImpl::mixerHeatingMaxTimeoutBand9_m(3);

void ColdCartImpl::setSISHeaterEnable(int pol, bool enable) {
    if (!hasSISHeater())
        return;
    if (pol == 0)
        sisHeaterPol0Enable(enable);
    else if (pol == 1)
        sisHeaterPol1Enable(enable);
}

void ColdCartImpl::resetMixerHeating() {
    mixerHeatingStop_m = false;
    mixerHeatingtimerStart_m = 0;
    if (mixerHeatingdataFile_mp) {
        (*mixerHeatingdataFile_mp).flush();
        delete mixerHeatingdataFile_mp;
        mixerHeatingdataFile_mp = NULL;
    }
}

bool ColdCartImpl::sisMixerHeatingProcess(int pol, float targetTemp, int timeout) {
    resetMixerHeating();

    if (!hasSISHeater()) {
        string msg("MixerHeating: no mixer heaters in band ");
        msg += to_string(band_m);
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    if (!(pol == -1 || pol == 0 || pol == 1)) {
        string msg("MixerHeating: pol must be 0 or 1 or -1 to indicate both.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    if (targetTemp < 0.0 || targetTemp > 20.0) {
        // checking against 0.0 here instead of mixerHeatingMinTargetTemp_m to support option
        //   of always heating until the timeout occurs.
        string msg("MixerHeating: targetTemp must be between 0 and 20 K.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    if (timeout < 0 || timeout > mixerHeatingMaxTimeout_m) {
        string msg("MixerHeating: timeout must be between 0 and 120 seconds.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    if (band_m == 9 && timeout > mixerHeatingMaxTimeoutBand9_m) {
        timeout = mixerHeatingMaxTimeoutBand9_m;
        string msg("MixerHeating: for band 9 timeout is limited to ");
        msg += to_string<int>(mixerHeatingMaxTimeoutBand9_m);
        msg += " seconds.";
        FEMCEventQueue::addStatusMessage(true, msg);
        LOG(LM_INFO) << msg << endl;
    }

    LOG(LM_INFO) << "MixerHeating start: band=" << band_m << " pol=" << pol << " targetTemp=" << targetTemp << " timeout=" << timeout << endl;

    const string &logDir = FEConfig::getLogDir();
    if (!logDir.empty()) {
        string fileName, bandTxt, polText;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);
        getBandText(bandTxt);
        polText = to_string(pol);
        if (pol == -1)
            polText = "Both";
        fileName = logDir + "MixerHeating-" + bandTxt + "-pol" + polText + "-" + fileName + ".txt";
        mixerHeatingdataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
    }
    string msg("MixerHeating: process started.");
    FEMCEventQueue::addStatusMessage(true, msg);

    sisMixerHeatingReadData(true); // write column headers to output file.

    // make sure the SIS magnets are off:
    bool magnetsPriorState = getSISMagnetEnableSetting();
    if (magnetsPriorState) {
        string msg("MixerHeating ALERT: Turning off SIS magnets.  They will not automatically be re-enabled.");
        FEMCEventQueue::addStatusMessage(true, msg);
        LOG(LM_INFO) << msg << endl;
    }
    setSISMagnetEnable(false);  // this sweeps the magnet currents to zero.

    // measure the current and mixer temperatures with the heater switched off:
    setSISHeaterEnable(0, false);
    setSISHeaterEnable(1, false);
    float offCurrentPol0(0.0), offCurrentPol1(0.0);
    float offTemp0(0.0), offTemp1(0.0);
    for (int i = 0; i < 10; ++i) {
        offCurrentPol0 += getSISHeaterCurrent(0);
        offCurrentPol1 += getSISHeaterCurrent(1);
        offTemp0 += cartridgeTemperature2();
        offTemp1 += cartridgeTemperature5();
        SLEEP(50);
    }
    offCurrentPol0 /= 10.0;
    offCurrentPol1 /= 10.0;
    offTemp0 /= 10.0;
    offTemp1 /= 10.0;

    LOG(LM_INFO) << fixed << setprecision(2) << "MixerHeating: base temp pol0=" << offTemp0 << " pol1=" << offTemp1
                 << " base current pol0=" << offCurrentPol0 << " pol1=" << offCurrentPol1 << endl;

    float thresholdPol0(offCurrentPol0 + 1.0);  // threshold is off current + 1mA
    float thresholdPol1(offCurrentPol1 + 1.0);
    float thresholdOffTemp0(offTemp0 + 0.20);   // threshold is base temp + 0.20 K.
    float thresholdOffTemp1(offTemp1 + 0.20);

    // Initialize loop variables:
    float heaterCurrentPol0(0), heaterCurrentPol1(0);
    float mixerTempPol0(0), mixerTempPol1(0);
    bool targetReachedPol0(false), targetReachedPol1(false);
    bool heaterOnPol0(false), heaterOnPol1(false);
    bool timedOut(false);
    bool retrigger(false);
    bool done(false);

    // enable the heaters:
    // pol == -1 means operate both heaters simultaneously;  else pol is 0 or 1:
    if (pol == -1 || pol == 0)
        setSISHeaterEnable(0, true); // set pol0 heater control line ON
    if (pol == -1 || pol == 1)
        setSISHeaterEnable(1, true); // set pol1 heater control line ON

    timeout *= 1000;
    mixerHeatingtimerStart_m = GETTIMER();  // get the timer value in ms.
    sisMixerHeatingReadData(); // write mixer temps and heater currents to output file.

    while (!done && !mixerHeatingStop_m) {
        if (GETTIMER() - mixerHeatingtimerStart_m > timeout)
            timedOut = done = true;
        else {
            SLEEP(20);

            sisMixerHeatingReadData(); // write mixer temps and heater currents to output file.

            // read the mixer temperatures:
            mixerTempPol0 = cartridgeTemperature2();
            mixerTempPol1 = cartridgeTemperature5();

            // check mixer temps against the target temperature:
            if (targetTemp >= mixerHeatingMinTargetTemp_m) {
                targetReachedPol0 = (mixerTempPol0 >= targetTemp);
                targetReachedPol1 = (mixerTempPol1 >= targetTemp);
            } else
                // we will heat until the timeout occurs:
                targetReachedPol0 = targetReachedPol1 = false;

            // read the heater currents:
            heaterCurrentPol0 = getSISHeaterCurrent(0);
            heaterCurrentPol1 = getSISHeaterCurrent(1);

            // check heater currents against threshold:
            heaterOnPol0 = (heaterCurrentPol0 > thresholdPol0);
            heaterOnPol1 = (heaterCurrentPol1 > thresholdPol1);

            // adjust loop exit conditions based on whether heating both pols or just one:
            if (pol == 0)
                targetReachedPol1 = true;
            else if (pol == 1)
                targetReachedPol0 = true;

            //LOG(LM_INFO) << "MixerHeating: heaterCurrentPol0=" << heaterCurrentPol0 << " heaterCurrentPol1=" << heaterCurrentPol1
             //            << " heaterOnPol0=" << heaterOnPol0 << " heaterOnPol1=" << heaterOnPol1
              //           << " mixerTempPol0=" << mixerTempPol0 << " mixerTempPol1=" << mixerTempPol1
               //          << " targetReachedPol0=" << targetReachedPol0 << " targetReachedPol1=" << targetReachedPol1 << endl;

            // if target temp reached, exit now:
            if (targetReachedPol0 && targetReachedPol1)
                done = true;

            // check for whether heaters need to be re-triggered:
            else {
                retrigger = true;
                if (pol == 0 || pol == -1) {
                    if (heaterOnPol0)
                        retrigger = false;
                }
                if (pol == 1 || pol == -1) {
                    if (heaterOnPol1)
                        retrigger = false;
                }
                if (retrigger) {
                    //LOG(LM_INFO) << "MixerHeating: heaters have switched off." << endl;
                    // re-enable the heaters:
                    if (pol == -1 || pol == 0)
                        setSISHeaterEnable(0, false);
                    if (pol == -1 || pol == 1)
                        setSISHeaterEnable(1, false);
                    if (pol == -1 || pol == 0)
                        setSISHeaterEnable(0, true); // set pol0 heater control line ON
                    if (pol == -1 || pol == 1)
                        setSISHeaterEnable(1, true); // set pol1 heater control line ON
                }
            }
        }
    }

    if (timedOut && targetTemp >= mixerHeatingMinTargetTemp_m)
        msg = "MixerHeating: timed out waiting for mixer temperature.";
    else
        msg = "MixerHeating: finished successfully.";
    msg += "  Waiting for temps to return to normal...";
    LOG(LM_INFO) << msg << endl;
    FEMCEventQueue::addStatusMessage(true, msg);

    // switch off the heaters:
    setSISHeaterEnable(0, false);
    setSISHeaterEnable(1, false);

    // wait for the temperatures to return below the threshold:
    unsigned long mixerCooldownTimerStart = GETTIMER();
    const unsigned long mixerCooldownTimeout = 5 * 60 * 1000;  // 5 minutes.
    timedOut = done = false;
    while (!done && !mixerHeatingStop_m) {
        sisMixerHeatingReadData(); // write mixer temps and heater currents to output file.
        if (GETTIMER() - mixerCooldownTimerStart > mixerCooldownTimeout)
            timedOut = done = true;
        else {
            SLEEP(1000);
            mixerTempPol0 = cartridgeTemperature2();
            mixerTempPol1 = cartridgeTemperature5();
            if ((mixerTempPol0 < thresholdOffTemp0) && (mixerTempPol1 < thresholdOffTemp1))
                done = true;
        }
    }

    if (timedOut)
        msg = "MixerHeating: timed out waiting for mixer temps to return to normal.";
    else
        msg = "MixerHeating: finished successfully.";
    LOG(LM_INFO) << msg << endl;
    FEMCEventQueue::addStatusMessage(true, msg);

    // re-enable the SIS magnets if they were enabled before:
    // This doesn't energize the magnets, just makes them enabled so that they may be set.
    setSISMagnetEnable(magnetsPriorState);  

    // flush the log file:
    if (mixerHeatingdataFile_mp) {
        (*mixerHeatingdataFile_mp).flush();
        delete mixerHeatingdataFile_mp;
        mixerHeatingdataFile_mp = NULL;
    }

    // Send the measurement finished event:
    if (mixerHeatingStop_m)
        msg = "MixerHeating: stopped.";

    return true;
}

void ColdCartImpl::sisMixerHeatingReadData(bool printHeader) {
    if (printHeader) {
        if (mixerHeatingdataFile_mp)
            (*mixerHeatingdataFile_mp) << "time sec\theaterCurrentPol0 mA\theaterCurrentPol1 mA\tmixerTempPol0 K\tmixerTempPol1 K\tIJ01\tIJ02\tIJ11\tIJ12" << endl;
    } else {
        float time = ((float) (GETTIMER() - mixerHeatingtimerStart_m)) / 1000.0;
        float heaterCurrentPol0 = getSISHeaterCurrent(0);
        float heaterCurrentPol1 = getSISHeaterCurrent(1);
        float mixerTempPol0 = cartridgeTemperature2();
        float mixerTempPol1 = cartridgeTemperature5();
        float IJ01 = getSISCurrent(0, 1, 8);
        float IJ02 = getSISCurrent(0, 2, 8);
        float IJ11 = getSISCurrent(1, 1, 8);
        float IJ12 = getSISCurrent(1, 2, 8);

        if (mixerHeatingdataFile_mp) {
            (*mixerHeatingdataFile_mp) << time << "\t" << heaterCurrentPol0 << "\t" << heaterCurrentPol1 << "\t"
                                       << mixerTempPol0 << "\t" << mixerTempPol1 << "\t"
                                       << IJ01 << "\t" << IJ02 << "\t" << IJ11 << "\t" << IJ12 << "\t" << endl;
        }
    }
}


//-------------------------------------------------------------------------------------------------
// LNA operations

void ColdCartImpl::lnaPol0Sb1Enable(bool val) {
    if (hasLNA()) {
        lnaEnableSet_m[0] = val;
        ColdCartImplBase::lnaPol0Sb1Enable(val);
    }
}

void ColdCartImpl::lnaPol0Sb2Enable(bool val) {
    if (hasSb2() && hasLNA()) {
        lnaEnableSet_m[1] = val;
        ColdCartImplBase::lnaPol0Sb2Enable(val);
    }
}

void ColdCartImpl::lnaPol1Sb1Enable(bool val) {
    if (hasLNA()) {
        lnaEnableSet_m[2] = val;
        ColdCartImplBase::lnaPol1Sb1Enable(val);
    }
}

void ColdCartImpl::lnaPol1Sb2Enable(bool val) {
    if (hasSb2() && hasLNA()) {
        lnaEnableSet_m[3] = val;
        ColdCartImplBase::lnaPol1Sb2Enable(val);
    }
}

bool ColdCartImpl::setLNAEnable(bool val) {
    if (!hasLNA()) {
        if (val == true) {
            string msg("setLNAEnable: no LNAs in band ");
            msg += to_string(band_m);
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
        }
        return false;
    }
    lnaPol0Sb1Enable(val);
    lnaPol0Sb2Enable(val);
    lnaPol1Sb1Enable(val);
    lnaPol1Sb2Enable(val);
    return true;
}

bool ColdCartImpl::setLNAEnable(int pol, int sb, bool val) {
    if (!hasLNA()) {
        if (val == true) {
            string msg("setLNAEnable: no LNAs in band ");
            msg += to_string(band_m);
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << "ColdCartImpl::" << msg << endl;
        }
        return false;
    }
    if (pol == 0) {
        if (sb == 1)
            lnaPol0Sb1Enable(val);
        else if (sb == 2)
            lnaPol0Sb2Enable(val);
    } else if (pol == 1) {
        if (sb == 1)
            lnaPol1Sb1Enable(val);
        else if (sb == 2)
            lnaPol1Sb2Enable(val);
    }
    return true;
}

bool ColdCartImpl::getMonitorTemp(CartridgeTemp_t &target) const {
    memset(&target, 0, sizeof(target));
    target.cartridgeTemperature0_value = cartridgeTemperature0_value;
    target.cartridgeTemperature1_value = cartridgeTemperature1_value;
    target.cartridgeTemperature2_value = cartridgeTemperature2_value;
    target.cartridgeTemperature3_value = cartridgeTemperature3_value;
    target.cartridgeTemperature4_value = cartridgeTemperature4_value;
    target.cartridgeTemperature5_value = cartridgeTemperature5_value;
    return true;
}

bool ColdCartImpl::getMonitorSIS(int pol, int sb, SIS_t &target) const {
    memset(&target, 0, sizeof(target));
    
    // No error if SIS not installed.  Just return zeros:
    if (!hasSIS())
        return true;

    if (pol == 0) {
        if (sb == 1) {
            target.sisOpenLoop_value = sisPol0Sb1OpenLoop_value;
            target.sisVoltage_value = sisPol0Sb1Voltage_value;
            target.sisCurrent_value = sisPol0Sb1Current_value;
            target.sisMagnetVoltage_value = sisMagnetPol0Sb1Voltage_value;
            target.sisMagnetCurrent_value = sisMagnetPol0Sb1Current_value;
            return true;
        } else if (sb == 2) {
            if (hasSb2()) {
                target.sisOpenLoop_value = sisPol0Sb2OpenLoop_value;
                target.sisVoltage_value = sisPol0Sb2Voltage_value;
                target.sisCurrent_value = sisPol0Sb2Current_value;
                target.sisMagnetVoltage_value = sisMagnetPol0Sb2Voltage_value;
                target.sisMagnetCurrent_value = sisMagnetPol0Sb2Current_value;
            }
            // No error if no sb2.  Just return zeros:
            return true;
        }
    } else if (pol == 1) {
        if (sb == 1) {
            target.sisOpenLoop_value = sisPol1Sb1OpenLoop_value;
            target.sisVoltage_value = sisPol1Sb1Voltage_value;
            target.sisCurrent_value = sisPol1Sb1Current_value;
            target.sisMagnetVoltage_value = sisMagnetPol1Sb1Voltage_value;
            target.sisMagnetCurrent_value = sisMagnetPol1Sb1Current_value;
            return true;
        } else if (sb == 2) {
            if (hasSb2()) {
                target.sisOpenLoop_value = sisPol1Sb2OpenLoop_value;
                target.sisVoltage_value = sisPol1Sb2Voltage_value;
                target.sisCurrent_value = sisPol1Sb2Current_value;
                target.sisMagnetVoltage_value = sisMagnetPol1Sb2Voltage_value;
                target.sisMagnetCurrent_value = sisMagnetPol1Sb2Current_value;
            }
            // No error if no sb2.  Just return zeros:
            return true;
        }
    }
    return false;
}

bool ColdCartImpl::getMonitorLNA(int pol, int sb, LNA_t &target) const {
    memset(&target, 0, sizeof(target));

    if (!hasLNA())
        return false;

    if (pol == 0) {
        if (sb == 1) {
            target.lnaEnable_value = lnaPol0Sb1Enable_value;
            target.lnaSt1DrainVoltage_value = lnaPol0Sb1St1DrainVoltage_value; 
            target.lnaSt1DrainCurrent_value = lnaPol0Sb1St1DrainCurrent_value;
            target.lnaSt1GateVoltage_value = lnaPol0Sb1St1GateVoltage_value;
            target.lnaSt2DrainVoltage_value = lnaPol0Sb1St2DrainVoltage_value;
            target.lnaSt2DrainCurrent_value = lnaPol0Sb1St2DrainCurrent_value;
            target.lnaSt2GateVoltage_value = lnaPol0Sb1St2GateVoltage_value;
            target.lnaSt3DrainVoltage_value = lnaPol0Sb1St3DrainVoltage_value; 
            target.lnaSt3DrainCurrent_value = lnaPol0Sb1St3DrainCurrent_value;
            target.lnaSt3GateVoltage_value = lnaPol0Sb1St3GateVoltage_value;
            return true;
        } else if (sb == 2 && hasSb2()) {
            target.lnaEnable_value = lnaPol0Sb2Enable_value;
            target.lnaSt1DrainVoltage_value = lnaPol0Sb2St1DrainVoltage_value; 
            target.lnaSt1DrainCurrent_value = lnaPol0Sb2St1DrainCurrent_value;
            target.lnaSt1GateVoltage_value = lnaPol0Sb2St1GateVoltage_value;
            target.lnaSt2DrainVoltage_value = lnaPol0Sb2St2DrainVoltage_value;
            target.lnaSt2DrainCurrent_value = lnaPol0Sb2St2DrainCurrent_value;
            target.lnaSt2GateVoltage_value = lnaPol0Sb2St2GateVoltage_value;
            target.lnaSt3DrainVoltage_value = lnaPol0Sb2St3DrainVoltage_value; 
            target.lnaSt3DrainCurrent_value = lnaPol0Sb2St3DrainCurrent_value;
            target.lnaSt3GateVoltage_value = lnaPol0Sb2St3GateVoltage_value;
            return true;
        }
    } else if (pol == 1) {
        if (sb == 1) {
            target.lnaEnable_value = lnaPol1Sb1Enable_value;
            target.lnaSt1DrainVoltage_value = lnaPol1Sb1St1DrainVoltage_value; 
            target.lnaSt1DrainCurrent_value = lnaPol1Sb1St1DrainCurrent_value;
            target.lnaSt1GateVoltage_value = lnaPol1Sb1St1GateVoltage_value;
            target.lnaSt2DrainVoltage_value = lnaPol1Sb1St2DrainVoltage_value;
            target.lnaSt2DrainCurrent_value = lnaPol1Sb1St2DrainCurrent_value;
            target.lnaSt2GateVoltage_value = lnaPol1Sb1St2GateVoltage_value;
            target.lnaSt3DrainVoltage_value = lnaPol1Sb1St3DrainVoltage_value; 
            target.lnaSt3DrainCurrent_value = lnaPol1Sb1St3DrainCurrent_value;
            target.lnaSt3GateVoltage_value = lnaPol1Sb1St3GateVoltage_value;
            return true;
        } else if (sb == 2 && hasSb2()) {
            target.lnaEnable_value = lnaPol1Sb2Enable_value;
            target.lnaSt1DrainVoltage_value = lnaPol1Sb2St1DrainVoltage_value; 
            target.lnaSt1DrainCurrent_value = lnaPol1Sb2St1DrainCurrent_value;
            target.lnaSt1GateVoltage_value = lnaPol1Sb2St1GateVoltage_value;
            target.lnaSt2DrainVoltage_value = lnaPol1Sb2St2DrainVoltage_value;
            target.lnaSt2DrainCurrent_value = lnaPol1Sb2St2DrainCurrent_value;
            target.lnaSt2GateVoltage_value = lnaPol1Sb2St2GateVoltage_value;
            target.lnaSt3DrainVoltage_value = lnaPol1Sb2St3DrainVoltage_value; 
            target.lnaSt3DrainCurrent_value = lnaPol1Sb2St3DrainCurrent_value;
            target.lnaSt3GateVoltage_value = lnaPol1Sb2St3GateVoltage_value;
            return true;
        }
    }
    return false;
}

bool ColdCartImpl::getMonitorAux(int pol, Aux_t &target) const {
    memset(&target, 0, sizeof(target));
    if (pol == 0) {
        if (hasLNA())
            target.lnaLedEnable_value = lnaLedPol0Enable_value;
        if (hasSIS())
            target.sisHeaterCurrent_value = sisHeaterPol0Current_value;
        return true;
    } else if (pol == 1) {
        if (hasLNA())
            target.lnaLedEnable_value = lnaLedPol1Enable_value;
        if (hasSIS())
            target.sisHeaterCurrent_value = sisHeaterPol1Current_value;
        return true;
    }
    return false;
}

void ColdCartImpl::appendThermalLogHeaderImpl(std::string &target) {
    target += "\t4K stage\t110K stage\tmixer pol0\tspare\t15K stage\tmixer pol1";
}

void ColdCartImpl::appendThermalLog(std::string &target) const {
    char buf[20];
    sprintf(buf, "\t%.2f", cartridgeTemperature0_value);
    target += buf;
    sprintf(buf, "\t%.2f", cartridgeTemperature1_value);
    target += buf;
    sprintf(buf, "\t%.2f", cartridgeTemperature2_value);
    target += buf;
    sprintf(buf, "\t%.2f", cartridgeTemperature3_value);
    target += buf;
    sprintf(buf, "\t%.2f", cartridgeTemperature4_value);
    target += buf;
    sprintf(buf, "\t%.2f", cartridgeTemperature5_value);
    target += buf;
}

void ColdCartImpl::appendThermalLogPlaceholder(std::string &target) {
    target += "\t0\t0\t0\t0\t0\t0";
}

void ColdCartImpl::monitorAction(Time *timestamp_p) {
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 15 * milliseconds;

    bool doMonitor = false;

    if (lastMonitorTime == 0 && logMonitors_m)
        logMon(true);

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        switch (monitorPhase) {
            case 0:
                // Execute the next registered analog monitor transaction:
                if (!executeNextMon())
                    monitorPhase = 1;
                break;
            case 1:
                // Special cases 1-4 do averaging:
                sisPol0Sb1Current_value = avgSisPol0Sb1Current(20);
                ++monitorPhase;
                break;

            case 2:
                sisPol0Sb2Current_value = avgSisPol0Sb2Current(20);
                ++monitorPhase;
                break;

            case 3:
                sisPol1Sb1Current_value = avgSisPol1Sb1Current(20);
                ++monitorPhase;
                break;

            case 4:
                sisPol1Sb2Current_value = avgSisPol1Sb2Current(20);
                ++monitorPhase;
                break;

            case 5:
                // Special cases 5-14 are not analog monitor points:
                sisPol0Sb1OpenLoop_value = sisPol0Sb1OpenLoop();
                ++monitorPhase;
                break;
            case 6:
                sisPol0Sb2OpenLoop_value = sisPol0Sb2OpenLoop();
                ++monitorPhase;
                break;
            case 7:
                sisPol1Sb1OpenLoop_value = sisPol1Sb1OpenLoop();
                ++monitorPhase;
                break;
            case 8:
                sisPol1Sb2OpenLoop_value = sisPol1Sb2OpenLoop();
                ++monitorPhase;
                break;
            case 9:
                lnaPol0Sb1Enable_value = ColdCartImplBase::lnaPol0Sb1Enable();
                ++monitorPhase;
                break;
            case 10:
                lnaPol0Sb2Enable_value = ColdCartImplBase::lnaPol0Sb2Enable();
                ++monitorPhase;
                break;
            case 11:
                lnaPol1Sb1Enable_value = ColdCartImplBase::lnaPol1Sb1Enable();
                ++monitorPhase;
                break;
            case 12:
                lnaPol1Sb2Enable_value = ColdCartImplBase::lnaPol1Sb2Enable();
                ++monitorPhase;
                break;
            case 13:
                lnaLedPol0Enable_value = ColdCartImplBase::lnaLedPol0Enable();
                ++monitorPhase;
                break;
            case 14:
                lnaLedPol1Enable_value = lnaLedPol1Enable();
                ++monitorPhase;
                // no break;
            default:
                if (logMonitors_m)
                    logMon();
                monitorPhase = 0;
                break;
        }
    }
}

DEFINE_MONITORS_REGISTRY(ColdCartImpl)

void ColdCartImpl::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:ColdCart(" << port_m << "): randomized,"
                         "IsisPol0Sb1,IsisPol0Sb2,IsisPol1Sb1,IsisPol1Sb2,"
                         "VsisPol0Sb1,VsisPol0Sb2,VsisPol1Sb1,VsisPol1Sb2,"
                         "ImagnetPol0Sb1,ImagnetPol0Sb2,ImagnetPol1Sb1,ImagnetPol1Sb2,"
                         "VmagnetPol0Sb1,VmagnetPol0Sb2,VmagnetPol1Sb1,VmagnetPol1Sb2,"
                         "VdrainLnaPol0Sb1St1,IdrainLnaPol0Sb1St1,VgateLnaPol0Sb1St1,"
                         "VdrainLnaPol0Sb1St2,IdrainLnaPol0Sb1St2,VgateLnaPol0Sb1St2,"
                         "VdrainLnaPol0Sb1St3,IdrainLnaPol0Sb1St3,VgateLnaPol0Sb1St3,"
                         "VdrainLnaPol0Sb2St1,IdrainLnaPol0Sb2St1,VgateLnaPol0Sb2St1,"
                         "VdrainLnaPol0Sb2St2,IdrainLnaPol0Sb2St2,VgateLnaPol0Sb2St2,"
                         "VdrainLnaPol0Sb2St3,IdrainLnaPol0Sb2St3,VgateLnaPol0Sb2St3,"
                         "VdrainLnaPol1Sb1St1,IdrainLnaPol1Sb1St1,VgateLnaPol1Sb1St1,"
                         "VdrainLnaPol1Sb1St2,IdrainLnaPol1Sb1St2,VgateLnaPol1Sb1St2,"
                         "VdrainLnaPol1Sb1St3,IdrainLnaPol1Sb1St3,VgateLnaPol1Sb1St3,"
                         "VdrainLnaPol1Sb2St1,IdrainLnaPol1Sb2St1,VgateLnaPol1Sb2St1,"
                         "VdrainLnaPol1Sb2St2,IdrainLnaPol1Sb2St2,VgateLnaPol1Sb2St2,"
                         "VdrainLnaPol1Sb2St3,IdrainLnaPol1Sb2St3,VgateLnaPol1Sb2St3,"
                         "IheaterPol0,IheaterPol1,cartTemp0,cartTemp1,"
                         "cartTemp2,cartTemp3,cartTemp4,cartTemp5,"
                         "sisPol0Sb1OpenLoop,sisPol0Sb2OpenLoop,sisPol1Sb1OpenLoop,sisPol1Sb2OpenLoop,"
                         "lnaPol0Sb1Enable,lnaPol0Sb2Enable,lnaPol1Sb1Enable,lnaPol1Sb2Enable,"
                         "lnaLedPol0Enable,lnaLedPol1Enable" << endl;

    } else {
        LOG(LM_INFO)  << "AllMonitors:ColdCart(" << port_m << "): " << randomizeAnalogMonitors_m << ","
                      << sisPol0Sb1Current_value << "," << sisPol0Sb2Current_value << "," << sisPol1Sb1Current_value << "," << sisPol1Sb2Current_value << ","
                      << sisPol0Sb1Voltage_value << "," << sisPol0Sb2Voltage_value << "," << sisPol1Sb1Voltage_value << "," << sisPol1Sb2Voltage_value << ","
                      << sisMagnetPol0Sb1Current_value << "," << sisMagnetPol0Sb2Current_value << "," << sisMagnetPol1Sb1Current_value << "," << sisMagnetPol1Sb2Current_value << ","
                      << sisMagnetPol0Sb1Voltage_value << "," << sisMagnetPol0Sb2Voltage_value << "," << sisMagnetPol1Sb1Voltage_value << "," << sisMagnetPol1Sb2Voltage_value << ","
                      << lnaPol0Sb1St1DrainVoltage_value << "," << lnaPol0Sb1St1DrainCurrent_value << "," << lnaPol0Sb1St1GateVoltage_value << ","
                      << lnaPol0Sb1St2DrainVoltage_value << "," << lnaPol0Sb1St2DrainCurrent_value << "," << lnaPol0Sb1St2GateVoltage_value << ","
                      << lnaPol0Sb1St3DrainVoltage_value << "," << lnaPol0Sb1St3DrainCurrent_value << "," << lnaPol0Sb1St3GateVoltage_value << ","
                      << lnaPol0Sb2St1DrainVoltage_value << "," << lnaPol0Sb2St1DrainCurrent_value << "," << lnaPol0Sb2St1GateVoltage_value << ","
                      << lnaPol0Sb2St2DrainVoltage_value << "," << lnaPol0Sb2St2DrainCurrent_value << "," << lnaPol0Sb2St2GateVoltage_value << ","
                      << lnaPol0Sb2St3DrainVoltage_value << "," << lnaPol0Sb2St3DrainCurrent_value << "," << lnaPol0Sb2St3GateVoltage_value << ","
                      << lnaPol1Sb1St1DrainVoltage_value << "," << lnaPol1Sb1St1DrainCurrent_value << "," << lnaPol1Sb1St1GateVoltage_value << ","
                      << lnaPol1Sb1St2DrainVoltage_value << "," << lnaPol1Sb1St2DrainCurrent_value << "," << lnaPol1Sb1St2GateVoltage_value << ","
                      << lnaPol1Sb1St3DrainVoltage_value << "," << lnaPol1Sb1St3DrainCurrent_value << "," << lnaPol1Sb1St3GateVoltage_value << ","
                      << lnaPol1Sb2St1DrainVoltage_value << "," << lnaPol1Sb2St1DrainCurrent_value << "," << lnaPol1Sb2St1GateVoltage_value << ","
                      << lnaPol1Sb2St2DrainVoltage_value << "," << lnaPol1Sb2St2DrainCurrent_value << "," << lnaPol1Sb2St2GateVoltage_value << ","
                      << lnaPol1Sb2St3DrainVoltage_value << "," << lnaPol1Sb2St3DrainCurrent_value << "," << lnaPol1Sb2St3GateVoltage_value << ","
                      << sisHeaterPol0Current_value << "," << sisHeaterPol1Current_value << "," << cartridgeTemperature0_value << "," << cartridgeTemperature1_value << ","
                      << cartridgeTemperature2_value << "," << cartridgeTemperature3_value << "," << cartridgeTemperature4_value << "," << cartridgeTemperature5_value << ","
                      << (sisPol0Sb1OpenLoop_value ? 1 : 0) << "," << (sisPol0Sb2OpenLoop_value ? 1 : 0) << "," << (sisPol1Sb1OpenLoop_value ? 1 : 0) << "," << (sisPol1Sb2OpenLoop_value ? 1 : 0) << ","
                      << (lnaPol0Sb1Enable_value ? 1 : 0) << "," << (lnaPol0Sb2Enable_value ? 1 : 0) << "," << (lnaPol1Sb1Enable_value ? 1 : 0) << "," << (lnaPol1Sb2Enable_value ? 1 : 0) << ","
                      << (lnaLedPol0Enable_value ? 1 : 0) << "," << (lnaLedPol1Enable_value ? 1 : 0) << endl;
    }
}
