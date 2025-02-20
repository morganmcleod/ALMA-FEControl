#ifndef _LV_WRAPPER_H_
#define _LV_WRAPPER_H_
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
 *
 * Shared infrastructure for configuration, logging, database connection
 * used by the LabVIEW wrappers for the Front End and the LORTM.
 * 
 *----------------------------------------------------------------------
 */


#include "dllExport.h"
#include <string>

namespace FrontEndLVWrapper {
    extern std::string logDir;
    extern bool useSocketServer;
    extern bool debugLVStructures;
    extern bool CAN_noTransmit;
    extern std::string iniFileName;
    extern std::string FrontEndIni;
    extern unsigned int thermalLogInterval;
};

// All functions return 0 for success or -1 for failure unless otherwise specified.

short LVWrapperInit();
///< Initialize the shared data.

short LVWrapperShutdown();
///< Disconnect from the shared data.

short LVWrapperFindIniFile();
///< Helper to load the FrontEndControlDLL.ini file


extern "C" {

DLLEXPORT short getSoftwareVersion(char *versionString);
///< Get the FrontEndControl.DLL version string

DLLEXPORT short getDataPath(char *pathString);
///< Get the configured logDir path.

DLLEXPORT short subscribeForEvents(short doSubscribe);
///< subscribe/unsubscribe for events.
///< events will be generated only if there is at least one subscriber.
///< pass 1 to subscribe, 0 to unsubscribe.

DLLEXPORT short getNextEvent(unsigned long *seq,
                             short *eventCode,
                             short *band,
                             short *pol,
                             short *param,
                             short *progress,
                             short message_len,
                             char *message);
///< Poll the event queue and remove the next event, if any.

DLLEXPORT short getNextEventSeqNo(unsigned long *seq);
///< Get the next sequence number that the event queue will return.


//----------------------------------------------------------------------------
// Miscellaneous:

#define WHACK(P) { if (P) { delete P; P = NULL; }}

DLLEXPORT short TestSocketClient();

//----------------------------------------------------------------------------

};  // extern "C"

#endif /*_LV_WRAPPER_H_*/
