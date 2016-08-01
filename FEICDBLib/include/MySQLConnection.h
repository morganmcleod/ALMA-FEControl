#ifndef MYSQLCONNECTION_H_
#define MYSQLCONNECTION_H_

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

#include "portable.h"
#include <mysql.h>
#include <string>

/// class MySQLConnection implements a thin wrapper for a connection
/// to a MySQL database server and includes simple utility functions.
///
/// Since the MySQL interfacing requirements for the low-level FE control 
/// software in the FETMS are fairly minimal, a thin wrapper around the
/// MySQL C API seems preferable to adopting one of the 3rdparty C++ MySQL 
/// libraries which abound on the internet.

class MySQLConnection {
public:
    MySQLConnection(const std::string &host, 
                    const std::string &user, 
                    const std::string &password,
                    const std::string &dbName);
    ///< construct with parameters for connection to a database on the server.                    
    
    virtual ~MySQLConnection();
    
    bool connect();
    ///< Connect to the database server.
    ///< Returns true if successful.

    void disconnect();
    ///< Disconnect from the server.
    
    bool isConnected() const;
    ///< Check the connection to the server.
    ///< Returns true if connected.

    class Result {
    ///< Thin wrapper class around MySQL's query result type.
    public:
        Result(MYSQL_RES *res = NULL)
          : res_mp(res)
            {}
        ///< construct with an optional MYSQL_RES to take ownership of.
        
        ~Result()
          { erase(); }
        ///< destructor frees contained MYSQL_RES object.
          
        MYSQL_ROW fetch_row();
        ///< fetch the next row from a select query.

    private:
        Result(const Result &other);
        ///< copy construct is forbidden.

        Result &operator =(const Result &other);
        ///< assignment is forbidden.

        MYSQL_RES *get() const
          { return res_mp; }
        ///< get access to the contained result.  Handle with care since this class will still be freeing it on destroy.

        void set(MYSQL_RES *res);
        ///< private helper to set a new result into this object.

        void erase();
        ///< private helper to free the contained MYSQL_RES object.
    
        MYSQL_RES *res_mp;
        ///< the contained query result object.
        
        friend class MySQLConnection;
        ///< the outer MySQLConnection class can accsess private members.
    };           

    int executeQuery(std::string SQL, Result *result = NULL);
    ///< Execute an SQL query against the selected database.
    ///< Returns the number of rows returned or affected or -1 on error.
    ///< Parameter result is a pointer a Result object, declared above and is optional. 
    ///< If the query returns a result set and result is non-NULL, it will be stored there.   
    
    unsigned long getLastInsertID();
    ///< Return the last ID arising from an INSERT query involving an AUTO_INCREMENT field.

    const std::string &escapeString(const std::string &src, std::string &target);
    ///< Add escape characters to the given string and return it.

    const std::string &rowToString(const Result &result, const MYSQL_ROW &row, std::string &target) const;
    // format a row as a text string for logging.

    static const std::string &unpackString(const MYSQL_ROW &row, unsigned index, std::string &target);
    ///< helper to check and get a string from a specified index in a row.

private:

    MySQLConnection();
    ///< default construct is private and forbidden.
    MySQLConnection(const MySQLConnection &other);
    ///< copy construct is private and forbidden.
    MySQLConnection &operator =(const MySQLConnection &other);
    ///< assignment is private and forbidden.

    std::string host;        ///< server hostname
    std::string user;        ///< server user name
    std::string password;    ///< login password cached here
    std::string dbName;      ///< database name on the server

    struct MySQLData_t;     	///< declare private implementation struct.
    MySQLData_t *MySQLData; 	///< one instance of impl struct.
};

#endif /*MYSQLCONNECTION_H_*/
