#ifndef FEICDATABASE_H_
#define FEICDATABASE_H_
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

#include <string>
#include <map>
class MySQLConnection;
class MySQLConnection_Result;

class FEICDataBase {
public:
    FEICDataBase();

    FEICDataBase(const std::string &host,
                 const std::string &user,
                 const std::string &password,
                 const std::string &dbName);

    virtual ~FEICDataBase();

    bool isConnected() const;
    ///< check whether successfully/still connected to the database server.

    /// structure for returning record IDs which are compound keys.
    struct ID_T {
        unsigned keyFacility;
        long keyId;

        ID_T(unsigned facility = 0, long id = 0)
          : keyFacility(facility),
            keyId(id)
            {}
        /// construct from components, default construct

        ID_T(const std::string &facility, const std::string &id);
        ///< construct from strings

        void reset()
          { keyFacility = 0; keyId = 0; }

        bool operator ==(const ID_T &other) const
          { return (keyId == other.keyId && keyFacility == other.keyFacility); }
        ///< equivalence test

        bool operator !=(const ID_T &other) const
          { return !(operator ==(other)); }
        ///< not equivalent test

        bool valid() const
          { return (keyId > 0 && keyFacility > 0); }
        ///< validity check for ID_T

        void setFacilityString(const std::string &s);
        ///< assign facility portion from a string.

        void setKeyIdString(const std::string &s);
        ///< assign Id portion from a string.

        const std::string facilityString() const;
        ///< get the facility portion as a string.

        const std::string keyIdString() const;
        ///< get the Id portion as a string.

        const std::string whereClause(const std::string &facilityColumn, const std::string &idColumn) const;
        ///< produce an SQL where clause from this ID using the given column name strings.

        const std::string insertText() const;
        ///< print as comma-separated values suitable for INSERT.

        const std::string print() const;
        ///< print an ID_T in a user-friendly format.
    };

    // special ID constants:
    static const ID_T unknown;          ///< uninitialized ID
    static const ID_T null;             ///< return indicating blank or not found
    static const ID_T notConnected;     ///< returned if server connection is bad
    static const ID_T badResult;        ///< returned if results from server are not as expected (e.g. too many rows.)

//-----------------------------------------------------------------------------

    class IDList_T : public std::map<std::string, ID_T> {};

//-----------------------------------------------------------------------------

    /// Data status types we know about:
    enum DATASTATUS_TYPES {
        DS_UNKOWN               = 0,
        DS_COLD_PAS             = 1,
        DS_WARM_PAS             = 2,
        DS_COLD_PAI             = 3,
        DS_HEALTH_CHECK         = 4,
        DS_CARTRIDGE_PAI        = 7,
        DS_REJECTED             = 6
    };

    std::string getDataStatusDescription(DATASTATUS_TYPES dataStatus) const;
    ///< lookup the description for the specified data type.

    std::string getDataTypeDescription(int testDataType) const;
    ///< lookup the description for the specified data type.

//-----------------------------------------------------------------------------
// Look up front end and configuration operations:

    ID_T getFrontEndId(unsigned facility, unsigned serialNum) const;
    ///< lookup the record ID for the given front end serial number at the specified facility.
    ///< If facility is <=0, tries to return a single match on serialNum.

    bool getFrontEndList(unsigned facility, IDList_T &target) const;
    ///< get a list of the SN and ID of all front ends at the specified facility.  If facility <= 0, return all front ends.

    bool isValidFrontEndId(const ID_T &frontEndId) const;
    ///< returns true if no error and the given front end ID exists;

    bool getFrontEndRecord(const ID_T &frontEndId, std::string *SN = NULL, std::string *ESN = NULL, std::string *docs = NULL, std::string *description = NULL) const;
    ///< lookup the whole front end record for the given front end ID.
    ///< place TS, ESN, docs, and description in the provided locations if not NULL.

    ID_T getConfigId(unsigned facility, unsigned serialNum) const;
    ///< lookup the latest configuration ID for the given front end serialNum at the specified facility.
    ///< If facility is <=0, tries to return a single match on serialNum.

    ID_T getConfigId(const ID_T &frontEndId) const;
    ///< lookup the latest configuration ID for the given front end Id.

    bool isValidConfigId(const ID_T &configId) const;
    ///< returns true if no error and the given config ID exists.

//-----------------------------------------------------------------------------
// Lookup components operations:

    struct FEComponent {
        int type_m;
        unsigned band_m;
        std::string SN_m;
        std::string ESN1_m;
        std::string ESN2_m;
        std::string desc_m;

        FEComponent(int type = 0,
                    unsigned band = 0,
                    std::string SN = "",
                    std::string ESN1 = "",
                    std::string ESN2 = "",
                    std::string desc = "")
          : type_m(type),
            band_m(band),
            SN_m(SN),
            ESN1_m(ESN1),
            ESN2_m(ESN2),
            desc_m(desc)
            {}

        void reset()
          { type_m = 0;
            band_m = 0;
            SN_m.clear();
            ESN1_m.clear();
            ESN2_m.clear();
            desc_m.clear(); }
    };

    bool getConfigComponentList(const ID_T &configId, IDList_T &target, int componentTypeFilter = 0, int bandFilter = 0) const;
    ///< get the components which are part of the the specified configuration.
    ///< If componentTypeFilter > 0, filter for a single type of component.
    ///< If bandFilter > 0, filter for a specific cartridge band component.

    bool getConfigComponent(const ID_T &configId, ID_T &componentId, std::string &SN, int componentTypeFilter, int bandFilter = 0) const;
    ///< get a single component ID and SN which is part of the specified configuration.
    ///< if there is more than one match, returns the item having the greatest value of keyId.

    bool getComponentRecord(const ID_T &componentID, FEComponent &target);
    ///< look up the contents of the requested component record.

//-----------------------------------------------------------------------------
// Update front end and configuration operations:

    bool updateFrontEndESN(const ID_T &frontEndId, const std::string &ESN) const;
    ///< update the ESN field of the specified front end.

//-----------------------------------------------------------------------------
// Create or find test data header:

    ID_T createTestDataHeader(const ID_T &configId, const ID_T &componentId, int testDataType, DATASTATUS_TYPES dataStatus, unsigned band,
                              const std::string &notes, const std::string &measSWVer) const;
    ///< create a test data header record and return its ID.
    ///< if componentId is not valid, it will be stored as NULL.

    ID_T findTestDataHeader(const ID_T &configId, const ID_T &componentId, int testDataType, DATASTATUS_TYPES dataStatus, unsigned band = 0) const;
    ///< find the latest test data header record for the given config, component, type, status, and band.
    ///< if componentId is not valid, it will not be used for the search.

//-----------------------------------------------------------------------------
protected:
    static bool checkQueryResult(const std::string &context,int numRows, unsigned expectedRows);
    ///< helper to check whether a query error occurred and the right number of rows returned.

    std::string makeTestDataNotes(const ID_T &configId, int testDataType, DATASTATUS_TYPES dataStatus, const std::string &legend = std::string()) const;
    ///< format a notes string for the test data header using the info provided.

    MySQLConnection *conn_mp;

private:
    // forbid default construct, copy and assign:
    FEICDataBase(const FEICDataBase &other);
    FEICDataBase &operator =(const FEICDataBase &other);

    bool getConfigComponentListImpl(const ID_T &configId, MySQLConnection_Result &res, int componentTypeFilter, int bandFilter) const;
    ///< private helper for getConfigComponentList and getConfigComponent.

    static bool getEnvConnectionInfo(std::string &host, std::string &user, std::string &password, std::string &dbName);
    ///< helper to get the database connection info from the mysql.ini file at the path specified by environment varible TS_CONFIG

    static bool getEnvFETMSDescription(std::string &description);
    ///< helper to get the FETMS description and computer name
};

inline std::ostream &operator << (std::ostream& out, FEICDataBase::ID_T &id)
  { out << id.print(); return out; }
///< stream output for debugging ID_T;

#endif /* FEICDATABASE_H_ */
