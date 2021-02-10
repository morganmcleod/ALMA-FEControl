#ifndef COLDCARTIMPL_H_
#define COLDCARTIMPL_H_
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
 * implementation class for the Cold Cartridge
 *----------------------------------------------------------------------
 */

#include "FEBASE/ColdCartImplBase.h"
#include "OPTIMIZE/ThermalLoggable.h"
#include "CONFIG/CartConfig.h"
#include <string>
class XYPlotArray;

class ColdCartImpl : public ColdCartImplBase, public ThermalLoggable {
public:
    ColdCartImpl(unsigned long channel, 
                 unsigned long nodeAddress,
                 const std::string &name,
                 int port,
                 int band,
                 const std::string &ESN = "");
    ///< construct with the CAN channel and node for the front end,
    ///< the name for the cold cart,
    ///< the location within the front end M&C and power distribution system,
    ///< the actual cartridge band which may be different than the location, and the ESN
    
    virtual ~ColdCartImpl()
      {}

    int getBand() const
      { return band_m; }

    const std::string &getBandText(std::string &toFill) const;
    
    static bool hasSb2(int band)
      { return band <= 8; }
    ///< true if the given band cartridge is 2SB or DSB, false if SSB.

    bool hasSb2() const
      { return hasSb2(band_m); }
    ///< true if this cartridge is 2SB or DSB, false if SSB.

    static bool hasSIS(int band)
      { return band >= 3; }
    ///< true if the given band cartridge has SIS mixers.

    bool hasSIS() const
      { return hasSIS(band_m); }
    ///< true if this cartridge has SIS mixers.

    static bool hasMagnet(int band)
      { return band >= 5; }
    ///< true if the given band cartridge has SIS magnets.)

    bool hasMagnet() const
      { return hasMagnet(band_m); }
    ///< true if this cartridge has SIS magnets.

    static bool hasSb2Magnet(int band)
      { return (hasMagnet(band) && hasSb2(band) && band != 6); }
    ///< true if the given band cartridge has a separate sb2 magnet coil.

    bool hasSb2Magnet() const
      { return hasSb2Magnet(band_m); }
    ///< true if this cartridge has a separate sb2 magnet coil.

    static bool hasSISHeater(int band)
      { return band >= 3; }
    ///< true if the given band cartridge has an SIS heater.

    bool hasSISHeater() const
      { return hasSISHeater(band_m); }
    ///< true if this cartridge has an SIS heater.

    static bool hasLNA(int band)
      { return true; }
    ///< true if the given band cartridge has LNAs.   All bands as far as we know.

    bool hasLNA() const
      { return hasLNA(band_m); }
    ///< true if this cartridge has LNAs.

    void reset();
    ///< set all state data to defaults, as if just constructed.

    void queryCartridgeState();
    ///< determine the cartridge state data based on monitor values.

    void measureSISVoltageError(std::string *resultStr = NULL);
    ///< find and store the offset between the set and readback SIS voltage.
    ///< resultStr, if provided, will be assigned like "(0.003, 0.005, 0.001, 0.040)".
    
    float measureSISVoltageErrorImpl(int pol, int sb);
    ///< Implementation of find and store the offset between the set and readback SIS voltage.
    
    bool measureIVCurve(XYPlotArray &target, int pol, int sb, float VJlow, float VJhigh, float VJstep);
    ///< Synchronous measurement of I-V curve, placing results in the given XYPlotArray:   X=VJ set, Y1=VJ read, Y2=IJ read.
    ///< Reports progress messages via FEMCEventQueue.
    ///< Returns VJ to whatever it was set to prior to the measurement.

private:
    bool measureIVCurveInnerLoop(XYPlotArray &target, int pol, int sb,
                                 float VJstart, float VJstop, float VJstep,
                                 int progressStart, int progressEnd);
    ///< Private helper to measure one sub-range of the I-V curve.

public:
    bool saveIVCurveData(const XYPlotArray &source, const std::string logDir, int pol, int sb) const;
    ///< Write out the contents of the source array as a text data file in the given logDir.

    void stopIVCurve()
      { ivCurveStop_m = true; }
    ///< Force the I-V curve measurement process to stop.

//-------------------------------------------------------------------------------------------------    
// SIS voltage monitor & control

    // promote the base class virtual functions so that they will not be hidden:
    using ColdCartImplBase::sisPol0Sb1Voltage;
    using ColdCartImplBase::sisPol0Sb2Voltage;
    using ColdCartImplBase::sisPol1Sb1Voltage;
    using ColdCartImplBase::sisPol1Sb2Voltage;
    
private:
    // Override but hide from public use the raw SIS voltage set functions: 
    
    virtual void sisPol0Sb1Voltage(float val);
    ///< override the base class SIS voltage control function so that we can cache the value in local state.
    virtual void sisPol0Sb2Voltage(float val);
    ///< override the base class SIS voltage control function so that we can cache the value in local state.
    virtual void sisPol1Sb1Voltage(float val);
    ///< override the base class SIS voltage control function so that we can cache the value in local state.
    virtual void sisPol1Sb2Voltage(float val);
    ///< override the base class SIS voltage control function so that we can cache the value in local state.
    
public:    
    bool setSISEnable(bool val);
    ///< set the SIS enable state.  If val is false, forces the SIS voltage to zero.
    ///< returns true this band has an SIS.  Otherwise false and logs an error.
    
    bool getSISEnableSetting() const
      { return sisEnableSet_m; }
    ///< get the SIS enable state.
    
    void setSISVoltage(int pol, int sb, float val, bool sweep = true);
    ///< set the SIS voltage control for the specified pol and sb.
    ///< sweep = true means to move gradually from the present setting to the new setting.
    
    float getSISVoltageSetting(int pol, int sb) const
      { if (!checkPolSb(pol, sb)) return 0;
        return sisVoltageSet_m[pol * 2 + sb - 1]; }
    ///< get the last SIS voltage setting for the specified pol and sb.

    float getSISVoltage(int pol, int sb, int average = 1, bool print = false);
    ///< get the SIS voltage monitor for the the specified pol and sb, averaging multiple readings if requested.
    
//-------------------------------------------------------------------------------------------------    
// SIS current monitoring
    
    float getSISCurrent(int pol, int sb, int average = 1);
    ///< get the SIS current monitor for the specified pol and sb, averaging multiple readings if requested.

//-------------------------------------------------------------------------------------------------
// SIS magnet monitor and control

    // Override the base class magnet monitor functions to filter for hasMagnet()
    virtual float sisMagnetPol0Sb1Voltage()
      { return (hasMagnet()) ? ColdCartImplBase::sisMagnetPol0Sb1Voltage() : 0.0; }
    virtual float sisMagnetPol0Sb1Current()
      { return (hasMagnet()) ? ColdCartImplBase::sisMagnetPol0Sb1Current() : 0.0; }
    virtual float sisMagnetPol0Sb2Voltage()
      { return (hasSb2Magnet()) ? ColdCartImplBase::sisMagnetPol0Sb2Voltage() : 0.0; }
    virtual float sisMagnetPol0Sb2Current()
      { return (hasSb2Magnet()) ? ColdCartImplBase::sisMagnetPol0Sb2Current() : 0.0; }
    virtual float sisMagnetPol1Sb1Voltage()
      { return (hasMagnet()) ? ColdCartImplBase::sisMagnetPol1Sb1Voltage() : 0.0; }
    virtual float sisMagnetPol1Sb1Current()
      { return (hasMagnet()) ? ColdCartImplBase::sisMagnetPol1Sb1Current() : 0.0; }
    virtual float sisMagnetPol1Sb2Voltage()
      { return (hasSb2Magnet()) ? ColdCartImplBase::sisMagnetPol1Sb2Voltage() : 0.0; }
    virtual float sisMagnetPol1Sb2Current()
      { return (hasSb2Magnet()) ? ColdCartImplBase::sisMagnetPol1Sb2Current() : 0.0; }

private:
    // Override but hide from public use the raw SIS magnet set functions:
    
    virtual void sisMagnetPol0Sb1Current(float val);
    ///< override the base class SIS magnet current control function so that we can cache the value in local state.
    virtual void sisMagnetPol0Sb2Current(float val);
    ///< override the base class SIS magnet current control function so that we can cache the value in local state.
    virtual void sisMagnetPol1Sb1Current(float val);
    ///< override the base class SIS magnet current control function so that we can cache the value in local state.
    virtual void sisMagnetPol1Sb2Current(float val);
    ///< override the base class SIS magnet current control function so that we can cache the value in local state.
    
public:    
    bool setSISMagnetEnable(bool val);
    ///< set the SIS magnet enable state.  If val is false, forces the magnets to zero bias.
    ///< return true of the band has a magnet and no error.   Else log an error and return false.
    
    bool getSISMagnetEnableSetting() const
      { return sisMagnetEnableSet_m; }
    ///< get the SIS magnet enable state.
    
    void setSISMagnetCurrent(int pol, int sb, float val,
                             bool sweep = true,
                             float sweepStep = 0.1,
                             unsigned sweepDwell = 0);
    ///< set the SIS magnet current control for the specified pol and sb.
    ///< sweep = true means to move gradually from the present setting to the new setting.
    ///< The sweepStep size may be specified to be anything down to 0.001 mA.
    ///< sweepDwell gives how many milliseconds to pause at each intermediate step.

    float getSISMagnetCurrentSetting(int pol, int sb) const
      { if (!checkPolSb(pol, sb)) return 0;
        return sisMagnetCurrentSet_m[pol * 2 + sb - 1]; }
    ///< get the last SIS magnet current setting for the specified pol and sb.
    
    float getSISMagnetCurrent(int pol, int sb);
    ///< get the SIS magnet current monitor for the the specified pol and sb.
    
//-------------------------------------------------------------------------------------------------
// SIS heater operation

    // Override the base class heater monitor functions to filter for hasSIS()
    virtual float sisHeaterPol0Current()
      { return (hasSIS()) ? ColdCartImplBase::sisHeaterPol0Current() : 0.0; }
    virtual float sisHeaterPol1Current()
      { return (hasSIS()) ? ColdCartImplBase::sisHeaterPol1Current() : 0.0; }

    void setSISHeaterEnable(int pol, bool enable);
    ///< Enable/disable the SIS heaters.  Pol must be 0 or 1.
    
    float getSISHeaterCurrent(int pol)
      { return (pol == 1) ? sisHeaterPol1Current() : ((pol == 0) ? sisHeaterPol0Current() : 0.0); } 
    ///< get the SIS heater current for the specified pol.
    
    bool sisMixerHeatingProcess(int pol = -1, float targetTemp = 12.0, int timeout = 30);
    ///< Start the mixer heating process.
    ///< If pol == -1, heat both polarizations.  Else pol must be 0 or 1.
    ///< If targetTemp >= 4, retrigger the the heater for up to timeout seconds to attain the target temperature.
    ///< Else, retrigger the heater for exactly timeout seconds.
    ///< For band 9, timeout may be no longer than 2 seconds.
    ///< For bands 678, timeout may be up to 2 minutes.
    ///< Watches member variable mixerHeatingStop_m and aborts if true.

    void stopMixerHeating()
      { mixerHeatingStop_m = true; }
    ///< Force the mixer heating process to stop.

//-------------------------------------------------------------------------------------------------
// LNA monitor and control
    
    virtual void lnaPol0Sb1Enable(bool val);
    ///< override the base class LNA enable control function so that we can cache the value in local state.
    virtual void lnaPol0Sb2Enable(bool val);
    ///< override the base class LNA enable control function so that we can cache the value in local state.
    virtual void lnaPol1Sb1Enable(bool val);
    ///< override the base class LNA enable control function so that we can cache the value in local state.
    virtual void lnaPol1Sb2Enable(bool val);
    ///< override the base class LNA enable control function so that we can cache the value in local state.

    bool setLNAEnable(bool val);
    ///< set the LNA enable state for all pol and sb.
    ///< returns true if LNA is defined for this band.  Else logs an error message and returns false.
    
    bool setLNAEnable(int pol, int sb, bool val);
    ///< set the LNA enable state for the specified pol and sb.
    ///< returns true if LNA is defined for this band.  Else logs an error message and returns false.

    bool getLNAEnableSetting() const
      { return lnaEnableSet_m[0] && lnaEnableSet_m[2]; }
    ///< get the LNA enable state for all pol and sb.
    
    bool getLNAEnableSetting(int pol, int sb) const
      { if (!checkPolSb(pol, sb)) return 0;
        return lnaEnableSet_m[pol * 2 + sb - 1]; }
    ///< get the LNA enable setting for the specified pol and sb.
    
//-------------------------------------------------------------------------------------------------
// misc public helpers:
    
    static bool checkPolSb(int pol, int sb)
      { return (pol == 0 || pol == 1) && (sb == 1 || sb == 2); }
    ///< check whether valid values are provided for pol and sb.
    
    static int getMultiplier(int band)
      { static int mults[11] = {
            1,  // band 0: no multiplier 
            1,  // band 1
            1,  // band 2
            1,  // band 3
            2,  // band 4
            2,  // band 5
            3,  // band 6
            3,  // band 7
            6,  // band 8
            9,  // band 9
            9   // band 10
        }; return mults[band]; }
    ///< get the cold multiplier for the given band.

//-------------------------------------------------------------------------------------------------
// retrieve monitor values:
    struct CartridgeTemp_t {
        float cartridgeTemperature0_value;
        float cartridgeTemperature1_value;
        float cartridgeTemperature2_value;
        float cartridgeTemperature3_value;
        float cartridgeTemperature4_value;
        float cartridgeTemperature5_value;
    };
    ///< structure for returning cartridge temperature monitor values
    bool getMonitorTemp(CartridgeTemp_t &target) const;
    ///< get the cartridge temperature monitor values.

    struct SIS_t {
        bool sisOpenLoop_value;
        float sisVoltage_value;
        float sisCurrent_value;
        float sisMagnetVoltage_value;
        float sisMagnetCurrent_value;
    };
    ///< structure for returning SIS and magnet monitor values
    bool getMonitorSIS(int pol, int sb, SIS_t &target) const;
    ///< get the SIS and magnet monitor values for the given pol and sb.

    struct LNA_t {
        bool lnaEnable_value;
        float lnaSt1DrainVoltage_value;
        float lnaSt1DrainCurrent_value;
        float lnaSt1GateVoltage_value;
        float lnaSt2DrainVoltage_value;
        float lnaSt2DrainCurrent_value;
        float lnaSt2GateVoltage_value;
        float lnaSt3DrainVoltage_value;
        float lnaSt3DrainCurrent_value;
        float lnaSt3GateVoltage_value;
    };
    ///< structure for returning LNA monitor values.
    bool getMonitorLNA(int pol, int sb, LNA_t &target) const;
    ///< get the LNA monitor values for the given pol and sb.

    struct Aux_t {
        float sisHeaterCurrent_value;
        bool lnaLedEnable_value;
    };
    ///< structure for returning auxiliary monitor values.
    bool getMonitorAux(int pol, Aux_t &target) const;
    ///< get the auxiliary monitor values for the given pol. 

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string

    void appendThermalLogHeader(std::string &target) const
      { appendThermalLogHeaderImpl(target); }
    ///< append thermal header information to a logging string

    static void appendThermalLogHeaderImpl(std::string &target);
    ///< append thermal header information to a logging string

    static void appendThermalLogPlaceholder(std::string &target);
    ///< append zero data to the thermal log so that columns will align.

protected:
    virtual void monitorAction(Time *timestamp_p);
    
private:
    int band_m;                     ///< which cartridge band this is.
    bool sisEnableSet_m;            ///< cached state: are the SIS mixers enabled?
    float sisVoltageSet_m[4];       ///< caches latest control values set. 
    float sisVoltageError_m[4];     ///< SIS voltage error measured at VJset = 0.
    bool sisMagnetEnableSet_m;      ///< cached state: are the SIS magnets enabled?    
    float sisMagnetCurrentSet_m[4]; ///< caches latest control values set.
    bool lnaEnableSet_m[4];         ///< caches latest control values set.

    // support functions and vars for mixer heating process:
    void resetMixerHeating();
    void sisMixerHeatingReadData(bool printHeader = false);

    bool ivCurveStop_m;
    bool mixerHeatingStop_m;
    unsigned long mixerHeatingtimerStart_m;
    std::ostream *mixerHeatingdataFile_mp;
    static const float mixerHeatingMinTargetTemp_m;
    static const int mixerHeatingMaxTimeout_m;
    static const int mixerHeatingMaxTimeoutBand9_m;

    DECLARE_MONITORS_REGISTRY(ColdCartImpl)
    void logMon(bool printHeader = false) const;
};
 
#endif /*COLDCARTIMPL_H_*/

