/*
 * CompressorTestFixture.h
 *
 *  Created on: Oct 13, 2010
 *      Author: dnagaraj
 */

#ifndef CompressorTestFixture_H_
#define CompressorTestFixture_H_

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class CompressorTestFixture : public AmbDeviceTestFixture {

	CPPUNIT_TEST_SUITE(CompressorTestFixture);
    //Monitor points, include exercising related commands.

    CPPUNIT_TEST(testSetCompressorON);
    CPPUNIT_TEST(testGET_TEMP_1);
    CPPUNIT_TEST(testGET_TEMP_2);
    CPPUNIT_TEST(testGET_TEMP_3);
    CPPUNIT_TEST(testGET_TEMP_4);
    CPPUNIT_TEST(testGET_AUX_1);
    CPPUNIT_TEST(testGET_AUX_2);
    CPPUNIT_TEST(testGET_PRESSURE);
    CPPUNIT_TEST(testGET_PRESSURE_ALARM);
    CPPUNIT_TEST(testGET_TEMP_AMARM);
   // CPPUNIT_TEST(testGET_DRIVE_INDICATION);
    CPPUNIT_TEST(testSetCompressorOFF);

    //CPPUNIT_TEST(testSET_REMOTE_DRIVE);

   CPPUNIT_TEST_SUITE_END();

public:
   CompressorTestFixture()
   : AmbDeviceTestFixture("CompressorTestFixture",0,0x25,false)
   {

   }

    void setUp();
    void tearDown();

    void testSetCompressorON();
    void testGET_TEMP_1();
    void testGET_TEMP_2();
    void testGET_TEMP_3();
    void testGET_TEMP_4();
    void testGET_AUX_1();
    void testGET_AUX_2();
    void testGET_PRESSURE();
    void testGET_PRESSURE_ALARM();
    void testGET_TEMP_AMARM();
   // void testGET_DRIVE_INDICATION();
    void testSetCompressorOFF();

    //void testSET_REMOTE_DRIVE();

private:

    enum MonitorControlOffset {
        	    TEMP_1_RCA           = 0x00001,
                TEMP_2_RCA           = 0x00002,
                TEMP_3_RCA           = 0x00003,
                TEMP_4_RCA           = 0x00004,
                AUX_1_RCA            = 0x00005,
                AUX_2_RCA            = 0x00006,
                PRESSURE_RCA		 = 0x00007,
                PRESSURE_ALARM_RCA   = 0x00008,
                TEMP_ALARM_RCA       = 0x00009,
                DRIVE_INDICATION_RCA = 0x0000A,
                REMOTE_DRIVE_RCA     = 0x01001,
                REMOTE_RESET_RCA     = 0x01002,
    	};


  void implSetRemoteDrive(int stat);

  void implCheckTemp(AmbRelativeAddr monitor_RCA,const std::string &callerDescription);

  void implCheckAux(AmbRelativeAddr monitor_RCA,const std::string &callerDescription);

  void implGetAlarm(AmbRelativeAddr monitor_RCA,const std::string &callerDescription);

};

#endif /* CompressorTestFixture_H_ */
