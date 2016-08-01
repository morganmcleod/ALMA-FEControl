#ifndef _LV_WRAPPER_LORTM_H_
#define _LV_WRAPPER_LORTM_H_
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
 * export declarations in lv_wrappper_LORTM.cpp, coupled with the .def file
 * is all that is needed to export.  However, having the header is
 * useful for linking from a C or C++ test program.
 * 
 *----------------------------------------------------------------------
 */


#include "dllExport.h"
extern "C" {

struct LORTMStatus_t;
struct LORTMLasers_t;
struct LORTMOther_t;

// All functions return 0 for success or -1 for failure unless otherwise specified.

//----------------------------------------------------------------------------
// Functions for connection and disconnection to the LORTM.
// Also information available directly from the the embedded controller.

DLLEXPORT short LORTMControlInit();
///< Initialize the control connection to the LORTM.

DLLEXPORT short LORTMControlShutdown();
///< Destroy the control connection to the LORTM.

//----------------------------------------------------------------------------
// LORTM operations:

DLLEXPORT short LORTMGetFirmwareInfo(char *serialNum,
                                     char *protocolRev,
                                     char *firmwareRev); 
///< Get the LORTM firmware info.

DLLEXPORT short LORTMGetAMBSIInfo(unsigned long *numErrors, 
                                  unsigned long *numTransactions);
///< Get the LORTM AMBSI transactions monitor data.                                 

DLLEXPORT short LORTMGetAmbientTemperature(float *temperature);
///< Get the LORTM AMBSI temperature.

DLLEXPORT short LORTMResetAMBSI();
///< Reset the LORTM CAN bus interface

DLLEXPORT short LORTMResetDevice();
///< Reset the LORTM
    
DLLEXPORT short LORTMSystemManualModeRequest();
///< Put the LORTM in manual mode

DLLEXPORT short LORTMSystemStandbyModeRequest();
///< Put the LORTM in standby mode

DLLEXPORT short LORTMSystemLoadAllParameters();
///< Load all LORTM parameters from EEPROM

DLLEXPORT short LORTMSystemSaveAllParameters();
///< Save all LORTM parateters to EEPROM

DLLEXPORT short LORTMGetErrorList(char *errorList);
///< Get the list of all errors seen since last call to LORTMGetErrorList or LORTMSystemClearErrors.
///< Result is comma-separated list of error codes.

DLLEXPORT short LORTMSystemClearErrors();
///< Clear the LORTM error queue

DLLEXPORT short LORTMGetSystemState(unsigned char *state);
///< Get the LORTM System State value (enum) as defined by the ICD:
///< 0=Startup, 1=Interlock, 2=Standby, 3=Locking, 4=Operational, 5=Manual

DLLEXPORT short LORTMGetInterlockStatus(short *status);
///< Get the LORTM interlock status.  Nonzero=true interlock is on.

DLLEXPORT short LORTMGetPhaselockStatus(short *status);
///< Get the LORTM phaselock status.  Nonzero=locked.

DLLEXPORT short LORTMGetTuningReady(short *status);
///< Get the LORTM tuning ready status.  Nonzero=ready.

DLLEXPORT short LORTMGetRFInputReady(short *status);
///< Get the LORTM RF input ready status.  Nonzero=ready.

DLLEXPORT short LORTMPhaseLockTuningInit(short band, double freqLO1, double freqLO2, double freqFLOOG, short sbLock);
///< Initiate phase lock sequence for the given cartridge band for LO output frequencies freqLO1 and freqLO2 in GHz.
///< sbLock = 0 means lock below the reference:  freqREF = freqLO + freqFLOOG;
///< sbLock = 1 means lock above the reference:  freqREF = freqLO - freqFLOOG.

DLLEXPORT short LORTMPhaseLockTuningFinalize();
///< Finish the phase lock sequence.

DLLEXPORT short LORTMPhaseLockGetTuning(double *freqLSB, double *freqUSB,
                                        double *freqRef1, double *freqRef2,
                                        double *freqSlaveLaser, short *factorM);
///< Get the frequencies to be used for phase locking the LORTM.  Software readback only.
///< freqLSB, freqUSB, freqRef1, and freqRef2 must not be NULL.  
///< freqSlaveLaser and factorM are optional.

DLLEXPORT short LORTMLaserEnable(short laser, short enable);
///< For use in manual mode, enable or disable the selected laser.
///< 0=master, 1=slave1, 2=slave2, 3=slave3

DLLEXPORT short LORTMLaserSetFrequency(short laser, double freq);
///< For use in manual mode, tune the selected laser to the given offset frequency from the master.
///< 1=slave1, 2=slave2, 3=slave3

DLLEXPORT short LORTMLaserCalibrationSelect(short laser);
///< For use in manual mode, select the specified laser signal to appear at the calibration output port.
///< 0=master, 1=slave1, 2=slave2, 3=slave3

//----------------------------------------------------------------------------
// Functions to read structures containing monitor values.

DLLEXPORT short LORTMGetMonitorStatus(LORTMStatus_t *target);
DLLEXPORT short LORTMGetMonitorLasers(LORTMLasers_t *target);
DLLEXPORT short LORTMGetMonitorOther(LORTMOther_t * target);

};  // extern "C"

#endif /*_LV_WRAPPER_LORTM_H_*/
