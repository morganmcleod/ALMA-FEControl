#ifndef WCAIMPL_H_
#define WCAIMPL_H_
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
 * implementation class for the Warm Cartridge Assembly
 *----------------------------------------------------------------------
 */

#include "FEBASE/WCAImplBase.h"
#include "OPTIMIZE/ThermalLoggable.h"
#include "CONFIG/CartConfig.h"

class WCAImpl : public WCAImplBase, public ThermalLoggable {
public:
    WCAImpl(unsigned long channel, 
            unsigned long nodeAddress,
            const std::string &name,
            int port,
            int band,
            const std::string &ESN = "");
    ///< conststruct with the CAN channel and node for the front end,
    ///< the name for the cold cart,
    ///< the location within the front end M&C and power distribution system,
    ///< the actual cartridge band which may be different than the location, and the ESN
    
    virtual ~WCAImpl()
      {}
      
    int getBand() const
      { return band_m; }

    const std::string &getBandText(std::string &toFill) const;
    
    void reset();
    ///< set all state data to defaults, as if just constructed.

    void queryCartridgeState();
    ///< determine the cartridge state data based on monitor values.
      
//-------------------------------------------------------------------------------------------------
// YTO and PLL control settings

    // promote the base class virtual functions so that they will not be hidden:
    using WCAImplBase::ytoCoarseTune;
    using WCAImplBase::pllSidebandLockSelect;
    using WCAImplBase::pllNullLoopIntegrator;
    
    virtual void ytoCoarseTune(unsigned short val);
    ///< override the base class YTO coarse tuning control and cache the value.
    
    unsigned short getYTOCoarseTuneSetting() const
      { return ytoCoarseTune_m; }
    ///< get the last set YTO coarse tuning state.

    void ytoCoarseTuneWithTrace(unsigned short coarseYIG, const char *traceText = NULL);
    ///< Debugging wrapper for setting the YTO coarse tuning.
    
    virtual void pllSidebandLockSelect(bool val);
    ///< override the base class sideband lock select control and cache the value.
    
    bool getPLLSidebandLockSelectSetting() const
      { return pllSidebandLockSelect_m; }
    ///< get the last set sideband lock select state.
    
    virtual void pllNullLoopIntegrator(bool val);
    ///< override the base class null integrator control and cache the value.
    
    bool getPLLNullLoopIntegratorSetting() const
      { return pllNullLoopIntegrator_m; }
    ///< get the last set null integrator state.

//-------------------------------------------------------------------------------------------------
// Lock detection and monitoring

    bool testPLLPowerLevels();
    ///< Test the Ref and IF power levels for normal in locked state.

    bool testPLLLockDetectVoltage();
    ///< Test the lock detect voltage for logical high.

    bool testNormalLockDetect();
    ///< Normal lock detection using the lock detect voltage and the IF and reference power levels.

    bool testPLLLockDetectVoltage2X();
    ///< Test the lock detect voltage twice with short delay, ANDing the results.

    bool test2XVoltageLockDetect();
    ///< Read the voltage twice and the IF and reference power levels.

    bool getLocked();
    ///< Interrogate normal and, if appropriate, alternate lock signals.

    bool monitorLockForDisplay() const;
    ///< Return whether the PLL seems locked based on prior state and latest monitor data.

//-------------------------------------------------------------------------------------------------
// Locking procedures

    bool adjustPLL(float targetCorrVoltage);
    ///< Step the YTO to achieve the target PLL correction voltage.

//-------------------------------------------------------------------------------------------------
// LO PA monitor and control:
    
    // promote the base class virtual functions so that they will not be hidden:
    using WCAImplBase::paPol0DrainVoltage;
    using WCAImplBase::paPol1DrainVoltage;
    using WCAImplBase::paPol0GateVoltage;
    using WCAImplBase::paPol1GateVoltage;

    virtual void paPol0DrainVoltage(float val);
    ///< override the base class PA drain voltage control and cache the value.
    virtual void paPol1DrainVoltage(float val);
    ///< override the base class PA drain voltage control and cache the value.
    virtual void paPol0GateVoltage(float val);
    ///< override the base class PA gate voltage control and cache the value.
    virtual void paPol1GateVoltage(float val);
    ///< override the base class PA gate voltage control and cache the value.
    
    void setPAEnable(bool val);
    ///< set the PA enable state.
    bool getPAEnableSetting() const
      { return paEnable_m; }
    ///< get the PA enable state.

    void setPADrainVoltage(int pol, float val)
      { if (pol == 0) paPol0DrainVoltage(val);
        else if (pol == 1) paPol1DrainVoltage(val); }       
    ///< set the PA drain voltage control for the specified pol.
    
    float getPADrainVoltageSetting(int pol) const
      { if (!checkPol(pol)) return 0.0;
        return paDrainVoltage_m[pol]; }
    ///< get the PA drain voltage setting for the specified pol.

    float getPADrainVoltage(int pol)
      { if (pol == 0) return paPol0DrainVoltage();
        else if (pol == 1) return paPol1DrainVoltage();
        else return 0.0; }
    ///< monitor the the PA drain voltage.

    float getPAGateVoltageSetting(int pol) const
      { if (!checkPol(pol)) return 0.0;
        return paGateVoltage_m[pol]; }
    ///< get the PA gate voltage setting for the specified pol.
    
    void setEnableLO(bool enable, bool setAMC = false, bool detune = false);
    ///< enable/disable LO output.
    ///< when disabled:
    ///<    sets the LO PA drain voltage to zero.
    ///<    if 'detune' is true, tunes the LO output to the far end of the band.
    ///<    if 'setAMC' is true, sets the AMC drain voltage to zero.
    
    bool getEnableLO() const
      { return LOEnable_m; }
    ///< is the LO output currently disabled?

    void setLOPower(int pol, float percent);
    ///< set the LO output power level as a percentage of maximum

//-------------------------------------------------------------------------------------------------
// AMC control:

    bool setAMC(float VDE, float VGE);

    void getAMCSetting(float &VDE, float &VGE) const
      { VDE = amcDrainEVoltage_m;
        VGE = amcGateEVoltage_m; }

//-------------------------------------------------------------------------------------------------
// Photomixer monitor and control:

    // promote the base class virtual functions so that they will not be hidden:
    using WCAImplBase::photomixerEnable;
    
    virtual void photomixerEnable(bool val);
    ///< set the photomixer enable state.
    bool getPhotomixerEnableSetting() const
      { return photomixerEnable_m; }
    ///< get the photomixer enable state.

//-------------------------------------------------------------------------------------------------
// misc public helpers:
    
    static bool checkPol(int pol)
      { return (pol == 0 || pol == 1); }
    ///< check whether valid value is provided for pol.
    
    static int getMultiplier(int band)
      { static int mults[11] = {
            1,  // band 0: no multiplier 
            1,  // band 1
            6,  // band 2
            6,  // band 3
            3,  // band 4
            6,  // band 5
            6,  // band 6
            6,  // band 7
            3,  // band 8
            3,  // band 9
            6   // band 10
        }; return mults[band]; }
    ///< get the warm multiplier for the given band.

    static bool getAltLoopBW(int band)
      { static bool selects[11] = {
            false,  // band 0: none
            false,  // band 1: don't care. fixed 2.5 MHz/V
            true,   // band 2: 1 -> 15MHz/V (Band 3,5,6,7,10 & band2-proto)
            true,   // band 3
            false,  // band 4: 0 -> 7.5MHz/V (Band 4,8,9)
            true,   // band 5
            true,   // band 6
            true,   // band 7
            false,  // band 8
            false,  // band 9
            true    // band 10
          }; return selects[band]; }

//-------------------------------------------------------------------------------------------------
// retrieve monitor values:
    
    struct YTO_t {
        unsigned short ytoCoarseTune_value;
        double ytoFrequency_value;
        void reset()
          { ytoCoarseTune_value = 0;
            ytoFrequency_value = 0; }
    };
    ///< structure for returning YTO monitor data.
    bool getMonitorYTO(YTO_t &target) const;
    ///< get the YTO monitor data.
    
    struct Photomixer_t {
        bool photomixerEnable_value;
        float photomixerVoltage_value;
        float photomixerCurrent_value;
    };
    ///< structure for returning photomixer monitor data.
    bool getMonitorPhotomixer(Photomixer_t &target) const;
    ///< get the photomixer monitor data.
    
    struct PLL_t {
        float pllLockDetectVoltage_value;
        float pllCorrectionVoltage_value;
        float pllAssemblyTemp_value;
        float pllYTOHeaterCurrent_value;
        float pllRefTotalPower_value;
        float pllIfTotalPower_value;
        bool pllLock_value;
        bool pllUnlockDetectLatch_value;
        bool pllLoopBandwidthSelect_value;
        bool pllSidebandLockSelect_value;
        bool pllNullLoopIntegrator_value;
    };
    ///< structure for returning PLL monitor data.
    bool getMonitorPLL(PLL_t &target) const;
    ///< get the PLL monitor data.
    
    struct AMC_t {
        float amcGateAVoltage_value;
        float amcDrainAVoltage_value;
        float amcDrainACurrent_value;
        float amcGateBVoltage_value;
        float amcDrainBVoltage_value;
        float amcDrainBCurrent_value;
        float amcGateEVoltage_value;
        float amcDrainEVoltage_value;
        float amcDrainECurrent_value;
        unsigned char amcMultiplierDCounts_value;
        float amcMultiplierDCurrent_value;
        float amcSupplyVoltage5V_value;
    };
    ///< structure for returning AMC monitor data.
    bool getMonitorAMC(AMC_t &target) const;
    ///< get the AMC monitor data.
    
    struct PA_t {
        float paPol0GateVoltage_value;
        float paPol0DrainVoltage_value;
        float paPol0DrainCurrent_value;
        float paPol1GateVoltage_value;
        float paPol1DrainVoltage_value;
        float paPol1DrainCurrent_value;
        float paSupplyVoltage3V_value;
        float paSupplyVoltage5V_value;
    };
    ///< structure for returning power amp monitor data.
    bool getMonitorPA(PA_t &target) const;
    ///< get the power amp monitor data.

//-------------------------------------------------------------------------------------------------
// Thermal Log interface:

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string

    void appendThermalLogHeader(std::string &target) const
      { appendThermalLogHeaderImpl(target); }
    ///< append thermal header information to a logging string

    static void appendThermalLogHeaderImpl(std::string &target);
    ///< append thermal header information to a logging string

    static void appendThermalLogPlaceholder(std::string &target);
    ///< append zero data to the thermal log so that columns will align.
    
private:
    int band_m;                     ///< which cartridge band this is.
    unsigned short ytoCoarseTune_m; ///< caches the last set course tuning.       
    int pllSidebandLockSelect_m;    ///< caches the last set sideband lock value.
    bool pllNullLoopIntegrator_m;   ///< caches the last set null integrator value.
    bool paEnable_m;                ///< is the LO PA enabled?
    bool LOEnable_m;                ///< is the LO output enabled/disabled via setEnableLO()?
    bool photomixerEnable_m;        ///< is the photomixer enabled?
    float paDrainVoltage_m[2];      ///< caches latest control values set.
    float paGateVoltage_m[2];       ///< caches latest control values set.
    float amcDrainEVoltage_m;       ///< caches latest control value set.
    float amcGateEVoltage_m;        ///< caches latest control value set.

    static const float lowLockDetectVoltage;    ///< minimum Lock Detect Voltage when locked
    static const float lowIFTotalPower;         ///< minimum IF Total Power Detect Voltage magnitude when locked
    static const float lowRefTotalPower;        ///< minimum Reference Total Power Detect Voltage magnitude when locked

    static const float minSettingPAVD;        ///< minumum allowed LO PA drain voltage setting
    static const float maxSettingPAVD;        ///< maximum allowed LO PA drain voltage setting
    static const float minSettingPAVG;        ///< minumum allowed LO PA gate voltage setting
    static const float maxSettingPAVG;        ///< maximum allowed LO PA gate voltage setting
};
 
#endif /*WCAIMPL_H_*/

