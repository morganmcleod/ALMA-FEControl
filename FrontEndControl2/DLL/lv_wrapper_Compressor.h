#ifndef LV_WRAPPER_COMPRESSOR_H_
#define LV_WRAPPER_COMPRESSOR_H_
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
/************************************************************************
 * declarations for the functions exported from the DLL for consumption
 * by LabVIEW.  Strictly, this header is not required, since the
 * export declarations in lv_wrapper_Compressor.cpp, coupled with the .def file
 * is all that is needed to export.  However, having the header is
 * useful for linking from a C or C++ test program.
 *
 *----------------------------------------------------------------------
 */


#include "dllExport.h"
extern "C" {

struct CompressorData_t;

// All functions return 0 for success or -1 for failure unless otherwise specified.

//----------------------------------------------------------------------------
// Functions for connection and disconnection to the Compressor.
// Also information available directly from the the embedded controller.

DLLEXPORT short CompressorControlInit();
///< Initialize the control connection to the FE.

DLLEXPORT short CompressorControlShutdown();
///< Destroy the control connection to the FE.

//----------------------------------------------------------------------------
// AMBSI operations:

DLLEXPORT short CompressorGetAMBSIInfo(char *serialNum,
                                       char *firmwareRev,
                                       unsigned long *numErrors,
                                       unsigned long *numTransactions);
///< Get the AMBSI standard data items.

DLLEXPORT short CompressorGetAMBSITemperature(float *temperature);
///< Get the AMBSI temperature.

//----------------------------------------------------------------------------
// Compressor operations:

DLLEXPORT short CompressorGetSWRevisionLevel(char *swRev);
///< Get the compressor embedded software revision.

DLLEXPORT short CompressorGetECUType(short *val);
///< Get the ECU type connected to the compressor M&C module.

DLLEXPORT short CompressorSetRemoteDrive(short val);
///< Enable/disable remote drive

DLLEXPORT short CompressorSetRemoteReset();
///< Reset the compressor.

DLLEXPORT short CompressorSetFaultLatchReset();
///< Reset the compressor M&C module fault latch

DLLEXPORT short CompressorGetMonitorData(CompressorData_t *target);
///< Get all monitor points for the compressor


};  // extern "C"

#endif /* LV_WRAPPER_COMPRESSOR_H_ */
