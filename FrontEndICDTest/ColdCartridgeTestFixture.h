#ifndef COLDCARTRIDGETESTFIXTURE_H
#define COLDCARTRIDGETESTFIXTURE_H
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


#include "CartridgeTestFixture.h"
#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <vector>

#define INSTANTIATE_COLDCART_SUITE(CLASS) \
    CPPUNIT_TEST_SUITE(CLASS); \
    CPPUNIT_TEST(ENABLE_CARTRIDGE); \
    CPPUNIT_TEST(ENABLE_LNA_POL0_SB1); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST1_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST2_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST3_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST1_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST2_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST3_GATE_VOLTAGE); \
    CPPUNIT_TEST(DISABLE_LNA_POL0_SB1); \
    CPPUNIT_TEST(ENABLE_LNA_POL0_SB2); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_LNA_ST1_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_LNA_ST2_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_LNA_ST3_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB2_LNA_ST1_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB2_LNA_ST2_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB2_LNA_ST3_GATE_VOLTAGE); \
    CPPUNIT_TEST(DISABLE_LNA_POL0_SB2); \
    CPPUNIT_TEST(ENABLE_LNA_POL1_SB1); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST1_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST2_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST3_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST1_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST2_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST3_GATE_VOLTAGE); \
    CPPUNIT_TEST(DISABLE_LNA_POL0_SB1); \
    CPPUNIT_TEST(ENABLE_LNA_POL1_SB2); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_LNA_ST1_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_LNA_ST2_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_LNA_ST3_DRAIN_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB2_LNA_ST1_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB2_LNA_ST2_GATE_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB2_LNA_ST3_GATE_VOLTAGE); \
    CPPUNIT_TEST(DISABLE_LNA_POL1_SB2); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_LNA_LED_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_LNA_LED_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_LNA_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_LNA_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_SIS_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_SIS_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_SIS_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_SIS_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_SIS_OPEN_LOOP); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_SIS_OPEN_LOOP); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_SIS_OPEN_LOOP); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_SIS_OPEN_LOOP); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_SIS_MAGNET_CURRENT); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB2_SIS_MAGNET_CURRENT); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_SIS_MAGNET_CURRENT); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB2_SIS_MAGNET_CURRENT); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SIS_HEATER_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SIS_HEATER_ENABLE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_SIS_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB2_SIS_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_SIS_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB2_SIS_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_SIS_MAGNET_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB2_SIS_MAGNET_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_SIS_MAGNET_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB2_SIS_MAGNET_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SIS_HEATER_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SIS_HEATER_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_TEMP0_TEMP); \
    CPPUNIT_TEST(testGET_CARTRIDGE_TEMP1_TEMP); \
    CPPUNIT_TEST(testGET_CARTRIDGE_TEMP2_TEMP); \
    CPPUNIT_TEST(testGET_CARTRIDGE_TEMP3_TEMP); \
    CPPUNIT_TEST(testGET_CARTRIDGE_TEMP4_TEMP); \
    CPPUNIT_TEST(testGET_CARTRIDGE_TEMP5_TEMP); \
    CPPUNIT_TEST(DISABLE_CARTRIDGE); \
    CPPUNIT_TEST_SUITE_END();

class ColdCartridgeTestFixture : public CartridgeTestFixture {
public:

    ColdCartridgeTestFixture(int band,
                             const std::string &name = std::string("ColdCartridgeTestFixture"),
                             AmbChannel channel = 0,
                             AmbNodeAddr nodeAddress = 0x13,
                             bool expectStatusByte = true);

    virtual ~ColdCartridgeTestFixture()
      {}

    void setUp();
    void tearDown();

    //Functions to call in the test script to enable and disable LNAs:
    void ENABLE_LNA_POL0_SB1();
    void DISABLE_LNA_POL0_SB1();
    void ENABLE_LNA_POL0_SB2();
    void DISABLE_LNA_POL0_SB2();
    void ENABLE_LNA_POL1_SB1();
    void DISABLE_LNA_POL1_SB1();
    void ENABLE_LNA_POL1_SB2();
    void DISABLE_LNA_POL1_SB2();

    //Monitor functions
    void testGET_CARTRIDGE_POL0_SB1_LNA_ST1_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB1_LNA_ST2_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB1_LNA_ST3_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB2_LNA_ST1_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB2_LNA_ST2_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB2_LNA_ST3_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB1_LNA_ST1_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB1_LNA_ST2_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB1_LNA_ST3_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB2_LNA_ST1_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB2_LNA_ST2_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB2_LNA_ST3_GATE_VOLTAGE();

    void testGET_CARTRIDGE_POL0_SB1_SIS_CURRENT();
    void testGET_CARTRIDGE_POL0_SB2_SIS_CURRENT();
    void testGET_CARTRIDGE_POL1_SB1_SIS_CURRENT();
    void testGET_CARTRIDGE_POL1_SB2_SIS_CURRENT();
    void testGET_CARTRIDGE_POL0_SB1_SIS_MAGNET_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB2_SIS_MAGNET_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB1_SIS_MAGNET_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB2_SIS_MAGNET_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SIS_HEATER_CURRENT();
    void testGET_CARTRIDGE_POL1_SIS_HEATER_CURRENT();

    //Control functions
    void testSET_CARTRIDGE_POL0_SB1_LNA_ST1_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB1_LNA_ST2_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB1_LNA_ST3_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB2_LNA_ST1_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB2_LNA_ST2_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB2_LNA_ST3_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB1_LNA_ST1_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB1_LNA_ST2_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB1_LNA_ST3_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB2_LNA_ST1_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB2_LNA_ST2_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB2_LNA_ST3_DRAIN_VOLTAGE();

    void testSET_CARTRIDGE_SIS_CURRENT_SENSE_RESISTOR();
    void testSET_CARTRIDGE_POL0_SB1_SIS_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB2_SIS_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB1_SIS_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB2_SIS_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB1_SIS_OPEN_LOOP();
    void testSET_CARTRIDGE_POL0_SB2_SIS_OPEN_LOOP();
    void testSET_CARTRIDGE_POL1_SB1_SIS_OPEN_LOOP();
    void testSET_CARTRIDGE_POL1_SB2_SIS_OPEN_LOOP();
    void testSET_CARTRIDGE_POL0_SB1_SIS_MAGNET_CURRENT();
    void testSET_CARTRIDGE_POL0_SB2_SIS_MAGNET_CURRENT();
    void testSET_CARTRIDGE_POL1_SB1_SIS_MAGNET_CURRENT();
    void testSET_CARTRIDGE_POL1_SB2_SIS_MAGNET_CURRENT();
    void testSET_CARTRIDGE_POL0_SIS_HEATER_ENABLE();
    void testSET_CARTRIDGE_POL1_SIS_HEATER_ENABLE();

    void testSET_CARTRIDGE_POL0_SB1_LNA_ENABLE();
    void testSET_CARTRIDGE_POL0_SB2_LNA_ENABLE();
    void testSET_CARTRIDGE_POL1_SB1_LNA_ENABLE();
    void testSET_CARTRIDGE_POL1_SB2_LNA_ENABLE();

    void testSET_CARTRIDGE_POL0_LNA_LED_ENABLE();
    void testSET_CARTRIDGE_POL1_LNA_LED_ENABLE();

    void testGET_CARTRIDGE_TEMP0_TEMP();
    void testGET_CARTRIDGE_TEMP1_TEMP();
    void testGET_CARTRIDGE_TEMP2_TEMP();
    void testGET_CARTRIDGE_TEMP3_TEMP();
    void testGET_CARTRIDGE_TEMP4_TEMP();
    void testGET_CARTRIDGE_TEMP5_TEMP();

protected:
    void SET_LNA_SIS_VALUE(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
                         const std::vector<float> &test_values,
                         const float validmin, const float validmax,
                         const float tolerance, const std::string &msg);

    void SET_LNA_VALUE_Id(AmbRelativeAddr controlVd_RCA,
             AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
             const std::vector<float> &test_values,
             const float validmin, const float validmax,
             const float tolerance, const std::string &msg);

    void implTestHeaters(AmbRelativeAddr controlSisHeaterEnable_RCA,
                         AmbRelativeAddr sisHeaterEnable_RCA,
                         AmbRelativeAddr sisHeaterCurrent_RCA,
                         const std::string &callerDescription);

// Get test values and limits, common to all cold carts:
    virtual const std::vector<float> &getTestValuesSISSenseResistor() const;
    ///< Get the test values for SIS current sense resistor.
    ///< Provided by this class but may be overridden.

    virtual const std::vector<float> &getTestValuesLNAVd() const;
    ///< Get the test values for LNA drain voltage.
    ///< Provided by this class but may be overridden.

    virtual void getLimitsLNAVd(float &min, float &max, float &tolerance) const;
    ///< Get the limits and tolerance for LNA drain voltage
    ///< Provided by this class but may be overridden.

    virtual const std::vector<float> &getTestValuesLNAId() const;
    ///< Get the test values for LNA drain current.
    ///< Provided by this class but may be overridden.

    virtual void getLimitsLNAId(float &min, float &max, float &tolerance) const;
    ///< Get the limits and tolerance for LNA drain current.
    ///< Provided by this class but may be overridden.

    virtual void getLimitsLNAVg(float &min, float &max) const;
    ///< Get the limits for LNA gate voltage.
    ///< Provided by this class but may be overridden.

// Get test values and limits, band specific values to be implemented in derived class:

    virtual const std::vector<float> &getTestValuesSISVoltage() const = 0;
    ///< Get the band-specific test values for SIS voltage.
    ///< Derived class must provide this function.

    virtual void getLimitsSisVoltage(float &min, float &max, float &tolerance) const = 0;
    ///< Get the band-specific limits and tolerance for SIS voltage.
    ///< Derived class must provide this function.

    virtual void getLimitsSISCurrent(float &min, float &max) const = 0;
    ///< Get the band-specific limits for SIS current.
    ///< Derived class must provide this function.

    virtual const std::vector<float> &getTestValuesMagnetCurrent() const = 0;
    ///< Get the band-specific test values for SIS magnet current.
    ///< Derived class must provide this function.

    virtual void getLimitsMagnetCurrent(float &min, float &max, float &tolerance) const = 0;
    ///< Get the band-specific limits for magnet current.
    ///< Derived class must provide this function.

    virtual void getLimitsMagnetVoltage(float &min, float &max) const = 0;
    ///< Get the band-specific limits for magnet voltage.
    ///< Derived class must provide this function.

    virtual void getLimitsHeaterCurrent(float &min, float &max) const = 0;
    ///< Get the band-specific limits for heater current.
    ///< Derived class must provide this function.

private:

    enum MonitorControlOffset {
        controlRCA              = 0x10000,  ///< add this to the monitor RCA to get the corresponding command RCA
        pol1                    = 0x0400,   ///< add this to the pol0 RCA to get the corresponding pol1 RCA
        sb2                     = 0x0080,   ///< add this to the sb1 RCA to get the corresponding sb2 RCA

        SIS_SENSE_RESISTOR      = 0x0000,
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
        SIS_HEATER_CURRENT      = 0x01C0,
        CARTRIDGE_TEMP          = 0x0880
    };

    AmbRelativeAddr sisSenseResistor_RCA;
    AmbRelativeAddr sisSenseResistor2_RCA;
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

    //Control RCAs
    AmbRelativeAddr ctrlsisSenseResistor_RCA;
    AmbRelativeAddr ctrlsisSenseResistor2_RCA;
    AmbRelativeAddr ctrlsisPol0Sb1Voltage_RCA;
    AmbRelativeAddr ctrlsisPol0Sb1OpenLoop_RCA;
    AmbRelativeAddr ctrlsisPol0Sb2Voltage_RCA;
    AmbRelativeAddr ctrlsisPol0Sb2OpenLoop_RCA;
    AmbRelativeAddr ctrlsisPol1Sb1Voltage_RCA;
    AmbRelativeAddr ctrlsisPol1Sb1OpenLoop_RCA;
    AmbRelativeAddr ctrlsisPol1Sb2Voltage_RCA;
    AmbRelativeAddr ctrlsisPol1Sb2OpenLoop_RCA;

    AmbRelativeAddr ctrlsisMagnetPol0Sb1Current_RCA;
    AmbRelativeAddr ctrlsisMagnetPol0Sb2Current_RCA;
    AmbRelativeAddr ctrlsisMagnetPol1Sb1Current_RCA;
    AmbRelativeAddr ctrlsisMagnetPol1Sb2Current_RCA;

    AmbRelativeAddr ctrllnaPol0Sb1St1DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol0Sb1St1DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol0Sb1St2DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol0Sb1St2DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol0Sb1St3DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol0Sb1St3DrainCurrent_RCA;

    AmbRelativeAddr ctrllnaPol0Sb2St1DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol0Sb2St1DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol0Sb2St2DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol0Sb2St2DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol0Sb2St3DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol0Sb2St3DrainCurrent_RCA;

    AmbRelativeAddr ctrllnaPol1Sb1St1DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol1Sb1St1DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol1Sb1St2DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol1Sb1St2DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol1Sb1St3DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol1Sb1St3DrainCurrent_RCA;

    AmbRelativeAddr ctrllnaPol1Sb2St1DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol1Sb2St1DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol1Sb2St2DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol1Sb2St2DrainCurrent_RCA;
    AmbRelativeAddr ctrllnaPol1Sb2St3DrainVoltage_RCA;
    AmbRelativeAddr ctrllnaPol1Sb2St3DrainCurrent_RCA;

    AmbRelativeAddr ctrllnaPol0Sb1Enable_RCA;
    AmbRelativeAddr ctrllnaPol0Sb2Enable_RCA;
    AmbRelativeAddr ctrllnaPol1Sb1Enable_RCA;
    AmbRelativeAddr ctrllnaPol1Sb2Enable_RCA;

    AmbRelativeAddr ctrllnaLedPol0Enable_RCA;
    AmbRelativeAddr ctrllnaLedPol1Enable_RCA;
    AmbRelativeAddr ctrlsisHeaterPol0Enable_RCA;
    AmbRelativeAddr ctrlsisHeaterPol1Enable_RCA;
};
#endif /* COLDCARTRIDGETESTFIXTURE_H */
