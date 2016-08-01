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

#include "setTimeStamp.h"
#include <stdio.h>

void setTimeStamp(Time* timestamp) {
    // Sets the given timestamp to the win32 system time.
    // This is certainly not the same as what the ABM uses but it will do for testing.
    if (timestamp) {
/*
#ifdef OSX
        time_t t;
        time(&t);
        *timestamp = t;        
        *timestamp *= 10000000UL;	// convert seconds to 100ns counts.
#endif
#ifdef WINDOWS
*/
        SYSTEMTIME st;
        FILETIME ft;
        GetLocalTime(&st);
        if (SystemTimeToFileTime(&st, &ft)) {
            memcpy(timestamp, &ft, sizeof(Time));            
            //*timestamp = ft.dwHighDateTime;
            //*timestamp <<= 32;
            //*timestamp |= ft.dwLowDateTime;
        }
//#endif
    }
}


const std::string &timestampToText(const Time *timestamp, std::string &timeText, bool forFilename) {

/* definition of SYSTEMTIME from MSDN:
    typedef struct _SYSTEMTIME {
      WORD wYear;
      WORD wMonth;
      WORD wDayOfWeek;
      WORD wDay;
      WORD wHour;
      WORD wMinute;
      WORD wSecond;
      WORD wMilliseconds
    }
*/

    SYSTEMTIME st;
    Time tsLow = 0, tsHigh = 0;
    if (timestamp) {
        Time ts = *(timestamp);
        tsLow = ts;
        ts >>= 32;
        tsHigh = ts;

        FILETIME ft;
        memcpy(&ft, timestamp, sizeof(Time));
        FileTimeToSystemTime(&ft, &st);
    } else
        memset(&st, 0, sizeof(SYSTEMTIME));

    char buf[30];
    if (forFilename) {
        sprintf(buf, "%04d-%02d-%02d-%02d%02d%02d",
                     (int) st.wYear, (int) st.wMonth, (int) st.wDay,
                     (int) st.wHour, (int) st.wMinute, (int) st.wSecond);
    } else {
        sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                     (int) st.wYear, (int) st.wMonth, (int) st.wDay,
                     (int) st.wHour, (int) st.wMinute, (int) st.wSecond, 
                     (int) st.wMilliseconds);
    }
    timeText = buf;
    return timeText;
}    



