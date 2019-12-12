#include "STANDBY2TestFixture.h"
#include <stringConvert.h>
#include <string>
using namespace std;


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(STANDBY2TestFixture);

void STANDBY2TestFixture::setUp()
{
    AmbDeviceTestFixture::setUp();
    // Write the special monitor point to establish comms between the AMBSI and ARCOM boards:
    monitor(MON_GET_SETUP_INFO, "GET_SETUP_INFO", NULL);
    // Set FE Mode to normal:
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(CTRL_FE_MODE, "CTRL_FE_MODE", NULL);
}

void STANDBY2TestFixture::tearDown()
{
    AmbDeviceTestFixture::tearDown();
}

void STANDBY2TestFixture::allBandsOff() {
    //TODO: is this sufficient in light of STANDBY2 mode?

    string details;
    //check if all bands are disabled
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    //if all bands not disabled then disable them.
    if(data_m[0] != 0)
    {
        data_m[0]=0;
        dataLength_m=1;

        for (unsigned band = 1; band <= 10; ++band) {
            commandImpl(CTRL_ENABLE_BAND1 + ((band - 1) << 4), "SET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details);
            SLEEP(100);
        }
    }
}


void STANDBY2TestFixture::testOnlyBand6Allowed() {
    //!< Test that only band6 may be set to STANDBY2 mode and that proper errors are returned for other bands.

    string details;
    AmbRelativeAddr RCA;

    // Make sure all bands are off:
    allBandsOff();

    // Loop over all bands:
    for (unsigned band = 1; band <= 10; ++band) {

        // Attempt to set STANDBY2 mode:
        data_m[0] = 2;
        dataLength_m = 1;
        RCA = CTRL_ENABLE_BAND1 + ((band - 1) << 4);
        bool checkSuccess = (band == 6);
        command(RCA, "SET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details, checkSuccess);

        // command() will have monitored on the command RCA.  Check the status byte for FEMC_HARDW_BLOCKED_ERR:
        if (band == 6)
            CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);
        else
            CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_HARDW_BLOCKED_ERR);

        // monitor the band power on state:
        RCA = MON_ENABLE_BAND1 + ((band - 1) << 4);
        monitor(RCA, "GET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details);
        CPPUNIT_ASSERT_MESSAGE(details, dataLength_m == 2);

        // should be "2" for band 6 and "0" for all others:
        if (band == 6)
            CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);
        else
            CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    }

    // Make sure all bands are off:
    allBandsOff();
}

void STANDBY2TestFixture::testFourBandsAllowed() {
    //!< Test that four bands may be powered on as long as one is in STANDBY2 mode.  Five not allowed.
    string details;

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set three bands on:
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND1, "CTRL_ENABLE_BAND1", &details);

    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND2, "CTRL_ENABLE_BAND2", &details);

    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);

    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND3, "CTRL_ENABLE_BAND3", &details);

    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 3);

    // Attempt to set a fourth:
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_HARDW_BLOCKED_ERR);

    // Num bands on should be 3:
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 3);

    // Set band 6 to STANDBY2:
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Num bands on should be 4:
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 4);

    // Attempt to set a fifth:
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND7, "CTRL_ENABLE_BAND7", &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_HARDW_BLOCKED_ERR);

    // Num bands on should be 4:
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 4);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
}

void STANDBY2TestFixture::testCommandPayload() {
    //!< Test that only 0, 1, 2 are allowed payloads to the SET_POWER_DISTRIBUTION_MODULEn_ENABLE command
    string details;

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is ON
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is STANDBY2
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);

    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to -1
    data_m[0] = 255;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) != FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to 72
    data_m[0] = 72;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) != FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to 3
    data_m[0] = 3;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, false);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) != FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Make sure all bands are off:
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
}

void STANDBY2TestFixture::testStateTransitions() {
    //!< Test all the state transitions between OFF, ON, STANDBY2
    string details;

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is ON
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to ON again
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is ON
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Verify 0 bands powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to OFF again
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Verify 0 bands powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is STANDBY2
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to STANDBY2 again
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is STANDBY2
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is ON
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to ON again
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is ON
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is STANDBY2
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to STANDBY2 again
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is STANDBY2
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 2);

    // Verify one band powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);


    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Verify 0 bands powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to OFF again
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[1]) == FEMC_NO_ERROR);

    // Verify it is OFF
    monitor(MON_ENABLE_BAND6, "MON_ENABLE_BAND6", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Verify 0 bands powered on
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
}

void STANDBY2TestFixture::testSetLNA() {
    //!< Test that LNA operation is blocked in STANDBY2 mode and allowed in ON mode
    string details;

    // Set up some RCAs for monitor and control of various LNA subystems:
    AmbRelativeAddr ctrlRCA10(CONTROL_RCA + BAND6 + LNA_ENABLE);
    AmbRelativeAddr ctrlRCA11(CONTROL_RCA + BAND6 + LNA1_DRAIN_VOLTAGE);
    AmbRelativeAddr ctrlRCA12(CONTROL_RCA + BAND6 + LNA1_DRAIN_CURRENT);

    AmbRelativeAddr monRCA10(BAND6 + LNA_ENABLE);
    AmbRelativeAddr monRCA11(BAND6 + LNA1_DRAIN_VOLTAGE);
    AmbRelativeAddr monRCA12(BAND6 + LNA1_DRAIN_CURRENT);
    AmbRelativeAddr monRCA13(BAND6 + LNA1_GATE_VOLTAGE);

    AmbRelativeAddr ctrlRCA20(CONTROL_RCA + BAND6 + LNA_ENABLE);
    AmbRelativeAddr ctrlRCA21(CONTROL_RCA + BAND6 + LNA3_DRAIN_VOLTAGE);
    AmbRelativeAddr ctrlRCA22(CONTROL_RCA + BAND6 + LNA3_DRAIN_CURRENT);

    AmbRelativeAddr monRCA20(BAND6 + LNA_ENABLE);
    AmbRelativeAddr monRCA21(BAND6 + LNA3_DRAIN_VOLTAGE);
    AmbRelativeAddr monRCA22(BAND6 + LNA3_DRAIN_CURRENT);
    AmbRelativeAddr monRCA23(BAND6 + LNA3_GATE_VOLTAGE);

    AmbRelativeAddr ctrlRCA30(CONTROL_RCA + BAND6 + POL1 + SB2 + LNA_ENABLE);
    AmbRelativeAddr ctrlRCA31(CONTROL_RCA + BAND6 + POL1 + SB2 + LNA2_DRAIN_VOLTAGE);
    AmbRelativeAddr ctrlRCA32(CONTROL_RCA + BAND6 + POL1 + SB2 + LNA2_DRAIN_CURRENT);

    AmbRelativeAddr monRCA30(BAND6 + POL1 + SB2 + LNA_ENABLE);
    AmbRelativeAddr monRCA31(BAND6 + POL1 + SB2 + LNA2_DRAIN_VOLTAGE);
    AmbRelativeAddr monRCA32(BAND6 + POL1 + SB2 + LNA2_DRAIN_CURRENT);
    AmbRelativeAddr monRCA33(BAND6 + POL1 + SB2 + LNA2_GATE_VOLTAGE);

    AmbRelativeAddr ctrlRCA40(CONTROL_RCA + BAND6 + POL1 + LNA_ENABLE);
    AmbRelativeAddr ctrlRCA41(CONTROL_RCA + BAND6 + POL1 + LNA3_DRAIN_VOLTAGE);
    AmbRelativeAddr ctrlRCA42(CONTROL_RCA + BAND6 + POL1 + LNA3_DRAIN_CURRENT);

    AmbRelativeAddr monRCA40(BAND6 + POL1 + LNA_ENABLE);
    AmbRelativeAddr monRCA41(BAND6 + POL1 + LNA3_DRAIN_VOLTAGE);
    AmbRelativeAddr monRCA42(BAND6 + POL1 + LNA3_DRAIN_CURRENT);
    AmbRelativeAddr monRCA43(BAND6 + POL1 + LNA3_GATE_VOLTAGE);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    SLEEP(1000);

    // Test some LNA enable and monitor and control commands:
    testLNAHelper(ctrlRCA10, ctrlRCA11, ctrlRCA12, monRCA10, monRCA11, monRCA12, monRCA13, false);
    testLNAHelper(ctrlRCA20, ctrlRCA21, ctrlRCA22, monRCA20, monRCA21, monRCA22, monRCA23, false);
    testLNAHelper(ctrlRCA30, ctrlRCA31, ctrlRCA32, monRCA30, monRCA31, monRCA32, monRCA33, false);
    testLNAHelper(ctrlRCA40, ctrlRCA41, ctrlRCA42, monRCA40, monRCA41, monRCA42, monRCA43, false);

    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);

    // Test some LNA enable and monitor and control commands:
    testLNAHelper(ctrlRCA10, ctrlRCA11, ctrlRCA12, monRCA10, monRCA11, monRCA12, monRCA13, true);
    testLNAHelper(ctrlRCA20, ctrlRCA21, ctrlRCA22, monRCA20, monRCA21, monRCA22, monRCA23, true);
    testLNAHelper(ctrlRCA30, ctrlRCA31, ctrlRCA32, monRCA30, monRCA31, monRCA32, monRCA33, true);
    testLNAHelper(ctrlRCA40, ctrlRCA41, ctrlRCA42, monRCA40, monRCA41, monRCA42, monRCA43, true);

    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
}

void STANDBY2TestFixture::testLNAHelper(const AmbRelativeAddr &ctrlRCA0,
                                        const AmbRelativeAddr &ctrlRCA1,
                                        const AmbRelativeAddr &ctrlRCA2,
                                        const AmbRelativeAddr &monRCA0,
                                        const AmbRelativeAddr &monRCA1,
                                        const AmbRelativeAddr &monRCA2,
                                        const AmbRelativeAddr &monRCA3,
                                        bool shouldSucceed)
{
    string details;
    string shouldSucceedStr(" shouldSucceed=");
    shouldSucceedStr += (shouldSucceed) ? "1" : "0";

    // Even disable isn't allowed when in STANDBY2 mode:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA0, string("LNA_ENABLE") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Enable and control some LNAs:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA0, string("LNA_ENABLE") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Set some LNA values:
    packSGL(1.0);
    command(ctrlRCA1, string("LNA_DRAIN_VOLTAGE") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Set some LNA values:
    packSGL(0.1);
    command(ctrlRCA2, string("LNA1_DRAIN_CURRENT") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Test LNA enable state
    monitor(monRCA0, string("LNA_ENABLE") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == (shouldSucceed ? 1 : 0));
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);

    // Monitoring permitted:
    monitor(monRCA1, string("LNA_DRAIN_VOLTAGE") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    monitor(monRCA2, string("LNA_DRAIN_CURRENT") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    monitor(monRCA3, string("LNA_GATE_VOLTAGE") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
}


void STANDBY2TestFixture::testSetLNALED() {
    //!< Test that LNA LED operation is blocked in STANDBY2 mode and allowed in ON mode
    string details;

    // Set up some RCAs for monitor and control of both LNA LED subystems:
    AmbRelativeAddr ctrlRCA1(CONTROL_RCA + BAND6 + LNA_LED_ENABLE);
    AmbRelativeAddr monRCA1(BAND6 + LNA_LED_ENABLE);

    AmbRelativeAddr ctrlRCA2(CONTROL_RCA + BAND6 + POL1 + LNA_LED_ENABLE);
    AmbRelativeAddr monRCA2(BAND6 + POL1 + LNA_LED_ENABLE);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    SLEEP(1000);


    // Set LNA LED enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA1, "LNA_LED_ENABLE", &details, false);
    // Should be rejected:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Monitoring permitted:
    monitor(monRCA1, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    // LNA LED should remain disabled:
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set LNA LED enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA2, "LNA_LED_ENABLE", &details, false);
    // Should be rejected:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Monitoring permitted:
    monitor(monRCA2, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    // LNA LED should remain disabled:
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);


    // Set LNA LED enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA1, "LNA_LED_ENABLE", &details, true);
    // Should be allowed:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);

    // Monitoring permitted:
    monitor(monRCA1, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    // LNA LED should now be enabled:
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Set LNA LED enable back to 0:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA1, "LNA_LED_ENABLE", &details, true);


    // Set LNA LED enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA2, "LNA_LED_ENABLE", &details, true);
    // Should be allowed:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);

    // Monitoring permitted:
    monitor(monRCA2, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    // LNA LED should now be enabled:
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Set LNA LED enable back to 0:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA2, "LNA_LED_ENABLE", &details, true);


    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
}

void STANDBY2TestFixture::testSetSIS() {
    //!< Test that SIS operation is blocked in STANDBY2 mode and allowed in ON mode
    string details;

    // Set up some RCAs for monitor and control of a two of the SIS subystems:
    AmbRelativeAddr ctrlRCA10(CONTROL_RCA + BAND6 + SIS_OPEN_LOOP);
    AmbRelativeAddr ctrlRCA11(CONTROL_RCA + BAND6 + SIS_VOLTAGE);

    AmbRelativeAddr monRCA10(BAND6 + SIS_OPEN_LOOP);
    AmbRelativeAddr monRCA11(BAND6 + SIS_VOLTAGE);
    AmbRelativeAddr monRCA12(BAND6 + SIS_CURRENT);

    AmbRelativeAddr ctrlRCA20(CONTROL_RCA + BAND6 + POL1 + SB2 + SIS_OPEN_LOOP);
    AmbRelativeAddr ctrlRCA21(CONTROL_RCA + BAND6 + POL1 + SB2 + SIS_VOLTAGE);

    AmbRelativeAddr monRCA20(BAND6 + POL1 + SB2 + SIS_OPEN_LOOP);
    AmbRelativeAddr monRCA21(BAND6 + POL1 + SB2 + SIS_VOLTAGE);
    AmbRelativeAddr monRCA22(BAND6 + POL1 + SB2 + SIS_CURRENT);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    SLEEP(1000);

    // Test some SIS monitor and control commands:
    testSISHelper(ctrlRCA10, ctrlRCA11, monRCA10, monRCA11, monRCA12, false);
    testSISHelper(ctrlRCA20, ctrlRCA21, monRCA20, monRCA21, monRCA22, false);

    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);

    // Test some SIS monitor and control commands:
    testSISHelper(ctrlRCA10, ctrlRCA11, monRCA10, monRCA11, monRCA12, true);
    testSISHelper(ctrlRCA20, ctrlRCA21, monRCA20, monRCA21, monRCA22, true);

    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
}

void STANDBY2TestFixture::testSISHelper(const AmbRelativeAddr &ctrlRCA0,
                                        const AmbRelativeAddr &ctrlRCA1,
                                        const AmbRelativeAddr &monRCA0,
                                        const AmbRelativeAddr &monRCA1,
                                        const AmbRelativeAddr &monRCA2,
                                        bool shouldSucceed)
{
    string details;
    string shouldSucceedStr(" shouldSucceed=");
    shouldSucceedStr += (shouldSucceed) ? "1" : "0";

    // Set the SIS open loop control:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA0, string("SIS_OPEN_LOOP") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Set SIS voltage:
    packSGL(2.3);
    command(ctrlRCA1, string("SIS_VOLTAGE") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Test SIS open loop state:
    monitor(monRCA0, string("SIS_OPEN_LOOP") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == (shouldSucceed ? 1 : 0));
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);

    // Monitoring permitted:
    monitor(monRCA1, string("SIS_VOLTAGE") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    monitor(monRCA2, string("SIS_CURRENT") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);

    // Set SIS current back to 0:
    packSGL(0.0);
    command(ctrlRCA1, string("SIS_VOLTAGE") + shouldSucceedStr, &details, false);
}

void STANDBY2TestFixture::testSetSISMagnet() {
    //!< Test that SIS Magnet operation is blocked in STANDBY2 mode and allowed in ON mode
    string details;

    // Set up some RCAs for monitor and control of two of the SIS magnet subystems:
    AmbRelativeAddr ctrlRCA10(CONTROL_RCA + BAND6 + SIS_MAGNET_CURRENT);

    AmbRelativeAddr monRCA10(BAND6 + SIS_MAGNET_CURRENT);
    AmbRelativeAddr monRCA11(BAND6 + SIS_MAGNET_VOLTAGE);

    AmbRelativeAddr ctrlRCA20(CONTROL_RCA + BAND6 + POL1 + SIS_MAGNET_CURRENT);

    AmbRelativeAddr monRCA20(BAND6 + POL1 + SIS_MAGNET_CURRENT);
    AmbRelativeAddr monRCA21(BAND6 + POL1 + SIS_MAGNET_VOLTAGE);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    SLEEP(1000);

    // Test some SIS monitor and control commands:
    testSISMagnetHelper(ctrlRCA10, monRCA10, monRCA11, false);
    testSISMagnetHelper(ctrlRCA20, monRCA20, monRCA21, false);

    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);

    // Test some SIS monitor and control commands:
    testSISMagnetHelper(ctrlRCA10, monRCA10, monRCA11, true);
    testSISMagnetHelper(ctrlRCA20, monRCA20, monRCA21, true);

    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
}

void STANDBY2TestFixture::testSISMagnetHelper(const AmbRelativeAddr &ctrlRCA0,
                                              const AmbRelativeAddr &monRCA0,
                                              const AmbRelativeAddr &monRCA1,
                                              bool shouldSucceed)
{
    string details;
    string shouldSucceedStr(" shouldSucceed=");
    shouldSucceedStr += (shouldSucceed) ? "1" : "0";

    // Set SIS magnet current:
    packSGL(5.1);
    command(ctrlRCA0, string("SIS_MAGNET_CURRENT") + shouldSucceedStr, &details, shouldSucceed);
    // Check monitor on command success:
    if (shouldSucceed)
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);
    else
        CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Monitoring permitted:
    monitor(monRCA0, string("SIS_MAGNET_CURRENT") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);
    monitor(monRCA1, string("SIS_MAGNET_VOLTAGE") + shouldSucceedStr, &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);

    // Set SIS magnet current back to 0:
    packSGL(0.0);
    command(ctrlRCA0, string("SIS_MAGNET_CURRENT") + shouldSucceedStr, &details, false);
}


void STANDBY2TestFixture::testSetSISHeater() {
    //!< Test that SIS Heater operation is blocked in STANDBY2 mode and allowed in ON mode
    string details;

    // Set up some RCAs for monitor and control of both SIS heater subystems:
    AmbRelativeAddr ctrlRCA1(CONTROL_RCA + BAND6 + SIS_HEATER_ENABLE);
    AmbRelativeAddr monRCA1(BAND6 + SIS_HEATER_CURRENT);

    AmbRelativeAddr ctrlRCA2(CONTROL_RCA + BAND6 + POL1 + SIS_HEATER_ENABLE);
    AmbRelativeAddr monRCA2(BAND6 + POL1 + SIS_HEATER_CURRENT);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);


    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    SLEEP(1000);


    // Set SIS heater enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA1, "SIS_HEATER_ENABLE", &details, false);
    // Should be rejected:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Monitoring permitted:
    monitor(monRCA1, "SIS_HEATER_CURRENT", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);


    // Set SIS heater enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA2, "SIS_HEATER_ENABLE", &details, false);
    // Should be rejected:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_HARDW_BLOCKED_ERR);

    // Monitoring permitted:
    monitor(monRCA2, "SIS_HEATER_CURRENT", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);


    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);


    // Set SIS heater enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA1, "SIS_HEATER_ENABLE", &details, true);
    // Should be allowed:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);

    // Monitoring permitted:
    monitor(monRCA1, "SIS_HEATER_CURRENT", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);

    // Set SIS heater enable back to 0:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA1, "SIS_HEATER_ENABLE", &details, true);


    // Set SIS heater enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA2, "SIS_HEATER_ENABLE", &details, true);
    // Should be allowed:
    CPPUNIT_ASSERT_MESSAGE(details, ((char) data_m[dataLength_m - 1]) == FEMC_NO_ERROR);

    // Monitoring permitted:
    monitor(monRCA2, "SIS_HEATER_CURRENT", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[dataLength_m - 1] == FEMC_NO_ERROR);

    // Set SIS heater enable back to 0:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA2, "SIS_HEATER_ENABLE", &details, true);


    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
}

void STANDBY2TestFixture::testColdElectronicsOff() {
    //!< Test that all cold eletronics are turned off when entering STANDBY2 mode.
    string details;
    unsigned char statusByte;
    float monValue;

    // Set up some RCAs for monitor and control of some LNAs:
    AmbRelativeAddr ctrlRCA10(CONTROL_RCA + BAND6 + LNA_ENABLE);
    AmbRelativeAddr ctrlRCA11(CONTROL_RCA + BAND6 + LNA1_DRAIN_VOLTAGE);
    AmbRelativeAddr ctrlRCA12(CONTROL_RCA + BAND6 + LNA1_DRAIN_CURRENT);

    AmbRelativeAddr monRCA10(BAND6 + LNA_ENABLE);
    AmbRelativeAddr monRCA11(BAND6 + LNA1_DRAIN_VOLTAGE);
    AmbRelativeAddr monRCA12(BAND6 + LNA1_DRAIN_CURRENT);

    AmbRelativeAddr ctrlRCA20(CONTROL_RCA + BAND6 + POL1 + SB2 + LNA_ENABLE);
    AmbRelativeAddr ctrlRCA21(CONTROL_RCA + BAND6 + POL1 + SB2 + LNA2_DRAIN_VOLTAGE);
    AmbRelativeAddr ctrlRCA22(CONTROL_RCA + BAND6 + POL1 + SB2 + LNA2_DRAIN_CURRENT);

    AmbRelativeAddr monRCA20(BAND6 + POL1 + SB2 + LNA_ENABLE);
    AmbRelativeAddr monRCA21(BAND6 + POL1 + SB2 + LNA2_DRAIN_VOLTAGE);
    AmbRelativeAddr monRCA22(BAND6 + POL1 + SB2 + LNA2_DRAIN_CURRENT);

    // Set up some RCAs for monitor and control of both LNA LED subystems:
    AmbRelativeAddr ctrlRCA30(CONTROL_RCA + BAND6 + LNA_LED_ENABLE);
    AmbRelativeAddr monRCA30(BAND6 + LNA_LED_ENABLE);

    AmbRelativeAddr ctrlRCA31(CONTROL_RCA + BAND6 + POL1 + LNA_LED_ENABLE);
    AmbRelativeAddr monRCA31(BAND6 + POL1 + LNA_LED_ENABLE);

    // Set up some RCAs for monitor and control of a two of the SIS subystems:
    AmbRelativeAddr ctrlRCA40(CONTROL_RCA + BAND6 + SIS_OPEN_LOOP);
    AmbRelativeAddr ctrlRCA41(CONTROL_RCA + BAND6 + SIS_VOLTAGE);

    AmbRelativeAddr monRCA40(BAND6 + SIS_OPEN_LOOP);
    AmbRelativeAddr monRCA41(BAND6 + SIS_VOLTAGE);

    AmbRelativeAddr ctrlRCA50(CONTROL_RCA + BAND6 + POL1 + SB2 + SIS_OPEN_LOOP);
    AmbRelativeAddr ctrlRCA51(CONTROL_RCA + BAND6 + POL1 + SB2 + SIS_VOLTAGE);

    AmbRelativeAddr monRCA50(BAND6 + POL1 + SB2 + SIS_OPEN_LOOP);
    AmbRelativeAddr monRCA51(BAND6 + POL1 + SB2 + SIS_VOLTAGE);

    // Set up some RCAs for monitor and control of two of the SIS magnet subystems:
    AmbRelativeAddr ctrlRCA60(CONTROL_RCA + BAND6 + SIS_MAGNET_CURRENT);
    AmbRelativeAddr monRCA60(BAND6 + SIS_MAGNET_CURRENT);

    AmbRelativeAddr ctrlRCA61(CONTROL_RCA + BAND6 + POL1 + SIS_MAGNET_CURRENT);
    AmbRelativeAddr monRCA61(BAND6 + POL1 + SIS_MAGNET_CURRENT);

    // Make sure all bands are off:
    allBandsOff();
    monitor(MON_NUM_ENABLED, "GET_POWER_DISTRIBUTION_POWERED_MODULES", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set band 6 to ON
    data_m[0] = 1;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
    SLEEP(1000);


    // Bias the LNAs:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA10, "LNA_ENABLE", &details, true);
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA20, "LNA_ENABLE", &details, true);

    // Set some LNA values:
    packSGL(1.0);
    command(ctrlRCA11, "LNA_DRAIN_VOLTAGE", &details, true);
    packSGL(1.0);
    command(ctrlRCA21, "LNA_DRAIN_VOLTAGE", &details, true);

    // Set some LNA values:
    packSGL(0.1);
    command(ctrlRCA12, "LNA1_DRAIN_CURRENT", &details, true);
    packSGL(0.1);
    command(ctrlRCA22, "LNA1_DRAIN_CURRENT", &details, true);

    // Monitor LNA enable:
    monitor(monRCA10, "LNA_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    monitor(monRCA20, "LNA_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    SLEEP(1000);

    // Monitor LNA Voltage and Current
    const float window(0.04);

    monitor(monRCA11, "LNA_DRAIN_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 1.0 - window < monValue && monValue < 1.0 + window);

    monitor(monRCA12, "LNA_DRAIN_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 0.1 - window < monValue && monValue < 0.1 + window);

    monitor(monRCA21, "LNA_DRAIN_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 1.0 - window < monValue && monValue < 1.0 + window);

    monitor(monRCA22, "LNA_DRAIN_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 0.1 - window < monValue && monValue < 1.0 + window);


    // Set LNA LED enable:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA30, "LNA_LED_ENABLE", &details, true);
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA31, "LNA_LED_ENABLE", &details, true);

    // Monitor LNA LED enable:
    monitor(monRCA30, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    monitor(monRCA31, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Set the SIS open loop control:
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA40, "SIS_OPEN_LOOP", &details, true);
    data_m[0] = 1;
    dataLength_m = 1;
    command(ctrlRCA50, "SIS_OPEN_LOOP", &details, true);

    // Monitor SIS open loop control:
    monitor(monRCA40, "SIS_OPEN_LOOP", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);
    monitor(monRCA50, "SIS_OPEN_LOOP", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 1);

    // Set the SIS closed loop control:
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA40, "SIS_OPEN_LOOP", &details, true);
    data_m[0] = 0;
    dataLength_m = 1;
    command(ctrlRCA50, "SIS_OPEN_LOOP", &details, true);

    // Monitor SIS closed loop control:
    monitor(monRCA40, "SIS_OPEN_LOOP", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    monitor(monRCA50, "SIS_OPEN_LOOP", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Set SIS voltage:
    packSGL(2.3);
    command(ctrlRCA41, "SIS_VOLTAGE", &details, true);
    packSGL(2.3);
    command(ctrlRCA51, "SIS_VOLTAGE", &details, true);

    // Monitor SIS voltage:
    monitor(monRCA41, "SIS_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 2.3 - 0.1 < monValue && monValue < 2.3 + 0.1);

    monitor(monRCA51, "SIS_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 2.3 - 0.1 < monValue && monValue < 2.3 + 0.1);

    // Set SIS magnet current:
    packSGL(5.1);
    command(ctrlRCA60, "SIS_MAGNET_CURRENT", &details, true);
    packSGL(5.1);
    command(ctrlRCA61, "SIS_MAGNET_CURRENT", &details, true);

    // Monitor SIS magnet current:
    monitor(monRCA60, "SIS_MAGNET_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 5.1 - 1.0 < monValue && monValue < 5.1 + 1.0);

    monitor(monRCA61, "SIS_MAGNET_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, 5.1 - 1.0 < monValue && monValue < 5.1 + 1.0);

    // Set band 6 to STANDBY2
    data_m[0] = 2;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);

    // Monitor LNA enable:
    monitor(monRCA10, "LNA_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    monitor(monRCA20, "LNA_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Monitor LNA Voltage and Current
    monitor(monRCA11, "LNA_DRAIN_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -window < monValue && monValue < window);

    monitor(monRCA12, "LNA_DRAIN_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -window < monValue && monValue < window);

    monitor(monRCA21, "LNA_DRAIN_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -window < monValue && monValue < window);

    monitor(monRCA22, "LNA_DRAIN_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -window < monValue && monValue < window);

    // Monitor LNA LED enable:
    monitor(monRCA30, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    monitor(monRCA31, "LNA_LED_ENABLE", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Monitor SIS open loop control:
    monitor(monRCA40, "SIS_OPEN_LOOP", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);
    monitor(monRCA50, "SIS_OPEN_LOOP", &details);
    CPPUNIT_ASSERT_MESSAGE(details, data_m[0] == 0);

    // Monitor SIS voltage:
    monitor(monRCA41, "SIS_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -0.1 < monValue && monValue < 0.1);

    monitor(monRCA51, "SIS_VOLTAGE", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -0.1 < monValue && monValue < 0.1);

    // Monitor SIS magnet current:
    monitor(monRCA60, "SIS_MAGNET_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -0.5 < monValue && monValue < 0.5);

    monitor(monRCA61, "SIS_MAGNET_CURRENT", &details);
    monValue = unpackSGL(&statusByte);
    details += " monValue=";
    details += to_string(monValue);
    CPPUNIT_ASSERT_MESSAGE(details, -0.5 < monValue && monValue < 0.5);

    // Set band 6 to OFF
    data_m[0] = 0;
    dataLength_m = 1;
    command(CTRL_ENABLE_BAND6, "CTRL_ENABLE_BAND6", &details, true);
}
