#include "FEMCTestFixture.h"
#include <FrontEndAMB/ds1820.h>
#include <logger.h>
#include <string>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(FEMCTestFixture);

void FEMCTestFixture::setUp() {
    AmbDeviceTestFixture::setUp();
}

void FEMCTestFixture::tearDown() {
    AmbDeviceTestFixture::tearDown();
}

void FEMCTestFixture::testAMBSI_SERIALNUM() {
    string details;
    monitor(0x00000, "AMBSI_SERIALNUM", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
}

void FEMCTestFixture::testAMBSI_SLAVE_REV() {
    string details;
    monitor(0x30000, "AMBSI_SLAVE_REV", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 3);
    // TODO: any other constraints on valid version data?
}

void FEMCTestFixture::testAMBSI_NUM_ERRORS() {
    string details;
    monitor(0x30001, "AMBSI_NUM_ERRORS", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);
    unsigned num(unpackU32());
    LOG(LM_DEBUG) << "AMBSI_NUM_ERRORS=" << num << endl;
}

void FEMCTestFixture::testAMBSI_NUM_TRANSACTIONS() {
    string details;
    monitor(0x30002, "AMBSI_NUM_TRANSACTIONS", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);
    unsigned num(unpackU32());
    LOG(LM_DEBUG) << "AMBSI_NUM_TRANSACTIONS=" << num << endl;
}

void FEMCTestFixture::testAMBSI_TEMPERATURE() {
    string details;
    monitor(0x30003, "AMBSI_TEMPERATURE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);
    float temp;
    CPPUNIT_ASSERT_MESSAGE(details, 0 == unpackDS1820Temperature(temp, dataLength_m, data_m));
    LOG(LM_DEBUG) << "AMBSI_TEMPERATURE=" << temp;
}

void FEMCTestFixture::testAMBSI_SOFTWARE_REV() {
    string details;
    monitor(0x30004, "AMBSI_SOFTWARE_REV", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 3);
    // TODO: any other constraints on valid version data?
}

void FEMCTestFixture::testAMBSI_HARDWARE_REV() {
    string details;
    monitor(0x30005, "AMBSI_HARDWARE_REV", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    // TODO: any other constraints on valid version data?
}

void FEMCTestFixture::testAMBSI_HARDWARE_RESET() {
    //31000 C 0 Hardware Reset
    // TODO:  can/should we test this?
}

void FEMCTestFixture::testGET_AMBSI1_VERSION() {
    string details;
    monitor(0x20000, "GET_AMBSI1_VERSION", &details);
    // we expect three bytes:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 3);
    // TODO: any other constraints on valid version data?
}

void FEMCTestFixture::testGET_SETUP_INFO() {
    string details;
    monitor(0x20001, "GET_SETUP_INFO", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0x00 || data_m[0] == 0x05);
//    SLEEP(1000);
}

void FEMCTestFixture::testGET_VERSION_INFO() {
    string details;
    monitor(0x20002, "GET_VERSION_INFO", &details);
    // we expect three bytes:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 3);
    // TODO: any other constraints on valid version data?
}

void FEMCTestFixture::testGET_SPECIAL_MONITOR_RCA() {
    string details;
    monitor(0x20003, "GET_SPECIAL_MONITOR_RCA", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
}

void FEMCTestFixture::testGET_SPECIAL_CONTROL_RCA() {
    string details;
    monitor(0x20004, "GET_SPECIAL_CONTROL_RCA", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
}

void FEMCTestFixture::testGET_MONITOR_RCA() {
    string details;
    monitor(0x20005, "GET_MONITOR_RCA", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
}

void FEMCTestFixture::testGET_CONTROL_RCA() {
    string details;
    monitor(0x20006, "GET_CONTROL_RCA", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
}

void FEMCTestFixture::testGET_PPCOMM_TIME() {
    string details;
    monitor(0x20007, "GET_PPCOMM_TIME", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[2] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[3] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[4] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[5] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[6] == 0xFF);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[7] == 0xFF);
}

void FEMCTestFixture::testGET_FPGA_VERSION_INFO() {
    string details;
    monitor(0x20008, "GET_FPGA_VERSION_details", &details);
    // we expect three bytes:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 3);
    // TODO: any other constraints on valid version data?
}

void FEMCTestFixture::testCONSOLE_ENABLE() {
    string details;
    // get the current console enable mode:
    monitor(0x20009, "GET_CONSOLE_ENABLE", &details);
    // we expect one byte, either 0 or 1:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0 || data_m[0] == 1);
    // cache it to set back at the end:
    unsigned char priorMode(data_m[0]);
    unsigned char newMode(1 - priorMode);

    // toggle it to the opposite:
    resetAmbVars();
    data_m[0] = newMode;
    dataLength_m = 1;

    // Use commandImpl because we don't want to monitor on the control RCA
    commandImpl(0x21009, "SET_CONSOLE_ENABLE", &details);
    // check that it changed:
    monitor(0x20009, "GET_CONSOLE_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == newMode);
    
    // toggle it back:
    resetAmbVars();
    data_m[0] = priorMode;
    dataLength_m = 1;

    // Use commandImpl because we don't want to monitor on the control RCA
    commandImpl(0x21009, "SET_CONSOLE_ENABLE", &details);
    // check that it changed:
    monitor(0x20009, "GET_CONSOLE_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == priorMode);
}

void FEMCTestFixture::testESNS() {
    string details;
    monitor(0x2000A, "GET_ESNS_FOUND", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
    unsigned numESNs(data_m[0]);
    if (!numESNs) {
        monitor(0x2000B, "GET_ESNS", &details);
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[2] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[3] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[4] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[5] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[6] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[7] == 0xFF);
    } else {
        unsigned count(numESNs + 1);
        while (count-- > 0) {
            monitor(0x2000B, "GET_ESNS", &details);
            CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 8);
            // TODO: can we assert anything else about the content of the ESNs?
        }
    }
}

void FEMCTestFixture::testERRORS() {
    string details;
    monitor(0x2000C, "GET_ERRORS_NUMBER", &details);
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
    unsigned numErrors(unpackU16());    
    if (!numErrors) {
        monitor(0x2000D, "GET_NEXT_ERROR", &details);
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0xFF);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[1] == 0xFF);
    } else {
        unsigned count(numErrors + 1);
        while (count-- > 0) {
            monitor(0x2000D, "GET_NEXT_ERROR", &details);
            CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);
            // TODO: can we assert anything else about the content of the error data?
        }
    }
}

void FEMCTestFixture::testFE_MODE() {
    string details;
    // get the current front end mode:
    monitor(0x2000E, "GET_FE_MODE", &details);
    // we expect one byte, either 0, 1, or 2:
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0 || data_m[0] == 1 || data_m[0] == 2);
    // cache it to set back at the end:
    unsigned char priorMode(data_m[0]);

    // cycle it through each mode and check:
    for (unsigned char newMode = 0; newMode < 3; ++newMode) {    
        resetAmbVars();
        data_m[0] = newMode;
        dataLength_m = 1;
        // Use commandImpl because we don't want to monitor on the control RCA
        commandImpl(0x2100E, "SET_FE_MODE", &details);
        // check that it changed:
        monitor(0x2000E, "GET_FE_MODE", &details);
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == newMode);
    }    
    // set it back how we found it:
    resetAmbVars();
    data_m[0] = priorMode;
    dataLength_m = 1;
    // Use commandImpl because we don't want to monitor on the control RCA
    commandImpl(0x2100E, "SET_FE_MODE", &details);
}

void FEMCTestFixture::testGET_TCPIP_ADDRESS() {
    // todo:  need to check FEMC firmware version and skip this test if < 2.9

    string details;
    // get front end IP address:
    monitor(0x2000F, "GET_TCPIP_ADDRES", &details);

    // we expect four bytes
    CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 4);
    // For now only check first byte non-zero:
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] != 0);
}

void FEMCTestFixture::testSET_EXIT_PROGRAM() {
    //TODO: can this be tested?
}

void FEMCTestFixture::testSET_REBOOT() {
    //TODO: can this be tested?
}

