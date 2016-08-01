/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
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
*/

/************************************************************************
 * base class for the Warm Cartridge Assembly, which contains the 
 * first LO circuitry.  This will be replaced by auto-generated code
 * in the Control environment.
 *----------------------------------------------------------------------
 */

#include "WCAImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

WCAImplBase::WCAImplBase(const std::string &name, int port)
  : FEHardwareDevice(name),
    ytoCoarseTune_value(0),
    photomixerEnable_value(false),
    photomixerVoltage_value(0),
    photomixerCurrent_value(0),
    pllLockDetectVoltage_value(0),
    pllCorrectionVoltage_value(0),
    pllAssemblyTemp_value(0),
    pllYTOHeaterCurrent_value(0),
    pllRefTotalPower_value(0),
    pllIfTotalPower_value(0),
    pllUnlockDetectLatch_value(false),
    pllLoopBandwidthSelect_value(false),
    pllSidebandLockSelect_value(false),
    pllNullLoopIntegrator_value(false),
    amcGateAVoltage_value(0),
    amcDrainAVoltage_value(0),
    amcDrainACurrent_value(0),
    amcGateBVoltage_value(0),
    amcDrainBVoltage_value(0),
    amcDrainBCurrent_value(0),
    amcMultiplierDVoltage_value(0),
    amcGateEVoltage_value(0),
    amcDrainEVoltage_value(0),
    amcDrainECurrent_value(0),
    amcMultiplierDCurrent_value(0),
    amcSupplyVoltage5V_value(0),
    paPol0GateVoltage_value(0),
    paPol0DrainVoltage_value(0),
    paPol0DrainCurrent_value(0),
    paPol1GateVoltage_value(0),
    paPol1DrainVoltage_value(0),
    paPol1DrainCurrent_value(0),
    paSupplyVoltage3V_value(0),
    paSupplyVoltage5V_value(0),
    ytoCoarseTune_status(FEMC_NOT_CONNECTED),
    photomixerEnable_status(FEMC_NOT_CONNECTED),
    photomixerVoltage_status(FEMC_NOT_CONNECTED),
    photomixerCurrent_status(FEMC_NOT_CONNECTED),
    pllLockDetectVoltage_status(FEMC_NOT_CONNECTED),
    pllCorrectionVoltage_status(FEMC_NOT_CONNECTED),
    pllAssemblyTemp_status(FEMC_NOT_CONNECTED),
    pllYTOHeaterCurrent_status(FEMC_NOT_CONNECTED),
    pllRefTotalPower_status(FEMC_NOT_CONNECTED),
    pllIfTotalPower_status(FEMC_NOT_CONNECTED),
    pllUnlockDetectLatch_status(FEMC_NOT_CONNECTED),
    pllClearUnlockDetectLatch_status(FEMC_NOT_CONNECTED),
    pllLoopBandwidthSelect_status(FEMC_NOT_CONNECTED),
    pllSidebandLockSelect_status(FEMC_NOT_CONNECTED),
    pllNullLoopIntegrator_status(FEMC_NOT_CONNECTED),
    amcGateAVoltage_status(FEMC_NOT_CONNECTED),
    amcDrainAVoltage_status(FEMC_NOT_CONNECTED),
    amcDrainACurrent_status(FEMC_NOT_CONNECTED),
    amcGateBVoltage_status(FEMC_NOT_CONNECTED),
    amcDrainBVoltage_status(FEMC_NOT_CONNECTED),
    amcDrainBCurrent_status(FEMC_NOT_CONNECTED),
    amcMultiplierDVoltage_status(FEMC_NOT_CONNECTED),
    amcGateEVoltage_status(FEMC_NOT_CONNECTED),
    amcDrainEVoltage_status(FEMC_NOT_CONNECTED),
    amcDrainECurrent_status(FEMC_NOT_CONNECTED),
    amcMultiplierDCurrent_status(FEMC_NOT_CONNECTED),
    amcSupplyVoltage5V_status(FEMC_NOT_CONNECTED),
    paPol0GateVoltage_status(FEMC_NOT_CONNECTED),
    paPol0DrainVoltage_status(FEMC_NOT_CONNECTED),
    paPol0DrainCurrent_status(FEMC_NOT_CONNECTED),
    paPol1GateVoltage_status(FEMC_NOT_CONNECTED),
    paPol1DrainVoltage_status(FEMC_NOT_CONNECTED),
    paPol1DrainCurrent_status(FEMC_NOT_CONNECTED),
    paSupplyVoltage3V_status(FEMC_NOT_CONNECTED),
    paSupplyVoltage5V_status(FEMC_NOT_CONNECTED),
    baseRCA(0),
    port_m(port),
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = port - 1;
    baseRCA <<= 12;
    
    AmbRelativeAddr pol1 = 0x0004;
        
    ytoCoarseTune_RCA                   = baseRCA + YTO_COARSE_TUNE;
    photomixerEnable_RCA                = baseRCA + PHOTOMIXER_ENABLE;
    photomixerVoltage_RCA               = baseRCA + PHOTOMIXER_VOLTAGE;
    photomixerCurrent_RCA               = baseRCA + PHOTOMIXER_CURRENT;
    pllLockDetectVoltage_RCA            = baseRCA + PLL_LOCK_DETECT_VOLTAGE;
    pllCorrectionVoltage_RCA            = baseRCA + PLL_CORRECTION_VOLTAGE;
    pllAssemblyTemp_RCA                 = baseRCA + PLL_ASSEMBLY_TEMP;
    pllYTOHeaterCurrent_RCA             = baseRCA + PLL_YTO_HEATER_CURRENT;
    pllRefTotalPower_RCA                = baseRCA + PLL_REF_TOTAL_POWER;
    pllIfTotalPower_RCA                 = baseRCA + PLL_IF_TOTAL_POWER;
    pllUnlockDetectLatch_RCA            = baseRCA + PLL_UNLOCK_DETECT_LATCH;
    pllClearUnlockDetectLatch_RCA       = baseRCA + PLL_CLEAR_UNLOCK_DETECT_LATCH;
    pllLoopBandwidthSelect_RCA          = baseRCA + PLL_LOOP_BANDWIDTH_SELECT;
    pllSidebandLockSelect_RCA           = baseRCA + PLL_SIDEBAND_LOCK_POL_SELECT;
    pllNullLoopIntegrator_RCA           = baseRCA + PLL_NULL_LOOP_INTEGRATOR;
    amcGateAVoltage_RCA                 = baseRCA + AMC_GATE_A_VOLTAGE;
    amcDrainAVoltage_RCA                = baseRCA + AMC_DRAIN_A_VOLTAGE;
    amcDrainACurrent_RCA                = baseRCA + AMC_DRAIN_A_CURRENT;
    amcGateBVoltage_RCA                 = baseRCA + AMC_GATE_B_VOLTAGE;
    amcDrainBVoltage_RCA                = baseRCA + AMC_DRAIN_B_VOLTAGE;
    amcDrainBCurrent_RCA                = baseRCA + AMC_DRAIN_B_CURRENT;
    amcMultiplierDVoltage_RCA           = baseRCA + AMC_MULTIPLIER_D_VOLTAGE;
    amcGateEVoltage_RCA                 = baseRCA + AMC_GATE_E_VOLTAGE;
    amcDrainEVoltage_RCA                = baseRCA + AMC_DRAIN_E_VOLTAGE;
    amcDrainECurrent_RCA                = baseRCA + AMC_DRAIN_E_CURRENT;
    amcMultiplierDCurrent_RCA           = baseRCA + AMC_MULTIPLIER_D_CURRENT;
    amcSupplyVoltage5V_RCA              = baseRCA + AMC_SUPPLY_VOLTAGE_5V;
    paPol0GateVoltage_RCA               = baseRCA + PA_GATE_VOLTAGE;
    paPol0DrainVoltage_RCA              = baseRCA + PA_DRAIN_VOLTAGE;
    paPol0DrainCurrent_RCA              = baseRCA + PA_DRAIN_CURRENT;
    paPol1GateVoltage_RCA               = baseRCA + pol1 + PA_GATE_VOLTAGE;
    paPol1DrainVoltage_RCA              = baseRCA + pol1 + PA_DRAIN_VOLTAGE;
    paPol1DrainCurrent_RCA              = baseRCA + pol1 + PA_DRAIN_CURRENT;
    paSupplyVoltage3V_RCA               = baseRCA + PA_SUPPLY_VOLTAGE_3V;
    paSupplyVoltage5V_RCA               = baseRCA + PA_SUPPLY_VOLTAGE_5V;
    
    // Add all the analog monitor points to the registry:
    
    addMon(&photomixerVoltage_value, &WCAImplBase::photomixerVoltage);
    addMon(&photomixerCurrent_value, &WCAImplBase::photomixerCurrent);
    addMon(&pllLockDetectVoltage_value, &WCAImplBase::pllLockDetectVoltage);
    addMon(&pllCorrectionVoltage_value, &WCAImplBase::pllCorrectionVoltage);
    addMon(&pllRefTotalPower_value, &WCAImplBase::pllRefTotalPower);
    addMon(&pllIfTotalPower_value, &WCAImplBase::pllIfTotalPower);
    addMon(&amcGateAVoltage_value, &WCAImplBase::amcGateAVoltage);
    addMon(&amcDrainAVoltage_value, &WCAImplBase::amcDrainAVoltage);
    addMon(&amcDrainACurrent_value, &WCAImplBase::amcDrainACurrent);
    addMon(&amcGateBVoltage_value, &WCAImplBase::amcGateBVoltage);
    addMon(&amcDrainBVoltage_value, &WCAImplBase::amcDrainBVoltage);
    addMon(&amcDrainBCurrent_value, &WCAImplBase::amcDrainBCurrent);
    addMon(&amcGateEVoltage_value, &WCAImplBase::amcGateEVoltage);
    addMon(&amcDrainEVoltage_value, &WCAImplBase::amcDrainEVoltage);
    addMon(&amcDrainECurrent_value, &WCAImplBase::amcDrainECurrent);
    addMon(&paPol0GateVoltage_value, &WCAImplBase::paPol0GateVoltage);
    addMon(&paPol0DrainVoltage_value, &WCAImplBase::paPol0DrainVoltage);
    addMon(&paPol0DrainCurrent_value, &WCAImplBase::paPol0DrainCurrent);
    addMon(&paPol1GateVoltage_value, &WCAImplBase::paPol1GateVoltage);
    addMon(&paPol1DrainVoltage_value, &WCAImplBase::paPol1DrainVoltage);
    addMon(&paPol1DrainCurrent_value, &WCAImplBase::paPol1DrainCurrent);
    addMon(&amcMultiplierDCurrent_value, &WCAImplBase::amcMultiplierDCurrent);
    addMon(&amcSupplyVoltage5V_value, &WCAImplBase::amcSupplyVoltage5V);
    addMon(&paSupplyVoltage3V_value, &WCAImplBase::paSupplyVoltage3V);
    addMon(&paSupplyVoltage5V_value, &WCAImplBase::paSupplyVoltage5V);
    addMon(&pllAssemblyTemp_value, &WCAImplBase::pllAssemblyTemp);
    addMon(&pllYTOHeaterCurrent_value, &WCAImplBase::pllYTOHeaterCurrent);

    nextMon = monitorRegistry.begin();
}

void WCAImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void WCAImplBase::shutdown() {
}

// monitor properties:
unsigned short WCAImplBase::ytoCoarseTune() {
	SYNCMON_LOG_INT(ytoCoarseTune, unsigned short, "LO_YTO_COURSE_TUNE")
}

bool WCAImplBase::photomixerEnable() {
	SYNCMON_LOG_BOOL(photomixerEnable, "LO_PHOTOMIXER_ENABLE")
}

float WCAImplBase::photomixerVoltage() {
	SYNCMON_LOG_FLOAT(photomixerVoltage, "LO_PHOTOMIXER_VOLTAGE")
}

float WCAImplBase::photomixerCurrent() {
    SYNCMON_LOG_FLOAT(photomixerCurrent, "LO_PHOTOMIXER_CURRENT")
}

float WCAImplBase::pllLockDetectVoltage() {
    SYNCMON_LOG_FLOAT(pllLockDetectVoltage, "LO_PLL_LOCK_DETECT_VOLTAGE")
}

float WCAImplBase::pllCorrectionVoltage() {
    SYNCMON_LOG_FLOAT(pllCorrectionVoltage, "LO_PLL_CORRECTION_VOLTAGE")
}

float WCAImplBase::pllAssemblyTemp() {
    SYNCMON_LOG_FLOAT(pllAssemblyTemp, "LO_PLL_ASSEMBLY_TEMP")
}

float WCAImplBase::pllYTOHeaterCurrent() {
    SYNCMON_LOG_FLOAT(pllYTOHeaterCurrent, "LO_PLL_YTO_HEATER_CURRENT")
}

float WCAImplBase::pllRefTotalPower() {
    SYNCMON_LOG_FLOAT(pllRefTotalPower, "LO_PLL_REF_TOTAL_POWER")
}

float WCAImplBase::pllIfTotalPower() {
    SYNCMON_LOG_FLOAT(pllIfTotalPower, "LO_PLL_IF_TOTAL_POWER")
}

bool WCAImplBase::pllUnlockDetectLatch() {
    SYNCMON_LOG_BOOL(pllUnlockDetectLatch, "LO_PLL_UNLOCK_DETECT_LATCH")
}

bool WCAImplBase::pllLoopBandwidthSelect() {
    SYNCMON_LOG_BOOL(pllLoopBandwidthSelect, "LO_PLL_LOOP_BANDWIDTH_SELECT")
}

bool WCAImplBase::pllSidebandLockSelect() {
    SYNCMON_LOG_BOOL(pllSidebandLockSelect, "LO_PLL_SIDEBAND_LOCK_POL_SELECT")
}

bool WCAImplBase::pllNullLoopIntegrator() {
    SYNCMON_LOG_BOOL(pllNullLoopIntegrator, "LO_PLL_NULL_LOOP_INTEGRATOR")
}
    
float WCAImplBase::amcGateAVoltage() {
    SYNCMON_LOG_FLOAT(amcGateAVoltage, "LO_AMC_GATE_A_VOLTAGE")
}

float WCAImplBase::amcDrainAVoltage() {
    SYNCMON_LOG_FLOAT(amcDrainAVoltage, "LO_AMC_DRAIN_A_VOLTAGE")
}

float WCAImplBase::amcDrainACurrent() {
    SYNCMON_LOG_FLOAT(amcDrainACurrent, "LO_AMC_DRAIN_A_CURRENT")
}

float WCAImplBase::amcGateBVoltage() {
    SYNCMON_LOG_FLOAT(amcGateBVoltage, "LO_AMC_GATE_B_VOLTAGE")
}

float WCAImplBase::amcDrainBVoltage() {
    SYNCMON_LOG_FLOAT(amcDrainBVoltage, "LO_AMC_DRAIN_B_VOLTAGE")
}

float WCAImplBase::amcDrainBCurrent() {
    SYNCMON_LOG_FLOAT(amcDrainBCurrent, "LO_AMC_DRAIN_B_CURRENT")
}

unsigned char WCAImplBase::amcMultiplierDVoltage() {
    SYNCMON_LOG_BYTE(amcMultiplierDVoltage, "LO_AMC_MULTIPLIER_D_VOLTAGE")
}

float WCAImplBase::amcGateEVoltage() {
    SYNCMON_LOG_FLOAT(amcGateEVoltage, "LO_AMC_GATE_E_VOLTAGE")
}

float WCAImplBase::amcDrainEVoltage() {
    SYNCMON_LOG_FLOAT(amcDrainEVoltage, "LO_AMC_DRAIN_E_VOLTAGE")
}

float WCAImplBase::amcDrainECurrent() {
    SYNCMON_LOG_FLOAT(amcDrainECurrent, "LO_AMC_DRAIN_E_CURRENT")
}

float WCAImplBase::amcMultiplierDCurrent() {
    SYNCMON_LOG_FLOAT(amcMultiplierDCurrent, "LO_AMC_MULTIPLIER_D_CURRENT")
}

float WCAImplBase::amcSupplyVoltage5V() {
    SYNCMON_LOG_FLOAT(amcSupplyVoltage5V, "LO_AMC_SUPPLY_VOLTAGE_5V")
}

float WCAImplBase::paPol0GateVoltage() {
    SYNCMON_LOG_FLOAT(paPol0GateVoltage, "LO_PA_GATE_VOLTAGE Po=0")
}

float WCAImplBase::paPol0DrainVoltage() {
    SYNCMON_LOG_FLOAT(paPol0DrainVoltage, "LO_PA_DRAIN_VOLTAGE Po=0")
}

float WCAImplBase::paPol0DrainCurrent() {
    SYNCMON_LOG_FLOAT(paPol0DrainCurrent, "LO_PA_DRAIN_CURRENT Po=0")
}

float WCAImplBase::paPol1GateVoltage() {
    SYNCMON_LOG_FLOAT(paPol1GateVoltage, "LO_PA_GATE_VOLTAGE Po=1")
}

float WCAImplBase::paPol1DrainVoltage() {
    SYNCMON_LOG_FLOAT(paPol1DrainVoltage, "LO_PA_DRAIN_VOLTAGE Po=1")
}

float WCAImplBase::paPol1DrainCurrent() {
    SYNCMON_LOG_FLOAT(paPol1DrainCurrent, "LO_PA_DRAIN_CURRENT Po=1")
}

float WCAImplBase::paSupplyVoltage3V() {
    SYNCMON_LOG_FLOAT(paSupplyVoltage3V, "LO_PA_SUPPLY_VOLTAGE_3V")
}

float WCAImplBase::paSupplyVoltage5V() {
    SYNCMON_LOG_FLOAT(paSupplyVoltage5V, "LO_PA_SUPPLY_VOLTAGE_5V")
}

// control points:
void WCAImplBase::ytoCoarseTune(unsigned short val) {
    SYNCCMD_LOG_INT(ytoCoarseTune, val, "LO_YTO_COURSE_TUNE");
}

void WCAImplBase::photomixerEnable(bool val) {
    SYNCCMD_LOG_BOOL(photomixerEnable, val, "LO_PHOTOMIXER_ENABLE");
}

void WCAImplBase::pllClearUnlockDetectLatch() {
    SYNCCMD_LOG_BOOL(pllClearUnlockDetectLatch, (unsigned char) 0, "LO_PLL_CLEAR_UNLOCK_DETECT_LATCH");
}

void WCAImplBase::pllLoopBandwidthSelect(bool val) {
    SYNCCMD_LOG_BOOL(pllLoopBandwidthSelect, val, "LO_PLL_LOOP_BANDWIDTH_SELECT");
}

void WCAImplBase::pllSidebandLockSelect(bool val) {
    SYNCCMD_LOG_BOOL(pllSidebandLockSelect, val, "LO_PLL_SIDEBAND_LOCK_POL_SELECT");
}

void WCAImplBase::pllNullLoopIntegrator(bool val) {
    SYNCCMD_LOG_BOOL(pllNullLoopIntegrator, val, "LO_PLL_NULL_LOOP_INTEGRATOR");
}

void WCAImplBase::amcDrainBVoltage(float val) {
    SYNCCMD_LOG_FLOAT(amcDrainBVoltage, val, "LO_AMC_DRAIN_B_VOLTAGE");
}

void WCAImplBase::amcMultiplierDVoltage(unsigned char val) {
    SYNCCMD_LOG_BYTE(amcMultiplierDVoltage, val, "LO_AMC_MULTIPLIER_D_VOLTAGE");
}

void WCAImplBase::amcGateEVoltage(float val) {
    SYNCCMD_LOG_FLOAT(amcGateEVoltage, val, "LO_AMC_GATE_E_VOLTAGE");
}

void WCAImplBase::amcDrainEVoltage(float val) {
    SYNCCMD_LOG_FLOAT(amcDrainEVoltage, val, "LO_AMC_DRAIN_E_VOLTAGE");
}

void WCAImplBase::paPol0GateVoltage(float val) {
    SYNCCMD_LOG_FLOAT(paPol0GateVoltage, val, "LO_PA_GATE_VOLTAGE Po=0");
}

void WCAImplBase::paPol0DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(paPol0DrainVoltage, val, "LO_PA_DRAIN_VOLTAGE Po=0");
}

void WCAImplBase::paPol1GateVoltage(float val) {
    SYNCCMD_LOG_FLOAT(paPol1GateVoltage, val, "LO_PA_GATE_VOLTAGE Po=1");
}

void WCAImplBase::paPol1DrainVoltage(float val) {
    SYNCCMD_LOG_FLOAT(paPol1DrainVoltage, val, "LO_PA_DRAIN_VOLTAGE Po=1");
}

void WCAImplBase::monitorAction(Time *timestamp_p) {
    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;   

    static const Time monitorInterval = 30 * milliseconds;
    static const Time minMonitorInterval = 5000 * milliseconds;

    bool doMonitor = false;

    if (lastMonitorTime == 0 && logMonitors_m)
        logMon(true);

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= ((minimalMonitoring_m) ? minMonitorInterval : monitorInterval))) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        if (minimalMonitoring_m) {
            pllUnlockDetectLatch_value = pllUnlockDetectLatch();
            pllLockDetectVoltage_value = pllLockDetectVoltage();
            pllCorrectionVoltage_value = pllCorrectionVoltage();
            photomixerCurrent_value = photomixerCurrent();
            pllRefTotalPower_value = pllRefTotalPower();
            pllIfTotalPower_value = pllIfTotalPower();
            pllAssemblyTemp_value = pllAssemblyTemp();
            pllNullLoopIntegrator_value = pllNullLoopIntegrator();
        } else {        
            switch (monitorPhase) {
                case 0:
                    if (!executeNextMon()) {
                        if (randomizeAnalogMonitors_m)
                            randomizeMon();
                        monitorPhase = 1;
                    }
                    break;
                case 1:
                    pllUnlockDetectLatch_value = pllUnlockDetectLatch();
                    ++monitorPhase;
                    break;
                case 2:
                    ytoCoarseTune_value = ytoCoarseTune();
                    ++monitorPhase;
                    break;
                case 3:
                    amcMultiplierDVoltage_value = amcMultiplierDVoltage();
                    ++monitorPhase;
                    break;
                case 4:
                    photomixerEnable_value = photomixerEnable();
                    ++monitorPhase;
                    break;
                case 5:
                    pllLoopBandwidthSelect_value = pllLoopBandwidthSelect();
                    ++monitorPhase;
                    break;
                case 6:
                    pllSidebandLockSelect_value = pllSidebandLockSelect();
                    ++monitorPhase;
                    break;
                case 7:
                    pllNullLoopIntegrator_value = pllNullLoopIntegrator();
                    ++monitorPhase;
                    //no break;
                default:
                    if (logMonitors_m)
                       logMon();
                    monitorPhase = 0;
                    break;
            }
        }
    }
}

DEFINE_MONITORS_REGISTRY(WCAImplBase)

void WCAImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:WCA(" << port_m << "): randomized,"
                         "ytoCoarseTune,photomixerEnable,pllLoopBandwidthSelect,pllSidebandLockSelect,"
                         "amcMultiplierDVoltage,pllNullLoopIntegrator,pllUnlockDetectLatch,"
                         "Vphotomixer,Iphotomixer,VpllLockDetect,VpllCorrection,"
                         "pllRefTotalPower,pllIfTotalPower,VamcGateA,VamcDrainA,"
                         "IamcDrainA,VamcGateB,VamcDrainB,IamcDrainB,"
                         "VamcGateE,VamcDrainE,IamcDrainE,VpaPol0Gate,"
                         "VpaPol0Drain,IpaPol0Drain,VpaPol1Gate,VpaPol1Drain,"
                         "IpaPol1Drain,IamcMultiplierD,amcSupply5V,paSupply3V,"
                         "paSupply5V,pllAssemblyTemp,IpllYTOHeater" << endl;
    } else {
        LOG(LM_INFO)  << "AllMonitors:WCA(" << port_m << "): " << randomizeAnalogMonitors_m << ","
                      << ytoCoarseTune_value << "," << (photomixerEnable_value ? 1 : 0) << "," << (pllLoopBandwidthSelect_value ? 1 : 0) << "," << (pllSidebandLockSelect_value ? 1 : 0) << ","
                      << (int) amcMultiplierDVoltage_value << "," << (pllNullLoopIntegrator_value ? 1 : 0) << "," << (pllUnlockDetectLatch_value ? 1 : 0) << ","
                      << photomixerVoltage_value << "," << photomixerCurrent_value << "," << pllLockDetectVoltage_value << "," << pllCorrectionVoltage_value << ","
                      << pllRefTotalPower_value << "," << pllIfTotalPower_value << "," << amcGateAVoltage_value << "," << amcDrainAVoltage_value << ","
                      << amcDrainACurrent_value << "," << amcGateBVoltage_value << "," << amcDrainBVoltage_value << "," << amcDrainBCurrent_value << ","
                      << amcGateEVoltage_value << "," << amcDrainEVoltage_value << "," << amcDrainECurrent_value << "," << paPol0GateVoltage_value << ","
                      << paPol0DrainVoltage_value << "," << paPol0DrainCurrent_value << "," << paPol1GateVoltage_value << "," << paPol1DrainVoltage_value << ","
                      << paPol1DrainCurrent_value << "," << amcMultiplierDCurrent_value << "," << amcSupplyVoltage5V_value << "," << paSupplyVoltage3V_value << ","
                      << paSupplyVoltage5V_value << "," << pllAssemblyTemp_value << "," << pllYTOHeaterCurrent_value << endl;
    }
}


