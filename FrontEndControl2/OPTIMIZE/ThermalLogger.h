#ifndef ThermalLogger_H_
#define ThermalLogger_H_
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

/// \file
/// \brief Background process which continuously logs temperatures, cryostat pressures, state, etc.

#include "OptimizeBase.h"
#include "ThermalLoggable.h"
#include <iostream>
#include <string>

class ThermalLogger : public OptimizeBase {
public:    
	ThermalLogger(const ThermalLoggable &source, const std::string &filenameTag = std::string())
      : OptimizeBase("ThermalLogger"),
        source_mp(&source),
        filenameTag_m(filenameTag),
        dataFile_mp(NULL)
        { reset(); }

    virtual ~ThermalLogger()
      { delete dataFile_mp; }

    void reset();
    ///< reset all state to initial/just constructed.

    bool start(unsigned int loggingInterval);
    ///< start logging with the specified interval, in seconds.
    
    void stop();
    ///< stop logging

    void setLoggingInterval(unsigned int loggingInterval) 
      { interval_m = loggingInterval; }
    ///< change the logging interval, in seconds.
    
protected:    
    virtual void optimizeAction();
    ///< called by the OptimizeBase worker thread.

    virtual void exitAction(bool success);
    ///< called by OptimizeBase prior to worker thread exit.

private:
    void getHeaderRow(std::string &target) const;
    ///< implement build a header row for the log
    void getLogRow(std::string &target) const;
    ///< implement build a data row for the log
    
    const ThermalLoggable *source_mp;   ///< The object we will be requesting log data from
    std::string filenameTag_m;         ///< String to use as part of the filename
    unsigned int interval_m;            ///< logging interval, seconds
    bool enable_m;                      ///< logging only happens while true
    std::ostream *dataFile_mp;          ///< output data file stream
};

#endif /*ThermalLogger_H_*/
