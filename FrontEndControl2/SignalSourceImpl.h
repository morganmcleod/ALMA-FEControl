#ifndef SIGNALSOURCEIMPL_H_
#define SIGNALSOURCEIMPL_H_
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
*/

#include "FEBASE/FrontEndImplBase.h"
#include "CONFIG/cartConfig.h"
#include "CartAssembly.h"
#include "WCAImpl.h"

/// SignalSource is a FEMC module with one WCA attached.

class SignalSourceImpl : public FrontEndImplBase {
public:
    SignalSourceImpl(unsigned long channel,
                     unsigned long nodeAddress,
                     const std::string &ESN = "");
    virtual ~SignalSourceImpl();

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

    bool connected() const
      { return connected_m; }
    
// State control:
    virtual void startMonitor();
    virtual void stopMonitor();

// query enabled/observing status of cartridges:
    void queryCartridgeState();

// Functions to add and remove sub-assemblies:
    bool addCartridge(int port, WCAImpl &WCA);
    bool deleteCartridge();
    
// Cartridge configuration: 
    bool setWCAConfig(const FEConfig::WCAConfig &params);
    bool setYIGLimits(double FLOYIG, double FHIYIG);

// Functions to manage the enabled and monitoring status of the cartridges:
    bool setCartridgeOff();
    bool setCartridgeOn();
    bool getCartridgeOn() const;
    bool cartPauseMonitor(bool pause);
    
// AMBSI & ARCOM monitor points:
    void getAMBSIInfo(std::string &serialNum,
                      std::string &firmwareRev,
                      unsigned long &numErrors, 
                      unsigned long &numTransactions);
    void getFirmwareInfo(std::string &AMBSILibraryVersion,
                         std::string &FEMCFirmwareVersion,
                         std::string &FEMCFPGAVersion);
    float getAMBSITemperature() const;

// Subsystem monitor points handled by FEMC module:
    virtual unsigned char numEnabledModules();
    unsigned short powerGetMonitorNumEnabled();

// Cartridge monitor points:
    bool cartGetMonitorYTO(WCAImpl::YTO_t &target) const;
    bool cartGetMonitorPhotomixer(WCAImpl::Photomixer_t &target) const;
    bool cartGetMonitorPLL(WCAImpl::PLL_t &target) const;
    bool cartGetMonitorAMC(WCAImpl::AMC_t &target) const;
    bool cartGetMonitorPA(WCAImpl::PA_t &target) const;

// Control commands for cartridges:
    bool cartSetLockingStrategy(int strategy);
    bool cartSetLOFrequency(double freqLO, double freqFLOOG, int sbLock);
    bool cartGetLOFrequency(double &freqLO, double &freqREF) const;
    bool cartLockPLL();
    bool cartGetLocked();
    bool cartAdjustPLL(float targetCorrVoltage);
    bool cartAdjustYTO(int steps);
    bool cartNullPLLIntegrator(bool enable = false);

    bool cartGetNullPLLIntegrator() const;
    
    bool cartSetLOPowerAmps(bool enable = false,
                            const float *VDP0 = NULL, 
                            const float *VGP0 = NULL,
                            const float *VDP1 = NULL, 
                            const float *VGP1 = NULL);
                            
    bool cartGetLOPowerAmpsSetting(bool &enable,
                            float *VDP0 = NULL,
                            float *VGP0 = NULL,
                            float *VDP1 = NULL,
                            float *VGP1 = NULL);

    bool cartGetEnableLOPowerAmps() const;
    
    bool cartSetEnableLO(bool enable);
    
    bool cartSetLOPower(int pol, float percent);
    
    bool cartSetAMC(const float *VDE = NULL, const float *VGE = NULL);

    bool cartSetEnablePhotomixer(bool enable = false);

    bool cartGetEnablePhotomixer() const;

private:
    // disallow copy and assignment:
    SignalSourceImpl(const SignalSourceImpl &other);
    SignalSourceImpl &operator =(const SignalSourceImpl &other);

    // pointer to the one and only CartAssembly WCA managed by this:
    CartAssembly *cart_mp;
    int port_m;
    
    // objects for the M&C subsystems:
    bool connected_m;   ///< true if successfully connected to the FEMC module.

};

#endif /*SIGNALSOURCEIMPL_H_*/
