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
#include "MySQLConnection.h"
#include "iniFile.h"
#include "logger.h"
#include "stringConvert.h"
using namespace std;

//-----------------------------------------------------------------------------

// special ID constants:
const FEICDataBase::ID_T FEICDataBase::unknown(0, 0);
const FEICDataBase::ID_T FEICDataBase::null(0, -1);
const FEICDataBase::ID_T FEICDataBase::notConnected(0, -2);
const FEICDataBase::ID_T FEICDataBase::badResult(0, -3);

//-----------------------------------------------------------------------------

FEICDataBase::ID_T::ID_T(const std::string &facility, const std::string &id)
  : keyFacility(from_string<unsigned>(facility)),
    keyId(from_string<long>(id))
    {}

void FEICDataBase::ID_T::setFacilityString(const std::string &s) {
    keyFacility = from_string<unsigned>(s);
}

void FEICDataBase::ID_T::setKeyIdString(const std::string &s) {
    keyId = from_string<long>(s);
}

const std::string FEICDataBase::ID_T::facilityString() const {
    return to_string(keyFacility);
}

const std::string FEICDataBase::ID_T::keyIdString() const {
    return to_string(keyId);
}

const std::string FEICDataBase::ID_T::insertText() const {
    string result(to_string(keyFacility));
    result += ", ";
    result += to_string(keyId);
    return result;
}

const std::string FEICDataBase::ID_T::whereClause(const std::string &facilityColumn, const std::string &idColumn) const {
    string result("(");
    result += idColumn;
    result += "=";
    result += to_string(keyId);
    result += " AND ";
    result += facilityColumn;
    result += "=";
    result += to_string(keyFacility);
    result += ")";
    return result;
}

const std::string FEICDataBase::ID_T::print() const {
    string result("(");

    if (valid()) {
        result += to_string(keyFacility) + ",";
        result += to_string(keyId);

    } else if (*this == null)
        result += "NULL";

    else if (*this  == notConnected)
        result += "NOCONNECT";

    else if (*this == badResult)
        result += "BADRESULT";

    else {
        result += to_string(keyFacility) + ",";
        result += to_string(keyId);
    }
    result += ")";
    return result;
}

FEICDataBase::ID_T unpackID(const MYSQL_ROW &row) {
    if (row != NULL) {
        if (row[0] && row[1])
            return FEICDataBase::ID_T(row[0], row[1]);
    }
    return FEICDataBase::null;
}
///< helper to extract an ID_T from the first two columns of a row.

//-----------------------------------------------------------------------------

FEICDataBase::FEICDataBase()
  : conn_mp(NULL)
{
    string host, user, password, dbName;
    getEnvConnectionInfo(host, user, password, dbName);
    conn_mp = new MySQLConnection(host, user, password, dbName);
}

FEICDataBase::FEICDataBase(const std::string &host,
                           const std::string &user,
                           const std::string &password,
                           const std::string &dbName)
  : conn_mp(new MySQLConnection(host, user, password, dbName))
  {}

FEICDataBase::~FEICDataBase() {
    conn_mp -> disconnect();
    delete conn_mp;
}

bool FEICDataBase::isConnected() const {
    return conn_mp -> isConnected();
}

unsigned FEICDataBase::getDefaultFacility() const {
    static const string context("FEICDataBase::getDefaultFacility");

    if (!isConnected())
        return 0;

    string query = "SELECT DefaultFacility FROM DatabaseDefaults;";

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (checkQueryResult(context, numRows, 1)) {
        MYSQL_ROW row = res.fetch_row();
        if (row != NULL && row[0] != NULL)
            return from_string<unsigned>(row[0]);
    }
    return 0;
}

//-----------------------------------------------------------------------------

string FEICDataBase::getDataTypeDescription(int testDataType) const {
    ///< lookup the description for the specified data type.
    static const string context("FEICDataBase::getDataTypeDescription");
    string description;

    if (isConnected()) {
        string query = "SELECT Description FROM TestData_Types WHERE keyId=" + to_string(testDataType) + ";";

        LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        if (checkQueryResult(context, numRows, 1)) {
            MYSQL_ROW row = res.fetch_row();
            description = row[0];
        }
    }
    return description;
}

string FEICDataBase::getDataStatusDescription(DATASTATUS_TYPES dataStatus) const {
    ///< lookup the description for the specified data type.
    static const string context("FEICDataBase::getDataStatusDescription");
    string description;

    if (isConnected()) {
        string query = "SELECT Description FROM DataStatus WHERE keyId=" + to_string(dataStatus) + ";";

        LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

        MySQLConnection::Result res;
        int numRows = conn_mp -> executeQuery(query, &res);

        if (checkQueryResult(context, numRows, 1)) {
            MYSQL_ROW row = res.fetch_row();
            description = row[0];
        }
    }
    return description;
}

//-----------------------------------------------------------------------------

FEICDataBase::ID_T FEICDataBase::getFrontEndId(unsigned facility, unsigned serialNum) const {
    ///< lookup the record ID for the given front end serial number at the specified facility.   If facility is <=0, tries to return a single match on serialNum.
    static const string context("FEICDataBase::getFrontEndId");

    if (!isConnected())
        return notConnected;

    string query = "SELECT keyFacility, keyFrontEnds FROM Front_Ends WHERE ";

    if (facility > 0)
        query += "keyFacility=" + to_string(facility) + " AND ";

    query += "SN=" + to_string(serialNum) + " ORDER BY keyFacility, keyFrontEnds;";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    // if facility was specified we should have exactly 0 or 1 row:
    if (facility > 0) {
        if (!checkQueryResult(context, numRows, 1))
            return (numRows == 0) ? null : badResult;
    }

    MYSQL_ROW row = res.fetch_row();
    ID_T ret = unpackID(row);

    string tmp;
    LOG(LM_INFO) << context << ": row=(" << conn_mp -> rowToString(res, row, tmp) << ") ID=" << ret << endl;
    return ret;
}

bool FEICDataBase::getFrontEndList(unsigned facility, IDList_T &target) const {
    static const string context("FEICDataBase::getFrontEndList");

    target.clear();

    if (!isConnected())
        return false;

    string query = "SELECT keyFacility, keyFrontEnds, SN FROM Front_Ends";

    if (facility > 0)
        query += " WHERE keyFacility=" + to_string(facility);

    query += " ORDER BY SN;";

    LOG(LM_INFO) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (numRows <= 0) {
        LOG(LM_ERROR) << context << ": error or no matching records." << endl;
        return false;
    }

    MYSQL_ROW row = NULL;
    do {
        row = res.fetch_row();
        if (row != NULL) {
            ID_T id = unpackID(row);
            string SN = row[2];
            // ignore fake and warm testing SNs above 99:
            if (from_string<unsigned>(SN) < 100) {
                // pad to two chars:
                if (SN.length() == 1)
                    SN = "0" + SN;
                target[SN] = id;
                LOG(LM_INFO) << context << ": found " << SN << " -> " << id << endl;
            }
        }
    } while (row != NULL);

    return true;
}

bool FEICDataBase::isValidFrontEndId(const ID_T &frontEndId) const {
    ///< returns true if no error and the given front end ID exists;
    static const string context("FEICDataBase::isValidFrontEndId");

    if (!frontEndId.valid())
        return false;

    if (!isConnected())
        return false;

    string query = "SELECT keyFacility, keyFrontEnds FROM Front_Ends WHERE " + frontEndId.whereClause("keyFacility", "keyFrontEnds") + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1))
        return false;

    MYSQL_ROW row = res.fetch_row();
    ID_T ret = unpackID(row);

    if (ret != frontEndId) {
        LOG(LM_ERROR) << context << ": got a non-matching frontEnd ID." << endl;
        return false;
    }
    return true;
}

bool FEICDataBase::getFrontEndRecord(const ID_T &frontEndId, std::string *SN, std::string *ESN, std::string *docs, std::string *description) const {
    ///< lookup the whole front end record for the given serial number at the specified facility.   If facility is <=0, tries to return a single match on serialNum.
    static const string context("FEICDataBase::getFrontEndRecord");

    if (SN)
        SN -> clear();
    if (ESN)
        ESN -> clear();
    if (docs)
        docs -> clear();
    if (description)
        description -> clear();

    if (!isConnected())
        return false;

    if (!frontEndId.valid()) {
        LOG(LM_ERROR) << context << ": given frontEndId is not valid." << endl;
        return false;
    }

    string query = "SELECT TS, SN, ESN, Docs, Description FROM Front_Ends WHERE " + frontEndId.whereClause("keyFacility", "keyFrontEnds") + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1)) {
        LOG(LM_ERROR) << context << ": expected numRows=1 and got numRows=" << numRows << endl;
        return false;
    }

    MYSQL_ROW row = res.fetch_row();
    if (row == NULL) {
        LOG(LM_ERROR) << context << ": result row is null." << endl;
        return false;
    }

    string tmp;
    LOG(LM_INFO) << context << ": row=(" << conn_mp -> rowToString(res, row, tmp) << ")" << endl;

    if (SN)
        MySQLConnection::unpackString(row, 1, *SN);
    if (ESN)
        MySQLConnection::unpackString(row, 2, *ESN);
    if (docs)
        MySQLConnection::unpackString(row, 3, *docs);
    if (description)
        MySQLConnection::unpackString(row, 4, *description);

    return true;
}

FEICDataBase::ID_T FEICDataBase::getConfigId(unsigned facility, unsigned serialNum) const {
    ///< lookup the latest configuration ID for the given front end serialNum at the specified facility.   If facility is <=0, tries to return a single match on serialNum.
    static const string context("FEICDataBase::getConfigId(facility, SN)");

    if (!isConnected())
        return notConnected;

    ID_T frontEndId = getFrontEndId(facility, serialNum);
    if (!frontEndId.valid())
        return frontEndId;

    return getConfigId(frontEndId);
}

FEICDataBase::ID_T FEICDataBase::getConfigId(const ID_T &frontEndId) const {
    ///< lookup the latest configuration ID for the given front end Id.
    static const string context("FEICDataBase::getConfigId(frontEndId)");

    if (!isConnected())
        return notConnected;

    if (!frontEndId.valid())
        return null;

    string query = "SELECT MAX(keyFEConfig) FROM FE_Config WHERE " + frontEndId.whereClause("keyFacility", "fkFront_Ends") + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1))
        return (numRows == 0) ? null : badResult;

    MYSQL_ROW row = res.fetch_row();
    ID_T ret(frontEndId.keyFacility, 0);
    ret.setKeyIdString(row[0]);

    string tmp;
    LOG(LM_INFO) << context << ": row=(" << conn_mp -> rowToString(res, row, tmp) << ") ID=" << ret << endl;
    return ret;
}

bool FEICDataBase::isValidConfigId(const ID_T &configId) const {
    ///< returns true if no error and the given config ID exists;
    static const string context("FEICDataBase::isValidConfigId");

    if (!isConnected())
        return false;

    string query = "SELECT keyFacility, keyFEConfig FROM FE_Config WHERE " + configId.whereClause("keyFacility", "keyFEConfig") + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1))
        return false;

    MYSQL_ROW row = res.fetch_row();
    ID_T ret = unpackID(row);

    if (ret != configId) {
        LOG(LM_ERROR) << context << ": got a non-matching config ID." << endl;
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------------------------------------------

class MySQLConnection_Result : public  MySQLConnection::Result {
    // alias class used to prevent including MySQLConnection in the header for this class.
};

bool FEICDataBase::getConfigComponentListImpl(const ID_T &configId, MySQLConnection_Result &res, int componentTypeFilter, int bandFilter) const {
// private helper for getConfigComponentList and getConfigComponent.

    static const string context("FEICDataBase::getConfigComponentListImpl");

    if (!isConnected())
        return false;

    if (!configId.valid())
        return false;

    string query = "SELECT T2.keyFacility, T2.keyId, T2.Band, T2.SN FROM FE_ConfigLink as T1 LEFT JOIN FE_Components AS T2"
                   " ON T2.keyId = T1.fkFE_Components AND T2.keyFacility = T1.fkFE_ComponentFacility WHERE ";

    if (componentTypeFilter > 0)
        query += "T2.fkFE_ComponentType=" + to_string(componentTypeFilter) + " AND ";

    if (bandFilter > 0)
        query += "T2.Band=" + to_string(bandFilter) + " AND ";

    query += configId.whereClause("T1.fkFE_ConfigFacility", "T1.fkFE_Config") + " ORDER BY T2.keyId DESC;";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;


    int numRows = conn_mp -> executeQuery(query, &res);

    if (numRows < 0) {
        LOG(LM_ERROR) << context << ": error." << endl;
        return false;
    }

    return (numRows > 0);
}

bool FEICDataBase::getConfigComponentList(const ID_T &configId, IDList_T &target, int componentTypeFilter, int bandFilter) const {

    ///< look up and return a list of IDs of components of the requested componentType.
    static const string context("FEICDataBase::getConfigComponentList");

    target.clear();
    MySQLConnection_Result res;

    if (!getConfigComponentListImpl(configId, res, componentTypeFilter, bandFilter))
        return false;

    MYSQL_ROW row = NULL;
    string bandStr, SN;
    unsigned band;
    do {
        row = res.fetch_row();
        if (row != NULL) {
            ID_T id = unpackID(row);
            MySQLConnection::unpackString(row, 2, bandStr);
            MySQLConnection::unpackString(row, 3, SN);
            band = from_string<unsigned>(bandStr);
            if (band > 0)
                SN = bandStr + ":" + SN;
            target[SN] = id;
            LOG(LM_INFO) << context << ": found " << SN << " -> " << id << endl;
        }
    } while (row != NULL);

    return true;
}

bool FEICDataBase::getConfigComponent(const ID_T &configId, ID_T &componentId, std::string &SN, int componentTypeFilter, int bandFilter) const {
    ///< get a single component which is part of the specified configuration.
    static const string context("FEICDataBase::getConfigComponent");

    componentId.reset();
    SN.clear();

    MySQLConnection_Result res;

    if (!getConfigComponentListImpl(configId, res, componentTypeFilter, bandFilter))
        return false;

    MYSQL_ROW row = res.fetch_row();
    if (row == NULL) {
        LOG(LM_ERROR) << context << ": error got NULL when expecting at least one row." << endl;
        return false;
    }
    componentId = unpackID(row);
    string bandStr;
    MySQLConnection::unpackString(row, 2, bandStr);
    MySQLConnection::unpackString(row, 3, SN);
    LOG(LM_INFO) << context << ": found band=" << bandStr << " SN=" << SN << " -> " << componentId << endl;
    return true;
}

bool FEICDataBase::getComponentRecord(const ID_T &componentID, FEComponent &target) {
    ///< look up the contents of the requested component record.
    static const string context("FEICDataBase::getComponentRecord");

    target.reset();

    if (!isConnected())
        return false;

    if (!componentID.valid())
        return false;

    string query = "SELECT fkFE_ComponentType, Band, SN, ESN1, ESN2, Description FROM FE_Components WHERE " + componentID.whereClause("keyFacility", "keyId") + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);
    if (!checkQueryResult(context, numRows, 1))
        return false;

    MYSQL_ROW row = res.fetch_row();

    string tmp;
    MySQLConnection::unpackString(row, 0, tmp);
    target.type_m = from_string<int>(tmp);

    MySQLConnection::unpackString(row, 1, tmp);
    target.band_m = from_string<unsigned>(tmp);

    MySQLConnection::unpackString(row, 2, target.SN_m);

    MySQLConnection::unpackString(row, 3, target.ESN1_m);

    MySQLConnection::unpackString(row, 4, target.ESN2_m);

    MySQLConnection::unpackString(row, 5, target.desc_m);
    return true;
}


//-------------------------------------------------------------------------------------------------------------------------------

bool FEICDataBase::updateFrontEndESN(const ID_T &frontEndId, const std::string &ESN) const {
    ///< update the ESN field of the specified front end.
    static const string context("FEICDataBase::updateFrontEndESN");

    if (!isValidFrontEndId(frontEndId)) {
        LOG(LM_ERROR) << context << " ERROR: frontEndId is not valid.: " << frontEndId.print() << endl;
        return false;
    }

    string query = "UPDATE Front_Ends SET ESN='" + ESN + "' WHERE " + frontEndId.whereClause("keyFacility", "keyFrontEnds") + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    return checkQueryResult(context, numRows, 1);
}

//-------------------------------------------------------------------------------------------------------------------------------

FEICDataBase::ID_T FEICDataBase::createTestDataHeader(const ID_T &configId, const ID_T &componentId, int testDataType, DATASTATUS_TYPES dataStatus, unsigned band,
                                                      const std::string &notes, const std::string &measSWVer) const
{
    ///< create a test data header record and return its ID.
    static const string context("FEICDataBase::createTestDataHeader");

    if (!isConnected())
        return notConnected;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << " ERROR: given configId is not valid." << endl;
        return badResult;
    }

    string tmp, tmp2;
    string query = "INSERT INTO TestData_header (keyFacility, fkTestData_Type, fkFE_Config, fkFE_Components, fkDataStatus, Band, Notes, Meas_SWVer) VALUES ("
                 + configId.facilityString() + ", "
                 + to_string(testDataType) + ", "
                 + to_string(configId.keyIdString()) + ", "
                 + (componentId.valid() ? componentId.keyIdString() : "NULL") + ", "
                 + to_string(dataStatus) + ", "
                 + to_string(band) + ", "
                 + "\"" + conn_mp -> escapeString(notes, tmp) + "\", "
                 + "\"" + conn_mp -> escapeString(measSWVer, tmp2) + "\");";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1))
        return badResult;

    ID_T ret(configId.keyFacility, conn_mp -> getLastInsertID());

    LOG(LM_INFO) << context << ": lastId=" << ret << endl;
    return ret;
}

FEICDataBase::ID_T FEICDataBase::findTestDataHeader(const ID_T &configId, const ID_T &componentId, int testDataType, DATASTATUS_TYPES dataStatus, unsigned band) const {
    ///< find the latest test data header record for the given config, type, status, and band.
    static const string context("FEICDataBase::findTestDataHeader");

    if (!isConnected())
        return notConnected;

    if (!isValidConfigId(configId)) {
        LOG(LM_ERROR) << context << ": given configId is not valid." << endl;
        return badResult;
    }

    string query = "SELECT MAX(keyId) FROM TestData_header WHERE " + configId.whereClause("keyFacility", "fkFE_Config");

    if (componentId.valid())
        query += " AND fkFE_Components=" + componentId.keyIdString();

    if (band >= 1 && band <= 10)
        query += " AND Band = " + to_string(band);

    query += " AND fkTestData_Type=" + to_string(testDataType)
           + " AND fkDataStatus=" + to_string(dataStatus) + ";";

    LOG(LM_DEBUG) << context << ": query='" << query << "'" << endl;

    MySQLConnection::Result res;
    int numRows = conn_mp -> executeQuery(query, &res);

    if (!checkQueryResult(context, numRows, 1))
        return (numRows == 0) ? null : badResult;

    ID_T ret = null;
    MYSQL_ROW row = res.fetch_row();
    if (row != NULL && row[0] != NULL) {
        ret.keyFacility = configId.keyFacility;
        ret.setKeyIdString(row[0]);
    }

    string tmp;
    LOG(LM_INFO) << context << ": row=(" << conn_mp -> rowToString(res, row, tmp) << ") ID=" << ret << endl;

    if (ret.valid())
        return ret;
    else
        return null;
}

//-------------------------------------------------------------------------------------------------------------------------------
// protected:

bool FEICDataBase::checkQueryResult(const std::string &context, int numRows, unsigned expectedRows) {
    ///< helper to check whether a query error occurred and the right number of rows returned.
    if (numRows < 0) {
        LOG(LM_ERROR) << context << ": ERROR" << endl;
        return false;
    }

    if ((unsigned) numRows == expectedRows) {
        LOG(LM_DEBUG) << context << ": returned " << to_string(numRows) << " rows." << endl;
        return true;

    } else {
        LOG(LM_ERROR) << context << " ERROR: expected " << expectedRows << " but got " << numRows << " rows." << endl;
        return false;
    }
}

string FEICDataBase::makeTestDataNotes(const ID_T &configId, int testDataType, DATASTATUS_TYPES dataStatus, const std::string &legend) const {
    string notes = "[";
    notes += getDataStatusDescription(dataStatus);
    notes += "] " + getDataTypeDescription(testDataType);
    if (!legend.empty())
        notes += " " + legend;
    notes += " for FEConfig" + configId.print();
    return notes;
}

//-------------------------------------------------------------------------------------------------------------------------------
// private:

bool FEICDataBase::getEnvConnectionInfo(std::string &host, std::string &user, std::string &password, std::string &dbName) {
    static const string context("FEICDataBase::getEnvConnectionInfo");
    string path("L:\\data");

    char *val = getenv("TS_CONFIG");
    if (val) {
        path = val;
    } else {
        LOG(LM_INFO) << context << ": Environment variable TS_CONFIG not found.  Assuming" << escape_string(path) << endl;
    }

    if (!path.empty()) {
        if (path[path.length() - 1] != '\\')
            path += "\\";
    }
    path += "mysql.ini";
    LOG(LM_INFO) << context << ": path=" << escape_string(path) << endl;

    bool ret = false;
    try {
        CIniFile configINI(path);
        if (configINI.ReadFile()) {
            host = configINI.GetValue("settings", "server");
            user = configINI.GetValue("settings", "uid");
            password = configINI.GetValue("settings", "pwd");
            dbName = configINI.GetValue("settings", "database");
            LOG(LM_INFO) << context << ": host=" << host << " user=" << user << " dbName=" << dbName << endl;
            ret = true;
        }
    } catch (...) {
        ret = false;
    }
    if (!ret) {
        LOG(LM_ERROR) << context << ": ERROR loading configuration file." << endl;
    }
    return ret;
}

