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
 * implementation of the top-level FrontEnd class, 
 * giving the entire control interface for the Front End.
 * 
 *----------------------------------------------------------------------
 */

#include "FrontEndImpl.h"
#include "logger.h"
#include "stringConvert.h"
#include "FEMCErrors.h"
#include "FEMCEventQueue.h"
#include "CartridgesContainer.h"
#include "PowerModulesContainer.h"
#include "LockingStrategy.h"
#include "DLL/SWVersion.h"
#include "CONFIG/FrontEndDataBase.h"
#include "CONFIG/IFPowerDataSet.h"
#include "OPTIMIZE/XYPlotArray.h"
#include <stdio.h>
#include <limits>
#include <cmath>
using namespace FEConfig;
using namespace std;

bool FrontEndImpl::correctSISVoltageError_m(true);

//---------------------------------------------------------------------------
// private constructor and destructor definitions:

FrontEndImpl::FrontEndImpl(unsigned long channel, 
                           unsigned long nodeAddress)
  : FrontEndImplBase("FrontEnd"),
    SN_m(0),
    carts_mp(new CartridgesContainer()),
    powerMods_mp(new PowerModulesContainer()),
    dbObject_mp(new FrontEndDatabase()),
    cryostat_mp(NULL),
    fetim_mp(NULL),
    ifSwitch_mp(NULL),
    lpr_mp(NULL),
    cpds_m(false),
    connected_m(false),
    thermalLogger_mp(new ThermalLogger(*this)),
    thermaLogInterval_m(30),
    hcStarted_m(false),
    hcReceiverIsCold_m(false),
    hcFacility_m(40),
    hcDataStatus_m(0),
    logMonTimers_m(false),
    monTimers_m(7, std::numeric_limits<unsigned short int>::min()),
    monTimersMin_m(7, std::numeric_limits<unsigned int>::max()),
    monTimersMax_m(7, unsigned(0)),
    monTimersCount_m(0),
    maxTimerValue_m(0)
{ 
    initialize(channel, nodeAddress);
}

FrontEndImpl::~FrontEndImpl() {
    delete thermalLogger_mp;
    delete carts_mp;
    delete powerMods_mp;
    delete ifSwitch_mp;
    delete fetim_mp;
    delete cryostat_mp;
    delete lpr_mp;
    delete dbObject_mp;
}

void FrontEndImpl::initialize(unsigned long channel, unsigned long nodeAddress) {
    FrontEndImplBase::initialize(channel, nodeAddress);
    int retries = 1 ;
    while (retries && !connected_m) {
        unsigned char stat = specialGetSetupInfo();
        if (stat == 0 || stat == 5) {
            connected_m = true;


            LOG(LM_INFO) << "Connected to front end on CAN" << channel
                         << " at nodeAddress=0x" << uppercase << hex << setw(2) << setfill('0') << nodeAddress << dec << setw(0)
                         << " firmware: ARCOM:" << FEMCFirmwareVersion() << " AMBSI1:" << AMBSIFirmwareRev()
                         << " serialNum=" << AMBSISerialNum() << endl;
        } else {
            retries--;
            string msg("Waiting to connect to front end. Retries=");
            msg += to_string(retries);
            FEMCEventQueue::addStatusMessage(false, msg);
            SLEEP(1000);
        }
    }
    if (!connected_m)
        LOG(LM_ERROR) << "Not connected to front end." << endl;
}

void FrontEndImpl::shutdown() {
    connected_m = false;
    FrontEndImplBase::shutdown();
}

bool FrontEndImpl::getNextError(unsigned char &moduleNum, unsigned char &errorNum, std::string &description) {
    union {
        unsigned short shortVal;
        struct {
            unsigned int err : 8;
            unsigned int mod : 8;
        } bytes;
    } result;

    moduleNum = 0xFF;
    errorNum = 0xFF;
    description.clear();

    unsigned short num = FrontEndImplBase::getNumErrors();
    if (num == 0)
        return false;

    result.shortVal = FrontEndImplBase::getNextError();
    moduleNum = result.bytes.mod;
    errorNum = result.bytes.err;

    // Don't report Cryostat serial timeout errors:
    if (moduleNum == 0x2B && errorNum == 0x08)
        return true;

    FEMCGetErrorDescription(moduleNum, errorNum, description);
    LOG(LM_INFO) << "FEMC(" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (short) moduleNum
                 << ":" << std::setw(2) << std::setfill('0') << (short) errorNum << ") " << description << std::endl;
    return true;
}

void FrontEndImpl::setFEMode(unsigned char val) {
    if (val > 3) {
        LOG(LM_ERROR) << "FrontEndImpl::setFEMode: invalid mode requested, using 0 instead." << endl;
        val = 0;
    }
    LOG(LM_INFO) << "FrontEndImpl::setFEMode: setting mode=" << (int) val << endl;
    FrontEndImplBase::setFEMode(val);
}

void FrontEndImpl::startMonitor() {
    FrontEndImplBase::startMonitor();
    if (cpds_m)
        powerMods_mp -> startMonitor();
    if (fetim_mp)
        fetim_mp -> startMonitor();
    if (ifSwitch_mp)
        ifSwitch_mp -> startMonitor();
    if (cryostat_mp)
        cryostat_mp -> startMonitor();
    if (lpr_mp)
        lpr_mp -> startMonitor();
    if (thermalLogger_mp)
        thermalLogger_mp -> start(thermaLogInterval_m);
}

void FrontEndImpl::stopMonitor() {
    if (thermalLogger_mp)
        thermalLogger_mp -> stop();
    FrontEndImplBase::stopMonitor();
    carts_mp -> stopMonitor();
    powerMods_mp -> stopMonitor();
    if (ifSwitch_mp)
        ifSwitch_mp -> stopMonitor();
    if (fetim_mp)
        fetim_mp -> stopMonitor();
    if (cryostat_mp)
        cryostat_mp -> stopMonitor();
    if (lpr_mp)
        lpr_mp -> stopMonitor();
}

bool FrontEndImpl::getDbConfigId(FEICDataBase::ID_T &configId) const {
    static const string context("FrontEndImpl::getFrontEndAndConfigId");

    // get the front end configuration ID to which we will attach all the monitor data:
    configId = dbObject_mp -> getConfigId(hcFacility_m, SN_m);

    LOG(LM_INFO) << context << ": getConfigId for SN=" << SN_m << " returned feConfig=" << configId << endl;

    return configId.valid();
}


bool FrontEndImpl::startHealthCheck(short _dataStatus) {
    static const string context("FrontEndImpl::startHealthCheck");

    // status of the data we are about to insert into the database:
    FEICDataBase::DATASTATUS_TYPES dataStatus = (_dataStatus == 0)
        ? FEICDataBase::DS_HEALTH_CHECK
        : static_cast<FEICDataBase::DATASTATUS_TYPES>(_dataStatus);

    // store the dataStatus for later use in cartHealthCheck:
    hcDataStatus_m = static_cast<int>(dataStatus);

    LOG(LM_INFO) << context << ": dataStatus=" << dataStatus << endl;

    // look up the front end ID and ESN for this front end:
    string ESN;
    FEICDataBase::ID_T frontEndId = dbObject_mp -> getFrontEndId(hcFacility_m, SN_m);

    if (dbObject_mp -> getFrontEndRecord(frontEndId, NULL, &ESN)) {

        LOG(LM_INFO) << context << ": getFrontEndRecord for SN=" << SN_m << " ESN='" << ESN << "' frontEndId=" << frontEndId << endl;

        // if the record has a blank ESN, update it with the actual AMBSI serial number:
        if (ESN.empty()) {
            ESN = FrontEndImplBase::AMBSISerialNum();
            dbObject_mp -> updateFrontEndESN(frontEndId, ESN);

            LOG(LM_INFO) << context << ": updated front end record with ESN='" << ESN << "'." << endl;
        }
    }

    // get the front end configuration to attribute test data to:
    FEICDataBase::ID_T feConfig;
    getDbConfigId(feConfig);

    // enable IF switch temperature servo so we can collect the temperatures:
    if (ifSwitch_mp) {
        if (!ifSwitchSetEnableTempServo(-1, -1, true))
            LOG(LM_ERROR) << context << ": ifSwitchSetEnableTempServo failed." << endl;
        else
            LOG(LM_INFO) << context << ": enabled IF switch temp servo." << endl;
    }

    // set LPR EDFA modulation so that we can collect the photomixer responses in the main cartrige loop:
    if (lpr_mp) {
        float EDFANom = 1.90;
        if (!lprSetEDFAModulationInput(EDFANom))
            LOG(LM_ERROR) << context << ": lprSetEDFAModulationInput failed." << endl;
        else
            LOG(LM_INFO) << context << ": set LPR EDFA modulation to " << EDFANom << " V." << endl;
    }

    // pause to allow monitor data to be collected:
    int waitTime = 10;  // seconds;  TODO: is this long enough to complete a monitor cycle for IF switch, LPR, and cryostat data?
    LOG(LM_INFO) << context << ": waiting " << waitTime << " seconds for IF switch, LPR, and cryostat monitor data..." << endl;
    SLEEP(1000 * waitTime);

    // get cryostat temperatures and determine if the receiver is cold:
    hcReceiverIsCold_m = false;  // Assume warm unless we can discover otherwise.

    if (cryostat_mp) {
        CryostatImpl::Cryostat_t cryoData;
        if (!cryostatGetMonitorModule(cryoData))
            LOG(LM_ERROR) << context << ": cryostatGetMonitorModule failed." << endl;
        else {
            float switchTemperature = 30.0;
            float maxTemperature = 350.0;
            // check the 4K & 15K stage temps.  If they are out of range, health check cannot proceed:
            if ((cryoData.cryostatTemperature0_value <= 0 || cryoData.cryostatTemperature0_value > maxTemperature) &&
                (cryoData.cryostatTemperature5_value <= 0 || cryoData.cryostatTemperature5_value > maxTemperature))
            {
                LOG(LM_ERROR) << context << ": Cryostat 4K and 15K temperatures are both out of range.  Health check cannot continue!" << endl;
                return false;
            }
            // check the 4K & 15K stage temps.  If either are below the threshold, consider it cold:
            if (cryoData.cryostatTemperature0_value < switchTemperature || cryoData.cryostatTemperature5_value < switchTemperature) {
                LOG(LM_INFO) << context << ": cryostat is cold." << endl;
                hcReceiverIsCold_m = true;
            } else {
                LOG(LM_INFO) << context << ": cryostat is warm." << endl;
                hcReceiverIsCold_m = false;
            }
            // log the data output.
            LOG(LM_INFO) << "4k_CryoCooler,4k_PlateLink1,4k_PlateLink2,4k_PlateFarSide1,4k_PlateFarSide2,15k_CryoCooler,15k_PlateLink,15k_PlateFarSide,15k_Shield,110k_CryoCooler,110k_PlateLink,110k_PlateFarSide,110k_Shield" << endl;
            LOG(LM_INFO) << fixed << setprecision(2)
                         << cryoData.cryostatTemperature0_value << ","
                         << cryoData.cryostatTemperature1_value << ","
                         << cryoData.cryostatTemperature2_value << ","
                         << cryoData.cryostatTemperature3_value << ","
                         << cryoData.cryostatTemperature4_value << ","
                         << cryoData.cryostatTemperature5_value << ","
                         << cryoData.cryostatTemperature6_value << ","
                         << cryoData.cryostatTemperature7_value << ","
                         << cryoData.cryostatTemperature8_value << ","
                         << cryoData.cryostatTemperature9_value << ","
                         << cryoData.cryostatTemperature10_value << ","
                         << cryoData.cryostatTemperature11_value << ","
                         << cryoData.cryostatTemperature12_value << endl;

            // save data to database:
            if (!dbObject_mp -> insertCryostatData(feConfig, dataStatus, cryoData))
                LOG(LM_ERROR) << context << ": database insertCryostatData failed." << endl;
        }
    }

    // Get IF switch temperatures:
    if (ifSwitch_mp) {
        IFSwitchImpl::IFSwitch_t ifswData;
        if (!ifSwitchGetMonitorModule(ifswData))
            LOG(LM_ERROR) << context << ": ifSwitchGetMonitorModule failed." << endl;
        else {
            LOG(LM_INFO) << context << ": got IF switch monitor data." << endl;
            // log the data output.
            LOG(LM_INFO) << "pol0sb1,pol0sb2,pol1sb1,pol1sb2" << endl;
            LOG(LM_INFO) << fixed << setprecision(2)
                         << ifswData.pol0Sb1AssemblyTemp_value << ","
                         << ifswData.pol0Sb2AssemblyTemp_value << ","
                         << ifswData.pol1Sb1AssemblyTemp_value << ","
                         << ifswData.pol1Sb2AssemblyTemp_value << endl;

            if (!dbObject_mp -> insertIfSwitchData(feConfig, dataStatus, ifswData))
                LOG(LM_ERROR) << context << ": database insertIfSwitchData failed." << endl;
        }
    }

    // Get LPR monitor data:
    if (lpr_mp) {
        LPRImpl::LPR_t lprData;
        if (!lprGetMonitorModule(lprData))
            LOG(LM_ERROR) << context << ": lprGetMonitorModule failed." << endl;
        else {
            LOG(LM_INFO) << context << ": got LPR monitor data." << endl;
            // log the data output.
            LOG(LM_INFO) << "LaserPumpTemp,LaserDrive,LaserPhotodetector,Photodetector_mA,Photodetector_mW,ModInput,Temperature0,Tempearature1" << endl;
            LOG(LM_INFO) << fixed << setprecision(2)
                         << lprData.EDFALaserPumpTemperature_value << ","
                         << lprData.EDFALaserDriveCurrent_value << ","
                         << lprData.EDFALaserPhotoDetectCurrent_value << ","
                         << lprData.EDFAPhotoDetectCurrent_value << ","
                         << lprData.EDFAPhotoDetectPower_value << ","
                         << lprData.EDFAModulationInput_value << ","
                         << lprData.LPRTemperature0_value << ","
                         << lprData.LPRTemperature1_value << endl;

            if (!dbObject_mp -> insertLPRMonitorData(feConfig, dataStatus, lprData))
                LOG(LM_ERROR) << context << ": database insertLPRMonitorData failed." << endl;
        }
    }
    // Set flag to indicate that health check has started:
    hcStarted_m = true;
    return true;
}

bool FrontEndImpl::cartHealthCheck(int port, int warmUpTimeSeconds, bool includeIFPower) {
    static const string context("FrontEndImpl::startHealthCheck");

    // Check that we are in health check mode:
    if (!hcStarted_m) {
        string msg("cartHealthCheck: front end must be put in healthCheck mode first.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << context << ": " << msg << endl;
        return false;
    }

    // Get the CartAssembly for the specified band.
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (!ca) {
        string msg("cartHealthCheck: no cartridge at port=");
        msg += to_string(port);
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << context << ": " << msg << endl;
        return false;
    }

    // Check that the CartAssembly includes a WCA.  Can't health check a CCA by itself:
    if (!ca -> existsWCA()) {
        string msg("cartHealthCheck: no WCA at port=");
        msg += to_string(port);
        msg += ".  Health check is not supported for a CCA alone.";
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << context << ": " << msg << endl;
        return false;
    }

    // Check that the cartridge is on:
    if (!getCartridgeOn(port)) {
        string msg("cartHealthCheck: cartridge is not powered on for port=");
        msg += to_string(port);
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << context << ": " << msg << endl;
        return false;
    }

    // Set the cartridge to observing:
    if (!setCartridgeObserving(port))
        LOG(LM_ERROR) << context << ": setCartridgeObserving failed for port=" << port << endl;
    else
        LOG(LM_INFO) << context << ": set cartridge observing at port=" << port << endl;

    // get the front end configuration ID and data status to which we will attach all the monitor data:
    FEICDataBase::ID_T feConfig = dbObject_mp -> getConfigId(hcFacility_m, SN_m);
    FEICDataBase::DATASTATUS_TYPES dataStatus = static_cast<FEICDataBase::DATASTATUS_TYPES>(hcDataStatus_m);

    // set up the cartridge for health check:
    double freqLO;
    if (!ca -> prepareHealthCheck(*dbObject_mp, feConfig, dataStatus, freqLO, hcReceiverIsCold_m, warmUpTimeSeconds, includeIFPower)) {
        LOG(LM_ERROR) << context << ": prepareHealthCheck failed." << endl;
        return false;
    }

    // Get CPDS monitor data:
    if (cpds_m) {
        PowerModuleImpl::PowerModule_t cpdsData;
        if (!powerGetMonitorModule(port, cpdsData))
            LOG(LM_ERROR) << context << ": powerGetMonitorModule failed for port=" << port << " band=" << ca -> getBand() << endl;
        else {
            LOG(LM_INFO) << context << ": got CPDS monitor data for port=" << port << " band=" << ca -> getBand() << endl;
            // log the data output.
            LOG(LM_INFO) << "Band,FreqLO,P6V_V,N6V_V,P15V_V,N15V_V,P24V_V,P8V_V,P6V_I,N6V_I,P15V_I,N15V_I,P24V_I,P8V_I" << endl;
            LOG(LM_INFO) << ca -> getBand() << "," << fixed << setprecision(3) << freqLO << ","
                         << cpdsData.voltageP6V_value << ","
                         << cpdsData.voltageN6V_value << ","
                         << cpdsData.voltageP15V_value << ","
                         << cpdsData.voltageN15V_value << ","
                         << cpdsData.voltageP24V_value << ","
                         << cpdsData.voltageP8V_value << ","
                         << cpdsData.currentP6V_value << ","
                         << cpdsData.currentN6V_value << ","
                         << cpdsData.currentP15V_value << ","
                         << cpdsData.currentN15V_value << ","
                         << cpdsData.currentP24V_value << ","
                         << cpdsData.currentP8V_value << endl;

            FEICDataBase::ID_T headerId;
            if (!dbObject_mp -> findOrCreatePowerModuleDataHeader(headerId, feConfig, dataStatus))
                LOG(LM_ERROR) << context << ": database findOrCreatePowerModuleDataHeader failed." << endl;
            else if (!dbObject_mp -> insertPowerModuleData(headerId, port, freqLO, cpdsData))
                LOG(LM_ERROR) << context << ": database insertPowerModuleData failed." << endl;
        }
    }

    // complete the health check procedure on the worker thread:
    return ca -> startHealthCheck();
}

bool FrontEndImpl::cartHealthCheckSaveIFPowerData(int port, const IFPowerDataSet &data) {
    static const string context("FrontEndImpl::cartHealthCheckSaveIFPowerData");

    // Check that we are in health check mode:
    if (!hcStarted_m) {
        string msg(context + ": front end must be put in healthCheck mode first.");
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }

    // Get the CartAssembly for the specified band.
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (!ca) {
        string msg(context + ": no cartridge at port=");
        msg += to_string(port);
        FEMCEventQueue::addStatusMessage(false, msg);
        LOG(LM_ERROR) << msg << endl;
        return false;
    }

    // get the front end configuration ID and data status to which we will attach the IF power and Y-factor data:
    FEICDataBase::ID_T configId = dbObject_mp -> getConfigId(hcFacility_m, SN_m);
    FEICDataBase::DATASTATUS_TYPES dataStatus = static_cast<FEICDataBase::DATASTATUS_TYPES>(hcDataStatus_m);

    // get the current LO frequency:
    double freqLO, freqREF;
    int band = ca -> getBand();
    ca -> getLOFrequency(freqLO, freqREF);

    // log the IF total power data.
    LOG(LM_INFO) << context << ": got IF Power data for port=" << port << " band=" << band << endl;
    LOG(LM_INFO) << "Band,FreqLO,IFChannel,Power_0dB_gain,Power_15dB_gain" << endl;
    LOG(LM_INFO) << band << "," << fixed << setprecision(3) << freqLO << ", 0, "
                 << data[IFPowerDataSet::PHOT0_IF0] << ", " << data[IFPowerDataSet::PHOT15_IF0] << endl;
    LOG(LM_INFO) << band << "," << fixed << setprecision(3) << freqLO << ", 1, "
                 << data[IFPowerDataSet::PHOT0_IF1] << ", " << data[IFPowerDataSet::PHOT15_IF1] << endl;

    if (ColdCartImpl::hasSb2(band)) {
        LOG(LM_INFO) << band << "," << fixed << setprecision(3) << freqLO << ", 2, "
                     << data[IFPowerDataSet::PHOT0_IF2] << ", " << data[IFPowerDataSet::PHOT15_IF2] << endl;
        LOG(LM_INFO) << band << "," << fixed << setprecision(3) << freqLO << ", 3, "
                     << data[IFPowerDataSet::PHOT0_IF3] << ", " << data[IFPowerDataSet::PHOT15_IF3] << endl;
    }

    // Decide whether to save Y-factor data as well...
    bool doYFactor = false;
    if (dataStatus == FEICDataBase::DS_COLD_PAS)
        doYFactor = true;   // alwyas save it in cold PAS mode.
    else if (dataStatus == FEICDataBase::DS_HEALTH_CHECK) {
        // in health check mode, check for any channel's valid data to save:
        if ((data[IFPowerDataSet::YFACTOR_IF0] != 0.0 && data[IFPowerDataSet::YFACTOR_IF0] != 1.0) ||
            (data[IFPowerDataSet::YFACTOR_IF1] != 0.0 && data[IFPowerDataSet::YFACTOR_IF1] != 1.0) ||
            (data[IFPowerDataSet::YFACTOR_IF2] != 0.0 && data[IFPowerDataSet::YFACTOR_IF2] != 1.0) ||
            (data[IFPowerDataSet::YFACTOR_IF3] != 0.0 && data[IFPowerDataSet::YFACTOR_IF3] != 1.0))
            doYFactor = true;
    }

    if (doYFactor) {
        // log the Y-Factor data.
        float Phot0 = (dataStatus == FEICDataBase::DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF0] : data[IFPowerDataSet::PHOT15_IF0];
        float Phot1 = (dataStatus == FEICDataBase::DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF1] : data[IFPowerDataSet::PHOT15_IF1];
        float Phot2 = (dataStatus == FEICDataBase::DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF2] : data[IFPowerDataSet::PHOT15_IF2];
        float Phot3 = (dataStatus == FEICDataBase::DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF3] : data[IFPowerDataSet::PHOT15_IF3];

        LOG(LM_INFO) << context << ": got Y-Factor data for port=" << port << " band=" << band << endl;
        LOG(LM_INFO) << "FreqLO,IFChannel,Phot_dBm,Pcold_dBm,Y" << endl;
        LOG(LM_INFO) << fixed << setprecision(3) << freqLO << ", 0, "
                     << Phot0 << ", " << data[IFPowerDataSet::PCOLD_IF0] << ", " << data[IFPowerDataSet::YFACTOR_IF0] << endl;
        LOG(LM_INFO) << fixed << setprecision(3) << freqLO << ", 1, "
                     << Phot1 << ", " << data[IFPowerDataSet::PCOLD_IF1] << ", " << data[IFPowerDataSet::YFACTOR_IF1] << endl;

        if (ColdCartImpl::hasSb2(band)) {
            LOG(LM_INFO) << fixed << setprecision(3) << freqLO << ", 2, "
                         << Phot2 << ", " << data[IFPowerDataSet::PCOLD_IF2] << ", " << data[IFPowerDataSet::YFACTOR_IF2] << endl;
            LOG(LM_INFO) << fixed << setprecision(3) << freqLO << ", 3, "
                         << Phot3 << ", " << data[IFPowerDataSet::PCOLD_IF3] << ", " << data[IFPowerDataSet::YFACTOR_IF3] << endl;
        }
    }
    if (!dbObject_mp -> insertIFTotalPowerData(configId, dataStatus, band, freqLO, "IF Total Power", data))
        LOG(LM_ERROR) << context << ": database insertIFTotalPowerData failed." << endl;
    if (doYFactor) {
        if (!dbObject_mp -> insertYFactorData(configId, dataStatus, band, freqLO, "Y-Factor", data))
            LOG(LM_ERROR) << context << ": database insertYFactorData failed." << endl;
    }
    return true;
}

bool FrontEndImpl::finishHealthCheck() {
    static const string context("FrontEndImpl::finishHealthCheck");

    if (lpr_mp) {
        // set LPR EDFA back to 0:
        if (!lprSetEDFAModulationInput(0.0))
            LOG(LM_ERROR) << context << ": lprSetEDFAModulationInput failed." << endl;
        else
            LOG(LM_INFO) << context << ": set LPR EDFA modulation to 0 V." << endl;
    }
    hcStarted_m = hcReceiverIsCold_m = false;
    hcFacility_m = hcDataStatus_m = 0;
    return true;
}

bool FrontEndImpl::existsCartAssembly(int port) {
    return carts_mp -> existsCartAssembly(port);
}

void FrontEndImpl::queryCartridgeState() {
    int observingBand = 0;

    if (ifSwitch_mp) {
        observingBand = ifSwitch_mp -> switchCartridge() + 1;
        if (observingBand < 0 || observingBand > 10)
            observingBand = 0;
        // TODO: when switchCartridge times out we get bogus data here.  Need to see the error somehow.
    }
    
    for (int port = 1; port <= 10; ++port) {
        if (cpds_m && powerMods_mp -> queryEnable(port))
            setCartridgeOn(port);
        else if (port == observingBand)
            observingBand = 0;
    } 
    
    LOG(LM_INFO) << "FrontEndImpl::queryCartridgeState observingBand=" << observingBand << endl;
    if (observingBand > 0)
        setCartridgeObserving(observingBand);
    carts_mp -> queryCartridgeState();
    FEMCEventQueue::addEvent(FEMCEventQueue::Event(FEMCEventQueue::EVENT_INITIALIZE));
}

void FrontEndImpl::measureSISVoltageError(int port) {
    if (port < 1 || port > 10)
        return;
    carts_mp -> measureSISVoltageError(port);
}

void FrontEndImpl::setFrontEndSN(const std::string &SN) {
    SN_m = numericPortion<unsigned>(SN);
}

const std::string &FrontEndImpl::getFrontEndSN() const {
    static string SN;
    SN = to_string(SN_m);
    return SN;
}

unsigned char FrontEndImpl::specialGetSetupInfo() {
    unsigned char ret = FrontEndImplBase::specialGetSetupInfo();
    AMBSIFirmwareRev();
    AMBSILibraryVersion();
    FEMCFirmwareVersion();
    FEMCFPGAVersion();
    AMBSISerialNum();
    return ret;
}    

void FrontEndImpl::getAMBSIInfo(std::string &serialNum,
                                std::string &firmwareRev,
                                unsigned long &numErrors, 
                                unsigned long &numTransactions)
{
    serialNum = AMBSISerialNum_value;
    firmwareRev = AMBSIFirmwareRev_value;
    numErrors = AMBSINumErrors_value;
    numTransactions = AMBSINumTransactions_value;   
}

void FrontEndImpl::getFirmwareInfo(std::string &_AMBSILibraryVersion,
                                   std::string &_FEMCFirmwareVersion,
                                   std::string &_FEMCFPGAVersion) {
    _AMBSILibraryVersion = AMBSILibraryVersion_value;
    _FEMCFirmwareVersion = FEMCFirmwareVersion_value;
    _FEMCFPGAVersion = FEMCFPGAVersion_value;
}

float FrontEndImpl::getAMBSITemperature() const {
    return AMBSITemperature_value;
}

unsigned short FrontEndImpl::powerGetMonitorNumEnabled() {
    return numEnabledModules();
}

bool FrontEndImpl::cartGetMonitorYTO(int port, WCAImpl::YTO_t &target) const {
    // If the requested CartAssembly exists and is enabled, return the YTO monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable())
        return ca -> getMonitorYTO(target);
    else
        return false;
}

bool FrontEndImpl::cartGetMonitorPhotomixer(int port, WCAImpl::Photomixer_t &target) const {
    // If the requested CartAssembly and WCA exist and are enabled, return the PMX monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const WCAImpl *wca = ca -> getWCA();
        if (wca)
            return wca -> getMonitorPhotomixer(target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorPLL(int port, WCAImpl::PLL_t &target) const {
    // If the requested CartAssembly and WCA exist and are enabled, return the PLL monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const WCAImpl *wca = ca -> getWCA();
        if (wca)
            return wca -> getMonitorPLL(target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorAMC(int port, WCAImpl::AMC_t &target) const {
    // If the requested CartAssembly and WCA exist and are enabled, return the AMC monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const WCAImpl *wca = ca -> getWCA();
        if (wca)
            return wca -> getMonitorAMC(target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorPA(int port, WCAImpl::PA_t &target) const {
    // If the requested CartAssembly and WCA exist and are enabled, return the PA monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const WCAImpl *wca = ca -> getWCA();
        if (wca)
            return wca -> getMonitorPA(target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorTemp(int port, ColdCartImpl::CartridgeTemp_t &target) const {
    // If the requested CartAssembly and ColdCart exist and are enabled, return the tempurature monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const ColdCartImpl *cc = ca -> getColdCart();
        if (cc)
            return cc -> getMonitorTemp(target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorSIS(int port, int pol, int sb, ColdCartImpl::SIS_t &target) const {
    // If the requested CartAssembly and ColdCart exist and are enabled, return the SIS monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const ColdCartImpl *cc = ca -> getColdCart();
        if (cc)
            return cc -> getMonitorSIS(pol, sb, target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorLNA(int port, int pol, int sb, ColdCartImpl::LNA_t &target) const {
    // If the requested CartAssembly and ColdCart exist and are enabled, return the LNA monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const ColdCartImpl *cc = ca -> getColdCart();
        if (cc)
            return cc -> getMonitorLNA(pol, sb, target);
    }
    return false;
}

bool FrontEndImpl::cartGetMonitorAux(int port, int pol, ColdCartImpl::Aux_t &target) const {
    // If the requested CartAssembly and ColdCart exist and are enabled, return the Aux monitor data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const ColdCartImpl *cc = ca -> getColdCart();
        if (cc)
            return cc -> getMonitorAux(pol, target);
    }
    return false;
}

bool FrontEndImpl::cartGetLastHeaterCurrents(int port, int pol, ColdCartImpl::HeaterCurrents_t &target) const {
    // If the requested CartAssembly and ColdCart exist and are enabled, return the recent heater currents data:
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const ColdCartImpl *cc = ca -> getColdCart();
        if (cc)
            return cc -> getLastHeaterCurrents(pol, target);
    }
    return false;
}

//----------------------------------------------------------------------------
// Functions to add and remove sub-assemblies:
bool FrontEndImpl::addCartridge(int port, WCAImpl &WCA, ColdCartImpl &coldCart) {
    return carts_mp -> addCartridge(port, WCA, coldCart);
}

bool FrontEndImpl::addCartridge(int port, WCAImpl &WCA) {
    return carts_mp -> addCartridge(port, WCA);
}

bool FrontEndImpl::addCartridge(int port, ColdCartImpl &coldCart) {
    return carts_mp -> addCartridge(port, coldCart);
}

bool FrontEndImpl::deleteCartridge(int port) {
    return carts_mp -> deleteCartridge(port);   
}

bool FrontEndImpl::addPowerModule(PowerModuleImpl &module) {
    return powerMods_mp -> addPowerModule(module);
}

bool FrontEndImpl::deletePowerModule(int port) {
    return powerMods_mp -> deletePowerModule(port);
}

void FrontEndImpl::addCryostat() {
    deleteCryostat();
    cryostat_mp = new CryostatImpl(m_channel, m_nodeAddress);
}

void FrontEndImpl::deleteCryostat() {
    if (cryostat_mp) {
        cryostat_mp -> stopMonitor();
        delete cryostat_mp;
        cryostat_mp = NULL;
    }
}

void FrontEndImpl::addFETIM() {
    deleteFETIM();
    fetim_mp = new FETIMImpl(m_channel, m_nodeAddress);
}

void FrontEndImpl::deleteFETIM() {
    if (fetim_mp) {
        fetim_mp -> stopMonitor();
        delete fetim_mp;
        fetim_mp = NULL;
    }
}

void FrontEndImpl::addIFSwitch() {
    deleteIFSwitch();
    ifSwitch_mp = new IFSwitchImpl(m_channel, m_nodeAddress);
}

void FrontEndImpl::deleteIFSwitch() {
    if (ifSwitch_mp) {
        ifSwitch_mp -> stopMonitor();
        delete ifSwitch_mp;
        ifSwitch_mp = NULL;
    }
}

void FrontEndImpl::addLPR() {
    deleteLPR();
    lpr_mp = new LPRImpl(m_channel, m_nodeAddress);
    LockingStrategy::setLPR(lpr_mp);
}

void FrontEndImpl::deleteLPR() {
    if (lpr_mp) {
        lpr_mp -> stopMonitor();
        delete lpr_mp;
        lpr_mp = NULL;
    }
}

void FrontEndImpl::addCPDS() {
    cpds_m = true;
}

void FrontEndImpl::deleteCPDS() {
    cpds_m = false;
}


//----------------------------------------------------------------------------
// Functions to create and configure cartridges:

bool FrontEndImpl::setColdCartConfig(int port, const ColdCartConfig &params) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> existsColdCart()) {
        ca -> setColdCartConfig(params);
        return true;
    } else
        return false;
}
    
bool FrontEndImpl::setWCAConfig(int port, const WCAConfig &params) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> existsWCA()) {
        ca -> setWCAConfig(params);
        return true;
    } else
        return false;
}

bool FrontEndImpl::setYIGLimits(int port, double FLOYIG, double FHIYIG) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> existsWCA()) {
        ca -> setYIGLimits(FLOYIG, FHIYIG);
        return true;
    } else
        return false;
}

//----------------------------------------------------------------------------
// Functions to manage the power and enabled/standby/observing status of the cartridges:    

bool FrontEndImpl::setCartridgeOff(int port) {
    LOG(LM_INFO) << "FrontEndImpl::setCartridgeOff port=" << port << endl;

    // set the CartAssembly to powered off state:
    carts_mp -> clearEnable(port);

    // If no CPDS, send the power command anyway so that FEMC is in proper state:
    if (!cpds_m)
        powerEnableModule(port, 0);

    // tell CPDS to turn off the cartridge:
    else
        powerMods_mp -> clearEnable(port);

    string msg("Cartridge band ");
    msg += to_string(port);
    msg += " is now powered OFF.";
    FEMCEventQueue::addStatusMessage(true, msg);
    return true;
}

bool FrontEndImpl::setCartridgeOn(int port) {
    LOG(LM_INFO) << "FrontEndImpl::setCartridgeOn port=" << port << endl;
    string msg("Cartridge band ");
    msg += to_string(port);

    // If no CPDS, send the power command anyway so that FEMC is in proper state:
    if (!cpds_m)
        powerEnableModule(port, 1);

    // tell CPDS to turn on the cartridge:
    else if (!powerMods_mp -> setEnable(port)) {
        msg += " NOT POWERED ON!";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    // set the CartAssembly to powered on state:
    carts_mp -> setEnable(port);
    msg += " is now powered ON.";
    FEMCEventQueue::addStatusMessage(true, msg);
    // Measure the SIS voltage setting error if configured to do so:
    if (correctSISVoltageError_m)
        measureSISVoltageError(port);
    return true;
}
    
bool FrontEndImpl::setCartridgeStandby2(int port, bool enable) {
    if (port != 6) {
        string msg("STANDBY2 mode not supported for cartridge band ");
        msg += to_string(port);
        FEMCEventQueue::addStatusMessage(true, msg);
        LOG(LM_ERROR) << msg;
        return false;

    }
    LOG(LM_INFO) << "FrontEndImpl::setCartridgeStandby2(" << enable << ") for port=" << port << endl;
    string msg("Cartridge band ");
    msg += to_string(port);

    // If no CPDS, send the power command anyway so that FEMC is in proper state:
    unsigned char cmd(carts_mp -> getEnable(port) ? 1 : 0);
    if (enable)
        cmd = 2;
    if (!cpds_m)
        powerEnableModule(port, cmd);

    // tell CPDS to turn on the cartridge:
    else if (!powerMods_mp -> setStandby2(port, enable)) {
        msg += " STANDBY2 command failed!";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }

    // set the CartAssembly to powered on state:
    carts_mp -> setEnable(port);
    msg += (enable ? " entered" : " exited");
    msg += " STANDBY2 mode.";
    FEMCEventQueue::addStatusMessage(true, msg);
    return true;
}

bool FrontEndImpl::setCartridgeObserving(int port) {
    if (port < 1 || port > 10) {
        LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " out of range." << endl;
        return false;
    }
        
    // check for no change:
    if (carts_mp -> getObserving() == port) {
        LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " is already observing." << endl;
        return true;
    }

    LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << endl;

    string msg("The observing band is ");
    msg += to_string(port);
    msg += ".";
    FEMCEventQueue::addStatusMessage(true, msg);
    
    unsigned long startTime = GETTIMER();
        
    // clear the existing observing port:
    clearCartridgeObserving();
    
    // Switch the LPR optical switch to the new observing port:
    if (!lprSetOpticalSwitchPort(port)) {
        LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " lprSetOpticalSwitchPort failed." << endl;
        FEMCEventQueue::addStatusMessage(false, "LPR: setting the observing port failed.");
        return false;
    }    
    // Switch the IF output to the new observing port:
    if (ifSwitch_mp) {
        
        
        // BEWARE HACK to work around bad IF switch channel:
        if (!ifSwitch_mp -> setObservingBand(port == 2 ? 6 : port)) {
        // HACK


            LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " IF switch setObservingBand failed." << endl;
            FEMCEventQueue::addStatusMessage(false, "IF Switch: setting the observing port failed.");
            return false;
        }
    }
    // Check whether the cartridge is enabled:
    if (!carts_mp -> getEnable(port)) {
        LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " cartridge is not enabled.  LPR and IF switch were changed." << endl;
        FEMCEventQueue::addStatusMessage(false, "The cartridge selected for observing port is not enabled.");
        return true;
    }
    // set the new observing port:
    if (!carts_mp -> setObserving(port)) {
        LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " setObserving failed.  LPR and IF switch were changed." << endl;
        FEMCEventQueue::addStatusMessage(false, "Set observing port failed.");
        return false;
    }
    
    unsigned long stopTime = GETTIMER();
    LOG(LM_INFO) << "FrontEndImpl::setCartridgeObserving port=" << port << " took " << (stopTime - startTime) << " ms." << endl;
    return true;
}

void FrontEndImpl::clearCartridgeObserving() {
    LOG(LM_INFO) << "FrontEndImpl::clearCartridgeObserving" << endl;
    carts_mp -> clearObserving();
}

int FrontEndImpl::getCartridgeObserving() const {
    return carts_mp -> getObserving();
}

bool FrontEndImpl::getCartridgeOn(int port) const {
    return carts_mp -> getEnable(port);
}

bool FrontEndImpl::cartPauseMonitor(int port, bool pauseWCA, bool pauseCC) {
    return carts_mp -> pauseMonitor(port, pauseWCA, pauseCC);
}

//----------------------------------------------------------------------------
// Control commands for CartAssemblies:

bool FrontEndImpl::cartSetLockingStrategy(int port, int strategy) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetLockingStrategy port=" << port << " strategy= " << strategy << endl;
        ca -> setLockingStrategy(static_cast<WCAConfig::LOCK_STRATEGY_OPTS>(strategy));
        return true;
    }
    return false;
}

bool FrontEndImpl::cartSetLOFrequency(int port, double freqLO, double freqFLOOG, int sbLock) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetLOFrequency port=" << port << " freqLO= " << freqLO 
            << " freqFLOOG=" << freqFLOOG << " sbLock=" << sbLock << endl;
        if (ca -> setLOFrequency(freqLO, freqFLOOG, sbLock)) {

            // if the LNA or SIS are enabled, load the configuration for the new frequency:
            ColdCartImpl *cca = ca -> useColdCart();
            if (cca) {
                if (cca -> getLNAEnableSetting())
                    ca -> setLNABias();
                if (cca -> getSISEnableSetting())
                    ca -> setSISBias(true);
            }
            // if the LO PA is enabled, load the configuration for the new frequency:
            WCAImpl *wca = ca -> useWCA();
            if (wca && wca -> getPAEnableSetting())
                ca -> setLOPowerAmps(true);

            return true;
        }
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartSetLOFrequency port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("Setting the LO frequency failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
    }
    return false;
}

bool FrontEndImpl::cartSetCenterLOFrequency(int port, double freqFLOOG, int sbLock) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        double freqLO = CartAssembly::getCenterLOFrequency(ca -> getBand());
        return cartSetLOFrequency(port, freqLO, freqFLOOG, sbLock);
    }
    return false;
}

bool FrontEndImpl::cartGetLOFrequency(int port, double &freqLO, double &freqREF) const {    
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA())
        return ca -> getLOFrequency(freqLO, freqREF);
    else {
        freqLO = freqREF = 0;
        return false;
    }
}

bool FrontEndImpl::cartLockPLL(int port) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartLockPLL port=" << port << endl;
        bool ret = ca -> lockPLL();
        if (!ret) {
            string msg("ERROR: band ");
            msg += to_string(port);
            msg += " PLL is NOT locked.";
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << msg << endl;
        } else {
            string msg("Cartridge band ");
            msg += to_string(port);
            msg += " PLL is locked.";
            FEMCEventQueue::addStatusMessage(true, msg);
            LOG(LM_INFO) << msg << endl;
        }
        return ret; 
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartLockPLL port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("Locking the PLL failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool FrontEndImpl::cartGetLocked(int port) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        WCAImpl *wca = ca -> useWCA();
        return wca -> getLocked();
    }
    return false;
}

bool FrontEndImpl::cartAdjustPLL(int port, float targetCorrVoltage) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartAdjustPLL port=" << port << " targetCV=" << targetCorrVoltage << endl;
        bool ret = ca -> adjustPLL(targetCorrVoltage);
        if (ret) {
            string msg("Cartridge band ");
            msg += to_string(port);
            msg += " PLL correction voltage adjusted to ";
            msg += to_string(targetCorrVoltage);
            msg += ".";
            FEMCEventQueue::addStatusMessage(true, msg);
            LOG(LM_INFO) << msg << endl;
        }
        return ret; 
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartAdjustPLL port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("Adjusting the PLL failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool FrontEndImpl::cartAdjustYTO(int port, int steps) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartAdjustYTO port=" << port << " steps=" << steps << endl;
        return ca -> adjustYTO(steps);
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartAdjustYTO port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("Adjusting the YTO failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool FrontEndImpl::cartNullPLLIntegrator(int port, bool enable) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartNullPLLIntegrator port=" << port << " enable=" << enable << endl;
        return ca -> nullPLLIntegrator(enable);
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartNullPLLIntegrator port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("The Zero Integrator setting failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool FrontEndImpl::cartGetNullPLLIntegrator(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {        
        return ca -> getNullPLLIntegrator();
    } else
        return false;
}

bool FrontEndImpl::cartSetLNABias(int port, int pol, int sb,
                                  const float *VD1, 
                                  const float *ID1,
                                  const float *VD2, 
                                  const float *ID2,
                                  const float *VD3, 
                                  const float *ID3)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetLNABias port=" << port << " pol=" << pol << " sb=" << sb
            << " VD1=" << (VD1 ? *VD1 : 0) << " ID1=" << (ID1 ? *ID1 : 0)
            << " VD2=" << (VD2 ? *VD2 : 0) << " ID2=" << (ID2 ? *ID2 : 0)
            << " VD3=" << (VD3 ? *VD3 : 0) << " ID3=" << (ID3 ? *ID3 : 0) << endl;
        return ca -> setLNABias(pol, sb, VD1, ID1, VD2, ID2, VD3, ID3);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetLNABias", "Setting the LNA bias failed");
        return false;
    }
}

bool FrontEndImpl::cartSetEnableLNABias(int port, bool enable, int pol, int sb) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetEnableLNABias port=" << port << " enable=" << enable << " pol=" << pol << " sb=" << sb << endl; 
        return ca -> setEnableLNABias(enable, pol, sb);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetEnableLNABias", "Enabling the LNA bias failed");
        return false;
    }
}

bool FrontEndImpl::cartGetEnableLNABias(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {        
        return ca -> getEnableLNABias();
    } else
        return false;
}

bool FrontEndImpl::cartSetSISBias(int port, bool enable, int pol, int sb,
                                  const float *VJ, int openLoop)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetSISBias port=" << port << " enable=" << enable << " pol=" << pol << " sb=" << sb
            << " VJ=" << (VJ ? *VJ : 0) << " openLoop=" << openLoop << endl;
        return ca -> setSISBias(enable, pol, sb, VJ, openLoop);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetSISBias", "Setting the SIS bias failed");
        return false;
    }
}

bool FrontEndImpl::cartGetEnableSISBias(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {        
        return ca -> getEnableSISBias();
    } else
        return false;
}

bool FrontEndImpl::cartSetSISMagnet(int port, bool enable, int pol, int sb, const float *IMag) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetSISMagnet port=" << port << " enable=" << enable << " pol=" << pol << " sb=" << sb
            << " Imag=" << (IMag ? *IMag : 0) << endl;
        return ca -> setSISMagnet(enable, pol, sb, IMag);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetSISMagnet", "Setting the SIS magnet bias failed");
        return false;
    }
}

bool FrontEndImpl::cartGetEnableSISMagnet(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {        
        return ca -> getEnableSISMagnet();
    } else
        return false;
}

bool FrontEndImpl::cartGetSISMagnet(short port, int pol, int sb, float *IMag) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart())
        return ca -> getSISMagnet(pol, sb, IMag);
    else {
        *IMag = 0;
        return false;
    }
}

bool FrontEndImpl::cartSetLOPowerAmps(int port, bool enable,
                                      const float *VDP0, 
                                      const float *VGP0,
                                      const float *VDP1, 
                                      const float *VGP1)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetLOPowerAmps port=" << port << " enable=" << enable
            << " VDP0=" << (VDP0 ? *VDP0 : 0) << " VGP0=" << (VGP0 ? *VGP0 : 0)
            << " VDP1=" << (VDP1 ? *VDP1 : 0) << " VGP1=" << (VGP1 ? *VGP1 : 0) << endl;
        return ca -> setLOPowerAmps(enable, VDP0, VGP0, VDP1, VGP1);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetLOPowerAmps", "Setting the LO power amplifier bias failed");
        return false;
    }
}

bool FrontEndImpl::cartGetLOPowerAmpsSetting(int port, bool &enable,
                                             float *VDP0,
                                             float *VGP0,
                                             float *VDP1,
                                             float *VGP1)
{
    const CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        const WCAImpl *wca = ca -> getWCA();
        if (wca) {
            enable = wca -> getPAEnableSetting();
            if (VDP0)
                *VDP0 = wca -> getPADrainVoltageSetting(0);
            if (VGP0)
                *VGP0 = wca -> getPAGateVoltageSetting(0);
            if (VDP1)
                *VDP1 = wca -> getPADrainVoltageSetting(1);
            if (VGP1)
                *VGP1 = wca -> getPAGateVoltageSetting(1);
            return true;
        }
    }
    LOG(LM_INFO) << "FrontEndImpl::cartGetLOPowerAmpsSetting port=" << port << " WCA does not exist or is disabled." << endl;
    return false;
}

bool FrontEndImpl::cartGetEnableLOPowerAmps(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {        
        return ca -> getLOPowerAmpsEnable();
    } else
        return false;
}

bool FrontEndImpl::cartAdjustLOPowerAmps(int port, int repeatCount) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartAdjustLOPowerAmps port=" << port << endl;
        return ca -> adjustLOPowerAmps(repeatCount);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartAdjustLOPowerAmps", "Adjusting the LO power amplifier bias failed");
        return false;
    }
}

bool FrontEndImpl::cartSetEnableLO(int port, bool enable) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        WCAImpl *wca = ca -> useWCA();
        if (wca) {
            LOG(LM_INFO) << "FrontEndImpl::cartSetEnableLO port=" << port << " enable=" << enable << endl;
            wca -> setEnableLO(enable);
            return true;
        }
    }
    LOG(LM_ERROR) << "FrontEndImpl::cartSetEnableLO port=" << port << " WCA does not exist or is disabled." << endl;
    return false;
}

bool FrontEndImpl::cartSetLOPower(int port, int pol, float percent) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        WCAImpl *wca = ca -> useWCA();
        if (wca) {
            LOG(LM_INFO) << "FrontEndImpl::cartSetLOPower port=" << port << " pol=" << pol << " percent=" << percent << endl;
            wca -> setLOPower(pol, percent);
            return true;
        }
    }
    LOG(LM_INFO) << "FrontEndImpl::cartSetLOPower port=" << port << " WCA does not exist or is disabled." << endl;
    return false;
}

bool FrontEndImpl::cartOptimizeIFPower(int port, bool doPol0, bool doPol1, float VDstart0, float VDstart1) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartOptimizeIFPower port=" << port << " doPol0=" << doPol0 << " doPol1=" << doPol1
                     << " VDstart0=" << VDstart0 << " VDstart1=" << VDstart1 << endl;
        return ca -> optimizeIFPower(doPol0, doPol1, VDstart0, VDstart1);
    }
    reportBadCartridge(port, "FrontEndImpl::cartOptimizeIFPower", "IF power optimization failed");
    return false;
}

bool FrontEndImpl::cartClearOptimizedResult(short port) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca) {
        return ca -> clearOptimizedResult();
    }
    reportBadCartridge(port, "FrontEndImpl::cartClearOptimizedResult");
    return false;
}

bool FrontEndImpl::cartGetOptimizedResult(int port, std::string &mixerParamsText) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca) {
        return ca -> getOptimizedResult(mixerParamsText);
    }
    reportBadCartridge(port, "FrontEndImpl::cartGetOptimizedResult");
    return false;
}

bool FrontEndImpl::cartSetIFPower(int port, int pol, float powerSB1, float powerSB2) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        LOG(LM_DEBUG) << "FrontEndImpl::cartSetIFPower port=" << port << " pol=" << pol << " powerSB1=" << setprecision(4) << scientific << powerSB1 << " powerSB2=" << powerSB2 << fixed << endl;
        return ca -> setIFPower(pol, powerSB1, powerSB2);
    } else {
        return false;
    }
}

bool FrontEndImpl::cartSetVJVD(int port, int pol, float *VJ1, float *VJ2, float *VD)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        bool ret = ca -> setVJVD(pol, VJ1, VJ2, VD); 
        LOG(LM_INFO) << "FrontEndImpl::cartSetVJVD port=" << port << " pol=" << pol
            << " VJ1=" << (VJ1 ? *VJ1 : 0) << " VJ2=" << (VJ2 ? *VJ2 : 0) << " VD=" << (VD ? *VD : 0) << endl;
        return ret;
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetVJVD", "Setting the SIS and PA bias failed");
        return false;
    }
}

bool FrontEndImpl::cartGetVJVD(int port, int pol, float *VJ1, float *VJ2, float *VD) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        bool ret = ca -> getVJVD(pol, VJ1, VJ2, VD); 
        return ret;
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartGetVJVD", "Getting the SIS and PA bias failed");
        return false;
    }
}

bool FrontEndImpl::cartSetAMC(int port, const float *VDE, const float *VGE) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetAMC port=" << port  << " VDE=" << (VDE ? *VDE : 0) << " VGE=" << (VGE ? *VGE : 0) << endl;
        return ca -> setAMC(VDE, VGE);
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartSetAMC port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("Setting the AMC bias failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool FrontEndImpl::cartMeasureFineLOSweep(int port, float tiltAngle, int pol,
                                          const float *VJ, const float *IJ, const float *fixedVD,
                                          const double *LOStart, const double *LOStop, const double *LOStep,
                                          int repeatCount)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartMeasureFineLOSweep port=" << port
            << " tiltAngle=" << tiltAngle
            << " pol=" << pol
            << " repeatCount=" << repeatCount << endl;

        // get the front end configuration to attribute test data to:
        FEICDataBase::ID_T configId;
        getDbConfigId(configId);

        FEICDataBase::DATASTATUS_TYPES dataStatus = FEICDataBase::DS_COLD_PAS;
        return ca -> measureFineLOSweep(*dbObject_mp, configId, dataStatus, tiltAngle, pol, VJ, IJ, fixedVD, LOStart, LOStop, LOStep, repeatCount);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartMeasureFineLOSweep", "Measuring fine LO sweep failed");
        return false;
    }
}

bool FrontEndImpl::cartMeasureIVCurve(int port, int pol, int sb, 
                                      const float *VJlow_p, 
                                      const float *VJhigh_p, 
                                      const float *VJstep_p,
                                      int repeatCount)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartMeasureIVCurve port=" << port << " pol=" << pol << " sb=" << sb
            << " VJlow=" << (VJlow_p ? *VJlow_p : 0) << " VJhigh=" << (VJhigh_p ? *VJhigh_p : 0) 
            << "  VJstep=" << (VJstep_p ? *VJstep_p : 0) << " repeatCount=" << repeatCount << endl;
        return ca -> measureIVCurve(pol, sb, VJlow_p, VJhigh_p, VJstep_p, repeatCount);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartMeasureIVCurve", "Measuring the IV curve failed");
        return false;
    }
}

bool FrontEndImpl::cartMeasureIJvsSISMagnet(int port, int pol, int sb, 
                                            float IMagStart, float IMagStop, float IMagStep,
                                            float VJLow, float VJHigh, float VJStep,
                                            int repeatCount) 
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartMeasureIJvsSISMagnet port=" << port << " pol=" << pol << " sb=" << sb
            << " IMagStart=" << IMagStart << " IMagStop=" << IMagStop << " IMagStep=" << IMagStep 
            << " VJLow=" << VJLow << " VJHigh=" << VJHigh << " VJStep=" << VJStep
            << " repeatCount=" << repeatCount << endl;
        return ca -> measureIJvsSISMagnet(pol, sb, IMagStart, IMagStop, IMagStep, VJLow, VJHigh, VJStep, repeatCount);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartMeasureIJvsSISMagnet", "Measuring the SIS bias vs. magnet current failed");
        return false;
    }
}

bool FrontEndImpl::cartMeasureIFPowerVsVJ(int port, int pol, int sb, 
                                          float VJStart, float VJStop, float VJStep, 
                                          bool doYFactor, int repeatCount)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {

        double freqLO, freqREF;
        ca -> getLOFrequency(freqLO, freqREF);
        std::string description = "LO" + to_string(freqLO);


        LOG(LM_INFO) << "FrontEndImpl::cartMeasureIFPowerVsVJ port=" << port << " pol=" << pol << " sb=" << sb
            << " '" << description << "' "
            << " VJStart=" << VJStart << " VJStop=" << VJStop << " VJStep=" << VJStep
            << " doYFactor=" << doYFactor << " repeatCount=" << repeatCount << endl;
        return ca -> measureIFPowerVsVJ(pol, sb, description, VJStart, VJStop, VJStep, doYFactor, repeatCount);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartMeasureIFPowerVsVJ", "Measuring the IF power vs. SIS bias failed");
        return false;
    }
}

bool FrontEndImpl::cartMeasureIFPowerVsVD(int port, int pol, 
                                          float VDStart, float VDStop, float VDStep, 
                                          bool doYFactor, int repeatCount)
{
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {

        double freqLO, freqREF;
        ca -> getLOFrequency(freqLO, freqREF);
        std::string description = "LO" + to_string(freqLO);

        LOG(LM_INFO) << "FrontEndImpl::cartMeasureIFPowerVsVD port=" << port << " pol=" << pol
            << " '" << description << "' "
            << " VDStart=" << VDStart << " VDStop=" << VDStop << " VDStep=" << VDStep
            << " doYFactor=" << doYFactor << " repeatCount=" << repeatCount << endl;
        return ca -> measureIFPowerVsVD(pol, description, VDStart, VDStop, VDStep, doYFactor, repeatCount);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartMeasureIFPowerVsVD", "Measuring the IF power vs. PA bias failed");
        return false;
    }
}

bool FrontEndImpl::cartMixerDeflux(int port, int pol, int sb, int IMagMax) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartMixerDeflux port=" << port << " pol=" << pol << " sb=" << sb
            << " IMagMax=" << IMagMax << endl;
        return ca -> mixerDeflux(pol, sb, IMagMax);
    } else {
        reportBadCartridge(port, "FrontEndImpl::mixerDeflux", "SIS mixer defluxing failed");
        return false;
    }
}

void FrontEndImpl::cartAbortMeasurement(int port) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable()) {
        LOG(LM_INFO) << "FrontEndImpl::cartAbortMeasurement port=" << port << endl;
        ca -> abortMeasurement();
    }
}

const XYPlotArray &FrontEndImpl::cartGetXYData(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    const XYPlotArray &data = ca -> getXYData();
    LOG(LM_INFO) << "FrontEndImpl::cartGetXYData port=" << port << " len=" << data.size() << endl;
    return data;
}

bool FrontEndImpl::cartSetEnableLNALEDs(int port, bool enable) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetEnableLNALEDs port=" << port << " enable=" << enable << endl;
        return ca -> setEnableLNALEDs(enable);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetEnableLNALEDs", "Enabling the LNA LEDs failed");
        return false;
    }
}

bool FrontEndImpl::cartSetEnableSISHeaters(int port, bool enable, int pol, float targetTemp, int timeout ) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsColdCart()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetEnableSISHeaters port=" << port << " enable=" << enable << endl;
        return ca -> setEnableSISHeaters(enable, pol, targetTemp, timeout);
    } else {
        reportBadCartridge(port, "FrontEndImpl::cartSetEnableSISHeaters", "Enabling the SIS heaters failed");
        return false;
    }
}

bool FrontEndImpl::cartSetEnablePhotomixer(int port, bool enable) {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {
        LOG(LM_INFO) << "FrontEndImpl::cartSetEnablePhotomixer port=" << port << " enable=" << enable << endl;
        return ca -> setEnablePhotomixer(enable);
    } else {
        LOG(LM_INFO) << "FrontEndImpl::cartSetEnablePhotomixer port=" << port << " WCA does not exist or is disabled." << endl;
        string msg("Enabling the photomixer failed for band ");
        msg += to_string(port);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool FrontEndImpl::cartGetEnablePhotomixer(int port) const {
    CartAssembly *ca = carts_mp -> getCartAssembly(port);
    if (ca && ca -> getEnable() && ca -> existsWCA()) {        
        return ca -> getEnablePhotomixer();
    } else
        return false;
}

//----------------------------------------------------------------------------
// Power distribution channel monitor and control points:

bool FrontEndImpl::powerGetMonitorModule(int port, PowerModuleImpl::PowerModule_t &target) const {
    if (cpds_m)
        return powerMods_mp -> getMonitorPowerModule(port, target);
    else
        return false;
}

bool FrontEndImpl::powerSetEnableModule(int port, bool enable) {
    bool ret = false;
    if (cpds_m) {
        if (enable)
            ret = powerMods_mp -> setEnable(port);
        else        
            ret = powerMods_mp -> clearEnable(port);
    }
    return ret;
}

//----------------------------------------------------------------------------
// IF switch monitor and control points:

bool FrontEndImpl::ifSwitchGetMonitorModule(IFSwitchImpl::IFSwitch_t &target) const {
    if (ifSwitch_mp)
        return ifSwitch_mp -> getMonitorIFSwitch(target);
    else
        return false;
}

bool FrontEndImpl::ifSwitchSetAttenuation(int pol, int sb, unsigned char atten) {
    if (!ifSwitch_mp)
        return false;
    if (atten > 15)
        atten = 15;
    bool setAll = (pol < 0 || sb < 1);    
    if (setAll || (pol == 0 && sb == 1))
        ifSwitch_mp -> pol0Sb1Attenuation(atten);
    if (setAll || (pol == 0 && sb == 2))
        ifSwitch_mp -> pol0Sb2Attenuation(atten);
    if (setAll || (pol == 1 && sb == 1))
        ifSwitch_mp -> pol1Sb1Attenuation(atten);
    if (setAll || (pol == 1 && sb == 2))
        ifSwitch_mp -> pol1Sb2Attenuation(atten);
    return true;        
}        

bool FrontEndImpl::ifSwitchSetEnableTempServo(int pol, int sb, bool enable) {
    if (!ifSwitch_mp)
        return false;
    bool setAll = (pol < 0 || sb < 1);
    if (setAll || (pol == 0 && sb == 1))
        ifSwitch_mp -> pol0Sb1TempServoEnable(enable);
    if (setAll || (pol == 0 && sb == 2))
        ifSwitch_mp -> pol0Sb2TempServoEnable(enable);
    if (setAll || (pol == 1 && sb == 1))
        ifSwitch_mp -> pol1Sb1TempServoEnable(enable);
    if (setAll || (pol == 1 && sb == 2))
        ifSwitch_mp -> pol1Sb2TempServoEnable(enable);
    return true;        
}        

//----------------------------------------------------------------------------
// Cryostat monitor and control points:

bool FrontEndImpl::cryostatGetMonitorModule(CryostatImpl::Cryostat_t &target) const {
    if (cryostat_mp)
        return cryostat_mp -> getMonitorCryostat(target);
    else
        return false;
}

bool FrontEndImpl::cryostatSetEnableBackingPump(bool enable) {
    if (cryostat_mp) {
        cryostat_mp -> backingPumpEnable(enable);
        return true;
    }
    return false;
}

bool FrontEndImpl::cryostatSetEnableTurboPump(bool enable) {
    if (cryostat_mp) {
        cryostat_mp -> turboPumpEnable(enable);
        return true;
    }
    return false;
}

bool FrontEndImpl::cryostatSetGateValveState(bool open) {
    if (cryostat_mp) {
        cryostat_mp -> gateValveState(open ? 1 : 0);
        return true;
    }
    return false;
}

bool FrontEndImpl::cryostatSetSolenoidValveState(bool open) {
    if (cryostat_mp) {
        cryostat_mp -> solenoidValveState(open ? 1 : 0);
        return true;
    }
    return false;
}

bool FrontEndImpl::cryostatSetEnableVacuumGauge(bool enable) {
    if (cryostat_mp) {
        cryostat_mp -> vacuumGaugeEnable(enable);
        return true;
    }
    return false;
}

bool FrontEndImpl::cryostatSetEnableCryoPumping(bool enable) {
    if (cryostat_mp) {
        return cryostat_mp -> cryoPumpingEnable(enable);
    }
    return false;
}

bool FrontEndImpl::cryostatStartStopPlot(bool enable) {
    if (cryostat_mp) {
        if (enable) {
            // get the front end configuration ID and data status to which we will attach the monitor data:
            FEICDataBase::ID_T feConfig = dbObject_mp -> getConfigId(hcFacility_m, SN_m);
            FEICDataBase::DATASTATUS_TYPES dataStatus = FEICDataBase::DS_COLD_PAI;
            cryostat_mp -> startCooldownPlot(*dbObject_mp, feConfig, dataStatus);
            return true;
        } else {
            cryostat_mp -> stopCooldownPlot();
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
// FETIM monitor and control points:

bool FrontEndImpl::fetimGetMonitorModule(FETIMImpl::FETIM_t &target) const {
    if (fetim_mp)
        return fetim_mp -> getMonitorFETIM(target);
    else
        return false;
}

bool FrontEndImpl::fetimSetTriggerDewarN2Fill(bool enable) {
    if (fetim_mp) {
        fetim_mp -> setTriggerDewarN2Fill(enable);
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
// LO photonic receiver monitor and control points:

bool FrontEndImpl::lprGetMonitorModule(LPRImpl::LPR_t &target) const {
    if (lpr_mp)
        return lpr_mp -> getMonitorLPR(target);
    else
        return false;
}

bool FrontEndImpl::lprSetOpticalSwitchPort(int port) {
    if (!lpr_mp)
        return true;    // support operating without an LPR.
    if (port >= 1 && port <= 10)
        return lpr_mp -> setObservingBand(port);
    return false;
}

bool FrontEndImpl::lprSetOpticalSwitchShutter() {
    if (lpr_mp) {
        lpr_mp -> opticalSwitchShutter((unsigned char) 0);
        return true;
    }
    return false;
}

bool FrontEndImpl::lprSetOpticalSwitchForceShutter() {
    if (lpr_mp) {
        lpr_mp -> opticalSwitchForceShutter((unsigned char) 0);
        return true;
    }
    return false;
}

bool FrontEndImpl::lprSetEDFAModulationInput(float value) {
    if (lpr_mp) {
        lpr_mp -> EDFAModulationInput(value);
        return true;
    }
    return false;
}

void FrontEndImpl::setThermalLogInterval(unsigned int interval) {
    thermaLogInterval_m = interval;
    LOG(LM_INFO) << "FrontEndImpl: setThermalLogInterval=" << thermaLogInterval_m << endl;
    if (thermalLogger_mp)
        thermalLogger_mp -> setLoggingInterval(thermaLogInterval_m);
}

void FrontEndImpl::appendThermalLogHeader(std::string &target) const {
    target += "\tAMBSI";
    if (cryostat_mp)
        cryostat_mp -> appendThermalLogHeader(target);
    if (fetim_mp)
        fetim_mp -> appendThermalLogHeader(target);
    if (ifSwitch_mp)
        ifSwitch_mp -> appendThermalLogHeader(target);
    if (lpr_mp)
        lpr_mp -> appendThermalLogHeader(target);
    if (carts_mp)
        carts_mp -> appendThermalLogHeader(target);
}

void FrontEndImpl::appendThermalLog(std::string &target) const {
    char buf[20];    
    sprintf(buf, "\t%.2f", getAMBSITemperature());
    target += buf;
    if (cryostat_mp)
        cryostat_mp -> appendThermalLog(target);
    if (fetim_mp)
        fetim_mp -> appendThermalLog(target);
    if (ifSwitch_mp)
        ifSwitch_mp -> appendThermalLog(target);
    if (lpr_mp)
        lpr_mp -> appendThermalLog(target);
    if (carts_mp)
        carts_mp -> appendThermalLog(target);
}

void FrontEndImpl::postMonitorHook(const AmbRelativeAddr &RCA) {
    if (!logMonTimers_m)
        return;

    if (AMBSIFirmwareRev_value == "1.0.0" || AMBSIFirmwareRev_value == "1.0.1")
        return;

    getMonitorTimers(monTimers_m[0], monTimers_m[1], monTimers_m[2], monTimers_m[3],
                     monTimers_m[4], monTimers_m[5], monTimers_m[6], maxTimerValue_m);

    monTimersCount_m++;

    for (int i = 0; i < 7; i++) {
        unsigned counts = maxTimerValue_m - monTimers_m[i];
        if (counts < monTimersMin_m[i])
            monTimersMin_m[i] = counts;
        if (counts < maxTimerValue_m && counts > monTimersMax_m[i])
            monTimersMax_m[i] = counts;
    }

    if (monTimersCount_m % 10000 == 0) {
        for (int i = 0; i < 7; i++)
            LOG(LM_INFO) << "MonTimers[" << i << "] min=" << monTimersMin_m[i] << " max=" << monTimersMax_m[i] << endl;
    }
}

void FrontEndImpl::reportBadCartridge(int port, std::string where, std::string msg) {
    LOG(LM_INFO) << where << " port=" << port << " cartridge does not exist or is disabled." << endl;
    if (!msg.empty()) {
        msg += " for band ";
        msg += to_string(port);
        msg += ". The cartridge does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
    }
}
