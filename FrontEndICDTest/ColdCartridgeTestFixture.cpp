/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2010
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

#include "ColdCartridgeTestFixture.h"
#include <string>
#include <cmath>
#include <logger.h>
using namespace std;

ColdCartridgeTestFixture::ColdCartridgeTestFixture(int band,
                                                   const std::string &name,
                                                   AmbChannel channel,
                                                   AmbNodeAddr nodeAddress,
                                                   bool expectStatusByte)
  : CartridgeTestFixture(band, name, channel, nodeAddress, expectStatusByte)
{
    sisSenseResistor_RCA            = cartBaseRCA_m + SIS_SENSE_RESISTOR;
    sisSenseResistor2_RCA           = cartBaseRCA_m + SIS_SENSE_RESISTOR + sb2;
    sisPol0Sb1Voltage_RCA           = cartBaseRCA_m + SIS_VOLTAGE;
    sisPol0Sb1Current_RCA           = cartBaseRCA_m + SIS_CURRENT;
    sisPol0Sb1OpenLoop_RCA          = cartBaseRCA_m + SIS_OPEN_LOOP;
    sisPol0Sb2Voltage_RCA           = cartBaseRCA_m + sb2 + SIS_VOLTAGE;
    sisPol0Sb2Current_RCA           = cartBaseRCA_m + sb2 + SIS_CURRENT;
    sisPol0Sb2OpenLoop_RCA          = cartBaseRCA_m + sb2 + SIS_OPEN_LOOP;
    sisPol1Sb1Voltage_RCA           = cartBaseRCA_m + pol1 + SIS_VOLTAGE;
    sisPol1Sb1Current_RCA           = cartBaseRCA_m + pol1 + SIS_CURRENT;
    sisPol1Sb1OpenLoop_RCA          = cartBaseRCA_m + pol1 + SIS_OPEN_LOOP;
    sisPol1Sb2Voltage_RCA           = cartBaseRCA_m + pol1 + sb2 + SIS_VOLTAGE;
    sisPol1Sb2Current_RCA           = cartBaseRCA_m + pol1 + sb2 + SIS_CURRENT;
    sisPol1Sb2OpenLoop_RCA          = cartBaseRCA_m + pol1 + sb2 + SIS_OPEN_LOOP;
    sisMagnetPol0Sb1Voltage_RCA     = cartBaseRCA_m + SIS_MAGNET_VOLTAGE;
    sisMagnetPol0Sb1Current_RCA     = cartBaseRCA_m + SIS_MAGNET_CURRENT;
    sisMagnetPol0Sb2Voltage_RCA     = cartBaseRCA_m + sb2 + SIS_MAGNET_VOLTAGE;
    sisMagnetPol0Sb2Current_RCA     = cartBaseRCA_m + sb2 + SIS_MAGNET_CURRENT;
    sisMagnetPol1Sb1Voltage_RCA     = cartBaseRCA_m + pol1 + SIS_MAGNET_VOLTAGE;
    sisMagnetPol1Sb1Current_RCA     = cartBaseRCA_m + pol1 + SIS_MAGNET_CURRENT;
    sisMagnetPol1Sb2Voltage_RCA     = cartBaseRCA_m + pol1 + sb2 + SIS_MAGNET_VOLTAGE;
    sisMagnetPol1Sb2Current_RCA     = cartBaseRCA_m + pol1 + sb2 + SIS_MAGNET_CURRENT;

    lnaPol0Sb1Enable_RCA            = cartBaseRCA_m + LNA_ENABLE;
    lnaPol0Sb1St1DrainVoltage_RCA   = cartBaseRCA_m + LNA1_DRAIN_VOLTAGE;
    lnaPol0Sb1St1DrainCurrent_RCA   = cartBaseRCA_m + LNA1_DRAIN_CURRENT;
    lnaPol0Sb1St1GateVoltage_RCA    = cartBaseRCA_m + LNA1_GATE_VOLTAGE;
    lnaPol0Sb1St2DrainVoltage_RCA   = cartBaseRCA_m + LNA2_DRAIN_VOLTAGE;
    lnaPol0Sb1St2DrainCurrent_RCA   = cartBaseRCA_m + LNA2_DRAIN_CURRENT;
    lnaPol0Sb1St2GateVoltage_RCA    = cartBaseRCA_m + LNA2_GATE_VOLTAGE;
    lnaPol0Sb1St3DrainVoltage_RCA   = cartBaseRCA_m + LNA3_DRAIN_VOLTAGE;
    lnaPol0Sb1St3DrainCurrent_RCA   = cartBaseRCA_m + LNA3_DRAIN_CURRENT;
    lnaPol0Sb1St3GateVoltage_RCA    = cartBaseRCA_m + LNA3_GATE_VOLTAGE;
    lnaPol0Sb1St4DrainVoltage_RCA   = cartBaseRCA_m + LNA4_DRAIN_VOLTAGE;
    lnaPol0Sb1St4DrainCurrent_RCA   = cartBaseRCA_m + LNA4_DRAIN_CURRENT;
    lnaPol0Sb1St4GateVoltage_RCA    = cartBaseRCA_m + LNA4_GATE_VOLTAGE;
    lnaPol0Sb1St5DrainVoltage_RCA   = cartBaseRCA_m + LNA5_DRAIN_VOLTAGE;
    lnaPol0Sb1St5DrainCurrent_RCA   = cartBaseRCA_m + LNA5_DRAIN_CURRENT;
    lnaPol0Sb1St5GateVoltage_RCA    = cartBaseRCA_m + LNA5_GATE_VOLTAGE;
    lnaPol0Sb1St6DrainVoltage_RCA   = cartBaseRCA_m + LNA6_DRAIN_VOLTAGE;
    lnaPol0Sb1St6DrainCurrent_RCA   = cartBaseRCA_m + LNA6_DRAIN_CURRENT;
    lnaPol0Sb1St6GateVoltage_RCA    = cartBaseRCA_m + LNA6_GATE_VOLTAGE;

    lnaPol0Sb2Enable_RCA            = cartBaseRCA_m + sb2 + LNA_ENABLE;
    lnaPol0Sb2St1DrainVoltage_RCA   = cartBaseRCA_m + sb2 + LNA1_DRAIN_VOLTAGE;
    lnaPol0Sb2St1DrainCurrent_RCA   = cartBaseRCA_m + sb2 + LNA1_DRAIN_CURRENT;
    lnaPol0Sb2St1GateVoltage_RCA    = cartBaseRCA_m + sb2 + LNA1_GATE_VOLTAGE;
    lnaPol0Sb2St2DrainVoltage_RCA   = cartBaseRCA_m + sb2 + LNA2_DRAIN_VOLTAGE;
    lnaPol0Sb2St2DrainCurrent_RCA   = cartBaseRCA_m + sb2 + LNA2_DRAIN_CURRENT;
    lnaPol0Sb2St2GateVoltage_RCA    = cartBaseRCA_m + sb2 + LNA2_GATE_VOLTAGE;
    lnaPol0Sb2St3DrainVoltage_RCA   = cartBaseRCA_m + sb2 + LNA3_DRAIN_VOLTAGE;
    lnaPol0Sb2St3DrainCurrent_RCA   = cartBaseRCA_m + sb2 + LNA3_DRAIN_CURRENT;
    lnaPol0Sb2St3GateVoltage_RCA    = cartBaseRCA_m + sb2 + LNA3_GATE_VOLTAGE;

    lnaPol1Sb1Enable_RCA            = cartBaseRCA_m + pol1 + LNA_ENABLE;
    lnaPol1Sb1St1DrainVoltage_RCA   = cartBaseRCA_m + pol1 + LNA1_DRAIN_VOLTAGE;
    lnaPol1Sb1St1DrainCurrent_RCA   = cartBaseRCA_m + pol1 + LNA1_DRAIN_CURRENT;
    lnaPol1Sb1St1GateVoltage_RCA    = cartBaseRCA_m + pol1 + LNA1_GATE_VOLTAGE;
    lnaPol1Sb1St2DrainVoltage_RCA   = cartBaseRCA_m + pol1 + LNA2_DRAIN_VOLTAGE;
    lnaPol1Sb1St2DrainCurrent_RCA   = cartBaseRCA_m + pol1 + LNA2_DRAIN_CURRENT;
    lnaPol1Sb1St2GateVoltage_RCA    = cartBaseRCA_m + pol1 + LNA2_GATE_VOLTAGE;
    lnaPol1Sb1St3DrainVoltage_RCA   = cartBaseRCA_m + pol1 + LNA3_DRAIN_VOLTAGE;
    lnaPol1Sb1St3DrainCurrent_RCA   = cartBaseRCA_m + pol1 + LNA3_DRAIN_CURRENT;
    lnaPol1Sb1St3GateVoltage_RCA    = cartBaseRCA_m + pol1 + LNA3_GATE_VOLTAGE;
    lnaPol1Sb1St4DrainVoltage_RCA   = cartBaseRCA_m + pol1 + LNA4_DRAIN_VOLTAGE;
    lnaPol1Sb1St4DrainCurrent_RCA   = cartBaseRCA_m + pol1 + LNA4_DRAIN_CURRENT;
    lnaPol1Sb1St4GateVoltage_RCA    = cartBaseRCA_m + pol1 + LNA4_GATE_VOLTAGE;
    lnaPol1Sb1St5DrainVoltage_RCA   = cartBaseRCA_m + pol1 + LNA4_DRAIN_VOLTAGE;
    lnaPol1Sb1St5DrainCurrent_RCA   = cartBaseRCA_m + pol1 + LNA4_DRAIN_CURRENT;
    lnaPol1Sb1St5GateVoltage_RCA    = cartBaseRCA_m + pol1 + LNA4_GATE_VOLTAGE;
    lnaPol1Sb1St6DrainVoltage_RCA   = cartBaseRCA_m + pol1 + LNA6_DRAIN_VOLTAGE;
    lnaPol1Sb1St6DrainCurrent_RCA   = cartBaseRCA_m + pol1 + LNA6_DRAIN_CURRENT;
    lnaPol1Sb1St6GateVoltage_RCA    = cartBaseRCA_m + pol1 + LNA6_GATE_VOLTAGE;

    lnaPol1Sb2Enable_RCA            = cartBaseRCA_m + pol1 + sb2 + LNA_ENABLE;
    lnaPol1Sb2St1DrainVoltage_RCA   = cartBaseRCA_m + pol1 + sb2 + LNA1_DRAIN_VOLTAGE;
    lnaPol1Sb2St1DrainCurrent_RCA   = cartBaseRCA_m + pol1 + sb2 + LNA1_DRAIN_CURRENT;
    lnaPol1Sb2St1GateVoltage_RCA    = cartBaseRCA_m + pol1 + sb2 + LNA1_GATE_VOLTAGE;
    lnaPol1Sb2St2DrainVoltage_RCA   = cartBaseRCA_m + pol1 + sb2 + LNA2_DRAIN_VOLTAGE;
    lnaPol1Sb2St2DrainCurrent_RCA   = cartBaseRCA_m + pol1 + sb2 + LNA2_DRAIN_CURRENT;
    lnaPol1Sb2St2GateVoltage_RCA    = cartBaseRCA_m + pol1 + sb2 + LNA2_GATE_VOLTAGE;
    lnaPol1Sb2St3DrainVoltage_RCA   = cartBaseRCA_m + pol1 + sb2 + LNA3_DRAIN_VOLTAGE;
    lnaPol1Sb2St3DrainCurrent_RCA   = cartBaseRCA_m + pol1 + sb2 + LNA3_DRAIN_CURRENT;
    lnaPol1Sb2St3GateVoltage_RCA    = cartBaseRCA_m + pol1 + sb2 + LNA3_GATE_VOLTAGE;

    lnaLedPol0Enable_RCA            = cartBaseRCA_m + LNA_LED_ENABLE;
    lnaLedPol1Enable_RCA            = cartBaseRCA_m + pol1 + LNA_LED_ENABLE;
    sisHeaterPol0Enable_RCA         = cartBaseRCA_m + SIS_HEATER_ENABLE;
    sisHeaterPol0Current_RCA        = cartBaseRCA_m + SIS_HEATER_CURRENT;
    sisHeaterPol1Enable_RCA         = cartBaseRCA_m + pol1 + SIS_HEATER_ENABLE;
    sisHeaterPol1Current_RCA        = cartBaseRCA_m + pol1 + SIS_HEATER_CURRENT;

    cartridgeTemperature0_RCA       = cartBaseRCA_m + CARTRIDGE_TEMP;
    cartridgeTemperature1_RCA       = cartBaseRCA_m + CARTRIDGE_TEMP + 0x10;
    cartridgeTemperature2_RCA       = cartBaseRCA_m + CARTRIDGE_TEMP + 0x20;
    cartridgeTemperature3_RCA       = cartBaseRCA_m + CARTRIDGE_TEMP + 0x30;
    cartridgeTemperature4_RCA       = cartBaseRCA_m + CARTRIDGE_TEMP + 0x40;
    cartridgeTemperature5_RCA       = cartBaseRCA_m + CARTRIDGE_TEMP + 0x50;

    //Control RCAs
    ctrlsisSenseResistor_RCA            = controlRCA + sisSenseResistor_RCA;
    ctrlsisSenseResistor2_RCA           = controlRCA + sisSenseResistor2_RCA;
    ctrlsisPol0Sb1Voltage_RCA           = controlRCA + sisPol0Sb1Voltage_RCA;
    ctrlsisPol0Sb1OpenLoop_RCA          = controlRCA + sisPol0Sb1OpenLoop_RCA;
    ctrlsisPol0Sb2Voltage_RCA           = controlRCA + sisPol0Sb2Voltage_RCA;
    ctrlsisPol0Sb2OpenLoop_RCA          = controlRCA + sisPol0Sb2OpenLoop_RCA;
    ctrlsisPol1Sb1Voltage_RCA           = controlRCA + sisPol1Sb1Voltage_RCA;
    ctrlsisPol1Sb1OpenLoop_RCA          = controlRCA + sisPol1Sb1OpenLoop_RCA;
    ctrlsisPol1Sb2Voltage_RCA           = controlRCA + sisPol1Sb2Voltage_RCA;
    ctrlsisPol1Sb2OpenLoop_RCA          = controlRCA + sisPol1Sb2OpenLoop_RCA;
    ctrlsisMagnetPol0Sb1Current_RCA     = controlRCA + sisMagnetPol0Sb1Current_RCA;
    ctrlsisMagnetPol0Sb2Current_RCA     = controlRCA + sisMagnetPol0Sb2Current_RCA;
    ctrlsisMagnetPol1Sb1Current_RCA     = controlRCA + sisMagnetPol1Sb1Current_RCA;
    ctrlsisMagnetPol1Sb2Current_RCA     = controlRCA + sisMagnetPol1Sb2Current_RCA;

    ctrllnaPol0Sb1St1DrainVoltage_RCA   = controlRCA + lnaPol0Sb1St1DrainVoltage_RCA;
    ctrllnaPol0Sb1St1DrainCurrent_RCA   = controlRCA + lnaPol0Sb1St1DrainCurrent_RCA;
    ctrllnaPol0Sb1St2DrainVoltage_RCA   = controlRCA + lnaPol0Sb1St2DrainVoltage_RCA;
    ctrllnaPol0Sb1St2DrainCurrent_RCA   = controlRCA + lnaPol0Sb1St2DrainCurrent_RCA;
    ctrllnaPol0Sb1St3DrainVoltage_RCA   = controlRCA + lnaPol0Sb1St3DrainVoltage_RCA;
    ctrllnaPol0Sb1St3DrainCurrent_RCA   = controlRCA + lnaPol0Sb1St3DrainCurrent_RCA;
    ctrllnaPol0Sb1St4DrainVoltage_RCA   = controlRCA + lnaPol0Sb1St4DrainVoltage_RCA;
    ctrllnaPol0Sb1St4DrainCurrent_RCA   = controlRCA + lnaPol0Sb1St4DrainCurrent_RCA;
    ctrllnaPol0Sb1St5DrainVoltage_RCA   = controlRCA + lnaPol0Sb1St5DrainVoltage_RCA;
    ctrllnaPol0Sb1St5DrainCurrent_RCA   = controlRCA + lnaPol0Sb1St5DrainCurrent_RCA;
    ctrllnaPol0Sb1St6DrainVoltage_RCA   = controlRCA + lnaPol0Sb1St6DrainVoltage_RCA;
    ctrllnaPol0Sb1St6DrainCurrent_RCA   = controlRCA + lnaPol0Sb1St6DrainCurrent_RCA;

    ctrllnaPol0Sb2St1DrainVoltage_RCA   = controlRCA + lnaPol0Sb2St1DrainVoltage_RCA;
    ctrllnaPol0Sb2St1DrainCurrent_RCA   = controlRCA + lnaPol0Sb2St1DrainCurrent_RCA;
    ctrllnaPol0Sb2St2DrainVoltage_RCA   = controlRCA + lnaPol0Sb2St2DrainVoltage_RCA;
    ctrllnaPol0Sb2St2DrainCurrent_RCA   = controlRCA + lnaPol0Sb2St2DrainCurrent_RCA;
    ctrllnaPol0Sb2St3DrainVoltage_RCA   = controlRCA + lnaPol0Sb2St3DrainVoltage_RCA;
    ctrllnaPol0Sb2St3DrainCurrent_RCA   = controlRCA + lnaPol0Sb2St3DrainCurrent_RCA;

    ctrllnaPol1Sb1St1DrainVoltage_RCA   = controlRCA + lnaPol1Sb1St1DrainVoltage_RCA;
    ctrllnaPol1Sb1St1DrainCurrent_RCA   = controlRCA + lnaPol1Sb1St1DrainCurrent_RCA;
    ctrllnaPol1Sb1St2DrainVoltage_RCA   = controlRCA + lnaPol1Sb1St2DrainVoltage_RCA;
    ctrllnaPol1Sb1St2DrainCurrent_RCA   = controlRCA + lnaPol1Sb1St2DrainCurrent_RCA;
    ctrllnaPol1Sb1St3DrainVoltage_RCA   = controlRCA + lnaPol1Sb1St3DrainVoltage_RCA;
    ctrllnaPol1Sb1St3DrainCurrent_RCA   = controlRCA + lnaPol1Sb1St3DrainCurrent_RCA;
    ctrllnaPol1Sb1St4DrainVoltage_RCA   = controlRCA + lnaPol1Sb1St4DrainVoltage_RCA;
    ctrllnaPol1Sb1St4DrainCurrent_RCA   = controlRCA + lnaPol1Sb1St4DrainCurrent_RCA;
    ctrllnaPol1Sb1St5DrainVoltage_RCA   = controlRCA + lnaPol1Sb1St5DrainVoltage_RCA;
    ctrllnaPol1Sb1St5DrainCurrent_RCA   = controlRCA + lnaPol1Sb1St5DrainCurrent_RCA;
    ctrllnaPol1Sb1St6DrainVoltage_RCA   = controlRCA + lnaPol1Sb1St6DrainVoltage_RCA;
    ctrllnaPol1Sb1St6DrainCurrent_RCA   = controlRCA + lnaPol1Sb1St6DrainCurrent_RCA;

    ctrllnaPol1Sb2St1DrainVoltage_RCA   = controlRCA + lnaPol1Sb2St1DrainVoltage_RCA;
    ctrllnaPol1Sb2St1DrainCurrent_RCA   = controlRCA + lnaPol1Sb2St1DrainVoltage_RCA;
    ctrllnaPol1Sb2St2DrainVoltage_RCA   = controlRCA + lnaPol1Sb2St2DrainVoltage_RCA;
    ctrllnaPol1Sb2St2DrainCurrent_RCA   = controlRCA + lnaPol1Sb2St2DrainCurrent_RCA;
    ctrllnaPol1Sb2St3DrainVoltage_RCA   = controlRCA + lnaPol1Sb2St3DrainVoltage_RCA;
    ctrllnaPol1Sb2St3DrainCurrent_RCA   = controlRCA + lnaPol1Sb2St3DrainCurrent_RCA;

    ctrllnaPol0Sb1Enable_RCA            = controlRCA + lnaPol0Sb1Enable_RCA;
    ctrllnaPol0Sb2Enable_RCA            = controlRCA + lnaPol0Sb2Enable_RCA;
    ctrllnaPol1Sb1Enable_RCA            = controlRCA + lnaPol1Sb1Enable_RCA;
    ctrllnaPol1Sb2Enable_RCA            = controlRCA + lnaPol1Sb2Enable_RCA;

    ctrllnaLedPol0Enable_RCA            = controlRCA + lnaLedPol0Enable_RCA;
    ctrllnaLedPol1Enable_RCA            = controlRCA + lnaLedPol1Enable_RCA;
    ctrlsisHeaterPol0Enable_RCA         = controlRCA + sisHeaterPol0Enable_RCA;
    ctrlsisHeaterPol1Enable_RCA         = controlRCA + sisHeaterPol1Enable_RCA;
}

void ColdCartridgeTestFixture::setUp() {
    CartridgeTestFixture::setUp();
}

void ColdCartridgeTestFixture::tearDown() {
    resetAmbVars();
    data_m[0] = 0;
    dataLength_m = 1;
    // Set all open loops to off:
    commandImpl(ctrlsisPol0Sb1OpenLoop_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisPol0Sb2OpenLoop_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisPol1Sb1OpenLoop_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisPol1Sb2OpenLoop_RCA, "ColdCartridgeTestFixture::tearDown");
    // Set all heaters to off:
    commandImpl(ctrlsisHeaterPol0Enable_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisHeaterPol1Enable_RCA, "ColdCartridgeTestFixture::tearDown");
    // Set all LEDs to off:
    commandImpl(ctrllnaLedPol0Enable_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrllnaLedPol0Enable_RCA, "ColdCartridgeTestFixture::tearDown");

    packSGL(0.0);
    // Set all magnet currents to 0:
    commandImpl(ctrlsisMagnetPol0Sb1Current_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisMagnetPol0Sb2Current_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisMagnetPol1Sb1Current_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisMagnetPol1Sb2Current_RCA, "ColdCartridgeTestFixture::tearDown");
    // Set all SIS voltages to 0:
    commandImpl(ctrlsisPol0Sb1Voltage_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisPol0Sb2Voltage_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisPol1Sb1Voltage_RCA, "ColdCartridgeTestFixture::tearDown");
    commandImpl(ctrlsisPol1Sb2Voltage_RCA, "ColdCartridgeTestFixture::tearDown");

    CartridgeTestFixture::tearDown();
}

void ColdCartridgeTestFixture::ENABLE_LNA_POL0_SB1(){
    //Turn on LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::ENABLE_LNA_POL0_SB1()" << endl;
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrllnaPol0Sb1Enable_RCA, "ENABLE_LNA_POL0_SB1", NULL);
    SLEEP(200);
}
void ColdCartridgeTestFixture::DISABLE_LNA_POL0_SB1(){
    //Turn off LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::DISABLE_LNA_POL0_SB1()" << endl;
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(ctrllnaPol0Sb1Enable_RCA, "DISABLE_LNA_POL0_SB1", NULL);
}
void ColdCartridgeTestFixture::ENABLE_LNA_POL0_SB2(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    //Turn on LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::ENABLE_LNA_POL0_SB2()" << endl;
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrllnaPol0Sb2Enable_RCA, "ENABLE_LNA_POL0_SB2", NULL);
    SLEEP(200);
}
void ColdCartridgeTestFixture::DISABLE_LNA_POL0_SB2(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    //Turn off LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::DISABLE_LNA_POL0_SB2()" << endl;
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(ctrllnaPol0Sb2Enable_RCA, "DISABLE_LNA_POL0_SB2", NULL);
}
void ColdCartridgeTestFixture::ENABLE_LNA_POL1_SB1(){
    //Turn on LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::ENABLE_LNA_POL1_SB1()" << endl;
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrllnaPol1Sb1Enable_RCA, "ENABLE_LNA_POL1_SB1", NULL);
    SLEEP(200);
}
void ColdCartridgeTestFixture::DISABLE_LNA_POL1_SB1(){
    //Turn off LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::DISABLE_LNA_POL1_SB1()" << endl;
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(ctrllnaPol1Sb1Enable_RCA, "DISABLE_LNA_POL1_SB1", NULL);
}
void ColdCartridgeTestFixture::ENABLE_LNA_POL1_SB2(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    //Turn on LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::ENABLE_LNA_POL1_SB2()" << endl;
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrllnaPol1Sb2Enable_RCA, "ENABLE_LNA_POL1_SB2", NULL);
    SLEEP(200);
}
void ColdCartridgeTestFixture::DISABLE_LNA_POL1_SB2(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    //Turn off LNA
    LOG(LM_INFO) << "ColdCartridgeTestFixture::DISABLE_LNA_POL1_SB2()" << endl;
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(ctrllnaPol1Sb2Enable_RCA, "DISABLE_LNA_POL1_SB2", NULL);
}

//////////////////////////////////////
// LNA Drain Voltage Pol 0 SB 1
/////////////////////////////////////
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_LNA_ST1_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb1St1DrainVoltage_RCA,ctrllnaPol0Sb1St1DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB1_LNA_ST1_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_LNA_ST2_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb1St2DrainVoltage_RCA,ctrllnaPol0Sb1St2DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB1_LNA_ST2_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_LNA_ST3_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb1St3DrainVoltage_RCA,ctrllnaPol0Sb1St3DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB1_LNA_ST3_DRAIN_VOLTAGE");
}


//////////////////////////////////////
// LNA Drain Voltage Pol 0 SB 2
//////////////////////////////////////
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_LNA_ST1_DRAIN_VOLTAGE() {
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb2St1DrainVoltage_RCA,ctrllnaPol0Sb2St1DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB2_LNA_ST1_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_LNA_ST2_DRAIN_VOLTAGE() {
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb2St2DrainVoltage_RCA,ctrllnaPol0Sb2St2DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB2_LNA_ST2_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_LNA_ST3_DRAIN_VOLTAGE() {
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb2St2DrainVoltage_RCA,ctrllnaPol0Sb2St2DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB2_LNA_ST2_DRAIN_VOLTAGE");
}

//////////////////////////////////////
// LNA Drain Voltage Pol 1 SB 1
//////////////////////////////////////
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_LNA_ST1_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb1St1DrainVoltage_RCA,ctrllnaPol1Sb1St1DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB1_LNA_ST1_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_LNA_ST2_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb1St2DrainVoltage_RCA,ctrllnaPol1Sb1St2DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB1_LNA_ST2_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_LNA_ST3_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb1St3DrainVoltage_RCA,ctrllnaPol1Sb1St3DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB1_LNA_ST3_DRAIN_VOLTAGE");
}
//////////////////////////////////////
// LNA Drain Voltage Pol 1 SB 2
//////////////////////////////////////
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_LNA_ST1_DRAIN_VOLTAGE() {
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb2St1DrainVoltage_RCA,ctrllnaPol1Sb2St1DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB2_LNA_ST1_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_LNA_ST2_DRAIN_VOLTAGE() {
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb2St2DrainVoltage_RCA,ctrllnaPol1Sb2St2DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB2_LNA_ST2_DRAIN_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_LNA_ST3_DRAIN_VOLTAGE() {
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb2St3DrainVoltage_RCA,ctrllnaPol1Sb2St3DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB2_LNA_ST3_DRAIN_VOLTAGE");
}

//////////////////////////////////////
// LNA Gate Voltage Pol 0 SB 1
//////////////////////////////////////

void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB1_LNA_ST1_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb1St1GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB1_LNA_ST1_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB1_LNA_ST2_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb1St2GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB1_LNA_ST2_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB1_LNA_ST3_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb1St3GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB1_LNA_ST3_GATE_VOLTAGE");
}


//////////////////////////////////////
// LNA Gate Voltage Pol 0 SB 2
//////////////////////////////////////

void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB2_LNA_ST1_GATE_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb2St1GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB2_LNA_ST1_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB2_LNA_ST2_GATE_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb2St2GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB2_LNA_ST2_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB2_LNA_ST3_GATE_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb2St3GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB2_LNA_ST3_GATE_VOLTAGE");
}

//////////////////////////////////////
// LNA Gate Voltage Pol 1 SB 1
//////////////////////////////////////
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB1_LNA_ST1_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb1St1GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB1_LNA_ST1_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB1_LNA_ST2_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb1St2GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB1_LNA_ST2_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB1_LNA_ST3_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb1St3GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB1_LNA_ST3_GATE_VOLTAGE");
}
//////////////////////////////////////
// LNA Gate Voltage Pol 1 SB 2
//////////////////////////////////////

void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB2_LNA_ST1_GATE_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb2St1GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB2_LNA_ST1_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB2_LNA_ST2_GATE_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb2St2GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB2_LNA_ST2_GATE_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB2_LNA_ST3_GATE_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb2St3GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB2_LNA_ST3_GATE_VOLTAGE");
}


//////////////////////////////////////
// LNA Enable
//////////////////////////////////////
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_LNA_ENABLE(){
    implToggleEnable(lnaPol0Sb1Enable_RCA ,ctrllnaPol0Sb1Enable_RCA,0,
                     "SET_CARTRIDGE_POL0_SB1_LNA_ENABLE_OFF");
    implToggleEnable(lnaPol0Sb1Enable_RCA ,ctrllnaPol0Sb1Enable_RCA,1,
                     "SET_CARTRIDGE_POL0_SB1_LNA_ENABLE_ON");
    implToggleEnable(lnaPol0Sb1Enable_RCA ,ctrllnaPol0Sb1Enable_RCA,2,
                     "SET_CARTRIDGE_POL0_SB1_LNA_ENABLE_ILLEGALVALUE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_LNA_ENABLE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    implToggleEnable(lnaPol0Sb2Enable_RCA ,ctrllnaPol0Sb2Enable_RCA,0,
                     "SET_CARTRIDGE_POL0_SB2_LNA_ENABLE_OFF");
    implToggleEnable(lnaPol0Sb2Enable_RCA ,ctrllnaPol0Sb2Enable_RCA,1,
                     "SET_CARTRIDGE_POL0_SB2_LNA_ENABLE_ON");
    implToggleEnable(lnaPol0Sb2Enable_RCA ,ctrllnaPol0Sb2Enable_RCA,2,
                     "SET_CARTRIDGE_POL0_SB2_LNA_ENABLE_ILLEGALVALUE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_LNA_ENABLE(){
    implToggleEnable(lnaPol1Sb1Enable_RCA ,ctrllnaPol1Sb1Enable_RCA,0,
                     "SET_CARTRIDGE_POL1_SB1_LNA_ENABLE_OFF");
    implToggleEnable(lnaPol1Sb1Enable_RCA ,ctrllnaPol1Sb1Enable_RCA,1,
                     "SET_CARTRIDGE_POL1_SB1_LNA_ENABLE_ON");
    implToggleEnable(lnaPol1Sb1Enable_RCA ,ctrllnaPol1Sb1Enable_RCA,2,
                     "SET_CARTRIDGE_POL1_SB1_LNA_ENABLE_ILLEGALVALUE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_LNA_ENABLE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    implToggleEnable(lnaPol1Sb2Enable_RCA ,ctrllnaPol1Sb2Enable_RCA,0,
                     "SET_CARTRIDGE_POL1_SB2_LNA_ENABLE_OFF");
    implToggleEnable(lnaPol1Sb2Enable_RCA ,ctrllnaPol1Sb2Enable_RCA,1,
                     "SET_CARTRIDGE_POL1_SB2_LNA_ENABLE_ON");
    implToggleEnable(lnaPol1Sb2Enable_RCA ,ctrllnaPol1Sb2Enable_RCA,2,
                     "SET_CARTRIDGE_POL1_SB2_LNA_ENABLE_ILLEGALVALUE");
}

//////////////////////////////////////
// LNA LED Enable
//////////////////////////////////////

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_LNA_LED_ENABLE(){
    if (band_m == 9 || band_m == 7)
        return; // not checking SB2 for band 9 and band 7
    implToggleEnable(lnaLedPol0Enable_RCA ,ctrllnaLedPol0Enable_RCA,0,
                     "SET_CARTRIDGE_POL0_LNA_LED_ENABLE_OFF");
    implToggleEnable(lnaLedPol0Enable_RCA ,ctrllnaLedPol0Enable_RCA,1,
                     "SET_CARTRIDGE_POL0_LNA_LED_ENABLE_ON");
    implToggleEnable(lnaLedPol0Enable_RCA ,ctrllnaLedPol0Enable_RCA,2,
                     "SET_CARTRIDGE_POL0_LNA_LED_ENABLE_ILLEGALVALUE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_LNA_LED_ENABLE(){
    if (band_m == 9 || band_m == 7)
        return; // not checking SB2 for band 9 and band 7
    implToggleEnable(lnaLedPol1Enable_RCA ,ctrllnaLedPol1Enable_RCA,0,
                     "SET_CARTRIDGE_POL1_LNA_LED_ENABLE_OFF");
    implToggleEnable(lnaLedPol1Enable_RCA ,ctrllnaLedPol1Enable_RCA,1,
                     "SET_CARTRIDGE_POL1_LNA_LED_ENABLE_ON");
    implToggleEnable(lnaLedPol1Enable_RCA ,ctrllnaLedPol1Enable_RCA,2,
                     "SET_CARTRIDGE_POL1_LNA_LED_ENABLE_ILLEGALVALUE");
}

/**********************************
 * SIS Monitor Functions
 **********************************/
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB1_SIS_CURRENT() {
    float min, max;
    getLimitsSISCurrent(min, max);
    implGetFloat(sisPol0Sb1Current_RCA, min, max,
                    "GET_CARTRIDGE_POL0_SB1_SIS_CURRENT");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB2_SIS_CURRENT(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsSISCurrent(min, max);
    implGetFloat(sisPol0Sb2Current_RCA, min, max,
                    "GET_CARTRIDGE_POL0_SB2_SIS_CURRENT");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB1_SIS_CURRENT(){
    float min, max;
    getLimitsSISCurrent(min, max);
    implGetFloat(sisPol1Sb1Current_RCA, min, max,
                    "GET_CARTRIDGE_POL1_SB1_SIS_CURRENT");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB2_SIS_CURRENT(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max;
    getLimitsSISCurrent(min, max);
    implGetFloat(sisPol1Sb2Current_RCA, min, max,
                    "GET_CARTRIDGE_POL1_SB2_SIS_CURRENT");
}

void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB1_SIS_MAGNET_VOLTAGE(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    float min, max;
    getLimitsMagnetVoltage(min, max);
    implGetFloat(sisMagnetPol0Sb1Voltage_RCA, min, max,
                    "GET_CARTRIDGE_POL0_SB1_SIS_MAGNET_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SB2_SIS_MAGNET_VOLTAGE(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    if (band_m == 6 || band_m == 9)
        return; // not checking SB2 for bands 6 & 9
    float min, max;
    getLimitsMagnetVoltage(min, max);
    implGetFloat(sisMagnetPol0Sb2Voltage_RCA, min, max,
                    "GET_CARTRIDGE_POL0_SB2_SIS_MAGNET_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB1_SIS_MAGNET_VOLTAGE(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    float min, max;
    getLimitsMagnetVoltage(min, max);
    implGetFloat(sisMagnetPol1Sb1Voltage_RCA, min, max,
                    "GET_CARTRIDGE_POL1_SB1_SIS_MAGNET_VOLTAGE");
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SB2_SIS_MAGNET_VOLTAGE(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    if (band_m == 6 || band_m == 9)
        return; // not checking SB2 for bands 6 & 9
    float min, max;
    getLimitsMagnetVoltage(min, max);
    implGetFloat(sisMagnetPol1Sb2Voltage_RCA, min, max,
                    "GET_CARTRIDGE_POL1_SB2_SIS_MAGNET_VOLTAGE");
}

void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL0_SIS_HEATER_CURRENT(){
    if (band_m == 3 || band_m == 4)
        return; // not checking heaters on bands 3 & 4
    float min, max;
    getLimitsHeaterCurrent(min, max);
    implGetFloat(sisHeaterPol0Current_RCA, min, max,
                    "GET_CARTRIDGE_POL0_SIS_HEATER_CURRENT");
}

void ColdCartridgeTestFixture::testGET_CARTRIDGE_POL1_SIS_HEATER_CURRENT(){
    if (band_m == 3 || band_m == 4)
        return; // not checking heaters on bands 3 & 4
    float min, max;
    getLimitsHeaterCurrent(min, max);
    implGetFloat(sisHeaterPol1Current_RCA, min, max,
                    "GET_CARTRIDGE_POL1_SIS_HEATER_CURRENT");
}
/**********************************
 * SIS Control Functions
 **********************************/
void ColdCartridgeTestFixture::testSET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR() {
    if (band_m < 3)
        return; // not checking current sense resistor on bands 1 & 2

    unsigned char statusByte;
    string info;

    // Read and back up the current resistor setting:
    monitor(sisSenseResistor_RCA, "GET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR", &info);
    // check datalength
    CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 5);
    float resistorBak(unpackSGL(&statusByte));

    // Read and back up the current resistor2 setting:
    monitor(sisSenseResistor2_RCA, "GET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR", &info);
    // check datalength
    CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 5);
    float resistor2Bak(unpackSGL(&statusByte));

    float min(0.000001), max(99999.9), tolerance(0.0);
    SET_LNA_SIS_VALUE(sisSenseResistor_RCA, ctrlsisSenseResistor_RCA,
                    getTestValuesSISSenseResistor(), min, max, tolerance,
                    "SET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR");
    SET_LNA_SIS_VALUE(sisSenseResistor2_RCA, ctrlsisSenseResistor2_RCA,
                    getTestValuesSISSenseResistor(), min, max, tolerance,
                    "SET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR");

    // Set the resistors back to the original values:
    packSGL(resistorBak);
    commandImpl(ctrlsisSenseResistor_RCA, "Restoring original resistor.\n", &info);
    packSGL(resistor2Bak);
    commandImpl(ctrlsisSenseResistor2_RCA, "Restoring original resistor.\n", &info);
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_SIS_VOLTAGE(){
    float min, max, tolerance;
    getLimitsSisVoltage(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisPol0Sb1Voltage_RCA, ctrlsisPol0Sb1Voltage_RCA,
                    getTestValuesSISVoltage(), min, max, tolerance,
                    "SET_CARTRIDGE_POL0_SB1_SIS_VOLTAGE");
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_SIS_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsSisVoltage(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisPol0Sb2Voltage_RCA, ctrlsisPol0Sb2Voltage_RCA,
                    getTestValuesSISVoltage(), min, max, tolerance,
                    "SET_CARTRIDGE_POL0_SB2_SIS_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_SIS_VOLTAGE(){
    float min, max, tolerance;
    getLimitsSisVoltage(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisPol1Sb1Voltage_RCA, ctrlsisPol1Sb1Voltage_RCA,
                    getTestValuesSISVoltage(), min, max, tolerance,
                    "SET_CARTRIDGE_POL1_SB1_SIS_VOLTAGE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_SIS_VOLTAGE(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    float min, max, tolerance;
    getLimitsSisVoltage(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisPol1Sb2Voltage_RCA, ctrlsisPol1Sb2Voltage_RCA,
                    getTestValuesSISVoltage(), min, max, tolerance,
                    "SET_CARTRIDGE_POL1_SB2_SIS_VOLTAGE");
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_SIS_MAGNET_CURRENT(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    float min, max, tolerance;
    getLimitsMagnetCurrent(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisMagnetPol0Sb1Current_RCA, ctrlsisMagnetPol0Sb1Current_RCA,
                    getTestValuesMagnetCurrent(), min, max, tolerance,
                    "SET_CARTRIDGE_POL0_SB1_SIS_MAGNET_CURRENT");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_SIS_MAGNET_CURRENT(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    if (band_m == 6 || band_m == 9)
        return; // not checking SB2 for bands 6 & 9
    float min, max, tolerance;
    getLimitsMagnetCurrent(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisMagnetPol0Sb2Current_RCA, ctrlsisMagnetPol0Sb2Current_RCA,
                    getTestValuesMagnetCurrent(), min, max, tolerance,
                    "SET_CARTRIDGE_POL0_SB2_SIS_MAGNET_CURRENT");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_SIS_MAGNET_CURRENT(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    float min, max, tolerance;
    getLimitsMagnetCurrent(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisMagnetPol1Sb1Current_RCA, ctrlsisMagnetPol1Sb1Current_RCA,
                    getTestValuesMagnetCurrent(), min, max, tolerance,
                    "SET_CARTRIDGE_POL1_SB1_SIS_MAGNET_CURRENT");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_SIS_MAGNET_CURRENT(){
    if (band_m == 3 || band_m == 4)
        return; // not checking magnets on bands 3 & 4
    if (band_m == 6 || band_m == 9)
        return; // not checking SB2 for bands 6 & 9
    float min, max, tolerance;
    getLimitsMagnetCurrent(min, max, tolerance);
    SET_LNA_SIS_VALUE(sisMagnetPol1Sb2Current_RCA, ctrlsisMagnetPol1Sb2Current_RCA,
                    getTestValuesMagnetCurrent(), min, max, tolerance,
                    "SET_CARTRIDGE_POL1_SB2_SIS_MAGNET_CURRENT");
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB1_SIS_OPEN_LOOP(){
    implToggleEnable(sisPol0Sb1OpenLoop_RCA ,ctrlsisPol0Sb1OpenLoop_RCA,0,
                     "SET_CARTRIDGE_POL0_SB1_SIS_CLOSE_LOOP");
    implToggleEnable(sisPol0Sb1OpenLoop_RCA ,ctrlsisPol0Sb1OpenLoop_RCA,1,
                     "SET_CARTRIDGE_POL0_SB1_SIS_OPEN_LOOP");
    implToggleEnable(sisPol0Sb1OpenLoop_RCA ,ctrlsisPol0Sb1OpenLoop_RCA,2,
                     "SET_CARTRIDGE_POL0_SB1_SIS_OPEN_LOOP_ILLEGALVALUE");
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SB2_SIS_OPEN_LOOP(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    implToggleEnable(sisPol0Sb2OpenLoop_RCA ,ctrlsisPol0Sb2OpenLoop_RCA,0,
                     "SET_CARTRIDGE_POL0_SB2_SIS_CLOSE_LOOP");
    implToggleEnable(sisPol0Sb2OpenLoop_RCA ,ctrlsisPol0Sb2OpenLoop_RCA,1,
                     "SET_CARTRIDGE_POL0_SB2_SIS_OPEN_LOOP");
    implToggleEnable(sisPol0Sb2OpenLoop_RCA ,ctrlsisPol0Sb2OpenLoop_RCA,2,
                     "SET_CARTRIDGE_POL0_SB2_SIS_OPEN_LOOP_ILLEGALVALUE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB1_SIS_OPEN_LOOP(){
    implToggleEnable(sisPol1Sb1OpenLoop_RCA ,ctrlsisPol1Sb1OpenLoop_RCA,0,
                     "SET_CARTRIDGE_POL1_SB1_SIS_CLOSE_LOOP");
    implToggleEnable(sisPol1Sb1OpenLoop_RCA ,ctrlsisPol1Sb1OpenLoop_RCA,1,
                     "SET_CARTRIDGE_POL1_SB1_SIS_OPEN_LOOP");
    implToggleEnable(sisPol1Sb1OpenLoop_RCA ,ctrlsisPol1Sb1OpenLoop_RCA,2,
                     "SET_CARTRIDGE_POL1_SB1_SIS_OPEN_LOOP_ILLEGALVALUE");
}
void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SB2_SIS_OPEN_LOOP(){
    if (band_m == 9)
        return; // not checking SB2 for band 9
    implToggleEnable(sisPol1Sb2OpenLoop_RCA ,ctrlsisPol1Sb2OpenLoop_RCA,0,
                     "SET_CARTRIDGE_POL1_SB2_SIS_CLOSE_LOOP");
    implToggleEnable(sisPol1Sb2OpenLoop_RCA ,ctrlsisPol1Sb2OpenLoop_RCA,1,
                     "SET_CARTRIDGE_POL1_SB2_SIS_OPEN_LOOP");
    implToggleEnable(sisPol1Sb2OpenLoop_RCA ,ctrlsisPol1Sb2OpenLoop_RCA,2,
                     "SET_CARTRIDGE_POL1_SB2_SIS_OPEN_LOOP_ILLEGALVALUE");
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL0_SIS_HEATER_ENABLE(){
    if (band_m == 3 || band_m == 4)
        return; // not checking heaters on bands 3 & 4
    implTestHeaters(ctrlsisHeaterPol0Enable_RCA, sisHeaterPol0Enable_RCA, sisHeaterPol0Current_RCA, "SET_CARTRIDGE_POL0_SIS_HEATER_ENABLE");
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_POL1_SIS_HEATER_ENABLE(){
    if (band_m == 3 || band_m == 4)
        return; // not checking heaters on bands 3 & 4
    implTestHeaters(ctrlsisHeaterPol1Enable_RCA, sisHeaterPol1Enable_RCA, sisHeaterPol1Current_RCA, "SET_CARTRIDGE_POL1_SIS_HEATER_ENABLE");
}

void ColdCartridgeTestFixture::testGET_CARTRIDGE_TEMP0_TEMP(){
    implGetFloat(cartridgeTemperature0_RCA,-1.0, 325.0,"GET_CARTRIDGE_TEMP1_TEMP", true);
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_TEMP1_TEMP(){
    implGetFloat(cartridgeTemperature1_RCA,-1.0, 325.0,"GET_CARTRIDGE_TEMP1_TEMP", true);
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_TEMP2_TEMP(){
    implGetFloat(cartridgeTemperature2_RCA,-1.0, 325.0,"GET_CARTRIDGE_TEMP2_TEMP", true);
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_TEMP3_TEMP(){
    implGetFloat(cartridgeTemperature3_RCA,-1.0, 325.0,"GET_CARTRIDGE_TEMP3_TEMP", true);
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_TEMP4_TEMP(){
    implGetFloat(cartridgeTemperature4_RCA,-1.0, 325.0,"GET_CARTRIDGE_TEMP4_TEMP", true);
}
void ColdCartridgeTestFixture::testGET_CARTRIDGE_TEMP5_TEMP(){
    implGetFloat(cartridgeTemperature5_RCA,-1.0, 325.0,"GET_CARTRIDGE_TEMP5_TEMP", true);
}

void ColdCartridgeTestFixture::testSET_CARTRIDGE_TEMP_OFFSET() {
    unsigned char statusByte;
    string info;

    // Loop through all cartridge temp sensors:
    for (AmbRelativeAddr sensorMon = cartridgeTemperature0_RCA; sensorMon <= cartridgeTemperature5_RCA; sensorMon += 0x10) {
        // We can read the current offset at this RCA:
        AmbRelativeAddr offsetMon = sensorMon + 0x08;
        // And this is the offset control RCA:
        AmbRelativeAddr offsetCtrl = offsetMon + controlRCA;

        // Read and back up the current offset:
        monitor(offsetMon, "SET_CARTRIDGE_TEMP_OFFSET", &info);
        // check datalength
        CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 5);
        float offsetBak(unpackSGL(&statusByte));

        // Set the offset to zero:
        packSGL(0.0);
        commandImpl(offsetCtrl, "initially setting offset to zero.\n", &info);

        // Read the temperature sensor:
        monitor(sensorMon, "GET_CARTRIDGE_TEMP", &info);
        float reading1(unpackSGL(&statusByte));

        // Set the offset to +1
        packSGL(1.0);
        commandImpl(offsetCtrl, "Setting offset to 1.0\n", &info);

        // Read the temperature sensor:
        monitor(sensorMon, "GET_CARTRIDGE_TEMP", &info);
        float reading2(unpackSGL(&statusByte));

        // Check that difference is about 1:
        bool inRange = ((reading2 - reading1) >= 0.8) && ((reading2 - reading1) <= 1.2);
        LOG(LM_INFO) << "ColdCartridgeTestFixture::testSET_CARTRIDGE_TEMP_OFFSET:" <<
                " sensorRCA=" << std::hex << sensorMon << std::dec << " reading1=" << reading1 << " reading2=" << reading2 << endl;
        // Only assert where temp sensor is installed (-1 means spare):
        if (reading1 >= 0)
            CPPUNIT_ASSERT_MESSAGE("Offset temperature out of range.", inRange);

        // Set the offset back to the original value:
        packSGL(offsetBak);
        commandImpl(offsetCtrl, "Restoring original offset.\n", &info);
    }
}

void ColdCartridgeTestFixture::SET_LNA_VALUE_Id(AmbRelativeAddr controlVd_RCA,
            AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
            const std::vector<float> &test_values, const float validmin, const float validmax,
            const float tolerance, const std::string &msg){
    string info;
    //Set Vd to 1
    packSGL(1);
    commandImpl(controlVd_RCA, "Setting Vd to 1.\n", &info);
    SLEEP(100);
    SET_LNA_SIS_VALUE(monitor_RCA,control_RCA,test_values, validmin, validmax,tolerance, msg);
}

void ColdCartridgeTestFixture::SET_LNA_SIS_VALUE(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
            const std::vector<float> &test_values, const float validmin, const float validmax,
            const float tolerance, const std::string &msg){

    string info,strSet,strReturn;
    ostringstream bufSet,bufReturn;
    bool InValidRange;
    unsigned int i;
    float OriginalMonitorValue;
    unsigned char statusByte;

    //LOG(LM_INFO) << "Number of elements in array=" << test_values.size() << endl;

    //Loop over test_values array
    for (i=0;i< test_values.size();i++){
        InValidRange = (test_values[i] >= validmin) && (test_values[i] <= validmax);
        resetAmbVars();

        //LOG(LM_INFO) << "test_values[i]=" << test_values[i] << endl;

        //Set 0 or min initially:
        if (validmin <= 0.0)
            packSGL(0);
        else
            packSGL(validmin);
        commandImpl(control_RCA, "initially setting to zero/validmin.\n", &info);
        SLEEP(100);
        monitor(monitor_RCA, strSet, &info);
        OriginalMonitorValue = unpackSGL();

        info= "";
        bufSet.str("");
        bufReturn.str("");

        //Append set value to the message
        bufSet <<  "\n" << msg << ", SetValue=" << test_values[i] << ",Range=[" << validmin << "," << validmax << "].";
        strSet = bufSet.str();

        //set control value
        packSGL(test_values[i]);
        command(control_RCA, strSet, &strSet, false);
        SLEEP(3000);

        //assert correct datalength
        CPPUNIT_ASSERT_MESSAGE(strSet, dataLength_m == 5);
        //get statusbyte
        unpackSGL(&statusByte);

        //If SetValue is illegal, cache the result for the final check at the end.
        if (InValidRange){
            //If SetValue is legal
            CPPUNIT_ASSERT_MESSAGE(strSet, statusByte == FEMC_NO_ERROR);
        }
        else{
            if (checkIllegalFloatCommands){
                //If SetValue is illegal
                CPPUNIT_ASSERT_MESSAGE(strSet + ": Error not raised for illegal set value.", statusByte != FEMC_NO_ERROR);
            }
        }
        //read monitor point

        monitor(monitor_RCA, strSet, &strSet);

        //assert correct datalength
        CPPUNIT_ASSERT_MESSAGE(strSet, dataLength_m == 5);

        float FinalMonitorValue(unpackSGL(&statusByte));
        //LOG(LM_INFO) << hex << monitor_RCA << endl;
        //LOG(LM_INFO) << "Target Value=" << test_values[i] << endl;
        //LOG(LM_INFO) << "Monitored Value=" << FinalMonitorValue << endl << endl;


        bufReturn <<  info << " \nSetValue= " << test_values[i] << ", OriginalMonitorValue="
                << OriginalMonitorValue << ", FinalMonitoredValue="
                << FinalMonitorValue << ", Range=[" << validmin << "," << validmax << "], Tolerance="
                << tolerance << "\n";
        strReturn = bufReturn.str();


        if (!InValidRange){
            //if SetValue is illegal
            if (checkIllegalFloatCommands) {
                //assert unchanged within tolerance
                CPPUNIT_ASSERT_MESSAGE(strReturn + ": Illegal value not ignored as expected.", fabs(OriginalMonitorValue - FinalMonitorValue) <= tolerance);
            }
        }
        else{
            //if SetValue is legal
            //assert that monitored value is close enough to the target value
            CPPUNIT_ASSERT_MESSAGE(strReturn + ": Monitored value not close enough to set value.", fabs(test_values[i] - FinalMonitorValue) <= tolerance);
        }
    }//end for loop

    //Set to 0 after test has been performed
    packSGL(0);
    info = "";
    commandImpl(control_RCA, "Setting to zero after performing test.\n", &info);
    SLEEP(100);
}

void ColdCartridgeTestFixture::implTestHeaters(
        AmbRelativeAddr controlSisHeaterEnable_RCA,
        AmbRelativeAddr sisHeaterEnable_RCA,
        AmbRelativeAddr sisHeaterCurrent_RCA,
        const std::string &callerDescription){

    string strReturn;
    unsigned char statusByte;

    //disable the heater
    dataLength_m = 1;
    data_m[0] = 0;
    command(controlSisHeaterEnable_RCA, callerDescription + " - disable heater.", &strReturn);

    //read the current
    monitor(sisHeaterCurrent_RCA, callerDescription + " - Read heater current while disabled.", &strReturn);

    //test returned datalength and statusByte
    CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 5);
    float heaterCurrentWhenDisabled(unpackSGL(&statusByte));
    CPPUNIT_ASSERT_MESSAGE(strReturn, statusByte == FEMC_NO_ERROR);

    LOG(LM_INFO) << "ColdCartridgeTestFixture::implTestHeaters heaterCurrentWhenDisabled=" << heaterCurrentWhenDisabled << endl;

    SLEEP(200);

    //enable the heater
    dataLength_m = 1;
    data_m[0] = 1;
    command(controlSisHeaterEnable_RCA, callerDescription + " - enable heater.", &strReturn);

    SLEEP(600);

    //read the current
    monitor(sisHeaterCurrent_RCA, callerDescription + " - Read heater current when enabled.", &strReturn);
    float heaterCurrentWhenEnabled(unpackSGL(&statusByte));

    LOG(LM_INFO) << "ColdCartridgeTestFixture::implTestHeaters heaterCurrentWhenEnabled=" << heaterCurrentWhenEnabled << endl;

    //test that heater current has increased
    CPPUNIT_ASSERT_MESSAGE(strReturn, heaterCurrentWhenEnabled > heaterCurrentWhenDisabled);

    SLEEP(600);

    //After 1 seconds test that heater turns itself off
    //read the current
    monitor(sisHeaterCurrent_RCA, callerDescription + " - Read heater current after 1.2 seconds.", &strReturn);
    float heaterCurrentAfterWait(unpackSGL(&statusByte));

    LOG(LM_INFO) << "ColdCartridgeTestFixture::implTestHeaters heaterCurrentAfterWait=" << heaterCurrentAfterWait << endl;

    CPPUNIT_ASSERT_MESSAGE(strReturn, heaterCurrentAfterWait < heaterCurrentWhenEnabled);

    //disable the heater before leaving function
    dataLength_m = 1;
    data_m[0] = 0;
    command(controlSisHeaterEnable_RCA, callerDescription + " - disable heater.", &strReturn);
}

const std::vector<float> &ColdCartridgeTestFixture::getTestValuesSISSenseResistor() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.reserve(4);
        values.resize(0);
        values.push_back(6.0);     // In-range value
        values.push_back(17.0);    // in-range value
        values.push_back(-51.0);   // negative value not allowed
        values.push_back(0.0);     // zero not allowed
    }
    return values;
}

const std::vector<float> &ColdCartridgeTestFixture::getTestValuesLNAVd() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.reserve(3);
        values.resize(0);
        values.push_back(0.5);      // in-range value
        values.push_back(0.7);      // in-range value
        values.push_back(200.0);    // impossible value
    }
    return values;
}

void ColdCartridgeTestFixture::getLimitsLNAVd(float &min, float &max, float &tolerance) const {
    //const float ColdCartridgeTestFixture::ValidMinLNAVd = -5.0;
    //const float ColdCartridgeTestFixture::ValidMaxLNAVd = 5.0;
    //const float ColdCartridgeTestFixture::ToleranceLNAVd = 0.1;
    min = -5.0;
    max = 5.0;
    tolerance = 0.1;
}

const std::vector<float> &ColdCartridgeTestFixture::getTestValuesLNAId() const {
    static bool firstTime(true);
    static std::vector<float> values;
    if (firstTime) {
        firstTime = false;
        values.reserve(3);
        values.resize(0);
        values.push_back(1.0);      // in-range value
        values.push_back(2.0);      // in-range value
        values.push_back(200.0);    // impossible value
    }
    return values;
}

void ColdCartridgeTestFixture::getLimitsLNAId(float &min, float &max, float &tolerance) const {
    //const float ColdCartridgeTestFixture::ValidMinLNAId = -50.0;
    //const float ColdCartridgeTestFixture::ValidMaxLNAId = 50.0;
    //const float ColdCartridgeTestFixture::ToleranceLNAId = 0.1;
    min = -50.0;
    max = 50.0;
    tolerance = 0.1;
}

void ColdCartridgeTestFixture::getLimitsLNAVg(float &min, float &max) const {
    //const float ColdCartridgeTestFixture::ValidMinLNAVg = -5.0;
    //const float ColdCartridgeTestFixture::ValidMaxLNAVg = 5.0;
    min = -5.0;
    max = 5.0;
}

