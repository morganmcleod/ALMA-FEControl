/*
 * CPDSTestFixture.h
 *
 *  Created on: Oct 11, 2010
 *      Author: dnagaraj
 */

#ifndef CPDSTestFixture_H_
#define CPDSTestFixture_H_

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class CPDSTestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(CPDSTestFixture);
    //Monitor points, include exercising related commands.
    CPPUNIT_TEST(testGET_POWER_DISTRIBUTION_POWERERED_MODULES);

    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE1_ENABLE);
    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE3_ENABLE);
    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE4_ENABLE);
    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE6_ENABLE);
    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE7_ENABLE);
    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE8_ENABLE);
    CPPUNIT_TEST(testSET_POWER_DISTRIBUTION_MODULE9_ENABLE);

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testGET_POWER_DISTRIBUTION_POWERERED_MODULES();

    void testSET_POWER_DISTRIBUTION_MODULE1_ENABLE();
    void testSET_POWER_DISTRIBUTION_MODULE3_ENABLE();
    void testSET_POWER_DISTRIBUTION_MODULE4_ENABLE();
    void testSET_POWER_DISTRIBUTION_MODULE6_ENABLE();
    void testSET_POWER_DISTRIBUTION_MODULE7_ENABLE();
    void testSET_POWER_DISTRIBUTION_MODULE8_ENABLE();
    void testSET_POWER_DISTRIBUTION_MODULE9_ENABLE();
   //TODO: These functions should be implemented in terms of the private helper implPwrDistributionModule() below.


private:

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
            ENABLE_MODULE       = 0x000C,
            NUM_ENABLED         = 0x00A0,

            MON_ENABLE_BAND1    = 0xA00C,
            MON_ENABLE_BAND2    = 0xA01C,
            MON_ENABLE_BAND3    = 0xA02C,
            MON_ENABLE_BAND4    = 0xA03C,
            MON_ENABLE_BAND5    = 0xA04C,
            MON_ENABLE_BAND6    = 0xA05C,
            MON_ENABLE_BAND7    = 0xA06C,
            MON_ENABLE_BAND8    = 0xA07C,
            MON_ENABLE_BAND9    = 0xA08C,
            MON_ENABLE_BAND10   = 0xA09C,

            CTRL_ENABLE_BAND1   = 0x1A00C,
            CTRL_ENABLE_BAND2   = 0x1A01C,
            CTRL_ENABLE_BAND3   = 0x1A02C,
            CTRL_ENABLE_BAND4   = 0x1A03C,
            CTRL_ENABLE_BAND5   = 0x1A04C,
            CTRL_ENABLE_BAND6   = 0x1A05C,
            CTRL_ENABLE_BAND7   = 0x1A06C,
            CTRL_ENABLE_BAND8   = 0x1A07C,
            CTRL_ENABLE_BAND9   = 0x1A08C,
            CTRL_ENABLE_BAND10  = 0x1A09C

        };

  void implPwrDistributionModule(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
    								unsigned int bandNum,const std::string &callerDescription);

  void checkCurrentandVoltage(AmbRelativeAddr monitor_RCA,const std::string &callerDescription);

  void checkCurrentWhenBandSet(AmbRelativeAddr monitor_RCA,const std::string &callerDescription);

  void checkVoltageWhenBandSet(AmbRelativeAddr monitor_RCA,float volt,const std::string &callerDescription);
 };

#endif /* CPDSTestFixture_H_ */
