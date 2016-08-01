/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
*
*This library is free software(0), you can redistribute it and/or
*modify it under the terms of the GNU Lesser General Public
*License as published by the Free Software Foundation(0), either
*version 2.1 of the License, or (at your option) any later version.
*
*This library is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY(0), without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*Lesser General Public License for more details.
*
*You should have received a copy of the GNU Lesser General Public
*License along with this library(0), if not, write to the Free Software
*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*
*/

/************************************************************************
 *----------------------------------------------------------------------
 */
 
#include "PowerModuleImplBase.h"
#include "MonCtrlMacros.h"
#include "logger.h"
using namespace std;

PowerModuleImplBase::PowerModuleImplBase(const std::string &name, int port)
  : FEHardwareDevice(name),
    voltageP6V_value(0),
    currentP6V_value(0),
    voltageN6V_value(0),
    currentN6V_value(0),
    voltageP15V_value(0),
    currentP15V_value(0),
    voltageN15V_value(0),
    currentN15V_value(0),
    voltageP24V_value(0),
    currentP24V_value(0),
    voltageP8V_value(0),
    currentP8V_value(0),
    enableModule_value(false),
    voltageP6V_status(FEMC_NOT_CONNECTED),
    currentP6V_status(FEMC_NOT_CONNECTED),
    voltageN6V_status(FEMC_NOT_CONNECTED),
    currentN6V_status(FEMC_NOT_CONNECTED),
    voltageP15V_status(FEMC_NOT_CONNECTED),
    currentP15V_status(FEMC_NOT_CONNECTED),
    voltageN15V_status(FEMC_NOT_CONNECTED),
    currentN15V_status(FEMC_NOT_CONNECTED),
    voltageP24V_status(FEMC_NOT_CONNECTED),
    currentP24V_status(FEMC_NOT_CONNECTED),
    voltageP8V_status(FEMC_NOT_CONNECTED),
    currentP8V_status(FEMC_NOT_CONNECTED),
    enableModule_status(FEMC_NOT_CONNECTED),
    baseRCA(0),
    port_m(port),
    lastMonitorTime(0),
    monitorPhase(0)
{
    baseRCA = port - 1;
    baseRCA <<= 4;
    baseRCA += 0xA000;
    
    voltageP6V_RCA  = baseRCA + VOLTAGE_P6V;  
    currentP6V_RCA  = baseRCA + CURRENT_P6V;
    voltageN6V_RCA  = baseRCA + VOLTAGE_N6V;
    currentN6V_RCA  = baseRCA + CURRENT_N6V;
    voltageP15V_RCA = baseRCA + VOLTAGE_P15V;
    currentP15V_RCA = baseRCA + CURRENT_P15V;
    voltageN15V_RCA = baseRCA + VOLTAGE_N15V;
    currentN15V_RCA = baseRCA + CURRENT_N15V;
    voltageP24V_RCA = baseRCA + VOLTAGE_P24V;
    currentP24V_RCA = baseRCA + CURRENT_N24V;
    voltageP8V_RCA  = baseRCA + VOLTAGE_P8V;
    currentP8V_RCA  = baseRCA + CURRENT_P8V;
    enableModule_RCA= baseRCA + ENABLE_MODULE;

    // Add all the analog monitor points to the registry:
    
    addMon(&voltageP6V_value, &PowerModuleImplBase::voltageP6V);
    addMon(&currentP6V_value, &PowerModuleImplBase::currentP6V);
    addMon(&voltageN6V_value, &PowerModuleImplBase::voltageN6V);
    addMon(&currentN6V_value, &PowerModuleImplBase::currentN6V);
    addMon(&voltageP15V_value, &PowerModuleImplBase::voltageP15V);
    addMon(&currentP15V_value, &PowerModuleImplBase::currentP15V);
    addMon(&voltageN15V_value, &PowerModuleImplBase::voltageN15V);
    addMon(&currentN15V_value, &PowerModuleImplBase::currentN15V);
    addMon(&voltageP24V_value, &PowerModuleImplBase::voltageP24V);
    addMon(&currentP24V_value, &PowerModuleImplBase::currentP24V);
    addMon(&voltageP8V_value, &PowerModuleImplBase::voltageP8V);
    addMon(&currentP8V_value, &PowerModuleImplBase::currentP8V);

    nextMon = monitorRegistry.begin();
}

void PowerModuleImplBase::initialize(unsigned long channel, unsigned long nodeAddress) {
    m_channel = channel;
    m_nodeAddress = nodeAddress;
}

void PowerModuleImplBase::shutdown() {
}

// monitor properties:
float PowerModuleImplBase::voltageP6V() {
    SYNCMON_LOG_FLOAT(voltageP6V, "POWER_DIST_VOLTAGE_P6V")
}
float PowerModuleImplBase::currentP6V() {
    SYNCMON_LOG_FLOAT(currentP6V, "POWER_DIST_CURRENT_P6V")
}
float PowerModuleImplBase::voltageN6V() {
    SYNCMON_LOG_FLOAT(voltageN6V, "POWER_DIST_VOLTAGE_N6V")
}
float PowerModuleImplBase::currentN6V() {
    SYNCMON_LOG_FLOAT(currentN6V, "POWER_DIST_CURRENT_N6V")
}
float PowerModuleImplBase::voltageP15V() {
    SYNCMON_LOG_FLOAT(voltageP15V, "POWER_DIST_VOLTAGE_P15V")
}
float PowerModuleImplBase::currentP15V() {
    SYNCMON_LOG_FLOAT(currentP15V, "POWER_DIST_CURRENT_P15V")
}
float PowerModuleImplBase::voltageN15V() {
    SYNCMON_LOG_FLOAT(voltageN15V, "POWER_DIST_VOLTAGE_N15V")
}
float PowerModuleImplBase::currentN15V() {
    SYNCMON_LOG_FLOAT(currentN15V, "POWER_DIST_CURRENT_N15V")
}
float PowerModuleImplBase::voltageP24V() {
    SYNCMON_LOG_FLOAT(voltageP24V, "POWER_DIST_VOLTAGE_P24V")
}
float PowerModuleImplBase::currentP24V() {
    SYNCMON_LOG_FLOAT(currentP24V, "POWER_DIST_CURRENT_N24V")
}
float PowerModuleImplBase::voltageP8V() {
    SYNCMON_LOG_FLOAT(voltageP8V, "POWER_DIST_VOLTAGE_P8V")
}
float PowerModuleImplBase::currentP8V() {
    SYNCMON_LOG_FLOAT(currentP8V, "POWER_DIST_CURRENT_P8V")
}
bool PowerModuleImplBase::enableModule() {
    SYNCMON_LOG_BOOL(enableModule, "POWER_DIST_ENABLE_MODULE")
}

// control points:
void PowerModuleImplBase::enableModule(bool val) {
    SYNCCMD_LOG_INT(enableModule, val, "POWER_DIST_ENABLE_MODULE")
}

void PowerModuleImplBase::monitorAction(Time *timestamp_p) {
    if (!timestamp_p)
        return;

    // constant factor to convert 100 ns ticks to ms:
    static const Time milliseconds = 10000;
    static const Time monitorInterval = 2000 * milliseconds;   

    bool doMonitor = false;

    if (lastMonitorTime == 0 && logMonitors_m)
        logMon(true);

    if (lastMonitorTime == 0 || (*timestamp_p - lastMonitorTime >= monitorInterval)) {
        lastMonitorTime = *timestamp_p;
        doMonitor = true;
    }

    if (doMonitor) {
        enableModule_value = enableModule();
        while (executeNextMon()) ;
        if (randomizeAnalogMonitors_m)
            randomizeMon();
        if (logMonitors_m)
           logMon();
    }
}

DEFINE_MONITORS_REGISTRY(PowerModuleImplBase)

void PowerModuleImplBase::logMon(bool printHeader) const {
    if (printHeader) {
        LOG(LM_INFO)  << "AllMonitors:PowerModule(" << port_m << "): randomized,enableModule,"
                         "voltageP6V,currentP6V,voltageN6V,currentN6V,"
                         "voltageP15V,currentP15V,voltageN15V,currentN15V,"
                         "voltageP24V,currentP24V,voltageP8V,currentP8V" << endl;
    } else {
        LOG(LM_INFO)  << "AllMonitors:PowerModule(" << port_m << "): " << randomizeAnalogMonitors_m << ","
                      << (enableModule_value ? 1 : 0) << ","
                      << voltageP6V_value << "," << currentP6V_value << "," << voltageN6V_value << "," << currentN6V_value << ","
                      << voltageP15V_value << "," << currentP15V_value << "," << voltageN15V_value << "," << currentN15V_value << ","
                      << voltageP24V_value << "," << currentP24V_value << "," << voltageP8V_value << "," << currentP8V_value << endl;
    }
}

