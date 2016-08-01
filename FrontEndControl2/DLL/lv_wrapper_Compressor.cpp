/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
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


#include "lv_wrapper_Compressor.h"
#include "lv_wrapper.h"
#include "lv_structs.h"

#include "iniFile.h"
#include "logger.h"
#include "FrontEndAMB/NICANBusInterface.h"
#include "FrontEndAMB/SocketClientBusInterface.h"
#include "stringConvert.h"

#include "CompressorImpl.h"
#include <iomanip>
using namespace std;

namespace FrontEndLVWrapper {
    unsigned long CANChannelCompressor = 0;
    unsigned long nodeAddressCompressor = 0;
    static CompressorImpl *Compressor = NULL;
    static bool CompressorValid = false;
    static int connectedCompressorClients = 0;
};
using namespace FrontEndLVWrapper;

DLLEXPORT short CompressorControlInit() {
    ++connectedCompressorClients;

    if (Compressor && CompressorValid) {
        LOG(LM_INFO) << "CompressorControlInit: connectedCompressorClients=" << connectedCompressorClients << endl;
        return 0;
    }
    if (LVWrapperInit() < 0) {
        LOG(LM_ERROR) << "CompressorControlInit: LVWrapperInit failed." << endl;
        return -1;
    }
    LOG(LM_INFO) << "CompressorControlInit: connectedCompressorClients=" << connectedCompressorClients << endl;
    try {
        CIniFile configINI(iniFileName);
        configINI.ReadFile();
        std::string tmp;

        tmp = configINI.GetValue("connection", "channelCompressor");
        if (!tmp.empty())
            CANChannelCompressor = from_string<unsigned long>(tmp);
        tmp  = configINI.GetValue("connection", "nodeAddressCompressor");
        if (!tmp.empty()) {
            if (tmp[0] == '0' && (tmp[1] == 'x' || tmp[1] == 'X'))
                tmp = tmp.substr(2);
            nodeAddressCompressor = from_string<unsigned long>(tmp, std::hex);
        }
        if (nodeAddressCompressor > 0) {
            LOG(LM_INFO) << "Connecting to Compressor M&C module on CAN" << CANChannelCompressor
                << " nodeAddress=" << uppercase << hex << setw(2) << setfill('0') << nodeAddressCompressor << dec << setw(0) << endl;
        }

    } catch (...) {
        LOG(LM_ERROR) << "CompressorControlInit exception loading configuration file.." << endl;
        return -1;
    }

    if (nodeAddressCompressor)
        Compressor = new CompressorImpl(CANChannelCompressor, nodeAddressCompressor);
    else
        LOG(LM_ERROR) << "CompressorControlInit failed. nodeAddressCompressor is 0." << endl;

    if (Compressor) {
        CompressorValid = true;
        Compressor -> setThermalLogInterval(thermalLogInterval);
        Compressor -> startMonitor();
        return 0;
    } else
        return -1;
}

DLLEXPORT short CompressorControlShutdown() {
    if (!CompressorValid)
        return -1;
    --connectedCompressorClients;
    LOG(LM_INFO) << "CompressorControlShutdown: connectedCompressorClients=" << connectedCompressorClients << endl;
    if (connectedCompressorClients <= 0) {
        CompressorValid = false;
        LOG(LM_INFO) << "CompressorControlShutdown..." << endl;
        if (Compressor) {
            Compressor -> stopMonitor();
            Compressor -> shutdown();
            WHACK(Compressor);
            LOG(LM_INFO) << "CompressorControlShutdown: Compressor destroyed." << endl;
        }
        LVWrapperShutdown();
    }
    return 0;
}

DLLEXPORT short CompressorGetAMBSIInfo(char *_serialNum,
                                       char *_firmwareRev,
                                       unsigned long *numErrors,
                                       unsigned long *numTransactions)
{
    if (!CompressorValid || !Compressor || !_serialNum || !_firmwareRev || !numErrors || !numTransactions)
        return -1;

    string serialNum = Compressor -> AMBSISerialNum();
    string firmwareRev = Compressor -> AMBSIFirmwareRev();
    *numErrors = Compressor -> AMBSINumErrors();
    *numTransactions = Compressor -> AMBSINumTransactions();
    strcpy(_serialNum, serialNum.c_str());
    strcpy(_firmwareRev, firmwareRev.c_str());
    return 0;
}

DLLEXPORT short CompressorGetAMBSITemperature(float *temperature) {
    if (!CompressorValid || !Compressor || !temperature)
        return -1;
    *temperature =  Compressor -> AMBSITemperature();
    return 0;
}

DLLEXPORT short CompressorGetSWRevisionLevel(char *_swRev) {
    if (!CompressorValid || !Compressor || !_swRev)
        return -1;
    string swRev = Compressor -> getSWRevisionLevel();
    strcpy(_swRev, swRev.c_str());
    return 0;
}

DLLEXPORT short CompressorGetECUType(short *val) {
    if (!CompressorValid || !Compressor || !val)
        return -1;
    *val = Compressor -> getECUType();
    return 0;
}

DLLEXPORT short CompressorGetInterlockOverride(short *val) {
    if (!CompressorValid || !Compressor || !val)
        return -1;
    *val = (Compressor -> getInterlockOverride()) ? 1 : 0;
    return 0;
}

DLLEXPORT short CompressorSetRemoteDrive(short val) {
    if (!CompressorValid || !Compressor)
        return -1;
    Compressor -> setRemoteDrive(val != 0);
    return 0;
}

DLLEXPORT short CompressorSetRemoteReset() {
    if (!CompressorValid || !Compressor)
        return -1;
    Compressor -> setRemoteReset();
    return 0;
}

DLLEXPORT short CompressorSetFaultLatchReset() {
    if (!CompressorValid || !Compressor)
        return -1;
    Compressor -> setFaultLatchReset();
    return 0;
}

DLLEXPORT short CompressorGetMonitorData(CompressorData_t *target) {
    if (!target)
        return -1;
    target -> reset();
    if (!CompressorValid || !Compressor)
        return -1;

    CompressorImpl::Compressor_t modInfo;
    if (Compressor -> getMonitorCompressor(modInfo)) {
        target -> setFloat(CompressorData_t::TEMP1, modInfo.getTemp1_value);
        target -> setFloat(CompressorData_t::TEMP2, modInfo.getTemp2_value);
        target -> setFloat(CompressorData_t::TEMP3, modInfo.getTemp3_value);
        target -> setFloat(CompressorData_t::TEMP4, modInfo.getTemp4_value);
        target -> setFloat(CompressorData_t::RETURN_PRESSURE, modInfo.getReturnLinePressure_value);
        target -> setFloat(CompressorData_t::SUPPLY_PRESSURE, modInfo.getSupplyPressure_value);
        target -> setFloat(CompressorData_t::AUX_TRANSDUCER, modInfo.getAuxTransducer_value);
        target -> setULong(CompressorData_t::TIME_SINCE_POWER_ON, modInfo.getTimeSinceLastPowerOn_value);
        target -> setULong(CompressorData_t::TIME_SINCE_POWER_OFF, modInfo.getTimeSinceLastPowerOff_value);
        target -> setBool(CompressorData_t::TEMP_ALARM, modInfo.getTempAlarm_value);
        target -> setBool(CompressorData_t::PRESSURE_ALARM, modInfo.getPressureAlarm_value);
        target -> setBool(CompressorData_t::DRIVE_INDICATOR, modInfo.getDriveIndicator_value);
        target -> setBool(CompressorData_t::ICCU_STATUS, modInfo.getICCUStatusError_value);
        target -> setBool(CompressorData_t::ICCU_CABLE_ERROR, modInfo.getICCUCableError_value);
        target -> setBool(CompressorData_t::FETIM_STATUS, modInfo.getFETIMStatusError_value);
        target -> setBool(CompressorData_t::FETIM_CABLE_ERROR, modInfo.getFETIMCableError_value);
        target -> setBool(CompressorData_t::INTERLOCK_OVERRIDE, modInfo.getInterlockOverride_value);
        target -> setBool(CompressorData_t::FAULT_STATUS, modInfo.getFaultStatusError_value);
        if (debugLVStructures)
            LOG(LM_DEBUG) << *target;
        return 0;
    }
    return -1;
}

