#ifndef FETIMIMPLBASE_H_
#define FETIMIMPLBASE_H_
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
 * base class for the FrontEnd Thermal Interlock Module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */

#include "FEHardwareDevice.h"

class FETIMImplBase : public FEHardwareDevice {
public:
    FETIMImplBase(const std::string &name);
    virtual ~FETIMImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual float internalTemperature1();
    virtual float internalTemperature2();
    virtual float internalTemperature3();
    virtual float internalTemperature4();
    virtual float internalTemperature5();
    virtual bool internalTemperatureOOR();

    virtual float externalTemperature1();
    virtual float externalTemperature2();
    virtual bool externalTemperatureOOR1();
    virtual bool externalTemperatureOOR2();

    virtual float getAirflowSensor1();
    virtual float getAirflowSensor2();
    virtual bool airflowSensorOOR();

    virtual float heliumBufferPressure();
    virtual bool heliumBufferPressureOOR();

    virtual bool sensorSingleFailed();
    virtual bool sensorMultiFailed();

    virtual float glitchValue();
    virtual bool glitchCounterTriggered();

    virtual bool delayShutdownTriggered();
    virtual bool finalShutdownTriggered();

    virtual bool getCompressorStatus();
    virtual bool getCompressorInterlockStatus();
    virtual bool getCompressorCableStatus();

// control points:
    virtual void setTriggerDewarN2Fill(bool val);

protected:
    virtual void monitorAction(Time *timestamp_p);

    enum MonitorControlOffset {
        FETIM_INTRLK_SENS_INT_TEMP      = 0x0000,
        FETIM_INTRLK_SENS_FLOW_SENS     = 0x0008,
        FETIM_INTRLK_SENS_SINGLE_FAIL   = 0x0010,
        FETIM_INTRLK_GLITCH_VALUE       = 0x0020,
        FETIM_INTRLK_GLITCH_CNT_TRIG    = 0x0022,
        FETIM_INTRLK_SENS_MULT_FAIL     = 0x0024,
        FETIM_INTRLK_SENS_INT_TEMP_OOR  = 0x0028,
        FETIM_INTRLK_SENS_FLOW_OOR      = 0x002C,
        FETIM_INTRLK_DELAY_TRIG         = 0x0030,
        FETIM_INTRLK_SHUTDOWN_TRIG      = 0x0034,
        FETIM_COMP_EXT_TEMP             = 0x0040,
        FETIM_COMP_EXT_TEMP_OOR         = 0x0044,
        FETIM_COMP_HE2_PRESS            = 0x0050,
        FETIM_COMP_HE2_PRESS_OOR        = 0x0054,
        FETIM_COMP_FE_STATUS            = 0x0058,
        FETIM_COMP_INTRLK_STATUS        = 0x0060,
        FETIM_COMP_CABLE_STATUS         = 0x0068,
        FETIM_COMP_DEWAR_N2_FILL        = 0x0080
    };

    float internalTemperature1_value;
    float internalTemperature2_value;
    float internalTemperature3_value;
    float internalTemperature4_value;
    float internalTemperature5_value;
    bool internalTemperatureOOR_value;
    float externalTemperature1_value;
    float externalTemperature2_value;
    bool externalTemperatureOOR1_value;
    bool externalTemperatureOOR2_value;
    float getAirflowSensor1_value;
    float getAirflowSensor2_value;
    bool airflowSensorOOR_value;
    float heliumBufferPressure_value;
    bool heliumBufferPressureOOR_value;
    bool sensorSingleFailed_value;
    bool sensorMultiFailed_value;
    float glitchValue_value;
    bool glitchCounterTriggered_value;
    bool delayShutdownTriggered_value;
    bool finalShutdownTriggered_value;
    bool getCompressorStatus_value;
    bool getCompressorInterlockStatus_value;
    bool getCompressorCableStatus_value;

    int internalTemperature1_status;
    int internalTemperature2_status;
    int internalTemperature3_status;
    int internalTemperature4_status;
    int internalTemperature5_status;
    int internalTemperatureOOR_status;
    int externalTemperature1_status;
    int externalTemperature2_status;
    int externalTemperatureOOR1_status;
    int externalTemperatureOOR2_status;
    int getAirflowSensor1_status;
    int getAirflowSensor2_status;
    int airflowSensorOOR_status;
    int heliumBufferPressure_status;
    int heliumBufferPressureOOR_status;
    int sensorSingleFailed_status;
    int sensorMultiFailed_status;
    int glitchValue_status;
    int glitchCounterTriggered_status;
    int delayShutdownTriggered_status;
    int finalShutdownTriggered_status;
    int getCompressorStatus_status;
    int getCompressorInterlockStatus_status;
    int getCompressorCableStatus_status;
    int setTriggerDewarN2Fill_status;

    AmbRelativeAddr baseRCA;
    AmbRelativeAddr internalTemperature1_RCA;
    AmbRelativeAddr internalTemperature2_RCA;
    AmbRelativeAddr internalTemperature3_RCA;
    AmbRelativeAddr internalTemperature4_RCA;
    AmbRelativeAddr internalTemperature5_RCA;
    AmbRelativeAddr internalTemperatureOOR_RCA;
    AmbRelativeAddr externalTemperature1_RCA;
    AmbRelativeAddr externalTemperature2_RCA;
    AmbRelativeAddr externalTemperatureOOR1_RCA;
    AmbRelativeAddr externalTemperatureOOR2_RCA;
    AmbRelativeAddr getAirflowSensor1_RCA;
    AmbRelativeAddr getAirflowSensor2_RCA;
    AmbRelativeAddr airflowSensorOOR_RCA;
    AmbRelativeAddr heliumBufferPressure_RCA;
    AmbRelativeAddr heliumBufferPressureOOR_RCA;
    AmbRelativeAddr sensorSingleFailed_RCA;
    AmbRelativeAddr sensorMultiFailed_RCA;
    AmbRelativeAddr glitchValue_RCA;
    AmbRelativeAddr glitchCounterTriggered_RCA;
    AmbRelativeAddr delayShutdownTriggered_RCA;
    AmbRelativeAddr finalShutdownTriggered_RCA;
    AmbRelativeAddr getCompressorStatus_RCA;
    AmbRelativeAddr getCompressorInterlockStatus_RCA;
    AmbRelativeAddr getCompressorCableStatus_RCA;
    AmbRelativeAddr setTriggerDewarN2Fill_RCA;

private:
    // forbid copy constructor:
    FETIMImplBase(const FETIMImplBase &other);

    Time lastMonitorTime;
    int monitorPhase;

    DECLARE_MONITORS_REGISTRY(FETIMImplBase)
    void logMon(bool printHeader = false) const;
};


#endif /* FETIMIMPLBASE_H_ */
