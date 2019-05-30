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

/************************************************************************
 * Implementation of the DLL exports for LabVIEW.
 * 
 *----------------------------------------------------------------------
 */

#include "lv_wrapper_FE.h"
#include "lv_wrapper_Compressor.h"
#include "lv_wrapper_sigSrc.h"
#include "lv_wrapper.h"
#include "lv_structs.h"
#include "FEMCEventQueue.h"
#include "FrontEndImpl.h"
#include "SignalSourceImpl.h"
#include "logger.h"
#include "CONFIG/ConfigProviderMySQL.h"
#include "CONFIG/ConfigProviderIniFile.h"
#include "CONFIG/ConfigManager.h"
#include "CONFIG/IFPowerDataSet.h"
#include "iniFile.h"
#include "stringConvert.h"
#include "splitPath.h"
#include "FEBASE/FEHardwareDevice.h"
#include "OPTIMIZE/XYPlotArray.h"
#include "ColdCartImpl.h"
#include "CryostatImpl.h"
#include "FETIMImpl.h"
#include "IFSwitchImpl.h"
#include "PowerModuleImpl.h"
#include "WCAImpl.h"
#include <math.h>

using namespace FEConfig;
using namespace std;

namespace FrontEndLVWrapper {
    unsigned long facilityId = 40;
    unsigned long configId = 1;
    bool logMonTimers = false;
    bool allowSISHeaters = false;
    bool SISOpenLoop = false;
    short FEMode = 0;
    extern bool tryAlternateLockMethod;
    extern bool defeatNormalLockDetect;
    bool correctSISVoltageError = true;
    bool randomizeMonitors = false;
    bool logMonitors = false;
    bool logAmbErrors = true;
    unsigned long CANChannel = 0;
    unsigned long nodeAddress = 0x13;
    bool startCompressorModule = false;
    static FrontEndImpl *frontEnd = NULL;
    static WCAImpl *WCAs[10] = {0,0,0,0,0,0,0,0,0,0};
    static ColdCartImpl *coldCarts[10] = {0,0,0,0,0,0,0,0,0,0};
    static PowerModuleImpl *powerMods[10] = {0,0,0,0,0,0,0,0,0,0};
    static bool FEValid = false;
    static int connectedFEClients = 0;
    std::string FineLoSweepIni("");
};
using namespace FrontEndLVWrapper;

// forward declare some helpers defined below:
void loadConfigIds();
void createCartridge(int port, const FrontEndConfig &feConfig,
                     WCAImpl **WCA,
                     ColdCartImpl **coldCart,
                     PowerModuleImpl **powerMod);


DLLEXPORT short FEControlInit() {
    ++connectedFEClients;
    if (frontEnd && FEValid) {
        LOG(LM_INFO) << "FEControlInit: connectedFEClients=" << connectedFEClients << endl;
        return 0;
    }

    // Initialize the DLL wrapper:
    if (LVWrapperInit() < 0) {
        LOG(LM_ERROR) << "FEControlInit: LVWrapperInit failed." << endl;
        return -1;
    }

    std::string iniPath, tmp;

    // get the path where the FrontendControlDLL.ini file is located:
    splitPath(iniFileName, iniPath, tmp);
    if (iniPath.empty())
        iniPath = ".";

    // Log the FrontEndIni file we are using:
    LOG(LM_INFO) << "Using FrontEnd configuration file '" << FrontEndIni << "'" << endl;

    LOG(LM_INFO) << "FEControlInit: connectedFEClients=" << connectedFEClients << endl;
    try {
        CIniFile configINI(iniFileName);
        configINI.ReadFile();

        tmp = configINI.GetValue("connection", "channel");
        if (!tmp.empty())
            CANChannel = from_string<unsigned long>(tmp);
        tmp  = configINI.GetValue("connection", "nodeAddress");
        if (!tmp.empty()) {
            if (tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X'))
                tmp = tmp.substr(2);
            nodeAddress = from_string<unsigned long>(tmp, std::hex);
        }
        LOG(LM_INFO) << "Front end is on CAN" << CANChannel
            << " at nodeAddress=" << uppercase << hex << setw(2) << setfill('0') << nodeAddress << dec << setw(0) << endl;

        tmp = configINI.GetValue("connection", "startCompressor");
        if (!tmp.empty())
            startCompressorModule = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "Also starting Compressor module=" << startCompressorModule << endl;

        tmp = configINI.GetValue("debug", "allowSISHeaters");
        if (!tmp.empty())
            allowSISHeaters = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "allowSISHeaters=" << allowSISHeaters << endl;
            
        tmp = configINI.GetValue("debug", "correctSISVoltageError");
        if (!tmp.empty())
            correctSISVoltageError = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "correctSISVoltageError=" << correctSISVoltageError << endl;
        FrontEndImpl::correctSISVoltageError(correctSISVoltageError);

        tmp = configINI.GetValue("debug", "SISOpenLoop");
        if (!tmp.empty())
            SISOpenLoop = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "SISOpenLoop=" << SISOpenLoop << endl;

        tmp = configINI.GetValue("debug", "tryAlternateLockMethod");
        if (!tmp.empty())
            tryAlternateLockMethod = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "tryAlternateLockMethod=" << tryAlternateLockMethod << endl;

        tmp = configINI.GetValue("debug", "defeatNormalLockDetect");
        if (!tmp.empty())
            defeatNormalLockDetect = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "defeatNormalLockDetect=" << defeatNormalLockDetect << endl;

        tmp = configINI.GetValue("debug", "FEMode");
        if (!tmp.empty())
            FEMode = from_string<short>(tmp);
        LOG(LM_INFO) << "debug:FEMode=" << FEMode << endl;

        tmp = configINI.GetValue("logger", "randomizeAnalogMonitors");
        if (!tmp.empty())
            randomizeMonitors = from_string<unsigned long>(tmp);
        FEHardwareDevice::randomizeAnalogMonitors(randomizeMonitors);
        LOG(LM_INFO) << "randomizeAnalogMonitors=" << randomizeMonitors << endl;
        
        tmp = configINI.GetValue("logger", "logMonitors");
        if (!tmp.empty())
            logMonitors = from_string<unsigned long>(tmp);
        FEHardwareDevice::logMonitors(logMonitors);
        LOG(LM_INFO) << "logMonitors=" << logMonitors << endl;

        tmp = configINI.GetValue("logger", "logAmbErrors");
        if (!tmp.empty())
            logAmbErrors = from_string<unsigned long>(tmp);
        if (!CAN_noTransmit) {
            LOG(LM_INFO) << "logAmbErrors=" << logAmbErrors << endl;
        } else {
            logAmbErrors = false;
            LOG(LM_INFO) << "logAmbErrors=0 because CAN_noTransmit=1" << endl;
        }
        FEHardwareDevice::logAmbErrors(logAmbErrors);

        tmp = configINI.GetValue("debug", "logMonTimers");
        if (!tmp.empty())
            logMonTimers = (from_string<unsigned int>(tmp) != 0);
        LOG(LM_INFO) << "logMonTimers=" << logMonTimers << endl;

        // look for the item specifying a separate file for FineLOSweep
        tmp = configINI.GetValue("configFiles", "FineLOSweep");
        if (tmp.empty())
            // not found.
            FineLoSweepIni = "";
        else
            // Found: use the current ini files path plus the specified name:
            FineLoSweepIni = iniPath + "/" + tmp;
        LOG(LM_INFO) << "FineLOSweep ini file='" << FineLoSweepIni  << "'" << endl;

    } catch (...) {
        LOG(LM_ERROR) << "FEControlInit exception loading configuration file." << endl;
        FEMCEventQueue::addStatusMessage(false, "An exception occurred while loading the configuration file.");
        return -1;
    }
    
    short ret = -1;
    if (nodeAddress) {
        frontEnd = new FrontEndImpl(CANChannel, nodeAddress, "ESN-FE");
        if (frontEnd -> connected()) {
            frontEnd -> setLogMonTimers(logMonTimers);
            ret = 0;
        } else {
            LOG(LM_ERROR) << "FEControlInit failed to connect to front end." << endl;
            FEMCEventQueue::addStatusMessage(false, "Failed to connect to front end.");
        }            
    }
    
    // If we are in a debugging mode where no CAN data is sent, pretend we got connected to the FE:
    if (CAN_noTransmit)
        ret = 0;

    // Set the INI file to use for FineLOSweep:
    frontEnd -> setFineLOSweepIniFile(FineLoSweepIni);

    // set the FE operating mode:
    frontEnd -> setFEMode((unsigned char) FEMode);

    // Initialize the Compressor M&C module here, if configured:
    if (startCompressorModule) {
        if (CompressorControlInit() != 0) {
            LOG(LM_ERROR) << "FEControlInit failed to start compressor M&C module." << endl;
        }
    }

    // Load the default front end configuration:
    loadConfigIds();
    ConfigProvider *provider(NULL);
    provider = new ConfigProviderIniFile(FrontEndIni);

    ConfigManager configMgr(*provider);
    Configuration config(facilityId, configId);
    if (!config.load(*provider))
        ret = -1;
    WHACK(provider);

    // Create the FE subsystems:
    if (ret == 0) {
        const FrontEndConfig *feConfig = config.getFrontEndConfig();
        if (feConfig) {
            for (int index = 0; index < 10; ++index) {
                createCartridge(index + 1, *feConfig, &(WCAs[index]), &(coldCarts[index]), &(powerMods[index]));
            }

            if (feConfig -> getCryostatConfig())
                frontEnd -> addCryostat();

            if (feConfig -> getFETIMConfig())
                frontEnd -> addFETIM();

            if (feConfig -> getIFSwitchConfig())
                frontEnd -> addIFSwitch();

            if (feConfig -> getLPRConfig())
                frontEnd -> addLPR();

            if (feConfig -> getPowerDistConfig())
                frontEnd -> addCPDS();

            // Apply the configuration:
            configMgr.configure(config, *frontEnd);
            FEValid = true;
            LOG(LM_INFO) << "FEControlInit successful" << endl;
        }
    }

    // Do final setup actions if no error so far:
    if (ret == 0) {

        // Start the thermal logger:
        frontEnd -> setThermalLogInterval(thermalLogInterval);

        // Flush and log all previous FEMC module errors:
        LOG(LM_INFO) << "Flushing FEMC Error Queue..." << endl;
        FEMCFlushErrors();

        // Start all monitor threads:
        frontEnd -> startMonitor();

        // Query the state of the cartridges:
        if (!CAN_noTransmit)
            frontEnd -> queryCartridgeState();
    }
    return ret;
}

DLLEXPORT short FEControlShutdown() {
    --connectedFEClients;
    LOG(LM_INFO) << "FEControlShutdown: connectedFEClients=" << connectedFEClients << endl;
    if (connectedFEClients <= 0) {
        FEValid = false;

        if (frontEnd) {
            frontEnd -> stopMonitor();
            frontEnd -> shutdown();

            WHACK(frontEnd);
            LOG(LM_INFO) << "FEControlShutdown: frontEnd destroyed." << endl;
        }
        for (int index = 0; index < 10; ++index) {
            WHACK(WCAs[index]);
            WHACK(coldCarts[index]);
            WHACK(powerMods[index]);
        }
        LOG(LM_INFO) << "FEControlShutdown: modules destroyed." << endl;

        // Disconnect from the Compressor M&C module here, if configured:
        if (startCompressorModule)
            CompressorControlShutdown();

        LVWrapperShutdown();
    }
    return 0;
}

DLLEXPORT short FEMCExit() {
    if (!FEValid)
        return -1;
    frontEnd -> specialExitProgram(true);
    return 0;
}

DLLEXPORT short FEGetConfigSN(long *_configId, char *_serialNum) {
    if (!FEValid || !_configId || !_serialNum)
        return -1;

    FEICDataBase::ID_T configId;
    frontEnd -> getDbConfigId(configId);
    *_configId = configId.keyId;

    std::string serialNum = frontEnd -> getFrontEndSN();
    strcpy(_serialNum, serialNum.c_str());
    return 0;
}

DLLEXPORT short FEMCGetAMBSIInfo(char *_serialNum,
                                 char *_firmwareRev, 
                                 unsigned long *numErrors, 
                                 unsigned long *numTransactions)
{
    if (!FEValid || !_serialNum || !_firmwareRev || !numErrors || !numTransactions)
        return -1;

    std::string serialNum;
    std::string firmwareRev;
    frontEnd -> getAMBSIInfo(serialNum, firmwareRev, *numErrors, *numTransactions);
    strcpy(_firmwareRev, firmwareRev.c_str());
    strcpy(_serialNum, serialNum.c_str());
    return 0;
}

DLLEXPORT short FEMCGetAMBSITemperature(float *temperature) {
    if (!FEValid || !temperature)
        return -1;
    *temperature = frontEnd -> getAMBSITemperature();
    return 0;
}

DLLEXPORT short FEMCGetFirmwareInfo(char *_AMBSILibraryVersion, 
                                    char *_FEMCFirmwareVersion,
                                    char *_FEMCFPGAVersion)
{
    if (!FEValid || !_AMBSILibraryVersion || !_FEMCFirmwareVersion)
        return -1;
    std::string AMBSILibraryVersion,
                FEMCFirmwareVersion,
                FEMCFPGAVersion;
    frontEnd -> getFirmwareInfo(AMBSILibraryVersion, FEMCFirmwareVersion, FEMCFPGAVersion);
    strcpy(_AMBSILibraryVersion, AMBSILibraryVersion.c_str());
    strcpy(_FEMCFirmwareVersion, FEMCFirmwareVersion.c_str());
    strcpy(_FEMCFPGAVersion, FEMCFPGAVersion.c_str());
    return 0;
}

DLLEXPORT short FEMCGetESNText(char *target) {
    if (!FEValid || !target)
        return -1;

    std::string ESN, text, ESN0("0000000000000000");

    int numESNs = frontEnd -> FEMCGetESNsFound();
    if (numESNs <= 0) {
        strcpy(target, "no ESNs found");
        return 0;
    }

    char buf[20];
    sprintf(buf, "ESNs: %d", numESNs);
    text += buf;
    
    LOG(LM_INFO) << text << endl;

    int count = numESNs + 1;
    while (count-- > 0) {
        ESN = frontEnd -> FEMCGetNextESN(false); // do not reverse ESN byte order.
        if (ESN != ESN0) {
            text += "\n";
            text += ESN;
            LOG(LM_INFO) << ESN << endl;
        }
    }
    strcpy(target, text.c_str());
    return 0;
}

DLLEXPORT short FEMCRescanESNs() {
    if (!FEValid)
        return -1;

    frontEnd -> specialReadESNs(true);
    return 0;
}

DLLEXPORT short FEMCGetNumErrors() {
    if (!FEValid)
        return -1;
    else
        return static_cast<short>(frontEnd -> getNumErrors());
}

DLLEXPORT short FEMCFlushErrors() {
    if (!FEValid)
        return -1;
    else {
        short moduleNum, errorNum;
        char description[255];
        while (FEMCGetNextError(&moduleNum, &errorNum, description) == 0) {
            0;
        }
    }
    return 0;
}

DLLEXPORT short FEMCGetNextError(short *moduleNum, short *errorNum, char *description) {

    if (moduleNum)
        *moduleNum = -1;
    if (errorNum)
        *errorNum = -1;
    if (description)
        *description = '\0';

    if (FEValid) {
        unsigned char mod, err;
        std::string desc;
        if (frontEnd -> getNextError(mod, err, desc)) {
            if (moduleNum)
                *moduleNum = (short) mod;
            if (errorNum)
                *errorNum = (short) err;
            if (description)
                strcpy(description, desc.c_str());
            return 0;
        }
    }
    return -1;
}

DLLEXPORT short FEMCSetFEMode(short mode) {
    if (!FEValid || mode < 0 || mode > 2)
        return -1;

    frontEnd -> setFEMode((unsigned char) mode);
    return 0;
}

//----------------------------------------------------------------------------

void createCartridge(int port, const FrontEndConfig &feConfig, 
                     WCAImpl **WCA,
                     ColdCartImpl **coldCart,
                     PowerModuleImpl **powerMod)
{
    const CartAssemblyConfig *caConfig = feConfig.getCartridgeConfig(port);  
    if (caConfig) {
        const WCAConfig &wcaConfig = caConfig -> WCA_m;
        const ColdCartConfig &ccConfig = caConfig -> coldCart_m;
        int wcaBand = wcaConfig.band_m; 
        int ccBand = ccConfig.band_m; 
        char name[30];

        if (port < 1 || port > 10)
            LOG(LM_ERROR) << "createCartridge error: out of range cartridge port=" << port << endl;
        
        else {        
            LOG(LM_INFO) << "createCartridge: port=" << port << " wcaBand=" << wcaBand << " ccBand=" << ccBand << endl;
            
            // create the WCA if the configuration calls for it: 
            if (WCA && wcaBand > 0) {
                sprintf(name, "band %d WCA", wcaBand);     
                (*WCA) = new WCAImpl(CANChannel, nodeAddress, name, port, wcaBand, wcaConfig.ESN_m);            
            }
            // create the coldCart if the configuration calls for it: 
            if (coldCart && ccBand > 0) {
                sprintf(name, "band %d ColdCart", ccBand);
                (*coldCart) = new ColdCartImpl(CANChannel, nodeAddress, name, port, ccBand, ccConfig.ESN_m);
            }
            // determine if we have a cartAssembly, or just a bare WCA or coldCart:
            if (WCA && *WCA) {
                if (coldCart && *coldCart)
                    // add a cartAssembly:                    
                    frontEnd -> addCartridge(port, **WCA, **coldCart);                
                else
                    // add a bare WCA:
                    frontEnd -> addCartridge(port, **WCA);
            } else if (coldCart && *coldCart) {
                // add a bare coldCart:
                frontEnd -> addCartridge(port, **coldCart);                
            }
            // create and add the power distribution module, if configured:
            if (powerMod) {
                sprintf(name, "port %d PowerModule", port);
                (*powerMod) = new PowerModuleImpl(CANChannel, nodeAddress, name, port);
                frontEnd -> addPowerModule(**powerMod);
            }
        }
    }
}

void loadConfigIds() {
    // get the facility code from the new key facilityId:
    CIniFile configINI(FrontEndIni);
    configINI.ReadFile();
    std::string tmp = configINI.GetValue("configuration", "facilityId");
    if (!tmp.empty())
        facilityId = from_string<unsigned long>(tmp);
    else {
        // but fall back on the old name if that doesn't work:
        tmp = configINI.GetValue("configuration", "providerCode");
        if (!tmp.empty())
            facilityId = from_string<unsigned long>(tmp);
    }

    tmp = configINI.GetValue("configuration", "configId");
    if (!tmp.empty())
        configId = from_string<unsigned long>(tmp);
    LOG(LM_INFO) << "Using configuration facilityId=" << facilityId << " configId=" << configId << endl;
}

DLLEXPORT short FELoadConfiguration(short configId_in) {
    short ret = 0;

    loadConfigIds();
    if (configId_in)
        configId = configId_in;

    // Load the default front end configuration:
    ConfigProvider *provider(NULL);
    provider = new ConfigProviderIniFile(FrontEndIni);

    ConfigManager configMgr(*provider);
    Configuration config(facilityId, configId);
    if (!config.load(*provider))
        ret = -1;
    WHACK(provider);

    // Create the FE subsystems:
    if (ret == 0) {
        // Apply the configuration:
        configMgr.configure(config, *frontEnd);
        FEValid = true;
        LOG(LM_INFO) << "FELoadConfiguration successful" << endl;
    }
    return ret;
}

//----------------------------------------------------------------------------

DLLEXPORT short FEGetConfiguredBands(short *size, short *bands) {
    if (!FEValid)
        return -1;
    if (!size || !bands)
        return -1;
    int index = 0;
    for (short band = 1; band <= 10; ++band) {
        if (index < *size && frontEnd -> existsCartAssembly(band)) {
            bands[index] = band;
            index++;
        }
    }
    *size = index;
    return 0;
}

DLLEXPORT short FEStartHealthCheck(short dataStatus) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> startHealthCheck(dataStatus))
        return -1;
    return 0;
}

DLLEXPORT short cartHealthCheck(short port, short warmUpTimeSeconds, short includeIFPower) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartHealthCheck(port, warmUpTimeSeconds, (includeIFPower != 0)))
        return -1;
    return 0;
}

DLLEXPORT short cartHealthCheckSaveIFPowerData(short port, CartIFPowerData_t *source) {
    if (!FEValid || !source)
        return -1;

    IFPowerDataSet data;
    for (int field = 0; field < 16; ++field)
        data[field] = source -> getFloat((CartIFPowerData_t::Field) field);

    if (!frontEnd -> cartHealthCheckSaveIFPowerData(port, data))
        return -1;
    return 0;
}

DLLEXPORT short FEFinishHealthCheck() {
    if (!FEValid)
        return -1;
    if (!frontEnd -> finishHealthCheck())
        return -1;
    return 0;
}

//----------------------------------------------------------------------------

bool validatePortNumber(short &port, bool *isSignalSource = NULL) {
    if (port < 0) {
        // IF port is negative, caller is referring to the signal source module instead.
        port = -1;
        if (isSignalSource)
            *isSignalSource = true;
        return true;
    }
    if (isSignalSource)
        *isSignalSource = false;
    if (port >= 1 && port <= 10)
        return true;
    else
        return false;
}


DLLEXPORT short FESetCartridgeOff(short port) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> setCartridgeOff())
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> setCartridgeOff(port))
            return -1;
        return 0;
    }
}

DLLEXPORT short FESetCartridgeOn(short port) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> setCartridgeOn())
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> getCartridgeOn(port)) {
            if (!frontEnd -> setCartridgeOn(port))
                return -1;
        }
        return 0;
    }
}

DLLEXPORT short FESetCartridgeStandby2(short port, short enable) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        return -1;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> setCartridgeStandby2(port, (enable!=0)))
            return -1;
        return 0;
    }
}

DLLEXPORT short FESetCartridgeObserving(short port) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> setCartridgeObserving(port))
        return -1;
    return 0;        
}

DLLEXPORT short FEClearCartridgeObserving(short port) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    frontEnd -> clearCartridgeObserving();
    return 0;        
}

DLLEXPORT short FEGetCartridgeState(short port, CartStateData_t *target) {
    if (!target)
        return -1;
    target -> reset();

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        // First check if the signal source is powered on:
        bool enabled = sigSrc -> getCartridgeOn();
        if (enabled)
            target -> setBool(CartStateData_t::CART_ENABLED, true);

        // Remaining checks are only meaningful if the power is on:
        if (enabled) {
            if (sigSrc -> cartGetEnableLOPowerAmps())
                target -> setBool(CartStateData_t::CART_LO_PA_ENABLED, true);
            if (sigSrc -> cartGetEnablePhotomixer())
                target -> setBool(CartStateData_t::CART_PHOTOMIXER_ENABLED, true);
            if (sigSrc -> cartGetNullPLLIntegrator())
                target -> setBool(CartStateData_t::CART_ZERO_INTEGRATOR_ENABLED, true);
        }
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;

    } else {
        if (!FEValid)
            return -1;

        // First check if the cartridge is powered on:
        bool enabled = frontEnd -> getCartridgeOn(port);
        if (enabled)
            target -> setBool(CartStateData_t::CART_ENABLED, true);

        // It is possible for the cartridge to be observing even if not on:
        if (port == frontEnd -> getCartridgeObserving())
            target -> setBool(CartStateData_t::CART_OBSERVING, true);

        // Remaining checks are only meaningful if the power is on:
        if (enabled) {
            if (frontEnd -> cartGetEnableLNABias(port))
                target -> setBool(CartStateData_t::CART_LNA_ENABLED, true);
            if (frontEnd -> cartGetEnableSISBias(port))
                target -> setBool(CartStateData_t::CART_SIS_ENABLED, true);
            if (frontEnd -> cartGetEnableSISMagnet(port))
                target -> setBool(CartStateData_t::CART_SIS_MAGNET_ENABLED, true);
            if (frontEnd -> cartGetEnableLOPowerAmps(port))
                target -> setBool(CartStateData_t::CART_LO_PA_ENABLED, true);
            if (frontEnd -> cartGetEnablePhotomixer(port))
                target -> setBool(CartStateData_t::CART_PHOTOMIXER_ENABLED, true);
            if (frontEnd -> cartGetNullPLLIntegrator(port))
                target -> setBool(CartStateData_t::CART_ZERO_INTEGRATOR_ENABLED, true);
        }
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;
    }
}

DLLEXPORT short FEGetNumCartridgesOn() {
    if (!FEValid)
        return -1;
    return (short) frontEnd -> powerGetMonitorNumEnabled();        
}

DLLEXPORT short cartPauseMonitor(short port, short pauseWCA, short pauseCC) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        return sigSrc -> cartPauseMonitor((pauseWCA!=0));

    } else {
        if (!FEValid)
            return -1;
        return frontEnd -> cartPauseMonitor(port, (pauseWCA!=0), (pauseCC!=0));
    }
}

DLLEXPORT short randomizeAnalogMonitors(short enable) {
    FEHardwareDevice::randomizeAnalogMonitors(enable != 0);
    return 0;
}

//----------------------------------------------------------------------------

DLLEXPORT short cartSetLOFrequency(short port, double freqLO, double freqFLOOG, short sbLock) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartSetLOFrequency(freqLO, freqFLOOG, sbLock))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetLOFrequency(port, freqLO, freqFLOOG, sbLock))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartSetCenterLOFrequency(short port, double freqFLOOG, short sbLock) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (!isSignalSource) {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetCenterLOFrequency(port, freqFLOOG, sbLock))
            return -1;
        return 0;
    }
    return -1;
}

DLLEXPORT short cartGetLOFrequency(short port, double *freqLO, double *freqREF) {
    if (!freqLO || !freqREF)
        return -1;

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetLOFrequency(*freqLO, *freqREF))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetLOFrequency(port, *freqLO, *freqREF))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartLockPLL(short port) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartLockPLL())
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartLockPLL(port))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartGetLocked(short port, short *locked) {
    if (!locked)
        return -1;

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        *locked = (sigSrc -> cartGetLocked()) ? 1 : 0;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        *locked = (frontEnd -> cartGetLocked(port)) ? 1 : 0;
        return 0;
    }
}

DLLEXPORT short cartAdjustPLL(short port, float targetCorrVoltage) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartAdjustPLL(targetCorrVoltage))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartAdjustPLL(port, targetCorrVoltage))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartAdjustYTO(short port, short steps) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartAdjustYTO(steps))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartAdjustYTO(port, steps))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartNullPLLIntegrator(short port, short enable) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartNullPLLIntegrator((enable != 0)))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartNullPLLIntegrator(port, (enable != 0)))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartSetLNABias(short port, short pol, short sb, 
                               float VD1, float ID1,
                               float VD2, float ID2,
                               float VD3, float ID3)
{
    if (!validatePortNumber(port))
        return -1;

    if (!FEValid)
        return -1;

    bool setAll = false;
    if (pol < 0 || pol > 1) {
        pol = -1;
        setAll = true;
    }
    if (sb < 1 || sb > 2) {        
        sb = -1;
        setAll = true;
    }        
    if (!frontEnd -> cartSetLNABias(port, pol, sb,
                                    (setAll || isinf(VD1)) ? NULL : &VD1,
                                    (setAll || isinf(ID1)) ? NULL : &ID1,
                                    (setAll || isinf(VD2)) ? NULL : &VD2,
                                    (setAll || isinf(ID2)) ? NULL : &ID2,
                                    (setAll || isinf(VD3)) ? NULL : &VD3,
                                    (setAll || isinf(ID3)) ? NULL : &ID3))
        return -1;
    return 0;
}

DLLEXPORT short cartSetEnableLNABias(short port, short enable, short pol, short sb) {
    if (!validatePortNumber(port))
        return -1;

    if (!FEValid)
        return -1;
        
    bool setAll = false;
    if (pol < 0 || pol > 1) {
        pol = -1;
        setAll = true;
    }
    if (sb < 1 || sb > 2) {        
        sb = -1;
        setAll = true;
    }        
    if (!frontEnd -> cartSetEnableLNABias(port, enable, pol, sb))
        return -1;
    return 0;
}

DLLEXPORT short cartSetSISBias(short port, short enable, short pol, short sb, float VJ, short openLoop)
{
    if (!validatePortNumber(port))
        return -1;

    if (!FEValid)
        return -1;

    bool setAll = false;
    if (pol < 0 || pol > 1) {
        pol = -1;
        setAll = true;
    }
    if (sb < 1 || sb > 2) {        
        sb = -1;
        setAll = true;
    }
    if (openLoop > 1)
        openLoop = 1;
    else if (openLoop < 0) {
        // WAS: openLoop = -1;
        // Apply the setting from the INI file:
        if (SISOpenLoop)
            openLoop = 1;
        else
            openLoop = 0;
    }
    if (!frontEnd -> cartSetSISBias(port, (enable != 0), pol, sb,
                                    (setAll || isinf(VJ)) ? NULL : &VJ,
                                    openLoop))
        return -1;
    return 0;
}

DLLEXPORT short cartSetSISMagnet(short port, short enable, short pol, short sb, float IMag)
{
    if (!validatePortNumber(port))
        return -1;

    if (!FEValid)
        return -1;

    bool setAll = false;
    if (pol < 0 || pol > 1) {
        pol = -1;
        setAll = true;
    }
    if (sb < 1 || sb > 2) {        
        sb = -1;
        setAll = true;
    }
    if (!frontEnd -> cartSetSISMagnet(port, (enable != 0), pol, sb,
                                      (setAll || isinf(IMag)) ? NULL : &IMag))
        return -1;
    return 0;
}

DLLEXPORT short cartGetSISMagnet(short port, short pol, short sb, float *IMag) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartGetSISMagnet(port, pol, sb, IMag))
        return -1;
    return 0;   
}

DLLEXPORT short cartSetLOPowerAmps(short port, short enable,
                                   float VDP0, float VGP0,
                                   float VDP1, float VGP1) 
{
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartSetLOPowerAmps((enable != 0),
                                          (isinf(VDP0)) ? NULL : &VDP0,
                                          (isinf(VGP0)) ? NULL : &VGP0,
                                          (isinf(VDP1)) ? NULL : &VDP1,
                                          (isinf(VGP1)) ? NULL : &VGP1))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetLOPowerAmps(port, (enable != 0),
                                            (isinf(VDP0)) ? NULL : &VDP0,
                                            (isinf(VGP0)) ? NULL : &VGP0,
                                            (isinf(VDP1)) ? NULL : &VDP1,
                                            (isinf(VGP1)) ? NULL : &VGP1))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartGetLOPowerAmpsSetting(short port, short *isEnabled,
                                          float *VDP0, float *VGP0,
                                          float *VDP1, float *VGP1)
{
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    bool enable(false);

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetLOPowerAmpsSetting(enable, VDP0, VGP0, VDP1, VGP1))
            return -1;
        if (isEnabled)
            *isEnabled = (enable) ? 1 : 0;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetLOPowerAmpsSetting(port, enable, VDP0, VGP0, VDP1, VGP1))
            return -1;
        if (isEnabled)
            *isEnabled = (enable) ? 1 : 0;
        return 0;
    }
}

DLLEXPORT short cartAdjustLOPowerAmps(short port, short repeatCount) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartAdjustLOPowerAmps(port, repeatCount))
        return -1;
    return 0;                                           
}

DLLEXPORT short cartSetEnableLO(short port, short enable) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartSetEnableLO((enable != 0)))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetEnableLO(port, (enable != 0)))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartSetLOPower(short port, short pol, float percent) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartSetLOPower(pol, percent))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetLOPower(port, pol, percent))
            return -1;
        return 0;
    }
}

DLLEXPORT short cartOptimizeIFPower(short port, short pol, float VDstart0, float VDstart1) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    bool doPol0 = (pol == 0 || pol == -1);      
    bool doPol1 = (pol == 1 || pol == -1);
    if (!(doPol0 || doPol1))
        return -1;
    if (!frontEnd -> cartOptimizeIFPower(port, doPol0, doPol1, VDstart0, VDstart1))
        return -1;
    return 0;                                           
}   

DLLEXPORT short cartClearOptimizedResult(short port) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartClearOptimizedResult(port))
        return -1;
    return 0;
}

DLLEXPORT short cartGetOptimizedResult(short port, char *_mixerParamsText) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid || !_mixerParamsText)
        return -1;

    std::string mixerParamsText;
    if (!frontEnd -> cartGetOptimizedResult(port, mixerParamsText))
        return -1;

    strcpy(_mixerParamsText, mixerParamsText.c_str());
    return 0;
}

DLLEXPORT short cartSetIFPower(short port, short pol, float powerSB1, float powerSB2) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartSetIFPower(port, pol, powerSB1, powerSB2))
        return -1;
    return 0;
}

DLLEXPORT short cartSetVJVD(short port, short pol,
                            float VJ1, float VJ2, float VD)
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartSetVJVD(port, pol,
                                 isinf(VJ1) ? NULL : &VJ1, 
                                 isinf(VJ2) ? NULL : &VJ2, 
                                 isinf(VD) ? NULL : &VD))
        return -1;
    return 0;   
}

DLLEXPORT short cartGetVJVD(short port, short pol,
                            float *VJ1, float *VJ2, float *VD)
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartGetVJVD(port, pol, VJ1, VJ2, VD))
        return -1;
    return 0;   
}

DLLEXPORT short cartSetAMC(short port, float VDE, float VGE) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartSetAMC((isinf(VDE) ? NULL : &VDE),
                                  (isinf(VGE) ? NULL : &VGE)))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetAMC(port,
                                    (isinf(VDE) ? NULL : &VDE),
                                    (isinf(VGE) ? NULL : &VGE)))
            return -1;
        return 0;
    }
}


DLLEXPORT short cartMeasureFineLOSweep(short port, float tiltAngle, short pol, float VJ, float IJ, float fixedVD,
                                       double LOStart, double LOStop, double LOStep, short repeatCount)
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartMeasureFineLOSweep(port, tiltAngle, pol,
                                            isinf(VJ) ? NULL : &VJ,
                                            isinf(IJ) ? NULL : &IJ,
                                            isinf(fixedVD) ? NULL : &fixedVD,
                                            (isinf(LOStart) || (LOStart == 0)) ? NULL : &LOStart,
                                            (isinf(LOStop) || (LOStop == 0)) ? NULL : &LOStop,
                                            (isinf(LOStep) || (LOStep == 0)) ? NULL : &LOStep,
                                            repeatCount))
        return -1;
    return 0;
}

DLLEXPORT short cartMeasureIVCurve(short port, short pol, short sb, 
                                   float VJLow, float VJHigh, float VJStep, short repeatCount) 
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartMeasureIVCurve(port, pol, sb,
                                        isinf(VJLow) ? NULL : &VJLow, 
                                        isinf(VJHigh) ? NULL : &VJHigh, 
                                        isinf(VJStep) ? NULL : &VJStep, 
                                        repeatCount))
        return -1;
    return 0;
}

DLLEXPORT short cartMeasureIJvsSISMagnet(short port, short pol, short sb, 
                                         float IMagStart, float IMagStop, float IMagStep, 
                                         float VJLow, float VJHigh, float VJStep,
                                         short repeatCount)
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartMeasureIJvsSISMagnet(port, pol, sb, IMagStart, IMagStop, IMagStep, VJLow, VJHigh, VJStep, repeatCount))
        return -1;
    return 0;
}

DLLEXPORT short cartMeasureIFPowerVsVJ(short port, short pol, short sb,
                                       float VJStart, float VJStop, float VJStep, 
                                       bool doYFactor, short repeatCount)
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartMeasureIFPowerVsVJ(port, pol, sb, VJStart, VJStop, VJStep, doYFactor, repeatCount))
        return -1;
    return 0;
}

DLLEXPORT short cartMeasureIFPowerVsVD(short port, short pol, 
                                       float VDLow, float VDHigh, float VDStep, 
                                       bool doYFactor, short repeatCount)
{
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartMeasureIFPowerVsVD(port, pol, VDLow, VDHigh, VDStep, doYFactor, repeatCount))
        return -1;
    return 0;
}

DLLEXPORT short cartMixerDeflux(short port, short pol, short sb, short IMagMax) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartMixerDeflux(port, pol, sb, IMagMax))
        return -1;
    return 0;
}


DLLEXPORT short cartAbortMeasurement(short port) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    frontEnd -> cartAbortMeasurement(port);
    return 0;
}


DLLEXPORT short cartGetXYData(short port, short *size, float *X, float *Y, float *Y2) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!size || !X || !Y)
        return -1;
    bool assignY2 = true;
    if (!Y2) {
        LOG(LM_INFO) << "cartGetXYData: Y2 is NULL" << endl;
        assignY2 = false;
    }
    
    const XYPlotArray &data = frontEnd -> cartGetXYData(port);

    XYPlotArray::const_iterator it = data.begin();
    int index = 0;
    while (index < *size && it != data.end()) {
        X[index] = (*it).X;
        Y[index] = (*it).Y;
        if (assignY2)
            Y2[index] = (*it).Y2;
        ++it;
        ++index;
    }
    *size = index;
    return 0;
}

DLLEXPORT short cartSetEnableLNALEDs(short port, short enable) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!frontEnd -> cartSetEnableLNALEDs(port, (enable != 0)))
        return -1;
    return 0;
}    

DLLEXPORT short cartSetEnableSISHeaters(short port, short enable, short pol, float targetTemp, short timeout) {
    if (!validatePortNumber(port))
        return -1;
    if (!FEValid)
        return -1;
    if (!allowSISHeaters)
        return -1;
    if (!frontEnd -> cartSetEnableSISHeaters(port, (enable != 0), pol, targetTemp, timeout))
        return -1;
    return 0;
}

DLLEXPORT short cartSetEnablePhotomixer(short port, short enable) {
    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartSetEnablePhotomixer((enable != 0)))
            return -1;
        return 0;

    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartSetEnablePhotomixer(port, (enable != 0)))
            return -1;
        return 0;
    }
}

DLLEXPORT short ifSwitchSetAttenuation(short pol, short sb, short atten) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> ifSwitchSetAttenuation(pol, sb, atten))
        return -1;
    return 0;
}

DLLEXPORT short ifSwitchSetEnableTempServo(short pol, short sb, short enable) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> ifSwitchSetEnableTempServo(pol, sb, (enable != 0)))
        return -1;
    return 0;
}

DLLEXPORT short cryostatSetEnableBackingPump(short enable) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cryostatSetEnableBackingPump(enable != 0))
        return -1;
    return 0;
}

DLLEXPORT short cryostatSetEnableTurboPump(short enable) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cryostatSetEnableTurboPump(enable != 0))
        return -1;
    return 0;
}

DLLEXPORT short cryostatSetGateValveState(short open) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cryostatSetGateValveState(open != 0))
        return -1;
    return 0;
}

DLLEXPORT short cryostatSetSolenoidValveState(short open) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cryostatSetSolenoidValveState(open != 0))
        return -1;
    return 0;
}

DLLEXPORT short cryostatSetEnableVacuumGauge(short enable) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cryostatSetEnableVacuumGauge(enable != 0))
        return -1;
    return 0;
}

DLLEXPORT short cryostatSetEnableCryoPumping(short enable) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> cryostatSetEnableCryoPumping(enable != 0))
        return -1;
    return 0;
}

DLLEXPORT short fetimSetTriggerDewarN2Fill(short enable) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> fetimSetTriggerDewarN2Fill(enable != 0))
        return -1;
    return 0;
}

DLLEXPORT short lprSetOpticalSwitchPort(short port) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> lprSetOpticalSwitchPort(port))
        return -1;
    return 0;
}

DLLEXPORT short lprSetOpticalSwitchShutter() {
    if (!FEValid)
        return -1;
    if (!frontEnd -> lprSetOpticalSwitchShutter())
        return -1;
    return 0;
}

DLLEXPORT short lprSetOpticalSwitchForceShutter() {
    if (!FEValid)
        return -1;
    if (!frontEnd -> lprSetOpticalSwitchForceShutter())
        return -1;
    return 0;
}
    
DLLEXPORT short lprSetEDFAModulationInput(float value) {
    if (!FEValid)
        return -1;
    if (!frontEnd -> lprSetEDFAModulationInput(value))
        return -1;
    return 0;
} 

//----------------------------------------------------------------------------

DLLEXPORT short cartGetMonitorYTO(short port, CartYTOData_t *target) {
    if (!target)
        return -1;
    target -> reset();

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    WCAImpl::YTO_t ytoInfo;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetMonitorYTO(ytoInfo))
            return -1;
    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetMonitorYTO(port, ytoInfo))
            return -1;
    }
    target -> setCoarseTune(ytoInfo.ytoCoarseTune_value);
    target -> setFrequency(ytoInfo.ytoFrequency_value);
    if (debugLVStructures)
        LOG(LM_DEBUG) << *target;
    return 0;
}

DLLEXPORT short cartGetMonitorPhotomixer(short port, CartPhotomixerData_t *target) {
    if (!target)
        return -1;
    target -> reset();

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    WCAImpl::Photomixer_t pmInfo;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetMonitorPhotomixer(pmInfo))
            return -1;
    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetMonitorPhotomixer(port, pmInfo))
            return -1;
    }
    target -> setEnable(pmInfo.photomixerEnable_value);
    target -> setFloat(CartPhotomixerData_t::PHOTOMIXER_VOLTAGE, pmInfo.photomixerVoltage_value);
    target -> setFloat(CartPhotomixerData_t::PHOTOMIXER_CURRENT, pmInfo.photomixerCurrent_value);
    if (debugLVStructures)
        LOG(LM_DEBUG) << *target;
    return 0;
}

DLLEXPORT short cartGetMonitorPLL(short port, CartPLLData_t *target) {
    if (!target)
        return -1;
    target -> reset();

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    WCAImpl::PLL_t pllInfo;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetMonitorPLL(pllInfo))
            return -1;
    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetMonitorPLL(port, pllInfo))
            return -1;
    }
    target -> setBool(CartPLLData_t::PLL_LOCK, pllInfo.pllLock_value);
    target -> setBool(CartPLLData_t::PLL_UNLOCK_DETECT_LATCH, pllInfo.pllUnlockDetectLatch_value);
    target -> setFloat(CartPLLData_t::PLL_LOCK_DETECT_VOLTAGE, pllInfo.pllLockDetectVoltage_value);
    target -> setFloat(CartPLLData_t::PLL_CORRECTION_VOLTAGE, pllInfo.pllCorrectionVoltage_value);
    target -> setFloat(CartPLLData_t::PLL_REF_TOTAL_POWER, pllInfo.pllRefTotalPower_value);
    target -> setFloat(CartPLLData_t::PLL_IF_TOTAL_POWER, pllInfo.pllIfTotalPower_value);
    target -> setBool(CartPLLData_t::PLL_LOOP_BANDWIDTH_SELECT, pllInfo.pllLoopBandwidthSelect_value);
    target -> setBool(CartPLLData_t::PLL_SIDEBAND_LOCK_POL_SELECT, pllInfo.pllSidebandLockSelect_value);
    target -> setBool(CartPLLData_t::PLL_NULL_LOOP_INTEGRATOR, pllInfo.pllNullLoopIntegrator_value);
    target -> setFloat(CartPLLData_t::PLL_ASSEMBLY_TEMP, pllInfo.pllAssemblyTemp_value);
    target -> setFloat(CartPLLData_t::PLL_YTO_HEATER_CURRENT, pllInfo.pllYTOHeaterCurrent_value);
    if (debugLVStructures)
        LOG(LM_DEBUG) << *target;
    return 0;
}

DLLEXPORT short cartGetMonitorAMC(short port, CartAMCData_t *target) {
    if (!target)
        return -1;
    target -> reset();

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    WCAImpl::AMC_t amcInfo;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetMonitorAMC(amcInfo))
            return -1;
    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetMonitorAMC(port, amcInfo))
            return -1;
    }
    target -> setFloat(CartAMCData_t::AMC_DRAIN_A_VOLTAGE, amcInfo.amcDrainAVoltage_value);
    target -> setFloat(CartAMCData_t::AMC_DRAIN_A_CURRENT, amcInfo.amcDrainACurrent_value);
    target -> setFloat(CartAMCData_t::AMC_GATE_A_VOLTAGE, amcInfo.amcGateAVoltage_value);
    target -> setFloat(CartAMCData_t::AMC_DRAIN_B_VOLTAGE, amcInfo.amcDrainBVoltage_value);
    target -> setFloat(CartAMCData_t::AMC_DRAIN_B_CURRENT, amcInfo.amcDrainBCurrent_value);
    target -> setFloat(CartAMCData_t::AMC_GATE_B_VOLTAGE, amcInfo.amcGateBVoltage_value);
    target -> setFloat(CartAMCData_t::AMC_DRAIN_E_VOLTAGE, amcInfo.amcDrainEVoltage_value);
    target -> setFloat(CartAMCData_t::AMC_DRAIN_E_CURRENT, amcInfo.amcDrainECurrent_value);
    target -> setFloat(CartAMCData_t::AMC_GATE_E_VOLTAGE, amcInfo.amcGateEVoltage_value);
    target -> setByte(CartAMCData_t::AMC_MULTIPLIER_D_COUNTS, amcInfo.amcMultiplierDCounts_value);
    target -> setFloat(CartAMCData_t::AMC_MULTIPLIER_D_CURRENT, amcInfo.amcMultiplierDCurrent_value);
    target -> setFloat(CartAMCData_t::AMC_SUPPLY_VOLTAGE_5V, amcInfo.amcSupplyVoltage5V_value);
    if (debugLVStructures)
        LOG(LM_DEBUG) << *target;
    return 0;
}

DLLEXPORT short cartGetMonitorPA(short port, CartPAData_t *target) {
    if (!target)
        return -1;
    target -> reset();

    bool isSignalSource(false);
    if (!validatePortNumber(port, &isSignalSource))
        return -1;

    WCAImpl::PA_t paInfo;

    if (isSignalSource) {
        SignalSourceImpl *sigSrc = getSignalSourceImpl();
        if (!sigSrc)
            return -1;
        if (!sigSrc -> cartGetMonitorPA(paInfo))
            return -1;
    } else {
        if (!FEValid)
            return -1;
        if (!frontEnd -> cartGetMonitorPA(port, paInfo))
            return -1;
    }
    target -> setFloat(CartPAData_t::PA_POL0_DRAIN_VOLTAGE, paInfo.paPol0DrainVoltage_value);
    target -> setFloat(CartPAData_t::PA_POL0_DRAIN_CURRENT, paInfo.paPol0DrainCurrent_value);
    target -> setFloat(CartPAData_t::PA_POL0_GATE_VOLTAGE, paInfo.paPol0GateVoltage_value);
    target -> setFloat(CartPAData_t::PA_POL1_DRAIN_VOLTAGE, paInfo.paPol1DrainVoltage_value);
    target -> setFloat(CartPAData_t::PA_POL1_DRAIN_CURRENT, paInfo.paPol1DrainCurrent_value);
    target -> setFloat(CartPAData_t::PA_POL1_GATE_VOLTAGE, paInfo.paPol1GateVoltage_value);
    target -> setFloat(CartPAData_t::PA_SUPPLY_VOLTAGE_3V, paInfo.paSupplyVoltage3V_value);
    target -> setFloat(CartPAData_t::PA_SUPPLY_VOLTAGE_5V, paInfo.paSupplyVoltage5V_value);
    if (debugLVStructures)
        LOG(LM_DEBUG) << *target;
    return 0;
}

DLLEXPORT short cartGetMonitorTemp(short port, CartTempData_t *target) {
    if (!validatePortNumber(port))
        return -1;
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;
    
    ColdCartImpl::CartridgeTemp_t tempInfo;
    if (frontEnd -> cartGetMonitorTemp(port, tempInfo)) {
        target -> setFloat(CartTempData_t::CARTRIDGE_TEMP0, tempInfo.cartridgeTemperature0_value);
        target -> setFloat(CartTempData_t::CARTRIDGE_TEMP1, tempInfo.cartridgeTemperature1_value);
        target -> setFloat(CartTempData_t::CARTRIDGE_TEMP2, tempInfo.cartridgeTemperature2_value);
        target -> setFloat(CartTempData_t::CARTRIDGE_TEMP3, tempInfo.cartridgeTemperature3_value);
        target -> setFloat(CartTempData_t::CARTRIDGE_TEMP4, tempInfo.cartridgeTemperature4_value);
        target -> setFloat(CartTempData_t::CARTRIDGE_TEMP5, tempInfo.cartridgeTemperature5_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short cartGetMonitorSIS(short port, short pol, short sb, CartSISData_t *target) {
    if (!validatePortNumber(port))
        return -1;
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;
    
    ColdCartImpl::SIS_t sisInfo;
    if (frontEnd -> cartGetMonitorSIS(port, pol, sb, sisInfo)) {
        target -> setOpenLoop(sisInfo.sisOpenLoop_value);
        target -> setFloat(CartSISData_t::SIS_VOLTAGE, sisInfo.sisVoltage_value);    
        target -> setFloat(CartSISData_t::SIS_CURRENT, sisInfo.sisCurrent_value);    
        target -> setFloat(CartSISData_t::SIS_MAGNET_VOLTAGE, sisInfo.sisMagnetVoltage_value);    
        target -> setFloat(CartSISData_t::SIS_MAGNET_CURRENT, sisInfo.sisMagnetCurrent_value);    
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short cartGetMonitorLNA(short port, short pol, short sb, CartLNAData_t *target) {
    if (!validatePortNumber(port))
        return -1;
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;
    
    ColdCartImpl::LNA_t lnaInfo;
    if (frontEnd -> cartGetMonitorLNA(port, pol, sb, lnaInfo)) {
        target -> setEnable(lnaInfo.lnaEnable_value);
        target -> setFloat(CartLNAData_t::LNA1_DRAIN_VOLTAGE, lnaInfo.lnaSt1DrainVoltage_value);    
        target -> setFloat(CartLNAData_t::LNA1_DRAIN_CURRENT, lnaInfo.lnaSt1DrainCurrent_value);    
        target -> setFloat(CartLNAData_t::LNA1_GATE_VOLTAGE, lnaInfo.lnaSt1GateVoltage_value);    
        target -> setFloat(CartLNAData_t::LNA2_DRAIN_VOLTAGE, lnaInfo.lnaSt2DrainVoltage_value);    
        target -> setFloat(CartLNAData_t::LNA2_DRAIN_CURRENT, lnaInfo.lnaSt2DrainCurrent_value);    
        target -> setFloat(CartLNAData_t::LNA2_GATE_VOLTAGE, lnaInfo.lnaSt2GateVoltage_value);    
        target -> setFloat(CartLNAData_t::LNA3_DRAIN_VOLTAGE, lnaInfo.lnaSt3DrainVoltage_value);    
        target -> setFloat(CartLNAData_t::LNA3_DRAIN_CURRENT, lnaInfo.lnaSt3DrainCurrent_value);    
        target -> setFloat(CartLNAData_t::LNA3_GATE_VOLTAGE, lnaInfo.lnaSt3GateVoltage_value);    
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short cartGetMonitorAux(short port, short pol, CartAuxData_t *target) {
    if (!validatePortNumber(port))
        return -1;
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;
    
    ColdCartImpl::Aux_t auxInfo;
    if (frontEnd -> cartGetMonitorAux(port, pol, auxInfo)) {
        target -> setHeaterCurrent(auxInfo.sisHeaterCurrent_value);
        target -> setLEDEnable(auxInfo.lnaLedEnable_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short powerGetMonitorModule(short port, PowerModuleData_t *target) {
    if (!validatePortNumber(port))
        return -1;
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;

    PowerModuleImpl::PowerModule_t modInfo;
    if (frontEnd -> powerGetMonitorModule(port, modInfo)) {
        target -> setFloat(PowerModuleData_t::VOLTAGE_P6V, modInfo.voltageP6V_value);    
        target -> setFloat(PowerModuleData_t::CURRENT_P6V, modInfo.currentP6V_value);    
        target -> setFloat(PowerModuleData_t::VOLTAGE_N6V, modInfo.voltageN6V_value);    
        target -> setFloat(PowerModuleData_t::CURRENT_N6V, modInfo.currentN6V_value);    
        target -> setFloat(PowerModuleData_t::VOLTAGE_P15V, modInfo.voltageP15V_value);    
        target -> setFloat(PowerModuleData_t::CURRENT_P15V, modInfo.currentP15V_value);    
        target -> setFloat(PowerModuleData_t::VOLTAGE_N15V, modInfo.voltageN15V_value);    
        target -> setFloat(PowerModuleData_t::CURRENT_N15V, modInfo.currentN15V_value);    
        target -> setFloat(PowerModuleData_t::VOLTAGE_P24V, modInfo.voltageP24V_value);    
        target -> setFloat(PowerModuleData_t::CURRENT_P24V, modInfo.currentP24V_value);    
        target -> setFloat(PowerModuleData_t::VOLTAGE_P8V, modInfo.voltageP8V_value);    
        target -> setFloat(PowerModuleData_t::CURRENT_P8V, modInfo.currentP8V_value);    
        target -> setEnable(modInfo.enableModule_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short ifSwitchGetMonitor(IFSwitchData_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;

    IFSwitchImpl::IFSwitch_t modInfo;
    if (frontEnd -> ifSwitchGetMonitorModule(modInfo)) {
        target -> setByte(IFSwitchData_t::ATTENUATION_POL0SB1, modInfo.pol0Sb1Attenuation_value);
        target -> setByte(IFSwitchData_t::ATTENUATION_POL0SB2, modInfo.pol0Sb2Attenuation_value);
        target -> setByte(IFSwitchData_t::ATTENUATION_POL1SB1, modInfo.pol1Sb1Attenuation_value);
        target -> setByte(IFSwitchData_t::ATTENUATION_POL1SB2, modInfo.pol1Sb2Attenuation_value);
        target -> setFloat(IFSwitchData_t::TEMPERATURE_POL0SB1, modInfo.pol0Sb1AssemblyTemp_value);
        target -> setFloat(IFSwitchData_t::TEMPERATURE_POL0SB2, modInfo.pol0Sb2AssemblyTemp_value);
        target -> setFloat(IFSwitchData_t::TEMPERATURE_POL1SB1, modInfo.pol1Sb1AssemblyTemp_value);
        target -> setFloat(IFSwitchData_t::TEMPERATURE_POL1SB2, modInfo.pol1Sb2AssemblyTemp_value);
        target -> setByte(IFSwitchData_t::TEMPSERVO_POL0SB1, modInfo.pol0Sb1TempServoEnable_value);
        target -> setByte(IFSwitchData_t::TEMPSERVO_POL0SB2, modInfo.pol0Sb2TempServoEnable_value);
        target -> setByte(IFSwitchData_t::TEMPSERVO_POL1SB1, modInfo.pol1Sb1TempServoEnable_value);
        target -> setByte(IFSwitchData_t::TEMPSERVO_POL1SB2, modInfo.pol1Sb2TempServoEnable_value);
        target -> setByte(IFSwitchData_t::OBSERVING_BAND, modInfo.switchCartridge_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short cryostatGetMonitor(CryostatData_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;

    CryostatImpl::Cryostat_t modInfo;
    if (frontEnd -> cryostatGetMonitorModule(modInfo)) {
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_0, modInfo.cryostatTemperature0_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_1, modInfo.cryostatTemperature1_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_2, modInfo.cryostatTemperature2_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_3, modInfo.cryostatTemperature3_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_4, modInfo.cryostatTemperature4_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_5, modInfo.cryostatTemperature5_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_6, modInfo.cryostatTemperature6_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_7, modInfo.cryostatTemperature7_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_8, modInfo.cryostatTemperature8_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_9, modInfo.cryostatTemperature9_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_10, modInfo.cryostatTemperature10_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_11, modInfo.cryostatTemperature11_value);
        target -> setFloat(CryostatData_t::CRYOSTAT_TEMP_12, modInfo.cryostatTemperature12_value);
        target -> setFloat(CryostatData_t::VACUUM_CRYOSTAT_PRES, modInfo.vacuumCryostatPressure_value);
        target -> setFloat(CryostatData_t::VACUUM_PORT_PRES, modInfo.vacuumPortPressure_value);        
        target -> setBool(CryostatData_t::BACKING_PUMP_ENABLE, modInfo.backingPumpEnable_value);
        target -> setBool(CryostatData_t::TURBO_PUMP_ENABLE, modInfo.turboPumpEnable_value);
        target -> setBool(CryostatData_t::TURBO_PUMP_STATE, modInfo.turboPumpErrorState_value);
        target -> setBool(CryostatData_t::TURBO_PUMP_SPEED, modInfo.turboPumpHighSpeed_value);
        target -> setByte(CryostatData_t::GATE_VALVE_STATE, modInfo.gateValveState_value);
        target -> setByte(CryostatData_t::SOLENOID_VALVE_STATE, modInfo.solenoidValveState_value);
        target -> setBool(CryostatData_t::VACUUM_GAUGE_ENABLE, modInfo.vacuumGaugeEnable_value);
        target -> setBool(CryostatData_t::VACUUM_GAUGE_STATE, modInfo.vacuumGaugeErrorState_value);
        target -> setFloat(CryostatData_t::SUPPLY_CURRENT_230V, modInfo.supplyCurrent230V_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short lprGetMonitor(LPRData_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;

    LPRImpl::LPR_t modInfo;
    if (frontEnd -> lprGetMonitorModule(modInfo)) {
        target -> setByte(LPRData_t::OPTICAL_SWITCH_PORT, modInfo.opticalSwitchPort_value);
        target -> setByte(LPRData_t::OPTICAL_SWITCH_SHUTTER, modInfo.opticalSwitchShutter_value);
        target -> setByte(LPRData_t::OPTICAL_SWITCH_STATE, modInfo.opticalSwitchState_value);
        target -> setByte(LPRData_t::OPTICAL_SWITCH_BUSY, modInfo.opticalSwitchBusy_value);
        target -> setFloat(LPRData_t::LPR_TEMPERATURE_0, modInfo.LPRTemperature0_value);
        target -> setFloat(LPRData_t::LPR_TEMPERATURE_1, modInfo.LPRTemperature1_value);
        target -> setFloat(LPRData_t::EDFA_LASER_PUMP_TEMPERATURE, modInfo.EDFALaserPumpTemperature_value);
        target -> setFloat(LPRData_t::EDFA_LASER_DRIVE_CURRENT, modInfo.EDFALaserDriveCurrent_value);
        target -> setFloat(LPRData_t::EDFA_LASER_PHOTODETECT_CURRENT, modInfo.EDFALaserPhotoDetectCurrent_value);
        target -> setFloat(LPRData_t::EDFA_PHOTODETECT_CURRENT, modInfo.EDFAPhotoDetectCurrent_value);
        target -> setFloat(LPRData_t::EDFA_PHOTODETECT_POWER, modInfo.EDFAPhotoDetectPower_value);
        target -> setFloat(LPRData_t::EDFA_MODULATION_INPUT, modInfo.EDFAModulationInput_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short fetimGetMonitor(FETIMData_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!FEValid)
        return -1;

    FETIMImpl::FETIM_t modInfo;
    if (frontEnd -> fetimGetMonitorModule(modInfo)) {
        target -> setFloat(FETIMData_t::INTERNAL_TEMP1, modInfo.internalTemperature1_value);
        target -> setFloat(FETIMData_t::INTERNAL_TEMP2, modInfo.internalTemperature2_value);
        target -> setFloat(FETIMData_t::INTERNAL_TEMP3, modInfo.internalTemperature3_value);
        target -> setFloat(FETIMData_t::INTERNAL_TEMP4, modInfo.internalTemperature4_value);
        target -> setFloat(FETIMData_t::INTERNAL_TEMP5, modInfo.internalTemperature5_value);
        target -> setFloat(FETIMData_t::EXTERNAL_TEMP1, modInfo.externalTemperature1_value);
        target -> setFloat(FETIMData_t::EXTERNAL_TEMP2, modInfo.externalTemperature2_value);
        target -> setFloat(FETIMData_t::AIRFLOW1, modInfo.getAirflowSensor1_value);
        target -> setFloat(FETIMData_t::AIRFLOW2, modInfo.getAirflowSensor2_value);
        target -> setFloat(FETIMData_t::HE2_PRESSURE, modInfo.heliumBufferPressure_value);
        target -> setFloat(FETIMData_t::GLITCH_VALUE, modInfo.glitchValue_value);
        target -> setBool(FETIMData_t::INTERNAL_TEMP_OOR, modInfo.internalTemperatureOOR_value);
        target -> setBool(FETIMData_t::EXTERNAL_TEMP1_OOR, modInfo.externalTemperatureOOR1_value);
        target -> setBool(FETIMData_t::EXTERNAL_TEMP2_OOR, modInfo.externalTemperatureOOR2_value);
        target -> setBool(FETIMData_t::AIRFLOW_OOR, modInfo.airflowSensorOOR_value);
        target -> setBool(FETIMData_t::HE2_PRESSURE_OOR, modInfo.heliumBufferPressureOOR_value);
        target -> setBool(FETIMData_t::SENSOR_SINGLE_FAIL, modInfo.sensorSingleFailed_value);
        target -> setBool(FETIMData_t::SENSOR_MULTI_FAIL, modInfo.sensorMultiFailed_value);
        target -> setBool(FETIMData_t::GLITCH_COUNTER_TRIG, modInfo.glitchCounterTriggered_value);
        target -> setBool(FETIMData_t::DELAY_SHUTDOWN_TRIG, modInfo.delayShutdownTriggered_value);
        target -> setBool(FETIMData_t::FINAL_SHUTDOWN_TRIG, modInfo.finalShutdownTriggered_value);
        target -> setBool(FETIMData_t::COMP_STATUS, modInfo.getCompressorStatus_value);
        target -> setBool(FETIMData_t::COMP_INTERLOCK_STATUS, modInfo.getCompressorInterlockStatus_value);
        target -> setBool(FETIMData_t::COMP_CABLE_STATUS, modInfo.getCompressorCableStatus_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;
    }
    return -1;
}


