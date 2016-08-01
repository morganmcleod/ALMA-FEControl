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
 *----------------------------------------------------------------------
 */
 
#include "ColdCartImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

ColdCartImplBase::ColdCartImplBase(const std::string &name, int port)
  : FEHardwareDevice(name),
    sisPol0Sb1Voltage_value(0),
    sisPol0Sb1Current_value(0),
    sisPol0Sb1OpenLoop_value(false),
    sisPol0Sb2Voltage_value(0),
    sisPol0Sb2Current_value(0),
    sisPol0Sb2OpenLoop_value(false),
    sisPol1Sb1Voltage_value(0),
    sisPol1Sb1Current_value(0),
    sisPol1Sb1OpenLoop_value(false),
    sisPol1Sb2Voltage_value(0),
    sisPol1Sb2Current_value(0),
    sisPol1Sb2OpenLoop_value(false),
    sisMagnetPol0Sb1Voltage_value(0),
    sisMagnetPol0Sb1Current_value(0),
    sisMagnetPol0Sb2Voltage_value(0),
    sisMagnetPol0Sb2Current_value(0),
    sisMagnetPol1Sb1Voltage_value(0),
    sisMagnetPol1Sb1Current_value(0),
    sisMagnetPol1Sb2Voltage_value(0),
    sisMagnetPol1Sb2Current_value(0),
    lnaPol0Sb1Enable_value(false),
    lnaPol0Sb1St1DrainVoltage_value(0),
    lnaPol0Sb1St1DrainCurrent_value(0),
    lnaPol0Sb1St1GateVoltage_value(0),
    lnaPol0Sb1St2DrainVoltage_value(0),
    lnaPol0Sb1St2DrainCurrent_value(0),
    lnaPol0Sb1St2GateVoltage_value(0),
    lnaPol0Sb1St3DrainVoltage_value(0),
    lnaPol0Sb1St3DrainCurrent_value(0),
    lnaPol0Sb1St3GateVoltage_value(0),
    lnaPol0Sb2Enable_value(false),
    lnaPol0Sb2St1DrainVoltage_value(0),
    lnaPol0Sb2St1DrainCurrent_value(0),
    lnaPol0Sb2St1GateVoltage_value(0),
    lnaPol0Sb2St2DrainVoltage_value(0),
    lnaPol0Sb2St2DrainCurrent_value(0),
    lnaPol0Sb2St2GateVoltage_value(0),
    lnaPol0Sb2St3DrainVoltage_value(0),
    lnaPol0Sb2St3DrainCurrent_value(0),
    lnaPol0Sb2St3GateVoltage_value(0),
    lnaPol1Sb1Enable_value(false),
    lnaPol1Sb1St1DrainVoltage_value(0),
    lnaPol1Sb1St1DrainCurrent_value(0),
    lnaPol1Sb1St1GateVoltage_value(0),
    lnaPol1Sb1St2DrainVoltage_value(0),
    lnaPol1Sb1St2DrainCurrent_value(0),
    lnaPol1Sb1St2GateVoltage_value(0),
    lnaPol1Sb1St3DrainVoltage_value(0),
    lnaPol1Sb1St3DrainCurrent_value(0),
    lnaPol1Sb1St3GateVoltage_value(0),
    lnaPol1Sb2Enable_value(false),
    lnaPol1Sb2St1DrainVoltage_value(0),
    lnaPol1Sb2St1DrainCurrent_value(0),
    lnaPol1Sb2St1GateVoltage_value(0),
    lnaPol1Sb2St2DrainVoltage_value(0),
    lnaPol1Sb2St2DrainCurrent_value(0),
    lnaPol1Sb2St2GateVoltage_value(0),
    lnaPol1Sb2St3DrainVoltage_value(0),
    lnaPol1Sb2St3DrainCurrent_value(0),
    lnaPol1Sb2St3GateVoltage_value(0),
    sisPol0Sb1Voltage_status(FEMC_NOT_CONNECTED),
    sisPol0Sb1Current_status(FEMC_NOT_CONNECTED),
    sisPol0Sb1OpenLoop_status(FEMC_NOT_CONNECTED),
    sisPol0Sb2Voltage_status(FEMC_NOT_CONNECTED),
    sisPol0Sb2Current_status(FEMC_NOT_CONNECTED),
    sisPol0Sb2OpenLoop_status(FEMC_NOT_CONNECTED),
    sisPol1Sb1Voltage_status(FEMC_NOT_CONNECTED),
    sisPol1Sb1Current_status(FEMC_NOT_CONNECTED),
    sisPol1Sb1OpenLoop_status(FEMC_NOT_CONNECTED),
    sisPol1Sb2Voltage_status(FEMC_NOT_CONNECTED),
    sisPol1Sb2Current_status(FEMC_NOT_CONNECTED),
    sisPol1Sb2OpenLoop_status(FEMC_NOT_CONNECTED),
    sisMagnetPol0Sb1Voltage_status(FEMC_NOT_CONNECTED),
    sisMagnetPol0Sb1Current_status(FEMC_NOT_CONNECTED),
    sisMagnetPol0Sb2Voltage_status(FEMC_NOT_CONNECTED),
    sisMagnetPol0Sb2Current_status(FEMC_NOT_CONNECTED),
    sisMagnetPol1Sb1Voltage_status(FEMC_NOT_CONNECTED),
    sisMagnetPol1Sb1Current_status(FEMC_NOT_CONNECTED),
    sisMagnetPol1Sb2Voltage_status(FEMC_NOT_CONNECTED),
    sisMagnetPol1Sb2Current_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1Enable_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St1DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St1DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St1GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St2DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St2DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St2GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St3DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St3DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb1St3GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2Enable_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St1DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St1DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St1GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St2DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St2DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St2GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St3DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St3DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol0Sb2St3GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1Enable_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St1DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St1DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St1GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St2DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St2DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St2GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St3DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St3DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb1St3GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2Enable_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St1DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St1DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St1GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St2DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St2DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St2GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St3DrainVoltage_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St3DrainCurrent_status(FEMC_NOT_CONNECTED),
    lnaPol1Sb2St3GateVoltage_status(FEMC_NOT_CONNECTED),
    lnaLedPol0Enable_status(FEMC_NOT_CONNECTED),
    lnaLedPol1Enable_status(FEMC_NOT_CONNECTED),
    sisHeaterPol0Current_status(FEMC_NOT_CONNECTED),
    sisHeaterPol1Current_status(FEMC_NOT_CONNECTED),
    cartridgeTemperature0_status(FEMC_NOT_CONNECTED),
    cartridgeTemperature1_status(FEMC_NOT_CONNECTED),
    cartridgeTemperature2_status(FEMC_NOT_CONNECTED),
    cartridgeTemperature3_status(FEMC_NOT_CONNECTED),
    cartridgeTemperature4_status(FEMC_NOT_CONNECTED),
    cartridgeTemperature5_status(FEMC_NOT_CONNECTED),
    lnaLedPol0Enable_value(false),
    lnaLedPol1Enable_value(false),
    sisHeaterPol0Current_value(0),
    sisHeaterPol1Current_value(0),
    baseRCA(0),
    port_m(port),
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = port - 1;
    baseRCA <<= 12;
    
    AmbRelativeAddr pol1 = 0x0400;
    AmbRelativeAddr sb2  = 0x0080;

    sisPol0Sb1Voltage_RCA           = baseRCA + SIS_VOLTAGE;
    sisPol0Sb1Current_RCA           = baseRCA + SIS_CURRENT;
    sisPol0Sb1OpenLoop_RCA          = baseRCA + SIS_OPEN_LOOP;
    sisPol0Sb2Voltage_RCA           = baseRCA + sb2 + SIS_VOLTAGE;
    sisPol0Sb2Current_RCA           = baseRCA + sb2 + SIS_CURRENT;
    sisPol0Sb2OpenLoop_RCA          = baseRCA + sb2 + SIS_OPEN_LOOP;
    sisPol1Sb1Voltage_RCA           = baseRCA + pol1 + SIS_VOLTAGE;
    sisPol1Sb1Current_RCA           = baseRCA + pol1 + SIS_CURRENT;
    sisPol1Sb1OpenLoop_RCA          = baseRCA + pol1 + SIS_OPEN_LOOP;
    sisPol1Sb2Voltage_RCA           = baseRCA + pol1 + sb2 + SIS_VOLTAGE;
    sisPol1Sb2Current_RCA           = baseRCA + pol1 + sb2 + SIS_CURRENT;
    sisPol1Sb2OpenLoop_RCA          = baseRCA + pol1 + sb2 + SIS_OPEN_LOOP;
    
    sisMagnetPol0Sb1Voltage_RCA     = baseRCA + SIS_MAGNET_VOLTAGE;
    sisMagnetPol0Sb1Current_RCA     = baseRCA + SIS_MAGNET_CURRENT;
    sisMagnetPol0Sb2Voltage_RCA     = baseRCA + sb2 + SIS_MAGNET_VOLTAGE;
    sisMagnetPol0Sb2Current_RCA     = baseRCA + sb2 + SIS_MAGNET_CURRENT;
    sisMagnetPol1Sb1Voltage_RCA     = baseRCA + pol1 + SIS_MAGNET_VOLTAGE;
    sisMagnetPol1Sb1Current_RCA     = baseRCA + pol1 + SIS_MAGNET_CURRENT;
    sisMagnetPol1Sb2Voltage_RCA     = baseRCA + pol1 + sb2+ SIS_MAGNET_VOLTAGE;
    sisMagnetPol1Sb2Current_RCA     = baseRCA + pol1 + sb2 + SIS_MAGNET_CURRENT;

    lnaPol0Sb1Enable_RCA            = baseRCA + LNA_ENABLE;
    lnaPol0Sb1St1DrainVoltage_RCA   = baseRCA + LNA1_DRAIN_VOLTAGE;
    lnaPol0Sb1St1DrainCurrent_RCA   = baseRCA + LNA1_DRAIN_CURRENT;
    lnaPol0Sb1St1GateVoltage_RCA    = baseRCA + LNA1_GATE_VOLTAGE;
    lnaPol0Sb1St2DrainVoltage_RCA   = baseRCA + LNA2_DRAIN_VOLTAGE;
    lnaPol0Sb1St2DrainCurrent_RCA   = baseRCA + LNA2_DRAIN_CURRENT;
    lnaPol0Sb1St2GateVoltage_RCA    = baseRCA + LNA2_GATE_VOLTAGE;
    lnaPol0Sb1St3DrainVoltage_RCA   = baseRCA + LNA3_DRAIN_VOLTAGE;
    lnaPol0Sb1St3DrainCurrent_RCA   = baseRCA + LNA3_DRAIN_CURRENT;
    lnaPol0Sb1St3GateVoltage_RCA    = baseRCA + LNA3_GATE_VOLTAGE;

    lnaPol0Sb2Enable_RCA            = baseRCA + sb2 + LNA_ENABLE;
    lnaPol0Sb2St1DrainVoltage_RCA   = baseRCA + sb2 + LNA1_DRAIN_VOLTAGE;
    lnaPol0Sb2St1DrainCurrent_RCA   = baseRCA + sb2 + LNA1_DRAIN_CURRENT;
    lnaPol0Sb2St1GateVoltage_RCA    = baseRCA + sb2 + LNA1_GATE_VOLTAGE;
    lnaPol0Sb2St2DrainVoltage_RCA   = baseRCA + sb2 + LNA2_DRAIN_VOLTAGE;
    lnaPol0Sb2St2DrainCurrent_RCA   = baseRCA + sb2 + LNA2_DRAIN_CURRENT;
    lnaPol0Sb2St2GateVoltage_RCA    = baseRCA + sb2 + LNA2_GATE_VOLTAGE;
    lnaPol0Sb2St3DrainVoltage_RCA   = baseRCA + sb2 + LNA3_DRAIN_VOLTAGE;
    lnaPol0Sb2St3DrainCurrent_RCA   = baseRCA + sb2 + LNA3_DRAIN_CURRENT;
    lnaPol0Sb2St3GateVoltage_RCA    = baseRCA + sb2 + LNA3_GATE_VOLTAGE;

    lnaPol1Sb1Enable_RCA            = baseRCA + pol1 + LNA_ENABLE;
    lnaPol1Sb1St1DrainVoltage_RCA   = baseRCA + pol1 + LNA1_DRAIN_VOLTAGE;
    lnaPol1Sb1St1DrainCurrent_RCA   = baseRCA + pol1 + LNA1_DRAIN_CURRENT;
    lnaPol1Sb1St1GateVoltage_RCA    = baseRCA + pol1 + LNA1_GATE_VOLTAGE;
    lnaPol1Sb1St2DrainVoltage_RCA   = baseRCA + pol1 + LNA2_DRAIN_VOLTAGE;
    lnaPol1Sb1St2DrainCurrent_RCA   = baseRCA + pol1 + LNA2_DRAIN_CURRENT;
    lnaPol1Sb1St2GateVoltage_RCA    = baseRCA + pol1 + LNA2_GATE_VOLTAGE;
    lnaPol1Sb1St3DrainVoltage_RCA   = baseRCA + pol1 + LNA3_DRAIN_VOLTAGE;
    lnaPol1Sb1St3DrainCurrent_RCA   = baseRCA + pol1 + LNA3_DRAIN_CURRENT;
    lnaPol1Sb1St3GateVoltage_RCA    = baseRCA + pol1 + LNA3_GATE_VOLTAGE;
    
    lnaPol1Sb2Enable_RCA            = baseRCA + pol1 + sb2 + LNA_ENABLE;
    lnaPol1Sb2St1DrainVoltage_RCA   = baseRCA + pol1 + sb2 + LNA1_DRAIN_VOLTAGE;
    lnaPol1Sb2St1DrainCurrent_RCA   = baseRCA + pol1 + sb2 + LNA1_DRAIN_CURRENT;
    lnaPol1Sb2St1GateVoltage_RCA    = baseRCA + pol1 + sb2 + LNA1_GATE_VOLTAGE;
    lnaPol1Sb2St2DrainVoltage_RCA   = baseRCA + pol1 + sb2 + LNA2_DRAIN_VOLTAGE;
    lnaPol1Sb2St2DrainCurrent_RCA   = baseRCA + pol1 + sb2 + LNA2_DRAIN_CURRENT;
    lnaPol1Sb2St2GateVoltage_RCA    = baseRCA + pol1 + sb2 + LNA2_GATE_VOLTAGE;
    lnaPol1Sb2St3DrainVoltage_RCA   = baseRCA + pol1 + sb2 + LNA3_DRAIN_VOLTAGE;
    lnaPol1Sb2St3DrainCurrent_RCA   = baseRCA + pol1 + sb2 + LNA3_DRAIN_CURRENT;
    lnaPol1Sb2St3GateVoltage_RCA    = baseRCA + pol1 + sb2 + LNA3_GATE_VOLTAGE;
    
    lnaLedPol0Enable_RCA            = baseRCA + LNA_LED_ENABLE;
    lnaLedPol1Enable_RCA            = baseRCA + pol1 + LNA_LED_ENABLE;
    
    sisHeaterPol0Enable_RCA         = baseRCA + SIS_HEATER_ENABLE;
    sisHeaterPol0Current_RCA        = baseRCA + SIS_HEATER_CURRENT;
    sisHeaterPol1Enable_RCA         = baseRCA + pol1 + SIS_HEATER_ENABLE;
    sisHeaterPol1Current_RCA        = baseRCA + pol1 + SIS_HEATER_CURRENT;

    cartridgeTemperature0_RCA       = baseRCA + CARTRIDGE_TEMP;
    cartridgeTemperature1_RCA       = baseRCA + CARTRIDGE_TEMP + 0x10;
    cartridgeTemperature2_RCA       = baseRCA + CARTRIDGE_TEMP + 0x20;
    cartridgeTemperature3_RCA       = baseRCA + CARTRIDGE_TEMP + 0x30;
    cartridgeTemperature4_RCA       = baseRCA + CARTRIDGE_TEMP + 0x40;
    cartridgeTemperature5_RCA       = baseRCA + CARTRIDGE_TEMP + 0x50;
    
    // Add all the analog monitor points to the registry:
    // EXCEPT SIS current monitoring which we handle separately to do averaging.
    //addMon(&sisPol0Sb1Current_value, &ColdCartImplBase::sisPol0Sb1Current);
    //addMon(&sisPol0Sb2Current_value, &ColdCartImplBase::sisPol0Sb2Current);
    //addMon(&sisPol1Sb1Current_value, &ColdCartImplBase::sisPol1Sb1Current);
    //addMon(&sisPol1Sb2Current_value, &ColdCartImplBase::sisPol1Sb2Current);
    addMon(&sisPol0Sb1Voltage_value, &ColdCartImplBase::sisPol0Sb1Voltage);
    addMon(&sisPol0Sb2Voltage_value, &ColdCartImplBase::sisPol0Sb2Voltage);
    addMon(&sisPol1Sb1Voltage_value, &ColdCartImplBase::sisPol1Sb1Voltage);
    addMon(&sisPol1Sb2Voltage_value, &ColdCartImplBase::sisPol1Sb2Voltage);
    addMon(&sisMagnetPol0Sb1Voltage_value, &ColdCartImplBase::sisMagnetPol0Sb1Voltage);
    addMon(&sisMagnetPol0Sb1Current_value, &ColdCartImplBase::sisMagnetPol0Sb1Current);
    addMon(&sisMagnetPol0Sb2Voltage_value, &ColdCartImplBase::sisMagnetPol0Sb2Voltage);
    addMon(&sisMagnetPol0Sb2Current_value, &ColdCartImplBase::sisMagnetPol0Sb2Current);
    addMon(&sisMagnetPol1Sb1Voltage_value, &ColdCartImplBase::sisMagnetPol1Sb1Voltage);
    addMon(&sisMagnetPol1Sb1Current_value, &ColdCartImplBase::sisMagnetPol1Sb1Current);
    addMon(&sisMagnetPol1Sb2Voltage_value, &ColdCartImplBase::sisMagnetPol1Sb2Voltage);
    addMon(&sisMagnetPol1Sb2Current_value, &ColdCartImplBase::sisMagnetPol1Sb2Current);
    addMon(&lnaPol0Sb1St1DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb1St1DrainVoltage);
    addMon(&lnaPol0Sb1St1DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb1St1DrainCurrent);
    addMon(&lnaPol0Sb1St1GateVoltage_value, &ColdCartImplBase::lnaPol0Sb1St1GateVoltage);
    addMon(&lnaPol0Sb1St2DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb1St2DrainVoltage);
    addMon(&lnaPol0Sb1St2DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb1St2DrainCurrent);
    addMon(&lnaPol0Sb1St2GateVoltage_value, &ColdCartImplBase::lnaPol0Sb1St2GateVoltage);
    addMon(&lnaPol0Sb1St3DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb1St3DrainVoltage);
    addMon(&lnaPol0Sb1St3DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb1St3DrainCurrent);
    addMon(&lnaPol0Sb1St3GateVoltage_value, &ColdCartImplBase::lnaPol0Sb1St3GateVoltage);
    addMon(&lnaPol0Sb2St1DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb2St1DrainVoltage);
    addMon(&lnaPol0Sb2St1DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb2St1DrainCurrent);
    addMon(&lnaPol0Sb2St1GateVoltage_value, &ColdCartImplBase::lnaPol0Sb2St1GateVoltage);
    addMon(&lnaPol0Sb2St2DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb2St2DrainVoltage);
    addMon(&lnaPol0Sb2St2DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb2St2DrainCurrent);
    addMon(&lnaPol0Sb2St2GateVoltage_value, &ColdCartImplBase::lnaPol0Sb2St2GateVoltage);
    addMon(&lnaPol0Sb2St3DrainVoltage_value, &ColdCartImplBase::lnaPol0Sb2St3DrainVoltage);
    addMon(&lnaPol0Sb2St3DrainCurrent_value, &ColdCartImplBase::lnaPol0Sb2St3DrainCurrent);
    addMon(&lnaPol0Sb2St3GateVoltage_value, &ColdCartImplBase::lnaPol0Sb2St3GateVoltage);
    addMon(&lnaPol1Sb1St1DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb1St1DrainVoltage);
    addMon(&lnaPol1Sb1St1DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb1St1DrainCurrent);
    addMon(&lnaPol1Sb1St1GateVoltage_value, &ColdCartImplBase::lnaPol1Sb1St1GateVoltage);
    addMon(&lnaPol1Sb1St2DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb1St2DrainVoltage);
    addMon(&lnaPol1Sb1St2DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb1St2DrainCurrent);
    addMon(&lnaPol1Sb1St2GateVoltage_value, &ColdCartImplBase::lnaPol1Sb1St2GateVoltage);
    addMon(&lnaPol1Sb1St3DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb1St3DrainVoltage);
    addMon(&lnaPol1Sb1St3DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb1St3DrainCurrent);
    addMon(&lnaPol1Sb1St3GateVoltage_value, &ColdCartImplBase::lnaPol1Sb1St3GateVoltage);
    addMon(&lnaPol1Sb2St1DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb2St1DrainVoltage);
    addMon(&lnaPol1Sb2St1DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb2St1DrainCurrent);
    addMon(&lnaPol1Sb2St1GateVoltage_value, &ColdCartImplBase::lnaPol1Sb2St1GateVoltage);
    addMon(&lnaPol1Sb2St2DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb2St2DrainVoltage);
    addMon(&lnaPol1Sb2St2DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb2St2DrainCurrent);
    addMon(&lnaPol1Sb2St2GateVoltage_value, &ColdCartImplBase::lnaPol1Sb2St2GateVoltage);
    addMon(&lnaPol1Sb2St3DrainVoltage_value, &ColdCartImplBase::lnaPol1Sb2St3DrainVoltage);
    addMon(&lnaPol1Sb2St3DrainCurrent_value, &ColdCartImplBase::lnaPol1Sb2St3DrainCurrent);
    addMon(&lnaPol1Sb2St3GateVoltage_value, &ColdCartImplBase::lnaPol1Sb2St3GateVoltage);
    addMon(&sisHeaterPol0Current_value, &ColdCartImplBase::sisHeaterPol0Current);
    addMon(&sisHeaterPol1Current_value, &ColdCartImplBase::sisHeaterPol1Current);
    addMon(&cartridgeTemperature0_value, &ColdCartImplBase::cartridgeTemperature0);
    addMon(&cartridgeTemperature1_value, &ColdCartImplBase::cartridgeTemperature1);
    addMon(&cartridgeTemperature2_value, &ColdCartImplBase::cartridgeTemperature2);
    addMon(&cartridgeTemperature3_value, &ColdCartImplBase::cartridgeTemperature3);
    addMon(&cartridgeTemperature4_value, &ColdCartImplBase::cartridgeTemperature4);
    addMon(&cartridgeTemperature5_value, &ColdCartImplBase::cartridgeTemperature5);
    
    nextMon = monitorRegistry.begin();
}

void ColdCartImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void ColdCartImplBase::shutdown() {
}

// monitor properties:
float ColdCartImplBase::sisPol0Sb1Voltage() {
    SYNCMON_LOG_FLOAT(sisPol0Sb1Voltage, "SIS_VOLTAGE Po=0 Sb=1")
}

float ColdCartImplBase::sisPol0Sb2Voltage() {
    SYNCMON_LOG_FLOAT(sisPol0Sb2Voltage, "SIS_VOLTAGE Po=0 Sb=2")
}

float ColdCartImplBase::sisPol1Sb1Voltage() {
    SYNCMON_LOG_FLOAT(sisPol1Sb1Voltage, "SIS_VOLTAGE Po=1 Sb=1")
}

float ColdCartImplBase::sisPol1Sb2Voltage() {
    SYNCMON_LOG_FLOAT(sisPol1Sb2Voltage, "SIS_VOLTAGE Po=1 Sb=2")
}

float ColdCartImplBase::sisPol0Sb1Current() {
    SYNCMON_LOG_FLOAT(sisPol0Sb1Current, "SIS_CURRENT Po=0 Sb=1")
}

float ColdCartImplBase::sisPol0Sb2Current() {
    SYNCMON_LOG_FLOAT(sisPol0Sb2Current, "SIS_CURRENT Po=0 Sb=2")
}

float ColdCartImplBase::sisPol1Sb1Current() {
    SYNCMON_LOG_FLOAT(sisPol1Sb1Current, "SIS_CURRENT Po=1 Sb=1")
}

float ColdCartImplBase::sisPol1Sb2Current() {
    SYNCMON_LOG_FLOAT(sisPol1Sb2Current, "SIS_CURRENT Po=1 Sb=2")
}

float ColdCartImplBase::avgSisPol0Sb1Current(int average) {
    SYNCMON_AVG_LOG_FLOAT(sisPol0Sb1Current, "SIS_CURRENT Po=0 Sb=1", average)
}

float ColdCartImplBase::avgSisPol0Sb2Current(int average) {
    SYNCMON_AVG_LOG_FLOAT(sisPol0Sb2Current, "SIS_CURRENT Po=0 Sb=2", average)
}

float ColdCartImplBase::avgSisPol1Sb1Current(int average) {
    SYNCMON_AVG_LOG_FLOAT(sisPol1Sb1Current, "SIS_CURRENT Po=1 Sb=1", average)
}

float ColdCartImplBase::avgSisPol1Sb2Current(int average) {
    SYNCMON_AVG_LOG_FLOAT(sisPol1Sb2Current, "SIS_CURRENT Po=1 Sb=2", average)
}

bool ColdCartImplBase::sisPol0Sb1OpenLoop() {
    SYNCMON_LOG_BOOL(sisPol0Sb1OpenLoop, "SIS_OPEN_LOOP Po=0 Sb=1")
}

bool ColdCartImplBase::sisPol0Sb2OpenLoop() {
    SYNCMON_LOG_BOOL(sisPol0Sb2OpenLoop, "SIS_OPEN_LOOP Po=0 Sb=2")
}

bool ColdCartImplBase::sisPol1Sb1OpenLoop() {
    SYNCMON_LOG_BOOL(sisPol1Sb1OpenLoop, "SIS_OPEN_LOOP Po=1 Sb=1")
}

bool ColdCartImplBase::sisPol1Sb2OpenLoop() {
    SYNCMON_LOG_BOOL(sisPol1Sb2OpenLoop, "SIS_OPEN_LOOP Po=1 Sb=2")
}
    
float ColdCartImplBase::sisMagnetPol0Sb1Voltage() {
    SYNCMON_LOG_FLOAT(sisMagnetPol0Sb1Voltage, "SIS_MAGNET_VOLTAGE Po=0 Sb=1")
}

float ColdCartImplBase::sisMagnetPol0Sb1Current() {
    SYNCMON_LOG_FLOAT(sisMagnetPol0Sb1Current, "SIS_MAGNET_CURRENT Po=0 Sb=1")
}

float ColdCartImplBase::sisMagnetPol0Sb2Voltage() {
    SYNCMON_LOG_FLOAT(sisMagnetPol0Sb2Voltage, "SIS_MAGNET_VOLTAGE Po=0 Sb=2")
}

float ColdCartImplBase::sisMagnetPol0Sb2Current() {
    SYNCMON_LOG_FLOAT(sisMagnetPol0Sb2Current, "SIS_MAGNET_CURRENT Po=0 Sb=2")
}

float ColdCartImplBase::sisMagnetPol1Sb1Voltage() {
    SYNCMON_LOG_FLOAT(sisMagnetPol1Sb1Voltage, "SIS_MAGNET_VOLTAGE Po=1 Sb=1")
}

float ColdCartImplBase::sisMagnetPol1Sb1Current() {
    SYNCMON_LOG_FLOAT(sisMagnetPol1Sb1Current, "SIS_MAGNET_CURRENT Po=1 Sb=1")
}

float ColdCartImplBase::sisMagnetPol1Sb2Voltage() {
    SYNCMON_LOG_FLOAT(sisMagnetPol1Sb2Voltage, "SIS_MAGNET_VOLTAGE Po=1 Sb=2")
}

float ColdCartImplBase::sisMagnetPol1Sb2Current() {
    SYNCMON_LOG_FLOAT(sisMagnetPol1Sb2Current, "SIS_MAGNET_CURRENT Po=1 Sb=2")
}

bool ColdCartImplBase::lnaPol0Sb1Enable() {
    SYNCMON_LOG_BOOL(lnaPol0Sb1Enable, "LNA_ENABLE Po=0 Sb=1")
}

float ColdCartImplBase::lnaPol0Sb1St1DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St1DrainVoltage, "LNA_DRAIN_VOLTAGE Po=0 Sb=1 St=1")
}

float ColdCartImplBase::lnaPol0Sb1St1DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St1DrainCurrent, "LNA_DRAIN_CURRENT Po=0 Sb=1 St=1")
}

float ColdCartImplBase::lnaPol0Sb1St1GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St1GateVoltage, "LNA_GATE_VOLTAGE Po=0 Sb=1 St=1")
}

float ColdCartImplBase::lnaPol0Sb1St2DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St2DrainVoltage, "LNA_DRAIN_VOLTAGE Po=0 Sb=1 St=2")
}

float ColdCartImplBase::lnaPol0Sb1St2DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St2DrainCurrent, "LNA_DRAIN_CURRENT Po=0 Sb=1 St=2")
}

float ColdCartImplBase::lnaPol0Sb1St2GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St2GateVoltage, "LNA_GATE_VOLTAGE Po=0 Sb=1 St=2")
}

float ColdCartImplBase::lnaPol0Sb1St3DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St3DrainVoltage, "LNA_DRAIN_VOLTAGE Po=0 Sb=1 St=3")
}

float ColdCartImplBase::lnaPol0Sb1St3DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St3DrainCurrent, "LNA_DRAIN_CURRENT Po=0 Sb=1 St=3")
}

float ColdCartImplBase::lnaPol0Sb1St3GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb1St3GateVoltage, "LNA_GATE_VOLTAGE Po=0 Sb=1 St=3")
}

bool ColdCartImplBase::lnaPol0Sb2Enable() {
    SYNCMON_LOG_BOOL(lnaPol0Sb2Enable, "LNA_ENABLE Po=0 Sb=2")
}

float ColdCartImplBase::lnaPol0Sb2St1DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St1DrainVoltage, "LNA_DRAIN_VOLTAGE Po=0 Sb=2 St=1")
}

float ColdCartImplBase::lnaPol0Sb2St1DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St1DrainCurrent, "LNA_DRAIN_CURRENT Po=0 Sb=2 St=1")
}

float ColdCartImplBase::lnaPol0Sb2St1GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St1GateVoltage, "LNA_GATE_VOLTAGE Po=0 Sb=2 St=1")
}

float ColdCartImplBase::lnaPol0Sb2St2DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St2DrainVoltage, "LNA_DRAIN_VOLTAGE Po=0 Sb=2 St=2")
}

float ColdCartImplBase::lnaPol0Sb2St2DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St2DrainCurrent, "LNA_DRAIN_CURRENT Po=0 Sb=2 St=2")
}

float ColdCartImplBase::lnaPol0Sb2St2GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St2GateVoltage, "LNA_GATE_VOLTAGE Po=0 Sb=2 St=2")
}

float ColdCartImplBase::lnaPol0Sb2St3DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St3DrainVoltage, "LNA_DRAIN_VOLTAGE Po=0 Sb=2 St=3")
}

float ColdCartImplBase::lnaPol0Sb2St3DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St3DrainCurrent, "LNA_DRAIN_CURRENT Po=0 Sb=2 St=3")
}

float ColdCartImplBase::lnaPol0Sb2St3GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol0Sb2St3GateVoltage, "LNA_GATE_VOLTAGE Po=0 Sb=2 St=3")
}

bool ColdCartImplBase::lnaPol1Sb1Enable() {
    SYNCMON_LOG_BOOL(lnaPol1Sb1Enable, "LNA_ENABLE Po=1 Sb=1")
}

float ColdCartImplBase::lnaPol1Sb1St1DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St1DrainVoltage, "LNA_DRAIN_VOLTAGE Po=1 Sb=1 St=1")
}

float ColdCartImplBase::lnaPol1Sb1St1DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St1DrainCurrent, "LNA_DRAIN_CURRENT Po=1 Sb=1 St=1")
}

float ColdCartImplBase::lnaPol1Sb1St1GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St1GateVoltage, "LNA_GATE_VOLTAGE Po=1 Sb=1 St=1")
}

float ColdCartImplBase::lnaPol1Sb1St2DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St2DrainVoltage, "LNA_DRAIN_VOLTAGE Po=1 Sb=1 St=2")
}

float ColdCartImplBase::lnaPol1Sb1St2DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St2DrainCurrent, "LNA_DRAIN_CURRENT Po=1 Sb=1 St=2")
}

float ColdCartImplBase::lnaPol1Sb1St2GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St2GateVoltage, "LNA_GATE_VOLTAGE Po=1 Sb=1 St=2")
}

float ColdCartImplBase::lnaPol1Sb1St3DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St3DrainVoltage, "LNA_DRAIN_VOLTAGE Po=1 Sb=1 St=3")
}

float ColdCartImplBase::lnaPol1Sb1St3DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St3DrainCurrent, "LNA_DRAIN_CURRENT Po=1 Sb=1 St=3")
}

float ColdCartImplBase::lnaPol1Sb1St3GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb1St3GateVoltage, "LNA_GATE_VOLTAGE Po=1 Sb=1 St=3")
}

bool ColdCartImplBase::lnaPol1Sb2Enable() {
    SYNCMON_LOG_BOOL(lnaPol1Sb2Enable, "LNA_ENABLE Po=1 Sb=2")
}

float ColdCartImplBase::lnaPol1Sb2St1DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St1DrainVoltage, "LNA_DRAIN_VOLTAGE Po=1 Sb=2 St=1")
}

float ColdCartImplBase::lnaPol1Sb2St1DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St1DrainCurrent, "LNA_DRAIN_CURRENT Po=1 Sb=2 St=1")
}

float ColdCartImplBase::lnaPol1Sb2St1GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St1GateVoltage, "LNA_GATE_VOLTAGE Po=1 Sb=2 St=1")
}

float ColdCartImplBase::lnaPol1Sb2St2DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St2DrainVoltage, "LNA_DRAIN_VOLTAGE Po=1 Sb=2 St=2")
}

float ColdCartImplBase::lnaPol1Sb2St2DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St2DrainCurrent, "LNA_DRAIN_CURRENT Po=1 Sb=2 St=2")
}

float ColdCartImplBase::lnaPol1Sb2St2GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St2GateVoltage, "LNA_GATE_VOLTAGE Po=1 Sb=2 St=2")
}

float ColdCartImplBase::lnaPol1Sb2St3DrainVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St3DrainVoltage, "LNA_DRAIN_VOLTAGE Po=1 Sb=2 St=3")
}

float ColdCartImplBase::lnaPol1Sb2St3DrainCurrent() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St3DrainCurrent, "LNA_DRAIN_CURRENT Po=1 Sb=2 St=3")
}

float ColdCartImplBase::lnaPol1Sb2St3GateVoltage() {
    SYNCMON_LOG_FLOAT(lnaPol1Sb2St3GateVoltage, "LNA_GATE_VOLTAGE Po=1 Sb=2 St=3")
} 
   
bool ColdCartImplBase::lnaLedPol0Enable() {
    SYNCMON_LOG_BOOL(lnaLedPol0Enable, "LNA_LED_ENABLE Po=0")
}

bool ColdCartImplBase::lnaLedPol1Enable() {
    SYNCMON_LOG_BOOL(lnaLedPol1Enable, "LNA_LED_ENABLE Po=1")
}
    
float ColdCartImplBase::sisHeaterPol0Current() {
    SYNCMON_LOG_FLOAT(sisHeaterPol0Current, "SIS_HEATER_CURRENT Po=0")
}

float ColdCartImplBase::sisHeaterPol1Current() {
    SYNCMON_LOG_FLOAT(sisHeaterPol1Current, "SIS_HEATER_CURRENT Po=1")
}

float ColdCartImplBase::cartridgeTemperature0() {
    SYNCMON_LOG_FLOAT(cartridgeTemperature0, "CARTRIDGE_TEMP Te=0")
}

float ColdCartImplBase::cartridgeTemperature1() {
    SYNCMON_LOG_FLOAT(cartridgeTemperature1, "CARTRIDGE_TEMP Te=1")
}

float ColdCartImplBase::cartridgeTemperature2() {
    SYNCMON_LOG_FLOAT(cartridgeTemperature2, "CARTRIDGE_TEMP Te=2")
}

float ColdCartImplBase::cartridgeTemperature3() {
    SYNCMON_LOG_FLOAT(cartridgeTemperature3, "CARTRIDGE_TEMP Te=3")
}

float ColdCartImplBase::cartridgeTemperature4() {
    SYNCMON_LOG_FLOAT(cartridgeTemperature4, "CARTRIDGE_TEMP Te=4")
}

float ColdCartImplBase::cartridgeTemperature5() {
    SYNCMON_LOG_FLOAT(cartridgeTemperature5, "CARTRIDGE_TEMP Te=5")
}

// control points:
void ColdCartImplBase::sisPol0Sb1Voltage(float val) {
    SYNCCMD_LOG_FLOAT(sisPol0Sb1Voltage, val, "SIS_VOLTAGE Po=0 Sb=1")
}

void ColdCartImplBase::sisPol0Sb1OpenLoop(bool val) {
    SYNCCMD_LOG_BOOL(sisPol0Sb1OpenLoop, val, "SIS_OPEN_LOOP Po=0 Sb=1")
}

void ColdCartImplBase::sisPol0Sb2Voltage(float val) {
    SYNCCMD_LOG_FLOAT(sisPol0Sb2Voltage, val, "SIS_VOLTAGE Po=0 Sb=2")
}

void ColdCartImplBase::sisPol0Sb2OpenLoop(bool val) {
    SYNCCMD_LOG_BOOL(sisPol0Sb2OpenLoop, val, "SIS_OPEN_LOOP Po=0 Sb=2")
}

void ColdCartImplBase::sisPol1Sb1Voltage(float val) {
    SYNCCMD_LOG_FLOAT(sisPol1Sb1Voltage, val, "SIS_VOLTAGE Po=1 Sb=1")
}

void ColdCartImplBase::sisPol1Sb1OpenLoop(bool val) {
    SYNCCMD_LOG_BOOL(sisPol1Sb1OpenLoop, val, "SIS_OPEN_LOOP Po=1 Sb=1")
}

void ColdCartImplBase::sisPol1Sb2Voltage(float val) {
    SYNCCMD_LOG_FLOAT(sisPol1Sb2Voltage, val, "SIS_VOLTAGE Po=1 Sb=2")
}

void ColdCartImplBase::sisPol1Sb2OpenLoop(bool val) {
    SYNCCMD_LOG_BOOL(sisPol1Sb2OpenLoop, val, "SIS_OPEN_LOOP Po=1 Sb=2")
}
    
void ColdCartImplBase::sisMagnetPol0Sb1Current(float val) {
    SYNCCMD_LOG_FLOAT(sisMagnetPol0Sb1Current, val, "SIS_MAGNET_CURRENT Po=0 Sb=1")
}

void ColdCartImplBase::sisMagnetPol0Sb2Current(float val) {
    SYNCCMD_LOG_FLOAT(sisMagnetPol0Sb2Current, val, "SIS_MAGNET_CURRENT Po=0 Sb=2")
}

void ColdCartImplBase::sisMagnetPol1Sb1Current(float val) {
    SYNCCMD_LOG_FLOAT(sisMagnetPol1Sb1Current, val, "SIS_MAGNET_CURRENT Po=1 Sb=1")
}

void ColdCartImplBase::sisMagnetPol1Sb2Current(float val) {
    SYNCCMD_LOG_FLOAT(sisMagnetPol1Sb2Current, val, "SIS_MAGNET_CURRENT Po=1 Sb=2")
}

void ColdCartImplBase::lnaPol0Sb1Enable(bool val) {
    SYNCCMD_LOG_BOOL(lnaPol0Sb1Enable, val, "LNA_ENABLE Po=0 Sb=1")
}

void ColdCartImplBase::lnaPol0Sb1St1DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb1St1DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=0 Sb=1 St=1")
}

void ColdCartImplBase::lnaPol0Sb1St1DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb1St1DrainCurrent, val, "LNA_DRAIN_CURRENT Po=0 Sb=1 St=1")
}

void ColdCartImplBase::lnaPol0Sb1St2DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb1St2DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=0 Sb=1 St=2")
}

void ColdCartImplBase::lnaPol0Sb1St2DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb1St2DrainCurrent, val, "LNA_DRAIN_CURRENT Po=0 Sb=1 St=2")
}

void ColdCartImplBase::lnaPol0Sb1St3DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb1St3DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=0 Sb=1 St=3")
}

void ColdCartImplBase::lnaPol0Sb1St3DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb1St3DrainCurrent, val, "LNA_DRAIN_CURRENT Po=0 Sb=1 St=3")
}

void ColdCartImplBase::lnaPol0Sb2Enable(bool val) {
    SYNCCMD_LOG_BOOL(lnaPol0Sb2Enable, val, "LNA_ENABLE Po=0 Sb=2")
}

void ColdCartImplBase::lnaPol0Sb2St1DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb2St1DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=0 Sb=2 St=1")
}

void ColdCartImplBase::lnaPol0Sb2St1DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb2St1DrainCurrent, val, "LNA_DRAIN_CURRENT Po=0 Sb=2 St=1")
}

void ColdCartImplBase::lnaPol0Sb2St2DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb2St2DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=0 Sb=2 St=2")
}

void ColdCartImplBase::lnaPol0Sb2St2DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb2St2DrainCurrent, val, "LNA_DRAIN_CURRENT Po=0 Sb=2 St=2")
}

void ColdCartImplBase::lnaPol0Sb2St3DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb2St3DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=0 Sb=2 St=3")
}

void ColdCartImplBase::lnaPol0Sb2St3DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol0Sb2St3DrainCurrent, val, "LNA_DRAIN_CURRENT Po=0 Sb=2 St=3")
}

void ColdCartImplBase::lnaPol1Sb1Enable(bool val) {
    SYNCCMD_LOG_BOOL(lnaPol1Sb1Enable, val, "LNA_ENABLE Po=1 Sb=1")
}

void ColdCartImplBase::lnaPol1Sb1St1DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb1St1DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=1 Sb=1 St=1")
}

void ColdCartImplBase::lnaPol1Sb1St1DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb1St1DrainCurrent, val, "LNA_DRAIN_CURRENT Po=1 Sb=1 St=1")
}

void ColdCartImplBase::lnaPol1Sb1St2DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb1St2DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=1 Sb=1 St=2")
}

void ColdCartImplBase::lnaPol1Sb1St2DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb1St2DrainCurrent, val, "LNA_DRAIN_CURRENT Po=1 Sb=1 St=2")
}

void ColdCartImplBase::lnaPol1Sb1St3DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb1St3DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=1 Sb=1 St=3")
}

void ColdCartImplBase::lnaPol1Sb1St3DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb1St3DrainCurrent, val, "LNA_DRAIN_CURRENT Po=1 Sb=1 St=3")
}

void ColdCartImplBase::lnaPol1Sb2Enable(bool val) {
    SYNCCMD_LOG_BOOL(lnaPol1Sb2Enable, val, "LNA_ENABLE Po=1 Sb=2")
}

void ColdCartImplBase::lnaPol1Sb2St1DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb2St1DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=1 Sb=2 St=1")
}

void ColdCartImplBase::lnaPol1Sb2St1DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb2St1DrainCurrent, val, "LNA_DRAIN_CURRENT Po=1 Sb=2 St=1")
}

void ColdCartImplBase::lnaPol1Sb2St2DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb2St2DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=1 Sb=2 St=2")
}

void ColdCartImplBase::lnaPol1Sb2St2DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb2St2DrainCurrent, val, "LNA_DRAIN_CURRENT Po=1 Sb=2 St=2")
}

void ColdCartImplBase::lnaPol1Sb2St3DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb2St3DrainVoltage, val, "LNA_DRAIN_VOLTAGE Po=1 Sb=2 St=3")
}

void ColdCartImplBase::lnaPol1Sb2St3DrainCurrent(float val) {
    SYNCCMD_LOG_FLOAT(lnaPol1Sb2St3DrainCurrent, val, "LNA_DRAIN_CURRENT Po=1 Sb=2 St=3")
}

void ColdCartImplBase::lnaLedPol0Enable(bool val) {
    SYNCCMD_LOG_BOOL(lnaLedPol0Enable, val, "LNA_LED_ENABLE Po=0")
}

void ColdCartImplBase::lnaLedPol1Enable(bool val) {
    SYNCCMD_LOG_BOOL(lnaLedPol1Enable, val, "LNA_LED_ENABLE Po=1")
}

void ColdCartImplBase::sisHeaterPol0Enable(bool val) {
    SYNCCMD_LOG_BOOL(sisHeaterPol0Enable, val, "SIS_HEATER_ENABLE Po=0")
}

void ColdCartImplBase::sisHeaterPol1Enable(bool val) {
    SYNCCMD_LOG_BOOL(sisHeaterPol1Enable, val, "SIS_HEATER_ENABLE Po=1")
}

void ColdCartImplBase::monitorAction(Time *timestamp_p) {
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 15 * milliseconds;   

    bool doMonitor = false;

    if (lastMonitorTime == 0 && logMonitors_m)
        logMon(true);
    
    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }
    
    if (doMonitor) {
        switch (monitorPhase) {
            case 0:
                if (!executeNextMon()) {
                    if (randomizeAnalogMonitors_m)
                        randomizeMon();
                    monitorPhase = 1;
                }
                break;
            case 1:
                sisPol0Sb1Current_value = avgSisPol0Sb1Current(20);
                ++monitorPhase;
                break;
                
            case 2:
                sisPol0Sb2Current_value = avgSisPol0Sb2Current(20);
                ++monitorPhase;
                break;

            case 3:
                sisPol1Sb1Current_value = avgSisPol1Sb1Current(20);
                ++monitorPhase;
                break;

            case 4:
                sisPol1Sb2Current_value = avgSisPol1Sb2Current(20);
                ++monitorPhase;
                break;
            
            case 5:
                sisPol0Sb1OpenLoop_value = sisPol0Sb1OpenLoop();
                ++monitorPhase;
                break;
            case 6:
                sisPol0Sb2OpenLoop_value = sisPol0Sb2OpenLoop();
                ++monitorPhase;
                break;
            case 7:
                sisPol1Sb1OpenLoop_value = sisPol1Sb1OpenLoop();
                ++monitorPhase;
                break;
            case 8:
                sisPol1Sb2OpenLoop_value = sisPol1Sb2OpenLoop();
                ++monitorPhase;
                break;
            case 9:
                lnaPol0Sb1Enable_value = lnaPol0Sb1Enable();
                ++monitorPhase;
                break;
            case 10:
                lnaPol0Sb2Enable_value = lnaPol0Sb2Enable();
                ++monitorPhase;
                break;
            case 11:
                lnaPol1Sb1Enable_value = lnaPol1Sb1Enable();
                ++monitorPhase;
                break;
            case 12:
                lnaPol1Sb2Enable_value = lnaPol1Sb2Enable();
                ++monitorPhase;
                break;
            case 13:
                lnaLedPol0Enable_value = lnaLedPol0Enable();
                ++monitorPhase;
                break;
            case 14:
                lnaLedPol1Enable_value = lnaLedPol1Enable();
                ++monitorPhase;
                // no break;
            default:
                if (logMonitors_m)
                    logMon();
                monitorPhase = 0;
                break;
        }
    }
}

DEFINE_MONITORS_REGISTRY(ColdCartImplBase)

void ColdCartImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:ColdCart(" << port_m << "): randomized,"
                         "IsisPol0Sb1,IsisPol0Sb2,IsisPol1Sb1,IsisPol1Sb2,"
                         "VsisPol0Sb1,VsisPol0Sb2,VsisPol1Sb1,VsisPol1Sb2,"
                         "ImagnetPol0Sb1,ImagnetPol0Sb2,ImagnetPol1Sb1,ImagnetPol1Sb2,"
                         "VmagnetPol0Sb1,VmagnetPol0Sb2,VmagnetPol1Sb1,VmagnetPol1Sb2,"
                         "VdrainLnaPol0Sb1St1,IdrainLnaPol0Sb1St1,VgateLnaPol0Sb1St1,"
                         "VdrainLnaPol0Sb1St2,IdrainLnaPol0Sb1St2,VgateLnaPol0Sb1St2,"
                         "VdrainLnaPol0Sb1St3,IdrainLnaPol0Sb1St3,VgateLnaPol0Sb1St3,"
                         "VdrainLnaPol0Sb2St1,IdrainLnaPol0Sb2St1,VgateLnaPol0Sb2St1,"
                         "VdrainLnaPol0Sb2St2,IdrainLnaPol0Sb2St2,VgateLnaPol0Sb2St2,"
                         "VdrainLnaPol0Sb2St3,IdrainLnaPol0Sb2St3,VgateLnaPol0Sb2St3,"
                         "VdrainLnaPol1Sb1St1,IdrainLnaPol1Sb1St1,VgateLnaPol1Sb1St1,"
                         "VdrainLnaPol1Sb1St2,IdrainLnaPol1Sb1St2,VgateLnaPol1Sb1St2,"
                         "VdrainLnaPol1Sb1St3,IdrainLnaPol1Sb1St3,VgateLnaPol1Sb1St3,"
                         "VdrainLnaPol1Sb2St1,IdrainLnaPol1Sb2St1,VgateLnaPol1Sb2St1,"        
                         "VdrainLnaPol1Sb2St2,IdrainLnaPol1Sb2St2,VgateLnaPol1Sb2St2,"
                         "VdrainLnaPol1Sb2St3,IdrainLnaPol1Sb2St3,VgateLnaPol1Sb2St3,"
                         "IheaterPol0,IheaterPol1,cartTemp0,cartTemp1,"
                         "cartTemp2,cartTemp3,cartTemp4,cartTemp5,"
                         "sisPol0Sb1OpenLoop,sisPol0Sb2OpenLoop,sisPol1Sb1OpenLoop,sisPol1Sb2OpenLoop,"
                         "lnaPol0Sb1Enable,lnaPol0Sb2Enable,lnaPol1Sb1Enable,lnaPol1Sb2Enable,"
                         "lnaLedPol0Enable,lnaLedPol1Enable" << endl;
        
    } else {
        LOG(LM_INFO)  << "AllMonitors:ColdCart(" << port_m << "): " << randomizeAnalogMonitors_m << ","
                      << sisPol0Sb1Current_value << "," << sisPol0Sb2Current_value << "," << sisPol1Sb1Current_value << "," << sisPol1Sb2Current_value << ","
                      << sisPol0Sb1Voltage_value << "," << sisPol0Sb2Voltage_value << "," << sisPol1Sb1Voltage_value << "," << sisPol1Sb2Voltage_value << ","
                      << sisMagnetPol0Sb1Current_value << "," << sisMagnetPol0Sb2Current_value << "," << sisMagnetPol1Sb1Current_value << "," << sisMagnetPol1Sb2Current_value << ","
                      << sisMagnetPol0Sb1Voltage_value << "," << sisMagnetPol0Sb2Voltage_value << "," << sisMagnetPol1Sb1Voltage_value << "," << sisMagnetPol1Sb2Voltage_value << ","
                      << lnaPol0Sb1St1DrainVoltage_value << "," << lnaPol0Sb1St1DrainCurrent_value << "," << lnaPol0Sb1St1GateVoltage_value << ","
                      << lnaPol0Sb1St2DrainVoltage_value << "," << lnaPol0Sb1St2DrainCurrent_value << "," << lnaPol0Sb1St2GateVoltage_value << ","
                      << lnaPol0Sb1St3DrainVoltage_value << "," << lnaPol0Sb1St3DrainCurrent_value << "," << lnaPol0Sb1St3GateVoltage_value << ","
                      << lnaPol0Sb2St1DrainVoltage_value << "," << lnaPol0Sb2St1DrainCurrent_value << "," << lnaPol0Sb2St1GateVoltage_value << ","
                      << lnaPol0Sb2St2DrainVoltage_value << "," << lnaPol0Sb2St2DrainCurrent_value << "," << lnaPol0Sb2St2GateVoltage_value << ","
                      << lnaPol0Sb2St3DrainVoltage_value << "," << lnaPol0Sb2St3DrainCurrent_value << "," << lnaPol0Sb2St3GateVoltage_value << ","
                      << lnaPol1Sb1St1DrainVoltage_value << "," << lnaPol1Sb1St1DrainCurrent_value << "," << lnaPol1Sb1St1GateVoltage_value << ","
                      << lnaPol1Sb1St2DrainVoltage_value << "," << lnaPol1Sb1St2DrainCurrent_value << "," << lnaPol1Sb1St2GateVoltage_value << ","
                      << lnaPol1Sb1St3DrainVoltage_value << "," << lnaPol1Sb1St3DrainCurrent_value << "," << lnaPol1Sb1St3GateVoltage_value << ","
                      << lnaPol1Sb2St1DrainVoltage_value << "," << lnaPol1Sb2St1DrainCurrent_value << "," << lnaPol1Sb2St1GateVoltage_value << ","
                      << lnaPol1Sb2St2DrainVoltage_value << "," << lnaPol1Sb2St2DrainCurrent_value << "," << lnaPol1Sb2St2GateVoltage_value << ","
                      << lnaPol1Sb2St3DrainVoltage_value << "," << lnaPol1Sb2St3DrainCurrent_value << "," << lnaPol1Sb2St3GateVoltage_value << ","
                      << sisHeaterPol0Current_value << "," << sisHeaterPol1Current_value << "," << cartridgeTemperature0_value << "," << cartridgeTemperature1_value << ","
                      << cartridgeTemperature2_value << "," << cartridgeTemperature3_value << "," << cartridgeTemperature4_value << "," << cartridgeTemperature5_value << ","
                      << (sisPol0Sb1OpenLoop_value ? 1 : 0) << "," << (sisPol0Sb2OpenLoop_value ? 1 : 0) << "," << (sisPol1Sb1OpenLoop_value ? 1 : 0) << "," << (sisPol1Sb2OpenLoop_value ? 1 : 0) << ","
                      << (lnaPol0Sb1Enable_value ? 1 : 0) << "," << (lnaPol0Sb2Enable_value ? 1 : 0) << "," << (lnaPol1Sb1Enable_value ? 1 : 0) << "," << (lnaPol1Sb2Enable_value ? 1 : 0) << ","
                      << (lnaLedPol0Enable_value ? 1 : 0) << "," << (lnaLedPol1Enable_value ? 1 : 0) << endl;
    }
}


