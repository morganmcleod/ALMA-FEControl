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

#include "FrontEndDatabase.h"
#include "MySQLConnection.h"
#include "stringConvert.h"
#include "DLL/SWVersion.h"
#include "CartConfig.h"
#include "IFPowerDataSet.h"
#include "OPTIMIZE/XYPlotArray.h"

using namespace std;
using namespace FEConfig;

//-----------------------------------------------------------------------------
// utility macros:

static char *f2EndPtr;

#define FIELD_TO_UNSIGNED(COL) \
    ((unsigned) (row[(COL)]) ? strtoul(row[(COL)], &f2EndPtr, 10) : 0)

#define FIELD_TO_SINGLE(COL) \
    ((row[(COL)]) ? (float) atof(row[(COL)]) : 0.0)

#define FIELD_TO_DOUBLE(COL) \
    ((row[(COL)]) ? (double) atof(row[(COL)]) : 0.0)

#define FIELD_TO_STRING(COL) \
    ((row[(COL)]) ? row[(COL)] : "")

//-----------------------------------------------------------------------------

FrontEndDatabase::FrontEndDatabase()
  : FEICDataBase(),
    measSWVer_m(FECONTROL_SW_VERSION_STRING)
    {}

FrontEndDatabase::FrontEndDatabase(const std::string &host,
                                   const std::string &user,
                                   const std::string &password,
                                   const std::string &dbName)
  : FEICDataBase(host, user, password, dbName),
    measSWVer_m(FECONTROL_SW_VERSION_STRING)
    {}


//-----------------------------------------------------------------------------
// Extended configuration operations:

bool FrontEndDatabase::getConfigurationRecord(const ID_T &configId, FEConfig::Configuration::Record &target) const {
    static const string context("FrontEndDatabase::getConfigurationRecord");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    string query = "SELECT keyFacility, keyFEConfig, fkFront_Ends, Description FROM FE_Config WHERE "
                 + configId.whereClause("keyFacility", "keyFEConfig") + ";";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1))
        return false;

    MYSQL_ROW row = res.fetch_row();
    if (row == NULL)
        return false;

    string tmp;
    LOG(LM_INFO) << context << ": row=(" << conn_mp -> rowToString(res, row, tmp) << ")" << endl;

    MySQLConnection::unpackString(row, 0, tmp);
    target.keyFacility_m = from_string<unsigned>(tmp);
    MySQLConnection::unpackString(row, 1, tmp);
    target.configId_m = from_string<unsigned>(tmp);
    MySQLConnection::unpackString(row, 2, tmp);
    target.keyFrontEnd_m = from_string<unsigned>(tmp);
    MySQLConnection::unpackString(row, 3, target.description_m);

    target.CAId_m = CartAssemblyID(); // TODO: support configuration with only a cartassembly?
    return true;
}

bool FrontEndDatabase::getMixerParams(const ID_T &componentId, FEConfig::MixerParams &mxrTarget, FEConfig::MagnetParams &magTarget) const {
    static const string context("FrontEndDatabase::getMixerParams");

    mxrTarget.clear();
    magTarget.clear();

    if (!isConnected())
        return false;

    if (!componentId.valid())
        return false;

    string query = "SELECT FreqLO, Pol, SB, VJ, IJ, IMAG FROM CCA_MixerParams WHERE " + componentId.whereClause("fkFacility", "fkComponent")
                 + " ORDER BY FreqLO, Pol, SB;";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (numRows <= 0) {
        LOG(LM_ERROR) << context << ": error or no matching records." << endl;
        return false;
    }

    MYSQL_ROW row = NULL;
    double freqLO;
    unsigned pol, sb;
    float VJ, IJ, IMAG;

    do {
        freqLO = 0.0;
        pol = sb = 999;
        VJ = IJ = IMAG = 0.0;

        row = res.fetch_row();
        if (row != NULL) {
            freqLO = FIELD_TO_DOUBLE(0);
            pol = FIELD_TO_UNSIGNED(1);
            sb = FIELD_TO_UNSIGNED(2);
            VJ = FIELD_TO_SINGLE(3);
            IJ = FIELD_TO_SINGLE(4);
            IMAG = FIELD_TO_SINGLE(5);

            if ((freqLO > 0.0) && (pol == 0 || pol == 1) && (sb == 1 || sb == 2)) {
                mxrTarget.set(freqLO, MixerParams::indexVJ(pol, sb), VJ);
                mxrTarget.set(freqLO, MixerParams::indexIJ(pol, sb), IJ);
                magTarget.set(freqLO, MagnetParams::indexIMag(pol, sb), IMAG);
            }
        }
    } while (row != NULL);

    return true;
}


bool FrontEndDatabase::getPreampParams(const ID_T &componentId, ColdCartConfig &target) const {
    static const string context("FrontEndDatabase::getPreampParams");

    if (!isConnected())
        return false;

    if (!componentId.valid())
        return false;

    string query = "SELECT FreqLO, Pol, SB, VD1, VD2, VD3, ID1, ID2, ID3 FROM CCA_PreampParams WHERE " + componentId.whereClause("fkFacility", "fkComponent")
                 + " ORDER BY FreqLO, Pol, SB;";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (numRows <= 0) {
        LOG(LM_ERROR) << context << ": error or no matching records." << endl;
        return false;
    }

    MYSQL_ROW row = NULL;
    double freqLO;
    unsigned pol, sb;
    float VD1, VD2, VD3, ID1, ID2, ID3;

    do {
        freqLO = 0.0;
        pol = sb = 999;
        VD1 = VD2 = VD3 = ID1 = ID2 = ID3 = 0.0;

        row = res.fetch_row();
        if (row != NULL) {
            freqLO = FIELD_TO_DOUBLE(0);
            pol = FIELD_TO_UNSIGNED(1);
            sb = FIELD_TO_UNSIGNED(2);
            VD1 = FIELD_TO_SINGLE(3);
            VD2 = FIELD_TO_SINGLE(4);
            VD3 = FIELD_TO_SINGLE(5);
            ID1 = FIELD_TO_SINGLE(6);
            ID2 = FIELD_TO_SINGLE(7);
            ID3 = FIELD_TO_SINGLE(8);

            if ((freqLO > 0.0) && (pol == 0 || pol == 1) && (sb == 1 || sb == 2)) {
                target.usePreampParams(pol, sb).set(freqLO, PreampParams::VD1, VD1);
                target.usePreampParams(pol, sb).set(freqLO, PreampParams::VD2, VD2);
                target.usePreampParams(pol, sb).set(freqLO, PreampParams::VD3, VD3);
                target.usePreampParams(pol, sb).set(freqLO, PreampParams::ID1, ID1);
                target.usePreampParams(pol, sb).set(freqLO, PreampParams::ID2, ID2);
                target.usePreampParams(pol, sb).set(freqLO, PreampParams::ID3, ID3);
            }
        }
    } while (row != NULL);

    return true;
}


bool FrontEndDatabase::getWCAYIGLimits(const ID_T &componentId, double &FLOYIG, double &FHIYIG) const {
    static const string context("FrontEndDatabase::getWCAYIGLimits");

    FLOYIG = FHIYIG = 0.0;

    if (!isConnected())
        return false;

    if (!componentId.valid())
        return false;

    string query = "SELECT keyId, FloYIG, FhiYIG FROM WCAs WHERE " + componentId.whereClause("fkFacility", "fkFE_Component") + ";";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (numRows <= 0) {
        LOG(LM_ERROR) << context << ": error or no matching records." << endl;
        return false;
    }

    if (numRows > 1) {
        LOG(LM_ERROR) << context << ": too many rows." << endl;
    }

    MYSQL_ROW row = res.fetch_row();
    if (row != NULL) {
        FLOYIG = FIELD_TO_DOUBLE(1);
        FHIYIG = FIELD_TO_DOUBLE(2);
        return true;
    }
    return false;
}


bool FrontEndDatabase::getWCALOParams(const ID_T &componentId, FEConfig::PowerAmpParams &target) const {
    static const string context("FrontEndDatabase::getWCALOParams");

    target.clear();

    if (!isConnected())
        return false;

    if (!componentId.valid())
        return false;

    string query = "SELECT FreqLO, VDP0, VDP1, VGP0, VGP1 FROM WCA_LOParams WHERE " + componentId.whereClause("fkFacility", "fkComponent")
                 + " ORDER BY FreqLO;";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (numRows <= 0) {
        LOG(LM_ERROR) << context << ": error or no matching records." << endl;
        return false;
    }

    MYSQL_ROW row = NULL;
    double freqLO;
    float VD0, VD1, VG0, VG1;

    do {
        freqLO = 0.0;
        VD0 = VD1 = VG0 = VG1 = 0.0;

        row = res.fetch_row();
        if (row != NULL) {
            freqLO = FIELD_TO_DOUBLE(0);
            VD0 = FIELD_TO_SINGLE(1);
            VD1 = FIELD_TO_SINGLE(2);
            VG0 = FIELD_TO_SINGLE(3);
            VG1 = FIELD_TO_SINGLE(4);

            if (freqLO > 0.0) {
                target.set(freqLO, PowerAmpParams::VD0, VD0);
                target.set(freqLO, PowerAmpParams::VD1, VD1);
                target.set(freqLO, PowerAmpParams::VG0, VG0);
                target.set(freqLO, PowerAmpParams::VG1, VG1);
            }
        }
    } while (row != NULL);

    return true;
}

//-----------------------------------------------------------------------------
// PAS monitor data insert operations.   These all create one TestData_header record in addition to the monitor data record(s).


bool FrontEndDatabase::insertCryostatData(const ID_T &configId, DATASTATUS_TYPES dataStatus, const CryostatImpl::Cryostat_t &source) const {
    static const string context("FrontEndDatabase::insertCryostatData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigCryostat(configId, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigCryostat failed." << endl;

    TESTDATA_TYPES testDataType(TD_CRYOSTATTEMPS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, 0, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO CryostatTemps (fkFacility, fkHeader, "
                       "4k_CryoCooler, 4k_PlateLink1, 4k_PlateLink2, 4k_PlateFarSide1, "
                       "4k_PlateFarSide2, 15k_CryoCooler, 15k_PlateLink, 15k_PlateFarSide, 15k_Shield, "
                       "110k_CryoCooler, 110k_PlateLink, 110k_PlateFarSide, 110k_Shield) VALUES ("
                     + headerId.insertText() + ", "
                     + to_string(source.cryostatTemperature0_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature1_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature2_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature3_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature4_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature5_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature6_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature7_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature8_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature9_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature10_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature11_value, std::fixed, 2) + ", "
                     + to_string(source.cryostatTemperature12_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}

bool FrontEndDatabase::insertIfSwitchData(const ID_T &configId, DATASTATUS_TYPES dataStatus, const IFSwitchImpl::IFSwitch_t &source) const {
    static const string context("FrontEndDatabase::insertIfSwitchData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigIFSwitchAssy(configId, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigIFSwitchAssy failed." << endl;

    TESTDATA_TYPES testDataType(TD_IFSWITCHTEMPS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, 0, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO IFSwitchTemps (fkFacility, fkHeader, pol0sb1, pol0sb2, pol1sb1, pol1sb2) VALUES ("
                     + headerId.insertText() + ", "
                     + to_string(source.pol0Sb1AssemblyTemp_value, std::fixed, 2) + ", "
                     + to_string(source.pol0Sb2AssemblyTemp_value, std::fixed, 2) + ", "
                     + to_string(source.pol1Sb1AssemblyTemp_value, std::fixed, 2) + ", "
                     + to_string(source.pol1Sb2AssemblyTemp_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}

bool FrontEndDatabase::insertLPRMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus, const LPRImpl::LPR_t &source) const {
    static const string context("FrontEndDatabase::insertLPRMonitorData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigLPR(configId, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigLPR failed." << endl;

    TESTDATA_TYPES testDataType(TD_LPR_WARMHEALTH);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, 0, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO LPR_WarmHealth (fkFacility, fkHeader, "
                       "LaserPumpTemp, LaserDrive, LaserPhotodetector, Photodetector_mA, Photodetector_mW, ModInput, TempSensor0, TempSensor1) VALUES ("
                     + headerId.insertText() + ", "
                     + to_string(source.EDFALaserPumpTemperature_value, std::fixed, 2) + ", "
                     + to_string(source.EDFALaserDriveCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.EDFALaserPhotoDetectCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.EDFAPhotoDetectCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.EDFAPhotoDetectPower_value, std::fixed, 2) + ", "
                     + to_string(source.EDFAModulationInput_value, std::fixed, 2) + ", "
                     + to_string(source.LPRTemperature0_value, std::fixed, 2) + ", "
                     + to_string(source.LPRTemperature1_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}

bool FrontEndDatabase::findOrCreatePowerModuleDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus) const {
    static const string context("FrontEndDatabase::findOrCreatePowerModuleDataHeader");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigCPDSAssy(configId, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigCPDSAssy failed." << endl;

    // find an existing test data header for this data type and status:
    TESTDATA_TYPES testDataType(TD_CPDS_MONITOR);
    headerId = findTestDataHeader(configId, componentId, testDataType, dataStatus, 0);

    // if not found, create one:
    if (!headerId.valid()) {
        string notes = makeTestDataNotes(configId, testDataType, dataStatus);
        headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, 0, notes, measSWVer_m);
    }
    return headerId.valid();
}

bool FrontEndDatabase::insertPowerModuleData(const ID_T &headerId, unsigned band, double FreqLO, PowerModuleImpl::PowerModule_t &source) const {
    static const string context("FrontEndDatabase::insertPowerModuleData");

    if (!isConnected())
        return false;

    if (!headerId.valid())
        return false;

    string query = "INSERT INTO CPDS_monitor (fkFacility, fkHeader, Band, FreqLO, "
                   "P6V_V, N6V_V, P15V_V, N15V_V, P24V_V, P8V_V, P6V_I, N6V_I, P15V_I, N15V_I, P24V_I, P8V_I) VALUES ("
                 + headerId.insertText() + ", " + to_string(band) + ", "
                 + to_string(FreqLO, std::fixed, 3) + ", "
                 + to_string(source.voltageP6V_value, std::fixed, 2) + ", "
                 + to_string(source.voltageN6V_value, std::fixed, 2) + ", "
                 + to_string(source.voltageP15V_value, std::fixed, 2) + ", "
                 + to_string(source.voltageN15V_value, std::fixed, 2) + ", "
                 + to_string(source.voltageP24V_value, std::fixed, 2) + ", "
                 + to_string(source.voltageP8V_value, std::fixed, 2) + ", "
                 + to_string(source.currentP6V_value, std::fixed, 2) + ", "
                 + to_string(source.currentN6V_value, std::fixed, 2) + ", "
                 + to_string(source.currentP15V_value, std::fixed, 2) + ", "
                 + to_string(source.currentN15V_value, std::fixed, 2) + ", "
                 + to_string(source.currentP24V_value, std::fixed, 2) + ", "
                 + to_string(source.currentP8V_value, std::fixed, 2) + ")";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, 1);
}

bool FrontEndDatabase::findOrCreatePhotomixerDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus) const {
    static const string context("FrontEndDatabase::findOrCreatePhotomixerDataHeader");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    // find an existing test data header for this data type and status:
    TESTDATA_TYPES testDataType(TD_PHOTOMIXER_WARMHEALTH);
    headerId = findTestDataHeader(configId, FEICDataBase::null, testDataType, dataStatus, 0);

    // if not found, create one:
    if (!headerId.valid()) {
        string notes = makeTestDataNotes(configId, testDataType, dataStatus);
        headerId = createTestDataHeader(configId, FEICDataBase::null, testDataType, dataStatus, 0, notes, measSWVer_m);
    }
    return headerId.valid();
}


bool FrontEndDatabase::insertPhotomixerData(const ID_T &headerId, unsigned band, const WCAImpl::Photomixer_t &source) const {
    static const string context("FrontEndDatabase::insertPhotomixerData");

    if (!isConnected())
        return false;

    if (!headerId.valid())
        return false;

    string query = "INSERT INTO Photomixer_WarmHealth (fkFacility, fkHeader, Band, Vpmx, Ipmx) VALUES ("
                 + headerId.insertText() + ", " + to_string(band) + ", "
                 + to_string(source.photomixerVoltage_value, std::fixed, 2) + ", "
                 + to_string(source.photomixerCurrent_value, std::fixed, 2) + ")";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, 1);
}

bool FrontEndDatabase::insertPLLMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                                            unsigned band, double FreqLO, const WCAImpl::PLL_t &source) const
{
    static const string context("FrontEndDatabase::insertPLLMonitorData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigWCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigWCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_WCA_PLL_BIAS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO WCA_Misc_bias (fkFacility, fkHeader, Band, FreqLO, PLLtemp, YTO_heatercurrent) VALUES ("
                     + headerId.insertText() + ", " + to_string(band) + ", "
                     + to_string(FreqLO, std::fixed, 3) + ", "
                     + to_string(source.pllAssemblyTemp_value, std::fixed, 2) + ", "
                     + to_string(source.pllYTOHeaterCurrent_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}

bool FrontEndDatabase::findOrCreateFLOOGDistHealthDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus) const {
    static const string context("FrontEndDatabase::findOrCreateFLOOGDistHealthDataHeader");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigFLOOGDistModule(configId, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigFLOOGDistModule failed." << endl;

    // find an existing test data header for this data type and status:
    TESTDATA_TYPES testDataType(TD_FLOOGDIST);
    headerId = findTestDataHeader(configId, componentId, testDataType, dataStatus, 0);

    // if not found, create one:
    if (!headerId.valid()) {
        string notes = makeTestDataNotes(configId, testDataType, dataStatus);
        headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, 0, notes, measSWVer_m);
    }
    return headerId.valid();
}

bool FrontEndDatabase::insertFLOOGDistHealthData(const ID_T &headerId, unsigned band, float refTotalPower) const {
    static const string context("FrontEndDatabase::insertFLOOGDistHealthData");

    if (!isConnected())
        return false;

    if (!headerId.valid())
        return false;

    string query = "INSERT INTO FLOOGdist (fkFacility, fkHeader, Band, RefTotalPower) VALUES ("
                 + headerId.insertText() + ", " + to_string(band) + ", "
                 + to_string(refTotalPower, std::fixed, 2) + ")";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, 1);
}

bool FrontEndDatabase::insertAMCMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                                            unsigned band, double FreqLO, const WCAImpl::AMC_t &source) const
{
    static const string context("FrontEndDatabase::insertAMCMonitorData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigWCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigWCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_WCA_AMC_BIAS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO WCA_AMC_bias (fkFacility, fkHeader, Band, FreqLO, "
                       "VDA, VDB, VDE, IDA, IDB, IDE, VGA, VGB, VGE, MultD, MultD_Current, 5Vsupply) VALUES ("
                     + headerId.insertText() + ", " + to_string(band) + ", "
                     + to_string(FreqLO, std::fixed, 3) + ", "
                     + to_string(source.amcDrainAVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.amcDrainBVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.amcDrainEVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.amcDrainACurrent_value, std::fixed, 2) + ", "
                     + to_string(source.amcDrainBCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.amcDrainECurrent_value, std::fixed, 2) + ", "
                     + to_string(source.amcGateAVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.amcGateBVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.amcGateEVoltage_value, std::fixed, 2) + ", "
                     + to_string((int) source.amcMultiplierDVoltage_value) + ", "
                     + to_string(source.amcMultiplierDCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.amcSupplyVoltage5V_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}

bool FrontEndDatabase::insertPAMonitorData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                                           unsigned band, double FreqLO, const WCAImpl::PA_t &source) const
{
    static const string context("FrontEndDatabase::insertPAMonitorData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigWCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigWCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_WCA_PA_BIAS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO WCA_PA_bias (fkFacility, fkHeader, Band, FreqLO, "
                       "VDp0, VDp1, IDp0, IDp1, VGp0, VGp1, 3Vsupply, 5Vsupply ) VALUES ("
                     + headerId.insertText() + ", " + to_string(band) + ", "
                     + to_string(FreqLO, std::fixed, 3) + ", "
                     + to_string(source.paPol0DrainVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.paPol1DrainVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.paPol0DrainCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.paPol1DrainCurrent_value, std::fixed, 2) + ", "
                     + to_string(source.paPol0GateVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.paPol1GateVoltage_value, std::fixed, 2) + ", "
                     + to_string(source.paSupplyVoltage3V_value, std::fixed, 2) + ", "
                     + to_string(source.paSupplyVoltage5V_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}


bool FrontEndDatabase::createSISMonitorDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band) const {
    static const string context("FrontEndDatabase::createSISMonitorDataHeader");

    headerId = null;

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigCCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigCCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_CCA_SIS_BIAS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);
    return headerId.valid();
}

bool FrontEndDatabase::insertSISMonitorData(const ID_T &headerId, double FreqLO, unsigned pol, unsigned sb, const ColdCartImpl::SIS_t &source) const {
    static const string context("FrontEndDatabase::insertSISMonitorData");

    if (!isConnected())
        return false;

    if (!headerId.valid())
        return false;

    string query = "INSERT INTO CCA_SIS_bias (fkFacility, fkHeader, FreqLO, Pol, SB, VjRead, IjRead, VmagRead, ImagRead ) VALUES ("
                 + headerId.insertText() + ", "
                 + to_string(FreqLO, std::fixed, 3) + ", "
                 + to_string(pol) + ", " + to_string(sb) + ", "
                 + to_string(source.sisVoltage_value, std::fixed, 3) + ", "
                 + to_string(1000.0 * source.sisCurrent_value, std::fixed, 3) + ", "
                 + to_string(source.sisMagnetVoltage_value, std::fixed, 3) + ", "
                 + to_string(source.sisMagnetCurrent_value, std::fixed, 3) + ")";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, 1);
}

bool FrontEndDatabase::createLNAMonitorDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band) const {
    static const string context("FrontEndDatabase::createLNAMonitorDataHeader");

    headerId = null;

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigCCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigCCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_CCA_LNA_BIAS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);
    return headerId.valid();
}

bool FrontEndDatabase::insertLNAMonitorData(const ID_T &headerId, double FreqLO, unsigned pol, unsigned sb, const ColdCartImpl::LNA_t &source) const {
    static const string context("FrontEndDatabase::insertLNAMonitorData");

    if (!isConnected())
        return false;

    if (!headerId.valid())
        return false;

    string query = "INSERT INTO CCA_LNA_bias (fkFacility, fkHeader, FreqLO, Pol, SB, Stage, VdRead, IdRead, VgRead ) VALUES ("
                 + headerId.insertText() + ", "
                 + to_string(FreqLO, std::fixed, 3) + ", "
                 + to_string(pol) + ", " + to_string(sb) + ", "
                 + "1, "
                 + to_string(source.lnaSt1DrainVoltage_value, std::fixed, 2) + ", "
                 + to_string(source.lnaSt1DrainCurrent_value, std::fixed, 2) + ", "
                 + to_string(source.lnaSt1GateVoltage_value, std::fixed, 2) + ")";

    query       += ", (" + headerId.insertText() + ", "
                 + to_string(FreqLO, std::fixed, 3) + ", "
                 + to_string(pol) + ", " + to_string(sb) + ", "
                 + "2, "
                 + to_string(source.lnaSt2DrainVoltage_value, std::fixed, 2) + ", "
                 + to_string(source.lnaSt2DrainCurrent_value, std::fixed, 2) + ", "
                 + to_string(source.lnaSt2GateVoltage_value, std::fixed, 2) + ")";

    query       += ", (" + headerId.insertText() + ", "
                 + to_string(FreqLO, std::fixed, 3) + ", "
                 + to_string(pol) + ", " + to_string(sb) + ", "
                 + "3, "
                 + to_string(source.lnaSt3DrainVoltage_value, std::fixed, 2) + ", "
                 + to_string(source.lnaSt3DrainCurrent_value, std::fixed, 2) + ", "
                 + to_string(source.lnaSt3GateVoltage_value, std::fixed, 2) + ");";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, 3);
}

bool FrontEndDatabase::insertCartTempData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                                          unsigned band, const ColdCartImpl::CartridgeTemp_t &source) const
{
    static const string context("FrontEndDatabase::insertCartTempData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigCCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigCCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_CCA_TEMPSENSORS);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO CCA_TempSensors (fkFacility, fkHeader, 4k, 110k , Pol0_mixer, Spare, 15k, Pol1_mixer ) VALUES ("
                     + headerId.insertText() + ", "
                     + to_string(source.cartridgeTemperature0_value, std::fixed, 2) + ", "
                     + to_string(source.cartridgeTemperature1_value, std::fixed, 2) + ", "
                     + to_string(source.cartridgeTemperature2_value, std::fixed, 2) + ", "
                     + to_string(source.cartridgeTemperature3_value, std::fixed, 2) + ", "
                     + to_string(source.cartridgeTemperature4_value, std::fixed, 2) + ", "
                     + to_string(source.cartridgeTemperature5_value, std::fixed, 2) + ")";

        LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, 1);
    }
    return false;
}

bool FrontEndDatabase::insertIVCurveData(const ID_T &configId, DATASTATUS_TYPES dataStatus,
                                         unsigned band, double FreqLO, unsigned pol, unsigned sb,
                                         const std::string &legend, const XYPlotArray &source) const
{
    static const string context("FrontEndDatabase::insertIVCurveData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId;
    string componentSN;
    if (!getConfigCCA(configId, band, componentId, componentSN))
        LOG(LM_ERROR) << context << ": database getConfigCCA failed." << endl;

    TESTDATA_TYPES testDataType(TD_IV_CURVE);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus, legend);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (headerId.valid()) {
        string query = "INSERT INTO CCA_TEST_IVCurve (fkFacility, fkHeader, FreqLO, Pol, SB, VJ, IJ) VALUES ";
        for (XYPlotArray::const_iterator it = source.begin(); it != source.end(); ++it) {
            if (it != source.begin())
                query += ", ";
            query += "(" + headerId.insertText() + ", " + to_string(FreqLO, std::fixed, 3) + ", "
                   + to_string(pol) + ", " + to_string(sb) + ", "
                   + to_string((*it).Y1) + ", "
                   + to_string((*it).Y2) + ")";
        }
        LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        return checkQueryResult(context, numRows, source.size());
    }
    return false;
}

bool FrontEndDatabase::insertIFTotalPowerData(const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band,
                                              double FreqLO, const std::string &legend, const IFPowerDataSet &data) const
{
    static const string context("FrontEndDatabase::insertIFTotalPowerData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId(FEICDataBase::null);

    TESTDATA_TYPES testDataType(TD_IFTOTALPOWER);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus, legend);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (!headerId.valid()) {
        LOG(LM_ERROR) << context << ": error in createTestDataHeader." << endl;
        return false;
    }

    bool hasSB2 = ColdCartImpl::hasSb2(band);

    string query = "INSERT INTO IFTotalPower (fkFacility, fkHeader, Band, FreqLO, IFChannel, Power_0dB_gain, Power_15dB_gain) VALUES ";

    query += "(" + headerId.insertText() + ", " + to_string(band) + ", " + to_string(FreqLO, std::fixed, 3) + ", 0, "
           + to_string(data[IFPowerDataSet::PHOT0_IF0], std::fixed, 2) + ", " + to_string(data[IFPowerDataSet::PHOT15_IF0], std::fixed, 2) + "), ";
    query += "(" + headerId.insertText() + ", " + to_string(band) + ", " + to_string(FreqLO, std::fixed, 3) + ", 1, "
           + to_string(data[IFPowerDataSet::PHOT0_IF1], std::fixed, 2) + ", " + to_string(data[IFPowerDataSet::PHOT15_IF1], std::fixed, 2) + ")";

    if (!hasSB2)
        query += ";";

    else {
        query += ", ";

        query += "(" + headerId.insertText() + ", " + to_string(band) + ", " + to_string(FreqLO, std::fixed, 3) + ", 2, "
               + to_string(data[IFPowerDataSet::PHOT0_IF2], std::fixed, 2) + ", " + to_string(data[IFPowerDataSet::PHOT15_IF2], std::fixed, 2) + "), ";
        query += "(" + headerId.insertText() + ", " + to_string(band) + ", " + to_string(FreqLO, std::fixed, 3) + ", 3, "
               + to_string(data[IFPowerDataSet::PHOT0_IF3], std::fixed, 2) + ", " + to_string(data[IFPowerDataSet::PHOT15_IF3], std::fixed, 2) + ");";
    }
    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, (hasSB2 ? 4 : 2));
}

bool FrontEndDatabase::insertYFactorData(const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band,
                                         double FreqLO, const std::string &legend, const IFPowerDataSet &data) const
{
    static const string context("FrontEndDatabase::insertYFactorData");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }

    ID_T componentId(FEICDataBase::null);

    TESTDATA_TYPES testDataType(TD_YFACTOR);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus, legend);
    ID_T headerId = createTestDataHeader(configId, componentId, testDataType, dataStatus, band, notes, measSWVer_m);

    if (!headerId.valid()) {
         LOG(LM_ERROR) << context << ": error in createTestDataHeader." << endl;
         return false;
    }

    float Phot0 = (dataStatus == DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF0] : data[IFPowerDataSet::PHOT15_IF0];
    float Phot1 = (dataStatus == DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF1] : data[IFPowerDataSet::PHOT15_IF1];
    float Phot2 = (dataStatus == DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF2] : data[IFPowerDataSet::PHOT15_IF2];
    float Phot3 = (dataStatus == DS_COLD_PAS) ? data[IFPowerDataSet::PHOT0_IF3] : data[IFPowerDataSet::PHOT15_IF3];

    bool hasSB2 = ColdCartImpl::hasSb2(band);

    string query = "INSERT INTO Yfactor (fkFacility, fkHeader, FreqLO, IFChannel, Phot_dBm, Pcold_dBm, Y) VALUES ";

    query += "(" + headerId.insertText() + ", " + to_string(FreqLO, std::fixed, 3) + ", 0, "
           + to_string(Phot0, std::fixed, 2) + ", "
           + to_string(data[IFPowerDataSet::PCOLD_IF0], std::fixed, 2) + ", "
           + to_string(data[IFPowerDataSet::YFACTOR_IF0], std::fixed, 2) + "), ";

    query += "(" + headerId.insertText() + ", " + to_string(FreqLO, std::fixed, 3) + ", 1, "
           + to_string(Phot1, std::fixed, 2) + ", "
           + to_string(data[IFPowerDataSet::PCOLD_IF1], std::fixed, 2) + ", "
           + to_string(data[IFPowerDataSet::YFACTOR_IF1], std::fixed, 2) + ")";

    if (!hasSB2)
        query += ";";

    else {
        query += ", ";

        query += "(" + headerId.insertText() + ", " + to_string(FreqLO, std::fixed, 3) + ", 2, "
               + to_string(Phot2, std::fixed, 2) + ", "
               + to_string(data[IFPowerDataSet::PCOLD_IF2], std::fixed, 2) + ", "
               + to_string(data[IFPowerDataSet::YFACTOR_IF2], std::fixed, 2) + "), ";

        query += "(" + headerId.insertText() + ", " + to_string(FreqLO, std::fixed, 3) + ", 3, "
               + to_string(Phot3, std::fixed, 2) + ", "
               + to_string(data[IFPowerDataSet::PCOLD_IF3], std::fixed, 2) + ", "
               + to_string(data[IFPowerDataSet::YFACTOR_IF3], std::fixed, 2) + ");";
    }

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, (hasSB2 ? 4 : 2));
}


bool FrontEndDatabase::createFineLOSweepDataHeader(ID_T &headerId, const ID_T &configId, DATASTATUS_TYPES dataStatus, unsigned band, const std::string &legend) const {
    static const string context("FrontEndDatabase::createFineLOSweepDataHeader");

    if (!isConnected())
        return false;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return false;
    }
    TESTDATA_TYPES testDataType(TD_FINE_LO_SWEEP);
    string notes = makeTestDataNotes(configId, testDataType, dataStatus, legend);
    headerId = createTestDataHeader(configId, FEICDataBase::null, testDataType, dataStatus, band, notes, measSWVer_m);
    return headerId.valid();
}

bool FrontEndDatabase::insertFineLOSweepSubHeader(ID_T &subHeaderId, const ID_T &headerId, unsigned band, unsigned pol,
                                                  float tiltAngle, double startLOFreq, double stopLOFreq, double stepLOFreq,
                                                  float SISVoltage, float targetSIS1Current, const std::string &legend) const
{
    static const string context("FrontEndDatabase::insertFineLOSweepSubHeader");

    if (!isConnected())
        return false;

    if (!headerId.valid())
        return false;

    string tmp;

    string query = "INSERT INTO TEST_FineLOSweep_SubHeader (keyFacility, fkHeader, "
                   "Band, Pol, TiltAngle_Deg, StartLOFreq, StopLOFreq, StepLOFreq, SISVoltage, TargetSIS1Current, Legend ) VALUES ("
                 + headerId.insertText() + ", "
                 + to_string(band) + ", "
                 + to_string(pol) + ", "
                 + to_string(tiltAngle, std::fixed, 2) + ", "
                 + to_string(startLOFreq, std::fixed, 3) + ", "
                 + to_string(stopLOFreq, std::fixed, 3) + ", "
                 + to_string(stepLOFreq, std::fixed, 3) + ", "
                 + to_string(SISVoltage, std::fixed, 2) + ", "
                 + to_string(targetSIS1Current, std::fixed, 2) + ", "
                 + "'" + conn_mp -> escapeString(legend, tmp) + "' );";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1)) {
        LOG(LM_ERROR) << context << ": returned numRows=" << numRows << " when expecting 1." << endl;
        return false;
    }

    subHeaderId = ID_T(headerId.keyFacility, conn_mp -> getLastInsertID());
    return subHeaderId.valid();
}

bool FrontEndDatabase::insertFineLOSSweepData(const ID_T &subHeaderId, const XYPlotArray &sisCurrents, const XYPlotArray &loPaVoltages) const {
    static const string context("FrontEndDatabase::insertFineLOSSweepData");

    if (!isConnected())
        return false;

    if (!subHeaderId.valid())
        return false;

    if (!sisCurrents.size() == loPaVoltages.size()) {
        LOG(LM_ERROR) << context << ": sisCurrents and loPaVoltages are not the same size." << endl;
        return false;
    }

    string query = "INSERT INTO TEST_FineLOSweep ( fkFacility, fkSubHeader, FreqLO, SIS1Current, SIS2Current, LOPADrainSetting, LOPADrainVMonitor ) VALUES ";

    XYPlotArray::const_iterator IJit, VDit;

    for (IJit = sisCurrents.begin(), VDit = loPaVoltages.begin(); IJit != sisCurrents.end(); ++IJit, ++VDit) {

        if (IJit != sisCurrents.begin())
            query += ", ";

        query += "(" + subHeaderId.insertText() + ", "
               + to_string((*IJit).X, std::fixed, 3) + ", "
               + to_string((*IJit).Y1, std::fixed, 2) + ", "
               + to_string((*IJit).Y2, std::fixed, 2) + ", "
               + to_string((*VDit).Y1, std::fixed, 2) + ", "
               + to_string((*VDit).Y2, std::fixed, 2) + ")";
    }

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, sisCurrents.size());
}

