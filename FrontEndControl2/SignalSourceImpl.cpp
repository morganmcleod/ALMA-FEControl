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
 * implementation of the SignalSource which is a single WCA controlled
 * by an FEMC module
 * 
 *----------------------------------------------------------------------
 */

#include "SignalSourceImpl.h"
#include "logger.h"
#include "stringConvert.h"
#include "FEMCEventQueue.h"
//#include <stdio.h>
using namespace FEConfig;
using namespace std;

//---------------------------------------------------------------------------
// private constructor and destructor definitions:

SignalSourceImpl::SignalSourceImpl(unsigned long channel,
                                   unsigned long nodeAddress,
                                   const std::string &ESN)
  : FrontEndImplBase("SignalSourceFEMC"),
    connected_m(false),
    cart_mp(NULL),
    port_m(0)
{ 
    setESN(ESN);
    initialize(channel, nodeAddress);
    FEHardwareDevice::setMaxErrorCount(5);
}

SignalSourceImpl::~SignalSourceImpl() {
    deleteCartridge();
}

void SignalSourceImpl::initialize(unsigned long channel, unsigned long nodeAddress) {
    FrontEndImplBase::initialize(channel, nodeAddress);
    int retries = 20;
    while (retries && !connected_m) {
        unsigned char stat = specialGetSetupInfo();
        if (stat == 0 || stat == 5) {
            connected_m = true;
            FEHardwareDevice::resetErrorCount();
            LOG(LM_INFO) << "Connected to RF signal source on CAN" << channel
                         << " at nodeAddress=0x" << uppercase << hex << setw(2) << setfill('0') << nodeAddress << dec << setw(0)
                         << " AMBSI serialNum=" << AMBSISerialNum() << endl;
            AMBSIFirmwareRev();
            AMBSILibraryVersion();
            FEMCFirmwareVersion();
            FEMCFPGAVersion();
        } else {
            retries--;
            string msg("Waiting to connect to RF signal source. Retries=");
            msg += to_string(retries);
            LOG(LM_INFO) << msg << endl;
            SLEEP(1000);
        }
    }
    if (!connected_m)
        LOG(LM_ERROR) << "Not connected to RF signal source." << endl;
}

void SignalSourceImpl::shutdown() {
    connected_m = false;
    FrontEndImplBase::shutdown();
}

//---------------------------------------------------------------------------
// front end operations:

void SignalSourceImpl::startMonitor() {
    FrontEndImplBase::startMonitor();
    if (cart_mp)
        cart_mp -> startMonitor();
}

void SignalSourceImpl::stopMonitor() {
    if (cart_mp)
        cart_mp -> stopMonitor();
}

void SignalSourceImpl::queryCartridgeState() {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        if (FrontEndImplBase::powerEnableModule(port_m)) {
            LOG(LM_INFO) << "SignalSourceImpl::queryCartridgeState is enabled at port=" << port_m << endl;
            cart_mp -> setEnable();
        }
        cart_mp -> queryCartridgeState();
    }
}

void SignalSourceImpl::getAMBSIInfo(std::string &serialNum,
                                    std::string &firmwareRev,
                                    unsigned long &numErrors,
                                    unsigned long &numTransactions)
{
    serialNum = AMBSISerialNum_value;
    firmwareRev = AMBSIFirmwareRev_value;
    numErrors = AMBSINumErrors_value;
    numTransactions = AMBSINumTransactions_value;   
}

void SignalSourceImpl::getFirmwareInfo(std::string &_AMBSILibraryVersion,
                                       std::string &_FEMCFirmwareVersion,
                                       std::string &_FEMCFPGAVersion) {
    _AMBSILibraryVersion = AMBSILibraryVersion_value;
    _FEMCFirmwareVersion = FEMCFirmwareVersion_value;
    _FEMCFPGAVersion = FEMCFPGAVersion_value;
}

float SignalSourceImpl::getAMBSITemperature() const {
    return AMBSITemperature_value;
}

unsigned char SignalSourceImpl::numEnabledModules() {
    if (!FEHardwareDevice::isErrorStop())
        return FrontEndImplBase::numEnabledModules();
    else
        return 0;
}

unsigned short SignalSourceImpl::powerGetMonitorNumEnabled() {
    return numEnabledModules();
}

bool SignalSourceImpl::cartGetMonitorYTO(WCAImpl::YTO_t &target) const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp)
        return cart_mp -> getMonitorYTO(target);
    target.reset();
    return false;
}

bool SignalSourceImpl::cartGetMonitorPhotomixer(WCAImpl::Photomixer_t &target) const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        const WCAImpl *wca = cart_mp -> getWCA();
        if (wca)
            return wca -> getMonitorPhotomixer(target);
    }
    return false;
}

bool SignalSourceImpl::cartGetMonitorPLL(WCAImpl::PLL_t &target) const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        const WCAImpl *wca = cart_mp -> getWCA();
        if (wca)
            return wca -> getMonitorPLL(target);
    }
    return false;
}

bool SignalSourceImpl::cartGetMonitorAMC(WCAImpl::AMC_t &target) const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        const WCAImpl *wca = cart_mp -> getWCA();
        if (wca)
            return wca -> getMonitorAMC(target);
    }
    return false;
}

bool SignalSourceImpl::cartGetMonitorPA(WCAImpl::PA_t &target) const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        const WCAImpl *wca = cart_mp -> getWCA();
        if (wca)
            return wca -> getMonitorPA(target);
    }
    return false;
}

//----------------------------------------------------------------------------
// Functions to add and remove sub-assemblies:
bool SignalSourceImpl::addCartridge(int port, WCAImpl &WCA) {
    deleteCartridge();
    WCA.FEHardwareDevice::setMaxErrorCount(5);
    cart_mp = new CartAssembly("SignalSource", &WCA, NULL);
    port_m = port;
    return true;
}

bool SignalSourceImpl::deleteCartridge() {
    delete cart_mp;
    cart_mp = NULL;
    port_m = 0;
    return true;
}

//----------------------------------------------------------------------------
// Functions to create and configure cartridges:

bool SignalSourceImpl::setWCAConfig(const WCAConfig &params) {
    if (cart_mp && cart_mp -> existsWCA()) {
        cart_mp -> setWCAConfig(params);
        return true;
    } else
        return false;
}

bool SignalSourceImpl::setYIGLimits(double FLOYIG, double FHIYIG) {
    if (cart_mp && cart_mp -> existsWCA()) {
        cart_mp -> setYIGLimits(FLOYIG, FHIYIG);
        return true;
    } else
        return false;
}

//----------------------------------------------------------------------------
// Functions to manage the power and enabled/standby/observing status of the cartridges:    

bool SignalSourceImpl::setCartridgeOff() {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        LOG(LM_INFO) << "SignalSourceImpl::setCartridgeOff" << endl;
        cart_mp -> clearEnable();
        powerEnableModule(port_m, false);
        string msg("Signal source at port ");
        msg += to_string(port_m);
        msg += " is now powered OFF.";
        FEMCEventQueue::addStatusMessage(true, msg);
        return true;
    }
    return false;
}

bool SignalSourceImpl::setCartridgeOn() {
    if (!FEHardwareDevice::isErrorStop() && cart_mp) {
        LOG(LM_INFO) << "SignalSourceImpl::setCartridgeOn" << endl;
        cart_mp -> setEnable();
        powerEnableModule(port_m, true);
        string msg("Signal source at port ");
        msg += to_string(port_m);
        msg += " is now powered ON.";
        FEMCEventQueue::addStatusMessage(true, msg);
        return true;
    }
    return false;
}

bool SignalSourceImpl::getCartridgeOn() const {
    if (cart_mp)
        return cart_mp -> getEnable();
    else
        return false;
}

bool SignalSourceImpl::cartPauseMonitor(bool pause) {
    if (cart_mp) {
        cart_mp -> pauseMonitor(pause, false);
        return true;
    } else
        return false;
}

//----------------------------------------------------------------------------
// Control commands for CartAssemblies:

bool SignalSourceImpl::cartSetLOFrequency(double freqLO, double freqFLOOG, int sbLock) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetLOFrequency port=" << port_m << " freqLO= " << freqLO
            << " freqFLOOG=" << freqFLOOG << " sbLock=" << sbLock << endl;

        // the band 1 signal source has a x2 multiplier that the LO doesn't have:
        if (cart_mp -> getBand() == 1) {
            freqLO /= 2.0;
            LOG(LM_INFO) << "SignalSourceImpl::cartSetLOFrequency divide by 2=" << freqLO << endl;
        }
        return cart_mp -> setLOFrequency(freqLO, freqFLOOG, sbLock);
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetLOFrequency port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("Setting the LO frequency failed for band ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartGetLOFrequency(double &freqLO, double &freqREF) const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        bool ret = cart_mp -> getLOFrequency(freqLO, freqREF);

        // the band 1 signal source has a x2 multiplier that the LO doesn't have:
        if (cart_mp -> getBand() == 1) {
            freqLO *= 2.0;
        }
        return ret;
    } else {
        freqLO = freqREF = 0;
        return false;
    }
}

bool SignalSourceImpl::cartLockPLL() {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartLockPLL port=" << port_m << endl;
        bool ret = cart_mp -> lockPLL();
        if (!ret) {
            string msg("ERROR: Signal source at port ");
            msg += to_string(port_m);
            msg += " PLL is NOT locked.";
            FEMCEventQueue::addStatusMessage(false, msg);
            LOG(LM_ERROR) << msg << endl;
        } else {
            string msg("Signal source at port ");
            msg += to_string(port_m);
            msg += " PLL is locked.";
            FEMCEventQueue::addStatusMessage(true, msg);
            LOG(LM_INFO) << msg << endl;
        }
        return ret; 
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartLockPLL port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("Locking the PLL failed for Signal source at port ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartGetLocked() {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        WCAImpl *wca = cart_mp -> useWCA();
        return wca -> interrogateLock();
    }
    return false;
}

bool SignalSourceImpl::cartAdjustPLL(float targetCorrVoltage) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartAdjustPLL port=" << port_m << " targetCV=" << targetCorrVoltage << endl;
        bool ret = cart_mp -> adjustPLL(targetCorrVoltage);
        if (ret) {
            string msg("Signal source at port ");
            msg += to_string(port_m);
            msg += " PLL correction voltage adjusted to ";
            msg += to_string(targetCorrVoltage);
            msg += ".";
            FEMCEventQueue::addStatusMessage(true, msg);
            LOG(LM_INFO) << msg << endl;
        }
        return ret; 
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartAdjustPLL port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("Adjusting the PLL failed for signal source at port ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartAdjustYTO(int steps) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartAdjustYTO port=" << port_m << " steps=" << steps << endl;
        return cart_mp -> adjustYTO(steps);
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartAdjustYTO port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("Adjusting the YTO failed for signal source at port ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartNullPLLIntegrator(bool enable) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartNullPLLIntegrator port=" << port_m << " enable=" << enable << endl;
        return cart_mp -> nullPLLIntegrator(enable);
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartNullPLLIntegrator port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("The Zero Integrator setting failed for signal source at port ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartGetNullPLLIntegrator() const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        return cart_mp -> getNullPLLIntegrator();
    } else
        return false;
}

bool SignalSourceImpl::cartSetLOPowerAmps(bool enable,
                                      const float *VDP0, 
                                      const float *VGP0,
                                      const float *VDP1, 
                                      const float *VGP1)
{
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetLOPowerAmps port=" << port_m << " enable=" << enable
            << " VDP0=" << (VDP0 ? *VDP0 : 0) << " VGP0=" << (VGP0 ? *VGP0 : 0)
            << " VDP1=" << (VDP1 ? *VDP1 : 0) << " VGP1=" << (VGP1 ? *VGP1 : 0) << endl;
        return cart_mp -> setLOPowerAmps(enable, VDP0, VGP0, VDP1, VGP1);
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetLOPowerAmps port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("Setting the LO power amplifier bias failed for signal source at port ");
        msg += to_string(port_m);
        msg += ". The cartridge does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartGetLOPowerAmpsSetting(bool &enable,
                                                 float *VDP0,
                                                 float *VGP0,
                                                 float *VDP1,
                                                 float *VGP1)
{
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        const WCAImpl *wca = cart_mp -> getWCA();
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
    LOG(LM_INFO) << "SignalSourceImpl::cartGetLOPowerAmpsSetting port=" << port_m << " Source does not exist or is disabled." << endl;
    return false;
}

bool SignalSourceImpl::cartGetEnableLOPowerAmps() const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA())
        return cart_mp -> getLOPowerAmpsEnable();
    else
        return false;
}

bool SignalSourceImpl::cartSetEnableLO(bool enable) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        WCAImpl *wca = cart_mp -> useWCA();
        if (wca) {
            LOG(LM_INFO) << "SignalSourceImpl::cartSetEnableLO port=" << port_m << " enable=" << enable << endl;
            // call setEnableLO with setAMC=true so that if its band 3, the AMC will be turned off:
            wca -> setEnableLO(enable, true);
            return true;
        }
    }
    LOG(LM_INFO) << "SignalSourceImpl::cartSetEnableLO port=" << port_m << " Source does not exist or is disabled." << endl;
    return false;
}

bool SignalSourceImpl::cartSetLOPower(int pol, float percent) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        WCAImpl *wca = cart_mp -> useWCA();
        if (wca) {
            LOG(LM_INFO) << "SignalSourceImpl::cartSetLOPower port=" << port_m << " pol=" << pol << " percent=" << percent << endl;
            wca -> setLOPower(pol, percent);
            return true;
        }
    }
    LOG(LM_INFO) << "SignalSourceImpl::cartSetLOPower port=" << port_m << " Source does not exist or is disabled." << endl;
    return false;
}

bool SignalSourceImpl::cartSetAMC(const float *VDE, const float *VGE) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetAMC port=" << port_m << " VDE=" << (VDE ? *VDE : 0) << " VGE=" << (VGE ? *VGE : 0) << endl;
        return cart_mp -> setAMC(VDE, VGE);
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetAMC port=" << port_m << " Source does not exist or is disabled." << endl;
        string msg("Setting the AMC bias failed for signal source at port ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartSetEnablePhotomixer(bool enable) {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA()) {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetEnablePhotomixer port=" << port_m << " enable=" << enable << endl;
        return cart_mp -> setEnablePhotomixer(enable);
    } else {
        LOG(LM_INFO) << "SignalSourceImpl::cartSetEnablePhotomixer port=" << port_m << " WCA does not exist or is disabled." << endl;
        string msg("Enabling the photomixer failed for band ");
        msg += to_string(port_m);
        msg += ". The WCA does not exist or is disabled.";
        FEMCEventQueue::addStatusMessage(false, msg);
        return false;
    }
}

bool SignalSourceImpl::cartGetEnablePhotomixer() const {
    if (!FEHardwareDevice::isErrorStop() && cart_mp && cart_mp -> getEnable() && cart_mp -> existsWCA())
        return cart_mp -> getEnablePhotomixer();
    else
        return false;
}

