#ifndef FRONTENDDATABASE_H_
#define FRONTENDDATABASE_H_
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

#include "FEICDataBase.h"
#include "ColdCartImpl.h"
#include "Configuration.h"
#include "CryostatImpl.h"
#include "FETIMImpl.h"
#include "IFSwitchImpl.h"
#include "LPRImpl.h"
#include "PowerModuleImpl.h"
#include "WCAImpl.h"
class IFPowerDataSet;
class XYPlotArray;
namespace FEConfig {
    class MixerParams;
    class MagnetParams;
    class ColdCartConfig;
    class PowerAmpParams;
};

class FrontEndDatabase : public FEICDataBase {
public:
    FrontEndDatabase();

    FrontEndDatabase(const std::string &host,
                     const std::string &user,
                     const std::string &password,
                     const std::string &dbName);

    virtual ~FrontEndDatabase()
      {}

//-----------------------------------------------------------------------------
// Extended configuration operations:

    bool getConfigurationRecord(const ID_T &configId, FEConfig::Configuration::Record &target) const;

    bool getMixerParams(const ID_T &componentId, FEConfig::MixerParams &mxrTarget, FEConfig::MagnetParams &magTarget) const;

    bool getPreampParams(const ID_T &componentId, FEConfig::ColdCartConfig &target) const;

    bool getWCAYIGLimits(const ID_T &componentId, double &FLOYIG, double &FHIYIG) const;

    bool getWCALOParams(const ID_T &componentId, FEConfig::PowerAmpParams &target) const;

//-----------------------------------------------------------------------------
// Get configuration component IDs:

    enum COMPONENT_TYPES {
        CTYPE_CRYOSTAT = 6,
        CTYPE_LPR = 17,
        CTYPE_CCA = 20,
        CTYPE_WCA = 11,
        CTYPE_FETIM = 221,
        CTYPE_FLOOGM_MODULE = 96,
        CTYPE_IFSWICH_ASSY = 129,
        CTYPE_IFSWICH_SUBRACK = 162,
        CTYPE_CPDS_ASSY = 4,
        CTYPE_CPDS_SUBRACK = 159
    };

    bool getConfigCryostat(const ID_T &configId, ID_T &componentId, std::string &SN) const
      { return getConfigComponent(configId, componentId, SN, CTYPE_CRYOSTAT); }

    bool getConfigLPR(const ID_T &configId, ID_T &componentId, std::string &SN) const
      { return getConfigComponent(configId, componentId, SN, CTYPE_LPR); }

    bool getConfigFETIM(const ID_T &configId, ID_T &componentId, std::string &SN) const
      { return getConfigComponent(configId, componentId, SN, CTYPE_FETIM); }

    bool getConfigFLOOGDistModule(const ID_T &configId, ID_T &componentId, std::string &SN) const
      { return getConfigComponent(configId, componentId, SN, CTYPE_FLOOGM_MODULE); }

    bool getConfigIFSwitchAssy(const ID_T &configId, ID_T &componentId, std::string &SN) const
      { if (getConfigComponent(configId, componentId, SN, CTYPE_IFSWICH_ASSY)) return true;
        else return getConfigComponent(configId, componentId, SN, CTYPE_IFSWICH_SUBRACK); }

    bool getConfigCPDSAssy(const ID_T &configId, ID_T &componentId, std::string &SN) const
      { if (getConfigComponent(configId, componentId, SN, CTYPE_CPDS_ASSY)) return true;
        else return getConfigComponent(configId, componentId, SN, CTYPE_CPDS_SUBRACK); }

    bool getConfigCCA(const ID_T &configId, int band, ID_T &componentId, std::string &SN) const
      { return getConfigComponent(configId, componentId, SN, CTYPE_CCA, band); }

    bool getConfigWCA(const ID_T &configId, int band, ID_T &componentId, std::string &SN) const
      { return getConfigComponent(configId, componentId, SN, CTYPE_WCA, band); }

//-----------------------------------------------------------------------------
// PAS monitor data insert operations.   These all create one TestData_header record in addition to the monitor data record(s).

    /// Types of test data we can insert/retrieve from the database:
    enum TESTDATA_TYPES {
        TD_CCA_LNA_BIAS         = 1,
        TD_CCA_TEMPSENSORS      = 2,
        TD_CCA_SIS_BIAS         = 3,
        TD_CRYOSTATTEMPS        = 4,
        TD_FLOOGDIST            = 5,
        TD_IFTOTALPOWER         = 6,
        TD_IFSPECTRUM           = 7,
        TD_LPR_WARMHEALTH       = 8,
        TD_PHOTOMIXER_WARMHEALTH= 9,
        TD_IFSWITCHTEMPS        = 10,
        TD_WCA_AMC_BIAS         = 12,
        TD_WCA_PA_BIAS          = 13,
        TD_WCA_PLL_BIAS         = 14,
        TD_YFACTOR              = 15,
        TD_CPDS_MONITOR         = 24,
        TD_IV_CURVE             = 39,
        TD_FINE_LO_SWEEP        = 59
    };

    bool insertCryostatData(const ID_T &configId, DATASTATUS_TYPES dataStatus, const CryostatImpl::Cryostat_t &source) const;
    ///< insert cryostat monitor data into database

    bool insertIfSwitchData(const ID_T &configId, DATASTATUS_TYPES dataStatus, const IFSwitchImpl::IFSwitch_t &source) const;
    ///< insert IF switch monitor data into database

    bool insertLPRMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus, const LPRImpl::LPR_t &source) const;
    ///< insert LPR monitor data into database

    bool findOrCreatePowerModuleDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus) const;
    ///< create or find the latest test data header for saving many bands' CPDS voltages and currents monitor data.

    bool insertPowerModuleData(const ID_T &headerId, unsigned band, double FreqLO, PowerModuleImpl::PowerModule_t &source) const;
    ///< insert CPDS voltages and currents monitor data into database

    bool findOrCreatePhotomixerDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus) const;
    ///< create or find the latest test data header for saving many bands' photomixer monitor data.

    bool insertPhotomixerData(const ID_T &headerId, unsigned band, const WCAImpl::Photomixer_t &source) const;
    ///< insert photomixer monitor data, attached to a previously created TestData_header record.

    bool insertPLLMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                              unsigned band, double FreqLO, const WCAImpl::PLL_t &source) const;
    ///< insert WCA PLL monitor data into database.

    bool findOrCreateFLOOGDistHealthDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus) const;
    ///< create or find the latest test data header for saving many bands' Ref Total Power monitor data.

    bool insertFLOOGDistHealthData(const ID_T &headerId, unsigned band, float refTotalPower) const;
    ///< insert Ref Total Power monitor data, attached to a previously created TestData_header record.

    bool insertAMCMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                              unsigned band, double FreqLO, const WCAImpl::AMC_t &source) const;
    ///< insert WCA AMC monitor data into database.

    bool insertPAMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                             unsigned band, double FreqLO, const WCAImpl::PA_t &source) const;
    ///< insert WCA PA monitor data into database.

    bool insertCartTempData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                            unsigned band, const ColdCartImpl::CartridgeTemp_t &source) const;
    ///< insert cartridge temperatures monitor data into database.

    bool createSISMonitorDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band) const;
    ///< create a test data header for saving up to four mixers' SIS monitor data.

    bool insertSISMonitorData(const ID_T &headerId, double FreqLO, unsigned pol, unsigned sb, const ColdCartImpl::SIS_t &source) const;
    ///< insert SIS monitor data for a single mixer, attached to a previously created TestData_header record.

    bool createLNAMonitorDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band) const;
    ///< create a test data header for saving up to four preamps' LNA monitor data.

    bool insertLNAMonitorData(const ID_T &headerId, double FreqLO, unsigned pol, unsigned sb, const ColdCartImpl::LNA_t &source) const;
    ///< insert LNA monitor data for three preamp stages, attached to a previously created TestData_header record.

    bool insertIVCurveData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                           unsigned band, double FreqLO, unsigned pol, unsigned sb,
                           const std::string &legend, const XYPlotArray &source) const;
    ///< insert the given I-V curve into the database. legend gives additional descriptive text for this curve.

    bool insertIFTotalPowerData(const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band,
                                double FreqLO, const std::string &legend, const IFPowerDataSet &data) const;
    ///< insert IF total power data into the database.

    bool insertYFactorData(const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band,
                           double FreqLO, const std::string &legend, const IFPowerDataSet &data) const;
    ///< insert Y-factor data into the database.

    bool createFineLOSweepDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band, const std::string &legend) const;
    ///< create a new TDH for saving multiple traces of Fine LO Sweep data.

    bool insertFineLOSweepSubHeader(ID_T &subHeaderId, const ID_T &headerId, unsigned band, unsigned pol,
                                    float tiltAngle, double startLOFreq, double stopLOFreq, double stepLOFreq,
                                    float SISVoltage, float targetSIS1Current, const std::string &legend) const;
    ///< create a new Fine LO Sweep subHeader attached to a previously created TestData_header record.

    bool insertFineLOSSweepData(const ID_T &subHeaderId, const XYPlotArray &sisCurrents, const XYPlotArray &loPaVoltages) const;
    ///< insert Fine LO Sweep data, attached to a previously created subHeader record.

private:
    std::string measSWVer_m;
};

#endif /* FRONTENDDATABASE_H_ */
