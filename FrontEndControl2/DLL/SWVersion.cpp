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

#include "SWVersion.h"

const std::string FECONTROL_SW_VERSION_STRING("FEControl 2.17.1");

//******* Be sure to update resource.rc!


/* Revision history:

2.17.1
  Fix Fine LO Sweep data being inserted with DataStatus 1=PAS, should be 3=PAI

2.17.0
  SocketClientBusInterface:  working well with timeout logic disabled.   To futher simplify.
  Relative paths in makefiles, NGFemc fixes for regression test, script to install MinGW dependencies
  Fixed Sb2St3 monitors and commands for Band 1
  Fixed Cryostat set cold head hours error
  Added build directory for FrontEndICDTest
  Fixed some commands adding an extra byte.
  Changed the Cryostat set hours to the one mentioned in the ICD.
  Removed previous HACK from 2.16.4 and 2.16.5
  FrontEndDatabase fixNaN() on cart temp sensors
  Added calculatom of latency statistics for CAN and SocketClient
  Added CAN_measureLatency to INI file options.

2.16.5 NRAO-B2 2025-11-25
  HACK Use IF switch port 6 for operating band 2
  Added check for hasLED in ColdCartImpl.  Band 2 doesn't have one.

2.16.4 NRAO-B2 2024-11-21
  HACK Use IF switch port 6 for operating band 2

2.16.3 2024-08-08
  Band 2 signal source has an x6 multiplier not x4 like the LO.  

2.16.2 2024-02-29: 
  Cart Healthcheck:  store SIS and LNA settings to database along with readings.
  CryostatPumping:  switch when 4K stage is below 5 K and 15K stage is below 30 K.
  Record cryostat cooldown plot

2.16.1: 2024-02-22
  Build with mingw-w64-i686-gcc 13.2.0-5

2.16.0 2023-03-30
  Add freqBeatNote param to LORTMPhaseLockGetTuning

2.15.3 NRAO-B2
  Special version for operating the NRAO band 2 prototype cartridge
  Add freqBeatNote param to LORTMPhaseLockGetTuning

2.15.2 2023-01-25
  Roll back monitor queue manipulations which were side-effect of setting SIS voltage and magenet voltage
  (ColdCartImpl.cpp 4a2466fe6)

2.15.1 2023-01-05
  When monitoring cryostat temps, if out of range (0...350) or NaN return -1.

2.15.0 2022-12-22
  Add DLL function cartGetLastHeaterCurrents to get average off and on currents from mixer heating process 
  Print firmware versions on startup
  Added Teledyne PA subsystem to FEMCErrors

2.14.1 2022-10-25
  LM_INFO logging
  fix loading FrontEndControlDLL.ini [configuration] with providerCode!=40
  LM_DEBUG logging
  Changes made to improve performance in the Bias Module Test Set:
  Remove all RandomizeAnalogMonitors functions
  Simplified monitorAction() for most classes having monitor threads 
  Modified the monitor thread action queue so that items can be inserted at the front for one-time monitoring
  All ColdCart control points now push the respective monitor action onto the front of the queue.
  Added FrontEndControlDLL.ini file options:
  [debug]
  sweepSISVoltage=0|1     ;Normally true: for bands 7 and up, gradually sweep SIS voltage to target value
  sweepMagnetCurrent=0|1  ;Normally true: gradually sweep SIS magnet current to target value

2.13.3 2022-08-18
  Extra debugging in cartGetMonitorSIS

2.13.2 2022-08-15
  LM_DEBUG logging

2.13.1 2022-07-07
  WCA multiplication factor = 4 for band 2 ESO first article

2.13.0 2022-07-01
  Add monitor and control for band 7 Teledyne PA chip.

2.12.1.1 2022-03-17
  Don't report Cryostat serial timeout errors

2.12.1 2022-02-23
  Logging level back to LM_INFO
  Add support for FEMode=3 simulation
  
2.12.0 2021-12-13
  Added LockingStrategy class used by CartAssembly to select a strategy at runtime
  Added 4 locking strategies, selectable from FrontEndControlDLL.INI or via cartSetLockingStrategy()
  0: original FETMS strategy with no LPR EDFA adjustment
  1: ALMA cycle 8 strategy with LPR EDFA adjustment
  2: Giorgio's 5-Points strategy, with LPR EDFA adjustment
  3: Giorgio's 9-Points strategy, with LPR EDFA adjustment
  Ripped out all the 'tryAlternateLockMethod' stuff which was unused since 2016

2.11.1b2 2021-10-01 (NOT MERGED)
  Added querying AMBSI command queue to MeasureIVCurve

2.11.1b0 2021-09-08
  Fix I-V curve default numPoints.
  
2.11.0 2021-09-07
  Fix compiler warnings, though some still suppressed.
  Compiled with msys64-mingw32 GCC 10.3.0

2.10.7b2 2021-08-19
  Log CAN bus timings to stdout - commented out
  Check hasMagnet in queryCartridgeState()

2.10.6 2021-02-10
  Additional monitor point filtering for band 1&2 (SIS and heater) 

2.10.5 2020-03-01
  Added to CppUtilityLib: listDir, stringSet, trimStr
  Added read ESNs at startup
  Removed all uses of keyFacility from CONFIG and the lv_wrappers.   
    keyFacility has never been implemented correctly in the database.
  Deleted ConfigProviderMySQL.
  ConfigManager no longer uses ConfigProvider.
  Improved comments in namespace FrontEndLVWrapper.   Deleted obsolete logLVStructures option. 
  Updated error messages to match Rev.D ICD.  Simplified FEMCErrors.cpp
  Added check for hasMagnet() and fixed hasSb2Magnet() to prevent monitoring errors in band 3&4
  Moved ColdCart monitor registry from ColdCartImplBase to ColdCartImpl
      
2.10.4.1 2020-02-23 Band 1 integration
  Removed divide-by-2 for band 1 RF source

2.10.4 2019-06-18
  AMC_MULTIPLIER_D_VOLTAGE -> AMC_MULTIPLIER_D_COUNTS, same for all var names
  getCenterLOFrequency() defaults to 80 GHz for band 2

2.10.3 2019-05-09
  getDataPath() ensures path has backslashes not forward slashes.
  Changed INI file name key "TeraxionLORTM" -> "LORTM"

2.10.2 2019-04-25
  Moved FrontEndIni loading and logDir override into lv_wrapper.cpp so that it works consistently 
  when SignalSource.vi or LORTM.vi is executed first.
  fix1: Fixed loading FineLOSweep.ini filename.
  
2.10.1 2019-04-23
  Added DLLEXPORT short FEGetConfigSN(long *configId, char *serialNum);
  When loading CCA and WCA config, if XML key is present load config from the specified XML file.
  Added option to load the FrontEnd configuration from a separate ini file, specified in [configFiles] FrontEnd=???
  If found, overrides logDir to use the same directory.
  Removed ConfigFromDatabase option.
  Now requires libexpatmm-0.dll

2.10.0 2019-02-25
  Added export sigSrcIsErrorStop() to check whether signal source has stopped because of CAN errors.
  Improve FEHardwareDevice error stop semantics.

2.9.3-beta1
  Always create a new TDH when measuring Fine LO Sweep
  Set repetitive Fine LO Sweep log messages to LM_DEBUG in CartAssembly

2.9.2 2018-10-17
  Changed logging of found ESNs

2.9.1 2018-09-26
  Added logging of monitor timers to use with experimental version of AMBSI1 firmware.

2.9.0 2018-08-29
  Add cartSetCenterLOFrequency(); refactor OPTIMIZE/CartHealthCheck.

2.8.9 2018-06-18
  Fix crash on shutdown: was deleting a FILE* returned by fopen().

2.8.8 2018-05-07
  Fix crash shortly after band startup.  (FETMS-201)
  Crash was caused by uninitialized floats in ColdCartImplBase.
  Added ExcHndl.DLL linkage so that all crashes will be logged to the logDir. 

2.8.7 2018-04-13
  Setup to load the DLL into  LabVIEW from the build directory.
  Mutex in lv_wrapper protects client count, valid, and other shared data.
  delete CorrectSISOnMainThread option. Constant defaults false in OPTIMIZE/MeasureSISVoltageError.cpp
  delete deprecated exports FEGetNextSerialNum() cartGetLOPADrainVoltageSetting() setCorrectSISOnMainThread()
  delete junk TODOs.
  Made correctSISVoltageError private to FrontEndImpl.  Called in SetCartridgeOn()
  modified I-V curve algorithm to always sweep towards VJ=0
  added VJstart0 and VJstart1 options for Optimize IF Power
  changed FEHardwareDevice::startMonitor() to initialize stopped=false
  fixed crashing sprintf usage in FEMCErrors.cpp
  fix ColdCartImplBase member vars initialization

2.8.6 2018-02-01
Added I-V-Power curves to Optimize > Measure IF Power vs VD

2.8.5 2017-12-18
Removed static FEHardwareDevice::maxErrorCount and INI file setting.
  instead each instance has a maxErrorCount_m defaults to 0.
  HW_BLKED is now counted as an error.
  FETIM sets it to 1 so FETIM pauses monitoring when an error is seen.
  2.8.4 from 2017-12-15 had a fatal bug in this new mechanism.
  ConfigProviders (INI & MySQL) assume FETIM=1 unless explicitly set as FETIM=0.
  FETIMImpl: monitoring returns error if maxErrorCount exceeded.

2.8.3 2017-11-30
Removed using pointer to member functions in get/set SIS bias.
  Now correctly (?) calling ColdCartImpl members, not ColdCartImplBase!

2.8.2 2017-11-17
Delay 1.5 sec for all bands before measureSISVoltageError
Fixes crash in lock test?

2.8.1 2017-11-09
Option and DLL interface to run measureSISVoltageError on the main thread.
Modified startup sequence and configuration so that FELoadConfiguration can be called to reload settings.
CartridgesContainer prints only the ESNs of the latest added cartAssy, not all ESNs.

2.8.0 2017-11-02
Prevent IF power measurements when called from FETMS Automated Test
  Changes DLL interface for cartHealthCheck() 

2.7.0 2017-09-19
Released version for CDL, OSF, ASIAA

2.7.0-beta5
Loading LoopBW override from FrontEndControlDLL.ini or SignalSource.ini
  Resolves previous HACK!

2.7.0-beta4
Added CartAssembly::overrideLoopBW()
  HACK! temporarily set to false for band 2 B02-2

2.7.0-beta3
Changed interface to FEStartHealthCheck():
  pass dataStatus or 0 to use default.
  removed fullAuto option.

2.7.0-beta2
Added and fixed FEGetConfiguredBands()

2.6.3-beta1
Gate health checks for cryostat, LPR, IF switch.

2.6.2 2017-02-28
Turn off logAmbErrors when CAN_noTransmit

2.6.1 2017-02-27
2.6.1-beta4
Rebuilt with -lpthreadGC-3.dll

2.6.1-beta2
adjustLOPASinglePol using MMPROPOSED algorithm again.  
  Wait for computing to decide http://ictjira.alma.cl/browse/ICT-6284
Added DLL version resource
queryCartridgeState(): round to 100 MHz.  Fix bugs computing freqREF_m.

2.6.1-beta1
adjustLOPASinglePol temporarily using FECONTROL2_5 algorithm

2.6.0 2017-02-20
New build system GCC 5.3.0, pthreadGC-3.dll using MinGW 0.6 libs, not separate 3rdparty
lv_wrapper: 
    added cartClearOptimizedResult();
    added cartGetOptimizedResult();
    don't setCartridgeOn() and measureSISVoltageError() if already on.
Changes for band 3 bias optimization:
    CartAssembly::clearOptimizedResult()
    CartAssembly::getOptimizedResult()
CartridgesContainer::pauseMonitor don't report "User Command"
added MixerParams::streamOut() asIniFileRecord
makefile: link with -s to reduce size of DLL.

2.5.4 2017-01-09
Added command to send STANDBY2 mode
STANDBY2 allowed for band 1 and 2  (2.5.4-beta2 not checked in.)

2.5.3 2016-12-31
Fixed bug loading config item [logger] logAmbErrors
Added RCAS for CRYOSTAT_COLD_HEAD_HOURS and EDFA_PHOTODETECT_COEFF
WCAImpl::getAltLoopBW fix for prototype band 2

2.5.2 2016-11-08
New lock search and detection strategy for band 1 called LOCK_DETECT_2XVOLTAGE.
The former experimental "Alternate" lock strategy is now called LOCK_DETECT_CORR_V.
Normal lock strategy is called LOCK_DETECT_VOLTAGE.

2.5.1 2016-11-07
Warm multiplier = 2 for the band 1 source only, not LO.
Band1 using alternate LO loop BW (which has no effect, actually.)
Fix compiler warnings.

2.5.0 2016-08-17
Cryostat pumping process: Report error if backing pump is not on when started.
GetNextEvent now includes an auto-incrementing sequence number.
  This breaks LV code relying on the 2.4.x GetNextEvent interface.
Removed redundant and unused utility functions from FrontEndAMBLib. 

2.4.8.3 2016-04-07
Mixer heater operation supported for band 3.
No error in cartGetMonitorSIS for CCAs with no sb2.  Just return zeros.

2.4.7 2016-02-08
Removed cartGetLOPADrainVoltageSetting() from implementation.  Still in interface.
- Added cartGetLOPowerAmpsSetting() which also returns enabled state and gate voltage settings.
No error in cartGetMonitorSIS for CCAs with no SIS.  Just return zeros.

2.4.6 2016-01-28
Testing ALMA SW algorithms in CartAssembly::adjustLOPASinglePol()
- Switched to my proposed variation on the 3-phase ALMA SW algorithm.   http://ictjira.alma.cl/browse/ICT-6284

2.4.5 2015-12-02
Don't return error for bands 1&2 in CartAssembly::setSISBias, setSISMagnet, adjustLOPowerAmps 

2.4.5-beta3
More debugging sigSrcGetBand()

2.4.5-beta2
Added sigSrcGetBand()
Fix warnings
Added DllMain

2.4.4 2015-08-18
Don't monitor/control SIS for bands 1 and 2.
Range checks and error messages on LO PA drain and gate voltage settings.
ThermalLogInterval=0 disables thermal logger.

2.4.3 2015-06-01
LO frequency included in filename of IF power sweeps.

2.4.2 2015-05-28
Band 2 specific changes
Cleanup of band 5 specific changes for LORTM
Testing changes to ThermalLogger shutdown to prevent crashes:
- Made ThermalLogger::stop() wait for the log stream to be NULL.
- TODO: remove caching of string name_m in OptimizeBase::optimizeThread

2.4.1 2015-03-29
FEHardwareDevice::clearLogger()

2.4.1-beta2 2015-03-16
Changes for band 5 production cartridge (was preproduction):
- ColdCartImpl::measureSISVoltageErrorImpl test voltage is 2.5 mV
- No longer reversing the sense of LO PA voltage  0 <--> 2.5 V
- Mixer Deflux IMag max = 30 mA

2.4.0 2015-03-15
Built with NICAN header and lib from NI-CAN 14.0.

2.3.25 2015-03-02
Fix LORTM getRef2DivideBy to use the CCA multiplication factor rather than a switch statement.

2.3.24beta1 2015-01-30
Update band 5 for production WCA and CCA.

2.3.23beta1 2015-01-23
Experimental:  When measuring SIS current vs magnet current, repeatCount is used
 as the number of times to repeat the inner measurement loop over VJ.
 Will this improve S/N ratio of the measurement? 

2.3.22 2015-01-20
Cold health check is allowed if either the 4 K or 18 K stage temps are below 30 K.
 Previously both had to be below the threshold.

2.3.21 2014-06-02
Fixed bug in waits for adjustLOPowerAmps() introduced in 2.3.20.
Added FrontEnd::getNumErrors() and getNextError().  Added FEMCError.cpp
FEControl app now displays FEMC module errors in the message history window.
Added FEMode setting to FRONTENDCONTROLDLL.INI [debug] section:
 0=Operational
 1=Troubleshooting 
 2=Maintenance

2.3.20 2014-02-07
Testing health check waits for adjustLOPowerAmps() to finish up to 45 seconds. 

2.3.20-beta2 2014-01-30
Testing alternateLockTest method for bands 9 and 10.   
  set tryAlternateLockMethod=1 in FRONTENDCONTROLDLL.INI [debug] section to enable.
  set defeatNormalLockDetect=1 to force lock detect to read as zero for bands 9 & 10. 

2.3.19 2013-11-22
Set band 5 warm and cold multipliers (6, 2).
Set band 1 warm and cold multipliers (1, 1).
Fine LO sweep now supports measurement at a fixed LO PA drain voltage.

2.3.18 2013-07-19
FETIM monitor data mostly defaults to 'bad' state and only goes to 'good' after monitoring.
Now returning ESNs with byte order 'not reversed' e.g. family code first, checksum last.
Increased FETIM monitor polling rate 5x.

2.3.18-beta2
Fix errors and warnings found by GCC version "(GCC) 3.4.5 (mingw-vista special r3)"
Fix not monitoring FETIM He buffer tank pressure.

2.3.18-beta1
Added a logging statement for when the TestDataHeader is reused for fine LO sweep.
Fix logging of IF total power and Y-factor data, don't log SB2 for bands 9 & 10. 

2.3.17 2012-07-18
Fix range-check bug preventing band 4 mixer heaters from operating.

2.3.16 2012-07-12
Fix (LV) bug where IF0 and IF1 power data were being swapped when inserted into the FEIC database.

2.3.16-beta1 2012-06-27 
Mixer heating to 12K for band 8.
Fixed?  Band 5 automatically measured unpumped I-V curves were not done right.

2.3.15 2012-06-21
Added support for mixer heating band 4.
Added 5-minute timeout to cooldown loop after mixer heating.

2.3.14 2012-06-07
New compiler tools.  Fixed a bunch of warnings.
MixerDeflux:  IMag_m and IMagMax_m are now float instead of int.
Added band 5 Mixer demag/deflux procedure.
Mixer heating process, all bands:  Now waits for temps to return to nominal + 0.2 K.

2.3.13 2012-04-24
Fixed bug in MaximizeIFPower (band 3 bias optimization) which was introduced in 2.3.11:
  Would quit if an oscillation was detected.  Now continues as before.
Changed behavior of OptimizeLOPowerAmp (PA Adjust) so that if it hits the PA VD limit,
  it reverts to the setting from the configuration file instead of zero.

2.3.12 2012-04-12
Fixed more database-related bugs for PAS data collection.
GUI no longer pops up IF Power Data Grid when only a WCA is present.

2.3.11 2012-04-04
Fixed database insert bug for bands 9 & 10 IF power and Y-factor.
Fixed bugs which prevented STOP button from working for I-V curve, fine LO sweep, other optimization measurements.
Fixed minor bugs and improved error reporting to the user in band 3 bias optimization.
Minor adjustments to database error logging.

2.3.10 2012-03-23
Changes to support band 10:
  PLL loop bandwidth setting.
  Temporary fix to reverse LO PA control channels is still in the code but #ifdef-ed out.
  (FEMC firmware 2.6.0 and before has the channel mappings wrong for band 10.)
  Band 10 I-V curves go to +/- 3.0 mV.
  special timing and behavior for band 10 mixer deflux, may need more work.
  special timing and behavior for band 5 mixer deflux, pending comments from cartridge builder.

2.3.9 2012-03-16
Fix bug where Y-factor not getting saved properly to database.
Additional error checking on band/port number in DLL wrapper.

2.3.8 2012-03-13
New strategy for mixer and magnet biases lookup tables:
  If the LO frequency interval between two rows in the bias table is <= 1 kHz, do not interpolate.
Fix bug where FLOOG dist. module component not getting written to test data header.
Added database support for IF power and Y-factor PAS reference data measurements.
New event type from DLL for "cart health check done" causes GUI to display IF power data grid. 

2.3.7 2012-02-16
Fixed default VJMax setting for band 4.
Fixed bug in text file report produced by band 3/4 bias optimization:  
  Measured SIS1 voltage was reported for both SIS1 and SIS2 in the summary line at the end of the process.
Fixed bug in text file report for Y-factor vs. PA VD measurement.
Moved SWVersion file into DLL source folder. 

2.3.6 2012-01-27
Fine LO Sweep settings come from an FineLOSweep.ini file instead of from the GUI.

2.3.5 2012-01-26
Band 8 bias optimization:  Deflux procedure heats mixers to 20 K instead of 12 K.

2.3.5-beta1
Experimental version for using the band 3 optimization method on band 4.
Code reverted.  Key diffs retained in CartAssembly-B3forB4.cpp and OPTIMIZE/MaximizeIFPower-B3forB4.cpp

2.3.4 2011-12-20
Added pause to CartHealthCheck::prepare to allow PA adjust to complete.
Known bug remains:  Pol0 SIS1 voltage setting on front panel displays an improper value during and after health check.  
  Receiver state is actually correct.  Health check data recorded is also correct.
Known bug remains:  If tilt table is powered off, GUI is very slow and hesitant to react to clicks.

2.3.3 2011-11-28
Bug fixes and corrected data processing of PAS reference and health check data.
Back to 'reversed' ESNs readout for now.

2.3.0 2011-10-31
Fixed bug where magnets were not being operated correctly in measureSISVoltageError.
Moved LORTM settings into TeraxionLORTM.ini
Fixed MonCtrlMacros so that CAN bus errors return monitor data =0.
IV Curves return current in uA.
Software Version string now comes from SWVersion.cpp.
Inserting Fine LO sweep data into database.
Inserting I-V curve data into database.
Supress adjustLOPASinglePol verbose logging messages during FineLOSweep.
Improved input detection for fine LO sweep.
Less logging for fine LO sweep.
Fine LO sweep supports pol=Both.

2.3.0-beta5
PAS reference data and health check data inserted into the database.
Refactored measureIVCurve so that it may be run synchronously from cartHealthCheck.
Created CartHealthCheck worker thread object.

2.3.0-beta4 2011-09-29
Added support for FETIM.
Added support for Compressor M&C module.
Updated FE Control GUI for FETIM and new Cryogenics tab.

----

2.2.9-fix16 2011-09-16
setEnableLNALEDs was returning error even when successful. 

2.2.9-fix15 2011-08-17
Don't waste time trying to demagnetize sb2 on band6.

2.2.9-fix14 2011-08-04
New behavior for MeasureSISCurrent vs magnet current: same as 2.2.9-fix4 except for band 6 where 
  SIS1 current is returned as Y1 and SIS2 as Y2.
Band 3 optimizer fix to logging results even after fail. 
Refactoring: Moved utility functions into CppUtilityLib; Moved database functions into FEICDBLib.

2.2.9-fix13
Rolled back MeasureSISCurrent vs magnet current to 2.2.9-fix4 because newer version was incompatible with RAL optimization SW. 
Uses C++ streams instead of sprintf() in CryostatImpl::appendThermalLog to prevent problem temp sensor from causing crashes.

2.2.9-fix12 2011-07-27
Increased speed of cryostat monitoring to cover all monitor points every second instead of three seconds.
Band 3 optimizer still logs results even if it failed due to oscillation.
Trapping all exceptions in CryostatImpl::appendThermalLog because a problem temp sensor was causing crashes.
Added gate valve driver test button to FEControl front panel.  Repetitive testing of the gate valve open/close.

2.2.9-fix11 2011-07-01
Added optimization measurement modes for band 6:
- IF power vs. SIS voltage measurement  
  SIS2 is swept through positive voltage range while SIS1 is simultaneously 
  swept through the inverse, negative voltages.
- SIS current vs. magnet current measurement
  Both SIS1 and SIS2 currents are measured simultaneously while sweeping the SIS1 magnet current control.
LO frequency for band 9 health check is now 662 GHz.
Made fine LO Sweep measurement more robust.
Fixed default ranges for I-V curves, especially band 6.
Fixed I-V curve measurement and SIS current monitoring bug.  Was averaging one fewer readings than requested.
Fixed bug where WCA state not getting set up correctly at program start, having random side-effects.
Re-enables monitoring earlier in the final steps of MixerDeflux.

2.2.9-fix10 full package for release 2011-05-20
Front panel 2.2.14
Changed LO PA monitor indicators to not have "A" and "B", only "Pol0" and "Pol1" 

2.2.9-fix10 2011-04-13
Fix bugs in band 3 bias optimization methods.
Improve band 3 bias optimization log output.

2.2.9-fix9 2011-03-25
Changes to support band 5:
  Cold multiplier is 6; warm multiplier is 2.
  Typical SIS voltage is 2.3 mV; typical SIS current is 60 uA
  Four magnet circuits.
  Maximum magnet current is 4 mA
  Wait 60 seconds after heating before energizing magnets.
  LO PA puts out max power with VD=0 and minimum with VD=2.5.  So the setting command will be 2.5 minus the given value. 
  Center LO frequency (for cartridge health check) is 181 GHz.
  Added support for measuring IJ vs. magnet current.
Added command to tell FEMC module to rescan the 1wire bus for ESNs.  

2.2.9-fix8 2011-03-10
Changed band 3 bias optimization to use a version of the script being used by the band 3 cartridge builders.
Added oscillation detection to band 3 bias optimization.
Added a 10 second delay at the end of band 3 bias optimization to work around LabView reporting an exception. 

2.2.9-fix7
cartSetEnableLO now turn off the band 3 signal source AMC.
New configuration switch in [logger]: logAmbErrors defaults to 1.  If 0, suppresses logging of CAN timeout error messages and others. 
HealthCheck logs AMBSI serial number.

2.2.9-fix6
Try: In CAN bus commandImpl() calls ncWaitForState(NC_ST_WRITE_SUCCESS) after every write to make sure
  that the data was written to the bus before proceeding.  Previously didn't think this was needed
  but now troubleshooting intermittent AMBSI1 timeout/lockup problem.
Fix: Fine LO sweep was using SIS voltage and current setting from the INI file rather than from
  the front panel.  This got broken in 2.2.9.

2.2.9-fix5 2011-02-24
Fix: ThermalLog output file had swapped headers ('gate valve' and 'solenoid valve'.)  Thanks JP!
Fix: HealthCheck logging for CPDS had extra freqLO column.
Fix: logMonitors output for CPDS had 24V voltage twice and no 24V current. 

2.2.9-fix4 2011-02-17
MixerDeflux process: Uses the nominal magnet current settings from the configuration file when
  setting the magnets at the end of demagnetization/defluxing.

2.2.9-fix3 2011-02-16
MixerHeating process:  Simplified loop.  No longer logging to FELOG inside loop.  20 ms loop delay.
MixerHeating process:  Makes sure that SIS magnets are off before heating.  
  Alert: does not return magnets to prior state after heating.  The MixerDeflux process will take care of
  that when it uses the MixerHeating routine.  When it is used stand-alone there is no safe way to
  re-enable the magnets after heating without adding a delay for the mixer temps to come back down.

2.2.9-fix2 2011-02-16
MixerHeating process:  Adjustments to loop timing.   Now 100 ms delay in outer loop after enabling
  heater and no delay in inner loop.

2.2.9-fix1 2011-02-15
Cartridge configuration gets written out to log file at the start of cartridge health check mode.
MixerHeating process:  Loop delay is now 25 ms instead of 100 to allow more granularity and time for 
  logging statements to print.
MixerDeflux: Perform MixerHeating with default target temperature and timeout after demagnetization. 
  This is followed by a pause of 2 seconds for band 9 and one minute for bands 678 to allow mixer temperatures 
  to return to normal.  Finally the magnet current is swept to nominal+10%, to 0, back to nominal.  
MixerDeflux: Fix bug where LO PA state was not being returned to prior value in cases where the PA was already
  off prior to starting the procedure.
MixerDeflux: Fixed (?) progress bar behavior.
Measure SIS current vs. magnet current:  At the end of a scan, sweep the magnet current down to 0 
  then back up to the prior/nominal value.  After this, sweep the SIS current up to nominal.

2.2.9 2011-02-03
Added detailed control of mixer heating process to the LabView interface.
ESNs are now displayed in the opposite byte order from previously, but in line with the majority of component builders.
Added FE and cartridge health check functions.
New format for loading signal source configuration from INI file.
Signal source starts up displaying the hardware state correctly (implemented SignalSourceImpl::queryCartridgeState.)
Bias settings are now retuned whenever the LO frequency is changed.
Refactored optimization exitAction(), FEMCEventQueue, logDir

2.2.8 2010-12-21
Removed option to log CAN transactions to the MySQL server. 
Removed logFile, logFileReplace, LogMySQL, and saveConfigToIniFile options from configuration file.
All bias settings are echoed to log file with fixed precision -- easier to read.
correctSISVoltageError is now enabled by default.

2.2.7 2010-11-17
Support for stand-alone FEMC module and signal source (BeaSTS).
All optimize and I-V curve operations now have a stop button.
CryostatPumping process:  Close the solenoid valve and turbo pump, even when gate valve fails to close.
Added subscribeForEvents to FEControl Init and Shutdown.
Fix: Flush log stream before deleting it at shutdown.  Some final entries were getting lost.
Fix: Bias values in logs now given in fixed point.
Fixes for compiling with GCC 4.5.0
FrontEndControl GUI now uses power meter and chopper drivers directly for better performance.

2.2.6 2010-05-12
Added longer delays when powering on cartridge before measuring SIS bias offset.
Added band 4 and 8 bias optimization support.
Increased timeout while waiting for power meters from 2 seconds to 10.
Optimize LO power detects oscillations around the nominal value.  Always picks the higher value.
Skips adjusting LO PA if target IJ01 or IJ11 is 0.
Increased averaging when monitoring SIS current.
Bug fix affecting band 7 mixer heating.
Refactored FrontEndAMBLib to support ICD unit test suite.
Default config file includes warm bias values for bands 4 & 8.

2.2.5.fix2 2010-03-05
Optimize LO Power now done to within 3% of IJ target rather than fixed 5 uA.

2.2.5.fix1 2010-01-18
Added monitoring LORTM SystemGetInterlockStatus and calibration coefficients.
LORTMGetErrorList does not clear the errors list.

2.2.5 2010-01-15
All monitor points may be optionally dumped to the log with logMonitors=1 in config file.
The order in which analog monitor points are read can be randomized.
SIS and magnet monitor values are now returned even when not biased.
CPDS monitors are now returned even when cartridge is off.
Changed PLL monitoring to get real-time values for key lock monitor points.
LORTM accounts for FLOOG offset when setting output frequencies.  WCA does not.
Made LORTM reference laser frequency configurable. 
Bugfix: workaround for LORTM ambient temp sensor bytes in wrong order.
Bugfix: GCC & std.lib. 3.4.5 fixes logging-related crash.
Bugfix: LORTM error reporting.

2.2.4 2009-12-01
Removed monitor point for SIS heater enable because was removed from FE-Computing ICD.
Removed FLOOG offset calculation from LORTM locking; was causing LORTM to lock 30 MHz away from nominal LO ref.
(But this needs to be put back... the FLOOG offset should be applied to the LORTM, not the LO!)
Added delays to LO locking algorithm to account for YTO slewing specs, based on comments from Eric Bryerton.
Extended timeout waiting for power meter readings while optimizing.
Added methods required for image rejection measurement:  
- Get LO PA drain voltage setting
- Set LO PA drain voltage as a percentage of maximum
- Enable/disable WCA LO Output
- Added cartGetLocked method to query the lock status from LabVIEW.

2.2.3.fix2
Fix timing bugs in LORTM methods to get status. 

2.2.3.fix1 2009-09-08
IV curve log file now named for pol and sb.
Fix only error status messages (for GUI) generated in CartAssembly.  All ok status msgs moved to FrontEndImpl
Fix bug where finding no ESNs kills the GUI.
Fix bug where an error in MeasureFineLOSweep could lead to an infinite loop 

2.2.3 2009-08-20
Added reading of all installed electronic serial numbers (ESNs.)
Added option to measure SIS bias setting error on cartridge power-on and then subsequently use to correct bias settings.
Added support for operating a cold cartridge with no WCA (for bias module test set.)
Save Window screens are now written to logDir;  removed path control from GUI.
Cartridge state control buttons (enabled, observing, SIS, etc.) are initialized to the actual state of the hardware. 
Mixer heater operation now logs temperatures and currents to logDir.
I-V Curve now writes both the set and readback values for Vbias to its output file.
Fixed bug where PLL addjust was not being performed after successful lock.
Fixed bug where mixer deflux would not measure heater current during heating.
Reading of SIS voltage and current both support averaging and printing data points to the FE log.

2.2.2 2009-06-22
Added separate executable for chopper and signal generator test programs.
Added status bar and status message history dialog.
Added fine LO stepping measurement, replacing cartStepMeasureLOPowerAmps (introduced in 2.2.0.)
Fixed thermal logging which was broken somewhere along the way.   Now it is always on and sent to logDir.
Added PA Adjust function to Band 3.  This will be useful when characterizing several WCAs with a single cold cartridge.

2.2.1 2009-06-09
Improved error messages when unable to connect to FE, power meters, or chopper.
Failure to connect to power meters does not prevent use of chopper.

2.2.0 2009-05-28
Added power meter readouts page.
Integrated control of the power meter and the chopper.
Added check for successful connection to the front end on startup (not working well yet.)
Added cartStepMeasureLOPowerAmps method.  This is a special diagnostic method not for normal use:  
- Steps the YTO and adjusts the PA drain voltage to hold the mixer current constant.
Refactored/simplified optimization and measurement routines.
Log now goes to a specified directory and is named automatically with the date and time.
I-V curve and optimization-related measurements create a data file in the log directory.
Added measurement of SIS junction current vs. magnet current.
Added measurement of IF power vs SIS voltage or LO PA Drain voltage.
Updated band 3 optimization routines to latest version from cartridge manual.
Band 7 and 9 optimization methods are fully supported, but not fully automated.

2.1.9 2009-03-06
Reduced NICAN queue sizes so that CAN0 and CAN1 can both be used at once.
Fixed bug where optimizing the LO power amp could stop short due to unsteady junciton current readings.
- It now averages three current readings at each step.
Fixed bug where WCA is in minimal monitoring mode when first powered on.
Fixed annoyance where Zero Integrator gets turned off whenever LO frequency is set.
Fixed library+GUI bugs in manual setting magnet current.
Fixed GUI bug in manual setting junction voltage.
Changed cryostat temp sensor reading method to workaround slow sensor electronics.

2.1.8 2009-02-13
Adopted Matthew's version of the GUI with the Save Window button.
LORTM refinements.

2.1.7 2008-12-18
Software now displays version number on the front panel.
Removed need for "Do init" control in LabVIEW client.  Library supports multiple simultaneous client apps.
Added LORTM support.
Monitoring may be turned off for individual CCs and WCAs
Band 3 optimization:  added chart, software now sets power meters to Watts.

2.1.6 2008-08-15
Fixed bug which gave bad data when reading INI file.
Added support for operating WCA and CC with no CPDS (CPDS=0 in INI file.)
Useful if external power supplies are used instead of CPDS.  

2.1.5 2008-07-22
Simplified CAN & SocketClient send/receive queue code.  
I belive the crash bug was lurking there but I did not find the exact bug mechanism.

2.1.4 2008-06-25
Changes to SIS/Magnet GUI screens.
Added controls for manual setting of magnet current.
Converted adjustVJVD function into separate setVJVD and getVJVD funcs.

2.1.3 2008-06-20:
Fixed(?) multi-threaded logging issues causing crashes, especially when optimizing PA drive.
Added INI file control of SIS open/closed loop. 
[Note added 2008-07-22]:  Crash bug is not fixed in this version.

2.1.2 2008-06-12: 
Fixed bugs found while operating in Chile.
Removed 'queryCartridgeState' action on startup -- was causeing EU & EA systems to freeze.
Consequence of this is that the software does not show which cartridges are on and biased at startup.

2.1.1 2008-05-15: 
Added socket client capability.  Needs testing and debugging.
Made configuration from INI file the default, rather than config from database.

2.1.0 2008-04-24: 
Initial release to the EU-FEIC and EA-FEIC.

prior to 2.1.0: Internal release to NA-FEIC only.
*/
