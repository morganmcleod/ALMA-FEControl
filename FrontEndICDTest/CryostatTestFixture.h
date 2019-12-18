#ifndef CRYOSTATTESTFIXTURE_H
#define CRYOSTATTESTFIXTURE_H

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>

class CryostatTestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(CryostatTestFixture);
    
    //Register unit tests.   Comment out tests to skip here.
    CPPUNIT_TEST(testGET_CRYOSTAT_TEMPS);
    CPPUNIT_TEST(testSET_CRYOSTAT_TEMP_TVO_COEFF);
    CPPUNIT_TEST(testGET_CRYOSTAT_TURBO_PUMP_ENABLE);
    CPPUNIT_TEST(testGET_CRYOSTAT_TURBO_PUMP_STATE);
    CPPUNIT_TEST(testGET_CRYOSTAT_TURBO_PUMP_SPEED);
    CPPUNIT_TEST(testGET_CRYOSTAT_SOLENOID_VALVE_STATE);
    CPPUNIT_TEST(testGET_CRYOSTAT_GATE_VALVE_STATE);
    CPPUNIT_TEST(testGET_CRYOSTAT_VACUUM_GAUGE_SENSOR_PRESSURE);
    CPPUNIT_TEST(testGET_CRYOSTAT_VACUUM_GAUGE_STATE);
    CPPUNIT_TEST(testGET_CRYOSTAT_SUPPLY_CURRENT_230V);
    CPPUNIT_TEST(testGET_CRYOSTAT_COLD_HEAD_HOURS);
    CPPUNIT_TEST(testSET_CRYOSTAT_RESET_COLD_HEAD_HOURS);
    CPPUNIT_TEST(testSET_CRYOSTAT_BACKING_PUMP_ENABLE);
    CPPUNIT_TEST(testSET_CRYOSTAT_TURBO_PUMP_ENABLE);
    CPPUNIT_TEST(testSET_CRYOSTAT_VACUUM_GAUGE_ENABLE);
    CPPUNIT_TEST(testSET_CRYOSTAT_GATE_SOLENOID_VALVES);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    
    // Declare the tests.
    void testGET_CRYOSTAT_TEMPS();
    void testSET_CRYOSTAT_TEMP_TVO_COEFF();
    void testGET_CRYOSTAT_TURBO_PUMP_ENABLE();
    void testGET_CRYOSTAT_TURBO_PUMP_STATE();
    void testGET_CRYOSTAT_TURBO_PUMP_SPEED();
    void testGET_CRYOSTAT_SOLENOID_VALVE_STATE();
    void testGET_CRYOSTAT_GATE_VALVE_STATE();
    void testGET_CRYOSTAT_VACUUM_GAUGE_SENSOR_PRESSURE();
    void testGET_CRYOSTAT_VACUUM_GAUGE_STATE();
    void testGET_CRYOSTAT_SUPPLY_CURRENT_230V();
    void testGET_CRYOSTAT_COLD_HEAD_HOURS();
    void testSET_CRYOSTAT_RESET_COLD_HEAD_HOURS();
    void testSET_CRYOSTAT_BACKING_PUMP_ENABLE();
    void testSET_CRYOSTAT_TURBO_PUMP_ENABLE();
    void testSET_CRYOSTAT_VACUUM_GAUGE_ENABLE();
    void testSET_CRYOSTAT_GATE_SOLENOID_VALVES();

private:
    enum MonitorControlOffset {
        CRYOSTAT_TEMP           = 0x0000,
        CRYOSTAT_TVO_COEFF     = 0x0001,
        BACKING_PUMP_ENABLE     = 0x0034,
        TURBO_PUMP_ENABLE       = 0x0038,
        TURBO_PUMP_STATE        = 0x0039,
        TURBO_PUMP_SPEED        = 0x003A,
        GATE_VALVE_STATE        = 0x003C,
        SOLENOID_VALVE_STATE    = 0x0040,
        VACUUM_CRYOSTAT_PRES    = 0x0044,
        VACUUM_PORT_PRES        = 0x0045,
        VACUUM_GAUGE_ENABLE     = 0x0046,
        VACUUM_GAUGE_STATE      = 0x0047,
        SUPPLY_CURRENT_230V     = 0x0048,
        COLD_HEAD_HOURS         = 0x004C,
        RESET_COLD_HEAD_HOURS   = 0x004D,

        controlRCA              = 0x10000,  /// add this to the monitor RCA to get the corresponding command RCA
        baseRCA                 = 0xC000,   /// all cryostat M&C points are in this range
        cryostatTempCoeff_RCA       = baseRCA + CRYOSTAT_TVO_COEFF,
        backingPumpEnable_RCA       = baseRCA + BACKING_PUMP_ENABLE,
        turboPumpEnable_RCA         = baseRCA + TURBO_PUMP_ENABLE,
        turboPumpErrorState_RCA     = baseRCA + TURBO_PUMP_STATE,
        turboPumpHighSpeed_RCA      = baseRCA + TURBO_PUMP_SPEED,
        gateValveState_RCA          = baseRCA + GATE_VALVE_STATE,
        solenoidValveState_RCA      = baseRCA + SOLENOID_VALVE_STATE,
        vacuumCryostatPressure_RCA  = baseRCA + VACUUM_CRYOSTAT_PRES,
        vacuumPortPressure_RCA      = baseRCA + VACUUM_PORT_PRES,
        vacuumGaugeEnable_RCA       = baseRCA + VACUUM_GAUGE_ENABLE,
        vacuumGaugeErrorState_RCA   = baseRCA + VACUUM_GAUGE_STATE,
        supplyCurrent230V_RCA       = baseRCA + SUPPLY_CURRENT_230V,
        coldHeadHours_RCA           = baseRCA + COLD_HEAD_HOURS,
        coldHeadHoursReset_RCA      = baseRCA + controlRCA + RESET_COLD_HEAD_HOURS
    };

    // helper functions for the above test cases:
    void implGetBackingPumpDependent(AmbRelativeAddr monRCA, char min, char max, const std::string &callerDescription);
    ///< Check a one-byte monitor point which is only available when the backing pump is on.
    ///< When the pump is off, should always get FEMC_HARDW_BLOCKED_ERR.
    ///< WHen on, should get a byte between min and max.

    void implSetBackingPump(bool enable, const std::string &callerDescription, bool checkSuccess = true);
    ///< Enables or disables the backing pump.
    ///< checkSuccess is passed through to base class command()

    void implSetTurboPump(bool enable, const std::string &callerDescription, std::string *cmdDetails = NULL, bool checkSuccess = true);
    ///< Enables or disables the turbo pump.
    ///< checkSuccess is passed through to base class command()

    void implSetVacuumGauge(bool enable, const std::string &callerDescription, bool checkSuccess = true);
    ///< Enables or disables the vacuum gauge.
    ///< checkSuccess is passed through to base class command()

    void implSetGateValve(bool open, const std::string &callerDescription, bool checkSuccess = true);
    ///< Opens or closes the gate valve.
    ///< checkSuccess is passed through to base class command()

    void implSetSolenoidValve(bool open, const std::string &callerDescription, bool checkSuccess = true);
    ///< Opens or closes the solenoid valve.
    ///< checkSuccess is passed through to base class command()
};

#endif // CRYOSTATTESTFIXTURE_H

