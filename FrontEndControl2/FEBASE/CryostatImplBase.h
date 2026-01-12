#ifndef CRYOSTATIMPLBASE_H_
#define CRYOSTATIMPLBASE_H_
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
 * base class for the FrontEnd Cryostat M&C module
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */
 
#include "FEHardwareDevice.h"

class CryostatImplBase : public FEHardwareDevice {
public:
    CryostatImplBase(const std::string &name);
    virtual ~CryostatImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual float cryostatTemperature0();
    virtual float cryostatTemperature1();
    virtual float cryostatTemperature2();
    virtual float cryostatTemperature3();
    virtual float cryostatTemperature4();
    virtual float cryostatTemperature5();
    virtual float cryostatTemperature6();
    virtual float cryostatTemperature7();
    virtual float cryostatTemperature8();
    virtual float cryostatTemperature9();
    virtual float cryostatTemperature10();
    virtual float cryostatTemperature11();
    virtual float cryostatTemperature12();
    
    virtual bool backingPumpEnable();

    virtual bool turboPumpEnable();
    virtual bool turboPumpErrorState();
    virtual bool turboPumpHighSpeed();
    
    virtual unsigned char gateValveState();
    virtual unsigned char solenoidValveState();
    
    virtual float vacuumCryostatPressure();
    virtual float vacuumPortPressure();
    virtual bool vacuumGaugeEnable();
    virtual bool vacuumGaugeErrorState();

    virtual float supplyCurrent230V();

// control points:
    virtual void backingPumpEnable(bool val);
    virtual void turboPumpEnable(bool val);
    virtual void gateValveState(unsigned char val);
    virtual void solenoidValveState(unsigned char val);
    virtual void vacuumGaugeEnable(bool val);
    virtual void setTVOCoefficient(unsigned char se,  unsigned char co, float val);

protected:
    FEMC_ERROR CryostatMonitorTempsensor(AmbRelativeAddr RCA, float &target);
    ///< helper to monitor a cryostat temperature sensor
    
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        CRYOSTAT_TEMP                   = 0x0000,
        BACKING_PUMP_ENABLE             = 0x0034,
        TURBO_PUMP_ENABLE               = 0x0038,
        TURBO_PUMP_STATE                = 0x0039,
        TURBO_PUMP_SPEED                = 0x003A,
        GATE_VALVE_STATE                = 0x003C,
        SOLENOID_VALVE_STATE            = 0x0040,
        VACUUM_CRYOSTAT_PRES            = 0x0044,
        VACUUM_PORT_PRES                = 0x0045,
        VACUUM_GAUGE_ENABLE             = 0x0046,
        VACUUM_GAUGE_STATE              = 0x0047,
        SUPPLY_CURRENT_230V             = 0x0048,
        COLD_HEAD_HOURS                 = 0x004C,
        RESET_COLD_HEAD_HOURS           = 0x004D,
        TVO_COEFF_SET                   = 0x0080,
        SET_CRYOSTAT_TEMP_TVO_COEFF     = 0x0008

    };

    float cryostatTemperature0_value;
    float cryostatTemperature1_value;
    float cryostatTemperature2_value;
    float cryostatTemperature3_value;
    float cryostatTemperature4_value;
    float cryostatTemperature5_value;
    float cryostatTemperature6_value;
    float cryostatTemperature7_value;
    float cryostatTemperature8_value;
    float cryostatTemperature9_value;
    float cryostatTemperature10_value;
    float cryostatTemperature11_value;
    float cryostatTemperature12_value;    
    bool backingPumpEnable_value;
    bool turboPumpEnable_value;
    bool turboPumpErrorState_value;
    bool turboPumpHighSpeed_value;
    unsigned char gateValveState_value;
    unsigned char solenoidValveState_value;
    float vacuumCryostatPressure_value;
    float vacuumPortPressure_value;
    bool vacuumGaugeEnable_value;
    bool vacuumGaugeErrorState_value;
    float supplyCurrent230V_value;

    int cryostatTemperature0_status;
    int cryostatTemperature1_status;
    int cryostatTemperature2_status;
    int cryostatTemperature3_status;
    int cryostatTemperature4_status;
    int cryostatTemperature5_status;
    int cryostatTemperature6_status;
    int cryostatTemperature7_status;
    int cryostatTemperature8_status;
    int cryostatTemperature9_status;
    int cryostatTemperature10_status;
    int cryostatTemperature11_status;
    int cryostatTemperature12_status;    
    int backingPumpEnable_status;
    int supplyCurrent230V_status;
    int turboPumpEnable_status;
    int turboPumpErrorState_status;
    int turboPumpHighSpeed_status;
    int gateValveState_status;
    int solenoidValveState_status;
    int vacuumCryostatPressure_status;
    int vacuumPortPressure_status;
    int vacuumGaugeEnable_status;
    int vacuumGaugeErrorState_status;
    int setTVOCoefficient_status;

    AmbRelativeAddr baseRCA;
    AmbRelativeAddr cryostatTemperature0_RCA;
    AmbRelativeAddr cryostatTemperature1_RCA;
    AmbRelativeAddr cryostatTemperature2_RCA;
    AmbRelativeAddr cryostatTemperature3_RCA;
    AmbRelativeAddr cryostatTemperature4_RCA;
    AmbRelativeAddr cryostatTemperature5_RCA;
    AmbRelativeAddr cryostatTemperature6_RCA;
    AmbRelativeAddr cryostatTemperature7_RCA;
    AmbRelativeAddr cryostatTemperature8_RCA;
    AmbRelativeAddr cryostatTemperature9_RCA;
    AmbRelativeAddr cryostatTemperature10_RCA;
    AmbRelativeAddr cryostatTemperature11_RCA;
    AmbRelativeAddr cryostatTemperature12_RCA;
    AmbRelativeAddr backingPumpEnable_RCA;
    AmbRelativeAddr supplyCurrent230V_RCA;
    AmbRelativeAddr turboPumpEnable_RCA;
    AmbRelativeAddr turboPumpErrorState_RCA;
    AmbRelativeAddr turboPumpHighSpeed_RCA;
    AmbRelativeAddr gateValveState_RCA;
    AmbRelativeAddr solenoidValveState_RCA;
    AmbRelativeAddr vacuumCryostatPressure_RCA;
    AmbRelativeAddr vacuumPortPressure_RCA;
    AmbRelativeAddr vacuumGaugeEnable_RCA;
    AmbRelativeAddr vacuumGaugeErrorState_RCA;
    AmbRelativeAddr setTVOCoefficient_RCA;

private:
    // forbid copy constructor:
    CryostatImplBase(const CryostatImplBase &other);
    
    Time lastMonitorTime;
    int monitorPhase;
    
    DECLARE_MONITORS_REGISTRY(CryostatImplBase)
    void logMon(bool printHeader = false) const;
};      


#endif /*CRYOSTATIMPLBASE_H_*/
