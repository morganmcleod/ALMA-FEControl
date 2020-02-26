#ifndef WCATESTFIXTURE_H_
#define WCATESTFIXTURE_H_
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
*/


#include "CartridgeTestFixture.h"
#include "AmbDeviceTestFixture.h"
#include <cppunit/extensions/HelperMacros.h>
#include <vector>
#include <string>

#define INSTANTIATE_WCA_SUITE(CLASS) \
    CPPUNIT_TEST_SUITE(CLASS); \
    CPPUNIT_TEST(ENABLE_CARTRIDGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PHOTOMIXER_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PHOTOMIXER_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PLL_LOCK_DETECT_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PLL_CORRECTION_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PLL_ASSEMBLY_TEMP); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_YIG_HEATER_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PLL_REF_TOTAL_POWER); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PLL_IF_TOTAL_POWER); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_GATE_A_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_DRAIN_A_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_DRAIN_A_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_GATE_B_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_DRAIN_B_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_DRAIN_E_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PA_POL0_DRAIN_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PA_POL1_DRAIN_CURRENT); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_AMC_SUPPLY_VOLTAGE_5V); \
    CPPUNIT_TEST(testGET_CARTRIDGE_LO_PA_SUPPLY_VOLTAGE_5V); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_YTO_COARSE_TUNE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PHOTOMIXER_ENABLE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PLL_CLEAR_UNLOCK_DETECT_LATCH); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PLL_LOOP_BANDWIDTH_SELECT); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_NULL_LOOP_INTEGRATOR); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PA_POL0_GATE_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PA_POL1_GATE_VOLTAGE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PA_POL0_DRAIN_VOLTAGE_SCALE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PA_POL1_DRAIN_VOLTAGE_SCALE); \
    CPPUNIT_TEST(testSET_CARTRIDGE_LO_PA_LIMITS_TABLE); \
    CPPUNIT_TEST(DISABLE_CARTRIDGE); \
    CPPUNIT_TEST_SUITE_END();

class WCATestFixture : public CartridgeTestFixture {
public:

    WCATestFixture(int band,
                   const std::string &name = std::string("WCATestFixture"),
                   AmbChannel channel = 0,
                   AmbNodeAddr nodeAddress = 0x13,
                   bool expectStatusByte = true);

    void setUp();
    void tearDown();

	//Monitor points
	void testGET_CARTRIDGE_LO_PHOTOMIXER_VOLTAGE();
	void testGET_CARTRIDGE_LO_PHOTOMIXER_CURRENT();
	void testGET_CARTRIDGE_LO_PLL_LOCK_DETECT_VOLTAGE();
	void testGET_CARTRIDGE_LO_PLL_CORRECTION_VOLTAGE();
	void testGET_CARTRIDGE_LO_PLL_ASSEMBLY_TEMP();
	void testGET_CARTRIDGE_LO_YIG_HEATER_CURRENT();
	void testGET_CARTRIDGE_LO_PLL_REF_TOTAL_POWER();
	void testGET_CARTRIDGE_LO_PLL_IF_TOTAL_POWER();
	void testGET_CARTRIDGE_LO_AMC_GATE_A_VOLTAGE();
	void testGET_CARTRIDGE_LO_AMC_DRAIN_A_VOLTAGE();
	void testGET_CARTRIDGE_LO_AMC_DRAIN_A_CURRENT();
	void testGET_CARTRIDGE_LO_AMC_GATE_B_VOLTAGE();
	void testGET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE();
	void testGET_CARTRIDGE_LO_AMC_DRAIN_B_CURRENT();
	void testGET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE();
	void testGET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE();
	void testGET_CARTRIDGE_LO_AMC_DRAIN_E_CURRENT();
    void testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS();
	void testGET_CARTRIDGE_LO_AMC_MULTIPLIER_D_CURRENT();



	void testGET_CARTRIDGE_LO_PA_POL0_DRAIN_CURRENT();
	void testGET_CARTRIDGE_LO_PA_POL1_DRAIN_CURRENT();
	void testGET_CARTRIDGE_LO_AMC_SUPPLY_VOLTAGE_5V();
	void testGET_CARTRIDGE_LO_PA_SUPPLY_VOLTAGE_5V();

	//Control points
	void testSET_CARTRIDGE_LO_YTO_COARSE_TUNE();
	void testSET_CARTRIDGE_LO_PHOTOMIXER_ENABLE();
	void testSET_CARTRIDGE_LO_PLL_CLEAR_UNLOCK_DETECT_LATCH();
	void testSET_CARTRIDGE_LO_PLL_LOOP_BANDWIDTH_SELECT();
	void testSET_CARTRIDGE_LO_PLL_SB_LOCK_POLARITY_SELECT();
	void testSET_CARTRIDGE_LO_NULL_LOOP_INTEGRATOR();
	void testSET_CARTRIDGE_LO_AMC_DRAIN_B_VOLTAGE();
	void testSET_CARTRIDGE_LO_AMC_MULTIPLIER_D_COUNTS();
	void testSET_CARTRIDGE_LO_AMC_GATE_E_VOLTAGE();
	void testSET_CARTRIDGE_LO_AMC_DRAIN_E_VOLTAGE();
	void testSET_CARTRIDGE_LO_PA_POL0_GATE_VOLTAGE();
	void testSET_CARTRIDGE_LO_PA_POL1_GATE_VOLTAGE();
	void testSET_CARTRIDGE_LO_PA_POL0_DRAIN_VOLTAGE_SCALE();
	void testSET_CARTRIDGE_LO_PA_POL1_DRAIN_VOLTAGE_SCALE();

	void testSET_CARTRIDGE_LO_PA_LIMITS_TABLE();

protected:
    void implSET_WCA_VALUE_U16(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
    		std::vector<unsigned short> &test_values, const unsigned short validmin, const unsigned short validmax,
            const unsigned short tolerance, const std::string &msg);
    void implSET_WCA_VALUE_FLOAT(AmbRelativeAddr monitor_RCA,AmbRelativeAddr control_RCA,
    		std::vector<float> &test_values, const float validmin, const float validmax,
            const float tolerance, const std::string &msg);


private:
	enum MonitorControlOffset {
		   controlRCA                      = 0x10000,   ///< add this to the monitor RCA to get the corresponding command RCA
           YTO_COARSE_TUNE                 = 0x0800,
	       PHOTOMIXER_ENABLE               = 0x0810,
	       PHOTOMIXER_VOLTAGE              = 0x0814,
	       PHOTOMIXER_CURRENT              = 0x0818,
	       PLL_LOCK_DETECT_VOLTAGE         = 0x0820,
	       PLL_CORRECTION_VOLTAGE          = 0x0821,
	       PLL_ASSEMBLY_TEMP               = 0x0822,
	       PLL_YTO_HEATER_CURRENT          = 0x0823,
	       PLL_REF_TOTAL_POWER             = 0x0824,
	       PLL_IF_TOTAL_POWER              = 0x0825,
	       PLL_UNLOCK_DETECT_LATCH         = 0x0827,
	       PLL_CLEAR_UNLOCK_DETECT_LATCH   = 0x0828,
	       PLL_LOOP_BANDWIDTH_SELECT       = 0x0829,
	       PLL_SIDEBAND_LOCK_POL_SELECT    = 0x082A,
	       PLL_NULL_LOOP_INTEGRATOR        = 0x082B,
	       AMC_GATE_A_VOLTAGE              = 0x0830,
	       AMC_DRAIN_A_VOLTAGE             = 0x0831,
	       AMC_DRAIN_A_CURRENT             = 0x0832,
	       AMC_GATE_B_VOLTAGE              = 0x0833,
	       AMC_DRAIN_B_VOLTAGE             = 0x0834,
	       AMC_DRAIN_B_CURRENT             = 0x0835,
	       AMC_MULTIPLIER_D_COUNTS         = 0x0836,
	       AMC_GATE_E_VOLTAGE              = 0x0837,
	       AMC_DRAIN_E_VOLTAGE             = 0x0838,
	       AMC_DRAIN_E_CURRENT             = 0x0839,
           AMC_MULTIPLIER_D_CURRENT        = 0x083A,
	       AMC_SUPPLY_VOLTAGE_5V           = 0x083B,
	       PA_GATE_VOLTAGE                 = 0x0840,
	       PA_DRAIN_VOLTAGE                = 0x0841,
	       PA_DRAIN_CURRENT                = 0x0842,
	       PA_SUPPLY_VOLTAGE_3V            = 0x0848,
	       PA_SUPPLY_VOLTAGE_5V            = 0x084C,
	       CLEAR_PA_LIMITS                 = 0x21020,
	       PA_LIMITS_ENTRY                 = 0x21030
	};

	AmbRelativeAddr ytoCoarseTune_RCA;
	AmbRelativeAddr photomixerEnable_RCA;
    AmbRelativeAddr photomixerVoltage_RCA;
    AmbRelativeAddr photomixerCurrent_RCA;
    AmbRelativeAddr pllLockDetectVoltage_RCA;
    AmbRelativeAddr pllCorrectionVoltage_RCA;
    AmbRelativeAddr pllAssemblyTemp_RCA;
    AmbRelativeAddr pllYTOHeaterCurrent_RCA;
    AmbRelativeAddr pllRefTotalPower_RCA;
    AmbRelativeAddr pllIfTotalPower_RCA;
    AmbRelativeAddr pllUnlockDetectLatch_RCA;
    AmbRelativeAddr pllClearUnlockDetectLatch_RCA;
    AmbRelativeAddr pllLoopBandwidthSelect_RCA;
    AmbRelativeAddr pllSidebandLockSelect_RCA;
    AmbRelativeAddr pllNullLoopIntegrator_RCA;
    AmbRelativeAddr amcGateAVoltage_RCA;
    AmbRelativeAddr amcDrainAVoltage_RCA;
    AmbRelativeAddr amcDrainACurrent_RCA;
    AmbRelativeAddr amcGateBVoltage_RCA;
    AmbRelativeAddr amcDrainBVoltage_RCA;
    AmbRelativeAddr amcDrainBCurrent_RCA;
    AmbRelativeAddr amcMultiplierDCounts_RCA;
    AmbRelativeAddr amcGateEVoltage_RCA;
    AmbRelativeAddr amcDrainEVoltage_RCA;
    AmbRelativeAddr amcDrainECurrent_RCA;
    AmbRelativeAddr amcMultiplierDCurrent_RCA;
    AmbRelativeAddr amcSupplyVoltage5V_RCA;
    AmbRelativeAddr paPol0GateVoltage_RCA;
    AmbRelativeAddr paPol0DrainVoltageScale_RCA;
    AmbRelativeAddr paPol0DrainCurrent_RCA;
    AmbRelativeAddr paPol1GateVoltage_RCA;
    AmbRelativeAddr paPol1DrainVoltageScale_RCA;
    AmbRelativeAddr paPol1DrainCurrent_RCA;
    AmbRelativeAddr paSupplyVoltage3V_RCA;
    AmbRelativeAddr paSupplyVoltage5V_RCA;

    //Control RCAs
    AmbRelativeAddr ctrlytoCoarseTune_RCA;
    AmbRelativeAddr ctrlphotomixerEnable_RCA;
    AmbRelativeAddr ctrlphotomixerVoltage_RCA;
    AmbRelativeAddr ctrlpllLoopBandwidthSelect_RCA;
    AmbRelativeAddr ctrlpllClearUnlockDetectLatch_RCA;
    AmbRelativeAddr ctrlpllSidebandLockSelect_RCA;
    AmbRelativeAddr ctrlpllNullLoopIntegrator_RCA;
    AmbRelativeAddr ctrlamcDrainBVoltage_RCA;
    AmbRelativeAddr ctrlamcGateEVoltage_RCA;
    AmbRelativeAddr ctrlamcDrainEVoltage_RCA;
    AmbRelativeAddr ctrlpaPol0GateVoltage_RCA;
    AmbRelativeAddr ctrlpaPol1GateVoltage_RCA;
    AmbRelativeAddr ctrlpaPol0DrainVoltageScale_RCA;
    AmbRelativeAddr ctrlpaPol1DrainVoltageScale_RCA;
    AmbRelativeAddr ctrlamcMultiplierDCounts_RCA;
    AmbRelativeAddr ctrlClearPALimits_RCA;
    AmbRelativeAddr ctrlSetPALimitsEntry_RCA;

    static std::vector<float> SetValuesLOpaGateVoltage;
    static const float ValidMinLOpaGateVoltage;
    static const float ValidMaxLOpaGateVoltage;
    static const float ToleranceLOpaGateVoltage;

    static std::vector<float> SetValuesLOpaDrainVoltageScale;
    static const float ValidMinLOpaDrainVoltageScale;
    static const float ValidMaxLOpaDrainVoltageScale;
    static const float ToleranceLOpaDrainVoltageScale;

	static const float ValidMinLOAMCDrainCurrent;
	static const float ValidMaxLOAMCDrainCurrent;

};

#endif /* WCATESTFIXTURE_H_ */
