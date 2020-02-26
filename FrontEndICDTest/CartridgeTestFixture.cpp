/*
 * CartridgeTestFixture.cpp
 *
 *  Created on: Sep 29, 2010
 *      Author: jcrabtre
 */

#include "CartridgeTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <logger.h>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

const bool CartridgeTestFixture::checkIllegalFloatCommands(false);
///< see header for description of why these checks are skipped.


CartridgeTestFixture::CartridgeTestFixture(int band, const std::string &name, AmbChannel channel, AmbNodeAddr nodeAddress, bool expectStatusByte)
  : AmbDeviceTestFixture(name, channel, nodeAddress, expectStatusByte),
    band_m(0),
    isEnabled_m(false),
    cartPowerRCA_m(0),
    cartBaseRCA_m(0)
{
    if (band >= 1 && band <= 10) {
        band_m = band;

        // the RCA to enable/disable the cartridge is 0x1A0nC where n is band-1:
        cartPowerRCA_m = band_m - 1;
        cartPowerRCA_m <<= 4;
        cartPowerRCA_m += 0x1A00C;

        // the offset for all cartridge RCAs is 0x0n000 when n is band-1:
        cartBaseRCA_m = band_m - 1;
        cartBaseRCA_m <<= 12;
    }
}


void CartridgeTestFixture::setUp() {
	AmbDeviceTestFixture::setUp();
}
void CartridgeTestFixture::tearDown() {
	AmbDeviceTestFixture::tearDown();
}

void CartridgeTestFixture::ENABLE_CARTRIDGE() {
    //Turn on cartridge
    if (band_m > 0) {
        LOG(LM_INFO) << "CartridgeTestFixture::ENABLE_CARTRIDGE()" << endl;
        data_m[0] = 1;
        dataLength_m = 1;
        commandImpl(cartPowerRCA_m, "ENABLE_CARTRIDGE", NULL);
        SLEEP(2000);
        isEnabled_m = true;
    } else
        LOG(LM_ERROR) << "CartridgeTestFixture::ENABLE_CARTRIDGE not a valid cartridge band." << endl;
}

void CartridgeTestFixture::DISABLE_CARTRIDGE() {
    //turn off cartridge
    if (band_m > 0) {
        LOG(LM_INFO) << "CartridgeTestFixture::DISABLE_CARTRIDGE()" << endl;
        data_m[0] = 0;
        dataLength_m = 1;
        commandImpl(cartPowerRCA_m, "ENABLE_CARTRIDGE", NULL);
        SLEEP(2000);
        isEnabled_m = false;
    } else
        LOG(LM_ERROR) << "CartridgeTestFixture::DISABLE_CARTRIDGE not a valid cartridge band." << endl;
}

/*******************
 * Helper functions
 *******************/
void CartridgeTestFixture::implToggleEnable(
	AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
	int SetVal, const std::string &callerDescription){

	string info,strSet,strReturn;
	ostringstream bufSet,bufReturn;

	data_m[0] = SetVal;
	dataLength_m = 1;

	bufSet <<  callerDescription << ", \nSetVal=" << SetVal << ".";
	strSet = bufSet.str();

	switch (SetVal) {
	case 0:
		command(control_RCA, strSet, &info);
		monitor(monitor_RCA, strSet, &info);
		//Add monitored value to info string
		bufReturn <<  info << ", MonitoredVal=" << data_m[0] << ".";
		strReturn = bufReturn.str();
		CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 2);
		CPPUNIT_ASSERT_MESSAGE(strReturn, data_m[0] == 0);
	  break;
	case 1:
		command(control_RCA, strSet, &info);
		monitor(monitor_RCA, strSet, &info);
		//Add monitored value to info string
		bufReturn <<  callerDescription << ", MonitoredVal=" << data_m[0] << ".";
		strReturn = bufReturn.str();
		CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 2);
		CPPUNIT_ASSERT_MESSAGE(strReturn, data_m[0] == 1);
	  break;

	default:
		//for illegal values
		command(control_RCA, strSet, &strSet, false);
		//Add monitored value to info string
		bufReturn <<  callerDescription << ", MonitoredVal=" << data_m[0] << ".";
		strReturn = bufReturn.str();
		CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 2);
		CPPUNIT_ASSERT_MESSAGE(strReturn + ": No error was raised for illegal value.", data_m[1] == FEMC_NO_ERROR);
	  break;
	}
}

void CartridgeTestFixture::implGenericTest(
		AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
		std::vector<float> &test_values,
		const float validmin, const float validmax, const float tolerance,
		const std::string &callerDescription){

	string info,strSet,strReturn;
	ostringstream bufSet,bufReturn;
	bool InValidRange;
	unsigned int i;
	float OriginalMonitorValue;
	unsigned char statusByte;

	//LOG(LM_INFO) << "Number of elements in array=" << test_values.size() << endl;

	//Loop over test_values array
	//for (i=0;i<(sizeof(test_values) / sizeof (test_values[0]));i++){
	for (i=0;i< test_values.size();i++){
		InValidRange = (test_values[i] >= validmin) && (test_values[i] <= validmax);
		resetAmbVars();
		//LOG(LM_INFO) << "test_values[i]=" << test_values[i] << endl;

		//Set 0 initially
		packSGL(0);
		commandImpl(control_RCA, "initially setting to zero.\n", &info);
		SLEEP(100);
		monitor(monitor_RCA, strSet, &info);
		OriginalMonitorValue = unpackSGL();

		info= "";
		bufSet.str("");
		bufReturn.str("");

		//Append set value to the message
		bufSet <<  "\n" << callerDescription << ", SetValue=" << test_values[i] <<
				",Range=[" << validmin << "," << validmax << "].";
		strSet = bufSet.str();

		//set control value
		packSGL(test_values[i]);
		command(control_RCA, strSet, &strSet, false);
		SLEEP(100);

		//assert correct datalength
		CPPUNIT_ASSERT_MESSAGE(strSet, dataLength_m == 5);
		//get statusbyte
		unpackSGL(&statusByte);

		//If SetValue is illegal, cache the result for the final check at the end.
		if (InValidRange){
			//If SetValue is legal
			CPPUNIT_ASSERT_MESSAGE(strSet, statusByte == FEMC_NO_ERROR);
		}
		else{
		    if (checkIllegalFloatCommands){
		        //If SetValue is illegal
		        CPPUNIT_ASSERT_MESSAGE(strSet + ": Error not raised for illegal set value.", statusByte != FEMC_NO_ERROR);
		    }
		}
		//read monitor point
		monitor(monitor_RCA, strSet, &strSet);

		//assert correct datalength
		CPPUNIT_ASSERT_MESSAGE(strSet, dataLength_m == 5);

		float FinalMonitorValue(unpackSGL(&statusByte));
		//LOG(LM_INFO) << hex << monitor_RCA << endl;
		//LOG(LM_INFO) << "Target Value=" << test_values[i] << endl;
		//LOG(LM_INFO) << "Monitored Value=" << FinalMonitorValue << endl << endl;


		bufReturn <<  info << " \nSetValue= " << test_values[i] << ", OriginalMonitorValue="
				<< OriginalMonitorValue << ", FinalMonitoredValue="
				<< FinalMonitorValue << ",Range=[" << validmin << "," << validmax << "], Tolerance="
				<< tolerance << ".\n";
		strReturn = bufReturn.str();


		if (!InValidRange){
			//if SetValue is illegal
		    if (checkIllegalFloatCommands) {
		        //assert unchanged within tolerance
		        CPPUNIT_ASSERT_MESSAGE(strReturn + ": Illegal value not ignored as expected.", fabs(OriginalMonitorValue - FinalMonitorValue) <= tolerance);
		    }
		}
		else{
			//if SetValue is legal
			//assert that monitored value is close enough to the target value
			CPPUNIT_ASSERT_MESSAGE(strReturn + ": Monitored value did not change after value set.", OriginalMonitorValue != FinalMonitorValue);
		}
	}//end for loop

	//Set to 0 after test has been performed
	packSGL(0);
	info = "";
	commandImpl(control_RCA, "Setting to zero after performing test.\n", &info);
	SLEEP(100);
}


void CartridgeTestFixture::implGetFloat( AmbRelativeAddr monitor_RCA, const float validmin, const float validmax, const std::string &callerDescription) {

    unsigned char statusByte;
	string info,strReturn;
	ostringstream bufReturn;

	bufReturn.str("");

	monitor(monitor_RCA, callerDescription, &info);
	// check datalength and value in range
	CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 5);
	float num(unpackSGL(&statusByte));

	//Append monitored value to the message
	bufReturn <<  "\n" << callerDescription << ", Monitored Value=" << num <<
			",Range=[" << validmin << "," << validmax << "]." << endl;
	strReturn = bufReturn.str();

	bool inRange;
	if (   num >= validmin
		&& num <= validmax)
		inRange=true;
	else inRange=false;
	CPPUNIT_ASSERT_MESSAGE(strReturn + "Monitored value out of range.", inRange);
}

