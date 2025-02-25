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

#include "MeasureSISCurrent.h"
#include "XYPlotArray.h"
#include "logger.h"
#include "stringConvert.h"
#include "ColdCartImpl.h"
#include <fstream>
#include <iomanip>
#include <math.h>
using namespace std;

void MeasureSISCurrent::reset() {
    pol_m = sbMagnet_m = sbMixer_m = -1;
    VJ1Nom_m = IMag1Nom_m = VJ2Nom_m = IMag2Nom_m = 0;
    IMagStart_m = IMagStop_m = IMagStep_m = IMagSet_m = VJLow_m = VJHigh_m = VJStep_m = 0;
    repeatCount_m = 0;
    progress_m = 0;
    data_m.clear();
    if (dataFile_mp) {
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
}

bool MeasureSISCurrent::start(int pol, int sbMixer,
                              float IMagStart, float IMagStop, float IMagStep, 
                              float VJLow, float VJHigh, float VJStep, int repeatCount) 
{
    // check parameters:
    if (pol < 0 || pol > 1) {
        string msg("MeasureSISCurrent ERROR: polarization must be 0 or 1.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        return false;
    }
    if (!(sbMixer == -1 || sbMixer == 1 || sbMixer == 2)) {
        string msg("MeasureSISCurrent ERROR: sbMixer must be 1 or 2, or -1 indicating both.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        return false;
    }
    sbMagnet_m = sbMixer_m = -1;
    switch (coldCart_m.getBand()) {
        case 6:
            sbMagnet_m = 1;
            sbMixer_m = sbMixer;
            break;

        case 5:
        case 7:
        case 8:
            sbMagnet_m = sbMixer_m = sbMixer;
            break;

        case 9:
        case 10:
            sbMagnet_m = sbMixer_m = 1;
            break;

        default:
            string msg("MeasureSISCurrent ERROR: not supported for the selected band.");
            LOG(LM_ERROR) << msg << endl;
            setStatusMessage(false, msg);
            return false;
    }
    if (IMagStart == IMagStop) {
        string msg("MeasureSISCurrent ERROR: IMagStart and IMagStop must specify a range.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        return false;
    }
    if (IMagStart < -100 || IMagStart > 100 || IMagStop < -100 || IMagStop > 100) {
        string msg("MeasureSISCurrent ERROR: IMagStart and IMagStop must both be in -100 to +100 mA.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        return false;
    }
    if (VJLow < -50 || VJLow > 50 || VJHigh < -50 || VJHigh > 50) {
        string msg("MeasureSISCurrent ERROR: VJLow and VJHigh must both be in -50 to +50 mV.");
        LOG(LM_ERROR) << msg << endl;
        setStatusMessage(false, msg);
        return false;
    }
    if (VJLow > 0 && (VJHigh < VJLow || VJStep <= 0)) {
        VJHigh = VJLow;
        VJStep = 0;
        LOG(LM_INFO) << "MeasureSISCurrent: Not sweeping VJ.  It will be held fixed at " << VJLow << " mV." << endl;
    }
    // setup operating variables:
    pol_m = pol;
    IMagStart_m = IMagStart;
    IMagStop_m = IMagStop;
    IMagStep_m = IMagStep;
    VJLow_m = VJLow;
    VJHigh_m = VJHigh;
    VJStep_m = VJStep;
    repeatCount_m = repeatCount;

    if (!logDir_m.empty()) {
        string fileName, bandTxt, polText;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);    
        coldCart_m.getBandText(bandTxt);
        polText = to_string(pol_m);
        fileName = logDir_m + "IJvsIMag-" + bandTxt + "-pol" + polText + "-" + fileName + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
    }
    string msg("MeasureSISCurrent: process started.");
    setStatusMessage(true, msg);
    return OptimizeBase::startWorkerThread();
} 

void MeasureSISCurrent::optimizeAction() {
    LOG(LM_INFO) << "MeasureSISCurrent::optimizeAction pol=" << pol_m << " sbMagnet=" << sbMagnet_m << " sbMixer=" << sbMixer_m 
        << fixed << setprecision(3) 
        << " IMagStart=" << IMagStart_m << " IMagStop=" << IMagStop_m << " IMagStep=" << IMagStep_m
        << " VJLow=" << VJLow_m << " VJHigh=" << VJHigh_m << " VJStep=" << VJStep_m
        << " repeatCount=" << repeatCount_m << endl;
    
    data_m.clear();
    
    // calculate sweep and step sizes:
    float IMagSpan = IMagStop_m - IMagStart_m;
    float IMagSign = (IMagSpan < 0.0) ? -1.0 : 1.0;
    IMagSpan = fabsf(IMagSpan);
    IMagStep_m = fabsf(IMagStep_m);
    
    if (IMagStep_m == 0)
        IMagStep_m = 0.1;       
    else if (IMagStep_m > IMagSpan)
        IMagStep_m = IMagSpan;

    int IMagSteps = (int) (IMagSpan / IMagStep_m) + 1;
    IMagStep_m *= IMagSign;
    
    float VJSpan = VJHigh_m - VJLow_m;
    int VJSteps = (VJSpan <= 0.0) ? 1 : (int) (VJSpan / VJStep_m) + 1;
    
    data_m.reserve(repeatCount_m * IMagSteps);
       
    setProgress(0);
    progress_m = 0.0;
    float progressIncrement = 100.0 / (repeatCount_m * IMagSteps);

    coldCart_m.pauseMonitor(true, "Measure IJ vs SIS magnet.");

    // write the data file header row:
    readData(0, true);

    // cache the prior values of SIS voltage and magnet current to return to at the end:
    VJ1Nom_m = coldCart_m.getSISVoltageSetting(pol_m, 1);
    IMag1Nom_m = coldCart_m.getSISMagnetCurrentSetting(pol_m, 1);
    VJ2Nom_m = coldCart_m.getSISVoltageSetting(pol_m, 2);
    IMag2Nom_m = coldCart_m.getSISMagnetCurrentSetting(pol_m, 2);

    // sweep the SIS magnet(s) current down to 0:
    if (sbMagnet_m == 1 || sbMagnet_m == -1)
        coldCart_m.setSISMagnetCurrent(pol_m, 1, 0.0);
    if (sbMagnet_m == 2 || sbMagnet_m == -1)
        coldCart_m.setSISMagnetCurrent(pol_m, 2, 0.0);

    // set the starting SIS voltage(s):
    if (sbMixer_m == 1 || sbMixer_m == -1)
        coldCart_m.setSISVoltage(pol_m, 1, VJLow_m);
    if (sbMixer_m == 2 || sbMixer_m == -1)
        coldCart_m.setSISVoltage(pol_m, 2, VJLow_m);
    
    // loop on repeatCount:
    int iter = 0;
    while (iter < repeatCount_m && !stopRequested()) {
        ++iter;
        IMagSet_m = IMagStart_m;

        // loop on IMagSteps:
        int step = 0;
        while (step < IMagSteps && !stopRequested()) {
            ++step;
            // set the magnet(s) current to the present value of IMagSet_m:
            if (sbMagnet_m == 1 || sbMagnet_m == -1)
                coldCart_m.setSISMagnetCurrent(pol_m, 1, IMagSet_m);
            if (sbMagnet_m == 2 || sbMagnet_m == -1)
                coldCart_m.setSISMagnetCurrent(pol_m, 2, IMagSet_m);

            // write to the log file:
            readData(VJSteps, false);

            // move to the next magnet step and update progress bar:
            IMagSet_m += IMagStep_m;
            progress_m += progressIncrement;
            setProgress((short) progress_m);
        }
        setEvent(FEMCEventQueue::EVENT_OPTIMIZE_DONE, coldCart_m.getBand(), -1, 0, 100);
    }

    // sweep the SIS magnet(s) current down to 0:
    if (sbMagnet_m == 1 || sbMagnet_m == -1)
        coldCart_m.setSISMagnetCurrent(pol_m, 1, 0.0);
    if (sbMagnet_m == 2 || sbMagnet_m == -1)
        coldCart_m.setSISMagnetCurrent(pol_m, 2, 0.0);

    // sweep the SIS magnet(s) current back up to nominal:
    if (sbMagnet_m == 1 || sbMagnet_m == -1)
        coldCart_m.setSISMagnetCurrent(pol_m, 1, IMag1Nom_m);
    if (sbMagnet_m == 2 || sbMagnet_m == -1)
        coldCart_m.setSISMagnetCurrent(pol_m, 2, IMag2Nom_m);

    // set the SIS voltage(s) back to nominal:
    if (sbMixer_m == 1 || sbMixer_m == -1)
        coldCart_m.setSISVoltage(pol_m, 1, VJ1Nom_m);
    if (sbMixer_m == 2 || sbMixer_m == -1)
        coldCart_m.setSISVoltage(pol_m, 2, VJ2Nom_m);

    // re-enable normal monitoring:
    coldCart_m.pauseMonitor(false);
    
    // flush and close the data file:
    if (dataFile_mp) {
        (*dataFile_mp).flush();
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
    setProgress(100);
    setEvent(FEMCEventQueue::EVENT_ALL_REPS_DONE, coldCart_m.getBand(), -1, 0, 100);
    if (stopRequested()) {
        string msg("MeasureSISCurrent: process stopped.");
        setStatusMessage(false, msg);
        setFinished(false);
    } else {
        string msg("MeasureSISCurrent: finished successfully.");
        setStatusMessage(true, msg);
        setFinished(true);
    }
}

void MeasureSISCurrent::readData(int VJSteps, bool printHeader) {
    if (printHeader) { 
        if (dataFile_mp)
            (*dataFile_mp) << "IMag1 mV (set)\tIMag2 mV (set)\tIJ1 uA\tIJ2 uA" << endl;
    } else {
        float IJ1(0.0), IJ2(0.0);
        if (VJSteps == 1) {
            // simply read mixer current with averaging(8) if not sweeping junction voltage:
            if (sbMixer_m == 1 || sbMixer_m == -1)
                IJ1 = 1000.0 * coldCart_m.getSISCurrent(pol_m, 1, 8);
            if (sbMixer_m == 2 || sbMixer_m == -1)
                IJ2 = 1000.0 * coldCart_m.getSISCurrent(pol_m, 2, 8);
            SLEEP(1);
        } else {
            // sweeping junction voltage.  Measure max-min mixer current seen:
            float IJ1Min(999.0), IJ1Max(-999.0);
            float IJ2Min(999.0), IJ2Max(-999.0);
            float VJ(VJLow_m);

            // loop over number of VJ steps:
            int step = 0;
            while (step < VJSteps && !stopRequested()) {
                ++step;
                // set the mixer voltage(s):
                if (sbMixer_m == 1 || sbMixer_m == -1)
                    coldCart_m.setSISVoltage(pol_m, 1, VJ);
                if (sbMixer_m == 2 || sbMixer_m == -1)
                    coldCart_m.setSISVoltage(pol_m, 2, VJ);
                SLEEP(1);
                // measure mixer current with averaging(8):
                if (sbMixer_m == 1 || sbMixer_m == -1)
                    IJ1 = coldCart_m.getSISCurrent(pol_m, 1, 8);
                if (sbMixer_m == 2 || sbMixer_m == -1)
                    IJ2 = coldCart_m.getSISCurrent(pol_m, 2, 8);
                // accumulate max and min:
                if (IJ1 < IJ1Min) IJ1Min = IJ1;
                if (IJ1 > IJ1Max) IJ1Max = IJ1;
                if (IJ2 < IJ2Min) IJ2Min = IJ2;
                if (IJ2 > IJ2Max) IJ2Max = IJ2;
                // next VJ step:
                VJ += VJStep_m;
            }
            // calculate (max-min) / 2:
            if (sbMixer_m == 1 || sbMixer_m == -1)
                IJ1 = 1000.0 * ((IJ1Max - IJ1Min) / 2.0);
            if (sbMixer_m == 2 || sbMixer_m == -1)
                IJ2 = 1000.0 * ((IJ2Max - IJ2Min) / 2.0);
        }
        if (sbMixer_m == 1)
            data_m.push_back(XYPlotPoint(IMagSet_m, IJ1, 0));
        else if (sbMixer_m == 2)
            data_m.push_back(XYPlotPoint(IMagSet_m, IJ2, 0));
        else
            data_m.push_back(XYPlotPoint(IMagSet_m, IJ1, IJ2));
        if (dataFile_mp) {
            if (sbMagnet_m == 1 || sbMagnet_m == -1)
                (*dataFile_mp) << IMagSet_m;
            else
                (*dataFile_mp) << IMag1Nom_m;
            (*dataFile_mp) << "\t";
            if (sbMagnet_m == 2 || sbMagnet_m == -1)
                (*dataFile_mp) << IMagSet_m;
            else
                (*dataFile_mp) << IMag2Nom_m;
            (*dataFile_mp) << "\t" << IJ1 << "\t" << IJ2 << endl;
        }
    }
}

