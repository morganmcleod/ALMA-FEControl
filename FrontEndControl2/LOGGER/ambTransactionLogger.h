#ifndef FE_TESTSYS_AMBTRANSACTIONLOGGER_H_
#define FE_TESTSYS_AMBTRANSACTIONLOGGER_H_

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
*/

/************************************************************************
 * class AmbTransactionLogger implements a monitor and command logger for the front end.
 * 
 *----------------------------------------------------------------------
 */

#include "FEBASE/FEHardwareDevice.h"
#include <string>
#include <vector>

class AmbTransactionLogger : public FEHardwareDevice::LogInterface {
public:    
    AmbTransactionLogger(bool useStream = false);
    ///< construct
        
    virtual ~AmbTransactionLogger();

    void setUseStream(bool useStream);
    ///< start/stop logging to the configured log stream.

    void shutdown();
    ///< flushes log queue in preparation for destruction.

    virtual void checkPoint(const char *text);
    ///< log a checkpoint transaction.
        
    virtual void log(FEHardwareDevice::Transaction_t trans,
                     const char *text,
                     AmbRelativeAddr RCA,
                     int FEStatus,
                     unsigned long iValue,
                     float fValue);
    ///< Log an actual transaction. 

    struct LogEntry;        ///< forward declare log entry structure.
    
private:
    class LogBuffer;        ///< forward declare the circular buffer class.
    LogBuffer *buffer_mp;   ///< circular buffer of log entries.

    bool useStream_m;       ///< if true, log to the stream output.

    pthread_t thread_m;     ///< handle for the logging worker thread.
    bool do_shutdown;       ///< True when worker should die.
    bool shutdown_done;     ///< True when worker has died.

    void logSingleItem(const LogEntry *entry) const;
    ///< Log a single item to the output stream

    static void *loggerThread(AmbTransactionLogger *arg);
    ///< The function for the worker thread to which periodically logs whats in the buffer.

    static const std::string &statusToText(const AmbErrorCode_t *status, std::string &statusText);
    ///< Helper function to format a AmbErrorCode_t as text for logging.
};

#endif
