#ifndef LPRIMPLBASE_H_
#define LPRIMPLBASE_H_
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
*
*/

/************************************************************************
 * base class for the FrontEnd LO Photonic Receiver module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */

#include "FEHardwareDevice.h"

class LPRImplBase : public FEHardwareDevice {
public:
    LPRImplBase(const std::string &name);
    virtual ~LPRImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual float LPRTemperature0();
    virtual float LPRTemperature1();
    virtual unsigned char opticalSwitchPort();
    virtual bool opticalSwitchShutter();
    virtual bool opticalSwitchState();
    virtual bool opticalSwitchBusy();
    virtual float EDFALaserPumpTemperature();
    virtual float EDFALaserDriveCurrent();
    virtual float EDFALaserPhotoDetectCurrent();
    virtual float EDFAPhotoDetectCurrent();
    virtual float EDFAPhotoDetectPower();
    virtual float EDFAModulationInput();

// control points:
    virtual void opticalSwitchPort(unsigned char val);
    virtual void opticalSwitchShutter(bool val);
    virtual void opticalSwitchForceShutter(bool val);
    virtual void EDFAModulationInput(float val);

protected:
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        LPR_TEMPERATURE_0               = 0x0000,
        LPR_TEMPERATURE_1               = 0x0010,
        OPTICAL_SWITCH_PORT             = 0x0020,
        OPTICAL_SWITCH_SHUTTER          = 0x0022,
        OPTICAL_SWITCH_FORCE_SHUTTER    = 0x0024,
        OPTICAL_SWITCH_STATE            = 0x0026,
        OPTICAL_SWITCH_BUSY             = 0x0028,
        EDFA_LASER_PUMP_TEMPERATURE     = 0x0030,
        EDFA_LASER_DRIVE_CURRENT        = 0x0031,
        EDFA_LASER_PHOTODETECT_CURRENT  = 0x0032,
        EDFA_PHOTODETECT_CURRENT        = 0x0034,
        EDFA_PHOTODETECT_POWER          = 0x0036,
        EDFA_MODULATION_INPUT           = 0x0038,
        EDFA_DRIVER_TEMPERATURE_ALARM   = 0x003C
    };

    float LPRTemperature0_value;
    float LPRTemperature1_value;
    unsigned char opticalSwitchPort_value;
    bool opticalSwitchShutter_value;
    bool opticalSwitchState_value;
    bool opticalSwitchBusy_value;
    float EDFALaserPumpTemperature_value;
    float EDFALaserDriveCurrent_value;
    float EDFALaserPhotoDetectCurrent_value;
    float EDFAPhotoDetectCurrent_value;
    float EDFAPhotoDetectPower_value;
    float EDFAModulationInput_value;

    int LPRTemperature0_status;
    int LPRTemperature1_status;
    int opticalSwitchPort_status;
    int opticalSwitchShutter_status;
    int opticalSwitchForceShutter_status;
    int opticalSwitchState_status;
    int opticalSwitchBusy_status;
    int EDFALaserPumpTemperature_status;
    int EDFALaserDriveCurrent_status;
    int EDFALaserPhotoDetectCurrent_status;
    int EDFAPhotoDetectCurrent_status;
    int EDFAPhotoDetectPower_status;
    int EDFAModulationInput_status;

    AmbRelativeAddr baseRCA;
    AmbRelativeAddr LPRTemperature0_RCA;
    AmbRelativeAddr LPRTemperature1_RCA;
    AmbRelativeAddr opticalSwitchPort_RCA;
    AmbRelativeAddr opticalSwitchShutter_RCA;
    AmbRelativeAddr opticalSwitchForceShutter_RCA;
    AmbRelativeAddr opticalSwitchState_RCA;
    AmbRelativeAddr opticalSwitchBusy_RCA;
    AmbRelativeAddr EDFALaserPumpTemperature_RCA;
    AmbRelativeAddr EDFALaserDriveCurrent_RCA;
    AmbRelativeAddr EDFALaserPhotoDetectCurrent_RCA;
    AmbRelativeAddr EDFAPhotoDetectCurrent_RCA;
    AmbRelativeAddr EDFAPhotoDetectPower_RCA;
    AmbRelativeAddr EDFAModulationInput_RCA;

private:
    // forbid copy constructor:
    LPRImplBase(const LPRImplBase &other);
    
    Time lastMonitorTime;
    int monitorPhase;

    DECLARE_MONITORS_REGISTRY(LPRImplBase)
    void logMon(bool printHeader = false) const;
};      


#endif /*LPRIMPLBASE_H_*/
