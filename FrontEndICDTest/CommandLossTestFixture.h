/*
 * CommandLossTestFixture.h
 *
 * FEMC rare and intermittent apparent command failures:
 * https://jira.alma.cl/browse/PRTSIR-15987
 * https://jira.alma.cl/browse/PRTSIR-17781
 * probably others
 *
 * This test aims to send lots of commands to try to catch the bug in the act.
 *
 */

#ifndef CommandLossTestFixture_H_
#define CommandLossTestFixture_H_

#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <string>

class CommandLossTestFixture : public AmbDeviceTestFixture {
    CPPUNIT_TEST_SUITE(CommandLossTestFixture);
//    CPPUNIT_TEST(testIVCurve);
    CPPUNIT_TEST(testPPComm);
    CPPUNIT_TEST_SUITE_END();

public:
    void allBandsOff();

    void getIVCurveDefaults(int band, float *VJlow_p, float *VJhigh_p, float *VJstep_p) const;

    bool measureIVCurveInnerLoop(int band, int pol, int sb, float VJstart, float VJstop, float VJstep);

    void testIVCurve();

    void testPPComm();

private:

    enum MonitorControlOffset {
        MON_NUM_ENABLED         = 0xA0A0,
        CTRL_ENABLE_BAND1       = 0x1A00C,
        CTRL_ENABLE_BAND2       = 0x1A01C,
        CTRL_ENABLE_BAND3       = 0x1A02C,
        CTRL_ENABLE_BAND4       = 0x1A03C,
        CTRL_ENABLE_BAND5       = 0x1A04C,
        CTRL_ENABLE_BAND6       = 0x1A05C,
        CTRL_ENABLE_BAND7       = 0x1A06C,
        CTRL_ENABLE_BAND8       = 0x1A07C,
        CTRL_ENABLE_BAND9       = 0x1A08C,
        CTRL_ENABLE_BAND10      = 0x1A09C,

        CONTROL_RCA             = 0x10000,  ///< add this to the monitor RCA to get the corresponding command RCA
        POL1                    = 0x0400,   ///< add this to the pol0 RCA to get the corresponding pol1 RCA
        SB2                     = 0x0080,   ///< add this to the sb1 RCA to get the corresponding sb2 RCA
        GET_PPCOMM_TIME         = 0x20007,
        SET_PPCOMM_BYTES        = 0x21007,

        CTRL_FE_MODE            = 0x2100E,

        SIS_VOLTAGE             = 0x0008,
        SIS_CURRENT             = 0x0010
    };
};

#endif /* CommandLossTestFixture */
