/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2005
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
 * Implementation of the DLL exports for LabVIEW.
 * 
 *----------------------------------------------------------------------
 */

#include "lv_wrapper_LORTM.h"
#include "lv_wrapper.h"
#include "lv_structs.h"

#include "iniFile.h"
#include "logger.h"
#include "FrontEndAMB/NICANBusInterface.h"
#include "FrontEndAMB/SocketClientBusInterface.h"
#include "stringConvert.h"
#include "splitPath.h"

#include "LORTMImpl.h"
#include <iomanip>
using namespace std;

namespace FrontEndLVWrapper {
    unsigned long CANChannelLORTM = 0; 
    unsigned long nodeAddressLORTM = 0;
    std::string LORTMIniFilename;
    static LORTMImpl *LORTM = NULL;
    static bool LORTMValid = false;
    static int connectedLORTMClients = 0;
    static double freqRefLaser = 0.0;
};
using namespace FrontEndLVWrapper;

DLLEXPORT short LORTMControlInit() {
    ++connectedLORTMClients;
    if (LORTM && LORTMValid) {
        LOG(LM_INFO) << "FEControlInit: connectedLORTMClients=" << connectedLORTMClients << endl;
        return 0;
    }
    if (LVWrapperInit() < 0) {
        LOG(LM_ERROR) << "FEControlInit: LVWrapperInit failed." << endl;
        return -1;
    }
    LOG(LM_INFO) << "FEControlInit: connectedLORTMClients=" << connectedLORTMClients << endl;
    try {
        // get the path where the FrontendControlDLL.ini file is located:
        string iniPath, tmp;
        splitPath(iniFileName, iniPath, tmp);
        if (iniPath.empty())
            iniPath = ".";

        // open the FrontendControlDLL.ini file:
        CIniFile configINI(iniFileName);
        configINI.ReadFile();

        // look for the item specifying a separate file for LORTM config:
        tmp = configINI.GetValue("configFiles", "LORTM");
        if (tmp.empty())
            // not found.  use the FrontendControlDLL.ini file to load signal source config:
            LORTMIniFilename = iniFileName;
        else {
            // if found, open that file to load the signal source config data from:
            LORTMIniFilename = iniPath + "/" + tmp;
            LOG(LM_INFO) << "Using Teraxion LORTM configuration file '" << LORTMIniFilename << "'" << endl;
            configINI.Clear();
            configINI.SetPath(LORTMIniFilename);
            if (!configINI.ReadFile()) {
                LOG(LM_ERROR) << "Error reading Teraxion LORTM config file." << endl;
                return -1;
            }
        }

        tmp = configINI.GetValue("connection", "channelLORTM");
        if (!tmp.empty())
            CANChannelLORTM = from_string<unsigned long>(tmp);
        tmp  = configINI.GetValue("connection", "nodeAddressLORTM");
        if (!tmp.empty()) {
            if (tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X'))
                tmp = tmp.substr(2);
            nodeAddressLORTM = from_string<unsigned long>(tmp, std::hex);
        }
        if (nodeAddressLORTM > 0) {
            LOG(LM_INFO) << "Connecting to LORTM on CAN" << CANChannelLORTM 
                << " nodeAddress=" << uppercase << hex << setw(2) << setfill('0') << nodeAddressLORTM << dec << setw(0) << endl;
        }
        tmp  = configINI.GetValue("LORTM", "freqRefLaser");
        if (!tmp.empty()) {
            freqRefLaser = from_string<double>(tmp);
            LOG(LM_INFO) << "LORTM configuration: freqRefLaser=" << fixed << setprecision(6) << freqRefLaser << " GHz" << endl;
        }

    } catch (...) {
    	LOG(LM_ERROR) << "LORTMControlInit exception loading configuration file.." << endl;
        return -1;
    }
    
    if (nodeAddressLORTM)
        LORTM = new LORTMImpl(CANChannelLORTM, nodeAddressLORTM);
    else
        LOG(LM_ERROR) << "LORTMControlInit failed. nodeAddressLORTM is 0." << endl;

    if (LORTM) {
        LORTMValid = true;
        if (freqRefLaser > 0.0)
            LORTM -> setFreqRefLaser(freqRefLaser);
        LORTM -> startMonitor();
        return 0;
    } else
        return -1;
}

DLLEXPORT short LORTMControlShutdown() {
    if (!LORTMValid)
        return -1;
    --connectedLORTMClients;
    LOG(LM_INFO) << "LORTMControlShutdown: connectedLORTMClients=" << connectedLORTMClients << endl;
    if (connectedLORTMClients <= 0) {    
        LORTMValid = false;
        LOG(LM_INFO) << "LORTMControlShutdown..." << endl;
        if (LORTM) {
            LORTM -> stopMonitor();
            LORTM -> shutdown();
            WHACK(LORTM);
            LOG(LM_INFO) << "LORTMControlShutdown: LORTM destroyed." << endl;
        }
        LVWrapperShutdown();
    }
    return 0;
}

//----------------------------------------------------------------------------

DLLEXPORT short LORTMGetFirmwareInfo(char *_serialNum,
                                     char *_protocolRev,
                                     char *_firmwareRev)
{
    if (!LORTMValid || !LORTM || !_serialNum || !_protocolRev || !_firmwareRev)
        return -1;
    
    std::string serialNum;
    std::string protocolRev;
    std::string firmwareRev;
    LORTM -> getFirmwareInfo(serialNum, protocolRev, firmwareRev);
    strcpy(_firmwareRev, firmwareRev.c_str());
    strcpy(_protocolRev, protocolRev.c_str());
    strcpy(_serialNum, serialNum.c_str());
    return 0;    
}

DLLEXPORT short LORTMGetAMBSIInfo(unsigned long *numErrors, 
                                  unsigned long *numTransactions)
{
    if (!LORTMValid || !LORTM || !numErrors || !numTransactions)
        return -1;
    LORTM -> getAMBSIInfo(*numErrors, *numTransactions);
    return 0;
}

DLLEXPORT short LORTMGetAmbientTemperature(float *temperature) {
    if (!LORTMValid || !LORTM || !temperature)
        return -1;
    *temperature = LORTM -> AmbientTemperature_value;
    return 0;
}


DLLEXPORT short LORTMResetAMBSI() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> ResetAMBSI();
    return 0;
}

DLLEXPORT short LORTMResetDevice() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> ResetDevice();
    return 0;
}
    
DLLEXPORT short LORTMSystemManualModeRequest() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> SystemManualModeRequest();
    return 0;
}

DLLEXPORT short LORTMSystemStandbyModeRequest() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> SystemStandbyModeRequest();
    return 0;
}

DLLEXPORT short LORTMSystemLoadAllParameters() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> SystemLoadAllParameters();
    return 0;
}

DLLEXPORT short LORTMSystemSaveAllParameters() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> SystemSaveAllParameters();
    return 0;
}

DLLEXPORT short LORTMGetErrorList(char *_errorList) {
    if (!LORTMValid || !LORTM || !_errorList)
        return -1;
    std::string errorList;
    LORTM -> getErrorList(errorList);
    strcpy(_errorList, errorList.c_str());
    return 0;
}

DLLEXPORT short LORTMSystemClearErrors() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> SystemClearErrors();
    return 0;
}

DLLEXPORT short LORTMGetSystemState(unsigned char *state) {
    if (!LORTMValid || !LORTM || !state)
        return -1;
    *state = LORTM -> getSystemState();
    return 0;
}


DLLEXPORT short LORTMGetInterlockStatus(short *status) {
    if (!LORTMValid || !LORTM || !status)
        return -1;
    *status = (LORTM -> getInterlockStatus()) ? 1 : 0;
    return 0;
}

DLLEXPORT short LORTMGetPhaselockStatus(short *status) {
    if (!LORTMValid || !LORTM || !status)
        return -1;
    *status = (LORTM -> getPhaselockStatus()) ? 1 : 0;
    return 0;
}

DLLEXPORT short LORTMGetTuningReady(short *status) {
    if (!LORTMValid || !LORTM || !status)
        return -1;
    *status = (LORTM -> getTuningReady()) ? 1 : 0;
    return 0;
}

DLLEXPORT short LORTMGetRFInputReady(short *status) {
    if (!LORTMValid || !LORTM || !status)
        return -1;
    *status = (LORTM -> getRFInputReady()) ? 1 : 0;
    return 0;
}

DLLEXPORT short LORTMPhaseLockTuningInit(short band, double freqLO1, double freqLO2, double freqFLOOG, short sbLock) {
    if (!LORTMValid || !LORTM)
        return -1;
    if (!LORTM -> PhaseLockTuningInit(band, freqLO1, freqLO2, freqFLOOG, sbLock))
        return -1;
    return 0;
}

DLLEXPORT short LORTMPhaseLockTuningFinalize() {
    if (!LORTMValid || !LORTM)
        return -1;
    LORTM -> PhaseLockTuningFinalize();
    return 0;
}

DLLEXPORT short LORTMPhaseLockGetTuning(double *freqLSB, double *freqUSB,
                                        double *freqRef1, double *freqRef2,
                                        double *freqSlaveLaser, short *factorM)
{
    if (!LORTMValid || !LORTM || !freqLSB || !freqUSB || !freqRef1 || !freqRef2)
        return -1;
    if (!LORTM -> PhaseLockGetTuning(freqLSB, freqUSB, freqRef1, freqRef2, freqSlaveLaser, factorM))
        return -1;
    return 0;
}

DLLEXPORT short LORTMLaserEnable(short laser, short enable) {
    if (!LORTMValid)
        return -1;
    if (!LORTM -> laserEnable(laser, (enable != 0)))
        return -1;
    return 0;
}

DLLEXPORT short LORTMLaserSetFrequency(short laser, double freq) {
    if (!LORTMValid)
        return -1;
    if (!LORTM -> laserSetFrequency(laser, freq))
        return -1;
    return 0;
}

DLLEXPORT short LORTMLaserCalibrationSelect(short laser) {
    if (!LORTMValid)
        return -1;
    LORTM -> LaserCalibrationSelect((int) laser);
    return 0;
}

//----------------------------------------------------------------------------

DLLEXPORT short LORTMGetMonitorStatus(LORTMStatus_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!LORTMValid || !LORTM)
        return -1;
    
    LORTMImpl::LORTMStatus_t modInfo;
    if (LORTM -> getMonitorLORTMStatus(modInfo)) {
        target -> setByte(LORTMStatus_t::SYSTEM_STATE, modInfo.systemState);
        target -> setBool(LORTMStatus_t::SYSTEM_WARNING, modInfo.systemWarning);
        target -> setBool(LORTMStatus_t::SYSTEM_ERROR, modInfo.systemError);
        target -> setBool(LORTMStatus_t::OPERATION_PENDING, modInfo.operationPending);
        target -> setBool(LORTMStatus_t::INTERLOCK_STATUS, modInfo.interlockStatus);
        target -> setShort(LORTMStatus_t::SYSTEM_WARNING_CODE, modInfo.warningCode);
        target -> setShort(LORTMStatus_t::SYSTEM_ERROR_CODE, modInfo.errorCode);
        target -> setBool(LORTMStatus_t::PHASELOCK_LOCKED, modInfo.phaselockLocked);
        target -> setBool(LORTMStatus_t::PHASELOCK_TUNING_READY, modInfo.phaselockTuningReady);
        target -> setBool(LORTMStatus_t::PHASELOCK_RF_INPUT_READY, modInfo.phaselockRFInputReady);
        target -> setByte(LORTMStatus_t::SELECTED_LASER, modInfo.selectedLaser);
        target -> setByte(LORTMStatus_t::SELECTED_BAND, modInfo.selectedBand);
        
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}

DLLEXPORT short LORTMGetMonitorLasers(LORTMLasers_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!LORTMValid || !LORTM)
        return -1;
    
    LORTMImpl::LORTMLasers_t modInfo;
    if (LORTM -> getMonitorLORTMLasers(modInfo)) {
        target -> setByte(LORTMLasers_t::LASER_STATUS1, modInfo.LaserGetStatus1_value);
        target -> setByte(LORTMLasers_t::LASER_STATUS2, modInfo.LaserGetStatus2_value);
        target -> setByte(LORTMLasers_t::LASER_STATUS3, modInfo.LaserGetStatus3_value);
        target -> setByte(LORTMLasers_t::LASER_STATUS4, modInfo.LaserGetStatus4_value);
        target -> setFloat(LORTMLasers_t::LASER_BIAS_MON1, modInfo.LaserBiasMon1_value);
        target -> setFloat(LORTMLasers_t::LASER_BIAS_MON2, modInfo.LaserBiasMon2_value);
        target -> setFloat(LORTMLasers_t::LASER_BIAS_MON3, modInfo.LaserBiasMon3_value);
        target -> setFloat(LORTMLasers_t::LASER_BIAS_MON4, modInfo.LaserBiasMon4_value);
        target -> setFloat(LORTMLasers_t::LASER_TEMP_MON1, modInfo.LaserTempMon1_value);
        target -> setFloat(LORTMLasers_t::LASER_TEMP_MON2, modInfo.LaserTempMon2_value);
        target -> setFloat(LORTMLasers_t::LASER_TEMP_MON3, modInfo.LaserTempMon3_value);
        target -> setFloat(LORTMLasers_t::LASER_TEMP_MON4, modInfo.LaserTempMon4_value);
        target -> setFloat(LORTMLasers_t::LASER_SLOW_CORR_MON, modInfo.LaserSlowCorrMon_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}


DLLEXPORT short LORTMGetMonitorOther(LORTMOther_t * target) {
    if (!target)
        return -1;
    target -> reset();
    if (!LORTMValid || !LORTM)
        return -1;
    
    LORTMImpl::LORTMOther_t modInfo;
    if (LORTM -> getMonitorLORTMOther(modInfo)) {
        target -> setFloat(LORTMOther_t::LASER_TEC_I_MON1, modInfo.LaserTecIMon1_value);
        target -> setFloat(LORTMOther_t::LASER_TEC_I_MON2, modInfo.LaserTecIMon2_value);
        target -> setFloat(LORTMOther_t::LASER_TEC_I_MON3, modInfo.LaserTecIMon3_value);
        target -> setFloat(LORTMOther_t::LASER_TEC_I_MON4, modInfo.LaserTecIMon4_value);
        target -> setFloat(LORTMOther_t::LASER_POW_MON1, modInfo.LaserPowMon1_value);
        target -> setFloat(LORTMOther_t::LASER_POW_MON2, modInfo.LaserPowMon2_value);
        target -> setFloat(LORTMOther_t::LASER_POW_MON3, modInfo.LaserPowMon3_value);
        target -> setFloat(LORTMOther_t::LASER_POW_MON4, modInfo.LaserPowMon4_value);
        target -> setFloat(LORTMOther_t::PHMIX_BIAS_MON1, modInfo.PhMixBiasMon1_value);
        target -> setFloat(LORTMOther_t::PHMIX_BIAS_MON2, modInfo.PhMixBiasMon2_value);
        target -> setFloat(LORTMOther_t::PHMIX_BIAS_MON3, modInfo.PhMixBiasMon3_value);
        target -> setFloat(LORTMOther_t::PHMIX_BIAS_MON4, modInfo.PhMixBiasMon4_value);        
        target -> setFloat(LORTMOther_t::RF_AGC_GAIN_MON, modInfo.RFAGCGainMon_value);
        target -> setFloat(LORTMOther_t::RF_POW_MON_34DB, modInfo.RFPowMon34dB_value);
        target -> setFloat(LORTMOther_t::EXTERN_THERM_MON, modInfo.ExternThermMon_value);
        target -> setFloat(LORTMOther_t::INTERN_THERM_MON, modInfo.InternThermMon_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;       
    }
    return -1;
}



