/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2024
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

#ifndef BAND2_H_
#define BAND2_H_

#include "ColdCartridgeTestFixture.h"
#include "WCATestFixture.h"

class Band2CCATestFixture : public ColdCartridgeTestFixture {
public:
    Band2CCATestFixture()
      : ColdCartridgeTestFixture(2, "Band2CCATestFixture")
        {}

    void testSET_CARTRIDGE_POL0_SB1_LNA_ST4_DRAIN_VOLTAGE();    
    void testSET_CARTRIDGE_POL0_SB1_LNA_ST5_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL0_SB1_LNA_ST6_DRAIN_VOLTAGE();

    void testGET_CARTRIDGE_POL0_SB1_LNA_ST4_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB1_LNA_ST5_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL0_SB1_LNA_ST6_GATE_VOLTAGE();
    
    void testSET_CARTRIDGE_POL1_SB1_LNA_ST4_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB1_LNA_ST5_DRAIN_VOLTAGE();
    void testSET_CARTRIDGE_POL1_SB1_LNA_ST6_DRAIN_VOLTAGE();

    void testGET_CARTRIDGE_POL1_SB1_LNA_ST4_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB1_LNA_ST5_GATE_VOLTAGE();
    void testGET_CARTRIDGE_POL1_SB1_LNA_ST6_GATE_VOLTAGE();
protected:

private:
    CPPUNIT_TEST_SUITE(Band2CCATestFixture);
    CPPUNIT_TEST(ENABLE_CARTRIDGE);
    CPPUNIT_TEST(ENABLE_LNA_POL0_SB1);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST1_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST2_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST3_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST4_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST5_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL0_SB1_LNA_ST6_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST1_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST2_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST3_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST4_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST5_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL0_SB1_LNA_ST6_GATE_VOLTAGE);
    CPPUNIT_TEST(DISABLE_LNA_POL0_SB1);
    CPPUNIT_TEST(ENABLE_LNA_POL1_SB1);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST1_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST2_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST3_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST4_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST5_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testSET_CARTRIDGE_POL1_SB1_LNA_ST6_DRAIN_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST1_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST2_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST3_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST4_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST5_GATE_VOLTAGE);
    CPPUNIT_TEST(testGET_CARTRIDGE_POL1_SB1_LNA_ST6_GATE_VOLTAGE);
    CPPUNIT_TEST(DISABLE_LNA_POL0_SB1);
    CPPUNIT_TEST_SUITE_END();
};

class Band2WCATestFixture : public WCATestFixture {
public:
    Band2WCATestFixture()
      : WCATestFixture(2, "Band2WCATestFixture")
        {}
private:
    INSTANTIATE_WCA_SUITE(Band2WCATestFixture)
};

#endif /* BAND2_H_ */
