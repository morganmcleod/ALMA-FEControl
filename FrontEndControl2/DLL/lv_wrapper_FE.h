#ifndef _LV_WRAPPER_FE_H_
#define _LV_WRAPPER_FE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
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

/************************************************************************
 * declarations for the functions exported from the DLL for consumption
 * by LabVIEW.  Strictly, this header is not required, since the
 * export declarations in lv_wrapper_FE.cpp, coupled with the .def file
 * is all that is needed to export.  However, having the header is
 * useful for linking from a C or C++ test program.
 * 
 *----------------------------------------------------------------------
 */


#include "dllExport.h"
extern "C" {

struct CartStateData_t;
struct CartIFPowerData_t;
struct CartYTOData_t;
struct CartPhotomixerData_t;
struct CartPLLData_t;
struct CartAMCData_t;
struct CartLNAData_t;
struct CartSISData_t;
struct CartPAData_t;
struct CartTempData_t;
struct CartAuxData_t;
struct CartLastHeaterCurrents_t;
struct PowerModuleData_t;
struct IFSwitchData_t;
struct CryostatData_t;
struct LPRData_t;
struct FETIMData_t;

// All functions return 0 for success or -1 for failure unless otherwise specified.
// Valid values for port are 1-10.
// Valid values for pol are 0 and 1.
// Valid values for sb are 1 and 2.

//----------------------------------------------------------------------------
// Functions for connection and disconnection to the front end.
// Also information available directly from the the embedded controller.

DLLEXPORT short FEControlInit();
///< Initialize the control connection to the FE.

DLLEXPORT short FEControlShutdown();
///< Destroy the control connection to the FE.

DLLEXPORT short FEMCExit();
///< Shutdown the firmware on the FE M&C module.

DLLEXPORT short FEGetConfigSN(long *configId, char *serialNum);
///< Return the currently configured FE serial number
///<  and the latest configuration ID for this FE.

DLLEXPORT short FEMCGetAMBSIInfo(char *serialNum,
                                 char *firmwareRev, 
                                 unsigned long *numErrors, 
                                 unsigned long *numTransactions);
///< Get the AMBSI1's monitor data.                                 

DLLEXPORT short FEMCGetAMBSITemperature(float *temperature);
///< Get the AMBSI1's temperature.

DLLEXPORT short FEMCGetFirmwareInfo(char *AMBSILibraryVersion, 
                                    char *FEMCFirmwareVersion,
                                    char *FEMCFPGAVersion);
///< Get the FE M&C firmware version information.

DLLEXPORT short FEMCGetESNText(char *target);
///< Get the list of electronic serial numbers found in the FE.

DLLEXPORT short FEMCRescanESNs();
///< Send a command to the FEMC module to rescan the 1-wire bus for ESNs.

DLLEXPORT short FEMCGetNumErrors();
///< How many unread errors are waiting in the error queue?
///< Returns >= 0 if there are unread errors;   -1 if not connected to the FE.

DLLEXPORT short FEMCFlushErrors();
///< Flush the FEMC module error queue.
///< Returns 0 normally;   -1 if not connected to the FE.

DLLEXPORT short FEMCGetNextError(short *moduleNum, short *errorNum, char *description);
///< Read the next unread error from the error queue.
///< Returns {-1, -1, ""} if the queue is empty or an error occurs.

DLLEXPORT short FEMCSetFEMode(short mode);
///< Set the FE operating mode to one of
///<  0 -> Operational
///<  1 -> Troubleshooting
///<  2 -> Maintenance
///<  3 -> Simulation (for testing with no connected FE hardware)
///< All other values are rejected.

//----------------------------------------------------------------------------
// Functions for configuring the hardware installed in the front end.                                    

DLLEXPORT short FELoadConfiguration(short configId);
///< Configure the FE from the configuration database according to the given configId.
///< If configId is 0, use the default configuration given in the FrontEndControlDLL.ini file.

//----------------------------------------------------------------------------
// Front end and cartridge health check operations.

DLLEXPORT short FEGetConfiguredBands(short *size, short *bands);
///< Retrieve an array of configured band numbers.
///< bands points to a data array. size and bands are required.
///< *size specifies how many elements are in the bands array before the call
///<  and how many valid elements are there after the call.  Array memory must be pre-allocated.

DLLEXPORT short FEStartHealthCheck(short dataStatus);
///< Set initial conditions for front end overall and cartridge health check.
///< if dataStatus is 0, use the default data status for healthcheck.  Otherwise, use the provided value.

DLLEXPORT short cartHealthCheck(short port, short warmUpTimeSeconds, short includeIFPower);
///< Peform a health check on the cartridge assembly at the given port.
///< Wait the specified number of seconds before taking data.
///< includeIFPower=false: don't include waiting for IF power measurements.

DLLEXPORT short cartHealthCheckSaveIFPowerData(short port, CartIFPowerData_t *source);
///< Save the provided IF power and Y-factor data to the datbase as part of the health check in progress.

DLLEXPORT short FEFinishHealthCheck();
///< Set the front end back to normal conditions after health check.

//----------------------------------------------------------------------------
// Functions for controlling power and operating mode of the cartridges.

DLLEXPORT short FESetCartridgeOff(short port);
DLLEXPORT short FESetCartridgeOn(short port);
DLLEXPORT short FESetCartridgeStandby2(short port, short enable);
DLLEXPORT short FESetCartridgeObserving(short port);
DLLEXPORT short FEClearCartridgeObserving(short port);

DLLEXPORT short FEGetCartridgeState(short port, CartStateData_t *target);
///< Get the state of the cartridge and its subsystem control switches (enable, observing, SIS enable, etc.)

DLLEXPORT short FEGetNumCartridgesOn();
///< ERROR              -1
///< 0-10 number of cartridges in ON or OBSERVING state.

DLLEXPORT short cartPauseMonitor(short port, short pauseWCA, short pauseCC);
///< Pause/restart monitoring for the specified port WCA and/or cold cart.

//----------------------------------------------------------------------------
// Cartridge control operations.

//-----------------------
// LO tuning and locking

DLLEXPORT short cartSetLockingStrategy(short port, short strategy);
///< Set the locking strategy to use:
///< 0: original FETMS strategy with no LPR EDFA adjustment
///< 1: ALMA cycle 8 strategy with LPR EDFA adjustment
///< 2: Giorgio's 5-Points strategy, with LPR EDFA adjustment
///< 3: Giorgio's 9-Points strategy, with LPR EDFA adjustment

DLLEXPORT short cartSetLOFrequency(short port, double freqLO, double freqFLOOG, short sbLock);
///< Set the YTO tuning according to the given LO reference & floog frequencies.
///< sbLock = 0 means lock below the reference:  freqREF = freqLO + freqFLOOG;
///< sbLock = 1 means lock above the reference:  freqREF = freqLO - freqFLOOG.

DLLEXPORT short cartSetCenterLOFrequency(short port, double freqFLOOG, short sbLock);
///< Set the YTO tuning to the center of the LO band.

DLLEXPORT short cartGetLOFrequency(short port, double *freqLO, double *freqREF);
///< Get the current freqLO and freqREF settings.  Software readback only.

DLLEXPORT short cartLockPLL(short port);
///< Initiate the search for a lock.   
///< Can take a long time if the YTO is not tuned correctly for the actual LO reference and FLOOG signals.

DLLEXPORT short cartGetLocked(short port, short *locked);
///< Get the LO locked status.

DLLEXPORT short cartAdjustPLL(short port, float targetCorrVoltage);
///< Adjust the YTO to get the correction voltage as close as possible to the given target voltage.

DLLEXPORT short cartAdjustYTO(short port, short steps);
///< Bump the YTO up or down by the given number of steps.  May be + or -.

DLLEXPORT short cartNullPLLIntegrator(short port, short enable);
///< Enable the PLL null integrator setting.  Prevents PLL locking and allows free-running operation.

DLLEXPORT short cartSetEnablePhotomixer(short port, short enable);
///< Enable/disable the photomixer.

//-----------------------
// Cryogenic LNA

DLLEXPORT short cartSetLNABias(short port, short pol, short sb,
                               float VD1, float ID1,
                               float VD2, float ID2,
                               float VD3, float ID3);
///< Set the LNA bias for a given pol and sb or for all LNAs simultaneously.
///< If any of the float values given is +Inf or -Inf it is ignored and the corresponding setting 
///<  is used from the configuration database. 
///< If either pol or sb is -1, then all float values are ignored
///<  and the values from the config database are used.

DLLEXPORT short cartSetEnableLNABias(short port, short enable, short pol, short sb);
///< Enable or disable the LNA for a given pol and sb or for all LNAs simultaneously.
///< If pol or sb is -1, all LNAs are simultaneously affected.

DLLEXPORT short cartSetEnableLNALEDs(short port, short enable);
///< Enable/disable the LNA LEDs.

//------------------------
// SIS Mixer and heater

DLLEXPORT short cartSetSISBias(short port, short enable, short pol, short sb,
                               float VJ, short openLoop);
///< Enable/disable and set the SIS bias for a given pol and sb or for all SIS mixers simultanously.
///< If VJ is +Inf or -Inf, it is ignored and the setting from the configuration database is used.
///< If openLoop is -1, no change is made to the SIS openLoop state.
///< If either pol or sb is -1, then all junctions are biased according to the database.

DLLEXPORT short cartSetEnableSISHeaters(short port, short enable, short pol, float targetTemp, short timeout);
///< Enable/disable the SIS heaters.
///< If enable, start the mixer heating process.
///< If pol == -1, heat both polarizations.  Else pol must be 0 or 1.
///< If targetTemp >= 4, retrigger the the heater for up to timeout seconds to attain the target temperature.
///< Else, retrigger the heater for exactly timeout seconds.
///< For band 9, timeout may be no longer than 2 seconds.
///< For bands 678, timeout may be up to 2 minutes.

//---------------------------
// SIS Magnet

DLLEXPORT short cartSetSISMagnet(short port, short enable, short pol, short sb, float IMag);
///< Enable/disable and set the SIS magnet bias for a given pol and sb or for all SIS magnets simultanously.
///< If IMAG is +Inf or -Inf, it is ignored and the setting from the configuration database is used.
///< If either pol or sb is -1, then all magnets are biased according to the database.

DLLEXPORT short cartGetSISMagnet(short port, short pol, short sb, float *IMag);
///< Get the SIS magnet control setting for a given pol and sb.

//---------------------------
// LO Power Amps

DLLEXPORT short cartSetLOPowerAmps(short port, short enable,
                                   float VDP0, float VGP0,
                                   float VDP1, float VGP1);
///< Enable/disable and set the LO power amps.
///< If any of the float values given is +Inf or -Inf, no change is made to the corresponding control point.
///< If all the float values are +Inf or -Inf, all settings are made according to the configuration database. 

DLLEXPORT short cartGetLOPowerAmpsSetting(short port, short *isEnabled,
                                   float *VDP0, float *VGP0,
                                   float *VDP1, float *VGP1);
///< Get the last set state of the LO power amps.  Software readback only.

DLLEXPORT short cartAdjustLOPowerAmps(short port, short repeatCount);
///< Adjust the LO PA drain voltages until the junction currents are as close as possible to 
///<  the the target values given in the configuration database.
///< Adjustment happens on a worker thread.
///< Sends the event EVENT_PA_ADJUST_DONE via getNextEvent whene finished.

DLLEXPORT short cartSetEnableLO(short port, short enable);
///< Enable or disable the LO output for the specified WCA.  Intended for use with BeaSTs.
///< This turns off the the LO PA and moves the YTO to the opposite extreme of the tuning range.
///< In conjunction with the IF Processor YIG filter this effectively removes all source power, 
///<  even in cases where source power may be leaking through or around the PA.

DLLEXPORT short cartSetLOPower(short port, short pol, float percent);
///< set the LO output power level as a percentage (0.0 - 100.0) of maximum

DLLEXPORT short cartOptimizeIFPower(short port, short pol, float VDstart0, float VDstart1);
///< Perform optimization of IF power vs. LO drive and junction voltage.
///< works in conjunction with setIFPower() which injects ongoing power readings.
///< Optimization happens on a worker thread.
///< if pol is -1, both polarizations are optimized in turn.
///< VDstart0 and VDstart1 give the starting value for LO PA VD search for pol0 and po1, respectively.
///< If either are out of range, 0.8 V is used as the start value.
///< Sends the event EVENT_REQUEST_IFPOWER via the client app event queue when power readings are required.
///< Sends the event EVENT_PA_ADJUST_DONE  when finished.                        

DLLEXPORT short cartClearOptimizedResult(short port);
///< Clear out tables accumulating results from cartOptimizeIFPower for the given port.

DLLEXPORT short cartGetOptimizedResult(short port, char *mixerParamsText);
///< Get the optimized MixerParams as text from recent calls to cartOptimizeIFPower.
///<    since the last call to cartClearOptimizedResult().
///< Returns a text section of the collectd CCA MixerParams.

DLLEXPORT short cartSetIFPower(short port, short pol, float powerSB1, float powerSB2);
///< Inject the current IF power level readings for a single port and polarization.
///< Used for optimizing mixer and LO power amp bias voltages.

DLLEXPORT short cartSetVJVD(short port, short pol,
                            float VJ1, float VJ2, float VD);                               
///< Set the junction voltages and/or PA drain voltage for the given port and polarization.
///< If any of VJ1, VJ2, or VD is +Inf or -Inf, no change is made to the corresponding control point.
                               
DLLEXPORT short cartGetVJVD(short port, short pol,
                            float *VJ1, float *VJ2, float *VD);                               
///< Read the junction control voltages and PA drain voltage for the given port and polarization.
///< The existing control values are returned as VJ1, VJ2, and VD.

//--------------------
// LO AMC

DLLEXPORT short cartSetAMC(short port, float VDE, float VGE);
///< Set or reset the AMC digital pots.
///< If both VDE and VGE are +Inf or -Inf then both are set to nominal (1.0, -0.1).
///< If VDE is not +Inf or -Inf then its value is set otherwise no change.
///< If VGE is not +Inf or -Inf then its value is set otherwise no change.

//------------------------
// Cartridge measurements

DLLEXPORT short cartMeasureFineLOSweep(short port, float tiltAngle, short pol,
                                       float VJ, float IJ, float fixedVD,
                                       double LOStart, double LOStop, double LOStep, 
                                       short repeatCount);
///< For the given port and pol, set both SIS junction voltages to VJ;
///< At each LO frequency from LOStart to LOStop by LOStep:
///<  adjust the LO PA drain voltage to get junction current IJ
///<  -or-
///<  set LO PA drain voltage to fixedVD if specified.
///<  record the resulting SIS current and PA drain voltage. 
///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent when finished.
///< pol may be -1, indicating both pols are to be measured.
///< If fixedVD is a valid value (not +Inf or -Inf) then use that VD setting and ignore IJ.
///< If either of VJ or IJ are +Inf or -Inf then the settings from FineLOSweep.ini are used.
///< If any of LOStart, LOStop, or LOStep are 0, then the settings from FineLOSweep.ini are used.

DLLEXPORT short cartMeasureIVCurve(short port, short pol, short sb, 
                                   float VJLow, float VJHigh, float VJStep, short repeatCount);
///< Take an I-V curve for the given port, pol, and sb.
///< pol may be -1, indicating both pols are to be measured.
///< sb  may be -1, indicating both sbs are to be measured.
///< If VJlow, VJhigh, or VJstep is +Inf or -Inf it will be computed from the nominal VJ.
///< Measurement happens on a worker thread.
///< Sends the event EVENT_IVCURVE_DONE via getNextEvent whene finished.                        

DLLEXPORT short cartMeasureIJvsSISMagnet(short port, short pol, short sb, 
                                         float IMagStart, float IMagStop, float IMagStep, 
                                         float VJLow, float VJHigh, float VJStep, 
                                         short repeatCount);
///< For the given port, pol, and sb, step the magnet current from IMagStart to IMagStop by IMagStep.
///< At each step, sweep the mixer voltage from VJLow to VJHigh by VJStep,
///<  if VJMin==VJMax, record the mixer junction current at each magnet step.
///<  else, record half the difference between the max and min junction current seen.
///< If sbMixer is -1, measure both mixers while stepping both magnets (or band-specific special cases.)
///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent whene finished.                        

DLLEXPORT short cartMeasureIFPowerVsVJ(short port, short pol, short sb,
                                       float VJStart, float VJStop, float VJStep,
                                       bool doYFactor, short repeatCount);
///< For the given port and pol, sweep both sidebands junction voltage
///<  from VJStart to VJStop by VJStep, record the IF output power.
///< If sb is -1, step both junctions.  Otherwise step one junction and leave the other set as it was.
///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent whene finished.                        

DLLEXPORT short cartMeasureIFPowerVsVD(short port, short pol, 
                                       float VDStart, float VDStop, float VDStep, 
                                       bool doYFactor, short repeatCount);
///< For the given port and pol, sweep the LO PA drain voltage
///<  from VDStart to VDStop by VDStep, record the IF output power from both sidebands.
///< repeatCount gives the number of times to measure. Measurement happens on a worker thread.
///< Sends the event EVENT_OPTIMIZE_DONE via getNextEvent whene finished.         

DLLEXPORT short cartMixerDeflux(short port, short pol, short sb, short IMagMax);
///< For the given port and pol, perform a coil demagnetization and mixer deflux operation.
///< sb may be 1, 2 or -1 specifying that both sidebands should be defluxed simultaneously.
///< IMagMax is the maximum magnet current to use in the demagnetization phase.
///< Produces an X-Y data set containing the magnet current and heater current vs. time.
///< Defluxing happens on a worker thread.  Sends the event EVENT_OPTIMIZE_DONE via getNextEvent whene finished.         

DLLEXPORT short cartAbortMeasurement(short port);
///< Stops any cartridge optimization measurement in progress on the given port.

DLLEXPORT short cartGetXYData(short port, short *size, float *X, float *Y, float *Y2);
///< Retrieve the X-Y data array from the last measurement on the specified port.
///< X, Y, and Y2 point to data arrays.  size, X and Y are required.  Y2 may be NULL.
///< *size specifies how many elements are in the arrays before the call
///<  and how many valid elements are there after the call.  Array memory must be pre-allocated.

//----------------------------------------------------------------------------
// IF Switch operations:

DLLEXPORT short ifSwitchSetAttenuation(short pol, short sb, short atten);
///< Set the IF switch attenuator(s) 0-15 dB
///< If pol < 0 or sb < 1, all four attenuators will be set the same.

DLLEXPORT short ifSwitchSetEnableTempServo(short pol, short sb, short enable);
///< Enable/disable the IF switch temperature servo.
///< If pol < 0 or sb < 1, all four temp servos will be set the same.

//----------------------------------------------------------------------------
// Cryostat monitor and control operations:

DLLEXPORT short cryostatSetEnableBackingPump(short enable);
///< Start/stop the backing pump.

DLLEXPORT short cryostatSetEnableTurboPump(short enable);
///< Start/stop the turbo pump.

DLLEXPORT short cryostatSetGateValveState(short open);
///< Open/close the gate valve.

DLLEXPORT short cryostatSetSolenoidValveState(short open);
///< Open/close the solenoid valve.

DLLEXPORT short cryostatSetEnableVacuumGauge(short enable);
///< Enable/disable the vacuum gauge.

DLLEXPORT short cryostatSetEnableCryoPumping(short enable);
///< Enable/disable the pump-down process.

//----------------------------------------------------------------------------
// FETIM operations:

DLLEXPORT short fetimSetTriggerDewarN2Fill(short enable);
///< Enable/disable the N2 fill trigger.

//----------------------------------------------------------------------------
// LPR operations:

DLLEXPORT short lprSetOpticalSwitchPort(short port);
///< Set the optical switch output port and disable the shutter.

DLLEXPORT short lprSetOpticalSwitchShutter();
///< Enables the shutter (disables the laser).

DLLEXPORT short lprSetOpticalSwitchForceShutter();
///< Enables the shutter (disables the laser) more forcefully (for debugging).
    
DLLEXPORT short lprSetEDFAModulationInput(float value);
///< Sets the EDFA modulation input value in Volts.

//----------------------------------------------------------------------------
// Functions to read structures containing monitor values.

DLLEXPORT short cartGetMonitorYTO(short port, CartYTOData_t *target);
DLLEXPORT short cartGetMonitorPhotomixer(short port, CartPhotomixerData_t *target);
DLLEXPORT short cartGetMonitorPLL(short port, CartPLLData_t *target);
DLLEXPORT short cartGetMonitorAMC(short port, CartAMCData_t *target);
DLLEXPORT short cartGetMonitorPA(short port, CartPAData_t *target);
DLLEXPORT short cartGetMonitorTemp(short port, CartTempData_t *target);
DLLEXPORT short cartGetMonitorSIS(short port, short pol, short sb, CartSISData_t *target);
DLLEXPORT short cartGetMonitorLNA(short port, short pol, short sb, CartLNAData_t *target);
DLLEXPORT short cartGetMonitorAux(short port, short pol, CartAuxData_t *target);
DLLEXPORT short cartGetLastHeaterCurrents(short port, short pol, CartLastHeaterCurrents_t *target);
DLLEXPORT short powerGetMonitorModule(short port, PowerModuleData_t *target);
DLLEXPORT short ifSwitchGetMonitor(IFSwitchData_t *target);
DLLEXPORT short cryostatGetMonitor(CryostatData_t *target);
DLLEXPORT short lprGetMonitor(LPRData_t *target);
DLLEXPORT short fetimGetMonitor(FETIMData_t *target);

//----------------------------------------------------------------------------

};  // extern "C"

#endif /*_LV_WRAPPER_FE_H_*/
