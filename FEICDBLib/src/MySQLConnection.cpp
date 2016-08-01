#include "MySQLConnection.h"
#include "logger.h"
using namespace std;

struct MySQLConnection::MySQLData_t {
    MYSQL *mysql_mp;
    bool connectionValid;    
    
    MySQLData_t()
      : mysql_mp(NULL),
        connectionValid(false)
        {}
        
    ~MySQLData_t()
      { disconnect(); }

    void disconnect()
      { if (connectionValid && mysql_mp)
            mysql_close(mysql_mp);
        connectionValid = false;
        mysql_mp = NULL; }

    const MYSQL *get() const
      { return mysql_mp; }

    MYSQL *&use()
      { return mysql_mp; }
};

//---------------------------------------------------------------------------

MySQLConnection::MySQLConnection(const std::string &_host, 
                                 const std::string &_user, 
                                 const std::string &_password, 
                                 const std::string &_dbName)
  : host(_host),
    user(_user),
    password(_password),
    dbName(_dbName),
    MySQLData(new MySQLData_t)
{    
    connect();
}    

MySQLConnection::~MySQLConnection() {
    disconnect();
    delete MySQLData;
}
    
bool MySQLConnection::connect() {
    disconnect();

    MYSQL *&mysql = MySQLData -> use();
    mysql = mysql_init((MYSQL*) NULL);
 
    if (!mysql) {
    	LOG(LM_INFO) << "MySQLConnection: Failed to initialize MySQL client." << endl;
        return false;

    } else if (!mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), NULL, MYSQL_PORT, NULL, 0 )) {
    	LOG(LM_INFO) << "MySQLConnection: Failed to connect to database; '" << mysql_error(mysql) << "'" << endl;
        mysql_close(mysql);
        return false;

    } else if (mysql_select_db(MySQLData -> use(), dbName.c_str()) != 0) {
    	LOG(LM_INFO) << "MySQLConnection: Can't select the '" << dbName << "' database." << endl;
        mysql_close(mysql);
        return false;
    }
    MySQLData -> connectionValid = true;
    LOG(LM_INFO) << "MySQLConnection: Connected to MySQL database '" << dbName << "' on host '" << host << "'." << endl;
    return true;   
}

void MySQLConnection::disconnect() {
    if (isConnected()) {
        MySQLData -> disconnect();
        LOG(LM_INFO) << "MySQLConnection: Disconnected from MySQL database." << endl;
    }
}

bool MySQLConnection::isConnected() const {
    if (!MySQLData)
        return false;   // no underlying connection object.
    if (!MySQLData -> connectionValid)
        return false;   // connection was never valid or purposefully disconnected.
    if (mysql_ping(MySQLData -> use()) == 0)
        return true;    // connection is good.
    return false;   // connection gone or unknown error
}

MYSQL_ROW MySQLConnection::Result::fetch_row() {
    if (res_mp)
        return mysql_fetch_row(res_mp);
    else
        return NULL;
}

void MySQLConnection::Result::set(MYSQL_RES *res) {
    erase();
    res_mp = res;
}

void MySQLConnection::Result::erase() {
    if (res_mp) {
        mysql_free_result(res_mp);
        res_mp = NULL;
    }
}

int MySQLConnection::executeQuery(string SQL, Result *result) {
    if (result)
        result -> erase();

    if (!isConnected()) {
    	LOG(LM_INFO) << "MySQLConnection: SQL query failed: not connected to MySQL database." << endl;
        return -1;
    }
    
    MYSQL *&mysql = MySQLData -> use();
    if (mysql_query(mysql, SQL.c_str()) != 0) {
    	LOG(LM_INFO) << "MySQLConnection: Query failed; '" << mysql_error(mysql) << "'" << endl
    	                << SQL << endl;
        return -1;
    } 
    
    int num_rows = 0;
    
    MYSQL_RES *res = mysql_store_result(mysql);
    if (!res)
        num_rows = mysql_affected_rows(mysql);
    else {
        num_rows = mysql_num_rows(res);
        if (result)
            result -> set(res);
        else
            mysql_free_result(res);
    }    
    return num_rows;
}

unsigned long MySQLConnection::getLastInsertID() {
    return (unsigned long) mysql_insert_id(MySQLData -> use());
}

const std::string &MySQLConnection::escapeString(const std::string &src, std::string &target) {
    unsigned long len = src.length();
    char *buf = new char[len * 2 + 1];
    mysql_real_escape_string(MySQLData -> use(), buf, src.c_str(), len);
    target = buf;
    delete [] buf;
    return target;
}

const std::string &MySQLConnection::rowToString(const Result &result, const MYSQL_ROW &row, std::string &target) const {
    target.clear();
    MYSQL_RES *res = result.get();
    if (row && res) {
        unsigned num_fields = mysql_num_fields(res);
        for (unsigned field = 0; field < num_fields; ++field) {
            if (field > 0)
                target += ", ";
            if (!row[field])
                target += "(NULL)";
            else
                target += "\'" + string(row[field]) + "\'";
        }
    }
    return target;
}

const std::string &MySQLConnection::unpackString(const MYSQL_ROW &row, unsigned index, std::string &target) {
    target.erase();
    if (row != NULL) {
        if (row[index])
            target = string(row[index]);
    }
    return target;
}


