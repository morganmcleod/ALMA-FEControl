#ifndef CRYOSTATIMPL_H_
#define CRYOSTATIMPL_H_
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

#include "FEBASE/CryostatImplBase.h"
#include "CONFIG/FrontEndConfig.h"
#include "OPTIMIZE/ThermalLoggable.h"
#include "FEICDataBase.h"
class FrontEndDatabase;
class CryostatPumping;

/// implementation class for the Cryostat module

class CryostatImpl : public CryostatImplBase, public ThermalLoggable {
public:
    CryostatImpl(unsigned long channel, 
                 unsigned long nodeAddress,
                 const std::string &ESN = "");
    virtual ~CryostatImpl();

    void setCryostatConfig(const FEConfig::CryostatConfig &params);
    
    // initiate or stop pump-down process:
    bool cryoPumpingEnable(bool val);

    // start log to database for plot:
    void startCooldownPlot(FrontEndDatabase &dbObject, const FEICDataBase::ID_T &feConfig, FEICDataBase::DATASTATUS_TYPES dataStatus);
    void stopCooldownPlot();

// Cryostat monitor data structure:
    struct Cryostat_t {
        float cryostatTemperature0_value;
        float cryostatTemperature1_value;
        float cryostatTemperature2_value;
        float cryostatTemperature3_value;
        float cryostatTemperature4_value;
        float cryostatTemperature5_value;
        float cryostatTemperature6_value;
        float cryostatTemperature7_value;
        float cryostatTemperature8_value;
        float cryostatTemperature9_value;
        float cryostatTemperature10_value;
        float cryostatTemperature11_value;
        float cryostatTemperature12_value;
        bool backingPumpEnable_value;
        bool turboPumpEnable_value;
        bool turboPumpErrorState_value;
        bool turboPumpHighSpeed_value;
        unsigned char gateValveState_value;
        unsigned char solenoidValveState_value;
        float vacuumCryostatPressure_value;
        float vacuumPortPressure_value;
        bool vacuumGaugeEnable_value;
        bool vacuumGaugeErrorState_value;
        float supplyCurrent230V_value;
    };
    bool getMonitorCryostat(Cryostat_t &target) const;

    void appendThermalLog(std::string &target) const;
    ///< append thermal information to a logging string
    
    void appendThermalLogHeader(std::string &target) const;
    ///< append thermal header information to a logging string
    
private:
    // database access:
    FrontEndDatabase *dbObject_mp;  ///< database interface for cooldown plot.
    FEICDataBase::ID_T headerId_m;  ///< of current cooldown plot.
    bool recordCoolDownPlot_m;      ///< true while recording.

    void impl_getMonitorCryostat(Cryostat_t &target) const;

    float checkCryostatTemperature(float value) const;
    ///< if the temperature is out of bounds, set it to -1



    class CryostatPumping *cryoPumpMonitor_mp;
};

#endif /*CRYOSTATIMPL_H_*/
