#ifndef CARTASSEMBLY_H_
#define CARTASSEMBLY_H_
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

#include "ColdCartImpl.h"
#include "WCAImpl.h"
#include "LockingStrategy.h"
#include "CONFIG/FrontEndConfig.h"
#include "CONFIG/FrontEndDatabase.h"
#include "OPTIMIZE/ThermalLoggable.h"

class CartHealthCheck;
class LPRImpl;
class MaximizeIFPower;
class MeasureFineLOSweep;
class MeasureIVCurve;
class MeasureSISCurrent;
class MeasureSISVoltageError;
class MeasureIFPower;
class MixerDeflux;
class MixerHeating;
class OptimizeLOPowerAmp;
class XYPlotArray;

/// The class representing a generic cartridge assembly, its configuration,
/// and operations which may coordinate action between the CCA and the WCA.
/// All cartridge operations are implemented here or in the constituent ColdCartImpl or WCAImpl objects.
/// Cartridge behavior which varies from band-to-band is discriminated here.

class CartAssembly : public ThermalLoggable {
public:
    CartAssembly(const std::string &name, WCAImpl *WCA, ColdCartImpl *coldCart);
    ///< construct with a name and providing WCA and coldCart pointers to use.
    ///< either WCA or coldCart may be NULL but not both.

    ~CartAssembly();

    void reset();
    ///< set all state data to defaults, as if just constructed.

    void setLockingStrategy(FEConfig::WCAConfig::LOCK_STRATEGY_OPTS strategy, bool allowEdfaAdjust = true);
    ///< Set the locking strategy to use.
    ///< allowEdfaAdjust is normally true for WCAs in the front end but should be false for RF signal sources

// Test/get WCA and CCA children:
    bool existsWCA() const
      { return WCA_mp != NULL; }
    bool existsColdCart() const
      { return coldCart_mp != NULL; }

    const WCAImpl *getWCA() const
      { return WCA_mp; }
    const ColdCartImpl *getColdCart() const
      { return coldCart_mp; }

    WCAImpl *useWCA() const
      { return WCA_mp; }
    ColdCartImpl *useColdCart() const
      { return coldCart_mp; }
    
    int getBand() const
      { return band_m; }

    const std::string &getBandText(std::string &toFill) const;
    
    void queryCartridgeState();
    ///< Uses monitor requests to synchronize state variables to the FE hardware state.

    void measureSISVoltageError();
    ///< find and store the offset between the set and readback SIS voltage.
    
// State control:
    void setEnable();
    ///< Power the cartridge assembly on, start monitoring, etc.    
    
    void clearEnable();
    ///< Power the cartridge off, stop monitoring, etc.

    bool getEnable() const
      { return enable_m; }

    void setObserving();
    ///< set this cartridge assembly to be the observing band.
    ///< Has side-effects of setting the YTO to its last observing value and enabling the PLL.
    
    void clearObserving();
    ///< set this cartridge assembly to be a non-observing band.
    ///< Has side-effects of setting the YTO to an extreme tuning and disabling the PLL. 
    
    bool getObserving() const
      { return observing_m; }

    void startMonitor();
    ///< start the monitor threads for the WCA and ColdCart.
    
    void stopMonitor();
    ///< stop the monitor threads for the WCA and ColdCart.
    
    void pauseMonitor(bool pauseWCA, bool pauseCC, const char *reason = NULL);
    ///< temporarily pause/unpause monitoring for the WCA and/or ColdCart 
    ///< This can used when some communication-intensive operation is in progress or to 
    ///< prevent monitoring while observing.  Optional reason string will be logged.

// Set/get configuration:

    void setColdCartConfig(const FEConfig::ColdCartConfig &params);
    ///< set all configuration database parameters for the cold cartridge (SIS and LNAs)
    
    void setWCAConfig(const FEConfig::WCAConfig &params);
    ///< set all configuration database parameters for the WCA (PLL, AMC, Power Amp, etc.)
    ///< includes the YIG oscillator limits.
    
    void setYIGLimits(double FLOYIG, double FHIYIG);   
    ///< set just the YIG oscillator lower and upper limits in GHz.

    const FEConfig::ColdCartConfig &getColdCartConfig()
      { return config_m.coldCart_m; }
    ///< get the cold cartridge configuration.

    const FEConfig::WCAConfig &getWCAConfig()
      { return config_m.WCA_m; }
    ///< get the WCA configuration.

// LO tuning:

    bool setLOFrequency(double freqLO, double freqFLOOG, int sbLock);
    ///< Set the YTO tuning according to the given LO reference & floog frequencies, all in GHz.
    ///< sbLock = 0 means lock below the reference:  freqREF = freqLO + freqFLOOG;
    ///< sbLock = 1 means lock above the reference:  freqREF = freqLO - freqFLOOG.
    
    bool overrideLoopBW(bool altLoopBW);
    ///< set the loop bandwidth to the value given, rather than the normal value for the current band.

    bool getLOFrequency(double &freqLO, double &freqREF) const
      { freqLO = freqLO_m; 
        freqREF = freqREF_m;
        return true; }
    ///< Get the last set LO frequency and the corresponding expected reference frequency.
      
    bool getLOLockSettings(double &freqFLOOG, int &sbLock) const
      { freqFLOOG = freqFLOOG_m;
        sbLock = (existsWCA() ? WCA_mp -> getPLLSidebandLockSelectSetting() : 0);
        return true; }
    ///< Get the last set FLOOG frequency and sbLock setting.
    
    static double getCenterLOFrequency(int band)
      { static double freqs[11] = {
            0.0,    // band 0: undefined
            35.0,	// band 1
            80.0,
            100.0,  // band 3
            145.0,
            187.0,  // band 5
            241.0,
            323.0,  // band 7
            440.0,
            662.0,  // band 9
            874.0
        }; return freqs[band]; }
    ///< get the center LO frequency for the specified band.  Used for warm-up and health check.

    bool getMonitorYTO(WCAImpl::YTO_t &target) const;
    ///< Get the monitor data for the YTO:  coarse tuning and equivalent frequency in GHz.

    bool lockPLL();
    ///< initiate the lock search process.   setLOFrequency must have previously succeeded.
    
    bool adjustPLL(float targetCorrVoltage);
    ///< Adjust the YTO to get the correction voltage as close as possible to the given target voltage.
    
    bool adjustYTO(int steps);
    ///< Adjust the YTO upward or downward as specified by 'steps'.
    
    bool nullPLLIntegrator(bool enable = false);
    ///< Enable/disable the LO_PLL_NULL_LOOP_INTEGRATOR.
    ///< when true, the PLL is defeated and the YTO correction voltage set to zero.
    ///< when false, the PLL operates.

    bool getNullPLLIntegrator() const;
    ///< return the state of the LO_PLL_NULL_LOOP_INTEGRATOR

    int getCoarseYIG(double &freqYIG, double &freqREF,
                     double freqLO, double freqFLOOG, int sbLock) const;
    ///< Compute the coarse YIG tuning given freqLO, freqFLOOG, and sbLock.
    ///< uses the lower and upper YIG limits and multAMC.
    ///< returns the resulting YIG frequency and expected reference frequency as freqYIG and freqREF.

// Other cold cartridge bias:

    bool setLNABias(int pol = -1, int sb = -1, 
                    const float *VD1 = NULL, 
                    const float *ID1 = NULL,
                    const float *VD2 = NULL, 
                    const float *ID2 = NULL,
                    const float *VD3 = NULL, 
                    const float *ID3 = NULL);
    ///< Set the LNA bias for a given pol and sb or for all LNAs simultaneously.
    ///< If any of the float values given is NULL the corresponding setting is used 
    ///<    from the configuration database. 
    ///< If either pol or sb is -1, then all float values are ignored and the values 
    ///<    from the config database are used for all LNAs in the cartridge.
                    
    bool setEnableLNABias(bool enable = false, int pol = -1, int sb = -1);
    ///< Enable or disable the LNA for a given pol and sb or for all LNAs simultaneously.
    ///< If either pol or sb is -1, all LNAs are simultaneously affected.

    bool getEnableLNABias() const;
    ///< Return true if the LNAs are enabled -- for updating the GUI.
    
    bool setSISBias(bool enable = false, int pol = -1, int sb = -1, 
                    const float *VJ = NULL, int openLoop = -1);
    ///< Enable/disable and set the SIS bias for a given pol and sb or for all SIS mixers simultanously.
    ///< If VJ is NULL the setting from the configuration database is used.
    ///< If openLoop is -1, no change is made to the SIS openLoop state.
    ///< If either pol or sb is -1, then all junctions are biased according to the database.

    static void sweepSISVoltage(bool doSweep)
      { sweepSISVoltage_m = doSweep; }
    ///< enable/disable for bands 7 and up, gradually sweep SIS voltage to target value

    bool getEnableSISBias() const;
    ///< Return true if the SIS bias is enabled -- for updating the GUI.

    bool setSISMagnet(bool enable = false, int pol = -1, int sb = -1, 
                      const float *IMag = NULL);
    ///< Enable/disable and set the SIS magnet bias for a given pol and sb or for all SIS magnets simultanously.
    ///< If IMag is NULL the setting from the configuration database is used.
    ///< If either pol or sb is -1, then all magnets are biased according to the database.

    static void sweepMagnetCurrent(bool doSweep)
      { sweepMagnetCurrent_m = doSweep; }
    ///< enable/disable gradually sweep SIS magnet current to target value

    bool getEnableSISMagnet() const;
    ///< Return true if the SIS magnet is enabled -- for updating the GUI.
    
    bool getSISMagnet(int pol, int sb, float *IMag);
    ///< Get the SIS magnet bias current control for a given pol and sb.
    
    bool getSISMagnetNominal(float &IMag01, float &IMag02, float &IMag11, float &IMag12) const;
    ///< Get the nominal SIS magnet current values from the configuration database for the currently set LO frequency.

// LO & Power Amp settings:

    bool setLOPowerAmps(bool enable = false,
                        const float *VDP0 = NULL, 
                        const float *VGP0 = NULL,
                        const float *VDP1 = NULL, 
                        const float *VGP1 = NULL);
    ///< Enable/disable and set the LO power amps.
    ///< If any of the float values given is NULL no change is made to the corresponding control point.
    ///< If all the float values are NULL all settings are made according to the configuration database. 

    bool getLOPowerAmpsEnable() const;
    ///< Return the enable state of the LO PA.
    
    bool adjustLOPowerAmps(int repeatCount = 1);
    ///< Adjust the LO PA drain voltages until the junction currents are as close as possible to 
    ///< the the target values given in the configuration database.
    ///< Adjustment happens on a worker thread.
    ///< Sends the event EVENT_PA_ADJUST_DONE via the client app event queue when finished.

    bool adjustLOPowerAmpsIsBusy() const;
    ///< Returns true if the asynchronous process adjustLOPowerAmps() is still running.
    ///< Returns false otherwise or if the process has not been called.

    bool adjustLOPASinglePol(int pol, float &VD, float targetIJ1, bool verboseLogging);
    ///< Adjust the LO PA drain voltage for the specified pol until the SIS1 junction current is close to targetIJ1.
    ///< Starting value is given and final value is returned in VD.
    ///< Adjustment happens on the calling thread.
    
    bool optimizeIFPower(bool doPol0, bool doPol1, float VDstart0, float VDstart1);
    ///< Perform optmization of IF power vs. LO drive and junction voltage.
    ///< works in conjunction with setIFPower() which injects ongoing power readings.
    ///< Optimization happens on a worker thread.
    ///< Sends the event EVENT_REQUEST_IFPOWER via the client app event queue power readings are required.
    ///< Sends the event EVENT_PA_ADJUST_DONE when finished.
    
    bool clearOptimizedResult();
    ///< Clear out tables accumulating results from cartOptimizeIFPower for the given port.

    bool getOptimizedResult(std::string &mixerParamsText);
    ///< Get the optimized MixerParams as text from recent calls to optimizeIFPower.
    ///<    since the last call to clearOptimizedResult().
    ///< Returns a text section of the collectd CCA MixerParams.

    bool setIFPower(int pol, float powerSB1, float powerSB2);
    ///< Inject the current IF power level readings for a single polarization.
    ///< Used for optimizing mixer and LO power amp bias voltages.

    bool setVJVD(int pol, float *VJ1, float *VJ2, float *VD);
    ///< Set the junction voltages and/or drain voltage for the given polarization.
    ///< If any of VJ1, VJ2, or VD is NULL, no change is made to the corresponding control point.

    bool getVJVD(int pol, float *VJ1, float *VJ2, float *VD);
    ///< Read the junction control voltages and/or drain voltage for the given polarization.
    ///< The existing control values are returend as VJ1, VJ2, and VD.
    
    bool setAMC(const float *VDE = NULL, const float *VGE = NULL);                        
    ///< Set or reset the AMC digital pots.
    ///< If both VDE and VGE are NULL then both are set to nominal (1.0, -0.1).
    ///< If VDE is not NULL then its value is set otherwise no change.
    ///< If VGE is not NULL then its value is set otherwise no change.
    ///< Note: these control points do not work for most cartridges.  They are used for the band 3 signal source.

// bias optimization and other measurements:

    static void setFineLOSweepIniFile(const std::string FineLoSweepIni)
      { FineLoSweepIni_m = FineLoSweepIni; }

    bool measureFineLOSweep(FrontEndDatabase &dbObject, const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus, float tiltAngle,
                            int pol = -1,
                            const float *VJ = NULL,
                            const float *IJ = NULL,
                            const float *fixedVD = NULL,
                            const double *LOStart = NULL,
                            const double *LOStop = NULL,
                            const double *LOStep = NULL,
                            int repeatCount = 1);
    ///< For the given pol, set both SIS junction voltages to VJ;
    ///< At each LO frequency from LOStart to LOStop by LOStep:
    ///<  adjust the LO PA drain voltage to get junction current IJ
    ///<  -or-
    ///<  set LO PA drain voltage to fixedVD if specified.
    ///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
    ///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent when finished.
    ///< pol may be -1, indicating both pols are to be measured.
    ///< If fixedVD is not NULL then use that VD setting and ignore IJ.
    ///< If either of VJ or IJ are NULL then the settings from FineLOSweep.ini are used.
    ///< If any of LOStart, LOStop, or LOStep are NULL, then the settings from FineLOSweep.ini are used.
    
    bool measureFineLOSweepSingleSynchronous(int pol, float VJ, float IJ, float fixedVD, float VDNom, double LOStart, double LOStop, double LOStep,
            XYPlotArray &sisCurrents, XYPlotArray &loPaVoltages);
    ///< Measure Fine LO sweep for a single polarization.  Place results in the given XYPlotArray objects.
    ///< runs on the calling thread.

    bool measureIVCurve(int pol, int sb, 
                        const float *VJlow_p = NULL, 
                        const float *VJhigh_p = NULL, 
                        const float *VJstep_p = NULL,
                        int repeatCount = 1);
    ///< Take an I-V curve for the given  pol, and sb.
    ///< pol may be -1, indicating both pols are to be measured.
    ///< sb  may be -1, indicating both sbs are to be measured.
    ///< If VJlow, VJhigh, or VJstep is NULL it will be computed from the nominal VJ.
    ///< Measurement happens on a worker thread.
    ///< Sends the event EVENT_IVCURVE_DONE via the client app event queue when finished.

    bool measureIVCurveSingleSynchronous(int pol, int sb, float VJlow, float VJhigh, float VJstep);
    ///< Measure the I-V curve once in a synchronous fashion -- not on the worker thread.
    ///< Sends the event EVENT_IVCURVE_DONE via the client app event queue when finished.

    bool getIVCurveDefaults(int pol, int sb, float *VJlow_p, float *VJhigh_p, float *VJstep_p);
    ///< get the default I-V curve range values appropriate for this cartridge band.

    bool measureIJvsSISMagnet(int pol, int sbMixer,
                              float IMagStart, float IMagStop, float IMagStep,
                              float VJLow, float VJHigh, float VJStep,
                              int repeatCount = 1);
    ///< For the given pol, and sbMixer, step the magnet current from IMagStart to IMagStop by IMagStep.
    ///< At each step, sweep the mixer voltage from VJLow to VJHigh by VJStep,
    ///<  if VJMin==VJMax, record the mixer junction current at each magnet step.
    ///<  else, record half the difference between the max and min junction current seen.
    ///< If sbMixer is -1, measure both mixers while stepping both magnets (or band-specific special cases.)
    ///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
    ///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent when finished.                        

    bool measureIFPowerVsVJ(int pol, int sb, const std::string &description,
                            float VJStart, float VJStop, float VJStep, 
                            bool doYFactor = false, int repeatCount = 1);
    ///< For the given pol, measure the IF output powers while stepping the SIS voltages
    ///<  from VJStart to VJStop by VJstep.
    ///< If sb is -1, step both junctions.  Otherwise step one junction and leave the other set as it was.
    ///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
    ///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent when finished.                        
    
    bool measureIFPowerVsVD(int pol, const std::string &description,
                            float VDStart, float VDStop, float VDStep, 
                            bool doYFactor = false, int repeatCount = 1);
    ///< For the given pol, measure the IF output powers while stepping the LO PA drain voltage
    ///<  from VDStart to VDStop by VDstep.
    ///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
    ///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent when finished.                        
    
    bool mixerDeflux(int pol, int sb, int IMagMax);
    ///< Perform a mixer defluxing procedure for the given pol and sb.
    ///<  pol may be 0 or 1.  pol=-1 indicates defluxing both polarizations.
    ///<  sb may be 1 or 2.  sb=-1 indicates defluxing both mixers.
    ///< IMagMax gives the starting value for the coil demagnetization process.
    ///< Defluxing happens on a worker thread.
    ///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent when finished.     
    
    bool prepareHealthCheck(FrontEndDatabase &dbObject, const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus,
            double &freqLOret, bool receiverIsCold, int warmUpTimeSeconds, bool includeIFPower);
    ///< set up the cartridge for performing the health check and wait the specified warmUpTimeSeconds.
    ///< executes on calling thread.
    ///<  dbObject is the database connection object to use
    ///<  feConfig and dataStatus give info for how to record data to the database.
    ///<  returns the cartridge LO frequency in ref freqLOret.
    ///<  receiverIsCold controls which measurements are performed.
    ///<  warmUpTimeSeconds specifies how long to wait before starting to take data.
    ///<  includeIFPower=false: don't wait for IF power data.

    bool startHealthCheck();
    ///< complete the health check last set up with prepareHealthCheck.  executes on worker thread.

    void abortMeasurement();
    ///< Stops all optimization measurements which are in progress.

    const XYPlotArray &getXYData() const
      { return *XYData_mp; }
    ///< Get the lastest measured X-Y data from any of the above optimize/measurement functions. 
    
    bool setEnableLNALEDs(bool enable = false);
    ///< Enable/disable the LNA LEDs.
    
    bool setEnableSISHeaters(bool enable = false, int pol = -1, float targetTemp = 12.0, int timeout = 30);
    ///< Enable/disable the SIS heaters.
    ///< If enable, start the mixer heating process.
    ///< If pol == -1, heat both polarizations.  Else pol must be 0 or 1.
    ///< If targetTemp >= 4, retrigger the the heater for up to timeout seconds to attain the target temperature.
    ///< Else, retrigger the heater for exactly timeout seconds.
    ///< For band 9, timeout may be no longer than 2 seconds.
    ///< For bands 678, timeout may be up to 2 minutes.

    bool setEnablePhotomixer(bool enable = false);
    ///< Enable/disable the photomixer.
    
    bool getEnablePhotomixer() const;
    ///< Return the enabled state of the photomixer.
    
// Private helper and implementation functions:    
private:

    void onEnableTasks();
    ///< setup tasks to be done immediately after a cartridge is powered on. 
    
    void onDisableTasks();
    ///< shutdown tasks to be done before a cartridge is powered off.

    int YIGFreqToCoarse(double freqYIG) const;
    ///< Convert freqYIG to the corresponding coarse tuning; uses the lower and upper YIG limits.

    double YIGCourseToFreq(int coarseYIG) const;
    ///< Convert coarseYIG to the actual YIG frequency; uses the lower and upper YIG limits.

// configuration and state data:

    FEConfig::CartAssemblyConfig config_m;  ///< The configuration database.

    WCAImpl *WCA_mp;                        ///< WCA sub-object.
    ColdCartImpl *coldCart_mp;              ///< cold cartridge sub-object.
    
    // what strategy to use for locking the LO:
    LockingStrategy *lockingStrategy_mp;

    MeasureSISVoltageError *measureSISVoltageErr_mp;	///< worker object for measure SIS voltage error
    CartHealthCheck *cartHealthCheck_mp;            	///< worker object for cartridge health check
    OptimizeLOPowerAmp *optimizerPA_mp;             	///< optimizer object for adjustLOPowerAmps()
    MeasureFineLOSweep *measureLOSweep_mp;          	///< worker object for fine LO sweep measurement
    mutable MeasureIVCurve *measureIV_mp;           	///< worker object for I-V curve measurement
    mutable MeasureSISCurrent *measureSISCurrent_mp;	///< worker object for IJ vs SIS magnet measurement
    MeasureIFPower *measureIFPower_mp;              	///< worker object for IF Power vs VJ or VD measurement
    MaximizeIFPower *optimizerIFPower_mp;           	///< optimizer for IF power vs LO drive.
    MixerDeflux *mixerDefluxer_mp;                  	///< worker object for mixer defluxing
    MixerHeating *mixerHeater_mp;                   	///< worker object for mixer heating
    XYPlotArray *XYData_mp;                         	///< X-Y data storage for various measurements.

    int band_m;         ///< what band this cartridge assembly is.  Cached here after querying WCA or ColdCart.  
    bool enable_m;      ///< true if this cartridge is enabled.           
    bool observing_m;   ///< true if this cartridge is observing.
    
    int multAMC_m;      ///< cache the warm LO multiplication factor.
    int multCold_m;     ///< cache the cold LO multiplication factor.
    
    double freqLO_m;    ///< the currently set LO frequency.
    double freqFLOOG_m; ///< the currently set FLOOG frequency.
    double freqREF_m;   ///< the expected reference frequency, computed from freqLO, freqFLOOG, multAMC, and sbLock. 
    bool isTunedLO_m;   ///< true if the LO frequency has been successfully set.

    static std::string FineLoSweepIni_m;    ///< path to fine LO sweep ini file.

    // Switches for debugging options:
    static bool sweepSISVoltage_m;
    static bool sweepMagnetCurrent_m;

public:        
    const std::string &asString(std::string &target) const;
    ///< format identification and state as a debugging string.

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string
       
    void appendThermalLogHeader(std::string &target) const
      { appendThermalLogHeaderImpl(target); }
    ///< append thermal header information to a logging string

    static void appendThermalLogHeaderImpl(std::string &target);
    ///< append thermal header information to a logging string

private:        
    bool checkWCA(const char *caller_name) const;
    ///< debugging helper to check wether WCA_mp is non-NULL.
    
    bool checkColdCart(const char *caller_name) const;
    ///< debugging helper to check wether coldCart_mp is non-NULL.
};

#endif /*CARTASSEMBLY_H_*/
