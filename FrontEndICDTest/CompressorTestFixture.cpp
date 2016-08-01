#include "CompressorTestFixture.h"
#include <string>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CompressorTestFixture);

void CompressorTestFixture::setUp()
{
    AmbDeviceTestFixture::setUp();
}
void CompressorTestFixture::tearDown()
{
    AmbDeviceTestFixture::tearDown();
}
void CompressorTestFixture::testSetCompressorON()
{
	implSetRemoteDrive(1);
	SLEEP(2000);
	string details;
	monitor(DRIVE_INDICATION_RCA, "GET_DRIVE_INDICATION", &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
	//check if readout is either 0 or 1
	CPPUNIT_ASSERT_MESSAGE(details, data_m[0]==1);
}
void CompressorTestFixture::testGET_TEMP_1()
{
	//turn on the compressor before testing
	//implSetRemoteDrive(1);
	//SLEEP(2000);
	//monitor temp
	implCheckTemp(TEMP_1_RCA, "GET_TEMP_1");
}
void CompressorTestFixture::testGET_TEMP_2()
{
	implCheckTemp(TEMP_2_RCA, "GET_TEMP_2");
}
void CompressorTestFixture::testGET_TEMP_3()
{
	implCheckTemp(TEMP_3_RCA, "GET_TEMP_3");
}
void CompressorTestFixture::testGET_TEMP_4()
{
	implCheckTemp(TEMP_4_RCA, "GET_TEMP_4");
}
void CompressorTestFixture::testGET_AUX_1()
{
	implCheckAux(AUX_1_RCA,"GET_AUX_1");
}
void CompressorTestFixture::testGET_AUX_2()
{
	implCheckAux(AUX_1_RCA,"GET_AUX_1");
}
void CompressorTestFixture::testGET_PRESSURE()
{
	string details;
	monitor(PRESSURE_RCA, "GET_PRESSURE", &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);

	char statusByte;
	float monValue = unpackSGL(&statusByte);

	std::stringstream streamOut;
	streamOut << details << " Readback value=" << monValue;
	string monDetails = streamOut.str();
	//check if readout is between 0-4 MPa
	CPPUNIT_ASSERT_MESSAGE(monDetails, monValue >=0 && monValue <=4);
}
void CompressorTestFixture::testGET_PRESSURE_ALARM()
{
	implGetAlarm(PRESSURE_ALARM_RCA,"GET_PRESSURE_ALARM");
}
void CompressorTestFixture::testGET_TEMP_AMARM()
{
	implGetAlarm(TEMP_ALARM_RCA,"GET_TEMP_ALARM");
}
void CompressorTestFixture::testSetCompressorOFF()
{
	implSetRemoteDrive(0);
	SLEEP(2000);
	string details;
	monitor(DRIVE_INDICATION_RCA, "GET_DRIVE_INDICATION", &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
	//check if readout is either 0 or 1
	CPPUNIT_ASSERT_MESSAGE(details, data_m[0]==0);

	//SLEEP(2000);
}
//helper functions
void CompressorTestFixture::implSetRemoteDrive(int stat)
{
	string details;

	data_m[0]=stat;
	dataLength_m=1;

	command(REMOTE_DRIVE_RCA,"SET_REMOTE_DRIVE",&details);
}
void CompressorTestFixture::implCheckTemp(AmbRelativeAddr monitor_RCA,const std::string &callerDescription)
{
	string details;

	monitor(monitor_RCA,callerDescription, &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);

	char statusByte;
	float monValue = unpackSGL(&statusByte);

	std::stringstream streamOut;
	streamOut << details << " Readback value=" << monValue;
	string monDetails = streamOut.str();
	//check if readout is between -30 to 60 deg C
	CPPUNIT_ASSERT_MESSAGE(monDetails, monValue >= -30 && monValue <= 60);
}
void CompressorTestFixture::implCheckAux(AmbRelativeAddr monitor_RCA,const std::string &callerDescription)
{
	string details;
	monitor(monitor_RCA,callerDescription, &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);

	char statusByte;
	float monValue = unpackSGL(&statusByte);

	std::stringstream streamOut;
	streamOut << details << " Readback value=" << monValue;
	string monDetails = streamOut.str();
	//check if readout is between 0 to 5V
	CPPUNIT_ASSERT_MESSAGE(monDetails, monValue >= 0 && monValue <= 5);
}
void CompressorTestFixture::implGetAlarm(AmbRelativeAddr monitor_RCA,const std::string &callerDescription)
{
	string details;
	monitor(PRESSURE_ALARM_RCA, "GET_PRESSURE_ALARM", &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
	//check if readout is either 0 or 1
	CPPUNIT_ASSERT_MESSAGE(details, data_m[0]==0 || data_m[0] == 1);
}
