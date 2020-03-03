/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) Associated Universities Inc., 2010
*
*This library is free software; you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation; either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*
*
* Implementation note:
* While the ICD names several control points in the AMC, most of these cannot
* actually be manipulated in production hardware.  They are left in for future
* development in case they are needed in band 10 or elsewhere.
*
* The following control have no effect for any production WCA:
* SET_CARTRIDGE[Ca]_LO_AMC_DRAIN_B_VOLTAGE
* SET_CARTRIDGE[Ca]_LO_AMC_MULTIPLIER_D_COUNTS
* SET_CARTRIDGE[Ca]_LO_AMC_GATE_E_VOLTAGE
* SET_CARTRIDGE[Ca]_LO_AMC_DRAIN_E_VOLTAGE
*
* Therefore we do not test controlling these points.
* We do read the corresponding monitor points and check data length, range, etc.
*
*/

#include "WCATestFixture.h"
#include "stringConvert.h"
#include <string>
#include <logger.h>
#include <cmath>
using namespace std;


WCATestFixture::WCATestFixture(int band,
                               const std::string &name,
                               AmbChannel channel,
                               AmbNodeAddr nodeAddress,
                               bool expectStatusByte)
  : CartridgeTestFixture(band, name, channel, nodeAddress, expectStatusByte)
{
    ytoCoarseTune_RCA               = cartBaseRCA_m + YTO_COARSE_TUNE;
    photomixerEnable_RCA            = cartBaseRCA_m + PHOTOMIXER_ENABLE;
    photomixerVoltage_RCA           = cartBaseRCA_m + PHOTOMIXER_VOLTAGE;
    photomixerCurrent_RCA           = cartBaseRCA_m + PHOTOMIXER_CURRENT;
    pllLockDetectVoltage_RCA        = cartBaseRCA_m + PLL_LOCK_DETECT_VOLTAGE;
    pllCorrectionVoltage_RCA        = cartBaseRCA_m + PLL_CORRECTION_VOLTAGE;
    pllAssemblyTemp_RCA             = cartBaseRCA_m + PLL_ASSEMBLY_TEMP;
    pllYTOHeaterCurrent_RCA         = cartBaseRCA_m + PLL_YTO_HEATER_CURRENT;
    pllRefTotalPower_RCA            = cartBaseRCA_m + PLL_REF_TOTAL_POWER;
    pllIfTotalPower_RCA             = cartBaseRCA_m + PLL_IF_TOTAL_POWER;
    pllUnlockDetectLatch_RCA        = cartBaseRCA_m + PLL_UNLOCK_DETECT_LATCH;
    pllClearUnlockDetectLatch_RCA   = cartBaseRCA_m + PLL_CLEAR_UNLOCK_DETECT_LATCH;
    pllLoopBandwidthSelect_RCA      = cartBaseRCA_m + PLL_LOOP_BANDWIDTH_SELECT;
    pllSidebandLockSelect_RCA       = cartBaseRCA_m + PLL_SIDEBAND_LOCK_POL_SELECT;
    pllNullLoopIntegrator_RCA       = cartBaseRCA_m + PLL_NULL_LOOP_INTEGRATOR;
    amcGateAVoltage_RCA             = cartBaseRCA_m + AMC_GATE_A_VOLTAGE;
    amcDrainAVoltage_RCA            = cartBaseRCA_m + AMC_DRAIN_A_VOLTAGE;
    amcDrainACurrent_RCA            = cartBaseRCA_m + AMC_DRAIN_A_CURRENT;
    amcGateBVoltage_RCA             = cartBaseRCA_m + AMC_GATE_B_VOLTAGE;
    amcDrainBVoltage_RCA            = cartBaseRCA_m + AMC_DRAIN_B_VOLTAGE;
    amcDrainBCurrent_RCA            = cartBaseRCA_m + AMC_DRAIN_B_CURRENT;
    amcMultiplierDCounts_RCA        = cartBaseRCA_m + AMC_MULTIPLIER_D_COUNTS;
    amcGateEVoltage_RCA             = cartBaseRCA_m + AMC_GATE_E_VOLTAGE;
    amcDrainEVoltage_RCA            = cartBaseRCA_m + AMC_DRAIN_E_VOLTAGE;
    amcDrainECurrent_RCA            = cartBaseRCA_m + AMC_DRAIN_E_CURRENT;
    amcMultiplierDCurrent_RCA       = cartBaseRCA_m + AMC_MULTIPLIER_D_CURRENT;
    amcSupplyVoltage5V_RCA          = cartBaseRCA_m + AMC_SUPPLY_VOLTAGE_5V;
    paPol0GateVoltage_RCA           = cartBaseRCA_m + PA_GATE_VOLTAGE;
    paPol0DrainVoltageScale_RCA     = cartBaseRCA_m + PA_DRAIN_VOLTAGE;
    paPol0DrainCurrent_RCA          = cartBaseRCA_m + PA_DRAIN_CURRENT;
    paPol1GateVoltage_RCA           = cartBaseRCA_m + 4 + PA_GATE_VOLTAGE;
    paPol1DrainVoltageScale_RCA     = cartBaseRCA_m + 4 + PA_DRAIN_VOLTAGE;
    paPol1DrainCurrent_RCA          = cartBaseRCA_m + 4 + PA_DRAIN_CURRENT;
    paSupplyVoltage3V_RCA           = cartBaseRCA_m + PA_SUPPLY_VOLTAGE_3V;
    paSupplyVoltage5V_RCA           = cartBaseRCA_m + PA_SUPPLY_VOLTAGE_5V;

    //Control RCAs
    ctrlytoCoarseTune_RCA               = controlRCA + ytoCoarseTune_RCA;
    ctrlphotomixerEnable_RCA            = controlRCA + photomixerEnable_RCA;
    ctrlphotomixerVoltage_RCA           = controlRCA + photomixerVoltage_RCA;
    ctrlpllLoopBandwidthSelect_RCA      = controlRCA + pllLoopBandwidthSelect_RCA;
    ctrlpllClearUnlockDetectLatch_RCA   = controlRCA + pllClearUnlockDetectLatch_RCA;
    ctrlpllSidebandLockSelect_RCA       = controlRCA + pllSidebandLockSelect_RCA;
    ctrlpllNullLoopIntegrator_RCA       = controlRCA + pllNullLoopIntegrator_RCA;
    ctrlamcDrainBVoltage_RCA            = controlRCA + amcDrainBVoltage_RCA;
    ctrlamcGateEVoltage_RCA             = controlRCA + amcGateEVoltage_RCA;
    ctrlamcDrainEVoltage_RCA            = controlRCA + amcDrainEVoltage_RCA;
    ctrlpaPol0GateVoltage_RCA           = controlRCA + paPol0GateVoltage_RCA;
    ctrlpaPol1GateVoltage_RCA           = controlRCA + paPol1GateVoltage_RCA;
    ctrlpaPol0DrainVoltageScale_RCA     = controlRCA + paPol0DrainVoltageScale_RCA;
    ctrlpaPol1DrainVoltageScale_RCA     = controlRCA + paPol1DrainVoltageScale_RCA;
    ctrlamcMultiplierDCounts_RCA        = controlRCA + amcMultiplierDCounts_RCA;

    ctrlClearPALimits_RCA               = CLEAR_PA_LIMITS + band_m - 1;
    ctrlSetPALimitsEntry_RCA            = PA_LIMITS_ENTRY + band_m - 1;
}

std::vector<float> WCATestFixture::SetValuesLOpaGateVoltage;
const float WCATestFixture::ValidMinLOpaGateVoltage=-5.0;
const float WCATestFixture::ValidMaxLOpaGateVoltage=0;
const float WCATestFixture::ToleranceLOpaGateVoltage=0.1;

std::vector<float> WCATestFixture::SetValuesLOpaDrainVoltageScale;
const float WCATestFixture::ValidMinLOpaDrainVoltageScale=0;
const float WCATestFixture::ValidMaxLOpaDrainVoltageScale=2.5;
const float WCATestFixture::ToleranceLOpaDrainVoltageScale=0.1;

void WCATestFixture::setUp() {
	CartridgeTestFixture::setUp();

	///PA gate Voltages
	///Values in the actual settable range, except last value which is illegal
	SetValuesLOpaGateVoltage.reserve(3);
	SetValuesLOpaGateVoltage.resize(0);
	SetValuesLOpaGateVoltage.push_back(-0.2);
	SetValuesLOpaGateVoltage.push_back(-0.5);
	SetValuesLOpaGateVoltage.push_back(20.0);

	///PA drain Voltages
	///Values in the actual settable range, except last value which is illegal
	SetValuesLOpaDrainVoltageScale.reserve(3);
	SetValuesLOpaDrainVoltageScale.resize(0);
	SetValuesLOpaDrainVoltageScale.push_back(1.0);
	SetValuesLOpaDrainVoltageScale.push_back(2.0);
	SetValuesLOpaDrainVoltageScale.push_back(20.0);
	SLEEP(100);
}
void WCATestFixture::tearDown() {
    resetAmbVars();
    data_m[0] = 0;
    dataLength_m = 1;
    // Clear any existing PA limits table:
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrlClearPALimits_RCA, "WCATestFixture::tearDown");

    if (isEnabled_m) {

        // Set the YTO to 0:
        SLEEP(100);
        packU16(0);     // YTO = 0
        commandImpl(ctrlytoCoarseTune_RCA, "WCATestFixture::tearDown");

        SLEEP(100);

        // Set the loop bw select back to normal:
        data_m[0] = 0;
        dataLength_m = 1;
        if (band_m == 3 || band_m == 6 || band_m == 7)
            data_m[0] = 1;
        commandImpl(ctrlpllLoopBandwidthSelect_RCA, "WCATestFixture::tearDown");

        SLEEP(100);

        // Set the LO PA gates and drains to off:
        packSGL(0.0);
        commandImpl(ctrlpaPol0GateVoltage_RCA, "WCATestFixture::tearDown");
        commandImpl(ctrlpaPol1GateVoltage_RCA, "WCATestFixture::tearDown");

        SLEEP(100);

        commandImpl(ctrlpaPol0DrainVoltageScale_RCA, "WCATestFixture::tearDown");
        SLEEP(100);
        commandImpl(ctrlpaPol1DrainVoltageScale_RCA, "WCATestFixture::tearDown");
    }
	CartridgeTestFixture::tearDown();
}

/**
 * MONITOR POINTS
 */
void WCATestFixture::testGET_CARTRIDGE_LO_PHOTOMIXER_VOLTAGE(){
	implGetFloat(photomixerVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_PHOTOMIXER_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PHOTOMIXER_CURRENT(){
	implGetFloat(photomixerCurrent_RCA,-50.0, 5.0,"GET_CARTRIDGE_LO_PHOTOMIXER_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PLL_LOCK_DETECT_VOLTAGE(){
	implGetFloat(pllLockDetectVoltage_RCA,-10.0, 10.0,"GET_CARTRIDGE_LO_PLL_LOCK_DETECT_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PLL_CORRECTION_VOLTAGE(){
	implGetFloat(pllCorrectionVoltage_RCA,-10.0, 10.0,"GET_CARTRIDGE_LO_PLL_LOCK_CORRECTION_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PLL_ASSEMBLY_TEMP(){
	implGetFloat(pllAssemblyTemp_RCA,-500.0, 500.0,"GET_CARTRIDGE_LO_PLL_ASSEMBLY_TEMP");
}
void WCATestFixture::testGET_CARTRIDGE_LO_YIG_HEATER_CURRENT(){
	implGetFloat(pllYTOHeaterCurrent_RCA,-50.0, 350.0,"GET_CARTRIDGE_LO_YIG_HEATER_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PLL_REF_TOTAL_POWER(){
	implGetFloat(pllRefTotalPower_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_PLL_REF_TOTAL_POWER");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PLL_IF_TOTAL_POWER(){
	implGetFloat(pllIfTotalPower_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_PLL_IF_TOTAL_POWER");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_GATE_A_VOLTAGE(){
    implGetFloat(amcGateAVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_AMC_GATE_A_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_DRAIN_A_VOLTAGE(){
    implGetFloat(amcDrainAVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_AMC_DRAIN_A_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_DRAIN_A_CURRENT(){
    implGetFloat(amcDrainACurrent_RCA,-500.0, 500.0,"GET_CARTRIDGE_LO_AMC_DRAIN_A_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_GATE_B_VOLTAGE(){
    implGetFloat(amcGateBVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_AMC_GATE_B_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE(){
    implGetFloat(amcDrainBVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_DRAIN_B_CURRENT(){
    implGetFloat(amcDrainBCurrent_RCA,-500.0, 500.0,"GET_CARTRIDGE_LO_AMC_DRAIN_B_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE(){
    implGetFloat(amcGateEVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE(){
    implGetFloat(amcDrainEVoltage_RCA,-5.0, 5.0,"GET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_DRAIN_E_CURRENT(){
    implGetFloat(amcDrainECurrent_RCA,-500.0, 500.0,"GET_CARTRIDGE_LO_AMC_DRAIN_E_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS(){
    unsigned char statusByte;
    monitorU8(amcMultiplierDCounts_RCA, "testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS", NULL, &statusByte);
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_CURRENT(){
	implGetFloat(amcMultiplierDCurrent_RCA,-50, 50.0,"GET_CARTRIDGE_LO_AMC_MULTIPLIER_D_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PA_POL0_DRAIN_CURRENT(){
	implGetFloat(paPol0DrainCurrent_RCA ,-500.0, 500.0,"GET_CARTRIDGE_LO_PA_POL0_DRAIN_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PA_POL1_DRAIN_CURRENT(){
	implGetFloat(paPol1DrainCurrent_RCA ,-500.0, 500.0,"GET_CARTRIDGE_LO_PA_POL0_DRAIN_CURRENT");
}
void WCATestFixture::testGET_CARTRIDGE_LO_AMC_SUPPLY_VOLTAGE_5V(){
	implGetFloat(amcSupplyVoltage5V_RCA,-10.0, 10.0,"GET_CARTRIDGE_LO_AMC_SUPPLY_VOLTAGE_5V");
}
void WCATestFixture::testGET_CARTRIDGE_LO_PA_SUPPLY_VOLTAGE_5V(){
	implGetFloat(paSupplyVoltage5V_RCA,-10.0, 10.0,"testGET_CARTRIDGE_LO_PA_SUPPLY_VOLTAGE_5V");
}


/**
 * CONTROL POINTS
 */
void WCATestFixture::testSET_CARTRIDGE_LO_YTO_COARSE_TUNE(){
    ///Values in the actual settable range, except last value which is illegal
    std::vector<unsigned short> SetValuesYTOCoarseTune;
    SetValuesYTOCoarseTune.reserve(5);
    SetValuesYTOCoarseTune.resize(0);
    SetValuesYTOCoarseTune.push_back(1);
    SetValuesYTOCoarseTune.push_back(2);
    SetValuesYTOCoarseTune.push_back(100);
    SetValuesYTOCoarseTune.push_back(4095);
    SetValuesYTOCoarseTune.push_back(10000);
	implSET_WCA_VALUE_U16(ytoCoarseTune_RCA,ctrlytoCoarseTune_RCA,
			SetValuesYTOCoarseTune, 0, 4095, 0,
			"SET_CARTRIDGE_LO_YTO_COARSE_TUNE");
}
void WCATestFixture::testSET_CARTRIDGE_LO_PHOTOMIXER_ENABLE() {
	implToggleEnable(photomixerEnable_RCA  ,ctrlphotomixerEnable_RCA,0,
			         "SET_CARTRIDGE_LO_PHOTOMIXER_ENABLE OFF");
	implToggleEnable(photomixerEnable_RCA  ,ctrlphotomixerEnable_RCA,1,
				     "SET_CARTRIDGE_LO_PHOTOMIXER_ENABLE ON");
	implToggleEnable(photomixerEnable_RCA  ,ctrlphotomixerEnable_RCA,2,
				     "SET_CARTRIDGE_LO_PHOTOMIXER_ENABLE 2 (ILLEGAL VALUE)");
}

void WCATestFixture::testSET_CARTRIDGE_LO_PLL_CLEAR_UNLOCK_DETECT_LATCH() {
    // The ICD describes the data to this control point as "dummy" meaning
    // it doesn't matter what data is set and there is no possible illegal value.
    // This test case is ok as now written but to truly test the operation
    // of this control point would require the ability to lock the WCA, which is not an ICD concern.

	string info,strSet,strReturn;
	ostringstream bufSet,bufReturn;

	//Set 0
	bufSet.str("");
	bufReturn.str("");
	data_m[0] = 0;
	dataLength_m = 1;
	bufSet <<  "\nSetVal=0.";
	strSet = bufSet.str();
	command(ctrlpllClearUnlockDetectLatch_RCA, strSet, &info);
	SLEEP(500);
	monitor(ctrlpllClearUnlockDetectLatch_RCA, strSet, &info);
	//Add monitored value to info string
	bufReturn <<  info << ", MonitoredVal=" << data_m[0] << ".";
	strReturn = bufReturn.str();
	CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 2);
	CPPUNIT_ASSERT_MESSAGE(strReturn, data_m[0] == 0);

	//Set 1
	bufSet.str("");
	bufReturn.str("");
	data_m[0] = 1;
	dataLength_m = 1;
	bufSet <<  "\nSetVal=1.";
	strSet = bufSet.str();
	command(ctrlpllClearUnlockDetectLatch_RCA, strSet, &info);
	SLEEP(500);
	monitor(ctrlpllClearUnlockDetectLatch_RCA, strSet, &info);
	//Add monitored value to info string
	bufReturn <<  info << ", MonitoredVal=" << data_m[0] << ".";
	strReturn = bufReturn.str();
	CPPUNIT_ASSERT_MESSAGE(strReturn, dataLength_m == 2);
	CPPUNIT_ASSERT_MESSAGE(strReturn, data_m[0] == 1);
}
void WCATestFixture::testSET_CARTRIDGE_LO_PLL_LOOP_BANDWIDTH_SELECT(){
    implToggleEnable(pllLoopBandwidthSelect_RCA,ctrlpllLoopBandwidthSelect_RCA,0,
                     "SET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT 0");
    implToggleEnable(pllLoopBandwidthSelect_RCA,ctrlpllLoopBandwidthSelect_RCA,1,
                     "SET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT 1");
    implToggleEnable(pllLoopBandwidthSelect_RCA,ctrlpllLoopBandwidthSelect_RCA,2,
                     "SET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT 2 (ILLEGAL VALUE)");
}
void WCATestFixture::testSET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT() {
	implToggleEnable(pllSidebandLockSelect_RCA,ctrlpllSidebandLockSelect_RCA,0,
			         "SET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT 0");
	implToggleEnable(pllSidebandLockSelect_RCA,ctrlpllSidebandLockSelect_RCA,1,
			         "SET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT 1");
	implToggleEnable(pllSidebandLockSelect_RCA,ctrlpllSidebandLockSelect_RCA,2,
			         "SET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT 2 (ILLEGAL VALUE)");
}
void WCATestFixture::testSET_CARTRIDGE_LO_NULL_LOOP_INTEGRATOR() {
	implToggleEnable(pllNullLoopIntegrator_RCA,ctrlpllNullLoopIntegrator_RCA,0,
			         "SET_CARTRIDGE_LO_NULL_LOOP_INTEGRATOR 0");
	implToggleEnable(pllNullLoopIntegrator_RCA,ctrlpllNullLoopIntegrator_RCA,1,
			         "SET_CARTRIDGE_LO_NULL_LOOP_INTEGRATOR 1");
	implToggleEnable(pllNullLoopIntegrator_RCA,ctrlpllNullLoopIntegrator_RCA,2,
			         "SET_CARTRIDGE_LO_NULL_LOOP_INTEGRATOR 2 (ILLEGAL VALUE)");
}
void WCATestFixture::testSET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS(){
    CPPUNIT_FAIL("Not testing SET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS");
}
void WCATestFixture::testSET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE(){
    CPPUNIT_FAIL("Not testing SET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE.");
}
void WCATestFixture::testSET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE(){
    CPPUNIT_FAIL("Not testing SET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE.");
}
void WCATestFixture::testSET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE(){
    CPPUNIT_FAIL("Not testing SET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE.");
}
void WCATestFixture::testSET_CARTRIDGE_LO_PA_POL0_GATE_VOLTAGE(){
	implGenericTest(paPol0GateVoltage_RCA,ctrlpaPol0GateVoltage_RCA,
			SetValuesLOpaGateVoltage, ValidMinLOpaGateVoltage, ValidMaxLOpaGateVoltage,
			ToleranceLOpaGateVoltage,"SET_CARTRIDGE_LO_PA_POL0_GATE_VOLTAGE");
}
void WCATestFixture::testSET_CARTRIDGE_LO_PA_POL1_GATE_VOLTAGE(){
	implGenericTest(paPol1GateVoltage_RCA,ctrlpaPol1GateVoltage_RCA,
			SetValuesLOpaGateVoltage, ValidMinLOpaGateVoltage, ValidMaxLOpaGateVoltage,
			ToleranceLOpaGateVoltage,"SET_CARTRIDGE_LO_PA_POL1_GATE_VOLTAGE");
}
void WCATestFixture::testSET_CARTRIDGE_LO_PA_POL0_DRAIN_VOLTAGE_SCALE(){
	implGenericTest(paPol0DrainVoltageScale_RCA,ctrlpaPol0DrainVoltageScale_RCA,
			SetValuesLOpaDrainVoltageScale, ValidMinLOpaDrainVoltageScale, ValidMaxLOpaDrainVoltageScale,
			ToleranceLOpaDrainVoltageScale,"SET_CARTRIDGE_LO_PA_POL0_DRAIN_VOLTAGE_SCALE");
}
void WCATestFixture::testSET_CARTRIDGE_LO_PA_POL1_DRAIN_VOLTAGE_SCALE(){
	implGenericTest(paPol1DrainVoltageScale_RCA,ctrlpaPol1DrainVoltageScale_RCA,
			SetValuesLOpaDrainVoltageScale, ValidMinLOpaDrainVoltageScale, ValidMaxLOpaDrainVoltageScale,
			ToleranceLOpaDrainVoltageScale,"SET_CARTRIDGE_LO_PA_POL1_DRAIN_VOLTAGE_SCALE");
}

void WCATestFixture::testSET_CARTRIDGE_LO_PA_LIMITS_TABLE() {
    string info, ytoString, msg;
    unsigned short yto = 0;

    // Step across the YTO tuning range:
    for (yto = 0; yto <= 4096; yto += 256) {
        // To measure the last endpoint at 4095:
        if (yto == 4096)
            yto--;

        SLEEP(100);
        packU16(yto);
        ytoString = to_string<unsigned short>(yto);
        msg = string("YTO_COARSE_TUNE ") + ytoString;
        LOG(LM_INFO) << msg << endl;
        command(ctrlytoCoarseTune_RCA, msg, &info);

        SLEEP(100);

        // Check that we can set the PA to 1.0 all across the range:
        //  Not setting 2.5 because that may be unsafe for some bands.
        packSGL(1.0);
        command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
        SLEEP(10);
        command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
    }

    // Set the LO PAs back to 0:
    packU16(0);     // YTO = 0
    command(ctrlytoCoarseTune_RCA, "YTO_COARSE_TUNE 0", &info);
    packSGL(0.0);   // PAVD = 0
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE1", &info);

    // Clear any existing PA limits table:
    resetAmbVars();
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrlClearPALimits_RCA, "CLEAR_PA_LIMITS", &info);

    // Now set up a PA LIMITS table:
    // YTO  maxVD0  maxVD1
    // 0    0.5     0.5
    // 1000 0.5     0.5
    // 1001 0.8     1.0
    // 2000 0.8     1.0
    // 2001 1.0     1.0
    // 4095 1.0     1.0

    resetAmbVars();
    data_m[0] = 2;      // pol=both
    dataLength_m = 1;
    packU16(0, true);   // append yto=0
    packSGL(0.5, true); // append 0.5
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 2;      // pol=both
    dataLength_m = 1;
    packU16(1000, true);// append yto=1000
    packSGL(0.5, true); // append 0.5
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 0;      // pol=0
    dataLength_m = 1;
    packU16(1001, true);// append yto=1001
    packSGL(0.8, true); // append 0.8
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 1;      // pol=1
    dataLength_m = 1;
    packU16(1001, true);// append yto=1001
    packSGL(1.0, true); // append 1.0
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 0;
    dataLength_m = 1;
    packU16(2000, true);
    packSGL(0.8, true);
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 1;
    dataLength_m = 1;
    packU16(2000, true);
    packSGL(1.0, true);
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 2;
    dataLength_m = 1;
    packU16(2001, true);
    packSGL(1.0, true);
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    resetAmbVars();
    data_m[0] = 2;
    dataLength_m = 1;
    packU16(4095, true);
    packSGL(1.0, true);
    commandImpl(ctrlSetPALimitsEntry_RCA, "PA_LIMITS_ENTRY");

    // Now check each limit zone:
    LOG(LM_INFO) << "WCATestFixture::testSET_CARTRIDGE_LO_PA_LIMITS_TABLE: YTO=0" << endl;
    resetAmbVars();
    packU16(0);     // YTO = 0
    command(ctrlytoCoarseTune_RCA, "YTO_COARSE_TUNE 0", &info);
    packSGL(0.5);   // PAVD = 0.5
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=0.5", &info);
    packSGL(0.5);   // PAVD = 0.5
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=0.5", &info);
    packSGL(1.0);   // PAVD = 1.0
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=1.0", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);
    packSGL(1.0);   // PAVD = 1.0
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=1.0", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);
    packSGL(0.0);   // PAVD = 0
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE1", &info);

    LOG(LM_INFO) << "WCATestFixture::testSET_CARTRIDGE_LO_PA_LIMITS_TABLE: YTO=500" << endl;
    resetAmbVars();
    packU16(500);   // YTO = 500
    command(ctrlytoCoarseTune_RCA, "YTO_COARSE_TUNE 500", &info);
    packSGL(0.5);
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=0.5 at 500", &info);
    packSGL(0.5);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=0.5 at 500", &info);
    packSGL(1.0);
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=1.0 at 500", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);
    packSGL(1.0);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=1.0 at 500", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);

    packSGL(0.0);   // PAVD = 0
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE1", &info);

    LOG(LM_INFO) << "WCATestFixture::testSET_CARTRIDGE_LO_PA_LIMITS_TABLE: YTO=1500" << endl;
    resetAmbVars();
    packU16(1500);   // YTO = 1500
    command(ctrlytoCoarseTune_RCA, "YTO_COARSE_TUNE 1500", &info, false);
    packSGL(0.8);
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=0.8 at 1500", &info);
    packSGL(0.8);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=0.8 at 1500", &info);
    packSGL(1.0);
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=1.0 at 1500", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);
    packSGL(1.0);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=1.0 at 1500", &info);

    packSGL(0.0);   // PAVD = 0
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE1", &info);

    LOG(LM_INFO) << "WCATestFixture::testSET_CARTRIDGE_LO_PA_LIMITS_TABLE: YTO=2500" << endl;
    packU16(2500);   // YTO = 2500
    command(ctrlytoCoarseTune_RCA, "YTO_COARSE_TUNE 2500", &info, false);
    packSGL(1.0);
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=1.0 at 2500", &info);
    packSGL(1.0);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=1.0 at 2500", &info);
    packSGL(1.1);
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 0=1.1 at 2500", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);
    packSGL(1.1);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE 1=1.1 at 2500", &info, false);
    CPPUNIT_ASSERT_MESSAGE(info, data_m[dataLength_m - 1] == FEMC_HARDW_BLOCKED_ERR);

    packSGL(0.0);   // PAVD = 0
    command(ctrlpaPol0DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE0", &info);
    command(ctrlpaPol1DrainVoltageScale_RCA, "PA_DRAIN_VOLTAGE1", &info);

    // Clear any existing PA limits table:
    data_m[0] = 1;
    dataLength_m = 1;
    commandImpl(ctrlClearPALimits_RCA, "CLEAR_PA_LIMITS", &info);
}

/**
 * Helper functions
 */


void WCATestFixture::implSET_WCA_VALUE_FLOAT(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
		std::vector<float> &test_values, const float validmin, const float validmax,
        const float tolerance, const std::string &msg){

	string info,strSet,strReturn;
	ostringstream bufSet,bufReturn;
	bool InValidRange;
	unsigned int i;
	float OriginalMonitorValue;
	unsigned char statusByte;

	//LOG(LM_INFO) << "Number of elements in array=" << test_values.size() << endl;

	//Loop over test_values array
	for (i=0;i< test_values.size();i++){
		InValidRange = (test_values[i] >= validmin) && (test_values[i] <= validmax);
		resetAmbVars();
		//LOG(LM_INFO) << "test_values[i]=" << test_values[i] << endl;

		//Set 0 initially
		packSGL(0);
		commandImpl(control_RCA, "initially setting to zero.\n", &info);
		SLEEP(100);
		monitor(monitor_RCA, strSet, &info);
		OriginalMonitorValue = unpackSGL();

		info= "";
		bufSet.str("");
		bufReturn.str("");

		//Append set value to the message
		bufSet <<  "\n" << msg << ", SetValue=" << test_values[i] <<
				",Range=[" << validmin << "," << validmax << "].";
		strSet = bufSet.str();

		//set control value
		packSGL(test_values[i]);
		command(control_RCA, strSet, &strSet, false);
		SLEEP(100);

		//assert correct datalength
		CPPUNIT_ASSERT_MESSAGE(strSet, dataLength_m == 5);
		//get statusbyte
		unpackSGL(&statusByte);

		//If SetValue is illegal, cache the result for the final check at the end.
		if (InValidRange){
			//If SetValue is legal
			CPPUNIT_ASSERT_MESSAGE(strSet, statusByte == FEMC_NO_ERROR);
		}
		else{
			//If SetValue is illegal
			CPPUNIT_ASSERT_MESSAGE(strSet + ": Error not raised for illegal set value.", statusByte != FEMC_NO_ERROR);
		}
		//read monitor point
		monitor(monitor_RCA, strSet, &strSet);

		//assert correct datalength
		CPPUNIT_ASSERT_MESSAGE(strSet, dataLength_m == 5);

		float FinalMonitorValue(unpackSGL(&statusByte));
		//LOG(LM_INFO) << hex << monitor_RCA << endl;
		//LOG(LM_INFO) << "Target Value=" << test_values[i] << endl;
		//LOG(LM_INFO) << "Monitored Value=" << FinalMonitorValue << endl << endl;


		bufReturn <<  info << " \nSetValue= " << test_values[i] << ", OriginalMonitorValue="
				<< OriginalMonitorValue << ", FinalMonitoredValue="
				<< FinalMonitorValue << ",Range=[" << validmin << "," << validmax << "], Tolerance="
				<< tolerance << ".\n";
		strReturn = bufReturn.str();


		if (!InValidRange){
			//if SetValue is illegal
			//assert unchanged within tolerance
			CPPUNIT_ASSERT_MESSAGE(strReturn + ": Illegal value not ignored as expected.", fabs(OriginalMonitorValue - FinalMonitorValue) <= tolerance);
		}
		else{
			//if SetValue is legal
			//assert that monitored value is close enough to the target value
			CPPUNIT_ASSERT_MESSAGE(strReturn + ": Monitored value not close enough to set value.", fabs(test_values[i] - FinalMonitorValue) <= tolerance);
		}
	}//end for loop

	//Set to 0 after test has been performed
	packSGL(0);
	info = "";
	commandImpl(control_RCA, "Setting to zero after performing test.\n", &info);
	SLEEP(100);
}
void WCATestFixture::implSET_WCA_VALUE_U16(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
		std::vector<unsigned short> &test_values, const unsigned short validmin, const unsigned short validmax,
        const unsigned short tolerance, const std::string &msg){

	string info,strSet,strReturn;
	ostringstream bufSet,bufReturn;
	bool InValidRange;
	unsigned int i;
	unsigned short OriginalMonitorValue;
	unsigned char statusByte;

	//LOG(LM_INFO) << "Number of elements in array=" << test_values.size() << endl;

	//Loop over test_values array
	for (i=0;i< test_values.size();i++){
		InValidRange = (test_values[i] >= validmin) && (test_values[i] <= validmax);
		resetAmbVars();
		LOG(LM_INFO) << "test_values[i]=" << test_values[i] << endl;

		//Set 0 initially
		packU16(0);
		commandImpl(control_RCA, "initially setting to zero.\n", &info);
		SLEEP(100);
		monitor(monitor_RCA, strSet, &info);
		OriginalMonitorValue = unpackU16();

		info= "";
		bufSet.str("");
		bufReturn.str("");

		//Append set value to the message
		bufSet <<  "\n" << msg << ", SetValue=" << test_values[i] <<
				",Range=[" << validmin << "," << validmax << "].";
		strSet = bufSet.str();

		//set control value
		packU16(test_values[i]);
		command(control_RCA, strSet, &info, false);
		SLEEP(100);

		//LOG(LM_INFO) << "(U16) datalength_m=" << dataLength_m << endl;

		//assert correct datalength
		CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 3);
		//get statusbyte
		unpackU16(&statusByte);

		//If SetValue is illegal, cache the result for the final check at the end.
		if (InValidRange){
			//If SetValue is legal
			CPPUNIT_ASSERT_MESSAGE(info, statusByte == FEMC_NO_ERROR);
		}
		else{
			//If SetValue is illegal
			CPPUNIT_ASSERT_MESSAGE(info + ": Error not raised for illegal set value.", statusByte != FEMC_NO_ERROR);
		}
		//read monitor point
		monitor(monitor_RCA, strSet, &info);

		//assert correct datalength
		CPPUNIT_ASSERT_MESSAGE(info, dataLength_m == 3);

		unsigned short FinalMonitorValue(unpackU16(&statusByte));
		//LOG(LM_INFO) << hex << monitor_RCA << endl;
		//LOG(LM_INFO) << "Target Value=" << test_values[i] << endl;
		//LOG(LM_INFO) << "Monitored Value=" << FinalMonitorValue << endl << endl;


		bufReturn <<  info << " \nSetValue= " << test_values[i] << ", OriginalMonitorValue="
				<< OriginalMonitorValue << ", FinalMonitoredValue="
				<< FinalMonitorValue << ",Range=[" << validmin << "," << validmax << "], Tolerance="
				<< tolerance << ".\n";
		strReturn = bufReturn.str();


		if (!InValidRange){
			//if SetValue is illegal
			//assert unchanged within tolerance
			CPPUNIT_ASSERT_MESSAGE(strReturn + ": Illegal value not ignored as expected.", fabs(OriginalMonitorValue - FinalMonitorValue) <= tolerance);
		}
		else{
			//if SetValue is legal
			//assert that monitored value is close enough to the target value
			CPPUNIT_ASSERT_MESSAGE(strReturn + ": Monitored value not close enough to set value.", fabs(test_values[i] - FinalMonitorValue) <= tolerance);
		}
	}//end for loop

	//Set to 0 after test has been performed
	packU16(0);
	info = "";
	commandImpl(control_RCA, "Setting to zero after performing test.\n", &info);
	SLEEP(100);
}




