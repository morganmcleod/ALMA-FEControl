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

#include "MixerDeflux.h"
#include "XYPlotArray.h"
#include "CartAssembly.h"
#include "logger.h"
#include "stringConvert.h"
#include <fstream>
#include <iomanip>
using namespace std;

void MixerDeflux::reset() {
    cca_m = ca_m.useColdCart();
    wca_m = ca_m.useWCA();
    pol_m = -1;
    IMagMax_m = IMag_m = 0.0;
    stepSleep_m = 50;
    stepSize_m = 1.0;
    band5sweepMode_m = false;
    VDNom0_m = VDNom1_m = 0.0;
    VJNom01_m = VJNom02_m = VJNom11_m = VJNom12_m = 0.0;
    IMagNom01_m = IMagNom02_m = IMagNom11_m = IMagNom12_m = 0.0;
    progress_m = progressIncrement_m = 0.0;
    timerStart_m = 0;
    data_m.clear(); 
    if (dataFile_mp) {
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
}

bool MixerDeflux::start(int pol) {
    if (!cca_m) {
        string msg("MixerDeflux: there's no cold cartridge in this position.");
        setStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    if (!(pol == -1 || pol == 0 || pol == 1)) {
        string msg("MixerDeflux: pol must be 0 or 1, or -1 to indicate both pols.");
        setStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }
    stepSleep_m = 50;
    stepSize_m = 1.0;
    switch (cca_m -> getBand()) {
        case 5:
            // Band 5 production cartridge uses 30 mA.
            IMagMax_m = 30;
            break;
        case 8:
            // Band 8 uses 20 mA.
            IMagMax_m = 20.0;
            break;
        case 10:
            // Band 10 uses 100 mA, sleep longer at each step:
            IMagMax_m = 100.0;
            stepSize_m = 2.0;
            stepSleep_m = 100;
            break;
        case 6:
        case 7:
        case 9:
            // Bands 6, 7 & 9 use 50 mA for maximum magnet current.
            IMagMax_m = 50.0;
            break;
        default:
            // Undefined for this band:
            IMagMax_m = 0.0;
            {
                string msg("MixerDeflux ERROR: maximum magnet current is undefined for this band.");
                setStatusMessage(false, msg);
                LOG(LM_ERROR) << msg << endl;
                return false;
            }
            break;
    }
    pol_m = pol;

    LOG(LM_INFO) << "MixerDeflux::start pol=" << pol_m << " IMagMax=" << IMagMax_m
                 << " stepSize=" << stepSize_m << " stepSleep=" << stepSleep_m
                 << " band5sweepMode=" << band5sweepMode_m << endl;
    
    // Clear the result data set:
    data_m.clear();

    if (!logDir_m.empty()) {
        string fileName, bandTxt, polText;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, fileName, true);    
        cca_m -> getBandText(bandTxt);
        polText = to_string(pol);
        if (pol == -1)
            polText = "Both";
        fileName = logDir_m + "MixerDeflux-" + bandTxt + "-pol" + polText + "-" + fileName + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
    }
    string msg("MixerDeflux: process started.");
    setStatusMessage(true, msg);
    return OptimizeBase::startWorkerThread();
}

void MixerDeflux::optimizeAction() {
    progress_m = 0.0;
    setProgress((short) progress_m);
    
    // TODO: fixed progress increment calculation?
    float steps = (IMagMax_m / stepSize_m) * (pol_m == -1 ? 2 : 1);
    progressIncrement_m = 80.0 / steps;
    
    // Pause monitoring during the measurement:
    ca_m.pauseMonitor(true, true, "MixerDeflux");

    // Save the current LO PA and SIS voltage settings to return to when done:
    if (wca_m) {
        VDNom0_m = wca_m -> getPADrainVoltageSetting(0);
        VDNom1_m = wca_m -> getPADrainVoltageSetting(1);
    }
    VJNom01_m = cca_m -> getSISVoltageSetting(0, 1);
    VJNom11_m = cca_m -> getSISVoltageSetting(1, 1);
    if (cca_m -> hasSb2()) {
        VJNom02_m = cca_m -> getSISVoltageSetting(0, 2);
        VJNom12_m = cca_m -> getSISVoltageSetting(1, 2);
    }

    // Get the *nominal*  IMag settings from the configuration database to return to when done:
    if (ca_m.getSISMagnetNominal(IMagNom01_m, IMagNom02_m, IMagNom11_m, IMagNom12_m)) {
        LOG(LM_INFO) << "MixerDeflux: Got nominal magnet current settings: "
                     << IMagNom01_m << ", "
                     << IMagNom02_m << ", "
                     << IMagNom11_m << ", "
                     << IMagNom12_m << endl;
    } else {
        // use the current settings instead:
        IMagNom01_m = cca_m -> getSISMagnetCurrentSetting(0, 1);
        IMagNom02_m = cca_m -> getSISMagnetCurrentSetting(0, 2);
        IMagNom11_m = cca_m -> getSISMagnetCurrentSetting(1, 1);
        IMagNom12_m = cca_m -> getSISMagnetCurrentSetting(1, 2);
        LOG(LM_INFO) << "MixerDeflux: Using present magnet current settings: "
                     << IMagNom01_m << ", "
                     << IMagNom02_m << ", "
                     << IMagNom11_m << ", "
                     << IMagNom12_m << endl;
    }

    // Disable the LO PA:
    if (wca_m) {
        wca_m -> setPADrainVoltage(0, 0.0);
        wca_m -> setPADrainVoltage(1, 0.0);
    }
    // Set all SIS voltages and magnet currents to zero
    cca_m -> setSISVoltage(0, 1, 0.0);
    cca_m -> setSISVoltage(1, 1, 0.0);
    cca_m -> setSISMagnetCurrent(0, 1, 0.0);
    cca_m -> setSISMagnetCurrent(1, 1, 0.0);
    if (cca_m -> hasSb2()) {
        cca_m -> setSISVoltage(0, 2, 0.0);
        cca_m -> setSISVoltage(1, 2, 0.0);
        cca_m -> setSISMagnetCurrent(0, 2, 0.0);
        cca_m -> setSISMagnetCurrent(1, 2, 0.0);
    }

    timerStart_m = GETTIMER();

    IMag_m = 0.0;
    // Write the log file header:
    readData(0, 0, true);
    
    // Demagnetize pol0 if selected:
    if (pol_m == -1 || pol_m == 0) {
        if (!stopRequested())
            demagnetize(0, 1);
        if (!stopRequested() && cca_m -> hasSb2Magnet())
            demagnetize(0, 2);
    }

    // Demagnetize pol1 if selected:
    if (pol_m == -1 || pol_m == 1) {
        if (!stopRequested())
            demagnetize(1, 1);
        if (!stopRequested() && cca_m -> hasSb2Magnet())
            demagnetize(1, 2);
    }

    setProgress(80);

    if (!stopRequested()) {
        // Try to heat the mixers to 12 K for all bands:
        float targetTemp = 12;

        // Perform the mixer heating process on the selected polarization, or both if requested,
        //   with the specified target temperature and the default timeout:
        cca_m -> sisMixerHeatingProcess(pol_m, targetTemp, 60);

        // restart monitoring:
        ca_m.pauseMonitor(false, false);

        setProgress(85);

        // Sweep all magnet currents back to nominal + 10%:
        cca_m -> setSISMagnetCurrent(0, 1, (1.10 * IMagNom01_m));
        cca_m -> setSISMagnetCurrent(1, 1, (1.10 * IMagNom11_m));
        if (cca_m -> hasSb2()) {
            cca_m -> setSISMagnetCurrent(0, 2, (1.10 * IMagNom02_m));
            cca_m -> setSISMagnetCurrent(1, 2, (1.10 * IMagNom12_m));
        }

        setProgress(90);

        // Sweep all magnet currents back to 0:
        cca_m -> setSISMagnetCurrent(0, 1, 0.0);
        cca_m -> setSISMagnetCurrent(1, 1, 0.0);
        if (cca_m -> hasSb2()) {
            cca_m -> setSISMagnetCurrent(0, 2, 0.0);
            cca_m -> setSISMagnetCurrent(1, 2, 0.0);
        }

        setProgress(95);

        // Sweep all magnet currents back to nominal:
        cca_m -> setSISMagnetCurrent(0, 1, IMagNom01_m);
        cca_m -> setSISMagnetCurrent(1, 1, IMagNom11_m);
        if (cca_m -> hasSb2()) {
            cca_m -> setSISMagnetCurrent(0, 2, IMagNom02_m);
            cca_m -> setSISMagnetCurrent(1, 2, IMagNom12_m);
        }

        // Sweep all junction voltages back to nominal:
        cca_m -> setSISVoltage(0, 1, VJNom01_m);
        cca_m -> setSISVoltage(1, 1, VJNom11_m);
        if (cca_m -> hasSb2()) {
            cca_m -> setSISVoltage(0, 2, VJNom02_m);
            cca_m -> setSISVoltage(1, 2, VJNom12_m);
        }

        // Re-enable the LO PA:
        if (wca_m) {
            wca_m -> setPADrainVoltage(0, VDNom0_m);
            wca_m -> setPADrainVoltage(1, VDNom1_m);
        }
    }
    
    // flush the log file:
    if (dataFile_mp) {
        (*dataFile_mp).flush();
        delete dataFile_mp;
        dataFile_mp = NULL;
    }

    // Send the measurement finished event:
    setProgress(100);
    setEvent(FEMCEventQueue::EVENT_OPTIMIZE_DONE, cca_m -> getBand(), -1, 0, 100);
    setEvent(FEMCEventQueue::EVENT_ALL_REPS_DONE, cca_m -> getBand(), -1, 0, 100);
    if (stopRequested()) {
        string msg("MixerDeflux: process stopped.");
        setStatusMessage(false, msg);
        setFinished(false);
    } else {
        string msg("MixerDeflux: finished successfully.");
        setStatusMessage(true, msg);
        setFinished(true);
    }
}

void MixerDeflux::demagnetize(int pol, int sb) {
    // Take an initial reading of the data points:
    readData(pol, sb);
    
    bool done = false;
    IMag_m = IMagMax_m;
    while (!done && !stopRequested()) {
        // step or sweep to +IMag_m:
        goToMagnetCurrent(pol, sb, IMag_m);
        // if we are actually at or below zero, stop here:
        if (IMag_m <= 0.0)
            done = true;
        else {
            // otherwise step or sweep to -IMag_m:
            IMag_m = -IMag_m;
            goToMagnetCurrent(pol, sb, IMag_m);
            // flip IMag_m back to positive and reduce by stepSize:
            IMag_m = -IMag_m - stepSize_m;
        }
        progress_m += progressIncrement_m;
        setProgress((short) progress_m);
    }
    // make sure we are at 0 at the end:
    goToMagnetCurrent(pol, sb, 0.0);
    // Take a reading of the data points:
    readData(pol, sb);
}


void MixerDeflux::goToMagnetCurrent(int pol, int sb, float IMagTarget) {
    if (band5sweepMode_m) {
        // sweep with 0.25 mA steps and 100 ms dwell at each step:
        cca_m -> setSISMagnetCurrent(pol, sb, IMagTarget, true, 0.25, 100);
        SLEEP(stepSleep_m);
        readData(pol, sb);
    } else {
        // no sweeping, go to zero and then to the commanded magnet current:
        cca_m -> setSISMagnetCurrent(pol, sb, 0.0, false);
        SLEEP(stepSleep_m);
        readData(pol, sb);
        SLEEP(stepSleep_m);
        readData(pol, sb);
        cca_m -> setSISMagnetCurrent(pol, sb, IMagTarget, false);
        SLEEP(stepSleep_m);
        readData(pol, sb);
        SLEEP(stepSleep_m);
        readData(pol, sb);
    }
}

void MixerDeflux::readData(int pol, int sb, bool printHeader) {
    if (printHeader && dataFile_mp) {
        (*dataFile_mp) << "time sec\tpol\tsb\tmixerTemp K\tmagnet1Current mA\tmagnet2Current mA\theaterCurrent mA\t"
                       << "SIS1 voltage mV\tSIS1 current uA\tSIS2 voltage mV\tSIS2 current uA" << endl;
    } else {
        float time = ((float) (GETTIMER() - timerStart_m)) / 1000.0;
        float mixerTemp = (pol == 0) ? cca_m -> cartridgeTemperature2() : cca_m -> cartridgeTemperature5();
        float magnet1Current = cca_m -> getSISMagnetCurrent(pol, 1);
        float magnet2Current = cca_m -> getSISMagnetCurrent(pol, 2);
        float heaterCurrent = cca_m -> getSISHeaterCurrent(pol);
        float SIS1Voltage = cca_m -> getSISVoltage(pol, 1);
        float SIS1Current = 1000.0 * cca_m -> getSISCurrent(pol, 2);
        float SIS2Voltage = cca_m -> getSISVoltage(pol, 1);
        float SIS2Current = 1000.0 * cca_m -> getSISCurrent(pol, 2);
        data_m.push_back(XYPlotPoint(time, (sb == 2) ? magnet2Current : magnet1Current, heaterCurrent));
        if (dataFile_mp) {
            (*dataFile_mp) << time << "\t" << pol << "\t" << sb << "\t" << mixerTemp << "\t" 
                           << magnet1Current << "\t" << magnet2Current << "\t" << heaterCurrent << "\t"
                           << SIS1Voltage  << "\t" << SIS1Current  << "\t" << SIS2Voltage << "\t" << SIS2Current << endl;
        }
    }
}



