/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2010
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

#include "lv_wrapper_sigSrc.h"
#include "lv_wrapper.h"
#include "FEMCEventQueue.h"
#include "SignalSourceImpl.h"
#include "logger.h"
#include "iniFile.h"
#include "stringConvert.h"
#include "splitPath.h"
#include "CONFIG/ConfigProviderIniFile.h"
#include "CONFIG/ConfigManager.h"
#include "WCAImpl.h"

using namespace FEConfig;
using namespace std;

namespace FrontEndLVWrapper {
	unsigned long CANChannelSigSrc = 0;
    unsigned long nodeAddressSigSrc = 0x14;
    unsigned long facilityIdSigSrc = 40;
    unsigned long configIdSigSrc = 1;
    std::string sigSrcIniFilename;
    static SignalSourceImpl *sigSrc = NULL;
    static WCAImpl *sigSrcWCA_p = NULL;
    static bool sigSrcValid = false;
    static int connectedSigSrcClients = 0;

    SignalSourceImpl *getSignalSourceImpl()
      { return (sigSrcValid) ? sigSrc : NULL; }
};
using namespace FrontEndLVWrapper;


DLLEXPORT short sigSrcControlInit() {
    ++connectedSigSrcClients;
    if (sigSrc && sigSrcValid) {
        LOG(LM_INFO) << "sigSrcControlInit: connectedSigSrcClients=" << connectedSigSrcClients << endl;
        return 0;
    }
    if (LVWrapperInit() < 0) {
        LOG(LM_ERROR) << "sigSrcControlInit: LVWrapperInit failed." << endl;
        return -1;
    }
    LOG(LM_INFO) << "sigSrcControlInit: connectedSigSrcClients=" << connectedSigSrcClients << endl;
    try {
        // get the path where the FrontendControlDLL.ini file is located:
        string iniPath, tmp;
        splitPath(iniFileName, iniPath, tmp);
        if (iniPath.empty())
            iniPath = ".";

        // open the FrontendControlDLL.ini file:
        CIniFile configINI(iniFileName);
        configINI.ReadFile();

        // look for the item specifying a separate file for signal source config:
        tmp = configINI.GetValue("configFiles", "signalSource");
        if (tmp.empty())
            // not found.  use the FrontendControlDLL.ini file to load signal source config:
            sigSrcIniFilename = iniFileName;
        else {
            // if found, open that file to load the signal source config data from:
            sigSrcIniFilename = iniPath + "/" + tmp;
            LOG(LM_INFO) << "Using signal source configuration file '" << sigSrcIniFilename << "'" << endl;
            configINI.Clear();
            configINI.SetPath(sigSrcIniFilename);
            if (!configINI.ReadFile()) {
                LOG(LM_ERROR) << "Error reading signal source config file." << endl;
                return -1;
            }
        }

        tmp = configINI.GetValue("connection", "channelSigSrc");
        if (!tmp.empty())
            CANChannelSigSrc = from_string<unsigned long>(tmp);
        tmp  = configINI.GetValue("connection", "nodeAddressSigSrc");
        if (!tmp.empty()) {
            if (tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X'))
                tmp = tmp.substr(2);
            nodeAddressSigSrc = from_string<unsigned long>(tmp, std::hex);
        }
        LOG(LM_INFO) << "Connecting to signal source on CAN" << CANChannelSigSrc
            << " nodeAddress=" << uppercase << hex << setw(2) << setfill('0') << nodeAddressSigSrc << dec << setw(0) << endl;

        // get the facility code from the new key facilityId:
        tmp = configINI.GetValue("configuration", "facilityIdSigSrc");
        if (!tmp.empty())
            facilityIdSigSrc = from_string<unsigned long>(tmp);
        else {
            // but fall back on the old name if that doesn't work:
            tmp = configINI.GetValue("configuration", "providerCodeSigSrc");
            if (!tmp.empty())
                facilityIdSigSrc = from_string<unsigned long>(tmp);
        }

        tmp = configINI.GetValue("configuration", "configIdSigSrc");
        if (!tmp.empty())
            configIdSigSrc = from_string<unsigned long>(tmp);
        LOG(LM_INFO) << "Using configuration facilityIdSigSrc=" << facilityIdSigSrc << " configIdSigSrc=" << configIdSigSrc << endl;

    } catch (...) {
    	LOG(LM_ERROR) << "sigSrcControlInit exception loading configuration file." << endl;
    	FEMCEventQueue::addStatusMessage(false, "An exception occurred while loading the configuration file.");
        return -1;
    }
    
    short ret = -1;
    if (nodeAddressSigSrc) {
        sigSrc = new SignalSourceImpl(CANChannelSigSrc, nodeAddressSigSrc, "ESN-SigSrc");
        if (sigSrc -> connected()) {
            ret = 0;
            
        } else {
            LOG(LM_ERROR) << "sigSrcControlInit failed to connect to signal source control module." << endl;
            FEMCEventQueue::addStatusMessage(false, "Failed to connect to signal source control module.");
        }            
    }
    
    // If we are in a debugging mode where no CAN data is sent, pretend we got connected anyway:
    if (CAN_noTransmit)
        ret = 0;
    
    if (ret == 0)
        ret = sigSrcLoadConfiguration((short) configIdSigSrc);

    return ret;
}

DLLEXPORT short sigSrcControlShutdown() {
    --connectedSigSrcClients;
    LOG(LM_INFO) << "sigSrcControlShutdown: connectedSigSrcClients=" << connectedSigSrcClients << endl;
    if (connectedSigSrcClients <= 0) {
        sigSrcValid = false;
        if (sigSrc) {
            sigSrc -> stopMonitor();
            sigSrc -> shutdown();
            WHACK(sigSrc);
            LOG(LM_INFO) << "sigSrcControlShutdown: sigSrc destroyed." << endl;
        }
        WHACK(sigSrcWCA_p);
        LOG(LM_INFO) << "sigSrcControlShutdown: modules destroyed." << endl;
        LVWrapperShutdown();
    }
    return 0;
}

DLLEXPORT short sigSrcExit() {
    if (!sigSrcValid)
        return -1;
    sigSrc -> specialExitProgram(true);
    return 0;
}

DLLEXPORT short sigSrcEnableMonitoring(short enable) {
    if (!sigSrcValid)
        return -1;
    if (enable != 0) {
        sigSrc -> startMonitor();
        sigSrc -> queryCartridgeState();
    } else
        sigSrc -> stopMonitor();
    return 0;
}

DLLEXPORT short sigSrcGetNextConfiguration(short reset, short *configId, char *description) {
    static vector<Configuration::Record> configsList;
    static vector<Configuration::Record>::const_iterator nextConfig = configsList.begin();
    bool ret = true;

    if (!configId)
        LOG(LM_ERROR) << "sigSrcGetNextConfiguration: configId is NULL." << endl;
    if (!description)
        LOG(LM_ERROR) << "sigSrcGetNextConfiguration: description is NULL." << endl;

    *configId = 0;
    *description = 0x00;

    if (reset) {
        ConfigProvider *provider = new ConfigProviderIniFile(sigSrcIniFilename);
        ret = provider -> getAllConfigurations(facilityIdSigSrc, configsList);
        nextConfig = configsList.begin();
        delete provider;
    } else
        nextConfig++;

    if (!ret)
        LOG(LM_ERROR) << "sigSrcGetNextConfiguration: got no configs." << endl;

    if (ret) {
        if (nextConfig == configsList.end()) {
            ret = false;
        } else {
            *configId = (*nextConfig).configId_m;
            strcpy(description, (*nextConfig).description_m.c_str());
            LOG(LM_INFO) << "sigSrcGetNextConfiguration: " << description << endl;
            ret = true;
        }
    }
    return ret ? 0 : -1;
}

DLLEXPORT short sigSrcLoadConfiguration(short configId) {

    if (configId <= 0)
        configId = configIdSigSrc;

	ConfigProvider *provider(NULL);
    provider = new ConfigProviderIniFile(sigSrcIniFilename);
	
    ConfigManager configMgr(*provider);
    Configuration config(facilityIdSigSrc, configId);
    if (!config.load(*provider))
        return -1;

    delete provider;
    
    const CartAssemblyConfig *caConfig = config.getCartAssemblyConfig();
    if (caConfig) {
        const WCAConfig &wcaConfig = caConfig -> WCA_m;
        int wcaBand = wcaConfig.band_m;
        char name[30];
        int port(caConfig -> Id_m.port_m);
        if (port < 1 || port > 10) {
            LOG(LM_ERROR) << "sigSrcLoadConfiguration error: out of range port=" << port << endl;
            return -1;

        } else if (wcaBand < 1 || wcaBand > 10) {
            LOG(LM_ERROR) << "sigSrcLoadConfiguration error: out of range band=" << wcaBand << endl;
            return -1;

        } else {
            LOG(LM_INFO) << "sigSrcLoadConfiguration: port=" << port << " wcaBand=" << wcaBand << endl;
            sprintf(name, "band %d Signal Source", wcaBand);
            if (sigSrcWCA_p) {
                sigSrc -> deleteCartridge();
                WHACK(sigSrcWCA_p);
            }
            sigSrcWCA_p = new WCAImpl(CANChannelSigSrc, nodeAddressSigSrc, name, port, wcaBand, wcaConfig.ESN_m);
            sigSrc -> addCartridge(port, *sigSrcWCA_p);
        }
    }
    configMgr.configure(config, *sigSrc);
    sigSrcValid = true;
    LOG(LM_INFO) << "sigSrcLoadConfiguration successful" << endl;
    return 0;
}

DLLEXPORT short sigSrcGetBand(short *band_p) {
    if (!band_p) {
        LOG(LM_ERROR) << "sigSrcGetBand: band_p is NULL." << endl;
        return -1;
    }

    if (!sigSrcWCA_p) {
        LOG(LM_ERROR) << "sigSrcGetBand: no source is configured." << endl;
        return -1;
    }

    (*band_p) = (short) sigSrcWCA_p -> getBand();
    LOG(LM_INFO) << "sigSrcGetBand: returning band " << (*band_p) << "." << endl;
    return 0;
}

DLLEXPORT short sigSrcGetAMBSIInfo(char *_serialNum,
                                   char *_firmwareRev,
                                   unsigned long *numErrors,
                                   unsigned long *numTransactions)
{
    if (!sigSrcValid || !_serialNum || !_firmwareRev || !numErrors || !numTransactions)
        return -1;

    std::string serialNum;
    std::string firmwareRev;
    sigSrc -> getAMBSIInfo(serialNum, firmwareRev, *numErrors, *numTransactions);
    strcpy(_firmwareRev, firmwareRev.c_str());
    strcpy(_serialNum, serialNum.c_str());
    return 0;
}

DLLEXPORT short sigSrcGetAMBSITemperature(float *temperature) {
    if (!sigSrcValid || !temperature)
        return -1;
    *temperature = sigSrc -> getAMBSITemperature();
    return 0;
}

DLLEXPORT short sigSrcGetFirmwareInfo(char *_AMBSILibraryVersion,
                                      char *_FEMCFirmwareVersion,
                                      char *_FEMCFPGAVersion)
{
    if (!sigSrcValid || !_AMBSILibraryVersion || !_FEMCFirmwareVersion)
        return -1;
    std::string AMBSILibraryVersion,
                FEMCFirmwareVersion,
                FEMCFPGAVersion;
    sigSrc -> getFirmwareInfo(AMBSILibraryVersion, FEMCFirmwareVersion, FEMCFPGAVersion);
    strcpy(_AMBSILibraryVersion, AMBSILibraryVersion.c_str());
    strcpy(_FEMCFirmwareVersion, FEMCFirmwareVersion.c_str());
    strcpy(_FEMCFPGAVersion, FEMCFPGAVersion.c_str());
    return 0;
}

DLLEXPORT short sigSrcGetNumCartridgesOn() {
    if (!sigSrcValid)
        return -1;
    return (short) sigSrc -> powerGetMonitorNumEnabled();
}




