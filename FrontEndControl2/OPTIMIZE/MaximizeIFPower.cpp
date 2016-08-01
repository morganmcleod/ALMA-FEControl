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

#include "MaximizeIFPower.h"
#include "logger.h"
#include "stringConvert.h"
#include "setTimeStamp.h"
#include "ColdCartImpl.h"
#include "WCAImpl.h"
#include <fstream>
#include <iomanip>
using namespace std;

const short MaximizeIFPower::maxIter_m(75);

void MaximizeIFPower::reset() {
    Maximizer::reset();
    freqLO_m = 0;
    doPol0_m = doPol1_m = false; 
    currentPol_m = -1;
    progress_m = 0;
    startVJ1_m = startVJ2_m = startVD_m = VJ1_m = VJ2_m = VD_m = powerSb1_m = powerSb2_m = 0.0;
    VJ01opt_m = VJ02opt_m = VJ11opt_m = VJ12opt_m = VD0opt_m = VD1opt_m = 0.0;
    opPhase_m = failPhase_m = OP_PHASE_NONE;
    mixerParams_m.clear();
    paParams_m.clear();
    if (dataFile_mp) {
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
}

bool MaximizeIFPower::start(double freqLO, 
                            bool doPol0, bool doPol1, 
                            float startVJ1, float startVJ2, 
                            float startVD) {
    reset();
    freqLO_m = freqLO;
    doPol0_m = doPol0;
    doPol1_m = doPol1;
    startVJ1_m = startVJ1;
    startVJ2_m = startVJ2;
    startVD_m = startVD;
    if (!logDir_m.empty()) {
        string fileName, bandTxt;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);
        coldCart_m.getBandText(bandTxt);
        fileName = logDir_m + "MaxIFPower-" + bandTxt + "-" + fileName + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
    }
    string msg("MaximizeIFPower: process started.");
    setStatusMessage(true, msg);
    return OptimizeBase::startWorkerThread();
}    

bool MaximizeIFPower::setIFPower(int pol, float powerSb1, float powerSb2) {
    if (pol != currentPol_m)
        return false;
    powerSb1_m = powerSb1;
    powerSb2_m = powerSb2;
    float sum = powerSb1 + powerSb2;

    LOG(LM_DEBUG) << "MaximizeIFPower::setIFPower: pol=" << pol << " sum=" << setprecision(4) << scientific << sum << fixed << endl;

    switch (opPhase_m) {
        case OP_PHASE_1C:
        case OP_PHASE_2C:
        case OP_PHASE_3C:
        case OP_PHASE_1B:
        case OP_PHASE_2B:
        case OP_PHASE_3B:
        case OP_PHASE_1A:
        case OP_PHASE_2A:
        case OP_PHASE_3A:
        case OP_PHASE_NONE:
            Maximizer::setDependentValue(sum);
            logOutput();
            break;

        default:
            return false;
    }
    return true;
}

static vector<float> controlHistory;    // array to collect recent control settings to prevent oscillating around the target.
//static std::stringstream historyStream;


bool MaximizeIFPower::setControlValue(float control) {
    switch (opPhase_m) {

        case OP_PHASE_1C:
        case OP_PHASE_2C:
        case OP_PHASE_3C:
            VJ1_m = control;
            LOG(LM_DEBUG) << "MaximizeIFPower::setControlValue(VJ1) pol=" << currentPol_m << " VJ1=" << VJ1_m << endl;
            coldCart_m.setSISVoltage(currentPol_m, 1, VJ1_m);
            break;

        case OP_PHASE_1B:
        case OP_PHASE_2B:
        case OP_PHASE_3B:
            VJ2_m = control;
            LOG(LM_DEBUG) << "MaximizeIFPower::setControlValue(VJ2) pol=" << currentPol_m << " VJ2=" << VJ2_m << endl;
            coldCart_m.setSISVoltage(currentPol_m, 2, VJ2_m);
            break;

        case OP_PHASE_1A:
        case OP_PHASE_2A:
        case OP_PHASE_3A:
            VD_m = control;
            LOG(LM_DEBUG) << "MaximizeIFPower::setControlValue(VD) pol=" << currentPol_m << " VD=" << VD_m << endl;
            WCA_m.setPADrainVoltage(currentPol_m, VD_m);
            break;

        case OP_PHASE_NONE:
        default:
            return false;
    }
    // Save the history of control values, limited to 5 elements:
    controlHistory.push_back(control);
    if (controlHistory.size() == 6)
        controlHistory.erase(controlHistory.begin());

    /*
     * TODO: This code was for debugging the oscillation detection.  Probably no longer needed.
    historyStream << fixed << setprecision(2);
    vector<float>::const_reverse_iterator it = controlHistory.rbegin();
    while (it != controlHistory.rend()) {
        historyStream << " " << *it;
        ++it;
    }

    LOG(LM_INFO) << "MaximizeIFPower::setControlValue controlHistory=[" << historyStream.str() << " ]" << endl;
    historyStream.str(string());    // clear the history string.
    */

    // check for oscillations:
    if (controlHistory.size() == 5 && controlHistory[0] == controlHistory[2] && controlHistory[2] == controlHistory[4]) {
        LOG(LM_ERROR) << "MaximizeIFPower::setControlValue: Oscillation detected in phase=" << opPhase_m << "." << endl;
        stopMaximizer();
    }
    return true;
}
    
bool MaximizeIFPower::requestDependentValue(float &depVal) {
    // Return false to indicate that the value will be set later when it is available:
    return false;
}
    
void MaximizeIFPower::optimizeAction() {
    progress_m = 0;
    setProgress(progress_m);

    bool success = true;

    currentPol_m = -1;

    // Write the top-level log header data:
    logOutput(true);

    // pause monitoring:
    coldCart_m.pauseMonitor(true, "MaximizeIFPower");
    
    if (doPol0_m && !stopRequested()) {
        success = optimizeSinglePol(0);
        // we ignore 'success' because the most common cause of failure is an oscillation detected which is normally recoverable.
        VJ01opt_m = VJ1_m;
        VJ02opt_m = VJ2_m;
        VD0opt_m = VD_m;
    }

    progress_m = 50;
    setProgress(progress_m);
    
    if (doPol1_m && !stopRequested()) {
        success = optimizeSinglePol(1);
        // we ignore 'success' because the most common cause of failure is an oscillation detected which is normally recoverable.
        VJ11opt_m = VJ1_m;
        VJ12opt_m = VJ2_m;
        VD1opt_m = VD_m;
    }

    if (success) {
        string msg("MaximizeIFPower: finished successfully.");
        LOG(LM_INFO) << msg << endl;
        setStatusMessage(true, msg);
    }
    setFinished(success);
}       

void MaximizeIFPower::exitAction(bool success) {
    // stop requesting IF power updates:
    setEvent(FEMCEventQueue::EVENT_STOP_IFPOWER, coldCart_m.getBand(), currentPol_m);

    // Send the measurement finished event:
    setEvent(FEMCEventQueue::EVENT_PA_ADJUST_DONE, coldCart_m.getBand(), -1, 0, 100);

    // restart monitoring:
    coldCart_m.pauseMonitor(false);

    // flush the log file:
    if (dataFile_mp) {
        (*dataFile_mp).flush();
        delete dataFile_mp;
        dataFile_mp = NULL;
    }

    // ensure that 100% progress is reported:
    progress_m = 100;
    setProgress(progress_m);

    // if failed, log and report a status message to the client:
    if (!success) {
        string msg("MaximizeIFPower ERROR: optimize failed for pol=");
        msg += to_string(currentPol_m);
        msg += ".";
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
    }

    // Even if failed: save (log) the optimized values:
    float IJ01(0.0), IJ02(0.0), IJ11(0.0), IJ12(0.0);
    float VG0(0.0), VG1(0.0);

    if (doPol0_m) {
        IJ01 = coldCart_m.getSISCurrent(0, 1, 20) * 1000;
        IJ02 = coldCart_m.getSISCurrent(0, 2, 20) * 1000;
        VG0 = WCA_m.getPAGateVoltageSetting(0);
    }
    if (doPol1_m) {
        IJ11 = coldCart_m.getSISCurrent(1, 1, 20) * 1000;
        IJ12 = coldCart_m.getSISCurrent(1, 2, 20) * 1000;
        VG1 = WCA_m.getPAGateVoltageSetting(1);
    }
    LOG(LM_INFO) << fixed << "MixerParam=" << setprecision(1) << freqLO_m << setprecision(2)
                 << "," << VJ01opt_m << "," << VJ02opt_m << "," << VJ11opt_m << "," << VJ12opt_m
                 << "," << IJ01 << "," << IJ02 << "," << IJ11 << "," << IJ12 << endl;

    LOG(LM_INFO) << fixed << "LOParam=" << setprecision(1) << freqLO_m << setprecision(2)
                 << "," << VD0opt_m << "," << VD1opt_m << "," << setprecision(3) << VG0 << "," << VG1 << endl;
}

bool MaximizeIFPower::optimizeSinglePol(int pol) {
    if (stopRequested())
        return false;

    currentPol_m = pol;

    // Set the initial SIS junction voltages and PA drain voltage:
    VJ1_m = startVJ1_m; 
    VJ2_m = startVJ2_m; 
    VD_m = startVD_m;
    coldCart_m.setSISVoltage(currentPol_m, 1, VJ1_m);
    coldCart_m.setSISVoltage(currentPol_m, 2, VJ2_m);
    WCA_m.setPADrainVoltage(currentPol_m, VD_m);

    // Reset the optimization phase variable so that waiting for first reading will work.
    // Reset the failure phase for this pass.
    opPhase_m = failPhase_m = OP_PHASE_NONE;

    // Set the power meter reading and averaging:
    static int averaging = 32;
    setEvent(FEMCEventQueue::EVENT_REQUEST_IFPOWER, coldCart_m.getBand(), currentPol_m, averaging);

    // Wait for the first power meter reading:
    float tmp;
    if (!Maximizer::waitForDependentValue(tmp, 20000, 50)) {
        LOG(LM_ERROR) << "MaximizeIFPower::optimizeSinglePol: failed waiting for initial value." << endl;
        return false;
    }

    // Declare the order and step sizes for the optimization phases:
    const int numPhases(9);
    const OperationPhases opPhases[] = {
        OP_PHASE_1A,
        OP_PHASE_1B,
        OP_PHASE_1C,
        OP_PHASE_2A,
        OP_PHASE_2B,
        OP_PHASE_2C,
        OP_PHASE_3A,
        OP_PHASE_3B,
        OP_PHASE_3C
    };

    // The step sizes are the same for both bands:
    const float stepSizes[] = {
        0.03,
        0.1,
        0.1,
        0.01,
        0.04,
        0.04,
        0.01,
        0.02,
        0.02
    };

    // loop through the phases and measure:
    bool success = true;
    for (int index = 0; index < numPhases; ++index) {
        if (stopRequested()) {
            failPhase_m = opPhase_m;
            index = numPhases;
        } else {
            success = performPhase(opPhases[index], stepSizes[index], true, averaging);
            if (!success && failPhase_m == OP_PHASE_NONE)
                failPhase_m = opPhase_m;
        }
        progress_m += 5;
        setProgress(progress_m);
    }

    // Write the final summary data for this pol:
    opPhase_m = OP_PHASE_POL_DONE;
    logOutput(true);

    if (failPhase_m != OP_PHASE_NONE) {
        std::stringstream msg;
        msg << "MaximizeIFPower ERROR: optimize " << ((stopRequested()) ? "stopped" : "failed") << " in phase=" << failPhase_m;
        LOG(LM_ERROR) << msg.str() << endl;
        setStatusMessage(false, msg.str());
    }
    // reset the averaging to 1-sample:
    setEvent(FEMCEventQueue::EVENT_REQUEST_IFPOWER, coldCart_m.getBand(), currentPol_m, 1);
    // stop reading power meters.
    setEvent(FEMCEventQueue::EVENT_STOP_IFPOWER, coldCart_m.getBand(), currentPol_m);

    return (failPhase_m == OP_PHASE_NONE);
}

bool MaximizeIFPower::performPhase(OperationPhases phase, float step, bool directionUp, int averaging) {
    // Set the phase register:
    opPhase_m = phase;

    // Write the log headers for this phase:
    logOutput(true);

    // Set the power meter reading and averaging:
    setEvent(FEMCEventQueue::EVENT_REQUEST_IFPOWER, coldCart_m.getBand(), currentPol_m, averaging);

    // Clear the control values history array:
    controlHistory.clear();

    LOG(LM_INFO) << "MaximizeIFPower::performPhase: " << opPhase_m << endl;
    bool success = false;

    static float VDMin = 0.0;
    static float VDMax = 2.5;
    static float VJMin = 0.0;
    static float VJMax = 25.0;

    switch (opPhase_m) {
        case OP_PHASE_1A:
        case OP_PHASE_2A:
        case OP_PHASE_3A:
            success = Maximizer::maximize(VD_m, step, VDMin, VDMax, directionUp, maxIter_m, 20000, 50);
            break;

        case OP_PHASE_1B:
        case OP_PHASE_2B:
        case OP_PHASE_3B:
            success = Maximizer::maximize(VJ2_m, step, VJMin, VJMax, directionUp, maxIter_m, 20000, 50);
            break;

        case OP_PHASE_1C:
        case OP_PHASE_2C:
        case OP_PHASE_3C:
            success = Maximizer::maximize(VJ1_m, step, VJMin, VJMax, directionUp, maxIter_m, 20000, 50);
            break;

        default:
            break;
    }
    return success;
}


void MaximizeIFPower::logOutput(bool printHeader) {
    if (!dataFile_mp)
        return;
    if (printHeader) {
        switch (opPhase_m) {
            case OP_PHASE_NONE:
                // print the summary header information for the top of the log file:
                (*dataFile_mp) << "MaximizeIFPower data log" << endl;
                (*dataFile_mp) << endl;
                (*dataFile_mp) << "FreqLO\t" << freqLO_m << endl;
                (*dataFile_mp) << "Optimizing Pol0?\t" << doPol0_m << endl;
                (*dataFile_mp) << "Optimizing Pol1?\t" << doPol1_m << endl;
                (*dataFile_mp) << fixed << setprecision(2) <<  "Starting SIS VJ1\t" << startVJ1_m << endl;
                (*dataFile_mp) << "Starting SIS VJ2\t" << startVJ2_m << endl;
                (*dataFile_mp) << "Starting PA VD\t" << startVD_m << endl;
                (*dataFile_mp) << endl;
                break;

            case OP_PHASE_1A:
            case OP_PHASE_2A:
            case OP_PHASE_3A:
                // print IF power vs LO PA data:
                (*dataFile_mp) << "Max Pif/LO(Power) Search for Pol=" << currentPol_m << ":" << endl;
                (*dataFile_mp) << "PA_VD\tP_SB1\tP_SB2\tP_sum" << endl;
                break;

            case OP_PHASE_1B:
            case OP_PHASE_2B:
            case OP_PHASE_3B:
                // print IF power vs IJ2 data:
                (*dataFile_mp) << "Max PIF/SIS VJ2 Search for Pol=" << currentPol_m << ":" << endl;
                (*dataFile_mp) << "SIS_VJ2\tP_SB1\tP_SB2\tP_sum" << endl;
                break;

            case OP_PHASE_1C:
            case OP_PHASE_2C:
            case OP_PHASE_3C:
                // print IF power vs IJ1 data:
                (*dataFile_mp) << "Max PIF/SIS VJ1 Search for Pol=" << currentPol_m << ":" << endl;
                (*dataFile_mp) << "SIS_VJ1\tP_SB1\tP_SB2\tP_sum" << endl;
                break;

            case OP_PHASE_POL_DONE: {
                    // print the summary information for at the end of optimizing a single pol:
                    Time timestamp;
                    string tsText;
                    setTimeStamp(&timestamp);
                    timestampToText(&timestamp, tsText);
                    float VJ1 = coldCart_m.getSISVoltage(currentPol_m, 1, 8);
                    float VJ2 = coldCart_m.getSISVoltage(currentPol_m, 2, 8);
                    float IJ1 = 1000.0 * coldCart_m.getSISCurrent(currentPol_m, 1, 8);
                    float IJ2 = 1000.0 * coldCart_m.getSISCurrent(currentPol_m, 2, 8);


                    (*dataFile_mp) << "Final bias values:" << endl;
                    (*dataFile_mp) << "FreqLO\tPol\tTS\tSuccess?\tPA_VD(set)\tSIS_VJ1(set)\tSIS_VJ2(set)\tSIS_VJ1(mon)\tSIS_VJ2(mon)\tSIS_IJ1\tSIS_IJ2\tP_SB1(W)\tP_SB2(W)" << endl;
                    (*dataFile_mp) << fixed << setprecision(2)
                                   << freqLO_m << "\t"
                                   << currentPol_m << "\t"
                                   << tsText << "\t"
                                   << ((failPhase_m == OP_PHASE_NONE) ? "OK\t" : "FAILED\t")
                                   << VD_m << "\t"
                                   << VJ1_m << "\t"
                                   << VJ2_m << "\t"
                                   << setprecision(4) << VJ1 << "\t"
                                   << VJ2 << "\t"
                                   << IJ1 << "\t"
                                   << IJ2 << "\t"
                                   << scientific
                                   << powerSb1_m << "\t"
                                   << powerSb2_m << endl;
                    (*dataFile_mp) << endl;
                }
                break;

            default:
                break;
        }
    } else {
        float Psum = powerSb1_m + powerSb2_m;
        switch (opPhase_m) {
            case OP_PHASE_1A:
            case OP_PHASE_2A:
            case OP_PHASE_3A:
                (*dataFile_mp) << fixed << setprecision(2) << VD_m << "\t"
                               << scientific << setprecision(4) << powerSb1_m << "\t" << powerSb2_m << "\t" << Psum << endl;
                break;

            case OP_PHASE_1B:
            case OP_PHASE_2B:
            case OP_PHASE_3B:
                (*dataFile_mp) << fixed << setprecision(2) << VJ2_m << "\t"
                               << scientific << setprecision(4) << powerSb1_m << "\t" << powerSb2_m << "\t" << Psum << endl;
                break;

            case OP_PHASE_1C:
            case OP_PHASE_2C:
            case OP_PHASE_3C:
                (*dataFile_mp) << fixed << setprecision(2) << VJ1_m << "\t"
                               << scientific << setprecision(4) << powerSb1_m << "\t" << powerSb2_m << "\t" << Psum << endl;
                break;

            case OP_PHASE_POL_DONE:
            case OP_PHASE_NONE:
            default:
                break;
        }
    }
}


