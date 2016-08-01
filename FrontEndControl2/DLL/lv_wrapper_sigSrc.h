#ifndef _LV_WRAPPER_SIGSRC_H_
#define _LV_WRAPPER_SIGSRC_H_
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

/************************************************************************
 * declarations for the functions exported from the DLL for consumption
 * by LabVIEW.  Strictly, this header is not required, since the
 * export declarations in lv_wrapper.cpp, coupled with the .def file
 * is all that is needed to export.  However, having the header is
 * useful for linking from a C or C++ test program.
 * 
 *----------------------------------------------------------------------
 */


#include "dllExport.h"
#include "lv_wrapper_FE.h"

class SignalSourceImpl;
namespace FrontEndLVWrapper {
    extern SignalSourceImpl *getSignalSourceImpl();
};

extern "C" {

// All functions return 0 for success or -1 for failure unless otherwise specified.

//----------------------------------------------------------------------------
// Functions for connection and disconnection to the front end.
// Also information available directly from the the embedded controllers.

DLLEXPORT short sigSrcControlInit();
///< Initialize the control connection to the signal source.

DLLEXPORT short sigSrcControlShutdown();
///< Destroy the control connection to the signal source.

DLLEXPORT short sigSrcExit();
///< Shutdown the firmware on the FE M&C module.

DLLEXPORT short sigSrcEnableMonitoring(short enable);
///< Start/stop the monitoring thread for the signal source.
///< Normally the signal source starts with monitoring disabled.

DLLEXPORT short sigSrcGetNextConfiguration(short reset, short *configId, char *description);
///< Iterate through the list of available configurations.

DLLEXPORT short sigSrcLoadConfiguration(short configId);
///< Configure the FE from the configuration database according to the given configId.

DLLEXPORT short sigSrcGetBand(short *band_p);
///< Get the cartridge band associated with the loaded configuration.

DLLEXPORT short sigSrcGetAMBSIInfo(char *serialNum,
                                   char *firmwareRev,
                                   unsigned long *numErrors,
                                   unsigned long *numTransactions);
///< Get the AMBSI1's monitor data.

DLLEXPORT short sigSrcGetAMBSITemperature(float *temperature);
///< Get the AMBSI1's temperature.

DLLEXPORT short sigSrcGetFirmwareInfo(char *AMBSILibraryVersion,
                                      char *FEMCFirmwareVersion,
                                      char *FEMCFPGAVersion);
///< Get the FE M&C firmware version information.

DLLEXPORT short sigSrcGetNumCartridgesOn();
///< ERROR              -1
///< 0-10 number of cartridges ON.

//----------------------------------------------------------------------------

};  // extern "C"

#endif /*_LV_WRAPPER_SIGSRC_H_*/
