#ifndef COMPRESSORIMPLBASE_H_
#define COMPRESSORIMPLBASE_H_
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
 * base class for the Compressor M&C module
 *----------------------------------------------------------------------
 */

#include "AMBSIHardwareDevice.h"

class CompressorImplBase : public AMBSIHardwareDevice {
public:
    CompressorImplBase(const std::string &name);
    virtual ~CompressorImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// Compressor monitor points:
    virtual float getTemp1();
    virtual float getTemp2();
    virtual float getTemp3();
    virtual float getTemp4();
    virtual float getReturnLinePressure();
    virtual float getAuxTransducer();
    virtual float getSupplyPressure();
    virtual bool getPressureAlarm();
    virtual bool getTempAlarm();
    virtual bool getDriveIndicator();
    virtual bool getICCUStatusError();
    virtual bool getICCUCableError();
    virtual bool getFETIMStatusError();
    virtual bool getFETIMCableError();
    virtual bool getInterlockOverride();
    virtual unsigned char getECUType();
    virtual bool getFaultStatusError();
    virtual std::string getSWRevisionLevel();
    virtual unsigned long getTimeSinceLastPowerOn();
    virtual unsigned long getTimeSinceLastPowerOff();

// Compressor control points;
    virtual void setRemoteDrive(bool val);
    virtual void setRemoteReset();
    virtual void setFaultLatchReset();

protected:
    virtual void monitorAction(Time *timestamp_p);

    enum MonitorControlOffset {
        // Compressor monitor points:
        GET_TEMP_1                      = 0x0001,
        GET_TEMP_2                      = 0x0002,
        GET_TEMP_3                      = 0x0003,
        GET_TEMP_4                      = 0x0004,
        GET_RET_PRESSURE                = 0x0005,
        GET_AUX_2                       = 0x0006,
        GET_SUPPLY_PRESSURE             = 0x0007,
        GET_PRESSURE_ALARM              = 0x0008,
        GET_TEMP_ALARM                  = 0x0009,
        GET_DRIVE_INDICATION            = 0x000A,
        GET_ICCU_STATUS_ERROR           = 0x000B,
        GET_ICCU_CABLE_DETECT_ERROR     = 0x000C,
        GET_FETIM_STATUS_ERROR          = 0x000D,
        GET_FETIM_CABLE_ERROR           = 0x000E,
        GET_INTERLOCK_OVERRIDE          = 0x000F,
        GET_ECU_TYPE                    = 0x0010,
        GET_FAULT_STATUS_ERROR          = 0x0011,
        GET_SW_REVISION_LEVEL           = 0x0012,
        GET_TIME_SINCE_LAST_POWER_ON    = 0x0013,
        GET_TIME_SINCE_LAST_POWER_OFF   = 0x0014,

        // Compressor control points:
        SET_REMOTE_DRIVE                = 0x1001,
        SET_REMOTE_RESET                = 0x1002,
        SET_FAULT_LATCH_RESET           = 0x1003,

        // All following control commands not yet implemented:
        SET_PUSH_4K_CRYOCOOLER_TEMP     = 0x1004,
        SET_PUSH_15K_CRYOCOOLER_TEMP    = 0x1005,
        SET_PUSH_110K_CRYOCOOLER_TEMP   = 0x1006,
        SET_PUSH_PORT_PRESSURE          = 0x1007,
        SET_PUSH_DEWAR_PRESSURE         = 0x1008,
        SET_PUSH_GATE_VALVE_STATE       = 0x1009,
        SET_PUSH_SOLENOID_VALVE_STATE   = 0x100A,
        SET_PUSH_BACKING_PUMP_ENABLE    = 0x100B,
        SET_PUSH_TURBO_PUMP_ENABLE      = 0x100C,
        SET_PUSH_TURBO_PUMP_STATE       = 0x100D,
        SET_PUSH_TURBO_PUMP_SPEED       = 0x100E,
        SET_PUSH_CRYO_SUPPLY_CURRENT_230V = 0x100F,
        SET_BYPASS_TIMERS               = 0x2000
    };

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
    unsigned char getECUType_value;
    bool getFaultStatusError_value;
    std::string getSWRevisionLevel_value;
    unsigned long getTimeSinceLastPowerOn_value;
    unsigned long getTimeSinceLastPowerOff_value;

private:
    // forbid copy constructor:
    CompressorImplBase(const CompressorImplBase &other);

    Time lastMonitorTime;
    int monitorPhase;
};

#endif /* COMPRESSORIMPLBASE_H_ */
