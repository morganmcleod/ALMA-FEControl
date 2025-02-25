/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2009
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

#include "MeasureIFPower.h"
#include "logger.h"
#include "stringConvert.h"
#include "ColdCartImpl.h"
#include "WCAImpl.h"
#include <fstream>
#include <iomanip>
#include <math.h>
using namespace std;

void MeasureIFPower::reset() {
    // reset all state to initial/just constructed.
    mode_m = MODE_NONE;
    pol_m = sb_m = -1;
    b6SweepMode_m = false;
    VJNom1_m = VJNom2_m = VDNom_m = 0;
    VStart_m = VStop_m = VStep_m = V_m = powerSb1_m = powerSb2_m = 0;
    doYFactor_m = false;
    coldHot_m = false;
    repeatCount_m = iter_m = noReadingCount_m = 0;
    waitForPowerReading_m = false;
    progress_m = progressIncrement_m = 0;
    dataIndex_m = 0;
    yFactorData_m.clear();
    data_m.clear();
}

bool MeasureIFPower::startVsVJ(int pol, int sb, const std::string &description,
                               float VJStart, float VJStop, float VJStep,
                               bool doYFactor, int repeatCount) 
{ 
    
    // Check parameters:
    if (pol < 0 || pol > 1)
        return false;
    if (!(sb == -1 || sb == 1 || sb == 2))
        return false;
    if (VJStart == VJStop)
        return false;
    if (VJStart < -25 || VJStart > 25 || VJStop < -25 || VJStop > 25)
        return false;
    if (repeatCount <= 0)
        repeatCount = 1;

    // Setup the operating variables:
    mode_m = MODE_VJ;
    pol_m = pol;
    sb_m = sb;
    // If measuring both sidebands on band 6, use a special sweeping mode where
    // the positive voltage is applied to SIS2 and the opposite negative voltage is applied to SIS1:
    b6SweepMode_m = (coldCart_m.getBand() == 6 && sb_m == -1);
    VStart_m = VJStart;
    VStop_m = VJStop;
    VStep_m = VJStep;
    doYFactor_m = doYFactor;
    repeatCount_m = repeatCount;
    noReadingCount_m = 0;
    
    float span = VStop_m - VStart_m;
    float sign = (span < 0.0) ? -1.0 : 1.0;
    span = fabsf(span);
    VStep_m = fabsf(VStep_m);

    if (VStep_m == 0)
        VStep_m = 0.1;
    else if (VStep_m > span)
        VStep_m = span;

    int steps = (int) (span / VStep_m) + 1;
    
    VStep_m *= sign;

    LOG(LM_INFO) << "MeasureIFPower::startVsVJ pol=" << pol_m << " sb=" << sb_m << " b6SweepMode=" << b6SweepMode_m
        << " '" << description << "' "
        << fixed << setprecision(3) << " VJStart=" << VStart_m << " VJStop=" << VStop_m << " VJStep=" << VStep_m
        << " doYFactor=" << doYFactor_m << " repeatCount=" << repeatCount_m << endl;
    
    // Clear the result data set and reserve space:
    data_m.clear();
    yFactorData_m.clear();
    data_m.reserve(repeatCount_m * steps);    
    yFactorData_m.reserve(repeatCount_m * steps);
    dataIndex_m = 0;
    
    // Create the log file:
    if (!logDir_m.empty()) {
        string fileName, bandTxt, polText;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);    
        coldCart_m.getBandText(bandTxt);
        polText = to_string(pol);
        fileName = logDir_m + "IFPowerVsVJ-" + bandTxt + "-pol" + polText + "-" + description + "-" + fileName + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
    }
    
    // Set up the "loop" variables: 
    iter_m = 0;
    coldHot_m = true;   // set to hot first
    V_m = VStart_m;
    
    // Set up the progress bar variables:
    setProgress(0);
    progress_m = 0.0;
    progressIncrement_m = 100.0 / (repeatCount_m * steps * (doYFactor_m ? 2 : 1));

    // Pause monitoring during the measurement:
    coldCart_m.pauseMonitor(true, "Measure IFPower vs SIS voltage.");

    // start reading the power meters:
    setEvent(FEMCEventQueue::EVENT_REQUEST_IFPOWER, coldCart_m.getBand(), pol_m, 4);

    // If measuring Y factor, set the initial chopper position:
    if (doYFactor_m) {
        setEvent(FEMCEventQueue::EVENT_CHOPPER_MOVE, 0, 0, (coldHot_m ? 1 : 0));
        SLEEP(500);
    }
    
    // Save the prior values so we can go back to them when done:
    VJNom1_m = coldCart_m.getSISVoltageSetting(pol_m, 1);
    VJNom2_m = coldCart_m.getSISVoltageSetting(pol_m, 2);
    VDNom_m = WCA_m.getPADrainVoltageSetting(pol_m);
    
    // Set the initial value to both sidebands and start waiting for the first power meter reading:
    if (sb_m == -1 || sb_m == 1)
        coldCart_m.setSISVoltage(pol_m, 1, (b6SweepMode_m) ? (-V_m) : (V_m));
    if (sb_m == -1 || sb_m == 2)
        coldCart_m.setSISVoltage(pol_m, 2, V_m);
    waitForPowerReading_m = true;
    
    // Print the headers to the data log file:
    readData(true);
    
    string msg("MeasureIFPower: process started.");
    setStatusMessage(true, msg);
    
    // Start the worker thread:
    return OptimizeBase::startWorkerThread();
}

bool MeasureIFPower::startVsVD(int pol, const std::string &description,
                               float VDStart, float VDStop, float VDStep,
                               bool doYFactor, int repeatCount)
{
    // Check parameters:
    if (pol < 0 || pol > 1)
        return false;
    if (VDStart == VDStop)
        return false;
    if (VDStart < 0 || VDStart > 5 || VDStop < 0 || VDStop > 5)
        return false;
    if (repeatCount <= 0)
        repeatCount = 1;
    
    // Setup the operating variables:
    if (VDStop < VDStart) {
        float t(VDStop);
        VDStop = VDStart;
        VDStart = t;
    }
    mode_m = MODE_VD;
    pol_m = pol;
    sb_m = -1;
    VStart_m = VDStart;
    VStop_m = VDStop;
    VStep_m = VDStep;
    doYFactor_m = doYFactor;
    repeatCount_m = repeatCount;
    noReadingCount_m = 0;

    // Check parameters and calculate the data set size:
    float span = VStop_m - VStart_m;
    span = fabsf(span);
    VStep_m = fabsf(VStep_m);
        
    if (VStep_m == 0)
        VStep_m = 0.1;
    else if (VStep_m > span)
        VStep_m = span;

    int steps = (int) (span / VStep_m) + 1;

    LOG(LM_INFO) << "MeasureIFPower::startVsVD pol=" << pol_m
        << " '" << description << "' " << fixed << setprecision(3)
        << " VDStart=" << VStart_m << " VDStop=" << VStop_m << " VDStep=" << VStep_m
        << " doYFactor=" << doYFactor_m << " repeatCount=" << repeatCount_m << endl;

    //TODO:  was exception after here when VStart=VStop=0  why?
    
    // Clear the result data set and reserve space:
    data_m.clear();
    yFactorData_m.clear();
    data_m.reserve(repeatCount_m * steps);    
    yFactorData_m.reserve(repeatCount_m * steps);
    dataIndex_m = 0;
    
    // Create the log file:
    if (!logDir_m.empty()) {
        string fileName, bandTxt, polText;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);    
        coldCart_m.getBandText(bandTxt);
        polText = to_string(pol);
        fileName = logDir_m + "IFPowerVsVD-" + bandTxt + "-pol" + polText + "-" + description + "-" + fileName + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
    }
    
    // Set up the "loop" variables: 
    iter_m = 0;
    coldHot_m = true;   // set to hot first
    V_m = VStart_m;
    
    // Set up the progress bar variables:
    setProgress(0);
    progress_m = 0.0;
    progressIncrement_m = 100.0 / (repeatCount_m * steps * (doYFactor_m ? 2 : 1));

    // Pause monitoring during the measurement:
    coldCart_m.pauseMonitor(true, "Measure IFPower vs LO PA drain voltage.");

    // If measuring Y factor, set the initial chopper position:
    if (doYFactor_m) {
        setEvent(FEMCEventQueue::EVENT_CHOPPER_MOVE, 0, 0, (coldHot_m ? 1 : 0));
        SLEEP(500);
    }

    // start reading the power meters:
    setEvent(FEMCEventQueue::EVENT_REQUEST_IFPOWER, coldCart_m.getBand(), pol_m, 4);
    
    // Save the nominal values so we can go back to them when done:
    VJNom1_m = coldCart_m.getSISVoltageSetting(pol_m, 1);
    VJNom2_m = coldCart_m.getSISVoltageSetting(pol_m, 2);
    VDNom_m = WCA_m.getPADrainVoltageSetting(pol_m);
    
    // Set the initial value and start watiting for the first power meter reading:
    WCA_m.setPADrainVoltage(pol_m, V_m);
    waitForPowerReading_m = true;

    // Print the headers to the data log file:
    readData(true);
    
    string msg("MeasureIFPower: process started.");
    setStatusMessage(true, msg);
    
    // Start the worker thread:
    return OptimizeBase::startWorkerThread();
}

    
bool MeasureIFPower::setIFPower(int pol, float powerSb1, float powerSb2) {
    // called by main thread to feed IF power readings into the optimization process.
    if (mode_m == MODE_NONE)
        return false;   // not measuring anything.
    if (pol != pol_m)
        return false;   // data for wrong polarization.
    // reset the "watchdog" counter for power readings:
    noReadingCount_m = 0;
    // If we are ready for a reading, save it:
    if (waitForPowerReading_m) {
        powerSb1_m = powerSb1;
        powerSb2_m = powerSb2;
        waitForPowerReading_m = false;
    }
    return true;
}


void MeasureIFPower::optimizeAction() {
    // This function is called repeatedly on the worker thread...
    const int stepSleep = 0; //ms pause between setting the control parameter and reading the result.

    if (mode_m == MODE_NONE) {
        // Not measuring anything.  Stop the thread:
        setFinished(false);
        return;        
    }
    
    if (stopRequested()) {
        // We've been told to stop:
        waitForPowerReading_m = false;
        setFinished(false);
        return;
    }

    // If we are already waiting for a power meter reading do nothing:    
    if (waitForPowerReading_m) {
        // but check that we aren't cycling forever:        
        if (++noReadingCount_m > 10000)
            setFinished(false);
        // do nothing this time through optimizeAction:
        SLEEP(1);
        return;
    }
    
    // Outer "loop" on repeatCount:
    if (iter_m < repeatCount_m) {
        
        // Inner "loop" on VJ or VD steps:
        // Store and log the latest control voltage and power reading:
        readData();
        
        // Report progress:
        progress_m += progressIncrement_m;
        setProgress((short) progress_m);

        // Go to the next control voltage:
        V_m += VStep_m;
        
        // Check whether still in parameter range:
        if ((VStep_m > 0 && V_m > VStop_m) || (VStep_m < 0 && V_m < VStop_m)) {    
            // Go to the next sweep:
            V_m = VStart_m;

            // Increment iterations if we've already done hot and cold at this step (if required):
            if (!doYFactor_m || (doYFactor_m && !coldHot_m))
                ++iter_m;

            // Optional intermediate "loop" on hot/cold chopper position:
            if (doYFactor_m) {
                coldHot_m = false;      
                // Move the chopper:
                setEvent(FEMCEventQueue::EVENT_CHOPPER_MOVE, 0, 0, (coldHot_m ? 1 : 0));
                SLEEP(500);
            }
        }
        // If not done, set the new voltage and wait for a power reading:
        if (iter_m < repeatCount_m) {
            switch (mode_m) {
                case MODE_VJ:
                    if (sb_m == -1 || sb_m == 1)
                        coldCart_m.setSISVoltage(pol_m, 1, (b6SweepMode_m) ? (-V_m) : (V_m));
                    if (sb_m == -1 || sb_m == 2)
                        coldCart_m.setSISVoltage(pol_m, 2, V_m);
                    break;
                case MODE_VD:
                    WCA_m.setPADrainVoltage(pol_m, V_m);
                    break;
                default:
                    break;
            }
            SLEEP(stepSleep);
            waitForPowerReading_m = true;
        }
        // Send the measurement finished event:
        setEvent(FEMCEventQueue::EVENT_OPTIMIZE_DONE, coldCart_m.getBand(), -1, 0, 100);
        
    // All iterations done:
    } else {
        waitForPowerReading_m = false;
        setFinished(true);
    }
}

void MeasureIFPower::exitAction(bool success) {
    // restart monitoring:
    coldCart_m.pauseMonitor(false);
    // stop reading the power meters:
    setEvent(FEMCEventQueue::EVENT_STOP_IFPOWER, coldCart_m.getBand(), pol_m);
    SLEEP(100);
    // set the nominal VJs:
    coldCart_m.setSISVoltage(pol_m, 1, VJNom1_m);
    coldCart_m.setSISVoltage(pol_m, 2, VJNom2_m);
    // set the nominal VD:
    WCA_m.setPADrainVoltage(pol_m, VDNom_m);
    // If measuring Y factor, set the chopper back to hot:
    if (doYFactor_m)
        setEvent(FEMCEventQueue::EVENT_CHOPPER_MOVE, 0, 0, 1);

    // flush the log file:
    if (dataFile_mp) {
        (*dataFile_mp).flush();
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
    if (success) {
        string msg("MeasureIFPower: finished successfully.");
        LOG(LM_INFO) << msg << endl;
        setStatusMessage(true, msg);
    } else if (stopRequested()) {
        string msg("MeasureIFPower: process stopped.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
    } else if (waitForPowerReading_m) {
        string msg("MeasureIFPower ERROR: timed out waiting for power readings.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
    }
    // Send the measurement finished event:
    setEvent(FEMCEventQueue::EVENT_ALL_REPS_DONE, coldCart_m.getBand(), -1, 0, 100);
}

void MeasureIFPower::readData(bool printHeader) {
    if (printHeader) {
        if (dataFile_mp) {
            if (mode_m == MODE_VJ)
                (*dataFile_mp) << "VJ1 mV (set)\tVJ2 mV (set)\tIJ1 mA\tIJ2 mA";
            else if (mode_m == MODE_VD)
                (*dataFile_mp) << "VD (set)";
            if (!doYFactor_m)
                (*dataFile_mp) << "\tpowerSB1 W\tpowerSB2 W" << endl;
            else
                (*dataFile_mp) << "\tPhotSB1 W\tPcoldSB1 W\tYfactorSB1\tPhotSB2 W\tPcoldSB2 W\tYfactorSB2" << endl;
        }
    } else {
        if (!doYFactor_m) {
            // power measurement mode...
            data_m.push_back(XYPlotPoint(V_m, powerSb1_m, powerSb2_m));
            if (dataFile_mp) {
                // output either both VJs or the single VD:
                if (mode_m == MODE_VJ) {
                    // record VJ1 (set):
                    if (sb_m == -1 || sb_m == 1)
                        (*dataFile_mp) << ((b6SweepMode_m) ? (-V_m) : (V_m));
                    else
                        (*dataFile_mp) << VJNom1_m;
                    (*dataFile_mp) << "\t";
                    // record VJ2 (set):
                    if (sb_m == -1 || sb_m == 2)
                        (*dataFile_mp) << V_m;
                    else
                        (*dataFile_mp) << VJNom2_m;
                    // record IJ1, IJ2:
                    (*dataFile_mp) << "\t" << coldCart_m.getSISCurrent(pol_m, 1, 8) << "\t" << coldCart_m.getSISCurrent(pol_m, 2, 8);
                } else if (mode_m == MODE_VD) {
                    (*dataFile_mp) << V_m;
                }
                // output the sb1 and sb2 power levels:
                (*dataFile_mp) << "\t" << powerSb1_m << "\t" << powerSb2_m << endl;
            }
        } else {
            // Y-factor mode...
            if (coldHot_m)
                // save the pHot values:
                yFactorData_m.push_back(XYPlotPoint(V_m, powerSb1_m, powerSb2_m));
            else {
                // compute the Y factors:
                float YFactorSB1 = yFactorData_m[dataIndex_m].Y1 / powerSb1_m;
                float YFactorSB2 = yFactorData_m[dataIndex_m].Y2 / powerSb2_m;
                // save to the output array:
                data_m.push_back(XYPlotPoint(V_m, YFactorSB1, YFactorSB2));
                if (dataFile_mp) {
                    // output the first control value:
                    if (sb_m == -1 || sb_m == 1)
                        (*dataFile_mp) << V_m;
                    else
                        (*dataFile_mp) << VJNom1_m;
                    (*dataFile_mp) << "\t";

                    // in VJ mode output VJ2, the 2nd control value:
                    if (mode_m == MODE_VJ) {
                        if (sb_m == -1 || sb_m == 2)
                            (*dataFile_mp) << V_m;
                        else
                            (*dataFile_mp) << VJNom2_m;

                        // output IJ1, IJ2:
                        (*dataFile_mp) << "\t" << coldCart_m.getSISCurrent(pol_m, 1, 8) << "\t" << coldCart_m.getSISCurrent(pol_m, 2, 8);
                        (*dataFile_mp) << "\t";
                    }

                    // output the measured values:
                    (*dataFile_mp) << yFactorData_m[dataIndex_m].Y1 << "\t" << powerSb1_m << "\t" << YFactorSB1 << "\t"
                                   << yFactorData_m[dataIndex_m].Y2 << "\t" << powerSb2_m << "\t" << YFactorSB2 << endl;
                }
                ++dataIndex_m;
            }
        }
    }
}


