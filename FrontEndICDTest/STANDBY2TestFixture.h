/*
 * STANDBY2TestFixture.h
 *
 * The STANDBY2 mode of the FEMC firmware version 2.8.x is stated as:
 *  essentially the same as the ready state except that none of the 4K stage and 15K stage
 *  electronics may be biased when in this state. This mode was added to support a low-power
 *  standby mode for band 6 as described in CRE [AD8]. In this mode, any command to bias the
 *  cold stage electronics will be rejected and the status HARDW_BLOCKED_ERR will be stored
 *  as the status for the command, as described in section 3.4.2.2.2 ALMA-40.00.00.00-70.35.25.00-C-ICD
 *
 *  The mode will only be permitted for band 6
 *
 *  The firmware will be modified to allow up to four bands to be powered on at a time, so long
 *  as one of them is in Cartridge STANDBY2 mode.
 *
 *  LNA and SIS biasing, magnet, heater, etc. commands will be rejected for any cartridge in STANDBY2 mode
 *
 *  The SET_POWER_DISTRIBUTION_MODULE[Ca]_ENABLE command handler will be modified to a ccept payload
 *  values 0, 1, or 2;  Values 0 and 1 are OFF and ON, as before
 *
 *  If the payload is 2 then the cartridge will enter STANDBY2 mode and all SIS and LNA biases will be set to zero
 *
 *  The GET_POWER_DISTRIBUTION_MODULE[Ca]_ENABLE monitor request will also be modified so that the current
 *  state of any band can be queried, to include STANDBY2 mode
 *
 *  In the event any command (power-up, biasing, or STANDBY2) is rejected as described herein
 *  the status HARDW_BLOCKED_ERR will be stored as the status for the command as described in
 *  section 3.4.2.2.2 of ALMA-40.00.00.00-70.35.25.00-C-ICD.
 *
 */

#ifndef STANDBY2TestFixture_H_
#define STANDBY2TestFixture_H_

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class STANDBY2TestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(STANDBY2TestFixture);
    CPPUNIT_TEST(testOnlyBand6Allowed);
    CPPUNIT_TEST(testFourBandsAllowed);
    CPPUNIT_TEST(testCommandPayload);
    CPPUNIT_TEST(testStateTransitions);
    CPPUNIT_TEST(testSetLNA);
    CPPUNIT_TEST(testSetLNALED);
    CPPUNIT_TEST(testSetSIS);
    CPPUNIT_TEST(testSetSISMagnet);
    CPPUNIT_TEST(testSetSISHeater);
    CPPUNIT_TEST(testColdElectronicsOff);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void allBandsOff();

    void testOnlyBand6Allowed();
    //!< Test that only band6 may be set to STANDBY2 mode and that proper errors are returned for other bands.

    void testFourBandsAllowed();
    //!< Test that four bands may be powered on as long as one is in STANDBY2 mode.  Five not allowed.

    void testCommandPayload();
    //!< Test that only 0, 1, 2 are allowed payloads to the SET_POWER_DISTRIBUTION_MODULEn_ENABLE command

    void testStateTransitions();
    //!< Test all the state transitions between OFF, ON, STANDBY2

    void testSetLNA();
    //!< Test that LNA operation is blocked in STANDBY2 mode and allowed in ON mode

    void testSetLNALED();
    //!< Test that LNA LED operation is blocked in STANDBY2 mode and allowed in ON mode

    void testSetSIS();
    //!< Test that SIS operation is blocked in STANDBY2 mode and allowed in ON mode

    void testSetSISMagnet();
    //!< Test that SIS Magnet operation is blocked in STANDBY2 mode and allowed in ON mode

    void testSetSISHeater();
    //!< Test that SIS Heater operation is blocked in STANDBY2 mode and allowed in ON mode

    void testColdElectronicsOff();
    //!< Test that all cold eletronics are turned off when entering STANDBY2 mode.

private:

    void testLNAHelper(const AmbRelativeAddr &ctrlRCA0,
                       const AmbRelativeAddr &ctrlRCA1,
                       const AmbRelativeAddr &ctrlRCA2,
                       const AmbRelativeAddr &monRCA0,
                       const AmbRelativeAddr &monRCA1,
                       const AmbRelativeAddr &monRCA2,
                       const AmbRelativeAddr &monRCA3,
                       bool shouldSucceed);
    //!< Private helper function to test an LNA subsystem

    void testSISHelper(const AmbRelativeAddr &ctrlRCA0,
                       const AmbRelativeAddr &ctrlRCA1,
                       const AmbRelativeAddr &monRCA0,
                       const AmbRelativeAddr &monRCA1,
                       const AmbRelativeAddr &monRCA2,
                       bool shouldSucceed);
    //!< Private helper function to test an SIS subsystem

    void testSISMagnetHelper(const AmbRelativeAddr &ctrlRCA0,
                             const AmbRelativeAddr &monRCA0,
                             const AmbRelativeAddr &monRCA1,
                             bool shouldSucceed);
    //!< Private helper function to test an SIS magnet subsystem

    enum MonitorControlOffset {
        MON_NUM_ENABLED     = 0xA0A0,

        MON_ENABLE_BAND1    = 0xA00C,
        MON_ENABLE_BAND2    = 0xA01C,
        MON_ENABLE_BAND3    = 0xA02C,
        MON_ENABLE_BAND4    = 0xA03C,
        MON_ENABLE_BAND5    = 0xA04C,
        MON_ENABLE_BAND6    = 0xA05C,
        MON_ENABLE_BAND7    = 0xA06C,
        MON_ENABLE_BAND8    = 0xA07C,
        MON_ENABLE_BAND9    = 0xA08C,
        MON_ENABLE_BAND10   = 0xA09C,

        CTRL_ENABLE_BAND1   = 0x1A00C,
        CTRL_ENABLE_BAND2   = 0x1A01C,
        CTRL_ENABLE_BAND3   = 0x1A02C,
        CTRL_ENABLE_BAND4   = 0x1A03C,
        CTRL_ENABLE_BAND5   = 0x1A04C,
        CTRL_ENABLE_BAND6   = 0x1A05C,
        CTRL_ENABLE_BAND7   = 0x1A06C,
        CTRL_ENABLE_BAND8   = 0x1A07C,
        CTRL_ENABLE_BAND9   = 0x1A08C,
        CTRL_ENABLE_BAND10  = 0x1A09C,

        CONTROL_RCA             = 0x10000,  ///< add this to the monitor RCA to get the corresponding command RCA
        POL1                    = 0x0400,   ///< add this to the pol0 RCA to get the corresponding pol1 RCA
        SB2                     = 0x0080,   ///< add this to the sb1 RCA to get the corresponding sb2 RCA
        BAND6                   = 0x5000,

        SIS_VOLTAGE             = 0x0008,
        SIS_CURRENT             = 0x0010,
        SIS_OPEN_LOOP           = 0x0018,
        SIS_MAGNET_VOLTAGE      = 0x0020,
        SIS_MAGNET_CURRENT      = 0x0030,
        LNA_ENABLE              = 0x0058,
        LNA1_DRAIN_VOLTAGE      = 0x0040,
        LNA1_DRAIN_CURRENT      = 0x0041,
        LNA1_GATE_VOLTAGE       = 0x0042,
        LNA2_DRAIN_VOLTAGE      = 0x0044,
        LNA2_DRAIN_CURRENT      = 0x0045,
        LNA2_GATE_VOLTAGE       = 0x0046,
        LNA3_DRAIN_VOLTAGE      = 0x0048,
        LNA3_DRAIN_CURRENT      = 0x0049,
        LNA3_GATE_VOLTAGE       = 0x004A,

        LNA_LED_ENABLE          = 0x0100,
        SIS_HEATER_ENABLE       = 0x0180,
        SIS_HEATER_CURRENT      = 0x01C0,

    };
};

#endif /* STANDBY2TestFixture_H_ */
