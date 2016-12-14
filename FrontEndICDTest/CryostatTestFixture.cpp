#include "CryostatTestFixture.h"
#include <logger.h>
#include <sstream>
#include <string>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CryostatTestFixture);

void CryostatTestFixture::setUp() {
    AmbDeviceTestFixture::setUp();
    // Write the special monitor point to establish comms between the AMBSI and ARCOM boards:
    monitor(0x20001, "GET_SETUP_INFO", NULL);
}

void CryostatTestFixture::tearDown() {
    implSetBackingPump(false, "CryostatTestFixture::tearDown()", false);
    AmbDeviceTestFixture::tearDown();
}

void CryostatTestFixture::testGET_CRYOSTAT_TEMPS() {
    //Valid range: 2° to 325° K
    string details;
    float temp;
    unsigned char statusByte;
    for (unsigned index = 0; index < 13; ++index) {
        stringstream streamName;
        streamName << "GET_CRYOSTAT_TEMP_" << index;
        resetAmbVars();
        monitor(baseRCA + CRYOSTAT_TEMP + 4 * index, streamName.str(), &details);
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
        temp = unpackSGL(&statusByte);
        CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
        CPPUNIT_ASSERT_MESSAGE(details, temp >= 2.0 && temp <= 325.0);
        LOG(LM_INFO) << details << " t" << index << "(" << temp << "," << (int) statusByte << ")" << endl;
    }
}

void CryostatTestFixture::testGET_CRYOSTAT_TURBO_PUMP_ENABLE() {
    // interpretation of ICD:
    //  If backing pump is off:
    //    Monitor will return (unknown) value plus HW_BLOCKED status byte
    //    Control(0) no effect
    //      Monitor on control will return 0 plus HW_BLOCKED
    //      Monitor same as before
    //    Control(1) no effect
    //      Monitor on control will return 1 plus HW_BLOCKED
    //      Monitor same as before
    //  If backing pump is on:
    //    Monitor will return 0 plus NO_ERROR
    //    Control(0) no effect
    //      Monitor on control will return 0 plus NO_ERROR
    //    Control(1) start pump
    //      Monitor on control will return 1 plus NO_ERROR
    //      Monitor will return 1 plus NO_ERROR

    string details;
    string cmdDetails;

    // Start with the backing pump off:
    implSetBackingPump(false, "1. Backing pump OFF", false);

    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    // We expect a value (unknown) plus a hardware blocked error since the backing pump is off:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    // no check on data_m[0]
    CPPUNIT_ASSERT_MESSAGE(details, statusByteEqual(data_m[1], FEMC_HARDW_BLOCKED_ERR));

    // Command to disable the pump:
    implSetTurboPump(false, "SET_CRYOSTAT_TURBO_PUMP_ENABLE(0)", &cmdDetails, false);

    // Check results of last MON_ON_CMD:
    CPPUNIT_ASSERT_MESSAGE(cmdDetails, dataLength_m == 2);      // value set + status byte
    CPPUNIT_ASSERT_MESSAGE(cmdDetails, data_m[0] == 0);         // last value set
    CPPUNIT_ASSERT_MESSAGE(cmdDetails, statusByteEqual(data_m[1], FEMC_HARDW_BLOCKED_ERR));    // blocked

    // Monitor results same as before:
    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    // no check on data_m[0]
    CPPUNIT_ASSERT_MESSAGE(details, statusByteEqual(data_m[1], FEMC_HARDW_BLOCKED_ERR));

    // Command to enable the pump:
    implSetTurboPump(true, "SET_CRYOSTAT_TURBO_PUMP_ENABLE(1)", &cmdDetails, false);

    // Check results of last MON_ON_CMD:
    CPPUNIT_ASSERT_MESSAGE(cmdDetails, dataLength_m == 2);      // value set + status byte
    CPPUNIT_ASSERT_MESSAGE(cmdDetails, data_m[0] == 1);         // last value set
    CPPUNIT_ASSERT_MESSAGE(cmdDetails, statusByteEqual(data_m[1], FEMC_HARDW_BLOCKED_ERR));    // blocked

    // Monitor results same as before:
    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    // no check on data_m[0]
    CPPUNIT_ASSERT_MESSAGE(details, statusByteEqual(data_m[1], FEMC_HARDW_BLOCKED_ERR));

    // Now enable the backing pump:
    implSetBackingPump(true, "2. Backing pump ON", false);

    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    // We expect 0 plus NO_ERROR
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);

    // Command to disable the pump.  Takes care of checking MON_ON_CMD.
    implSetTurboPump(false, "SET_CRYOSTAT_TURBO_PUMP_ENABLE(0)", &cmdDetails, true);

    // Monitor results same as before:
    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);

    // Command to enable the pump.  Takes care of checking MON_ON_CMD.
    implSetTurboPump(true, "SET_CRYOSTAT_TURBO_PUMP_ENABLE(1)", &cmdDetails, true);

    // Monitor results show pump is on:
    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);

    // Finish with the backing pump off:
    implSetBackingPump(false, "3. Backing pump OFF", false);
}

void CryostatTestFixture::testGET_CRYOSTAT_TURBO_PUMP_STATE() {
    // interpretation of ICD:
    //  If backing pump is off:
    //    Monitor will return (unknown) value plus HW_BLOCKED status byte
    //  If backing pump is on:
    //    Monitor will return 0 or 1, plus NO_ERROR
    implGetBackingPumpDependent(turboPumpErrorState_RCA, 0, 1, "GET_CRYOSTAT_TURBO_PUMP_STATE");
}

void CryostatTestFixture::testGET_CRYOSTAT_TURBO_PUMP_SPEED() {
    // interpretation of ICD:
    //  If backing pump is off:
    //    Monitor will return (unknown) value plus HW_BLOCKED status byte
    //  If backing pump is on:
    //    Monitor will return 0 or 1, plus NO_ERROR
    implGetBackingPumpDependent(turboPumpHighSpeed_RCA, 0, 1, "GET_CRYOSTAT_TURBO_PUMP_SPEED");
}

void CryostatTestFixture::testGET_CRYOSTAT_SOLENOID_VALVE_STATE() {
    // interpretation of ICD:
    //  If backing pump is off:
    //    Monitor will return (unknown) value plus HW_BLOCKED status byte
    //  If backing pump is on:
    //    Monitor will return [0123], plus NO_ERROR
    implGetBackingPumpDependent(solenoidValveState_RCA, 0, 3, "GET_CRYOSTAT_SOLENOID_VALVE_STATE");
}

// private implementation helper for the above few.
void CryostatTestFixture::implGetBackingPumpDependent(AmbRelativeAddr monRCA, char min, char max, const std::string &callerDescription) {
    string details;
    // Several monitor points behave differently depending on whether the backing pump is on.
    // Start with the backing pump off:
    implSetBackingPump(false, "1. Backing pump OFF");
    // Attempt to get the specified monitor data:
    monitor(monRCA, callerDescription, &details);
    // We expect a value (unknown) plus a hardware blocked error since the backing pump is off:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, statusByteEqual(data_m[1], FEMC_HARDW_BLOCKED_ERR));
    // Now enable the backing pump:
    implSetBackingPump(true, "2. Backing pump ON");
    // Attempt to get the specified monitor data:
    monitor(monRCA, callerDescription, &details);
    // Now we expect valid data with no error and either 0 or 1:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details + " NOTE this will fail if the interlock is in effect!", data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] >= min && data_m[0] <= max);
    // Finish with the backing pump off:
    implSetBackingPump(false, "3. Backing pump OFF");
}


void CryostatTestFixture::testGET_CRYOSTAT_GATE_VALVE_STATE() {
    string details;
    monitor(gateValveState_RCA, "GET_CRYOSTAT_GATE_VALVE_STATE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] <= char(4));  // values in [01234]
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
}

void CryostatTestFixture::testGET_CRYOSTAT_VACUUM_GAUGE_SENSOR_PRESSURE() {
    //Valid range: 10^-9 to 10^3
    string details;
    float temp;
    stringstream streamOut;
    unsigned char statusByte;
    monitor(vacuumCryostatPressure_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_CRYOSTAT_PRESSURE", &details);
    LOG(LM_INFO) << details << endl;
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
    temp = unpackSGL(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, temp >= 10.0e-9 && temp <= 10.0e+3);
    streamOut << "p0(" << temp << "," << (int) statusByte << ") ";

    resetAmbVars();
    monitor(vacuumPortPressure_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_PORT_PRESSURE", &details);
    LOG(LM_INFO) << details << endl;
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
    temp = unpackSGL(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, temp >= 10.0e-9 && temp <= 10.0e+3);
    streamOut << "p1(" << temp << "," << (int) statusByte << ") ";

    LOG(LM_INFO) << streamOut.str() << endl;
    // Not handling the case where the asynchronous monitoring is disabled via the console.  Won't happen in normal use.
}

void CryostatTestFixture::testGET_CRYOSTAT_VACUUM_GAUGE_STATE() {
    string details;
    monitor(vacuumGaugeErrorState_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_STATE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0 || data_m[0] == 1);
}

void CryostatTestFixture::testGET_CRYOSTAT_SUPPLY_CURRENT_230V() {
    string details;
    float current;
    unsigned char statusByte;
    // Behaves differently when the backing pump is on or off:
    // Start with the backing pump off:
    implSetBackingPump(false, "1. Backing pump OFF");
    // Attempt to get the specified monitor data:
    monitor(supplyCurrent230V_RCA, "GET_CRYOSTAT_SUPPLY_CURRENT_230V", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
    current = unpackSGL(&statusByte);
    // We expect a hardware blocked error since the backing pump is off:
    CPPUNIT_ASSERT_MESSAGE(details, statusByteEqual(statusByte, FEMC_HARDW_BLOCKED_ERR));

    // Now enable the backing pump:
    implSetBackingPump(true, "2. Backing pump ON");
    SLEEP(5000);
    // Attempt to get the specified monitor data:
    monitor(supplyCurrent230V_RCA, "GET_CRYOSTAT_SUPPLY_CURRENT_230V", &details);
    // Now we expect valid data with no error and either 0 or 1:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 5);
    current = unpackSGL(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, current >= 0.0 && current <= 14.88);
    // Finish with the backing pump off:
    implSetBackingPump(false, "3. Backing pump OFF");
}

void CryostatTestFixture::testGET_CRYOSTAT_COLD_HEAD_HOURS() {
    string details;
    // request the cold head hours:
    monitor(coldHeadHours_RCA, "GET_CRYOSTAT_COLD_HEAD_HOURS", &details);
    unsigned char statusByte;
    unpackU16(&statusByte);

    // check data length and status byte:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 3);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);

    // attempt to set the corresponding, nonexistent control RCA:
    resetAmbVars();
    packU16(123);
    command(coldHeadHours_RCA + controlRCA, "GET_CRYOSTAT_COLD_HEAD_HOURS", &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] != FEMC_NO_ERROR);
}

void CryostatTestFixture::testSET_CRYOSTAT_RESET_COLD_HEAD_HOURS() {
    string details;
    // send the command to do nothing, then check for no errors:
    resetAmbVars();
    dataLength_m = 1;
    data_m[0] = 0;
    command(coldHeadHoursReset_RCA + controlRCA, "SET_CRYOSTAT_RESET_COLD_HEAD_HOURS(0)", &details, true);

    // send the command to reset the cold head hours and check for no errors:
    resetAmbVars();
    dataLength_m = 1;
    data_m[0] = 1;
    command(coldHeadHoursReset_RCA + controlRCA, "SET_CRYOSTAT_RESET_COLD_HEAD_HOURS(1)", &details, true, 300);

    // check that the hours are now zero, not sufficient if they were zero before this test.
    monitor(coldHeadHours_RCA, "GET_CRYOSTAT_COLD_HEAD_HOURS", &details);
    unsigned char statusByte;
    unsigned short hours = unpackU16(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, hours == 0);
}

void CryostatTestFixture::testSET_CRYOSTAT_BACKING_PUMP_ENABLE() {
    string details;
    // Start with the backing pump off:
    implSetBackingPump(false, "1. Backing pump OFF");
    // Read the backing pump monitor point:
    monitor(backingPumpEnable_RCA, "GET_CRYOSTAT_BACKING_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    // Now turn the backing pump on:
    implSetBackingPump(true, "2. Backing pump ON");
    // Read the backing pump monitor point:
    monitor(backingPumpEnable_RCA, "GET_CRYOSTAT_BACKING_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    // Finish with the backing pump off:
    implSetBackingPump(false, "3. Backing pump OFF");
}

void CryostatTestFixture::testSET_CRYOSTAT_TURBO_PUMP_ENABLE() {
    string details;
    // This can only be checked with the backing pump on:
    implSetBackingPump(true, "1. Backing pump ON");
    // Start with the turbo pump off:
    implSetTurboPump(false, "2. Turbo pump OFF");
    // Read the turbo pump monitor point:
    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    // Now turn the turbo pump on:
    implSetTurboPump(true, "3. Turbo pump ON");
    // Read the turbo pump monitor point:
    monitor(turboPumpEnable_RCA, "GET_CRYOSTAT_TURBO_PUMP_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    // Finish with both pumps off:
    implSetTurboPump(false, "4. Turbo pump OFF");
    implSetBackingPump(false, "5. Backing pump OFF");
}

void CryostatTestFixture::testSET_CRYOSTAT_VACUUM_GAUGE_ENABLE() {
    string details;
    // Start with the vacuum gauge enabled (default state):
    implSetVacuumGauge(true, "1. Vacuum gauge ON");
    // Read the vacuum gauge monitor point:
    monitor(vacuumGaugeEnable_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    // Now turn the vacuum gauge off:
    implSetVacuumGauge(false, "2. Vacuum gauge OFF");
    // Read the vacuum gauge monitor point:
    monitor(vacuumGaugeEnable_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    // Finish with the vacuum gauge enabled (default state):
    implSetVacuumGauge(true, "3. Vacuum gauge ON");
}

void CryostatTestFixture::testSET_CRYOSTAT_GATE_SOLENOID_VALVES() {
    // This test goes through the normal operating sequence for the pumps and valves.
    // For safety, this test will only run if the cryostat pressure is below 30 mBar.
    // However this test can be run when the cryostat is warm or cold.

    string details;
    // Start with the vacuum gauge enabled (default state):
    implSetVacuumGauge(true, "1. Vacuum gauge ON");

    // check that cryostat pressure is below 30 mBar
    monitor(vacuumCryostatPressure_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_CRYOSTAT_PRESSURE", &details);
    unsigned char statusByte;
    float pressure = unpackSGL(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
    {
        stringstream streamOut;
        streamOut << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: cryostatPressure=" << pressure;
        if (pressure >= 30.0) {
            streamOut << " ERROR: this test cannot continue unless the cryostat is below 30 mBar.";
            LOG(LM_INFO) << streamOut.str() << endl;
            CPPUNIT_FAIL(streamOut.str());
        }
        LOG(LM_INFO) << streamOut.str() << endl;
    }
    // turn on the backing pump:
    implSetBackingPump(true, "2. Backing pump ON");

    // check that the gate valve is closed:
    monitor(gateValveState_RCA, "GET_CRYOSTAT_GATE_VALVE_STATE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    {
        stringstream streamOut;
        streamOut << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: gateValveState=" << (int) data_m[0];
        // check for closed, value 0:
        if (data_m[0] != 0) {
            streamOut << " ERROR: this test cannot continue with the gate valve not closed.";
            LOG(LM_INFO) << streamOut.str() << endl;
            // turn off the backing pump and abort:
            implSetBackingPump(false, "Aborting: Backing pump OFF", false);
            CPPUNIT_FAIL(streamOut.str());
        }
        LOG(LM_INFO) << streamOut.str() << endl;
    }

    // close the solenoid valve (just to test the monitor point):
    implSetSolenoidValve(false, "3. Close solenoid valve");
    monitor(solenoidValveState_RCA, "GET_CRYOSTAT_SOLENOID_VALVE_STATE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // open the solenoid valve
    implSetSolenoidValve(true, "4. Open solenoid valve");
    monitor(solenoidValveState_RCA, "GET_CRYOSTAT_SOLENOID_VALVE_STATE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == FEMC_NO_ERROR);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: solenoid valve is open." << endl;

    // turn on the turbo pump:
    implSetTurboPump(true, "5. Turbo pump ON.");
    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: turbo pump is on." << endl;

    // wait up to two minutes for the turbo pump to come up to speed:
    int retries = 25;
    bool upToSpeed = false;
    while (!upToSpeed && retries > 0) {
        LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: waiting for turbo pump to come up to speed." << endl;
        SLEEP(5000);
        monitor(turboPumpHighSpeed_RCA, "GET_CRYOSTAT_TURBO_PUMP_SPEED", &details);
        if (dataLength_m == 2 && data_m[1] == FEMC_NO_ERROR && data_m[0] == 1) {
            upToSpeed = true;
            LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: turbo pump is up to speed." << endl;
        } else
            retries--;
    }
    if (!upToSpeed) {
        // failed to come up to speed.  Abort:
        implSetSolenoidValve(false, "Aborting: Close solenoid valve", false);
        implSetTurboPump(false, "Aborting: Turbo pump OFF", false);
        implSetBackingPump(false, "Aborting: Backing pump OFF", false);
        string msg("testSET_CRYOSTAT_GATE_SOLENOID_VALVES: turbo pump failed to come up to speed.");
        LOG(LM_INFO) << msg;
        CPPUNIT_FAIL(msg);
    }

    // open the gate valve:
    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: Opening gate valve..." << endl;
    implSetGateValve(true, "6. Open gate valve");

    // wait up to 30 seconds for the gate valve to finish opening:
    retries = 30;
    bool opened = false;
    while (!opened && retries > 0) {
        LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: waiting for gate valve to open." << endl;
        SLEEP(1000);
        monitor(gateValveState_RCA, "GET_CRYOSTAT_GATE_VALVE_STATE", &details);
        if (dataLength_m == 2 && data_m[1] == FEMC_NO_ERROR && data_m[0] == 1) {
            opened = true;
            LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: gate valve is open." << endl;
        } else
            retries--;
    }
    // monitor the cryostat and port pressure here, whether or not the gate valve succeeded to open:
    monitor(vacuumCryostatPressure_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_CRYOSTAT_PRESSURE", &details);
    pressure = unpackSGL(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: cryostatPressure=" << pressure << endl;

    monitor(vacuumPortPressure_RCA, "GET_CRYOSTAT_VACUUM_GAUGE_CRYOSTAT_PRESSURE", &details);
    pressure = unpackSGL(&statusByte);
    CPPUNIT_ASSERT_MESSAGE(details, statusByte == FEMC_NO_ERROR);
    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: vacuumPortPressure=" << pressure << endl;

    if (!opened) {
        // failed to open.  Abort:
        implSetGateValve(false, "Aborting: Close gate valve", false);
        implSetSolenoidValve(false, "Aborting: Close solenoid valve", false);
        implSetTurboPump(false, "Aborting: Turbo pump OFF", false);
        SLEEP(20000);
        implSetBackingPump(false, "Aborting: Backing pump OFF", false);
        string msg("testSET_CRYOSTAT_GATE_SOLENOID_VALVES: gate valve failed to open.");
        LOG(LM_INFO) << msg;
        CPPUNIT_FAIL(msg);
    }
    // close gate valve:
    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: Closing gate valve..." << endl;
    implSetGateValve(false, "7. Close gate valve", false);

    // Shutdown and exit:
    implSetSolenoidValve(false, "8. Close solenoid valve", false);
    implSetTurboPump(false, "9. Turbo pump OFF", false);
    SLEEP(20000);
    implSetBackingPump(false, "10. Backing pump OFF", false);
    LOG(LM_INFO) << "testSET_CRYOSTAT_GATE_SOLENOID_VALVES: test complete." << endl;
}

// private implementation functions:

void CryostatTestFixture::implSetBackingPump(bool enable, const std::string &callerDescription, bool checkSuccess) {
    string details;
    resetAmbVars();
    unsigned char newVal = (enable) ? 1 : 0;
    data_m[0] = newVal;
    dataLength_m = 1;
    command(controlRCA + backingPumpEnable_RCA, callerDescription + " SET_CRYOSTAT_BACKING_PUMP_ENABLE", &details, checkSuccess, 300);
    // 1 second delay after setting backing pump:
    SLEEP(500);
}

void CryostatTestFixture::implSetTurboPump(bool enable, const std::string &callerDescription, std::string *cmdDetails, bool checkSuccess) {
    string details;
    resetAmbVars();
    unsigned char newVal = (enable) ? 1 : 0;
    data_m[0] = newVal;
    dataLength_m = 1;
    command(controlRCA + turboPumpEnable_RCA, callerDescription + " SET_CRYOSTAT_TURBO_PUMP_ENABLE", &details, checkSuccess);
    if (cmdDetails)
        *cmdDetails = details;
    // 1 second delay after setting turbo pump:
    SLEEP(500);
}

void CryostatTestFixture::implSetVacuumGauge(bool enable, const std::string &callerDescription, bool checkSuccess) {
    string details;
    resetAmbVars();
    unsigned char newVal = (enable) ? 1 : 0;
    data_m[0] = newVal;
    dataLength_m = 1;
    command(controlRCA + vacuumGaugeEnable_RCA, callerDescription + " SET_CRYOSTAT_VACUUM_GAUGE_ENABLE", &details, checkSuccess);
}

void CryostatTestFixture::implSetGateValve(bool open, const std::string &callerDescription, bool checkSuccess) {
    string details;
    resetAmbVars();
    unsigned char newVal = (open) ? 1 : 0;
    data_m[0] = newVal;
    dataLength_m = 1;
    command(controlRCA + gateValveState_RCA, callerDescription + " SET_CRYOSTAT_GATE_VALVE_STATE", &details, checkSuccess);
    // short delay after setting gate valve:
    SLEEP(200);
}

void CryostatTestFixture::implSetSolenoidValve(bool open, const std::string &callerDescription, bool checkSuccess) {
    string details;
    resetAmbVars();
    unsigned char newVal = (open) ? 1 : 0;
    data_m[0] = newVal;
    dataLength_m = 1;
    command(controlRCA + solenoidValveState_RCA, callerDescription + " SET_CRYOSTAT_SOLENOID_VALVE_STATE", &details, checkSuccess);
    // short delay after setting solenoid valve:
    SLEEP(200);
}


