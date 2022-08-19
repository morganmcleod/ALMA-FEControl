#ifndef SRC_DLLMAIN_H_
#define SRC_DLLMAIN_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2022
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

// A simplified DLL interface to the basic CAN bus machinery provided by FrontEndControl.DLL
// Supports initializing the NI-CAN interface (future SocketServer interface),
// sending commands, sending monitor requests and receiving the response.
//
// Does not (yet?) support background subsystem logging on a worker thread,
// logging CAN messages sent and received.

#include "DLLExport.h"

DLL_API short DLL_CALL getSoftwareVersion(char *versionString);
///< Get the DLL software version

DLL_API int DLL_CALL initialize();
///< Open the CAN bus connection and start logging

DLL_API int DLL_CALL setTimeout(unsigned long timeout);
///< Override the default CAN receive timeout in ms.

DLL_API int DLL_CALL shutdown();
///< Stop logs and close the CAN connection

DLL_API int DLL_CALL findNodes(unsigned short *numFound, unsigned char *nodeAddrs, unsigned char **serialNums, unsigned short maxLen);
///< Find all CAN nodes on bus
// numFound:   returns the number of nodes found
// nodeAddrs:  returns array of 1-byte node addresses found
// serialNums: returns array of 8-byte serial numbers found
// maxLen:     don't return more than this many nodeAddrs & serialNums

DLL_API int DLL_CALL command(unsigned char nodeAddr, unsigned long RCA, unsigned short dataLength, const unsigned char *data);
///< CAN Command
// nodeAddr:   where to send the command.  If zero, use the configured node address.
// RCA:        relative CAN address
// dataLength: how many bytes in the command data payload
// data:       1-8 bytes command payload

DLL_API int DLL_CALL monitor(unsigned char nodeAddr, unsigned long RCA, unsigned short *dataLength, unsigned char *data);
/// CAN Monitor request
// nodeAddr:   where to send the request.  If zero, use the configured node address.
// RCA:        relative CAN address
// dataLength: returns the number of valid bytes in the response data payload
// data:       1-8 bytes response payload

#endif /* SRC_DLLMAIN_H_ */
