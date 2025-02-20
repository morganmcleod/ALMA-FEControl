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

#include "band1.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Band1CCATestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(Band1WCATestFixture);

void Band1CCATestFixture::testSET_CARTRIDGE_POL0_SB1_LNA_ST4_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb1St4DrainVoltage_RCA,ctrllnaPol0Sb1St4DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB1_LNA_ST5_DRAIN_VOLTAGE");
}
void Band1CCATestFixture::testSET_CARTRIDGE_POL0_SB1_LNA_ST5_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol0Sb1St5DrainVoltage_RCA,ctrllnaPol0Sb1St5DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL0_SB1_LNA_ST5_DRAIN_VOLTAGE");
}
void Band1CCATestFixture::testSET_CARTRIDGE_POL1_SB1_LNA_ST4_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb1St4DrainVoltage_RCA,ctrllnaPol1Sb1St4DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB1_LNA_ST5_DRAIN_VOLTAGE");
}
void Band1CCATestFixture::testSET_CARTRIDGE_POL1_SB1_LNA_ST5_DRAIN_VOLTAGE() {
    float min, max, tolerance;
    getLimitsLNAVd(min, max, tolerance);
    SET_LNA_SIS_VALUE(lnaPol1Sb1St5DrainVoltage_RCA,ctrllnaPol1Sb1St5DrainVoltage_RCA,
            getTestValuesLNAVd(), min, max, tolerance,
            "SET_CARTRIDGE_POL1_SB1_LNA_ST5_DRAIN_VOLTAGE");
}

void Band1CCATestFixture::testGET_CARTRIDGE_POL0_SB1_LNA_ST4_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb1St4GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB1_LNA_ST4_GATE_VOLTAGE");
}
void Band1CCATestFixture::testGET_CARTRIDGE_POL0_SB1_LNA_ST5_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol0Sb1St5GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL0_SB1_LNA_ST5_GATE_VOLTAGE");
}
void Band1CCATestFixture::testGET_CARTRIDGE_POL1_SB1_LNA_ST4_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb1St4GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB1_LNA_ST4_GATE_VOLTAGE");
}
void Band1CCATestFixture::testGET_CARTRIDGE_POL1_SB1_LNA_ST5_GATE_VOLTAGE(){
    float min, max;
    getLimitsLNAVg(min, max);
    implGetFloat(lnaPol1Sb1St5GateVoltage_RCA, min, max,
                "GET_CARTRIDGE_POL1_SB1_LNA_ST5_GATE_VOLTAGE");
}
