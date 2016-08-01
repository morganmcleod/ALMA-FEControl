#ifndef POWERSUPPLYTESTFIXTURE_H_
#define POWERSUPPLYTESTFIXTURE_H_
#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>


class PowerSupplyTestFixture : public AmbDeviceTestFixture {
	CPPUNIT_TEST_SUITE(PowerSupplyTestFixture);
	CPPUNIT_TEST(ENABLE_POWER);

    CPPUNIT_TEST(testGET_PLUS5V_VOLTAGE);
	CPPUNIT_TEST(testGET_PLUS6V_VOLTAGE);
	CPPUNIT_TEST(testGET_PLUS7V_VOLTAGE);
	CPPUNIT_TEST(testGET_MINUS7V_VOLTAGE);
	CPPUNIT_TEST(testGET_PLUS9V_VOLTAGE);
	CPPUNIT_TEST(testGET_PLUS16V_VOLTAGE);
	CPPUNIT_TEST(testGET_MINUS16V_VOLTAGE);
	CPPUNIT_TEST(testGET_PLUS25V_VOLTAGE);
	CPPUNIT_TEST(testGET_PLUS5V_CURRENT);
	CPPUNIT_TEST(testGET_PLUS6V_CURRENT);
	CPPUNIT_TEST(testGET_PLUS7V_CURRENT);
	CPPUNIT_TEST(testGET_MINUS7V_CURRENT);
	CPPUNIT_TEST(testGET_PLUS9V_CURRENT);
	CPPUNIT_TEST(testGET_PLUS16V_CURRENT);
	CPPUNIT_TEST(testGET_MINUS16V_CURRENT);
	CPPUNIT_TEST(testGET_PLUS25V_CURRENT);

	CPPUNIT_TEST(testGET_PLUS5V_TEMP);
	CPPUNIT_TEST(testGET_PLUS6V_TEMP);
	CPPUNIT_TEST(testGET_PLUS7V_TEMP);
	CPPUNIT_TEST(testGET_MINUS7V_TEMP);
	CPPUNIT_TEST(testGET_PLUS9V_TEMP);
	CPPUNIT_TEST(testGET_PLUS16V_TEMP);
	CPPUNIT_TEST(testGET_MINUS16V_TEMP);
	CPPUNIT_TEST(testGET_PLUS25V_TEMP);

	CPPUNIT_TEST(testGET_PLUS5V_TEMP_WARN);
	CPPUNIT_TEST(testGET_PLUS6V_TEMP_WARN);
	CPPUNIT_TEST(testGET_PLUS7V_TEMP_WARN);
	CPPUNIT_TEST(testGET_MINUS7V_TEMP_WARN);
	CPPUNIT_TEST(testGET_PLUS9V_TEMP_WARN);
	CPPUNIT_TEST(testGET_PLUS16V_TEMP_WARN);
	CPPUNIT_TEST(testGET_MINUS16V_TEMP_WARN);
	CPPUNIT_TEST(testGET_PLUS25V_TEMP_WARN);

	CPPUNIT_TEST(testGET_PLUS5V_TEMP_ERR);
	CPPUNIT_TEST(testGET_PLUS6V_TEMP_ERR);
	CPPUNIT_TEST(testGET_PLUS7V_TEMP_ERR);
	CPPUNIT_TEST(testGET_MINUS7V_TEMP_ERR);
	CPPUNIT_TEST(testGET_PLUS9V_TEMP_ERR);
	CPPUNIT_TEST(testGET_PLUS16V_TEMP_ERR);
	CPPUNIT_TEST(testGET_MINUS16V_TEMP_ERR);
	CPPUNIT_TEST(testGET_PLUS25V_TEMP_ERR);

	CPPUNIT_TEST(testGET_PLUS5V_OVER_VOLT);
	CPPUNIT_TEST(testGET_PLUS6V_OVER_VOLT);
	CPPUNIT_TEST(testGET_PLUS7V_OVER_VOLT);
	CPPUNIT_TEST(testGET_MINUS7V_OVER_VOLT);
	CPPUNIT_TEST(testGET_PLUS9V_OVER_VOLT);
	CPPUNIT_TEST(testGET_PLUS16V_OVER_VOLT);
	CPPUNIT_TEST(testGET_MINUS16V_OVER_VOLT);
	CPPUNIT_TEST(testGET_PLUS25V_OVER_VOLT);

	CPPUNIT_TEST(testGET_PLUS5V_OVERLOAD);
	CPPUNIT_TEST(testGET_PLUS6V_OVERLOAD);
	CPPUNIT_TEST(testGET_PLUS7V_OVERLOAD);
	CPPUNIT_TEST(testGET_MINUS7V_OVERLOAD);
	CPPUNIT_TEST(testGET_PLUS9V_OVERLOAD);
	CPPUNIT_TEST(testGET_PLUS16V_OVERLOAD);
	CPPUNIT_TEST(testGET_MINUS16V_OVERLOAD);
	CPPUNIT_TEST(testGET_PLUS25V_OVERLOAD);

	CPPUNIT_TEST(testGET_PLUS5V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_PLUS6V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_PLUS7V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_MINUS7V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_PLUS9V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_PLUS16V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_MINUS16V_UNDER_VOLT);
	CPPUNIT_TEST(testGET_PLUS25V_UNDER_VOLT);

	CPPUNIT_TEST(testGET_PLUS5V_COMM_FAIL);
	CPPUNIT_TEST(testGET_PLUS6V_COMM_FAIL);
	CPPUNIT_TEST(testGET_PLUS7V_COMM_FAIL);
	CPPUNIT_TEST(testGET_MINUS7V_COMM_FAIL);
	CPPUNIT_TEST(testGET_PLUS9V_COMM_FAIL);
	CPPUNIT_TEST(testGET_PLUS16V_COMM_FAIL);
	CPPUNIT_TEST(testGET_MINUS16V_COMM_FAIL);
	CPPUNIT_TEST(testGET_PLUS25V_COMM_FAIL);

	CPPUNIT_TEST(testGET_SYS_INTRLK_A);
	CPPUNIT_TEST(testGET_SYS_INTRLK_B);
	CPPUNIT_TEST(testGET_OUTPUT_STATE);
	CPPUNIT_TEST(testGET_ID_NAME);
	CPPUNIT_TEST(testGET_ID_UNIT);
	CPPUNIT_TEST(testGET_ID_SFTW_REV);
	CPPUNIT_TEST(testGET_ID_SER_NUM);
	CPPUNIT_TEST(testGET_SW_REV);


	CPPUNIT_TEST(DISABLE_POWER);
	CPPUNIT_TEST_SUITE_END();

public:
	PowerSupplyTestFixture(
	                        const std::string &name = std::string("PowerSupplyTestFixture"),
	                        AmbChannel channel = 0,
	                        AmbNodeAddr nodeAddress = 0x26,
	                        bool expectStatusByte = false);

    void setUp();
    void tearDown();

	void testGET_PLUS5V_VOLTAGE();
	void testGET_PLUS6V_VOLTAGE();
	void testGET_PLUS7V_VOLTAGE();
	void testGET_MINUS7V_VOLTAGE();
	void testGET_PLUS9V_VOLTAGE();
	void testGET_PLUS16V_VOLTAGE();
	void testGET_MINUS16V_VOLTAGE();
	void testGET_PLUS25V_VOLTAGE();
	void testGET_PLUS5V_CURRENT();
	void testGET_PLUS6V_CURRENT();
	void testGET_PLUS7V_CURRENT();
	void testGET_MINUS7V_CURRENT();
	void testGET_PLUS9V_CURRENT();
	void testGET_PLUS16V_CURRENT();
	void testGET_MINUS16V_CURRENT();
	void testGET_PLUS25V_CURRENT();

	void testGET_PLUS5V_TEMP();
	void testGET_PLUS6V_TEMP();
	void testGET_PLUS7V_TEMP();
	void testGET_MINUS7V_TEMP();
	void testGET_PLUS9V_TEMP();
	void testGET_PLUS16V_TEMP();
	void testGET_MINUS16V_TEMP();
	void testGET_PLUS25V_TEMP();

	void testGET_PLUS5V_TEMP_WARN();
	void testGET_PLUS6V_TEMP_WARN();
	void testGET_PLUS7V_TEMP_WARN();
	void testGET_MINUS7V_TEMP_WARN();
	void testGET_PLUS9V_TEMP_WARN();
	void testGET_PLUS16V_TEMP_WARN();
	void testGET_MINUS16V_TEMP_WARN();
	void testGET_PLUS25V_TEMP_WARN();

	void testGET_PLUS5V_TEMP_ERR();
	void testGET_PLUS6V_TEMP_ERR();
	void testGET_PLUS7V_TEMP_ERR();
	void testGET_MINUS7V_TEMP_ERR();
	void testGET_PLUS9V_TEMP_ERR();
	void testGET_PLUS16V_TEMP_ERR();
	void testGET_MINUS16V_TEMP_ERR();
	void testGET_PLUS25V_TEMP_ERR();

	void testGET_PLUS5V_OVER_VOLT();
	void testGET_PLUS6V_OVER_VOLT();
	void testGET_PLUS7V_OVER_VOLT();
	void testGET_MINUS7V_OVER_VOLT();
	void testGET_PLUS9V_OVER_VOLT();
	void testGET_PLUS16V_OVER_VOLT();
	void testGET_MINUS16V_OVER_VOLT();
	void testGET_PLUS25V_OVER_VOLT();

	void testGET_PLUS5V_OVERLOAD();
	void testGET_PLUS6V_OVERLOAD();
	void testGET_PLUS7V_OVERLOAD();
	void testGET_MINUS7V_OVERLOAD();
	void testGET_PLUS9V_OVERLOAD();
	void testGET_PLUS16V_OVERLOAD();
	void testGET_MINUS16V_OVERLOAD();
	void testGET_PLUS25V_OVERLOAD();

	void testGET_PLUS5V_UNDER_VOLT();
	void testGET_PLUS6V_UNDER_VOLT();
	void testGET_PLUS7V_UNDER_VOLT();
	void testGET_MINUS7V_UNDER_VOLT();
	void testGET_PLUS9V_UNDER_VOLT();
	void testGET_PLUS16V_UNDER_VOLT();
	void testGET_MINUS16V_UNDER_VOLT();
	void testGET_PLUS25V_UNDER_VOLT();

	void testGET_PLUS5V_COMM_FAIL();
	void testGET_PLUS6V_COMM_FAIL();
	void testGET_PLUS7V_COMM_FAIL();
	void testGET_MINUS7V_COMM_FAIL();
	void testGET_PLUS9V_COMM_FAIL();
	void testGET_PLUS16V_COMM_FAIL();
	void testGET_MINUS16V_COMM_FAIL();
	void testGET_PLUS25V_COMM_FAIL();

	void testGET_SYS_INTRLK_A();
	void testGET_SYS_INTRLK_B();
	void testGET_OUTPUT_STATE();
	void testGET_ID_NAME();
	void testGET_ID_UNIT();
	void testGET_ID_SFTW_REV();
	void testGET_ID_SER_NUM();
	void testGET_SW_REV();



    void implGetFloat(AmbRelativeAddr monitor_RCA,const float validmin,
    				    const float validmax, const std::string &msg);
    void implCheckDataLength(
    		AmbRelativeAddr monitor_RCA, const float expected_length, const std::string &msg);
    void implGetByte(AmbRelativeAddr monitor_RCA, const std::string &msg);
    void implGetByte_0_1(AmbRelativeAddr monitor_RCA, const std::string &msg);
    void ENABLE_POWER();
    void DISABLE_POWER();


private:
    enum MonitorControlOffset {
    	   //Monitor points
    	   RCA_GET_PLUS5V_VOLTAGE             = 0x00001,
    	   RCA_GET_PLUS6V_VOLTAGE             = 0x00002,
    	   RCA_GET_PLUS7V_VOLTAGE             = 0x00003,
    	   RCA_GET_MINUS7V_VOLTAGE            = 0x00004,
    	   RCA_GET_PLUS9V_VOLTAGE             = 0x00005,
    	   RCA_GET_PLUS16V_VOLTAGE            = 0x00006,
    	   RCA_GET_MINUS16V_VOLTAGE           = 0x00007,
    	   RCA_GET_PLUS25V_VOLTAGE            = 0x00008,
    	   RCA_GET_PLUS5V_CURRENT             = 0x00009,
    	   RCA_GET_PLUS6V_CURRENT             = 0x0000A,
    	   RCA_GET_PLUS7V_CURRENT             = 0x0000B,
    	   RCA_GET_MINUS7V_CURRENT            = 0x0000C,
    	   RCA_GET_PLUS9V_CURRENT             = 0x0000D,
    	   RCA_GET_PLUS16V_CURRENT            = 0x0000E,
    	   RCA_GET_MINUS16V_CURRENT           = 0x0000F,
    	   RCA_GET_PLUS25V_CURRENT            = 0x00010,

    	   RCA_GET_PLUS5V_TEMP                = 0x00011,
    	   RCA_GET_PLUS6V_TEMP                = 0x00012,
    	   RCA_GET_PLUS7V_TEMP                = 0x00013,
    	   RCA_GET_MINUS7V_TEMP               = 0x00014,
    	   RCA_GET_PLUS9V_TEMP                = 0x00015,
    	   RCA_GET_PLUS16V_TEMP               = 0x00016,
    	   RCA_GET_MINUS16V_TEMP              = 0x00017,
    	   RCA_GET_PLUS25V_TEMP               = 0x00018,

    	   RCA_GET_PLUS5V_TEMP_WARN           = 0x00019,
    	   RCA_GET_PLUS6V_TEMP_WARN           = 0x0001A,
    	   RCA_GET_PLUS7V_TEMP_WARN           = 0x0001B,
    	   RCA_GET_MINUS7V_TEMP_WARN          = 0x0001C,
    	   RCA_GET_PLUS9V_TEMP_WARN           = 0x0001D,
    	   RCA_GET_PLUS16V_TEMP_WARN          = 0x0001E,
    	   RCA_GET_MINUS16V_TEMP_WARN         = 0x0001F,
    	   RCA_GET_PLUS25V_TEMP_WARN          = 0x00020,

    	   RCA_GET_PLUS5V_TEMP_ERR            = 0x00021,
    	   RCA_GET_PLUS6V_TEMP_ERR            = 0x00022,
    	   RCA_GET_PLUS7V_TEMP_ERR            = 0x00023,
    	   RCA_GET_MINUS7V_TEMP_ERR           = 0x00024,
    	   RCA_GET_PLUS9V_TEMP_ERR            = 0x00025,
    	   RCA_GET_PLUS16V_TEMP_ERR           = 0x00026,
    	   RCA_GET_MINUS16V_TEMP_ERR          = 0x00027,
    	   RCA_GET_PLUS25V_TEMP_ERR           = 0x00028,

    	   RCA_GET_PLUS5V_OVER_VOLT           = 0x00029,
    	   RCA_GET_PLUS6V_OVER_VOLT           = 0x0002A,
    	   RCA_GET_PLUS7V_OVER_VOLT           = 0x0002B,
    	   RCA_GET_MINUS7V_OVER_VOLT          = 0x0002C,
    	   RCA_GET_PLUS9V_OVER_VOLT           = 0x0002D,
    	   RCA_GET_PLUS16V_OVER_VOLT          = 0x0002E,
    	   RCA_GET_MINUS16V_OVER_VOLT         = 0x0002F,
    	   RCA_GET_PLUS25V_OVER_VOLT          = 0x00030,

    	   RCA_GET_PLUS5V_OVERLOAD           = 0x00031,
    	   RCA_GET_PLUS6V_OVERLOAD           = 0x00032,
    	   RCA_GET_PLUS7V_OVERLOAD           = 0x00033,
    	   RCA_GET_MINUS7V_OVERLOAD          = 0x00034,
    	   RCA_GET_PLUS9V_OVERLOAD           = 0x00035,
    	   RCA_GET_PLUS16V_OVERLOAD          = 0x00036,
    	   RCA_GET_MINUS16V_OVERLOAD         = 0x00037,
    	   RCA_GET_PLUS25V_OVERLOAD          = 0x00038,

    	   RCA_GET_PLUS5V_UNDER_VOLT         = 0x00039,
    	   RCA_GET_PLUS6V_UNDER_VOLT         = 0x0003A,
    	   RCA_GET_PLUS7V_UNDER_VOLT         = 0x0003B,
    	   RCA_GET_MINUS7V_UNDER_VOLT        = 0x0003C,
    	   RCA_GET_PLUS9V_UNDER_VOLT         = 0x0003D,
    	   RCA_GET_PLUS16V_UNDER_VOLT        = 0x0003E,
    	   RCA_GET_MINUS16V_UNDER_VOLT       = 0x0003F,
    	   RCA_GET_PLUS25V_UNDER_VOLT        = 0x00040,

    	   RCA_GET_PLUS5V_COMM_FAIL          = 0x00041,
    	   RCA_GET_PLUS6V_COMM_FAIL          = 0x00042,
    	   RCA_GET_PLUS7V_COMM_FAIL          = 0x00043,
    	   RCA_GET_MINUS7V_COMM_FAIL         = 0x00044,
    	   RCA_GET_PLUS9V_COMM_FAIL          = 0x00045,
    	   RCA_GET_PLUS16V_COMM_FAIL         = 0x00046,
    	   RCA_GET_MINUS16V_COMM_FAIL        = 0x00047,
    	   RCA_GET_PLUS25V_COMM_FAIL         = 0x00048,

    	   RCA_GET_SYS_INTRLK_A              = 0x00049,
    	   RCA_GET_SYS_INTRLK_B              = 0x0004A,
    	   RCA_GET_OUTPUT_STATE              = 0x0004B,
    	   RCA_GET_ID_NAME                   = 0x0004C,
    	   RCA_GET_ID_UNIT                   = 0x0004D,
    	   RCA_GET_ID_SFTW_REV               = 0x0004E,
    	   RCA_GET_ID_SER_NUM                = 0x0004F,
    	   RCA_GET_SW_REV                    = 0x00050,

    	   //Control points
    	   RCA_SET_ENABLE_PWR                 = 0x01001,
    };

    static const float ValidMinTemperature;
    static const float ValidMaxTemperature;
};

#endif /* POWERSUPPLYTESTFIXTURE_H_ */
