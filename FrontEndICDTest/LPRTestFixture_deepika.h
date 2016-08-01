/*
 * LPRTestFixture.h
 *
 *  Created on: Sep 21, 2010
 *      Author: dnagaraj
 */

#ifndef LPRTestFixture_H_
#define LPRTestFixture_H_

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class LPRTestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(LPRTestFixture);
    //Monitor points, include exercising related commands.
    CPPUNIT_TEST(testGET_LPR_OPT_SWITCH_STATE);
    CPPUNIT_TEST(testGET_LPR_OPT_SWITCH_BUSY);

    CPPUNIT_TEST(testGET_LPR_TEMP0_TEMP);
    CPPUNIT_TEST(testGET_LPR_TEMP1_TEMP);
    CPPUNIT_TEST(testGET_LPR_EDFA_PHOTO_DETECTOR_POWER);
    CPPUNIT_TEST(testGET_LPR_EDFA_LASER_PUMP_TEMP);

    CPPUNIT_TEST(testGET_LPR_EDFA_LASER_DRIVE_CURRENT);
    CPPUNIT_TEST(testGET_LPR_EDFA_LASER_PHOTO_DETECT_CURRENT);
    CPPUNIT_TEST(testGET_LPR_EDFA_PHOTO_DETECTOR_CURRENT);

    CPPUNIT_TEST(testGET_LPR_EDFA_DRIVER_TEMPERATURE_ALARM);

    //Control points, include exercising related monitor points.
    CPPUNIT_TEST(testSET_OPTICAL_SWITCH_SHUTTER);
    CPPUNIT_TEST(testSET_LPR_OPTICAL_SWITCH_PORT);
    CPPUNIT_TEST(testSET_LPR_EDFA_MODULATION_INPUT_VALUE);

    CPPUNIT_TEST(testSET_OPTICAL_SWITCH_FORCE_SHUTTER);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testGET_LPR_OPT_SWITCH_STATE();
    void testGET_LPR_OPT_SWITCH_BUSY();
    //TODO: These functions should be implemented in terms of the private helper implGetSwitchStatus() below.

    void testGET_LPR_TEMP0_TEMP();
    void testGET_LPR_TEMP1_TEMP();
    void testGET_LPR_EDFA_PHOTO_DETECTOR_POWER();
    void testGET_LPR_EDFA_LASER_PUMP_TEMP();
    //TODO: These functions should be implemented in terms of the private helper implCheckInavlidVals

    void testGET_LPR_EDFA_LASER_DRIVE_CURRENT();
    void testGET_LPR_EDFA_LASER_PHOTO_DETECT_CURRENT();
    void testGET_LPR_EDFA_PHOTO_DETECTOR_CURRENT();
    //TODO: These functions should be implemented in terms of the private helper implGetProperty.

    void testGET_LPR_EDFA_DRIVER_TEMPERATURE_ALARM();

    void testSET_LPR_OPTICAL_SWITCH_PORT();
    //TODO: This function should be implemented in terms of the private helper implOpticalSwitchPort.

    void testSET_LPR_EDFA_MODULATION_INPUT_VALUE();
    void testSET_OPTICAL_SWITCH_SHUTTER();
    void testSET_OPTICAL_SWITCH_FORCE_SHUTTER();


private:

    enum MonitorControlOffset {
            LPR_TEMPERATURE_0               = 0x0000,
            LPR_TEMPERATURE_1               = 0x0010,
            OPTICAL_SWITCH_PORT             = 0x0020,
            OPTICAL_SWITCH_SHUTTER          = 0x0022,
            OPTICAL_SWITCH_FORCE_SHUTTER    = 0x0024,
            OPTICAL_SWITCH_STATE            = 0x0026,
            OPTICAL_SWITCH_BUSY             = 0x0028,
            EDFA_LASER_PUMP_TEMPERATURE     = 0x0030,
            EDFA_LASER_DRIVE_CURRENT        = 0x0031,
            EDFA_LASER_PHOTODETECT_CURRENT  = 0x0032,
            EDFA_PHOTODETECT_CURRENT        = 0x0034,
            EDFA_PHOTODETECT_POWER          = 0x0036,
            EDFA_MODULATION_INPUT           = 0x0038,
            EDFA_DRIVER_TEMPERATURE_ALARM   = 0x003C,
            controlRCA          			= 0x10000,
            baseRCA 						= 0xD000,

            LPRTemperature0_RCA             = baseRCA + LPR_TEMPERATURE_0,
            LPRTemperature1_RCA             = baseRCA + LPR_TEMPERATURE_1,
            opticalSwitchPort_RCA           = baseRCA + OPTICAL_SWITCH_PORT,
            opticalSwitchShutter_RCA        = baseRCA + OPTICAL_SWITCH_SHUTTER,
            opticalSwitchForceShutter_RCA   = baseRCA + OPTICAL_SWITCH_FORCE_SHUTTER,
            opticalSwitchState_RCA          = baseRCA + OPTICAL_SWITCH_STATE,
            opticalSwitchBusy_RCA           = baseRCA + OPTICAL_SWITCH_BUSY,
            EDFALaserPumpTemperature_RCA    = baseRCA + EDFA_LASER_PUMP_TEMPERATURE,
            EDFALaserDriveCurrent_RCA       = baseRCA + EDFA_LASER_DRIVE_CURRENT,
            EDFALaserPhotoDetectCurrent_RCA = baseRCA + EDFA_LASER_PHOTODETECT_CURRENT,
            EDFAPhotoDetectCurrent_RCA      = baseRCA + EDFA_PHOTODETECT_CURRENT,
            EDFAPhotoDetectPower_RCA        = baseRCA + EDFA_PHOTODETECT_POWER,
            EDFAModulationInput_RCA         = baseRCA + EDFA_MODULATION_INPUT,
            EDFADriverTemperatureAlarm_RCA	= baseRCA + EDFA_DRIVER_TEMPERATURE_ALARM,

            ctrlopticalSwitchPort_RCA           = controlRCA + opticalSwitchPort_RCA,
            ctrlopticalSwitchShutter_RCA        = controlRCA + opticalSwitchShutter_RCA,
            ctrlopticalSwitchForceShutter_RCA   = controlRCA + opticalSwitchForceShutter_RCA,
            ctrlEDFAModulationInput_RCA         = controlRCA + EDFAModulationInput_RCA
    };

    static const float TempSensorLL;
    static const float TempSensorUL;
    static const float LaserDriveCurrentLL;
    static const float LaserDriveCurrentUL;
    static const float LaserPhotoDetectCurrentLL;
    static const float LaserPhotoDetectCurrentUL;
    static const float PhotoDetectorCurrentLL;
    static const float PhotoDetectorCurrentUL;
    static const float PhotoDetectorPowerLL;
    static const float PhotoDetectorPowerUL;
    static const float LaserPumpTempLL;
    static const float LaserPumpTempUL;

    void implCheckWithinRange(AmbRelativeAddr tempSensor_RCA,const float minVal,
    						  const float maxVal, const std::string &msg);

    void implOpticalSwitchPort(unsigned int bandMinusOne,const std::string &msg);

};

#endif /* LPRTestFixture_H_ */
