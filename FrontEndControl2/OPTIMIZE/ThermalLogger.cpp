/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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

#include "ThermalLogger.h"
#include "logger.h"
#include "stringConvert.h"
#include <fstream>
#include <iomanip>
using namespace std;

void ThermalLogger::reset() {
    interval_m = 0;
    enable_m = false;
}

bool ThermalLogger::start(unsigned int loggingInterval) {
    if (enable_m) {
        LOG(LM_ERROR) << "ThermalLogger ERROR: already started." << endl;
        return false;
    }
    
    interval_m = loggingInterval;
    enable_m = true;

    // create the data file:
    if (!logDir_m.empty()) {
        string fileName, tss;
        Time ts;
        setTimeStamp(&ts);
        timestampToText(&ts, tss, true);
        fileName = logDir_m + "ThermalLog-";
        if (!filenameTag_m.empty())
            fileName += filenameTag_m + "-";
        fileName += tss + ".txt";
        dataFile_mp = new ofstream(fileName.c_str(), ios_base::trunc);
        
        // get and save the header row to the file:
        string headerRow;
        getHeaderRow(headerRow);
        (*dataFile_mp) << headerRow << endl;
    }
    // start the logging thread:
    return OptimizeBase::startWorkerThread();
}

void ThermalLogger::stop() {
    LOG(LM_INFO) << "ThermalLogger: stopping..." << endl;
    int retry = 50;
    enable_m = false;

    do {
        SLEEP(100);
    } while (dataFile_mp != NULL && retry--);

    if (dataFile_mp != NULL) {
        int elapsed = (250 - retry) / 10;
        LOG(LM_INFO) << "ThermalLogger: NOT stopped after " << elapsed << " seconds." << endl;
    }
}

void ThermalLogger::optimizeAction() {
    // loop for interval_m seconds:
    for (unsigned count = 0; enable_m && count < 10 * interval_m; ++count)
        SLEEP(100);
    
    // if still enabled and interval > 0, get and save a row of data:
    if (enable_m && interval_m > 0) {
        string row;
        getLogRow(row);
        (*dataFile_mp) << row << endl;
    } else {
        // else quit:
        setFinished(true);
    }
}

void ThermalLogger::exitAction(bool success) {
    // flush the log file:
    if (dataFile_mp) {
        (*dataFile_mp).flush();
        delete dataFile_mp;
        dataFile_mp = NULL;
    }
}

void ThermalLogger::getHeaderRow(string &target) const {
    target = "TS";
    if (source_mp)
        source_mp -> appendThermalLogHeader(target);
}

void ThermalLogger::getLogRow(string &target) const {
    Time timestamp;
    setTimeStamp(&timestamp);
    timestampToText(&timestamp, target);
    if (source_mp)
        source_mp -> appendThermalLog(target);
}

