#include "IFSwitchTestFixture.h"
#include <string>
using namespace std;

const float IFSwitchTestFixture::assemblyTempLowerLimit_m = -20.0; //degrees Celcius
const float IFSwitchTestFixture::assemblyTempUpperLimit_m = 100.0; //degrees Celcius

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(IFSwitchTestFixture);

void IFSwitchTestFixture::setUp() {
    AmbDeviceTestFixture::setUp();
}

void IFSwitchTestFixture::tearDown() {
    AmbDeviceTestFixture::tearDown();
}

void IFSwitchTestFixture::testGET_IF_SWITCH_CHANNEL01_ASSEMBLY_TEMP() {
    implGetTemperaure(ctrlPol0Sb1TempServoEnable_RCA, pol0Sb1AssemblyTemp_RCA, "GET_IF_SWITCH_CHANNEL01_ASSEMBLY_TEMP");
}
void IFSwitchTestFixture::testGET_IF_SWITCH_CHANNEL02_ASSEMBLY_TEMP() {
    implGetTemperaure(ctrlPol0Sb2TempServoEnable_RCA, pol0Sb2AssemblyTemp_RCA, "GET_IF_SWITCH_CHANNEL02_ASSEMBLY_TEMP");
}
void IFSwitchTestFixture::testGET_IF_SWITCH_CHANNEL11_ASSEMBLY_TEMP() {
    implGetTemperaure(ctrlPol1Sb1TempServoEnable_RCA, pol1Sb1AssemblyTemp_RCA, "GET_IF_SWITCH_CHANNEL11_ASSEMBLY_TEMP");
}
void IFSwitchTestFixture::testGET_IF_SWITCH_CHANNEL12_ASSEMBLY_TEMP() {
    implGetTemperaure(ctrlPol1Sb2TempServoEnable_RCA, pol1Sb2AssemblyTemp_RCA, "GET_IF_SWITCH_CHANNEL12_ASSEMBLY_TEMP");
}

void IFSwitchTestFixture::testSET_IF_SWITCH_CARTRIDGE() {
    //test sending bandMinusOne = 0 through 10; expecting 10 to fail.
    for (unsigned int bandMinusOne=0; bandMinusOne<=10; bandMinusOne++) {
        implSwitchCartridge(bandMinusOne, "SET_IF_SWITCH_CARTRIDGE");
    }
}

void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL01_ATTENUATION() {
    implSetAttenuation(ctrlPol0Sb1Attenuation_RCA, pol0Sb1Attenuation_RCA, "SET_IF_SWITCH_CHANNEL01_ATTENUATION");
}
void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL02_ATTENUATION() {
    implSetAttenuation(ctrlPol0Sb2Attenuation_RCA, pol0Sb2Attenuation_RCA, "SET_IF_SWITCH_CHANNEL02_ATTENUATION");
}
void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL11_ATTENUATION() {
    implSetAttenuation(ctrlPol1Sb1Attenuation_RCA, pol1Sb1Attenuation_RCA, "SET_IF_SWITCH_CHANNEL11_ATTENUATION");
}
void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL12_ATTENUATION() {
    implSetAttenuation(ctrlPol1Sb2Attenuation_RCA, pol1Sb2Attenuation_RCA, "SET_IF_SWITCH_CHANNEL12_ATTENUATION");
}

void IFSwitchTestFixture::testSET_IF_SWITCH_ALL_CHANNELS_ATTENUATION() {
    string details;
    string callerDescription("SET_IF_SWITCH_ALL_CHANNELS_ATTENUATION");
    for (int atten = 15; atten >= 0; atten--){
        data_m[0] = data_m[1] = data_m[2] = data_m[3] = (AmbDataMem_t) atten;
        dataLength_m = 4;
            command(ctrlAllChannelAttenuation_RCA, callerDescription, &details);
            monitor(allChannelAttenuation_RCA, callerDescription, &details);
            CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
            CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == atten);
            CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == atten);
            CPPUNIT_ASSERT_MESSAGE(details, data_m[2] == atten);
            CPPUNIT_ASSERT_MESSAGE(details, data_m[3] == atten);
        }
        // Try an illegal value, expecting an error:
        data_m[0] = data_m[1] = data_m[2] = data_m[3] = 16;
        dataLength_m = 4;
        command(ctrlAllChannelAttenuation_RCA, callerDescription, &details, false);
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
        CPPUNIT_ASSERT_MESSAGE(details + ": No error was raised for illegal value.", data_m[5] != FEMC_NO_ERROR);
        // Set the value to 0:
        data_m[0] = data_m[1] = data_m[2] = data_m[3] = 0;
        dataLength_m = 4;
        command(ctrlAllChannelAttenuation_RCA, callerDescription, &details);
}

void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL01_TEMP_SERVO_ENABLE() {
    implSetTempServo(ctrlPol0Sb1TempServoEnable_RCA, pol0Sb1TempServoEnable_RCA, "SET_IF_SWITCH_CHANNEL01_TEMP_SERVO_ENABLE");
}
void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL02_TEMP_SERVO_ENABLE() {
    implSetTempServo(ctrlPol0Sb2TempServoEnable_RCA, pol0Sb2TempServoEnable_RCA, "SET_IF_SWITCH_CHANNEL02_TEMP_SERVO_ENABLE");
}
void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL11_TEMP_SERVO_ENABLE() {
    implSetTempServo(ctrlPol1Sb1TempServoEnable_RCA, pol1Sb1TempServoEnable_RCA, "SET_IF_SWITCH_CHANNEL11_TEMP_SERVO_ENABLE");
}
void IFSwitchTestFixture::testSET_IF_SWITCH_CHANNEL12_TEMP_SERVO_ENABLE() {
    implSetTempServo(ctrlPol1Sb2TempServoEnable_RCA, pol1Sb2TempServoEnable_RCA, "SET_IF_SWITCH_CHANNEL12_TEMP_SERVO_ENABLE");
}

// private helper functions follow:

void IFSwitchTestFixture::implGetTemperaure(AmbRelativeAddr ctrlTempServoEnable_RCA, AmbRelativeAddr assemblyTemp_RCA, const std::string &callerDescription) {
	string details;
	unsigned char statusByte;

	//check that when Servo is disabled, HARDW_BLKD_ERR occurs
    data_m[0] = 0; //Servo Enable OFF
    dataLength_m = 1;
    //set servo enable
    command(ctrlTempServoEnable_RCA, callerDescription, &details);
    //read assembly temperature
    monitor(assemblyTemp_RCA, callerDescription, &details);
    // check datalength and value in range
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);

    //get statusbyte
    unpackSGL(&statusByte);

    //Should get no error
    CPPUNIT_ASSERT_MESSAGE(details + ": Got unexpected error.", statusByteEqual(statusByte, FEMC_NO_ERROR));

    data_m[0] = 1; //Servo Enable ON
    dataLength_m = 1;
    //set servo enable
    command(ctrlTempServoEnable_RCA, callerDescription, &details);
    //read assembly temperature
    monitor(assemblyTemp_RCA, callerDescription, &details);
    // check datalength and value in range
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
    float num = unpackSGL(&statusByte);

    //Should get no error
    CPPUNIT_ASSERT_MESSAGE(details + ": Got unexpected error.", statusByteEqual(statusByte, FEMC_NO_ERROR));

    bool inRange;
    if (   num > IFSwitchTestFixture::assemblyTempLowerLimit_m
        && num < IFSwitchTestFixture::assemblyTempUpperLimit_m)
    	inRange=true;
    else inRange=false;
    CPPUNIT_ASSERT(inRange);

}

void IFSwitchTestFixture::implSwitchCartridge(unsigned int bandMinusOne, const std::string &callerDescription){
	string details;
	data_m[0] = bandMinusOne;
    dataLength_m = 1;
    // Call command with checkSuccess==false
    command(ctrlSwitchCartridge_RCA, callerDescription, &details, false);
    // Check that the command succeeded for values 0-9 and failed otherwise:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    if (bandMinusOne > 9)
        CPPUNIT_ASSERT_MESSAGE(details + ": No error was raised for illegal value.", data_m[1] != FEMC_NO_ERROR);

    // now monitor the selected cartridge:
    monitor(switchCartridge_RCA, callerDescription, &details);
    //check 1 data byte + 1 status byte
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    //check range is 0-9
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] <= 0x09);
}

void IFSwitchTestFixture::implSetAttenuation(AmbRelativeAddr ctrlAttenuation_RCA, AmbRelativeAddr getAttenuation_RCA, const std::string &callerDescription){
	string details;
	for (int atten = 15; atten >= 0; atten--){
		data_m[0] = (AmbDataMem_t) atten;
		dataLength_m = 1;
	    command(ctrlAttenuation_RCA, callerDescription, &details);
	    monitor(getAttenuation_RCA, callerDescription, &details);
	    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[0] ==atten);
	}
	// Try an illegal value, expecting an error:
    data_m[0] = 16;
    dataLength_m = 1;
    command(ctrlAttenuation_RCA, callerDescription, &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details + ": No error was raised for illegal value.", data_m[1] != FEMC_NO_ERROR);
    // Set the value to 0:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlAttenuation_RCA, callerDescription, &details);
}

void IFSwitchTestFixture::implSetTempServo(AmbRelativeAddr ctrlTempServoEnable_RCA, AmbRelativeAddr getTempServoEnable_RCA, const std::string &callerDescription) {
    string details;
	//turn Servo OFF
	data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlTempServoEnable_RCA, callerDescription, &details);
    SLEEP(1000);
    monitor(getTempServoEnable_RCA, callerDescription, &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    //try illegal value of 2
    data_m[0] = 2;
    dataLength_m = 1;
    command(ctrlTempServoEnable_RCA, callerDescription, &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details + ": No error was raised for illegal value.", data_m[1] != FEMC_NO_ERROR);

    //try illegal value of 0x0F
    data_m[0] = 0x0F;
    dataLength_m = 1;
    command(ctrlTempServoEnable_RCA, callerDescription, &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details + ": No error was raised for illegal value.", data_m[1] != FEMC_NO_ERROR);

    //set Servo ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlTempServoEnable_RCA, callerDescription, &details);
    SLEEP(1000);
    monitor(getTempServoEnable_RCA, callerDescription, &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
}

