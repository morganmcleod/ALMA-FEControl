#ifndef SETTIMESTAMP_H_
#define SETTIMESTAMP_H_
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
 *----------------------------------------------------------------------
 */

#include "timeDef.h"
#include <string>

extern void setTimeStamp(Time* timestamp);
// Sets the given timestamp to the system time.
// This is certainly not the same as what the ABM uses but it will do for testing.

extern const std::string &timestampToText(const Time *timestamp, std::string &timeText, bool forFilename = false);
// Format a timestamp as a string for printing.


#endif /*SETTIMESTAMP_H_*/
