#include "CPDSTestFixture_deepika.h"
#include <string>
#include <math.h>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CPDSTestFixture);

void CPDSTestFixture::setUp()
{
    AmbDeviceTestFixture::setUp();
}
void CPDSTestFixture::tearDown()
{
    AmbDeviceTestFixture::tearDown();
}
void CPDSTestFixture::testGET_POWER_DISTRIBUTION_POWERERED_MODULES()
{
	string details;
	monitor(0xA0A0, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
	CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
	CPPUNIT_ASSERT_MESSAGE(details, data_m[0] <= 3);    // valid range [0123]
	CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE1_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND1,CTRL_ENABLE_BAND1,1,"POWER_DISTRIBUTION_MODULE1_ENABLE");
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE3_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND3,CTRL_ENABLE_BAND3,3,"POWER_DISTRIBUTION_MODULE3_ENABLE");
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE4_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND4,CTRL_ENABLE_BAND4,4,"POWER_DISTRIBUTION_MODULE4_ENABLE");
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE6_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND6,CTRL_ENABLE_BAND6,6,"POWER_DISTRIBUTION_MODULE6_ENABLE");
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE7_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND7,CTRL_ENABLE_BAND7,7,"POWER_DISTRIBUTION_MODULE7_ENABLE");
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE8_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND8,CTRL_ENABLE_BAND8,8,"POWER_DISTRIBUTION_MODULE8_ENABLE");
}
void CPDSTestFixture::testSET_POWER_DISTRIBUTION_MODULE9_ENABLE()
{
	implPwrDistributionModule(MON_ENABLE_BAND9,CTRL_ENABLE_BAND9,9,"POWER_DISTRIBUTION_MODULE9_ENABLE");
}
void CPDSTestFixture::implPwrDistributionModule(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
		unsigned int bandNum,const std::string &callerDescription)
{


    AmbRelativeAddr baseRCA = 0xA000;

    AmbRelativeAddr bandRCA = bandNum - 1;
	bandRCA <<= 4;
	bandRCA += baseRCA;

	AmbRelativeAddr voltageP6V_RCA  = bandRCA + VOLTAGE_P6V;
	AmbRelativeAddr currentP6V_RCA  = bandRCA + CURRENT_P6V;
	AmbRelativeAddr voltageN6V_RCA  = bandRCA + VOLTAGE_N6V;
	AmbRelativeAddr currentN6V_RCA  = bandRCA + CURRENT_N6V;
	AmbRelativeAddr voltageP15V_RCA = bandRCA + VOLTAGE_P15V;
	AmbRelativeAddr currentP15V_RCA = bandRCA + CURRENT_P15V;
	AmbRelativeAddr voltageN15V_RCA = bandRCA + VOLTAGE_N15V;
	AmbRelativeAddr currentN15V_RCA = bandRCA + CURRENT_N15V;
	AmbRelativeAddr voltageP24V_RCA = bandRCA + VOLTAGE_P24V;
	AmbRelativeAddr currentP24V_RCA = bandRCA + CURRENT_N24V;
	AmbRelativeAddr voltageP8V_RCA  = bandRCA + VOLTAGE_P8V;
	AmbRelativeAddr currentP8V_RCA  = bandRCA + CURRENT_P8V;
	//AmbRelativeAddr enableModule_RCA= bandRCA + ENABLE_MODULE;

	string details;
	//check if all bands are disabled
	monitor(0xA0A0, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
	//if all bands not disabled then disable them.
	if(data_m[0] != 0)
	{
	    data_m[0]=0;
	    dataLength_m=1;

	    for (unsigned band = 0; band < 10; ++band) {
	        commandImpl(CTRL_ENABLE_BAND1 + (band << 4), "SET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details);
	        SLEEP(2000);
	    }
	}

	//check again to make sure all bands are disabled
	monitor(0xA0A0, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
	CPPUNIT_ASSERT_MESSAGE(details,data_m[0]==0);

	//check if all voltage and current readouts are close to 0
	if(bandNum != 1)
	{
		checkCurrentandVoltage(voltageP6V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL0_VOLTAGE");
		checkCurrentandVoltage(currentP6V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL0_CURRENT");
		checkCurrentandVoltage(voltageN6V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL1_VOLTAGE");
		checkCurrentandVoltage(currentN6V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL1_CURRENT");
		checkCurrentandVoltage(voltageP15V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL2_VOLTAGE");
		checkCurrentandVoltage(currentP15V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL2_CURRENT");
		checkCurrentandVoltage(voltageN15V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL3_VOLTAGE");
		checkCurrentandVoltage(currentN15V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL3_CURRENT");
		checkCurrentandVoltage(voltageP24V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL4_VOLTAGE");
		checkCurrentandVoltage(currentP24V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL4_CURRENT");
		checkCurrentandVoltage(voltageP8V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL5_VOLTAGE");
		checkCurrentandVoltage(currentP8V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL5_CURRENT");
	}
	//now enable the band
	data_m[0]=1;
	dataLength_m=1;

	commandImpl(control_RCA,callerDescription, &details);
	SLEEP(2000);
	monitor(control_RCA,callerDescription + " monitor after 2s wait.", &details);

	monitor(monitor_RCA,callerDescription,&details);
	CPPUNIT_ASSERT_MESSAGE(details,dataLength_m==2);
	CPPUNIT_ASSERT_MESSAGE(details,data_m[0]==1);

	if (bandNum!=1) {
        //monitor the 6 currents and 6 voltages
        checkVoltageWhenBandSet(voltageP6V_RCA,6.0,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL0_VOLTAGE");
        checkVoltageWhenBandSet(voltageN6V_RCA,-6.0,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL1_VOLTAGE");
        checkVoltageWhenBandSet(voltageP15V_RCA,15.0,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL2_VOLTAGE");
        checkVoltageWhenBandSet(voltageN15V_RCA,-15.0,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL3_VOLTAGE");
        checkVoltageWhenBandSet(voltageP24V_RCA,24.0,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL4_VOLTAGE");
        checkVoltageWhenBandSet(voltageP8V_RCA,8.0,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL5_VOLTAGE");
	}
	//assume that only band 3 is installed
	if (bandNum==3)
	{
		checkCurrentWhenBandSet(currentP6V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL0_CURRENT");
		checkCurrentWhenBandSet(currentN6V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL1_CURRENT");
		checkCurrentWhenBandSet(currentP15V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL2_CURRENT");
		checkCurrentWhenBandSet(currentN15V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL3_CURRENT");
		checkCurrentWhenBandSet(currentP24V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL4_CURRENT");
		checkCurrentWhenBandSet(currentP8V_RCA,"GET_POWER_DISTRIBUTION_MODULE_CHANNEL5_CURRENT");
	}
	//disable the band
	data_m[0]=0;
    dataLength_m=1;
	commandImpl(control_RCA,callerDescription, &details);
}

void CPDSTestFixture::checkCurrentandVoltage(AmbRelativeAddr monitor_RCA,const std::string &callerDescription)
{
	string details;
	monitor(monitor_RCA,callerDescription,&details);
	CPPUNIT_ASSERT_MESSAGE(details,dataLength_m==5);

	unsigned char statusByte;
	float monValue = unpackSGL(&statusByte);

	std::stringstream streamOut;
	streamOut << details << " Readout Value=" << monValue;
	string moreDetails = streamOut.str();

	//check if read out is close to zero
	CPPUNIT_ASSERT_MESSAGE(moreDetails,monValue <= 2.0 && monValue >= -2.0);
}

void CPDSTestFixture::checkCurrentWhenBandSet(AmbRelativeAddr monitor_RCA,const std::string &callerDescription)
{
	string details;
	monitor(monitor_RCA,callerDescription,&details);
	CPPUNIT_ASSERT_MESSAGE(details,dataLength_m==5);

	unsigned char statusByte;
	float monValue = unpackSGL(&statusByte);

	std::stringstream streamOut;
	streamOut << details << " Readout Value=" << monValue;
	string moreDetails = streamOut.str();

	//check if current is non zero
	CPPUNIT_ASSERT_MESSAGE(moreDetails,monValue != 0);
}
void CPDSTestFixture::checkVoltageWhenBandSet(AmbRelativeAddr monitor_RCA,float volt,const std::string &callerDescription)
{
	string details;

	float u_volt=volt + fabs((volt*10)/100);
	float l_volt=volt - fabs((volt*10)/100);

	monitor(monitor_RCA,callerDescription,&details);
	CPPUNIT_ASSERT_MESSAGE(details,dataLength_m==5);

	unsigned char statusByte;
	float monValue = unpackSGL(&statusByte);

	std::stringstream streamOut;
	streamOut << details << " Nominal value=" << volt << " Readout Value=" << monValue
	          << " Upper limit=" << u_volt << " Lower limit=" << l_volt;
	string moreDetails = streamOut.str();

	//check if voltage is within 10% of nominal value.
	CPPUNIT_ASSERT_MESSAGE(moreDetails,monValue <= u_volt && monValue >= l_volt);
}
