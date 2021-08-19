#include "CommandLossTestFixture.h"
#include <math.h>
#include <stringConvert.h>
#include <portable.h>
#include <string>
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CommandLossTestFixture);

void CommandLossTestFixture::setUp()
{
    AmbDeviceTestFixture::setUp();
    // Set FE Mode to normal:
    data_m[0] = 0;
    dataLength_m = 1;
    commandImpl(CTRL_FE_MODE, "CTRL_FE_MODE", NULL);
    // Make sure all bands are off:
    allBandsOff();
}

void CommandLossTestFixture::tearDown()
{
    // Make sure all bands are off:
    allBandsOff();
    AmbDeviceTestFixture::tearDown();
}

void CommandLossTestFixture::allBandsOff() {

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

void CommandLossTestFixture::getIVCurveDefaults(int band, float *VJlow_p, float *VJhigh_p, float *VJstep_p) const {
    float VJMax = 3.0;
        switch (band) {
            case 4:
                VJMax = 6.5;
                break;

            case 3:
            case 6:
                VJMax = 12.0;
                break;

            case 5:
            case 7:
            case 8:
            case 9:
            case 10:
            default:
                VJMax = 3.0;
                break;
        }
        if (VJlow_p)
            *VJlow_p = -VJMax;
        if (VJhigh_p)
            *VJhigh_p = VJMax;
        if (VJstep_p)
            *VJstep_p = 0.001;
}

bool CommandLossTestFixture::measureIVCurveInnerLoop(int band, int pol, int sb, float VJstart, float VJstop, float VJstep) {
    // Inner loop measures in one direction only.
    // NOTE: no provision to ensure exactly VJstop is measured.

    // Sanity check inputs:
    // Range is at least 1 step
    float range = VJstop - VJstart;

    if (fabsf(range) < fabsf(VJstep))
        return false;

    // Sign of range is same as step
    bool range_neg(range < 0.0);
    bool step_neg(VJstep < 0.0);

    if (range_neg != step_neg)
        return false;

    // move slowly to the first point:
    float VJset = VJstart;
    string details;
    packSGL(VJset);
    AmbRelativeAddr RCA = CONTROL_RCA + ((band - 1) << 12) + SIS_VOLTAGE;
    if (pol == 1)
        RCA += POL1;
    if (sb == 2)
        RCA += SB2;

    command(RCA, "SIS_VOLTAGE", &details, true);
    SLEEP(10);

    bool done = false;
    while (!done) {
        packSGL(VJset);
        command(RCA, "SIS_VOLTAGE", &details, true);

        // increment and loop end condition:
        VJset += VJstep;

        // check loop end condition:
        if (step_neg) {
            if (VJset <= VJstop)
                done = true;
        } else {
            if (VJset >= VJstop)
                done = true;
        }
    }
    packSGL(0.0);
    command(RCA, "SIS_VOLTAGE", &details, true);
    return true;
}

void CommandLossTestFixture::testIVCurve() {
    //!< Test that only band6 may be set to STANDBY2 mode and that proper errors are returned for other bands.

    string details;
    AmbRelativeAddr RCA;

    // Enable three bands:
    data_m[0] = 1;
    dataLength_m = 1;
    RCA = CTRL_ENABLE_BAND3;
    command(RCA, "SET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details, true);
    RCA = CTRL_ENABLE_BAND6;
    command(RCA, "SET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details, true);
    RCA = CTRL_ENABLE_BAND7;
    command(RCA, "SET_POWER_DISTRIBUTION_MODULEn_ENABLE", &details, true);

    SLEEP(10000);

    int maxIter(10);
    float VJlow, VJhigh, VJstep;
    for (int iter = 0; iter < maxIter; iter++) {
        getIVCurveDefaults(3, &VJlow, &VJhigh, &VJstep);
        measureIVCurveInnerLoop(3, 0, 1, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(3, 0, 1, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(3, 0, 2, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(3, 0, 2, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(3, 1, 1, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(3, 1, 1, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(3, 1, 2, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(3, 1, 2, VJhigh, VJlow, -VJstep);

        getIVCurveDefaults(6, &VJlow, &VJhigh, &VJstep);
        measureIVCurveInnerLoop(6, 0, 1, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(6, 0, 1, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(6, 0, 2, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(6, 0, 2, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(6, 1, 1, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(6, 1, 1, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(6, 1, 2, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(6, 1, 2, VJhigh, VJlow, -VJstep);

        getIVCurveDefaults(7, &VJlow, &VJhigh, &VJstep);
        measureIVCurveInnerLoop(7, 0, 1, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(7, 0, 1, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(7, 0, 2, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(7, 0, 2, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(7, 1, 1, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(7, 1, 1, VJhigh, VJlow, -VJstep);
        measureIVCurveInnerLoop(7, 1, 2, VJlow, VJhigh, VJstep);
        measureIVCurveInnerLoop(7, 1, 2, VJhigh, VJlow, -VJstep);
    }
}
