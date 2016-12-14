#include "PowerSupplyTestFixture.h"
#include <logger.h>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;


const float PowerSupplyTestFixture::ValidMinTemperature = 0.0;
const float PowerSupplyTestFixture::ValidMaxTemperature = 100.0;

CPPUNIT_TEST_SUITE_REGISTRATION(PowerSupplyTestFixture);

PowerSupplyTestFixture::PowerSupplyTestFixture(
		const std::string &name, AmbChannel channel, AmbNodeAddr nodeAddress, bool expectStatusByte)
 	 	 : AmbDeviceTestFixture(name, channel, nodeAddress, expectStatusByte){
}


void PowerSupplyTestFixture::setUp() {
	AmbDeviceTestFixture::setUp();
}
void PowerSupplyTestFixture::tearDown() {
	AmbDeviceTestFixture::tearDown();
}

void PowerSupplyTestFixture::testGET_PLUS5V_VOLTAGE(){
	implGetFloat(RCA_GET_PLUS5V_VOLTAGE, 0, 6, "GET_PLUS5V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_PLUS6V_VOLTAGE(){
	implGetFloat(RCA_GET_PLUS6V_VOLTAGE, 0, 7, "GET_PLUS6V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_PLUS7V_VOLTAGE(){
	implGetFloat(RCA_GET_PLUS7V_VOLTAGE, 0, 8, "GET_PLUS7V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_MINUS7V_VOLTAGE(){
	implGetFloat(RCA_GET_MINUS7V_VOLTAGE, 0, 8, "GET_MINUS7V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_PLUS9V_VOLTAGE(){
	implGetFloat(RCA_GET_PLUS9V_VOLTAGE, 0, 10, "GET_PLUS9V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_PLUS16V_VOLTAGE(){
	implGetFloat(RCA_GET_PLUS16V_VOLTAGE, 0, 17, "GET_PLUS16V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_MINUS16V_VOLTAGE(){
	implGetFloat(RCA_GET_MINUS16V_VOLTAGE, 0, 17, "GET_MINUS16V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_PLUS25V_VOLTAGE(){
	implGetFloat(RCA_GET_PLUS25V_VOLTAGE, 0, 27, "GET_PLUS25V_VOLTAGE");
}
void PowerSupplyTestFixture::testGET_PLUS5V_CURRENT(){
	implGetFloat(RCA_GET_PLUS5V_CURRENT, 0, 5, "GET_PLUS5V_CURRENT");
}
void PowerSupplyTestFixture::testGET_PLUS6V_CURRENT(){
	implGetFloat(RCA_GET_PLUS6V_CURRENT, 0, 5, "GET_PLUS6V_CURRENT");
}
void PowerSupplyTestFixture::testGET_PLUS7V_CURRENT(){
	implGetFloat(RCA_GET_PLUS7V_CURRENT, 0, 20, "GET_PLUS7V_CURRENT");
}
void PowerSupplyTestFixture::testGET_MINUS7V_CURRENT(){
	implGetFloat(RCA_GET_MINUS7V_CURRENT, 0, 2, "GET_MINUS7V_CURRENT");
}
void PowerSupplyTestFixture::testGET_PLUS9V_CURRENT(){
	implGetFloat(RCA_GET_PLUS9V_CURRENT, 0, 10, "GET_PLUS9V_CURRENT");
}
void PowerSupplyTestFixture::testGET_PLUS16V_CURRENT(){
	implGetFloat(RCA_GET_PLUS16V_CURRENT, 0, 10, "GET_PLUS16V_CURRENT");
}
void PowerSupplyTestFixture::testGET_MINUS16V_CURRENT(){
	implGetFloat(RCA_GET_MINUS16V_CURRENT, 0, 5, "GET_MINUS16V_CURRENT");
}
void PowerSupplyTestFixture::testGET_PLUS25V_CURRENT(){
	implGetFloat(RCA_GET_PLUS25V_CURRENT, 0, 5, "GET_PLUS25V_CURRENT");
}


void PowerSupplyTestFixture::testGET_PLUS5V_TEMP(){
	implGetFloat(RCA_GET_PLUS5V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_PLUS5V_TEMP");
}
void PowerSupplyTestFixture::testGET_PLUS6V_TEMP(){
	implGetFloat(RCA_GET_PLUS6V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_PLUS6V_TEMP");
}
void PowerSupplyTestFixture::testGET_PLUS7V_TEMP(){
	implGetFloat(RCA_GET_PLUS7V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_PLUS7V_TEMP");
}
void PowerSupplyTestFixture::testGET_MINUS7V_TEMP(){
	implGetFloat(RCA_GET_MINUS7V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_MINUS7V_TEMP");
}
void PowerSupplyTestFixture::testGET_PLUS9V_TEMP(){
	implGetFloat(RCA_GET_PLUS9V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_PLUS9V_TEMP");
}
void PowerSupplyTestFixture::testGET_PLUS16V_TEMP(){
	implGetFloat(RCA_GET_PLUS16V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_PLUS16V_TEMP");
}
void PowerSupplyTestFixture::testGET_MINUS16V_TEMP(){
	implGetFloat(RCA_GET_MINUS16V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_MINUS16V_TEMP");
}
void PowerSupplyTestFixture::testGET_PLUS25V_TEMP(){
	implGetFloat(RCA_GET_PLUS25V_TEMP, ValidMinTemperature, ValidMaxTemperature, "GET_PLUS25V_TEMP");
}


void PowerSupplyTestFixture::testGET_PLUS5V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_PLUS5V_TEMP_WARN, "GET_PLUS5V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_PLUS6V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_PLUS6V_TEMP_WARN, "GET_PLUS6V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_PLUS7V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_PLUS7V_TEMP_WARN, "GET_PLUS7V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_MINUS7V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_MINUS7V_TEMP_WARN, "GET_MINUS7V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_PLUS9V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_PLUS9V_TEMP_WARN, "GET_PLUS9V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_PLUS16V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_PLUS16V_TEMP_WARN, "GET_PLUS16V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_MINUS16V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_MINUS16V_TEMP_WARN, "GET_MINUS16V_TEMP_WARN");
}
void PowerSupplyTestFixture::testGET_PLUS25V_TEMP_WARN(){
	implGetByte_0_1(RCA_GET_PLUS25V_TEMP_WARN, "GET_PLUS25V_TEMP_WARN");
}

void PowerSupplyTestFixture::testGET_PLUS5V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_PLUS5V_TEMP_ERR, "GET_PLUS5V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_PLUS6V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_PLUS6V_TEMP_ERR, "GET_PLUS6V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_PLUS7V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_PLUS7V_TEMP_ERR, "GET_PLUS7V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_MINUS7V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_MINUS7V_TEMP_ERR, "GET_MINUS7V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_PLUS9V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_PLUS9V_TEMP_ERR, "GET_PLUS9V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_PLUS16V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_PLUS16V_TEMP_ERR, "GET_PLUS16V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_MINUS16V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_MINUS16V_TEMP_ERR, "GET_MINUS16V_TEMP_ERR");
}
void PowerSupplyTestFixture::testGET_PLUS25V_TEMP_ERR(){
	implGetByte_0_1(RCA_GET_PLUS25V_TEMP_ERR, "GET_PLUS25V_TEMP_ERR");
}

void PowerSupplyTestFixture::testGET_PLUS5V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS5V_OVER_VOLT, "GET_PLUS5V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS6V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS6V_OVER_VOLT, "GET_PLUS6V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS7V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS7V_OVER_VOLT, "GET_PLUS7V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_MINUS7V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_MINUS7V_OVER_VOLT, "GET_MINUS7V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS9V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS9V_OVER_VOLT, "GET_PLUS9V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS16V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS16V_OVER_VOLT, "GET_PLUS16V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_MINUS16V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_MINUS16V_OVER_VOLT, "GET_MINUS16V_OVER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS25V_OVER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS25V_OVER_VOLT, "GET_PLUS25V_OVER_VOLT");
}

void PowerSupplyTestFixture::testGET_PLUS5V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_PLUS5V_OVERLOAD, "GET_PLUS5V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_PLUS6V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_PLUS6V_OVERLOAD, "GET_PLUS6V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_PLUS7V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_PLUS7V_OVERLOAD, "GET_PLUS7V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_MINUS7V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_MINUS7V_OVERLOAD, "GET_MINUS7V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_PLUS9V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_PLUS9V_OVERLOAD, "GET_PLUS9V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_PLUS16V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_PLUS16V_OVERLOAD, "GET_PLUS16V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_MINUS16V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_MINUS16V_OVERLOAD, "GET_MINUS16V_OVERLOAD");
}
void PowerSupplyTestFixture::testGET_PLUS25V_OVERLOAD(){
	implGetByte_0_1(RCA_GET_PLUS25V_OVERLOAD, "GET_PLUS25V_OVERLOAD");
}



void PowerSupplyTestFixture::testGET_PLUS5V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS5V_UNDER_VOLT, "GET_PLUS5V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS6V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS6V_UNDER_VOLT, "GET_PLUS6V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS7V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS7V_UNDER_VOLT, "GET_PLUS7V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_MINUS7V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_MINUS7V_UNDER_VOLT, "GET_MINUS7V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS9V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS9V_UNDER_VOLT, "GET_PLUS9V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS16V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS16V_UNDER_VOLT, "GET_PLUS16V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_MINUS16V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_MINUS16V_UNDER_VOLT, "GET_MINUS16V_UNDER_VOLT");
}
void PowerSupplyTestFixture::testGET_PLUS25V_UNDER_VOLT(){
	implGetByte_0_1(RCA_GET_PLUS25V_UNDER_VOLT, "GET_PLUS25V_UNDER_VOLT");
}


void PowerSupplyTestFixture::testGET_PLUS5V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_PLUS5V_COMM_FAIL, "GET_PLUS5V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_PLUS6V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_PLUS6V_COMM_FAIL, "GET_PLUS6V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_PLUS7V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_PLUS7V_COMM_FAIL, "GET_PLUS7V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_MINUS7V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_MINUS7V_COMM_FAIL, "GET_MINUS7V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_PLUS9V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_PLUS9V_COMM_FAIL, "GET_PLUS9V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_PLUS16V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_PLUS16V_COMM_FAIL, "GET_PLUS16V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_MINUS16V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_MINUS16V_COMM_FAIL, "GET_MINUS16V_COMM_FAIL");
}
void PowerSupplyTestFixture::testGET_PLUS25V_COMM_FAIL(){
	implGetByte_0_1(RCA_GET_PLUS25V_COMM_FAIL, "GET_PLUS25V_COMM_FAIL");
}

void PowerSupplyTestFixture::testGET_SYS_INTRLK_A(){
	implGetByte_0_1(RCA_GET_SYS_INTRLK_A, "GET_SYS_INTRLK_A");
}
void PowerSupplyTestFixture::testGET_SYS_INTRLK_B(){
	implGetByte_0_1(RCA_GET_SYS_INTRLK_B, "GET_SYS_INTRLK_B");
}
void PowerSupplyTestFixture::testGET_OUTPUT_STATE(){
	implGetByte_0_1(RCA_GET_OUTPUT_STATE, "GET_OUTPUT_STATE");
}
void PowerSupplyTestFixture::testGET_ID_NAME(){
	implCheckDataLength(RCA_GET_ID_NAME, 8, "GET_ID_NAME");
}
void PowerSupplyTestFixture::testGET_ID_UNIT(){
	implCheckDataLength(RCA_GET_ID_UNIT, 8, "GET_ID_UNIT");
}
void PowerSupplyTestFixture::testGET_ID_SFTW_REV(){
	implCheckDataLength(RCA_GET_ID_SFTW_REV, 2, "GET_ID_SFTW_REV");
}
void PowerSupplyTestFixture::testGET_ID_SER_NUM(){
	implCheckDataLength(RCA_GET_ID_SER_NUM, 1, "GET_ID_SER_NUM");
}
void PowerSupplyTestFixture::testGET_SW_REV(){
	implCheckDataLength(RCA_GET_SW_REV, 3, "GET_SW_REV");
}



/**
 * HELPER FUNCTIONS
 */
void PowerSupplyTestFixture::ENABLE_POWER() {
    //Turn on power
    //LOG(LM_INFO) << "PowerSupplyTestFixture::ENABLE_POWER)" << endl;
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(RCA_SET_ENABLE_PWR, "ENABLE_POWER", NULL);
    SLEEP(10000);
}
void PowerSupplyTestFixture::DISABLE_POWER() {
    //Turn off power
    //LOG(LM_INFO) << "PowerSupplyTestFixture::DISABLE_POWER()" << endl;
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(RCA_SET_ENABLE_PWR, "DISABLE_POWER", NULL);
}
void PowerSupplyTestFixture::implCheckDataLength(
		AmbRelativeAddr monitor_RCA, const float expected_length, const std::string &msg){

	string info,strReturn;
	ostringstream bufReturn;

	bufReturn.str("");
	monitor(monitor_RCA, msg, &info);
	//Check datalength, append monitored value to the message
	bufReturn <<  endl << msg << ", Expected datalength=" << expected_length
			  << ", actual datalength=" << dataLength_m << endl;
	strReturn = bufReturn.str();
	LOG(LM_INFO) << strReturn << endl;
	CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == expected_length);
}

void PowerSupplyTestFixture::implGetFloat(
		AmbRelativeAddr monitor_RCA, const float validmin, const float validmax, const std::string &msg){

	string info,strReturn;
	ostringstream bufReturn;
	float num;

	bufReturn.str("");


	//monitor(monitor_RCA, msg, &info);
	//CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 4);
	//num = unpackSGL();

	num = monitorSGL(monitor_RCA, msg, &info);

	//Append monitored value to the message
	bufReturn <<  "\n" << msg << ", Monitored Value=" << num <<
			",Range=[" << validmin << "," << validmax << "]." << endl;
	strReturn = bufReturn.str();
	//LOG(LM_INFO) << strReturn;

	bool inRange;
	if (   num >= validmin
		&& num <= validmax)
		inRange=true;
	else inRange=false;
	CPPUNIT_ASSERT_MESSAGE(strReturn + "Monitored value out of range.", inRange);
}
void PowerSupplyTestFixture:: implGetByte(AmbRelativeAddr monitor_RCA, const std::string &msg){
	/*
	 * Check to see if monitored value length is 1.
	 */
	string info,strReturn;
	ostringstream bufReturn;
	int num;

	bufReturn.str("");
	num = monitorU8(monitor_RCA, msg, &info);

	//Append monitored value to the message
	bufReturn <<  "\n" << msg << ", Monitored Value=" << data_m[0] << "." << endl;
	strReturn = bufReturn.str();
	//LOG(LM_INFO) << strReturn << endl;

	// check datalength
	CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 1);
}
void PowerSupplyTestFixture:: implGetByte_0_1(AmbRelativeAddr monitor_RCA, const std::string &msg){
/*
 * Check to see if monitored value is 0 or 1.
 */

	string info,strReturn;
	ostringstream bufReturn;
	int num;

	bufReturn.str("");
	//monitor(monitor_RCA, msg, &info);
	num = monitorU8(monitor_RCA, msg, &info);

	//Append monitored value to the message
	bufReturn <<  "\n" << msg << ", Monitored Value=" << num << "." << endl;
	strReturn = bufReturn.str();

	//LOG(LM_INFO) << strReturn << endl;

	//Check to see if value is 0 or 1.
	CPPUNIT_ASSERT_MESSAGE(strReturn, (data_m[0] == 0) || (data_m[0] == 1));
}
