#ifndef LORTMIMPLBASE_H_
#define LORTMIMPLBASE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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
 * base class for the LO Reference Test Module (LORTM)
 *----------------------------------------------------------------------
 */

#include "FEHardwareDevice.h"

class LORTMImplBase : public FEHardwareDevice {
public:
    LORTMImplBase(const std::string &name);
    virtual ~LORTMImplBase()
      { shutdown(); }

    virtual void initialize(unsigned long channel, unsigned long nodeAddress);
    virtual void shutdown();

// monitor properties:
    virtual std::string GetSerialNumber();
    virtual std::string GetProtocolRevision();
    virtual std::string GetFirmwareVersion();
    virtual unsigned long AMBSINumErrors();
    virtual unsigned long AMBSINumTransactions();
    virtual float AmbientTemperature();

    virtual unsigned short SystemGetStatus();
    virtual short SystemGetError();
    virtual short SystemGetWarning();
    
    virtual float LaserBiasMon1();
    virtual float LaserBiasMon2();
    virtual float LaserBiasMon3();
    virtual float LaserBiasMon4();
    virtual float TempIntegOutMon1();
    virtual float TempIntegOutMon2();
    virtual float TempIntegOutMon3();
    virtual float TempIntegOutMon4();
    virtual float LaserTempMon1();
    virtual float LaserTempMon2();
    virtual float LaserTempMon3();
    virtual float LaserTempMon4();
    virtual float LaserTecIMon1();
    virtual float LaserTecIMon2();
    virtual float LaserTecIMon3();
    virtual float LaserTecIMon4();
    virtual float LaserPowMon1();
    virtual float LaserPowMon2();
    virtual float LaserPowMon3();
    virtual float LaserPowMon4();
    virtual float PhMixBiasMon1();
    virtual float PhMixBiasMon2();
    virtual float PhMixBiasMon3();
    virtual float PhMixBiasMon4();
    virtual float LaserSlowCorrMon();
    virtual float RFAGCGainMon();
    virtual float RFPowMon34dB();
    virtual float ExternThermMon();
    virtual float InternThermMon();
    
    virtual unsigned char LaserGetStatus1();
    virtual unsigned char LaserGetStatus2();
    virtual unsigned char LaserGetStatus3();
    virtual unsigned char LaserGetStatus4();

    virtual bool LaserISrcEnable(int laser);
    virtual unsigned long LaserFrequency(int laser);
    
    /*
    virtual float LaserISrcBias(int laser);
    virtual float LaserTempSetpoint(int laser);
    virtual float LaserTempCtrlEnable(int laser);
    virtual float LaserOperatingCurrent(int laser);
    virtual float LaserPowerCalibCoeff(int laser);
    virtual float LaserCalibCurrent(int laser);
    */
    virtual float LaserCalibCoeffA(int laser, int coeff);
    virtual float LaserCalibCoeffB(int laser, int coeff);
    virtual float LaserCalibCoeffC(int laser, int coeff);
    
    virtual unsigned short PhaselockGetStatus();
    virtual unsigned char PhaselockGetSelectedLaser();
    virtual unsigned char PhaselockGetSelectedBand();
    
// control points:
    virtual void ResetAMBSI();
    virtual void ResetDevice();
    
    virtual void SystemManualModeRequest();
    virtual void SystemStandbyModeRequest();
    virtual void SystemLoadAllParameters();
    virtual void SystemSaveAllParameters();
    virtual void SystemClearErrors();
    
    virtual void PhaseLockTuningInit(unsigned long slaveFreqMHz);
    virtual void PhaseLockTuningFinalize();

    virtual void LaserISrcEnable(int laser, bool enable);
    virtual void LaserSetFrequency(int laser, unsigned long slaveFreqMHz);
    virtual void LaserCalibrationSelect(int laser);
    
protected:
    virtual void monitorAction(Time *timestamp_p);
    
    enum MonitorControlOffset {
        GET_SERIAL_NUMBER                   = 0x00000,
        GET_PROTOCOL_REV_LEVEL              = 0x30000,
        AMBSI_NUM_ERRORS                    = 0x30001,
        AMBSI_NUM_TRANSACIONS               = 0x30002,
        GET_AMBIENT_TEMPERATURE             = 0x30003,
        GET_SW_REV_LEVEL                    = 0x30004,
        RESET_AMBSI                         = 0x31000,
        RESET_DEVICE                        = 0x31001,
        
        SYSTEM_MANUAL_MODE_REQUEST          = 0x01000,
        SYSTEM_STANDBY_MODE_REQUEST         = 0x01001,
        SYSTEM_LOAD_ALL_PARAMS              = 0X01002,
        SYSTEM_SAVE_ALL_PARAMS              = 0x01003,
        SYSTEM_CLEAR_ERRORS                 = 0x01004,
        SYSTEM_GET_STATUS                   = 0x01100,
        SYSTEM_GET_ERROR                    = 0x01101,
        SYSTEM_GET_WARNING                  = 0x01102,
        SYSTEM_GET_INTERLOCK_STATUS         = 0x01120,
        SYSTEM_STARTUP_MODE                 = 0x01800,
        
        SIGNAL_LASER_BIAS_MON1              = 0x01500,
        SIGNAL_LASER_BIAS_MON2              = 0x01501,
        SIGNAL_LASER_BIAS_MON3              = 0x01502,
        SIGNAL_LASER_BIAS_MON4              = 0x01503,
        SIGNAL_TEMP_INTEG_OUT_MON1          = 0x01504,
        SIGNAL_TEMP_INTEG_OUT_MON2          = 0x01505,
        SIGNAL_TEMP_INTEG_OUT_MON3          = 0x01506,
        SIGNAL_TEMP_INTEG_OUT_MON4          = 0x01507,
        SIGNAL_LASER_TEMP_MON1              = 0x01508,
        SIGNAL_LASER_TEMP_MON2              = 0x01509,
        SIGNAL_LASER_TEMP_MON3              = 0x0150A,
        SIGNAL_LASER_TEMP_MON4              = 0x0150B,
        SIGNAL_LASER_TEC_I_MON1             = 0x0150C,
        SIGNAL_LASER_TEC_I_MON2             = 0x0150D,
        SIGNAL_LASER_TEC_I_MON3             = 0x0150E,
        SIGNAL_LASER_TEC_I_MON4             = 0x0150F,
        SIGNAL_LASER_POW_MON1               = 0x01510,
        SIGNAL_LASER_POW_MON2               = 0x01511,
        SIGNAL_LASER_POW_MON3               = 0x01512,
        SIGNAL_LASER_POW_MON4               = 0x01513,
        SIGNAL_PHMIX_BIAS_MON1              = 0x01514,
        SIGNAL_PHMIX_BIAS_MON2              = 0x01515,
        SIGNAL_PHMIX_BIAS_MON3              = 0x01516,
        SIGNAL_PHMIX_BIAS_MON4              = 0x01517,
        SIGNAL_LASER_SLOW_CORR_MON          = 0x01518,
        SIGNAL_RF_AGC_GAIN_MON              = 0x01519,
        SIGNAL_RF_POW_MON_34DB              = 0x0151A,
        SIGNAL_EXTERN_THERM_MON             = 0x0151B,
        SIGNAL_INTERN_THERM_MON             = 0x0151C,
        SIGNAL_GROUND                       = 0x0151F,
        
        LASER_GET_STATUS1                   = 0x0203C,
        LASER_GET_STATUS2                   = 0x0203D,
        LASER_GET_STATUS3                   = 0x0203E,
        LASER_GET_STATUS4                   = 0x0203F,
        
        LASER_ICRC_BIAS                     = 0x02000,        
        LASER_TEMP_SETPOINT                 = 0x02004,
        LASER_ISRC_ENABLE                   = 0x02008,
        LASER_TEMP_CTRL_ENABLE              = 0x0200C,        
        LASER_FREQUENCY                     = 0x02050,
        LASER_OPERATING_CURRENT             = 0x02070,
        LASER_POWER_CALIB_COEFF             = 0x02074,
        LASER_CALIBRATION_CURRENT           = 0x02100,        
        LASER_CALIBRATION_COEFF_A           = 0x02110,
        LASER_CALIBRATION_COEFF_B           = 0x02120,
        LASER_CALIBRATION_COEFF_C           = 0x02130,

        PHASELOCK_TUNING_INIT               = 0x03000,
        PHASELOCK_TUNING_FINALIZE           = 0x03001,
        PHASELOCK_GET_STATUS                = 0x03020,
        PHASELOCK_GET_SELECTED_LASER        = 0x03034,
        PHASELOCK_GET_SELECTED_BAND         = 0x03035,
        
        LL_OPTSW_CALIBRATION                = 0x06110,
        LL_OPTSW_BANDSEL                    = 0x06111,
        LL_OPTSW_SLAVESEL                   = 0x06112
    };

    std::string SerialNumber_value;
    std::string ProtocolRevision_value;
    std::string FirmwareVersion_value;
    unsigned long CANStatus_value;
    unsigned long AMBSINumErrors_value;
    unsigned long AMBSINumTransactions_value;
    
public:    
    float AmbientTemperature_value;
    
protected:    
    int AmbientTemperature_status;
    
    unsigned short SystemGetStatus_value;
    unsigned char systemState;
    bool systemWarning;
    bool systemError;
    bool operationPending;
    
    short SystemGetError_value;
    short SystemGetWarning_value;
    bool SystemGetInterlockStatus_value;
    
    float LaserBiasMon1_value;
    float LaserBiasMon2_value;
    float LaserBiasMon3_value;
    float LaserBiasMon4_value;
    float TempIntegOutMon1_value;
    float TempIntegOutMon2_value;
    float TempIntegOutMon3_value;
    float TempIntegOutMon4_value;
    float LaserTempMon1_value;
    float LaserTempMon2_value;
    float LaserTempMon3_value;
    float LaserTempMon4_value;
    float LaserTecIMon1_value;
    float LaserTecIMon2_value;
    float LaserTecIMon3_value;
    float LaserTecIMon4_value;
    float LaserPowMon1_value;
    float LaserPowMon2_value;
    float LaserPowMon3_value;
    float LaserPowMon4_value;
    float PhMixBiasMon1_value;
    float PhMixBiasMon2_value;
    float PhMixBiasMon3_value;
    float PhMixBiasMon4_value;
    float LaserSlowCorrMon_value;
    float RFAGCGainMon_value;
    float RFPowMon34dB_value;
    float ExternThermMon_value;
    float InternThermMon_value;
    
    unsigned char LaserGetStatus1_value;
    unsigned char LaserGetStatus2_value;
    unsigned char LaserGetStatus3_value;
    unsigned char LaserGetStatus4_value;
    
    unsigned short PhaselockGetStatus_value;
    bool phaselockLocked;
    bool phaselockTuningReady;
    bool phaselockRFInputReady;
    
    unsigned char PhaselockGetSelectedLaser_value;
    unsigned char PhaselockGetSelectedBand_value;

private:
    // forbid copy constructor:
    LORTMImplBase(const LORTMImplBase &other);
    
    Time lastMonitorTime;
    Time lastMonitorTime30s;
    int monitorPhase;
    
    static float convertLaserBiasMon(unsigned short adValue);
    static float convertTempIntegMon(unsigned short adValue);
    static float convertTemperatureMon(unsigned short adValue);
    static float convertTECIMon(unsigned short adValue);
    static float convertLaserPowerMon(unsigned short adValue);
    static float convertPhmixBiasMon(unsigned short adValue);
    static float convertSlowCorrMon(unsigned short adValue);
    static float convertRFAGCGainMon(unsigned short adValue);
    static float convertRFPowMon34dB(unsigned short adValue);    
};      

#endif /*LORTMIMPLBASE_H_*/
