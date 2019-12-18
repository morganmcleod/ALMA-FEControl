#ifndef IFSWITCHTESTFIXTURE_H
#define IFSWITCHTESTFIXTURE_H

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class IFSwitchTestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(IFSwitchTestFixture);
    //Monitor points, include exercising related commands.
    CPPUNIT_TEST(testGET_IF_SWITCH_CHANNEL01_ASSEMBLY_TEMP);
    CPPUNIT_TEST(testGET_IF_SWITCH_CHANNEL02_ASSEMBLY_TEMP);
    CPPUNIT_TEST(testGET_IF_SWITCH_CHANNEL11_ASSEMBLY_TEMP);
    CPPUNIT_TEST(testGET_IF_SWITCH_CHANNEL12_ASSEMBLY_TEMP);

    //Control points, include exercising related monitor points.
    CPPUNIT_TEST(testSET_IF_SWITCH_CARTRIDGE);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL01_ATTENUATION);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL02_ATTENUATION);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL11_ATTENUATION);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL12_ATTENUATION);
    CPPUNIT_TEST(testSET_IF_SWITCH_ALL_CHANNELS_ATTENUATION);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL01_TEMP_SERVO_ENABLE);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL02_TEMP_SERVO_ENABLE);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL11_TEMP_SERVO_ENABLE);
    CPPUNIT_TEST(testSET_IF_SWITCH_CHANNEL12_TEMP_SERVO_ENABLE);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp();
    void tearDown();

    void testGET_IF_SWITCH_CHANNEL01_ASSEMBLY_TEMP();
    void testGET_IF_SWITCH_CHANNEL02_ASSEMBLY_TEMP();
    void testGET_IF_SWITCH_CHANNEL11_ASSEMBLY_TEMP();
    void testGET_IF_SWITCH_CHANNEL12_ASSEMBLY_TEMP();
    // These functions are implemented in terms of the private helper implGetTemperaure() below.

    void testSET_IF_SWITCH_CARTRIDGE();
    // This function is implemented in terms of the private helper implSwitchCartridge() below.

    void testSET_IF_SWITCH_CHANNEL01_ATTENUATION();
    void testSET_IF_SWITCH_CHANNEL02_ATTENUATION();
    void testSET_IF_SWITCH_CHANNEL11_ATTENUATION();
    void testSET_IF_SWITCH_CHANNEL12_ATTENUATION();
    // These functions are implemented in terms of the private helper implSetAttenuation() below.

    void testSET_IF_SWITCH_ALL_CHANNELS_ATTENUATION();

    void testSET_IF_SWITCH_CHANNEL01_TEMP_SERVO_ENABLE();
    void testSET_IF_SWITCH_CHANNEL02_TEMP_SERVO_ENABLE();
    void testSET_IF_SWITCH_CHANNEL11_TEMP_SERVO_ENABLE();
    void testSET_IF_SWITCH_CHANNEL12_TEMP_SERVO_ENABLE();
    // These functions are implemented in terms of the private helper implSetTempServo() below.

private:

    enum MonitorControlOffset {
        TEMP_SERVO_ENABLE   = 0x0000,   /// monitor/command the state of the temperature servo
        ATTENUATION         = 0x0001,   /// monitor/command each IF channel attenuation
        ASSEMBLY_TEMP       = 0x0002,   /// monitor each IF switch temperature
        SWITCH_CARTRIDGE    = 0x0010,   /// monitor/command the currently selected band
        ALL_CH_ATTENUATION  = 0x0014,   /// monitor/command all channels attenuation
        controlRCA          = 0x10000,  /// add this to the monitor RCA to get the corresponding command RCA
        baseRCA             = 0xB000,   /// all IF switch M&C points are in this range
        pol1                = 0x0004,   /// add this to the pol0 RCA to get the corresponding pol1 RCA
        sb2                 = 0x0008,   /// add this to the sb1 RCA to get the corresponding sb2 RCA

        switchCartridge_RCA         = baseRCA + SWITCH_CARTRIDGE,
        pol0Sb1Attenuation_RCA      = baseRCA + ATTENUATION,
        pol0Sb2Attenuation_RCA      = baseRCA + ATTENUATION + sb2,
        pol1Sb1Attenuation_RCA      = baseRCA + ATTENUATION + pol1,
        pol1Sb2Attenuation_RCA      = baseRCA + ATTENUATION + pol1 + sb2,
        pol0Sb1AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP,
        pol0Sb2AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP + sb2,
        pol1Sb1AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP + pol1,
        pol1Sb2AssemblyTemp_RCA     = baseRCA + ASSEMBLY_TEMP + pol1 + sb2,
        pol0Sb1TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE,
        pol0Sb2TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE + sb2,
        pol1Sb1TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE + pol1,
        pol1Sb2TempServoEnable_RCA  = baseRCA + TEMP_SERVO_ENABLE + pol1 + sb2,
        allChannelAttenuation_RCA   = baseRCA + ALL_CH_ATTENUATION,

        ctrlSwitchCartridge_RCA    = switchCartridge_RCA + controlRCA,
        ctrlPol0Sb1Attenuation_RCA = pol0Sb1Attenuation_RCA + controlRCA,
        ctrlPol0Sb2Attenuation_RCA = pol0Sb2Attenuation_RCA + controlRCA,
        ctrlPol1Sb1Attenuation_RCA = pol1Sb1Attenuation_RCA + controlRCA,
        ctrlPol1Sb2Attenuation_RCA = pol1Sb2Attenuation_RCA + controlRCA,
        ctrlPol0Sb1TempServoEnable_RCA  = pol0Sb1TempServoEnable_RCA + controlRCA,
        ctrlPol0Sb2TempServoEnable_RCA  = pol0Sb2TempServoEnable_RCA + controlRCA,
        ctrlPol1Sb1TempServoEnable_RCA  = pol1Sb1TempServoEnable_RCA + controlRCA,
        ctrlPol1Sb2TempServoEnable_RCA  = pol1Sb2TempServoEnable_RCA + controlRCA,
        ctrlAllChannelAttenuation_RCA   = allChannelAttenuation_RCA + controlRCA
    };

	//TODO: put in the valid range from the ICD (in the CPP file.)
    static const float assemblyTempLowerLimit_m;
	static const float assemblyTempUpperLimit_m;

	void implGetTemperaure(AmbRelativeAddr ctrlTempServoEnable_RCA, AmbRelativeAddr assemblyTemp_RCA, const std::string &callerDescription);
	/// Enable the temp servo with the given RCA and then read the temperature.
	/// Check the returned temperature data.
	/// The msg string describes which pol/sb is being checked and is to be passed to the base class monitor() and command() functions.

	void implSwitchCartridge(unsigned int bandMinusOne, const std::string &callerDescription);
	/// Switch to the specified band position and then monitor to see that it switched.
	/// If the specified band is not allowed (outside the range 0-9) then we raise a failure if the command was allowed.
    /// The msg string describes which pol/sb is being checked and is to be passed to the base class monitor() and command() functions.

	void implSetAttenuation(AmbRelativeAddr ctrlAttenuation_RCA, AmbRelativeAddr getAttenuation_RCA, const std::string &callerDescription);
	/// Exercise all legal and two illegal values for the given attenuation control and monitor points.
	/// Legal values are (0-15). Illegal values are 16 and 255.
	/// Finish with the attenuation set to 0.
    /// The msg string describes which pol/sb is being checked and is to be passed to the base class monitor() and command() functions.

	void implSetTempServo(AmbRelativeAddr ctrlTempServoEnable_RCA, AmbRelativeAddr getTempServoEnable_RCA, const std::string &callerDescription);
	/// Exercise all legal and two illegal values for the temp servo enable.
	/// Legal values are (0, 1). Illegal values are 2 and 255.
    /// Finish with the temp servo enable set to 1.
    /// The msg string describes which pol/sb is being checked and is to be passed to the base class monitor() and command() functions.
};

#endif  //IFSWITCHTESTFIXTURE_H

