#ifndef FRONTEND_H_
#define FRONTEND_H_
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
*/

#include "FEBASE/FrontEndImplBase.h"
#include "OPTIMIZE/ThermalLogger.h"
#include "CONFIG/cartConfig.h"
#include "CartAssembly.h"
#include "ColdCartImpl.h"
#include "CryostatImpl.h"
#include "FETIMImpl.h"
#include "IFSwitchImpl.h"
#include "LPRImpl.h"
#include "PowerModuleImpl.h"
#include "WCAImpl.h"
#include <stdint.h>
#include <vector>

class CartridgesContainer;  
class IFPowerDataSet;
class FrontEndDatabase;
class PowerModulesContainer;

/// The top-level FrontEnd class, giving the entire control interface for the Front End.

class FrontEndImpl : public FrontEndImplBase, public ThermalLoggable {
public:
    FrontEndImpl(unsigned long channel, 
                 unsigned long nodeAddress, 
                 const std::string &ESN = "");
    virtual ~FrontEndImpl();

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

    bool connected() const
      { return connected_m; }

// Debugging options:
    static void correctSISVoltageError(bool doCorrect)
      { correctSISVoltageError_m = doCorrect; }
    ///< enable/disable correcting SIS bias setting error at cartridge power-up.

// FEMC error reporting:
    bool getNextError(unsigned char &moduleNum, unsigned char &errorNum, std::string &description);
    ///< fetch the next error from the FEMC error queue.
    ///<    if none waiting returns false.
    ///<    else returns true and populates moduleNum, errorNum, and description with info.

// State control:
    virtual void setFEMode(unsigned char val);
    virtual void startMonitor();
    virtual void stopMonitor();

// Health check:
    bool getDbConfigId(FEICDataBase::ID_T &onfigId) const;
    bool startHealthCheck(short dataStatus);
    bool cartHealthCheck(int port, int warmUpTimeSeconds, bool includeIFPower);
    bool cartHealthCheckSaveIFPowerData(int port, const IFPowerDataSet &source);
    bool finishHealthCheck();
    bool existsCartAssembly(int port);

// query enabled/observing status of powermodules and cartridges:
    void queryCartridgeState();

// Configuration functions:
    void setFrontEndSN(const std::string &SN);

// Functions to add and remove sub-assemblies:
    bool addCartridge(int port, WCAImpl &WCA, ColdCartImpl &coldCart);
    bool addCartridge(int port, WCAImpl &WCA);
    bool addCartridge(int port, ColdCartImpl &coldCart);
    bool deleteCartridge(int port);
    
    bool addPowerModule(PowerModuleImpl &module);
    bool deletePowerModule(int port);

    void addCryostat();
    void deleteCryostat();

    void addFETIM();
    void deleteFETIM();

    void addIFSwitch();
    void deleteIFSwitch();

    void addLPR();
    void deleteLPR();

    void addCPDS();
    void deleteCPDS();

// Cartridge configuration: 
    bool setColdCartConfig(int port, const FEConfig::ColdCartConfig &params);
    bool setWCAConfig(int port, const FEConfig::WCAConfig &params);
    bool setYIGLimits(int port, double FLOYIG, double FHIYIG);

// Functions to manage the enabled/standby/observing status of the cartridges:    
    bool setCartridgeOff(int port);
    bool setCartridgeOn(int port);
    bool setCartridgeStandby2(int port, bool enable);
    bool setCartridgeObserving(int port);
    void clearCartridgeObserving();
    int getCartridgeObserving() const;
    bool getCartridgeOn(int port) const;
    bool cartPauseMonitor(int port, bool pauseWCA, bool pauseCC);        
    
// AMBSI & ARCOM monitor points:
    virtual unsigned char specialGetSetupInfo();

    void getAMBSIInfo(std::string &serialNum,
                      std::string &firmwareRev,
                      unsigned long &numErrors, 
                      unsigned long &numTransactions);
    void getFirmwareInfo(std::string &AMBSILibraryVersion,
                         std::string &FEMCFirmwareVersion,
                         std::string &FEMCFPGAVersion);
    float getAMBSITemperature() const;

// Subsystem monitor points handled by FrontEnd:
    unsigned short powerGetMonitorNumEnabled();

// Cartridge monitor points:
    bool cartGetMonitorYTO(int port, WCAImpl::YTO_t &target) const;
    bool cartGetMonitorPhotomixer(int port, WCAImpl::Photomixer_t &target) const;
    bool cartGetMonitorPLL(int port, WCAImpl::PLL_t &target) const;
    bool cartGetMonitorAMC(int port, WCAImpl::AMC_t &target) const;
    bool cartGetMonitorPA(int port, WCAImpl::PA_t &target) const;
    bool cartGetMonitorTemp(int port, ColdCartImpl::CartridgeTemp_t &target) const;
    bool cartGetMonitorSIS(int port, int pol, int sb, ColdCartImpl::SIS_t &target) const;
    bool cartGetMonitorLNA(int port, int pol, int sb, ColdCartImpl::LNA_t &target) const;
    bool cartGetMonitorAux(int port, int pol, ColdCartImpl::Aux_t &target) const;

// Control commands for cartridges:
    bool cartSetLOFrequency(int port, double freqLO, double freqFLOOG, int sbLock);
    bool cartSetCenterLOFrequency(int port, double freqFLOOG, int sbLock);
    bool cartGetLOFrequency(int port, double &freqLO, double &freqREF) const;
    bool cartLockPLL(int port);
    bool cartGetLocked(int port);
    bool cartAdjustPLL(int port, float targetCorrVoltage);
    bool cartAdjustYTO(int port, int steps);
    bool cartNullPLLIntegrator(int port, bool enable = false);

    bool cartGetNullPLLIntegrator(int port) const;
    
    bool cartSetLNABias(int port, int pol = -1, int sb = -1, 
                        const float *VD1 = NULL, 
                        const float *ID1 = NULL,
                        const float *VD2 = NULL, 
                        const float *ID2 = NULL,
                        const float *VD3 = NULL, 
                        const float *ID3 = NULL);

    bool cartSetEnableLNABias(int port, bool enable = false, int pol = -1, int sb = -1);

    bool cartGetEnableLNABias(int port) const;
    
    bool cartSetSISBias(int port, bool enable = false, int pol = -1, int sb = -1, 
                        const float *VJ = NULL, int openLoop = -1);

    bool cartGetEnableSISBias(int port) const;
    
    bool cartSetSISMagnet(int port, bool enable = false, int pol = -1, int sb = -1, 
                          const float *IMag = NULL);
    
    bool cartGetEnableSISMagnet(int port) const;
    
    bool cartGetSISMagnet(short port, int pol, int sb, float *IMag);
    
    bool cartSetLOPowerAmps(int port, bool enable = false,
                            const float *VDP0 = NULL, 
                            const float *VGP0 = NULL,
                            const float *VDP1 = NULL, 
                            const float *VGP1 = NULL);

    bool cartGetLOPowerAmpsSetting(int port, bool &enable,
                            float *VDP0 = NULL,
                            float *VGP0 = NULL,
                            float *VDP1 = NULL,
                            float *VGP1 = NULL);
                            
    bool cartGetEnableLOPowerAmps(int port) const;
    
    bool cartAdjustLOPowerAmps(int port, int repeatCount = 1);

    bool cartSetEnableLO(int port, bool enable);
    
    bool cartSetLOPower(int port, int pol, float percent);
    
    bool cartOptimizeIFPower(int port, bool doPol0, bool doPol1, float VDstart0, float VDstart1);

    bool cartClearOptimizedResult(short port);

    bool cartGetOptimizedResult(int port, std::string &mixerParamsText);

    bool cartSetIFPower(int port, int pol, float powerSB1, float powerSB2);
    ///< Inject the current IF power level readings for a single port and polarization.
    ///< Used for optimizing mixer and LO power amp bias voltages.
                               
    bool cartSetVJVD(int port, int pol, float *VJ1, float *VJ2, float *VD);

    bool cartGetVJVD(int port, int pol, float *VJ1, float *VJ2, float *VD);

    bool cartSetAMC(int port, const float *VDE = NULL, const float *VGE = NULL);                        

    void setFineLOSweepIniFile(const std::string FineLoSweepIni)
      { CartAssembly::setFineLOSweepIniFile(FineLoSweepIni); }

    bool cartMeasureFineLOSweep(int port, float tiltAngle,
                                int pol = -1,
                                const float *VJ = NULL,
                                const float *IJ = NULL,
                                const float *fixedVD = NULL,
                                const double *LOStart = NULL,
                                const double *LOStop = NULL,
                                const double *LOStep = NULL,
                                int repeatCount = 1);
    
    bool cartMeasureIVCurve(int port, int pol, int sb, 
                            const float *VJlow_p = NULL, 
                            const float *VJhigh_p = NULL, 
                            const float *VJstep_p = NULL,
                            int repeatCount = 1);
                        
    bool cartMeasureIJvsSISMagnet(int port, int pol, int sb, 
                                  float IMagStart, float IMagStop, float IMagStep,
                                  float VJLow, float VJHigh, float VJStep,
                                  int repeatCount = 1);
                        
    bool cartMeasureIFPowerVsVJ(int port, int pol, int sb, 
                                float VJStart, float VJStop, float VJStep, 
                                bool doYFactor = false, int repeatCount = 1);
    
    bool cartMeasureIFPowerVsVD(int port, int pol, 
                                float VDStart, float VDStop, float VDStep, 
                                bool doYFactor = false, int repeatCount = 1);
    
    bool cartMixerDeflux(int port, int pol, int sb, int IMagMax);
    
    void cartAbortMeasurement(int port);

    const XYPlotArray &cartGetXYData(int port) const;                         
    
    bool cartSetEnableLNALEDs(int port, bool enable = false);
    
    bool cartSetEnableSISHeaters(int port, bool enable = false, int pol = -1, float targetTemp = 12.0, int timeout = 30);

    bool cartSetEnablePhotomixer(int port, bool enable = false);

    bool cartGetEnablePhotomixer(int port) const;
    
// Power distribution monitor and control points:
    bool powerGetMonitorModule(int port, PowerModuleImpl::PowerModule_t &target) const;
    bool powerSetEnableModule(int port, bool enable = false);    

// IF switch monitor and control points:
    bool ifSwitchGetMonitorModule(IFSwitchImpl::IFSwitch_t &target) const;
    
    bool ifSwitchSetAttenuation(int pol = -1, int sb = -1, unsigned char atten = 15); 

    bool ifSwitchSetEnableTempServo(int pol = -1, int sb = -1, bool enable = false);

// Cryostat monitor and control points:
    bool cryostatGetMonitorModule(CryostatImpl::Cryostat_t &target) const;
    
    bool cryostatSetEnableBackingPump(bool enable);
    ///< Start/stop the backing pump.
    
    bool cryostatSetEnableTurboPump(bool enable);
    ///< Start/stop the turbo pump.
    
    bool cryostatSetGateValveState(bool open);
    ///< Open/close the gate valve.
    
    bool cryostatSetSolenoidValveState(bool open);
    ///< Open/close the solenoid valve.
    
    bool cryostatSetEnableVacuumGauge(bool enable);
    ///< Enable/disable the vacuum gauge.

    bool cryostatSetEnableCryoPumping(bool enable);
    ///< Enable/disable the pump-down process.

// FE Thermal Interlock Module monitor and control points:
    bool fetimGetMonitorModule(FETIMImpl::FETIM_t &target) const;

    bool fetimSetTriggerDewarN2Fill(bool enable);
    ///< Enable/disable the N2 fill trigger.

// LO photonic receiver monitor and control points:
    bool lprGetMonitorModule(LPRImpl::LPR_t &target) const;

    bool lprSetOpticalSwitchPort(int port);
    ///< Set the optical switch output port and disable the shutter.
    
    bool lprSetOpticalSwitchShutter();
    ///< Enables the shutter (disables the laser).
    
    bool lprSetOpticalSwitchForceShutter();
    ///< Enables the shutter (disables the laser) more forcefully (for debugging).
    
    bool lprSetEDFAModulationInput(float value);
    ///< Sets the EDFA modulation input value in Volts.
    
// Thermal logging settings:
    
    void setThermalLogInterval(unsigned int interval);
    ///< set the frequency of thermal logging, in seconds.
    
    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string

    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string

public:
    virtual void setLogMonTimers(bool val)
      { logMonTimers_m = val; }

protected:
    virtual void postMonitorHook(const AmbRelativeAddr &RCA);
    ///< called by base class after each monitor operation
    
private:
    // disallow copy and assignment:
    FrontEndImpl(const FrontEndImpl &other);
    FrontEndImpl &operator =(const FrontEndImpl &other);

    // private helper for logging and error reporting:
    void reportBadCartridge(int port, std::string where, std::string msg = std::string());

    // private helper for measuring SIS voltage setting offset:
    static bool correctSISVoltageError_m;
    void measureSISVoltageError(int port);

    // configuration data for the front end:
    unsigned SN_m;      ///< this front end's serial number, numeric portion only.
    std::string ESN_m;  ///< this front end's ESN (can serial number)

    // class to contain cartridge configuration and CartAssembly objects:
    CartridgesContainer *carts_mp;
    
    // class to contain power distribution objects:
    PowerModulesContainer *powerMods_mp;

    // database access:
    FrontEndDatabase *dbObject_mp;

    // objects for the M&C subsystems:
    CryostatImpl *cryostat_mp;
    FETIMImpl *fetim_mp;
    IFSwitchImpl *ifSwitch_mp;
    LPRImpl *lpr_mp;
    bool cpds_m;        ///< true if the CPDS is present.
    bool connected_m;   ///< true if successfully connected to the FE.

    // thermal logger object:
    ThermalLogger *thermalLogger_mp;
    unsigned int thermaLogInterval_m;

    // objects for FE health check:
    bool hcStarted_m;
    bool hcReceiverIsCold_m;
    int hcFacility_m;
    int hcDataStatus_m;

    // monitor timing statistics:
    bool logMonTimers_m;
    std::vector<unsigned short> monTimers_m;
    std::vector<unsigned> monTimersMin_m;
    std::vector<unsigned> monTimersMax_m;
    unsigned monTimersCount_m;
    unsigned short maxTimerValue_m;
};

#endif /*FRONTEND_H_*/
