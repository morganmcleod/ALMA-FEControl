#include "FEModeTestFixture.h"
#include <FrontEndAMB/ds1820.h>
#include <logger.h>
#include <string>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(FEModeTestFixture);

void FEModeTestFixture::setUp() {
    AmbDeviceTestFixture::setUp();
    string details;
    resetAmbVars();
    data_m[0] = 0;  // Operational mode
    dataLength_m = 1;
    // Use commandImpl because we don't want to monitor on the control RCA
    commandImpl(0x2100E, "SET_FE_MODE", &details);
}

void FEModeTestFixture::tearDown() {
    string details;
    resetAmbVars();
    data_m[0] = 0;  // Operational mode
    dataLength_m = 1;
    // Use commandImpl because we don't want to monitor on the control RCA
    commandImpl(0x2100E, "SET_FE_MODE", &details);
    AmbDeviceTestFixture::tearDown();
}

void FEModeTestFixture::testFE_MODE() {
   
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
        // wait up to 2 minutes for complete:  
        // Starting with ARCOM 3.6.7 it loads the socket driver on transition to Troublesooting mode.
        // It can take a long time to try to get a DHCP address.
        bool done = false;
        int retry = 12;
        while (!done && retry > 0) {
            retry--;
            SLEEP(5000);
            // check that it changed:
            monitorImpl(0x2000E, "GET_FE_MODE", &details);
            if (dataLength_m == 1 && data_m[0] == newMode)
                done = true;
        }
        // check that it changed:
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 1);
        CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == newMode);
    }    
    // set it back how we found it:
    resetAmbVars();
    data_m[0] = priorMode;
    dataLength_m = 1;
    // Use commandImpl because we don't want to monitor on the control RCA
    commandImpl(0x2100E, "SET_FE_MODE", &details);
    SLEEP(5000);
}
