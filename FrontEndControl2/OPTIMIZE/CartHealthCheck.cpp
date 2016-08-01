/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
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

#include "CartHealthCheck.h"
#include "CONFIG/cartConfig.h"
#include "XYPlotArray.h"
#include "CartAssembly.h"
#include "DLL/SWVersion.h"
#include "logger.h"
#include "stringConvert.h"
using namespace std;
using namespace FEConfig;

void CartHealthCheck::reset() {
    ivData_m.clear();
    feConfig_m.reset();
    dataStatus_m = FEICDataBase::DS_UNKOWN;
    receiverIsCold_m = false;
    warmUpTimeSeconds_m = 10;
    freqLO_m = 0;
}

bool CartHealthCheck::prepare(const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus,
        double &freqLOret, bool receiverIsCold, int warmUpTimeSeconds)
{
    static const string context("CartHealthCheck::prepare");

    freqLO_m = freqLOret = 0;
    feConfig_m = feConfig;
    dataStatus_m = dataStatus;
    receiverIsCold_m = receiverIsCold;
    warmUpTimeSeconds_m = warmUpTimeSeconds;
    if (warmUpTimeSeconds_m < 10)
        warmUpTimeSeconds_m = 10;

    // Get the configuration details for the CartAssembly:
    int band = ca_m.getBand();
    bool hasSb2 = ColdCartImpl::hasSb2(band);
    bool hasCCA = ca_m.existsColdCart();

    if (!ca_m.existsWCA()) {
        string msg("CartHealthCheck ERROR: no WCA configured for band=");
        msg += to_string(band);
        msg += ". Heath check is not supported for a CCA alone.";
        setStatusMessage(false, msg);
        LOG(LM_INFO) << context << ": " << msg << endl;
        return false;
    }

    string msg("CartHealthCheck: process started.");
    setStatusMessage(true, msg);

    double freqLO;
    int pol, sb;
    ParamTableRow pRow, mRow;
    ParamTable::const_iterator it;

    // output the coldCart configuration if a CCA is configured:
    if (hasCCA) {
        const ColdCartConfig &ccaConfig = ca_m.getColdCartConfig();

        // Output the "MixerParams" records:
        LOG(LM_INFO) << context << ": got CCA mixer params for band=" << band << endl;
        LOG(LM_INFO) << "FreqLO,Pol,Sb,VJ,IJ,IMAG" << endl;

        // iterate over the MixerParams table:
        const MixerParams &mixp = ccaConfig.mixerParams_m;
        const MagnetParams &magp = ccaConfig.magnetParams_m;
        for (it = mixp.begin(); it != mixp.end(); ++it) {
            // get each LO frequency and row:
            if (mixp.get(it, freqLO, pRow)) {
                // get the corresponding MagnetParams row, no interpolation:
                bool magOk = magp.get(freqLO, mRow, false);
                // for each pol and sb:
                for (pol = 0; pol <= 1; ++pol) {
                    for (sb = 1; sb <= (hasSb2 ? 2 : 1); ++sb) {
                        // output a row combining mixerparams and magnetparams together:
                        LOG(LM_INFO) << fixed << setw(mixp.freqWidth_m) << setprecision(mixp.freqPrecision_m)
                                     << freqLO << "," << pol << "," << sb << ","
                                     << setw(pRow.fWidth_m) << setprecision(pRow.fPrecision_m)
                                     << pRow[MixerParams::indexVJ(pol, sb)] << ","
                                     << pRow[MixerParams::indexIJ(pol, sb)] << ","
                                     << (magOk ? mRow[MagnetParams::indexIMag(pol, sb)] : 0.0) << endl;
                    }
                }
            }
        }

        // Output the PreampParams records:
        LOG(LM_INFO) << context << ": got CCA preamp params for band=" << band << endl;
        LOG(LM_INFO) << "FreqLO,Pol,Sb,VD1,VD2,VD3,ID1,ID2,ID3,VG1,VG2,VG3" << endl;

        // for each pol and sb:
        for (pol = 0; pol <= 1; ++pol) {
            for (sb = 1; sb <= (hasSb2 ? 2 : 1); ++sb) {
                // iterate over the table of PreampParams:
                const PreampParams &prep = ccaConfig.getPreampParams(pol, sb);
                for (it = prep.begin(); it != prep.end(); ++it) {
                    // get each LO frequency and row:
                    if (prep.get(it, freqLO, pRow)) {
                        // output a row of the parameters:
                        LOG(LM_INFO) << fixed << setw(prep.freqWidth_m) << setprecision(prep.freqPrecision_m)
                                     << freqLO << "," << pol << "," << sb << ","
                                     << setw(pRow.fWidth_m) << setprecision(4)
                                     << pRow[PreampParams::VD1] << ","
                                     << pRow[PreampParams::VD2] << ","
                                     << pRow[PreampParams::VD3] << ","
                                     << pRow[PreampParams::ID1] << ","
                                     << pRow[PreampParams::ID2] << ","
                                     << pRow[PreampParams::ID3] << ",0,0,0" << endl;
                    }
                }
            }
        }
    }

    // output the WCA configuration:
    const WCAConfig &wcaConfig = ca_m.getWCAConfig();

    // Output the LOParams records:
    LOG(LM_INFO) << context << ": got WCA LO params for band=" << band << endl;
    LOG(LM_INFO) << "FreqLO,VDP0,VDP1,VGP0,VGP1" << endl;

    // iterate over PowerAmpParams table:
    const PowerAmpParams &pap = wcaConfig.PAParams_m;
    for (it = pap.begin(); it != pap.end(); ++it) {
        // get each LO frequency and row:
        if (pap.get(it, freqLO, pRow)) {
            // output a row of the paramters:
            LOG(LM_INFO) << fixed << setw(pap.freqWidth_m) << setprecision(pap.freqPrecision_m)
                                  << freqLO << ","
                                  << setw(pRow.fWidth_m) << setprecision(pRow.fPrecision_m)
                                  << pRow[PowerAmpParams::VD0] << ","
                                  << pRow[PowerAmpParams::VD1] << ","
                                  << pRow[PowerAmpParams::VG0] << ","
                                  << pRow[PowerAmpParams::VG1] << endl;
        }
    }

    // Get the center LO frequency for the band:
    switch (band) {
        case 2:
            freqLO_m = 72.0;
            break;
        case 3:
            freqLO_m = 100.0;
            break;
        case 4:
            freqLO_m = 145.0;
            break;
        case 5:
            freqLO_m = 181.0;
            break;
        case 6:
            freqLO_m = 241.0;
            break;
        case 7:
            freqLO_m = 323.0;
            break;
        case 8:
            freqLO_m = 440.0;
            break;
        case 9:
            freqLO_m = 662.0;
            break;
        case 10:
            freqLO_m = 874.0;
            break;
        default:
            freqLO_m = 0.0;
            LOG(LM_ERROR) << context << ": no LO frequency defined for band=" << band << endl;
            break;
    }
    // Set to the center LO frequency:
    if (!ca_m.setLOFrequency(freqLO_m, 0.0, 0))
        LOG(LM_ERROR) << context << ": setLOFrequency failed for freqLO=" << freqLO_m << " band=" << band << endl;
    else
        LOG(LM_INFO) << context << ": set freqLO_m=" << freqLO_m << " for band=" << band << endl;

    // Set the null integrator to enabled:
    if (!ca_m.nullPLLIntegrator(true))
        LOG(LM_ERROR) << context << ": nullPLLIntegrator failed for band=" << band << endl;
    else
        LOG(LM_INFO) << context << ": enabled NULL integrator for band=" << band << endl;

    // Enable the photomixer:
    if (!ca_m.setEnablePhotomixer(true))
        LOG(LM_ERROR) << context << ": setEnablePhotomixer failed for band=" << band << endl;
    else
        LOG(LM_INFO) << context << ": enabled photomixer for band=" << band << endl;

    if (hasCCA) {
        // Set LNA bias:
        if (!ca_m.setLNABias())
            LOG(LM_ERROR) << context << ": setLNABias failed for band=" << band << endl;
        else
            LOG(LM_INFO) << context << ": set LNA bias for band=" << band << endl;

        // Enable LNAs:
        if (!ca_m.setEnableLNABias(true))
            LOG(LM_ERROR) << context << ": setEnableLNABias failed for band=" << band << endl;
        else
            LOG(LM_INFO) << context << ": enabled LNAs for band=" << band << endl;

        // Enable SIS:
        if (!ca_m.setSISBias(true))
            LOG(LM_ERROR) << context << ": setSISBias failed for band=" << band << endl;
        else
            LOG(LM_INFO) << context << ": enabled SISs for band=" << band << endl;

        // Enable magnets:
        if (band >= 5) {
            if (!ca_m.setSISMagnet(true))
                LOG(LM_ERROR) << context << ": setSISMagnet failed for band=" << band << endl;
            else
                LOG(LM_INFO) << context << ": enabled SIS magnets for band=" << band << endl;
        }
    }

    // Pause here to allow GUI to get monitor data from all above settings.   TODO:  Is this really necessary?
    SLEEP(2500);

    // If cold, enable LO PA:
    if (receiverIsCold_m && hasCCA) {
        if (!ca_m.setLOPowerAmps(true))
            LOG(LM_ERROR) << context << ": setLOPowerAmps failed for band=" << band << endl;
        else if (!ca_m.adjustLOPowerAmps())
            LOG(LM_ERROR) << context << ": adjustLOPowerAmps failed for band=" << band << endl;
        else {
            // Pause here to until PA power amp procedure finishes:
            int seconds = 0;
            bool done = false;
            while (!done) {
                SLEEP(1000);
                if (ca_m.adjustLOPowerAmpsIsBusy()) {
                    if (++seconds > 45) {
                        done = true;
                        seconds = 99;
                    }
                } else
                    done = true;
            }
            if (seconds == 99)
                LOG(LM_INFO) << context << ": timed out waiting for adjustLOPowerAmps for band=" << band << endl;
            else
                LOG(LM_INFO) << context << ": enabled and set LO power amps for band=" << band << endl;
        }

    // Otherwise, make sure they are off:
    } else {
        if (!ca_m.setLOPowerAmps(false))
            LOG(LM_ERROR) << context << ": setLOPowerAmps failed for band=" << band << endl;
        else
            LOG(LM_INFO) << context << ": disabled LO power amps for band=" << band << endl;
    }

    // Wait for the cartridge to get all warmed up:
    LOG(LM_INFO) << context << ": waiting " << warmUpTimeSeconds_m << " seconds for the cartridge to warm up..." << endl;
    SLEEP(1000 * warmUpTimeSeconds_m);

    freqLOret = freqLO_m;
    return true;
}

bool CartHealthCheck::start() {
    // Start the worker thread:
    return OptimizeBase::startWorkerThread();
}

void CartHealthCheck::optimizeAction() {
    static const string context("CartHealthCheck::optimizeAction");

    // test data header ID:
    FEICDataBase::ID_T headerId;

    // Get the configuration details for the CartAssembly:
    int band = ca_m.getBand();
    bool hasSb2 = ColdCartImpl::hasSb2(band);

    // record WCA monitor data:
    const WCAImpl *wca_p = ca_m.getWCA();

    // Get WCA AMC monitor data:
    WCAImpl::AMC_t amcData;
    if (!wca_p -> getMonitorAMC(amcData))
        LOG(LM_ERROR) << context << ": getMonitorAMC failed for band=" << band << endl;
    else {
        LOG(LM_INFO) << context << ": got LO AMC monitor data for band=" << band << endl;
        // log the data output.
        LOG(LM_INFO) << "Band,FreqLO,VDA,VDB,VDE,IDA,IDB,IDE,VGA,VGB,VGE,MultD,MultD_Current,5Vsupply" << endl;
        LOG(LM_INFO) << band << "," << fixed << setprecision(2) << freqLO_m << ","
                     << amcData.amcDrainAVoltage_value << ","
                     << amcData.amcDrainBVoltage_value << ","
                     << amcData.amcDrainEVoltage_value << ","
                     << amcData.amcDrainACurrent_value << ","
                     << amcData.amcDrainBCurrent_value << ","
                     << amcData.amcDrainECurrent_value << ","
                     << amcData.amcGateAVoltage_value << ","
                     << amcData.amcGateBVoltage_value << ","
                     << amcData.amcGateEVoltage_value << ","
                     << (int) amcData.amcMultiplierDVoltage_value << ","
                     << amcData.amcMultiplierDCurrent_value << ","
                     << amcData.amcSupplyVoltage5V_value << endl;

        if (!dbObject_m.insertAMCMonitorData(feConfig_m, dataStatus_m, band, freqLO_m, amcData))
            LOG(LM_ERROR) << context << ": database insertAMCMonitorData failed." << endl;
    }

    // Get WCA PA bias data:
    WCAImpl::PA_t paData;
    if (!wca_p -> getMonitorPA(paData))
        LOG(LM_ERROR) << context << ": getMonitorPA failed for band=" << band << endl;
    else {
        LOG(LM_INFO) << context << ": got LO PA monitor data for band=" << band << endl;
        // log the data output.
        LOG(LM_INFO) << "Band,FreqLO,VDp0,VDp1,IDp0,IDp1,VGp0,VGp1,3Vsupply,5Vsupply" << endl;
        LOG(LM_INFO) << band << "," << fixed << setprecision(2) << freqLO_m << ","
                     << paData.paPol0DrainVoltage_value << ","
                     << paData.paPol1DrainVoltage_value << ","
                     << paData.paPol0DrainCurrent_value << ","
                     << paData.paPol1DrainCurrent_value << ","
                     << paData.paPol0GateVoltage_value << ","
                     << paData.paPol1GateVoltage_value << ","
                     << paData.paSupplyVoltage3V_value << ","
                     << paData.paSupplyVoltage5V_value << endl;

        if (!dbObject_m.insertPAMonitorData(feConfig_m, dataStatus_m, band, freqLO_m, paData))
            LOG(LM_ERROR) << context << ": database insertPAMonitorData failed." << endl;
    }

    // create a test data header for the FLOOG distributor data:
    headerId.reset();
    if (!dbObject_m.findOrCreateFLOOGDistHealthDataHeader(headerId, feConfig_m, dataStatus_m))      // TODO:  needs component ID!
        LOG(LM_ERROR) << context << ": database createFLOOGDistHealthDataHeader failed." << endl;

    // Get WCA misc bias & FLOOG distributor health check data:
    WCAImpl::PLL_t pllData;
    if (!wca_p -> getMonitorPLL(pllData))
        LOG(LM_ERROR) << context << ": getMonitorPLL failed for band=" << band << endl;
    else {
        LOG(LM_INFO) << context << ": got LO PLL monitor data for band=" << band << endl;
        // log the data output.
        LOG(LM_INFO) << "Band,FreqLO,PLLtemp,YTO_heatercurrent" << endl;
        LOG(LM_INFO) << band << "," << fixed << setprecision(2) << freqLO_m << ","
                     << pllData.pllAssemblyTemp_value << ","
                     << pllData.pllYTOHeaterCurrent_value << endl;

        if (!dbObject_m.insertPLLMonitorData(feConfig_m, dataStatus_m, band, freqLO_m, pllData))
            LOG(LM_ERROR) << context << ": database insertPllMonitorData failed." << endl;

        // FLOOG dist health data.
        LOG(LM_INFO) << "Band,RefTotalPower" << endl;
        LOG(LM_INFO) << band << "," << fixed << setprecision(2) << pllData.pllRefTotalPower_value << endl;

        if (!dbObject_m.insertFLOOGDistHealthData(headerId, band, pllData.pllRefTotalPower_value))
            LOG(LM_ERROR) << context << ": database insertFLOOGDistHealthData failed." << endl;
    }

    // create a test data header for the photomixers data:
    headerId.reset();
    if (!dbObject_m.findOrCreatePhotomixerDataHeader(headerId, feConfig_m, dataStatus_m))
        LOG(LM_ERROR) << context << ": database createPhotomixerDataHeader failed." << endl;

    // photomixers health check
    WCAImpl::Photomixer_t pmxData;
    if (!wca_p -> getMonitorPhotomixer(pmxData))
        LOG(LM_ERROR) << context << ": getMonitorPhotomixer failed for band=" << band << endl;
    else {
        LOG(LM_INFO) << context << ": got photomixer monitor data for band=" << band << endl;
        // log the data output.
        LOG(LM_INFO) << "Band,Vpmx,Ipmx" << endl;
        LOG(LM_INFO) << band << "," << fixed << setprecision(2)
                     << pmxData.photomixerVoltage_value << ","
                     << pmxData.photomixerCurrent_value << endl;

        if (!dbObject_m.insertPhotomixerData(headerId, band, pmxData))
            LOG(LM_ERROR) << context << ": database insertPhotomixerData failed." << endl;
    }

    // record CCA monitor data if a CCA is configured:
    if (ca_m.existsColdCart()) {
        const ColdCartImpl *cca_p = ca_m.getColdCart();

        // Get cold cartridge temperatures:
        ColdCartImpl::CartridgeTemp_t tempData;
        if (!cca_p -> getMonitorTemp(tempData))
            LOG(LM_ERROR) << context << ": getMonitorTemp failed for band=" << band << endl;
        else {
            LOG(LM_INFO) << context << ": got cold cartridge temperatures for band=" << band << endl;
            // log the data output.
            LOG(LM_INFO) << "Band,4k,110k,Pol0_mixer,Spare,15k,Pol1_mixer" << endl;
            LOG(LM_INFO) << band << "," << fixed << setprecision(2)
                         << tempData.cartridgeTemperature0_value << ","
                         << tempData.cartridgeTemperature1_value << ","
                         << tempData.cartridgeTemperature2_value << ","
                         << tempData.cartridgeTemperature3_value << ","
                         << tempData.cartridgeTemperature4_value << ","
                         << tempData.cartridgeTemperature5_value << endl;

            if (!dbObject_m.insertCartTempData(feConfig_m, dataStatus_m, band, tempData))
                LOG(LM_ERROR) << context << ": database insertCartTempData failed." << endl;
        }

        // create a test data header for the LNA monitor data:
        headerId.reset();
        if (!dbObject_m.createLNAMonitorDataHeader(headerId, feConfig_m, dataStatus_m, band))
            LOG(LM_ERROR) << context << ": database createLNAMonitorDataHeader failed." << endl;

        // Get cold cartridge LNA monitor data:
        ColdCartImpl::LNA_t lnaData;
        LOG(LM_INFO) << context << ": LNA monitor data for band=" << band << "..." << endl;
        // log the data header.
        LOG(LM_INFO) << "Band,FreqLO,Pol,SB,Stage,VdRead,IdRead,VgRead" << endl;
        bool ok = false;
        bool dbError = false;   // to log whether any errors seen inside the loop.
        for (int pol = 0; pol <= 1; ++pol) {
            for (int sb = 1; sb <= 2; ++sb) {
                if (cca_p -> getMonitorLNA(pol, sb, lnaData)) {
                    ok = true;
                    // log the data output.
                    LOG(LM_INFO) << band << "," << fixed << setprecision(2) << freqLO_m << "," << pol << "," << sb << ","
                                 << "1," << lnaData.lnaSt1DrainVoltage_value << ","
                                         << lnaData.lnaSt1DrainCurrent_value << ","
                                         << lnaData.lnaSt1GateVoltage_value << endl;
                    LOG(LM_INFO) << band << "," << fixed << setprecision(2) << freqLO_m << "," << pol << "," << sb << ","
                                 << "2," << lnaData.lnaSt2DrainVoltage_value << ","
                                         << lnaData.lnaSt2DrainCurrent_value << ","
                                         << lnaData.lnaSt2GateVoltage_value << endl;
                    LOG(LM_INFO) << band << "," << fixed << setprecision(2) << freqLO_m << "," << pol << "," << sb << ","
                                 << "3," << lnaData.lnaSt3DrainVoltage_value << ","
                                         << lnaData.lnaSt3DrainCurrent_value << ","
                                         << lnaData.lnaSt3GateVoltage_value << endl;

                    if (!dbObject_m.insertLNAMonitorData(headerId, freqLO_m, pol, sb, lnaData))
                        dbError = true;
                }
            }
        }
        if (!ok)
            LOG(LM_ERROR) << context << ": getMonitorLNA failed for band=" << band << endl;
        if (dbError)
            LOG(LM_ERROR) << context << ": database insertLNAMonitorData failed." << endl;

        // create a test data header for the LNA monitor data:
        headerId.reset();
        if (!dbObject_m.createSISMonitorDataHeader(headerId, feConfig_m, dataStatus_m, band))
            LOG(LM_ERROR) << context << ": database createSISMonitorDataHeader failed." << endl;

        // Get cold cartridge SIS monitor data:
        ColdCartImpl::SIS_t sisData;
        LOG(LM_INFO) << context << ": SIS monitor data for band=" << band << "..." << endl;
        // log the data header.
        LOG(LM_INFO) << "Band,FreqLO,Pol,SB,VjRead,IjRead,VmagRead,ImagRead" << endl;
        ok = false;
        dbError = false;
        for (int pol = 0; pol <= 1; ++pol) {
            for (int sb = 1; sb <= 2; ++sb) {
                if (cca_p -> getMonitorSIS(pol, sb, sisData)) {
                    ok = true;
                    // log the data output.
                    LOG(LM_INFO) << band << "," << fixed << setprecision(3) << freqLO_m << "," << pol << "," << sb << ","
                                 << sisData.sisVoltage_value << ","
                                 << 1000 * sisData.sisCurrent_value << ","
                                 << sisData.sisMagnetVoltage_value << ","
                                 << sisData.sisMagnetCurrent_value << endl;

                    if (!dbObject_m.insertSISMonitorData(headerId, freqLO_m, pol, sb, sisData))
                        dbError = true;
                }
            }
        }
        if (!ok)
            LOG(LM_ERROR) << context << ": getMonitorSIS failed for band=" << band << endl;
        if (dbError)
            LOG(LM_ERROR) << context << ": database insertSISMonitorData failed." << endl;

        // if front end is cold, capture I-V curves:
        if (receiverIsCold_m) {

            const ColdCartImpl *cca_p = ca_m.getColdCart();

            string msg, legend;
            bool dbError = false;
            bool measError = false;
            bool fileError = false;
            unsigned pol, sb;

            // pause other monitoring of the CCA and WCA:
            ca_m.pauseMonitor(true, true, "Measure I-V curves.");

            // If a WCA is present, measure pumped I-V curves first:
            WCAImpl *wca_p = ca_m.useWCA();
            if (wca_p) {
                msg = "cartHealthCheck: measuring I-V curves: PUMPED...";
                FEMCEventQueue::addStatusMessage(true, msg);
                LOG(LM_INFO) << msg << endl;

                for (pol = 0; pol <= 1; ++pol) {
                    for (sb = 1; sb <= (hasSb2 ? 2 : 1); ++sb) {
                        if (!ca_m.measureIVCurveSingleSynchronous(pol, sb))
                            measError = true;
                        else {
                            SLEEP(3000);    // TODO:  this sleep is a hack to allow the GUI to grab the trace data.

                            // save the I-V curve text data file:
                            if (!cca_p -> saveIVCurveData(ca_m.getXYData(), logDir_m, pol, sb))
                                fileError = true;

                            // and insert it into the datbase:
                            legend = " pol" + to_string(pol) + " sis" + to_string(sb) + " PUMPED";
                            if (!dbObject_m.insertIVCurveData(feConfig_m, dataStatus_m, band, freqLO_m, pol, sb, legend, ca_m.getXYData()))
                                dbError = true;
                        }
                    }
                }
            }

            // measure unpumped curves with the LO disabled:
            if (wca_p) {
                wca_p -> setEnableLO(false);
                SLEEP(1000);    // pause to make sure it is off.  TODO:  ugly hack really necessary?
            }

            msg = "cartHealthCheck: measuring I-V curves: UN-PUMPED...";
            FEMCEventQueue::addStatusMessage(true, msg);
            LOG(LM_INFO) << msg << endl;

            for (pol = 0; pol <= 1; ++pol) {
                for (sb = 1; sb <= (hasSb2 ? 2 : 1); ++sb) {
                    if (!ca_m.measureIVCurveSingleSynchronous(pol, sb))
                        measError = true;
                    else {
                        SLEEP(3000);    // TODO:  this sleep is a hack to allow the GUI to grab the trace data.

                        // save the I-V curve text data file:
                        if (!cca_p -> saveIVCurveData(ca_m.getXYData(), logDir_m, pol, sb))
                            fileError = true;

                        // and insert it into the datbase:
                        legend = " pol" + to_string(pol) + " sis" + to_string(sb) + " UN-PUMPED";
                        if (!dbObject_m.insertIVCurveData(feConfig_m, dataStatus_m, band, freqLO_m, pol, sb, legend, ca_m.getXYData()))
                            dbError = true;
                    }
                }
            }

            // resume monitoring of the CCA and WCA:
            ca_m.pauseMonitor(false, false);

            // re-enable the LO PA:
            if (wca_p)
                wca_p -> setEnableLO(true);

            if (measError)
                LOG(LM_ERROR) << context << ": measureIVCurveSingleSynchronous failed for band=" << band << endl;
            if (fileError)
                LOG(LM_ERROR) << context << ": saveIVCurveData failed for band=" << band << endl;
            if (dbError)
                LOG(LM_ERROR) << context << ": database insertIVCurveData failed." << endl;
        }
    }
    setFinished(true);
}


void CartHealthCheck::exitAction(bool success) {
    if (success) {
        // Send the measurement finished event.  Optional param indicates whether a CCA is present:
        setEvent(FEMCEventQueue::EVENT_CARTHC_DONE, ca_m.getBand(), -1, (ca_m.existsColdCart() ? 1 : 0), 100);
        string msg("CartHealthCheck: finished successfully.");
        setStatusMessage(true, msg);
        LOG(LM_INFO) << msg << endl;
    } else {
        string msg("CartHealthCheck: failed.");
        setStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
    }
}

