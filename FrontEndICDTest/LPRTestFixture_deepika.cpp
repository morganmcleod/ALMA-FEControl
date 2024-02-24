#include "LPRTestFixture_deepika.h"
#include <string>
using namespace std;

const float LPRTestFixture::TempSensorLL = 200.0; //degrees Kelvin
const float LPRTestFixture::TempSensorUL=400.0; //degrees Kelvin
const float LPRTestFixture::LaserDriveCurrentLL=0.0; //mA
const float LPRTestFixture::LaserDriveCurrentUL=250.0; //mA
const float LPRTestFixture::LaserPhotoDetectCurrentLL=-1.0; //micro amps
const float LPRTestFixture::LaserPhotoDetectCurrentUL=2000.0;//micro amps
const float LPRTestFixture::PhotoDetectorCurrentLL=0.0; //mA
const float LPRTestFixture::PhotoDetectorCurrentUL=500.0;//mA
const float LPRTestFixture::PhotoDetectorPowerLL=0.0;//mW
const float LPRTestFixture::PhotoDetectorPowerUL=20.0; //mW
const float LPRTestFixture::LaserPumpTempLL=200.0;//degress Kelvin
const float LPRTestFixture::LaserPumpTempUL=400.0;//degrees Kelvin

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(LPRTestFixture);

void LPRTestFixture::setUp()
{
    AmbDeviceTestFixture::setUp();
    // Write the special monitor point to establish comms between the AMBSI and ARCOM boards:
    monitor(0x20001, "GET_SETUP_INFO", NULL);
}
void LPRTestFixture::tearDown()
{
    AmbDeviceTestFixture::tearDown();
}
void LPRTestFixture::testGET_LPR_OPT_SWITCH_STATE()
{
	string info;
	monitor(opticalSwitchState_RCA, "LPR_OPT_SWITCH_STATE", &info);
	CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 2);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[0] == 0 || data_m[0] == 1);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[1] == FEMC_NO_ERROR);
}
void LPRTestFixture::testGET_LPR_OPT_SWITCH_BUSY()
{
	string info;
	//set switch port
	data_m[0]=6;
	dataLength_m=1;
	command(ctrlopticalSwitchPort_RCA,"SET_LPR_OPT_SWITCH_PORT",&info, false);
    SLEEP(1000);
    data_m[0]=5;
    command(ctrlopticalSwitchPort_RCA,"SET_LPR_OPT_SWITCH_PORT",&info,false);
	SLEEP(5);
	//check if LPR_OPT_SWITCH_BUSY was set.
	monitor(opticalSwitchBusy_RCA,"LPR_OPT_SWITCH_BUSY",&info);
	CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 2);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[0] == 1);
	//monitor switch port.
	monitor(opticalSwitchPort_RCA,"LPR_OPT_SWITCH_PORT",&info);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[0]==5);
	SLEEP(2000);
	//check that opticl switch busy state is set to idle.
	monitor(opticalSwitchBusy_RCA,"LPR_OPT_SWITCH_BUSY",&info);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[0] == 0);
}
void LPRTestFixture::testGET_LPR_TEMP0_TEMP()
{
	implCheckWithinRange(LPRTemperature0_RCA,LPRTestFixture::TempSensorLL,
						 LPRTestFixture::TempSensorUL,"LPR_TEMP0_TEMP");
}
void LPRTestFixture::testGET_LPR_TEMP1_TEMP()
{
	implCheckWithinRange(LPRTemperature1_RCA,LPRTestFixture::TempSensorLL,
						 LPRTestFixture::TempSensorUL,"LPR_TEMP1_TEMP");
}
void LPRTestFixture::testGET_LPR_EDFA_PHOTO_DETECTOR_POWER()
{
	implCheckWithinRange(EDFAPhotoDetectPower_RCA,LPRTestFixture::PhotoDetectorPowerLL,
						 LPRTestFixture::PhotoDetectorPowerUL,"LPR_EDFA_PHOTO_DETECTOR_POWER");
}
void LPRTestFixture::testGET_LPR_EDFA_LASER_PUMP_TEMP()
{
	implCheckWithinRange(EDFALaserPumpTemperature_RCA,LPRTestFixture::LaserPumpTempLL,
						LPRTestFixture::LaserPumpTempUL,"GET_LPR_EDFA_LASER_PUMP_TEMP");
}
void LPRTestFixture::testGET_LPR_EDFA_LASER_DRIVE_CURRENT()
{
	implCheckWithinRange(EDFALaserDriveCurrent_RCA,
					LPRTestFixture::LaserDriveCurrentLL,LPRTestFixture::LaserDriveCurrentUL,
					"LPR_EDFA_LASER_DRIVE_CURRENT");
}
void LPRTestFixture::testGET_LPR_EDFA_LASER_PHOTO_DETECT_CURRENT()
{
	implCheckWithinRange(EDFALaserPhotoDetectCurrent_RCA,
		LPRTestFixture::LaserPhotoDetectCurrentLL,LPRTestFixture::LaserPhotoDetectCurrentUL,
		"LPR_EDFA_LASER_PHOTO_DETECT_CURRENT");
}
void LPRTestFixture::testGET_LPR_EDFA_PHOTO_DETECTOR_CURRENT()
{
	implCheckWithinRange(EDFAPhotoDetectCurrent_RCA,
			LPRTestFixture::PhotoDetectorCurrentLL,LPRTestFixture::PhotoDetectorCurrentUL,
			"LPR_EDFA_PHOTO_DETECTOR_CURRENT");
}

void LPRTestFixture::testSET_LPR_OPTICAL_SWITCH_PORT()
{
	for(unsigned int bandMinusOne=0; bandMinusOne<=10; bandMinusOne++)
	{
		implOpticalSwitchPort(bandMinusOne,"SET_LPR_OPT_SWITCH_PORT");
	}
}
void LPRTestFixture::testSET_LPR_EDFA_MODULATION_INPUT_VALUE()
{
	//Disable optical switch shutter
	string info;
	//data_m[0]=6;
	dataLength_m=1;
	//command(ctrlopticalSwitchPort_RCA,"SET_LPR_OPT_SWITCH_PORT",&info, false);
	monitor(opticalSwitchShutter_RCA,"GET_LPR_OPT_SWITCH_SHUTTER",&info);   //command to close shutter
	CPPUNIT_ASSERT_MESSAGE(info,data_m[0]==0);

	//check SET_LPR_EDFA_MODULATION_INPUT_VALUE using values between 0-5V
	for(int modval=5;modval >=0; modval--)
	{
	    packSGL((float) modval);
		command(ctrlEDFAModulationInput_RCA,"SET_LPR_EDFA_MODULATION_INPUT_VALUE",&info);
		monitor(EDFAModulationInput_RCA,"GET_LPR_EDFA_MODULATION_INPUT_VALUE",&info);

		/// add the set and readback float vals to the info string
		unsigned char statusByte;
		float monValue = unpackSGL(&statusByte);

		std::stringstream streamOut;
		streamOut << info << " Value Set= " << modval << " Readback value=" << monValue;
		string monDetails = streamOut.str();

		CPPUNIT_ASSERT_MESSAGE(monDetails,dataLength_m==5);
        CPPUNIT_ASSERT_MESSAGE(monDetails,statusByte==FEMC_NO_ERROR);
		CPPUNIT_ASSERT_MESSAGE(monDetails,monValue==modval);
	}
}
void LPRTestFixture::testGET_LPR_EDFA_DRIVER_TEMPERATURE_ALARM()
{
	string info;
	monitor(EDFADriverTemperatureAlarm_RCA,"LPR_EDFA_DRIVER_ERROR_TEMPERATURE_ALARM",&info);
	CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 2);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[0] == 0 || data_m[0] == 1);
	CPPUNIT_ASSERT_MESSAGE(info, data_m[1] == FEMC_NO_ERROR);
}
void LPRTestFixture::testSET_OPTICAL_SWITCH_SHUTTER()
{
	//enable opt switch shutter.
	string info;
	data_m[0]=1;
	dataLength_m=1;

	command(ctrlopticalSwitchShutter_RCA,"SET_LPR_OPT_SWITCH_SHUTTER",&info,false);
	SLEEP(2000);
	//check if switch port readout is 0XFF
    monitor(opticalSwitchPort_RCA,"LPR_OPT_SWITCH_PORT", &info);
    CPPUNIT_ASSERT_MESSAGE(info,data_m[0]== 0xFF);
}

void LPRTestFixture::testSET_OPTICAL_SWITCH_FORCE_SHUTTER()
{
    //enable opt switch shutter.
    string info;
    data_m[0]=1;
    dataLength_m=1;

    command(ctrlopticalSwitchForceShutter_RCA,"SET_LPR_OPT_SWITCH_FORCE_SHUTTER",&info,false);
    SLEEP(2000);
    //check if switch port readout is 0XFF
    monitor(opticalSwitchPort_RCA,"LPR_OPT_SWITCH_PORT", &info);
    CPPUNIT_ASSERT_MESSAGE(info,data_m[0]== 0xFF);
}


//helper functions
void LPRTestFixture::implCheckWithinRange(AmbRelativeAddr tempSensor_RCA, const float minVal,
					const float maxVal,const std::string &msg)
{
		string info;
		//set device
		data_m[0]=1;
		dataLength_m=1;

		monitor(tempSensor_RCA,msg,&info);

		//check data length and value in range

		CPPUNIT_ASSERT_MESSAGE(info,dataLength_m==5);
		float num=unpackSGL();
		bool inRange;
		if(num >= minVal && num <= maxVal)
			inRange=true;
		else
			inRange=false;

		std::stringstream streamOut;
		streamOut << info << " Min Value= " << minVal << " Max Value=" << maxVal
				<< " Readout value=" << num;
		string monDetails = streamOut.str();
		CPPUNIT_ASSERT_MESSAGE(monDetails,inRange);
}

void LPRTestFixture::implOpticalSwitchPort(unsigned int bandMinusOne,const std::string &msg)
{
	string info;

	// set optical switch port to given band
	data_m[0] = bandMinusOne;
	dataLength_m = 1;

	command(ctrlopticalSwitchPort_RCA,msg,&info, false);

	/// wait on switch BUSY.
	SLEEP(2000);

	CPPUNIT_ASSERT_MESSAGE(info,dataLength_m==2);
	if (bandMinusOne > 9)
	    CPPUNIT_ASSERT_MESSAGE(info + ": No error was raised for illegal value.", data_m[1] != FEMC_NO_ERROR);

	// now monitor the selected cartridge:
	monitor(opticalSwitchPort_RCA, msg, &info);

	std::stringstream streamOut;
	streamOut << info << " Value Set= " << bandMinusOne << " Readout Value=" << data_m[0];
	string monDetails = streamOut.str();

	//check 1 data byte + 1 status byte
	CPPUNIT_ASSERT_MESSAGE(monDetails, dataLength_m == 2);
	//check range is 0-9
	if (bandMinusOne > 9)
		CPPUNIT_ASSERT_MESSAGE(monDetails, data_m[0] <= 0x09);
	else
		CPPUNIT_ASSERT_MESSAGE(monDetails, data_m[0] == bandMinusOne);

	//check if the shutter was disabled.
	monitor(opticalSwitchShutter_RCA,"GET_LPR_OPT_SWITCH_SHUTTER", &info);
	CPPUNIT_ASSERT_MESSAGE(info + ": Shutter not disabled after port selection",data_m[0]==0);
}










