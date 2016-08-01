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
 
#include "LORTMImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
#include "stringConvert.h"
#include <FrontEndAMB/ds1820.h>
using namespace std;
#include <math.h>

LORTMImplBase::LORTMImplBase(const std::string &name)
  : FEHardwareDevice(name),
    CANStatus_value(0),
    AMBSINumErrors_value(0),
    AMBSINumTransactions_value(0),  
    AmbientTemperature_value(0),
    AmbientTemperature_status(0),
    SystemGetStatus_value(0),
    systemState(0),
    systemWarning(false),
    systemError(false),
    operationPending(false),
    SystemGetError_value(0),
    SystemGetWarning_value(0),
    SystemGetInterlockStatus_value(false),  
    LaserBiasMon1_value(0),
    LaserBiasMon2_value(0),
    LaserBiasMon3_value(0),
    LaserBiasMon4_value(0),
    TempIntegOutMon1_value(0),
    TempIntegOutMon2_value(0),
    TempIntegOutMon3_value(0),
    TempIntegOutMon4_value(0),
    LaserTempMon1_value(0),
    LaserTempMon2_value(0),
    LaserTempMon3_value(0),
    LaserTempMon4_value(0),
    LaserTecIMon1_value(0),
    LaserTecIMon2_value(0),
    LaserTecIMon3_value(0),
    LaserTecIMon4_value(0),
    LaserPowMon1_value(0),
    LaserPowMon2_value(0),
    LaserPowMon3_value(0),
    LaserPowMon4_value(0),
    PhMixBiasMon1_value(0),
    PhMixBiasMon2_value(0),
    PhMixBiasMon3_value(0),
    PhMixBiasMon4_value(0),
    LaserSlowCorrMon_value(0),
    RFAGCGainMon_value(0),
    RFPowMon34dB_value(0),
    ExternThermMon_value(0),
    InternThermMon_value(0),
    LaserGetStatus1_value(0),
    LaserGetStatus2_value(0),
    LaserGetStatus3_value(0),
    LaserGetStatus4_value(0),  
    PhaselockGetStatus_value(0),
    PhaselockGetSelectedLaser_value(0),
    PhaselockGetSelectedBand_value(0),
    lastMonitorTime(0),
    lastMonitorTime30s(0),
    monitorPhase(0)
{
}

void LORTMImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void LORTMImplBase::shutdown() {
}

std::string LORTMImplBase::GetSerialNumber() {
    FEHardwareDevice::syncMonitorEightByteESN(GET_SERIAL_NUMBER, SerialNumber_value, false);
    return SerialNumber_value;    
}

std::string LORTMImplBase::GetProtocolRevision() {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock; 
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(GET_PROTOCOL_REV_LEVEL, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        ProtocolRevision_value = "";
    else {
        char buf[20];
        sprintf(buf, "%d.%d.%d", (int) data[0], (int) data[1], (int) data[2]);
        ProtocolRevision_value = buf;
    }
    return ProtocolRevision_value;
}

std::string LORTMImplBase::GetFirmwareVersion() {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock; 
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(GET_SW_REV_LEVEL, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        FirmwareVersion_value = "";
    else {
        char buf[20];
        sprintf(buf, "%d.%d.%d", (int) data[0], (int) data[1], (int) data[2]);
        FirmwareVersion_value = buf;
    }
    return FirmwareVersion_value;
}

unsigned long LORTMImplBase::AMBSINumErrors() {
    AMBSINumErrors_value = 0;
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    sem_init(&synchLock, 0, 0);
    monitor(AMBSI_NUM_ERRORS, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status == AMBERR_NOERR)
        feStatus = unpack(AMBSINumErrors_value, dataLength, data);
    else
        feStatus = FEMC_AMB_ERROR;
    LOG2_INT(FEMC_LOG_MONITOR, AMBSI_NUM_ERRORS, "LORTM:AMBSI_NUM_ERRORS", feStatus, AMBSINumErrors_value);
    return AMBSINumErrors_value;
}
    
unsigned long LORTMImplBase::AMBSINumTransactions() {
    AMBSINumTransactions_value = 0;
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    sem_init(&synchLock, 0, 0);
    monitor(AMBSI_NUM_TRANSACIONS, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status == AMBERR_NOERR)
        feStatus = unpack(AMBSINumTransactions_value, dataLength, data);
    else
        feStatus = FEMC_AMB_ERROR;
    LOG2_INT(FEMC_LOG_MONITOR, AMBSI_NUM_TRANSACIONS, "LORTM:AMBSI_NUM_TRANSACIONS", feStatus, AMBSINumTransactions_value);
    return AMBSINumTransactions_value;
}

float LORTMImplBase::AmbientTemperature() {
    int AmbientTemperature_status = 0; 
    AmbientTemperature_value = 0;
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    sem_init(&synchLock, 0, 0);
    monitor(GET_AMBIENT_TEMPERATURE, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);    
    if (status == AMBERR_NOERR) {
        // TODO: remove this workaround -- LORTM returns temperature bytes in reverse order.
        data[7] = data[0];
        data[0] = data[1];
        data[1] = data[7];        
        AmbientTemperature_status = unpackDS1820Temperature(AmbientTemperature_value, dataLength, data);
    } else
        AmbientTemperature_status = FEMC_UNPACK_ERROR;
    LOG2_FLOAT(FEMC_LOG_MONITOR, GET_AMBIENT_TEMPERATURE, "LORTM:GET_AMBIENT_TEMPERATURE", AmbientTemperature_status, AmbientTemperature_value);
    return AmbientTemperature_value;
}

unsigned short LORTMImplBase::SystemGetStatus() {
    SystemGetStatus_value = 0;
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    sem_init(&synchLock, 0, 0);
    monitor(SYSTEM_GET_STATUS, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status == AMBERR_NOERR) {
        feStatus = unpack(SystemGetStatus_value, dataLength, data);
        if (feStatus == 0) {
            systemState = (SystemGetStatus_value & 0x0038) >> 3;
            systemWarning = (SystemGetStatus_value & 0x0004) != 0;
            systemError = (SystemGetStatus_value & 0x0002) != 0;
            operationPending = (SystemGetStatus_value & 0x0001) != 0;
            SystemGetInterlockStatus_value = (SystemGetStatus_value & 0x1000) != 0;
        }
    } else
        feStatus = FEMC_UNPACK_ERROR;
    getLogger().log(FEMC_LOG_MONITOR, "LORTM:SYSTEM_GET_STATUS", SYSTEM_GET_STATUS, feStatus, SystemGetStatus_value, 0.0);
    return SystemGetStatus_value;
}

short LORTMImplBase::SystemGetError() {
    SYNCMON2_LOG_INT(SYSTEM_GET_ERROR, short, "LORTM:SYSTEM_GET_ERROR")
}

short LORTMImplBase::SystemGetWarning() {
    SYNCMON2_LOG_INT(SYSTEM_GET_WARNING, short, "LORTM:SYSTEM_GET_WARNING")
}

float LORTMImplBase::LaserBiasMon1() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_BIAS_MON1, "LORTM:SIGNAL_LASER_BIAS_MON1", convertLaserBiasMon)
}

float LORTMImplBase::LaserBiasMon2() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_BIAS_MON2, "LORTM:SIGNAL_LASER_BIAS_MON2", convertLaserBiasMon)
}

float LORTMImplBase::LaserBiasMon3() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_BIAS_MON3, "LORTM:SIGNAL_LASER_BIAS_MON3", convertLaserBiasMon)
}

float LORTMImplBase::LaserBiasMon4() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_BIAS_MON4, "LORTM:SIGNAL_LASER_BIAS_MON4", convertLaserBiasMon)
}

float LORTMImplBase::TempIntegOutMon1() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_TEMP_INTEG_OUT_MON1, "LORTM:SIGNAL_TEMP_INTEG_OUT_MON1", convertTempIntegMon)
}

float LORTMImplBase::TempIntegOutMon2() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_TEMP_INTEG_OUT_MON2, "LORTM:SIGNAL_TEMP_INTEG_OUT_MON2", convertTempIntegMon)
}

float LORTMImplBase::TempIntegOutMon3() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_TEMP_INTEG_OUT_MON3, "LORTM:SIGNAL_TEMP_INTEG_OUT_MON3", convertTempIntegMon)
}

float LORTMImplBase::TempIntegOutMon4() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_TEMP_INTEG_OUT_MON4, "LORTM:SIGNAL_TEMP_INTEG_OUT_MON4", convertTempIntegMon)
}

float LORTMImplBase::LaserTempMon1() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEMP_MON1, "LORTM:SIGNAL_LASER_TEMP_MON1", convertTemperatureMon)
}

float LORTMImplBase::LaserTempMon2() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEMP_MON2, "LORTM:SIGNAL_LASER_TEMP_MON2", convertTemperatureMon)
}

float LORTMImplBase::LaserTempMon3() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEMP_MON1, "LORTM:SIGNAL_LASER_TEMP_MON3", convertTemperatureMon)
}

float LORTMImplBase::LaserTempMon4() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEMP_MON1, "LORTM:SIGNAL_LASER_TEMP_MON4", convertTemperatureMon)
}

float LORTMImplBase::LaserTecIMon1() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEC_I_MON1, "LORTM:SIGNAL_LASER_TEC_I_MON1", convertTECIMon)
}

float LORTMImplBase::LaserTecIMon2() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEC_I_MON2, "LORTM:SIGNAL_LASER_TEC_I_MON2", convertTECIMon)
}

float LORTMImplBase::LaserTecIMon3() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEC_I_MON3, "LORTM:SIGNAL_LASER_TEC_I_MON3", convertTECIMon)
}

float LORTMImplBase::LaserTecIMon4() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_TEC_I_MON4, "LORTM:SIGNAL_LASER_TEC_I_MON4", convertTECIMon)
}

float LORTMImplBase::LaserPowMon1() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_POW_MON1, "LORTM:SIGNAL_LASER_POW_MON1", convertLaserPowerMon)
}

float LORTMImplBase::LaserPowMon2() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_POW_MON2, "LORTM:SIGNAL_LASER_POW_MON2", convertLaserPowerMon)
}

float LORTMImplBase::LaserPowMon3() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_POW_MON3, "LORTM:SIGNAL_LASER_POW_MON3", convertLaserPowerMon)
}

float LORTMImplBase::LaserPowMon4() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_POW_MON4, "LORTM:SIGNAL_LASER_POW_MON4", convertLaserPowerMon)
}

float LORTMImplBase::PhMixBiasMon1() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_PHMIX_BIAS_MON1, "LORTM:SIGNAL_PHMIX_BIAS_MON1", convertPhmixBiasMon)
}

float LORTMImplBase::PhMixBiasMon2() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_PHMIX_BIAS_MON2, "LORTM:SIGNAL_PHMIX_BIAS_MON2", convertPhmixBiasMon)
}

float LORTMImplBase::PhMixBiasMon3() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_PHMIX_BIAS_MON3, "LORTM:SIGNAL_PHMIX_BIAS_MON3", convertPhmixBiasMon)
}

float LORTMImplBase::PhMixBiasMon4() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_PHMIX_BIAS_MON4, "LORTM:SIGNAL_PHMIX_BIAS_MON4", convertPhmixBiasMon)
}

float LORTMImplBase::LaserSlowCorrMon() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_LASER_SLOW_CORR_MON, "LORTM:SIGNAL_LASER_SLOW_CORR_MON", convertSlowCorrMon)
}

float LORTMImplBase::RFAGCGainMon() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_RF_AGC_GAIN_MON, "LORTM:SIGNAL_RF_AGC_GAIN_MON", convertRFAGCGainMon)
}

float LORTMImplBase::RFPowMon34dB() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_RF_POW_MON_34DB, "LORTM:SIGNAL_RF_POW_MON_34DB", convertRFPowMon34dB)
}

float LORTMImplBase::ExternThermMon() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_EXTERN_THERM_MON, "LORTM:SIGNAL_EXTERN_THERM_MON", convertTemperatureMon)
}

float LORTMImplBase::InternThermMon() {
    SYNCMON2_LOG_FLOAT_CONV(SIGNAL_INTERN_THERM_MON, "LORTM:SIGNAL_INTERN_THERM_MON", convertTemperatureMon)
}

unsigned char LORTMImplBase::LaserGetStatus1() {
    SYNCMON2_LOG_INT(LASER_GET_STATUS1, unsigned char, "LORTM:LASER_GET_STATUS1")
}

unsigned char LORTMImplBase::LaserGetStatus2() {
    SYNCMON2_LOG_INT(LASER_GET_STATUS2, unsigned char, "LORTM:LASER_GET_STATUS2")
}

unsigned char LORTMImplBase::LaserGetStatus3() {
    SYNCMON2_LOG_INT(LASER_GET_STATUS3, unsigned char, "LORTM:LASER_GET_STATUS3")
}

unsigned char LORTMImplBase::LaserGetStatus4() {
    SYNCMON2_LOG_INT(LASER_GET_STATUS4, unsigned char, "LORTM:LASER_GET_STATUS4")
}

bool LORTMImplBase::LaserISrcEnable(int laser) {
    SYNCMON2_LOG_INT(LASER_ISRC_ENABLE, unsigned char, "LORTM:LASER_ISRC_ENABLE")
}

unsigned long LORTMImplBase::LaserFrequency(int laser) {
    SYNCMON2_LOG_INT(LASER_FREQUENCY, unsigned long, "LORTM:LASER_FREQUENCY")
}

float LORTMImplBase::LaserCalibCoeffA(int laser, int coeff) {
    string msg("LORTM:LASER_CALIBRATION_COEFF_A");
    msg += to_string<int>(laser);
    msg += "_";
    msg += to_string<int>(coeff);
    SYNCMON2_LOG_FLOAT(LASER_CALIBRATION_COEFF_A + 3 * laser + coeff, msg.c_str())
}

float LORTMImplBase::LaserCalibCoeffB(int laser, int coeff) {
    string msg("LORTM:LASER_CALIBRATION_COEFF_B");
    msg += to_string<int>(laser);
    msg += "_";
    msg += to_string<int>(coeff);
    SYNCMON2_LOG_FLOAT(LASER_CALIBRATION_COEFF_B + 3 * laser + coeff, msg.c_str())
}

float LORTMImplBase::LaserCalibCoeffC(int laser, int coeff) {
    string msg("LORTM:LASER_CALIBRATION_COEFF_C");
    msg += to_string<int>(laser);
    msg += "_";
    msg += to_string<int>(coeff);
    SYNCMON2_LOG_FLOAT(LASER_CALIBRATION_COEFF_C + 3 * laser + coeff, msg.c_str())
}

unsigned short LORTMImplBase::PhaselockGetStatus() {
    PhaselockGetStatus_value = 0;
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    FEMC_ERROR feStatus = FEMC_NO_ERROR;
    sem_init(&synchLock, 0, 0);
    monitor(PHASELOCK_GET_STATUS, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status == AMBERR_NOERR) {
        feStatus = unpack(PhaselockGetStatus_value, dataLength, data);
        if (feStatus == 0) {
            phaselockLocked = (PhaselockGetStatus_value & 0x0001) != 0; 
            phaselockTuningReady = (PhaselockGetStatus_value & 0x0040) != 0;
            phaselockRFInputReady = (PhaselockGetStatus_value & 0x0080) != 0;
        }
    } else
        feStatus = FEMC_UNPACK_ERROR;
    getLogger().log(FEMC_LOG_MONITOR, "LORTM:PHASELOCK_GET_STATUS", PHASELOCK_GET_STATUS, feStatus, PhaselockGetStatus_value, 0.0);
    return PhaselockGetStatus_value;    
}

unsigned char LORTMImplBase::PhaselockGetSelectedLaser() {
    SYNCMON2_LOG_INT(PHASELOCK_GET_SELECTED_LASER, unsigned char, "LORTM:PHASELOCK_GET_SELECTED_LASER")    
}
    
unsigned char LORTMImplBase::PhaselockGetSelectedBand() {
    SYNCMON2_LOG_INT(PHASELOCK_GET_SELECTED_BAND, unsigned char, "LORTM:PHASELOCK_GET_SELECTED_BAND")    
}

void LORTMImplBase::ResetAMBSI() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(RESET_AMBSI, dummy, "LORTM:RESET_AMBSI")
}

void LORTMImplBase::ResetDevice() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(RESET_DEVICE, dummy, "LORTM:RESET_DEVICE")
}

void LORTMImplBase::SystemManualModeRequest() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SYSTEM_MANUAL_MODE_REQUEST, dummy, "LORTM:SYSTEM_MANUAL_MODE_REQUEST")
}

void LORTMImplBase::SystemStandbyModeRequest() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SYSTEM_STANDBY_MODE_REQUEST, dummy, "LORTM:SYSTEM_STANDBY_MODE_REQUEST")
}

void LORTMImplBase::SystemLoadAllParameters() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SYSTEM_LOAD_ALL_PARAMS, dummy, "LORTM:SYSTEM_LOAD_ALL_PARAMS")
}

void LORTMImplBase::SystemSaveAllParameters() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SYSTEM_SAVE_ALL_PARAMS, dummy, "LORTM:SYSTEM_SAVE_ALL_PARAMS")
}

void LORTMImplBase::SystemClearErrors() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(SYSTEM_CLEAR_ERRORS, dummy, "LORTM:SYSTEM_CLEAR_ERRORS")
}

void LORTMImplBase::PhaseLockTuningInit(unsigned long slaveFreqMHz) {    
    SYNCCMD2_LOG_INT(PHASELOCK_TUNING_INIT, slaveFreqMHz, "LORTM:PHASELOCK_TUNING_INIT")
}

void LORTMImplBase::PhaseLockTuningFinalize() {
    unsigned char dummy(1);
    SYNCCMD2_LOG_BYTE(PHASELOCK_TUNING_FINALIZE, dummy, "LORTM:PHASELOCK_TUNING_FINALIZE")
}

void LORTMImplBase::LaserISrcEnable(int laser, bool enable) {
    if (laser < 0 || laser > 3)
        return;
    unsigned char val(enable ? 1 : 0);
    SYNCCMD2_LOG_BYTE(LASER_ISRC_ENABLE + laser, val, "LORTM:LASER_ISRC_ENABLE")
}

void LORTMImplBase::LaserSetFrequency(int laser, unsigned long slaveFreqMHz) {
    if (laser < 1 || laser > 3)
        return;
    SYNCCMD2_LOG_INT(LASER_FREQUENCY + laser, slaveFreqMHz, "LORTM:LASER_FREQUENCY")
}

void LORTMImplBase::LaserCalibrationSelect(int laser) {
    if (laser < 0 || laser > 3)
        return;
    unsigned char val(laser);
    SYNCCMD2_LOG_BYTE(LL_OPTSW_CALIBRATION, val, "LORTM:LL_OPTSW_CALIBRATION")
}

void LORTMImplBase::monitorAction(Time *timestamp_p) {
    
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 5000 * milliseconds;   
    bool doMonitor = false;

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        LOG(LM_TRACE) << "LORTMImplBase::monitorAction" << endl;
        SystemGetStatus();
        if (systemError)
            SystemGetError_value = SystemGetError();
        if (systemWarning)
            SystemGetWarning_value = SystemGetWarning();
        PhaselockGetStatus();
        if (phaselockRFInputReady) {
            PhaselockGetSelectedLaser_value = PhaselockGetSelectedLaser();
            PhaselockGetSelectedBand_value = PhaselockGetSelectedBand();
        }            
        LaserBiasMon1_value = LaserBiasMon1();
        LaserBiasMon2_value = LaserBiasMon2();
        LaserBiasMon3_value = LaserBiasMon3();
        LaserBiasMon4_value = LaserBiasMon4();
        LaserTempMon1_value = LaserTempMon1();
        LaserTempMon2_value = LaserTempMon2();
        LaserTempMon3_value = LaserTempMon3();
        LaserTempMon4_value = LaserTempMon4();
        LaserSlowCorrMon_value = LaserSlowCorrMon();
        LaserGetStatus1_value = LaserGetStatus1();
        LaserGetStatus2_value = LaserGetStatus2();
        LaserGetStatus3_value = LaserGetStatus3();
        LaserGetStatus4_value = LaserGetStatus4();
    }

    static const Time monitorInterval30s = 30000 * milliseconds;   
    bool doMonitor30s = false;

    if (lastMonitorTime30s == 0 || (*timestamp_p - lastMonitorTime30s >= monitorInterval30s)) {
        lastMonitorTime30s = *timestamp_p;
        doMonitor30s = true;
    }
    
    if (doMonitor30s) {
        AMBSINumErrors();
        AMBSINumTransactions();
        AmbientTemperature();
        
        LaserTecIMon1_value = LaserTecIMon1();
        LaserTecIMon2_value = LaserTecIMon2();
        LaserTecIMon3_value = LaserTecIMon3();
        LaserTecIMon4_value = LaserTecIMon4();
        LaserPowMon1_value = LaserPowMon1();
        LaserPowMon2_value = LaserPowMon2();
        LaserPowMon3_value = LaserPowMon3();
        LaserPowMon4_value = LaserPowMon4();
        PhMixBiasMon1_value = PhMixBiasMon1();
        PhMixBiasMon2_value = PhMixBiasMon2();
        PhMixBiasMon3_value = PhMixBiasMon3();
        PhMixBiasMon4_value = PhMixBiasMon4();
        RFAGCGainMon_value = RFAGCGainMon();
        RFPowMon34dB_value = RFPowMon34dB();
        ExternThermMon_value = ExternThermMon();
        InternThermMon_value = InternThermMon();
        
        /*
        for (int laser = 0; laser < 3; ++laser) {
            for (int coeff = 0; coeff < 3; ++coeff) {        
                LaserCalibCoeffA(laser, coeff);
                LaserCalibCoeffB(laser, coeff);
                LaserCalibCoeffC(laser, coeff);
            }
        }
        */
    }
}


float LORTMImplBase::convertLaserBiasMon(unsigned short adValue) {
    return (float) ((double(adValue) / 65536.0) * 1000.0);
}

float LORTMImplBase::convertTempIntegMon(unsigned short adValue) {
    return (float) ((double(adValue) / 65536.0) * 5.0);
}

float LORTMImplBase::convertTemperatureMon(unsigned short adValue) {
    const double R25C(10000.0);
    double n(adValue);
    n *= R25C;
    double d(32768);
    d -= (double) adValue;
    double x = log (n / d / R25C);
    double y = 1 / (3.3540154e-3 
                  + 2.5627725e-4 * x
                  + 2.0829210e-6 * x * x
                  + 7.3003206e-8 * x * x * x);
    //y -= 273.15;
    return (float) y;
}

float LORTMImplBase::convertTECIMon(unsigned short adValue) {
    return (float) ((double(adValue) / 65536.0) * 5.0);
}

float LORTMImplBase::convertLaserPowerMon(unsigned short adValue) {
    return (float) ((double(adValue) / 65536.0) * 5.0);
}

float LORTMImplBase::convertPhmixBiasMon(unsigned short adValue) {
    return (float) ((double(adValue) / 65536.0) * 25.0);
}

float LORTMImplBase::convertSlowCorrMon(unsigned short adValue) {
    return (float) ((double(adValue) / 65536.0) * 10.0 - 5.0);
}

float LORTMImplBase::convertRFAGCGainMon(unsigned short adValue) {
    return (float) ((-38.0 * (double(adValue) / 65536.0) * 5.0) + 24.0);
}

float LORTMImplBase::convertRFPowMon34dB(unsigned short adValue) {
    return (float) ((22.72 * ((double(adValue) / 65536.0) * 5.0) - 2.64) - 34.4);
}

