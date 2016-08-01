#ifndef COMPRESSORIMPL_H_
#define COMPRESSORIMPL_H_
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

#include "FEBASE/CompressorImplBase.h"
#include "OPTIMIZE/ThermalLogger.h"

/// implementation class for the Compressor M&C module

class CompressorImpl : public CompressorImplBase, public ThermalLoggable {
public:
    CompressorImpl(unsigned long channel,
                   unsigned long nodeAddress);
    virtual ~CompressorImpl();

// State control:
    void startMonitor();
    void stopMonitor();

    void setThermalLogInterval(unsigned int interval);

// Structure for monitoring all compressor parameters:
    struct Compressor_t {
        float getTemp1_value;
        float getTemp2_value;
        float getTemp3_value;
        float getTemp4_value;
        float getReturnLinePressure_value;
        float getAuxTransducer_value;
        float getSupplyPressure_value;
        bool getPressureAlarm_value;
        bool getTempAlarm_value;
        bool getDriveIndicator_value;
        bool getICCUStatusError_value;
        bool getICCUCableError_value;
        bool getFETIMStatusError_value;
        bool getFETIMCableError_value;
        bool getInterlockOverride_value;
        bool getFaultStatusError_value;
        unsigned long getTimeSinceLastPowerOn_value;
        unsigned long getTimeSinceLastPowerOff_value;
    };

    bool getMonitorCompressor(Compressor_t &target);

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string

    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string

private:
    // thermal logger object:
    ThermalLogger *thermalLogger_mp;
    unsigned int thermaLogInterval_m;
};


#endif /* COMPRESSORIMPL_H_ */
