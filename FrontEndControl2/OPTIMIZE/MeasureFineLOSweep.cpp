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

#include "MeasureFineLOSweep.h"
#include "logger.h"
#include "stringConvert.h"
#include "CartAssembly.h"
#include <fstream>
#include <iomanip>
#include <math.h>
using namespace std;

void MeasureFineLOSweep::reset() {
    // reset all state to initial/just constructed.
    pol_m = -1;
    tiltAngle_m = -1;
    VJNom01_m = VJNom02_m = VJNom11_m = VJNom12_m = VDNom0_m = VDNom1_m = 0.0;
    freqLONom_m = freqFLOOG_m = 0.0;
    sbLock_m = 0;
    VJ_m = IJ_m = 0.0;
    fixedVD_m = -1.0;
    LOStart_m = LOStop_m = LOStep_m = FLO_m = 0.0;
    steps_m = repeatCount_m = 0;
    progress_m = progressIncrement_m = 0;
    sisCurrents_m.clear();
    loPaVoltages_m.clear();
}

bool MeasureFineLOSweep::start(const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus,
        float tiltAngle, int pol, float VJ, float IJ,
        const float *fixedVD,
        double LOStart, double LOStop, double LOStep, int repeatCount)
{
    // Check parameters:
    if (pol < -1 || pol > 1) {
        string msg("MeasureFineLOSweep ERROR: polarization must be 0 or 1, or -1 indicating both.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);        
        return false;
    }
    if (VJ < -25 || VJ > 25) {
        string msg("MeasureFineLOSweep ERROR: junction voltage is out of range.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);        
        return false;
    }
    if (IJ < -150 || IJ > 150) {
        string msg("MeasureFineLOSweep ERROR: junction current is out of range.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);        
        return false;
    }
    if (LOStart == LOStop) {
        string msg("MeasureFineLOSweep ERROR: LOStop must be different from LOStart.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);        
        return false;
    }
    if (LOStart < 20.0 || LOStart > 1000.0 || LOStop < 20.0 || LOStop > 1000.0) {
        string msg("MeasureFineLOSweep ERROR: LOStart and LOStop must be valid frequencies in GHz.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        return false;
    }
    if (repeatCount <= 0)
        repeatCount = 1;

    // Setup the operating variables:
    feConfig_m = feConfig;
    dataStatus_m = dataStatus;
    tiltAngle_m = tiltAngle;
    pol_m = pol;
    VJ_m = VJ;
    IJ_m = IJ;
    fixedVD_m = ((fixedVD) ? *fixedVD : -1.0);
    LOStart_m = LOStart;
    LOStop_m = LOStop;
    LOStep_m = LOStep;
    repeatCount_m = repeatCount;
    
    double span = LOStop_m - LOStart_m;
    double sign = (span < 0.0) ? -1.0 : 1.0;
    span = fabs(span);
    LOStep_m = fabs(LOStep_m);

    if (LOStep_m == 0)
        LOStep_m = 0.1;
    else if (LOStep_m > span)
        LOStep_m = span;

    steps_m = (int) (span / LOStep_m) + 1;
    
    LOStep_m *= sign;

    LOG(LM_INFO) << "MeasureFineLOSweep::start tiltAngle=" << fixed << setprecision(2) << tiltAngle_m
        << " pol=" << pol_m
        << setprecision(3) << " VJ=" << VJ_m << " IJ=" << IJ_m << " fixedVD=" << ((fixedVD) ? to_string(fixedVD_m, std::fixed, 3) : "none")
        << setprecision(6) << " LOStart=" << LOStart_m << " LOStop=" << LOStop_m << " LOStep=" << LOStep_m << " steps=" << steps_m
        << " repeatCount=" << repeatCount_m << endl;

    // Set up the progress bar variables:
    setProgress(0);
    //progress_m = 0.0;
    //progressIncrement_m = 100.0 / (repeatCount_m * steps_m);
    
    // Create the log file:
    if (!logDir_m.empty()) {
        string fileName, bandTxt, polText;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);    
        cartAssembly_m.getBandText(bandTxt);
        polText = (pol_m == -1) ? "Both" : to_string(pol);
        fileName = logDir_m + "FineLOSweep-" + bandTxt + "-pol" + polText + "-" + fileName + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);

        // Print the headers to the data log file:
        (*dataFile_mp) << "Pol\tFLO GHz (set)\tIJ1 uA\tIJ2 uA\tPA drain control (set)\tPA drain V" << endl;
    }
    
    // Start the worker thread:
    return OptimizeBase::startWorkerThread();
}
    
void MeasureFineLOSweep::optimizeAction() {
    // Pause monitoring during the measurement:
    cartAssembly_m.pauseMonitor(true, true, "Measure fine LO sweep");
    
    // Post a status message for the GUI user:
    string msg("MeasureFineLOSweep: process started.");
    setStatusMessage(true, msg);

    // Save the nominal values so we can go back to them when done:
    VJNom01_m = cartAssembly_m.getColdCart() -> getSISVoltageSetting(0, 1);
    VJNom02_m = cartAssembly_m.getColdCart() -> getSISVoltageSetting(0, 2);
    VJNom11_m = cartAssembly_m.getColdCart() -> getSISVoltageSetting(1, 1);
    VJNom12_m = cartAssembly_m.getColdCart() -> getSISVoltageSetting(1, 2);
    VDNom0_m = cartAssembly_m.getWCA() -> getPADrainVoltageSetting(0);
    VDNom1_m = cartAssembly_m.getWCA() -> getPADrainVoltageSetting(1);

    double trash;
    cartAssembly_m.getLOFrequency(freqLONom_m, trash);
    cartAssembly_m.getLOLockSettings(freqFLOOG_m, sbLock_m);

    // outer loop on repeatCount:
    bool error = false;
    int iter = 0;
    while (!error && iter < repeatCount_m && !stopRequested()) {
        ++iter;

        if (pol_m == 0 || pol_m == -1) {
            // Set the SIS voltages:
            cartAssembly_m.useColdCart() -> setSISVoltage(0, 1, VJ_m);
            cartAssembly_m.useColdCart() -> setSISVoltage(0, 2, VJ_m);

            // measure pol0:
            if (!cartAssembly_m.measureFineLOSweepSingleSynchronous(0, VJ_m, IJ_m, fixedVD_m, VDNom0_m, LOStart_m, LOStop_m, LOStep_m, sisCurrents_m, loPaVoltages_m))
                error = true;
            else
                saveData(0);
        }
        if (pol_m == 1 || pol_m == -1) {
            // Set the SIS voltages:
            cartAssembly_m.useColdCart() -> setSISVoltage(1, 1, VJ_m);
            cartAssembly_m.useColdCart() -> setSISVoltage(1, 2, VJ_m);

            // measure pol1:
            if (!cartAssembly_m.measureFineLOSweepSingleSynchronous(1, VJ_m, IJ_m, fixedVD_m, VDNom1_m, LOStart_m, LOStop_m, LOStep_m, sisCurrents_m, loPaVoltages_m))
                error = true;
            else
                saveData(1);
        }
    }        
    if (!error) {
        string msg("MeasureFineLOSweep: finished successfully.");
        setStatusMessage(true, msg);
    }
    setFinished(!error);
}

void MeasureFineLOSweep::exitAction(bool success) {
    string context("MeasureFineLOSweep::exitAction");

    // restart monitoring:
    cartAssembly_m.pauseMonitor(false, false);
    
    // set the previous LO frequency:
    if (freqLONom_m > 0.0) {
        if (!cartAssembly_m.setLOFrequency(freqLONom_m, freqFLOOG_m, sbLock_m)) {
            LOG(LM_ERROR) << "MeasureFineLOSweep ERROR: failed setting LO frequency to " << freqLONom_m << " GHz in exitAction: out of range." << endl;
        }
    }
    // set the previous SIS voltages:
    cartAssembly_m.useColdCart() -> setSISVoltage(0, 1, VJNom01_m);
    cartAssembly_m.useColdCart() -> setSISVoltage(0, 2, VJNom02_m);
    cartAssembly_m.useColdCart() -> setSISVoltage(1, 1, VJNom11_m);
    cartAssembly_m.useColdCart() -> setSISVoltage(1, 2, VJNom12_m);

    // set the previous LO PA drain voltage:
    cartAssembly_m.useWCA() -> setPADrainVoltage(0, VDNom0_m);
    cartAssembly_m.useWCA() -> setPADrainVoltage(1, VDNom1_m);

    // flush the log file:
    if (dataFile_mp) {
        (*dataFile_mp).flush();
        delete dataFile_mp;
        dataFile_mp = NULL;
    }

    // Send the measurement finished event:
    setEvent(FEMCEventQueue::EVENT_OPTIMIZE_DONE, cartAssembly_m.getBand(), -1, 0, 100);
}

void MeasureFineLOSweep::saveData(int pol) {
    string context("MeasureFineLOSweep::saveData");

    if (dataFile_mp) {
        XYPlotArray::const_iterator IJit, VDit;
        for (IJit = sisCurrents_m.begin(), VDit = loPaVoltages_m.begin(); IJit != sisCurrents_m.end(); ++IJit, ++VDit) {

            (*dataFile_mp) << pol << "\t" << to_string((*IJit).X, std::fixed, 3)
                                  << "\t" << to_string((*IJit).Y1, std::fixed, 2)
                                  << "\t" << to_string((*IJit).Y2, std::fixed, 2)
                                  << "\t" << to_string((*VDit).Y1, std::fixed, 2)
                                  << "\t" << to_string((*VDit).Y2, std::fixed, 2) << endl;
        }
    }

    // find or create the test data header for this data:
    string legend = "pol " + to_string(pol) + " tiltAngle=" + to_string(tiltAngle_m, std::fixed, 2);

    FrontEndDatabase::ID_T headerId, subHeaderId;
    int band = cartAssembly_m.getBand();
    if (!dbObject_m.findOrCreateFineLOSSweepDataHeader(headerId, feConfig_m, dataStatus_m, band, string()))
        LOG(LM_ERROR) << context << ": findOrCreateFineLOSSweepDataHeader failed." << endl;

    // create a sub-header record:
    else if (!dbObject_m.insertFineLOSweepSubHeader(subHeaderId, headerId, band, pol, tiltAngle_m, LOStart_m, LOStop_m, LOStep_m, VJ_m, IJ_m, legend))
        LOG(LM_ERROR) << context << ": insertFineLOSweepSubHeader failed." << endl;

    // and insert the raw data:
    else if (!dbObject_m.insertFineLOSSweepData(subHeaderId, sisCurrents_m, loPaVoltages_m))
        LOG(LM_ERROR) << context << ": insertFineLOSSweepData failed." << endl;
}
