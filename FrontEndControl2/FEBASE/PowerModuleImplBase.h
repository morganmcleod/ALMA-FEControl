#ifndef POWERMODULEIMPLBASE_H_
#define POWERMODULEIMPLBASE_H_
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
 * base class for the Power Distribution module.
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */

#include "FEHardwareDevice.h"

class PowerModuleImplBase : public FEHardwareDevice {
public:
    PowerModuleImplBase(const std::string &name, int port);
    virtual ~PowerModuleImplBase()
      { shutdown(); }

    int getPort() const
      { return port_m; }
    
    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();
    
// monitor properties:
    virtual float voltageP6V();
    virtual float currentP6V();
    virtual float voltageN6V();
    virtual float currentN6V();
    virtual float voltageP15V();
    virtual float currentP15V();
    virtual float voltageN15V();
    virtual float currentN15V();
    virtual float voltageP24V();
    virtual float currentP24V();
    virtual float voltageP8V();
    virtual float currentP8V();
    virtual bool enableModule();

// control points:
    virtual void enableModule(bool val);

    virtual void standby2Module(bool val);

protected:
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        CURRENT_P6V         = 0x0000,
        VOLTAGE_P6V         = 0x0001,
        CURRENT_N6V         = 0x0002,
        VOLTAGE_N6V         = 0x0003,
        CURRENT_P15V        = 0x0004,
        VOLTAGE_P15V        = 0x0005,
        CURRENT_N15V        = 0x0006,
        VOLTAGE_N15V        = 0x0007,
        CURRENT_N24V        = 0x0008,
        VOLTAGE_P24V        = 0x0009,
        CURRENT_P8V         = 0x000A,
        VOLTAGE_P8V         = 0x000B,
        ENABLE_MODULE       = 0x000C
    };
        
    float voltageP6V_value;
    float currentP6V_value;
    float voltageN6V_value;
    float currentN6V_value;
    float voltageP15V_value;
    float currentP15V_value;
    float voltageN15V_value;
    float currentN15V_value;
    float voltageP24V_value;
    float currentP24V_value;
    float voltageP8V_value;
    float currentP8V_value;
    bool enableModule_value;

    int voltageP6V_status;
    int currentP6V_status;
    int voltageN6V_status;
    int currentN6V_status;
    int voltageP15V_status;
    int currentP15V_status;
    int voltageN15V_status;
    int currentN15V_status;
    int voltageP24V_status;
    int currentP24V_status;
    int voltageP8V_status;
    int currentP8V_status;
    int enableModule_status;
        
    AmbRelativeAddr baseRCA;
    AmbRelativeAddr voltageP6V_RCA;    
    AmbRelativeAddr currentP6V_RCA;
    AmbRelativeAddr voltageN6V_RCA;
    AmbRelativeAddr currentN6V_RCA;
    AmbRelativeAddr voltageP15V_RCA;
    AmbRelativeAddr currentP15V_RCA;
    AmbRelativeAddr voltageN15V_RCA;
    AmbRelativeAddr currentN15V_RCA;
    AmbRelativeAddr voltageP24V_RCA;
    AmbRelativeAddr currentP24V_RCA;
    AmbRelativeAddr voltageP8V_RCA;
    AmbRelativeAddr currentP8V_RCA;
    AmbRelativeAddr enableModule_RCA;
    
protected:    
    int port_m; ///< to which FEMC port is this module connected. 

private:
    // forbid copy constructor:
    PowerModuleImplBase(const PowerModuleImplBase &other);
    
    Time lastMonitorTime;
    int monitorPhase;

    DECLARE_MONITORS_REGISTRY(PowerModuleImplBase)
    void logMon(bool printHeader = false) const;
};      
    
    
#endif /*POWERMODULEIMPLBASE_H_*/
