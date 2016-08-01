#ifndef COLDCARTIMPLBASE_H_
#define COLDCARTIMPLBASE_H_
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
 * base class for the Cold Cartridge including bias and temperature 
 * monitor circuitry.
 * This will be replaced by auto-generated code in the Control environment.
 *----------------------------------------------------------------------
 */

#include "FEHardwareDevice.h"

class ColdCartImplBase : public FEHardwareDevice {
public:
    ColdCartImplBase(const std::string &name, int port);
    virtual ~ColdCartImplBase()
      { shutdown(); }

    int getPort() const
      { return port_m; }
    
    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual float sisPol0Sb1Voltage();
    virtual float sisPol0Sb2Voltage();
    virtual float sisPol1Sb1Voltage();
    virtual float sisPol1Sb2Voltage();
    
    virtual float sisPol0Sb1Current();
    virtual float sisPol0Sb2Current();
    virtual float sisPol1Sb1Current();
    virtual float sisPol1Sb2Current();

    virtual float avgSisPol0Sb1Current(int average = 8);
    virtual float avgSisPol0Sb2Current(int average = 8);
    virtual float avgSisPol1Sb1Current(int average = 8);
    virtual float avgSisPol1Sb2Current(int average = 8);

    virtual bool sisPol0Sb1OpenLoop();
    virtual bool sisPol0Sb2OpenLoop();
    virtual bool sisPol1Sb1OpenLoop();
    virtual bool sisPol1Sb2OpenLoop();
    
    virtual float sisMagnetPol0Sb1Voltage();
    virtual float sisMagnetPol0Sb1Current();
    virtual float sisMagnetPol0Sb2Voltage();
    virtual float sisMagnetPol0Sb2Current();
    virtual float sisMagnetPol1Sb1Voltage();
    virtual float sisMagnetPol1Sb1Current();
    virtual float sisMagnetPol1Sb2Voltage();
    virtual float sisMagnetPol1Sb2Current();

    virtual bool lnaPol0Sb1Enable();
    virtual float lnaPol0Sb1St1DrainVoltage();
    virtual float lnaPol0Sb1St1DrainCurrent();
    virtual float lnaPol0Sb1St1GateVoltage();
    virtual float lnaPol0Sb1St2DrainVoltage();
    virtual float lnaPol0Sb1St2DrainCurrent();
    virtual float lnaPol0Sb1St2GateVoltage();
    virtual float lnaPol0Sb1St3DrainVoltage();
    virtual float lnaPol0Sb1St3DrainCurrent();
    virtual float lnaPol0Sb1St3GateVoltage();
    virtual bool lnaPol0Sb2Enable();
    virtual float lnaPol0Sb2St1DrainVoltage();
    virtual float lnaPol0Sb2St1DrainCurrent();
    virtual float lnaPol0Sb2St1GateVoltage();
    virtual float lnaPol0Sb2St2DrainVoltage();
    virtual float lnaPol0Sb2St2DrainCurrent();
    virtual float lnaPol0Sb2St2GateVoltage();
    virtual float lnaPol0Sb2St3DrainVoltage();
    virtual float lnaPol0Sb2St3DrainCurrent();
    virtual float lnaPol0Sb2St3GateVoltage();
    virtual bool lnaPol1Sb1Enable();
    virtual float lnaPol1Sb1St1DrainVoltage();
    virtual float lnaPol1Sb1St1DrainCurrent();
    virtual float lnaPol1Sb1St1GateVoltage();
    virtual float lnaPol1Sb1St2DrainVoltage();
    virtual float lnaPol1Sb1St2DrainCurrent();
    virtual float lnaPol1Sb1St2GateVoltage();
    virtual float lnaPol1Sb1St3DrainVoltage();
    virtual float lnaPol1Sb1St3DrainCurrent();
    virtual float lnaPol1Sb1St3GateVoltage();
    virtual bool lnaPol1Sb2Enable();
    virtual float lnaPol1Sb2St1DrainVoltage();
    virtual float lnaPol1Sb2St1DrainCurrent();
    virtual float lnaPol1Sb2St1GateVoltage();
    virtual float lnaPol1Sb2St2DrainVoltage();
    virtual float lnaPol1Sb2St2DrainCurrent();
    virtual float lnaPol1Sb2St2GateVoltage();
    virtual float lnaPol1Sb2St3DrainVoltage();
    virtual float lnaPol1Sb2St3DrainCurrent();
    virtual float lnaPol1Sb2St3GateVoltage();
    
    virtual bool lnaLedPol0Enable();
    virtual bool lnaLedPol1Enable();
    
    virtual float sisHeaterPol0Current();
    virtual float sisHeaterPol1Current();
    
    virtual float cartridgeTemperature0();
    virtual float cartridgeTemperature1();
    virtual float cartridgeTemperature2();
    virtual float cartridgeTemperature3();
    virtual float cartridgeTemperature4();
    virtual float cartridgeTemperature5();
    
// control points:
    virtual void sisPol0Sb1Voltage(float val);
    virtual void sisPol0Sb1OpenLoop(bool val);
    virtual void sisPol0Sb2Voltage(float val);
    virtual void sisPol0Sb2OpenLoop(bool val);
    virtual void sisPol1Sb1Voltage(float val);
    virtual void sisPol1Sb1OpenLoop(bool val);
    virtual void sisPol1Sb2Voltage(float val);
    virtual void sisPol1Sb2OpenLoop(bool val);
    
    virtual void sisMagnetPol0Sb1Current(float val);
    virtual void sisMagnetPol0Sb2Current(float val);
    virtual void sisMagnetPol1Sb1Current(float val);
    virtual void sisMagnetPol1Sb2Current(float val);

    virtual void lnaPol0Sb1Enable(bool val);
    virtual void lnaPol0Sb1St1DrainVoltage(float val);
    virtual void lnaPol0Sb1St1DrainCurrent(float val);
    virtual void lnaPol0Sb1St2DrainVoltage(float val);
    virtual void lnaPol0Sb1St2DrainCurrent(float val);
    virtual void lnaPol0Sb1St3DrainVoltage(float val);
    virtual void lnaPol0Sb1St3DrainCurrent(float val);
    virtual void lnaPol0Sb2Enable(bool val);
    virtual void lnaPol0Sb2St1DrainVoltage(float val);
    virtual void lnaPol0Sb2St1DrainCurrent(float val);
    virtual void lnaPol0Sb2St2DrainVoltage(float val);
    virtual void lnaPol0Sb2St2DrainCurrent(float val);
    virtual void lnaPol0Sb2St3DrainVoltage(float val);
    virtual void lnaPol0Sb2St3DrainCurrent(float val);
    virtual void lnaPol1Sb1Enable(bool val);
    virtual void lnaPol1Sb1St1DrainVoltage(float val);
    virtual void lnaPol1Sb1St1DrainCurrent(float val);
    virtual void lnaPol1Sb1St2DrainVoltage(float val);
    virtual void lnaPol1Sb1St2DrainCurrent(float val);
    virtual void lnaPol1Sb1St3DrainVoltage(float val);
    virtual void lnaPol1Sb1St3DrainCurrent(float val);
    virtual void lnaPol1Sb2Enable(bool val);
    virtual void lnaPol1Sb2St1DrainVoltage(float val);
    virtual void lnaPol1Sb2St1DrainCurrent(float val);
    virtual void lnaPol1Sb2St2DrainVoltage(float val);
    virtual void lnaPol1Sb2St2DrainCurrent(float val);
    virtual void lnaPol1Sb2St3DrainVoltage(float val);
    virtual void lnaPol1Sb2St3DrainCurrent(float val);

    virtual void lnaLedPol0Enable(bool val);
    virtual void lnaLedPol1Enable(bool val);

    virtual void sisHeaterPol0Enable(bool val);
    virtual void sisHeaterPol1Enable(bool val);

protected:
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        CARTRIDGE_TEMP          = 0x0880,
        SIS_VOLTAGE             = 0x0008,
        SIS_CURRENT             = 0x0010,
        SIS_OPEN_LOOP           = 0x0018,
        SIS_MAGNET_VOLTAGE      = 0x0020,
        SIS_MAGNET_CURRENT      = 0x0030,
        LNA_ENABLE              = 0x0058,
        LNA1_DRAIN_VOLTAGE      = 0x0040,
        LNA1_DRAIN_CURRENT      = 0x0041,
        LNA1_GATE_VOLTAGE       = 0x0042,
        LNA2_DRAIN_VOLTAGE      = 0x0044,
        LNA2_DRAIN_CURRENT      = 0x0045,
        LNA2_GATE_VOLTAGE       = 0x0046,
        LNA3_DRAIN_VOLTAGE      = 0x0048,
        LNA3_DRAIN_CURRENT      = 0x0049,
        LNA3_GATE_VOLTAGE       = 0x004A,
        LNA4_DRAIN_VOLTAGE      = 0x004C,
        LNA4_DRAIN_CURRENT      = 0x004D,
        LNA4_GATE_VOLTAGE       = 0x004E,
        LNA5_DRAIN_VOLTAGE      = 0x0050,
        LNA5_DRAIN_CURRENT      = 0x0051,
        LNA5_GATE_VOLTAGE       = 0x0052,
        LNA6_DRAIN_VOLTAGE      = 0x0054,
        LNA6_DRAIN_CURRENT      = 0x0055,
        LNA6_GATE_VOLTAGE       = 0x0056,
        LNA_LED_ENABLE          = 0x0100,
        SIS_HEATER_ENABLE       = 0x0180,
        SIS_HEATER_CURRENT      = 0x01C0
    };    
    
    float sisPol0Sb1Voltage_value;
    float sisPol0Sb1Current_value;
    bool sisPol0Sb1OpenLoop_value;
    float sisPol0Sb2Voltage_value;
    float sisPol0Sb2Current_value;
    bool sisPol0Sb2OpenLoop_value;
    float sisPol1Sb1Voltage_value;
    float sisPol1Sb1Current_value;
    bool sisPol1Sb1OpenLoop_value;
    float sisPol1Sb2Voltage_value;
    float sisPol1Sb2Current_value;
    bool sisPol1Sb2OpenLoop_value;
    
    float sisMagnetPol0Sb1Voltage_value;
    float sisMagnetPol0Sb1Current_value;
    float sisMagnetPol0Sb2Voltage_value;
    float sisMagnetPol0Sb2Current_value;
    float sisMagnetPol1Sb1Voltage_value;
    float sisMagnetPol1Sb1Current_value;
    float sisMagnetPol1Sb2Voltage_value;
    float sisMagnetPol1Sb2Current_value;

    bool lnaPol0Sb1Enable_value;
    float lnaPol0Sb1St1DrainVoltage_value;
    float lnaPol0Sb1St1DrainCurrent_value;
    float lnaPol0Sb1St1GateVoltage_value;
    float lnaPol0Sb1St2DrainVoltage_value;
    float lnaPol0Sb1St2DrainCurrent_value;
    float lnaPol0Sb1St2GateVoltage_value;
    float lnaPol0Sb1St3DrainVoltage_value;
    float lnaPol0Sb1St3DrainCurrent_value;
    float lnaPol0Sb1St3GateVoltage_value;
    bool lnaPol0Sb2Enable_value;
    float lnaPol0Sb2St1DrainVoltage_value;
    float lnaPol0Sb2St1DrainCurrent_value;
    float lnaPol0Sb2St1GateVoltage_value;
    float lnaPol0Sb2St2DrainVoltage_value;
    float lnaPol0Sb2St2DrainCurrent_value;
    float lnaPol0Sb2St2GateVoltage_value;
    float lnaPol0Sb2St3DrainVoltage_value;
    float lnaPol0Sb2St3DrainCurrent_value;
    float lnaPol0Sb2St3GateVoltage_value;
    bool lnaPol1Sb1Enable_value;
    float lnaPol1Sb1St1DrainVoltage_value;
    float lnaPol1Sb1St1DrainCurrent_value;
    float lnaPol1Sb1St1GateVoltage_value;
    float lnaPol1Sb1St2DrainVoltage_value;
    float lnaPol1Sb1St2DrainCurrent_value;
    float lnaPol1Sb1St2GateVoltage_value;
    float lnaPol1Sb1St3DrainVoltage_value;
    float lnaPol1Sb1St3DrainCurrent_value;
    float lnaPol1Sb1St3GateVoltage_value;
    bool lnaPol1Sb2Enable_value;
    float lnaPol1Sb2St1DrainVoltage_value;
    float lnaPol1Sb2St1DrainCurrent_value;
    float lnaPol1Sb2St1GateVoltage_value;
    float lnaPol1Sb2St2DrainVoltage_value;
    float lnaPol1Sb2St2DrainCurrent_value;
    float lnaPol1Sb2St2GateVoltage_value;
    float lnaPol1Sb2St3DrainVoltage_value;
    float lnaPol1Sb2St3DrainCurrent_value;
    float lnaPol1Sb2St3GateVoltage_value;
    
    bool lnaLedPol0Enable_value;
    bool lnaLedPol1Enable_value;
    
    float sisHeaterPol0Current_value;
    float sisHeaterPol1Current_value;

    float cartridgeTemperature0_value;
    float cartridgeTemperature1_value;
    float cartridgeTemperature2_value;
    float cartridgeTemperature3_value;
    float cartridgeTemperature4_value;
    float cartridgeTemperature5_value;

//
    int sisPol0Sb1Voltage_status;
    int sisPol0Sb1Current_status;
    int sisPol0Sb1OpenLoop_status;
    int sisPol0Sb2Voltage_status;
    int sisPol0Sb2Current_status;
    int sisPol0Sb2OpenLoop_status;
    int sisPol1Sb1Voltage_status;
    int sisPol1Sb1Current_status;
    int sisPol1Sb1OpenLoop_status;
    int sisPol1Sb2Voltage_status;
    int sisPol1Sb2Current_status;
    int sisPol1Sb2OpenLoop_status;
    
    int sisMagnetPol0Sb1Voltage_status;
    int sisMagnetPol0Sb1Current_status;
    int sisMagnetPol0Sb2Voltage_status;
    int sisMagnetPol0Sb2Current_status;
    int sisMagnetPol1Sb1Voltage_status;
    int sisMagnetPol1Sb1Current_status;
    int sisMagnetPol1Sb2Voltage_status;
    int sisMagnetPol1Sb2Current_status;

    int lnaPol0Sb1Enable_status;
    int lnaPol0Sb1St1DrainVoltage_status;
    int lnaPol0Sb1St1DrainCurrent_status;
    int lnaPol0Sb1St1GateVoltage_status;
    int lnaPol0Sb1St2DrainVoltage_status;
    int lnaPol0Sb1St2DrainCurrent_status;
    int lnaPol0Sb1St2GateVoltage_status;
    int lnaPol0Sb1St3DrainVoltage_status;
    int lnaPol0Sb1St3DrainCurrent_status;
    int lnaPol0Sb1St3GateVoltage_status;
    int lnaPol0Sb2Enable_status;
    int lnaPol0Sb2St1DrainVoltage_status;
    int lnaPol0Sb2St1DrainCurrent_status;
    int lnaPol0Sb2St1GateVoltage_status;
    int lnaPol0Sb2St2DrainVoltage_status;
    int lnaPol0Sb2St2DrainCurrent_status;
    int lnaPol0Sb2St2GateVoltage_status;
    int lnaPol0Sb2St3DrainVoltage_status;
    int lnaPol0Sb2St3DrainCurrent_status;
    int lnaPol0Sb2St3GateVoltage_status;
    int lnaPol1Sb1Enable_status;
    int lnaPol1Sb1St1DrainVoltage_status;
    int lnaPol1Sb1St1DrainCurrent_status;
    int lnaPol1Sb1St1GateVoltage_status;
    int lnaPol1Sb1St2DrainVoltage_status;
    int lnaPol1Sb1St2DrainCurrent_status;
    int lnaPol1Sb1St2GateVoltage_status;
    int lnaPol1Sb1St3DrainVoltage_status;
    int lnaPol1Sb1St3DrainCurrent_status;
    int lnaPol1Sb1St3GateVoltage_status;
    int lnaPol1Sb2Enable_status;
    int lnaPol1Sb2St1DrainVoltage_status;
    int lnaPol1Sb2St1DrainCurrent_status;
    int lnaPol1Sb2St1GateVoltage_status;
    int lnaPol1Sb2St2DrainVoltage_status;
    int lnaPol1Sb2St2DrainCurrent_status;
    int lnaPol1Sb2St2GateVoltage_status;
    int lnaPol1Sb2St3DrainVoltage_status;
    int lnaPol1Sb2St3DrainCurrent_status;
    int lnaPol1Sb2St3GateVoltage_status;
    
    int lnaLedPol0Enable_status;
    int lnaLedPol1Enable_status;
    
    int sisHeaterPol0Current_status;
    int sisHeaterPol1Current_status;

    int sisHeaterPol0Enable_status;
    int sisHeaterPol1Enable_status;
    
    int cartridgeTemperature0_status;
    int cartridgeTemperature1_status;
    int cartridgeTemperature2_status;
    int cartridgeTemperature3_status;
    int cartridgeTemperature4_status;
    int cartridgeTemperature5_status;

//

    AmbRelativeAddr baseRCA;
    AmbRelativeAddr sisPol0Sb1Voltage_RCA;
    AmbRelativeAddr sisPol0Sb1Current_RCA;
    AmbRelativeAddr sisPol0Sb1OpenLoop_RCA;
    AmbRelativeAddr sisPol0Sb2Voltage_RCA;
    AmbRelativeAddr sisPol0Sb2Current_RCA;
    AmbRelativeAddr sisPol0Sb2OpenLoop_RCA;
    AmbRelativeAddr sisPol1Sb1Voltage_RCA;
    AmbRelativeAddr sisPol1Sb1Current_RCA;
    AmbRelativeAddr sisPol1Sb1OpenLoop_RCA;
    AmbRelativeAddr sisPol1Sb2Voltage_RCA;
    AmbRelativeAddr sisPol1Sb2Current_RCA;
    AmbRelativeAddr sisPol1Sb2OpenLoop_RCA;
    
    AmbRelativeAddr sisMagnetPol0Sb1Voltage_RCA;
    AmbRelativeAddr sisMagnetPol0Sb1Current_RCA;
    AmbRelativeAddr sisMagnetPol0Sb2Voltage_RCA;
    AmbRelativeAddr sisMagnetPol0Sb2Current_RCA;
    AmbRelativeAddr sisMagnetPol1Sb1Voltage_RCA;
    AmbRelativeAddr sisMagnetPol1Sb1Current_RCA;
    AmbRelativeAddr sisMagnetPol1Sb2Voltage_RCA;
    AmbRelativeAddr sisMagnetPol1Sb2Current_RCA;

    AmbRelativeAddr lnaPol0Sb1Enable_RCA;
    AmbRelativeAddr lnaPol0Sb1St1DrainVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb1St1DrainCurrent_RCA;
    AmbRelativeAddr lnaPol0Sb1St1GateVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb1St2DrainVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb1St2DrainCurrent_RCA;
    AmbRelativeAddr lnaPol0Sb1St2GateVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb1St3DrainVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb1St3DrainCurrent_RCA;
    AmbRelativeAddr lnaPol0Sb1St3GateVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb2Enable_RCA;
    AmbRelativeAddr lnaPol0Sb2St1DrainVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb2St1DrainCurrent_RCA;
    AmbRelativeAddr lnaPol0Sb2St1GateVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb2St2DrainVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb2St2DrainCurrent_RCA;
    AmbRelativeAddr lnaPol0Sb2St2GateVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb2St3DrainVoltage_RCA;
    AmbRelativeAddr lnaPol0Sb2St3DrainCurrent_RCA;
    AmbRelativeAddr lnaPol0Sb2St3GateVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb1Enable_RCA;
    AmbRelativeAddr lnaPol1Sb1St1DrainVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb1St1DrainCurrent_RCA;
    AmbRelativeAddr lnaPol1Sb1St1GateVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb1St2DrainVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb1St2DrainCurrent_RCA;
    AmbRelativeAddr lnaPol1Sb1St2GateVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb1St3DrainVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb1St3DrainCurrent_RCA;
    AmbRelativeAddr lnaPol1Sb1St3GateVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb2Enable_RCA;
    AmbRelativeAddr lnaPol1Sb2St1DrainVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb2St1DrainCurrent_RCA;
    AmbRelativeAddr lnaPol1Sb2St1GateVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb2St2DrainVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb2St2DrainCurrent_RCA;
    AmbRelativeAddr lnaPol1Sb2St2GateVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb2St3DrainVoltage_RCA;
    AmbRelativeAddr lnaPol1Sb2St3DrainCurrent_RCA;
    AmbRelativeAddr lnaPol1Sb2St3GateVoltage_RCA;
    
    AmbRelativeAddr lnaLedPol0Enable_RCA;
    AmbRelativeAddr lnaLedPol1Enable_RCA;
    
    AmbRelativeAddr sisHeaterPol0Enable_RCA;
    AmbRelativeAddr sisHeaterPol0Current_RCA;
    AmbRelativeAddr sisHeaterPol1Enable_RCA;
    AmbRelativeAddr sisHeaterPol1Current_RCA;
        
    AmbRelativeAddr cartridgeTemperature0_RCA;
    AmbRelativeAddr cartridgeTemperature1_RCA;
    AmbRelativeAddr cartridgeTemperature2_RCA;
    AmbRelativeAddr cartridgeTemperature3_RCA;
    AmbRelativeAddr cartridgeTemperature4_RCA;
    AmbRelativeAddr cartridgeTemperature5_RCA;

protected:    
    int port_m; ///< to which FEMC port is this module connected. 

private:
    // forbid copy constructor:
    ColdCartImplBase(const ColdCartImplBase &other);
    
    Time lastMonitorTime;
    int monitorPhase;

    DECLARE_MONITORS_REGISTRY(ColdCartImplBase)
    void logMon(bool printHeader = false) const;
};      

#endif /*COLDCARTIMPLBASE_H_*/
