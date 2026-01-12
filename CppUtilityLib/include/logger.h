#ifndef LOGGER_H_
#define LOGGER_H_
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

/// \file
/// \brief defines a simple, thread-safe, multi-level logging mechanism.  Based on http://www.ddj.com/cpp/201804215

#include "setTimeStamp.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

/// supported log levels.  Higher values provide more detailed logging.
enum logLevel {
    LM_EMERGENCY,   /// conditions requiring immediate attention and preventative action
    LM_ALERT,       /// conditions requiring immediate attention
    LM_CRITICAL,    /// critical error conditions
    LM_ERROR,       /// error conditions
    LM_WARNING,     /// warning conditions
    LM_NOTICE,      /// normal but significant system activity
    LM_INFO,        /// normal system activity
    LM_DEBUG,       /// dumping object state
    LM_TRACE,       /// tracing entry and exit of function calls
};

/// a string corresponding to each logLevel:
extern const char *logLevelText[];

template <typename OutputPolicy>
class Logger {
///< The Logger imparts thread safety by holding one line per object in an internal string buffer
///<  and then using fprintf, which is by definition thread-safe, to write to the underlying FILE stream.
///< Logger is parameterized by OutputPolicy which handles the details of writing to the FILE stream.
public:
    Logger() {}
    ///< A new Logger object will be constructed for each line to log.
    
    virtual ~Logger()
      { OutputPolicy::output(os.str()); }
    ///< The writing to the log happens in the destructor.
    
    std::ostringstream &get(logLevel level = LM_INFO, const Time *TS = NULL);
    ///< Prepares the Logger object by prepending the timestamp and logging level to the internal string.

    inline static logLevel levelFromString(std::string &inputString) {
      if(inputString.compare("EMERGENCY") == 0)
        return logLevel::LM_EMERGENCY;
      else if(inputString.compare("ALERT")==0)
        return logLevel::LM_ALERT;
      else if(inputString.compare("CRITICAL")==0)
        return logLevel::LM_CRITICAL;
      else if(inputString.compare("ERROR")==0)
        return logLevel::LM_ERROR;
      else if(inputString.compare("WARNING")==0)
        return logLevel::LM_WARNING;
      else if(inputString.compare("NOTICE")==0)
        return logLevel::LM_NOTICE;
      else if(inputString.compare("INFO")==0)
        return logLevel::LM_INFO;
      else if(inputString.compare("DEBUG")==0)
        return logLevel::LM_DEBUG;
      else if(inputString.compare("TRACE")==0)
        return logLevel::LM_TRACE;
      else
        return logLevel::LM_INFO;
    }

    inline static logLevel &reportingLevel()
      { return reportingLevel_m; }
    ///< Access an object giving the maximum loging level to print.  This object can be modified at runtime.

    inline static void setReportingLevel(logLevel reportingLevel)
      { reportingLevel_m = reportingLevel; }
    ///< Access an object giving the maximum loging level to print.  This object can be modified at runtime.

protected:
    std::ostringstream os;
    ///< internal string buffer for the line.
   
private:
    static logLevel reportingLevel_m;
    ///< Holds the maximum logging level to print.
    
    // Forbid copy and assignment:
    Logger(const Logger &);    
    Logger &operator =(const Logger &);
};

class StreamOutput {
///< StreamOutput is an OutputPolicy for writing to a FILE stream.
public:
    inline static FILE *&stream()
      { return logStream_mp; }
    ///< Access the stream to use for output
    
    static void output(const std::string& msg);
    ///< Logger calls this method to write a line.
    
    static void setStream(FILE *stream_p = NULL)
      { logStream_mp = (stream_p) ? stream_p : stdout; }
    ///< Assign a new stream for output.

private:
    static FILE *logStream_mp;
    ///< The stream to use for output.  This may be assigned to at runtime.
};

inline void StreamOutput::output(const std::string& msg) {
    // get and check that the target stream is valid:
    FILE *logStream = stream();
    if (!logStream)
        return;
    // use thread-safe stdio methods to write the buffer and flush:
    fprintf(logStream, "%s", msg.c_str());
    fflush(logStream);
}

typedef Logger<StreamOutput> StreamLogger;

#define LOGT(level, pts) \
    if (level > StreamLogger::reportingLevel() || !StreamOutput::stream()) ; \
    else StreamLogger().get(level, pts)
///< Log with a timestamp provided.
///< this macro means we don't execute the line which formats the log text unless
///< it is at or below the current logging level and there is a valid target stream.

#define LOG(level) LOGT(level, NULL)
///< Log with the Logger generating the timestamp.

#endif /*LOGGER_H_*/
