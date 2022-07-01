#ifndef WCAIMPLBASE_H_
#define WCAIMPLBASE_H_
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

#include "FEHardwareDevice.h"

class WCAImplBase : public FEHardwareDevice {
public:
    WCAImplBase(const std::string &name, int port);
    virtual ~WCAImplBase()
      { shutdown(); }
      
    int getPort() const
      { return port_m; }
    
    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual unsigned short ytoCoarseTune();
    virtual bool photomixerEnable();
    virtual float photomixerVoltage();
    virtual float photomixerCurrent();
    virtual float pllLockDetectVoltage();
    virtual float pllCorrectionVoltage();
    virtual float pllAssemblyTemp();
    virtual float pllYTOHeaterCurrent();
    virtual float pllRefTotalPower();
    virtual float pllIfTotalPower();
    virtual bool pllUnlockDetectLatch();
    virtual bool pllLoopBandwidthSelect();
    virtual bool pllSidebandLockSelect();
    virtual bool pllNullLoopIntegrator();
    virtual float amcGateAVoltage();
    virtual float amcDrainAVoltage();
    virtual float amcDrainACurrent();
    virtual float amcGateBVoltage();
    virtual float amcDrainBVoltage();
    virtual float amcDrainBCurrent();
    virtual unsigned char amcMultiplierDCounts();
    virtual float amcGateEVoltage();
    virtual float amcDrainEVoltage();
    virtual float amcDrainECurrent();
    virtual float amcMultiplierDCurrent();
    virtual float amcSupplyVoltage5V();
    virtual float paPol0GateVoltage();
    virtual float paPol0DrainVoltage();
    virtual float paPol0DrainCurrent();
    virtual float paPol1GateVoltage();
    virtual float paPol1DrainVoltage();
    virtual float paPol1DrainCurrent();
    virtual float paSupplyVoltage3V();
    virtual float paSupplyVoltage5V();
    virtual bool paHasTeledyneChip();
    virtual unsigned char paPol0TeledyneCollector();
    virtual unsigned char paPol1TeledyneCollector();
    
// control points:
    virtual void ytoCoarseTune(unsigned short val);
    virtual void photomixerEnable(bool val);
    virtual void pllClearUnlockDetectLatch();
    virtual void pllLoopBandwidthSelect(bool val);
    virtual void pllSidebandLockSelect(bool val);
    virtual void pllNullLoopIntegrator(bool val);
    virtual void amcDrainBVoltage(float val);
    virtual void amcMultiplierDCounts(unsigned char val);
    virtual void amcGateEVoltage(float val);
    virtual void amcDrainEVoltage(float val);
    virtual void paPol0GateVoltage(float val);
    virtual void paPol0DrainVoltage(float val);
    virtual void paPol1GateVoltage(float val);
    virtual void paPol1DrainVoltage(float val);
    virtual void paHasTeledyneChip(bool val);
    virtual void paPol0TeledyneCollector(unsigned char val);
    virtual void paPol1TeledyneCollector(unsigned char val);

protected:
    virtual void monitorAction(Time *timestamp_p);

    enum MonitorControlOffset {
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
        PA_HAS_TELEDYNE_CHIP            = 0x0850,
        PA_TELEDYNE_COLLECTOR           = 0x0851
    };
    
    unsigned short ytoCoarseTune_value;
    bool photomixerEnable_value;
    float photomixerVoltage_value;
    float photomixerCurrent_value;
    float pllLockDetectVoltage_value;
    float pllCorrectionVoltage_value;
    float pllAssemblyTemp_value;
    float pllYTOHeaterCurrent_value;
    float pllRefTotalPower_value;
    float pllIfTotalPower_value;
    bool pllUnlockDetectLatch_value;
    bool pllLoopBandwidthSelect_value;
    bool pllSidebandLockSelect_value;
    bool pllNullLoopIntegrator_value;
    float amcGateAVoltage_value;
    float amcDrainAVoltage_value;
    float amcDrainACurrent_value;
    float amcGateBVoltage_value;
    float amcDrainBVoltage_value;
    float amcDrainBCurrent_value;
    unsigned char amcMultiplierDCounts_value;
    float amcGateEVoltage_value;
    float amcDrainEVoltage_value;
    float amcDrainECurrent_value;
    float amcMultiplierDCurrent_value;
    float amcSupplyVoltage5V_value;
    float paPol0GateVoltage_value;
    float paPol0DrainVoltage_value;
    float paPol0DrainCurrent_value;
    float paPol1GateVoltage_value;
    float paPol1DrainVoltage_value;
    float paPol1DrainCurrent_value;
    float paSupplyVoltage3V_value;
    float paSupplyVoltage5V_value;

    int ytoCoarseTune_status;
    int photomixerEnable_status;
    int photomixerVoltage_status;
    int photomixerCurrent_status;
    int pllLockDetectVoltage_status;
    int pllCorrectionVoltage_status;
    int pllAssemblyTemp_status;
    int pllYTOHeaterCurrent_status;
    int pllRefTotalPower_status;
    int pllIfTotalPower_status;
    int pllUnlockDetectLatch_status;
    int pllClearUnlockDetectLatch_status;
    int pllLoopBandwidthSelect_status;
    int pllSidebandLockSelect_status;
    int pllNullLoopIntegrator_status;
    int amcGateAVoltage_status;
    int amcDrainAVoltage_status;
    int amcDrainACurrent_status;
    int amcGateBVoltage_status;
    int amcDrainBVoltage_status;
    int amcDrainBCurrent_status;
    int amcMultiplierDCounts_status;
    int amcGateEVoltage_status;
    int amcDrainEVoltage_status;
    int amcDrainECurrent_status;
    int amcMultiplierDCurrent_status;
    int amcSupplyVoltage5V_status;
    int paPol0GateVoltage_status;
    int paPol0DrainVoltage_status;
    int paPol0DrainCurrent_status;
    int paPol1GateVoltage_status;
    int paPol1DrainVoltage_status;
    int paPol1DrainCurrent_status;
    int paSupplyVoltage3V_status;
    int paSupplyVoltage5V_status;
    int paHasTeledyneChip_status;
    int paPol0TeledyneCollector_status;
    int paPol1TeledyneCollector_status;

    AmbRelativeAddr baseRCA;
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
    AmbRelativeAddr paPol0DrainVoltage_RCA;
    AmbRelativeAddr paPol0DrainCurrent_RCA;
    AmbRelativeAddr paPol1GateVoltage_RCA;
    AmbRelativeAddr paPol1DrainVoltage_RCA;
    AmbRelativeAddr paPol1DrainCurrent_RCA;
    AmbRelativeAddr paSupplyVoltage3V_RCA;
    AmbRelativeAddr paSupplyVoltage5V_RCA;
    AmbRelativeAddr paHasTeledyneChip_RCA;
    AmbRelativeAddr paPol0TeledyneCollector_RCA;
    AmbRelativeAddr paPol1TeledyneCollector_RCA;

protected:    
    int port_m; ///< to which FEMC port is this module connected. 

private:
    // forbid copy constructor:
    WCAImplBase(const WCAImplBase &other);

    Time lastMonitorTime;
    int monitorPhase;
    
    DECLARE_MONITORS_REGISTRY(WCAImplBase)
    void logMon(bool printHeader = false) const;
};      

#endif /*WCAIMPLBASE_H_*/
