#ifndef _SOCKETCLIENTBUSINTERFACE_H_
#define _SOCKETCLIENTBUSINTERFACE_H_
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
* "@(#) $Id$"
*
*/

/************************************************************************
 * class SocketClientBusInterface partially implements the command and monitor 
 * protocol in terms of the SocketServer interface described here:
 * http://almasw.hq.eso.org/almasw/bin/view/ITS/PsilUserSupport#AmbSocketServer
 *----------------------------------------------------------------------
 */

#include <winsock2.h>
#include <windows.h>
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include "CANBusInterface.h"

class SocketServerResponse;

class SocketClientBusInterface : public CANBusInterface {
public:
    SocketClientBusInterface(const std::string& host, int port);

    virtual ~SocketClientBusInterface();
    
    virtual const nodeList_t* findNodes(AmbChannel channel) {
        return NULL;   //TODO: implement
    }
    ///< Find all nodes on the specified channel.  Opens the channel if necessary.
    ///< Returns NULL if the channel could not be opened.

private:
    void openSocket();
    void closeSocket();

    virtual bool openChannel(AmbChannel channel);
    // initialize a CAN interface channel.
    
    virtual void closeChannel(AmbChannel channel);
    // shut down a CAN interface channel.

    virtual void monitorImpl(unsigned long handle, AmbMessage_t &msg);  
    // Called by worker thread.  Performs the actual work of requesting 
    // and recieving a monitor point on the CAN bus.
    
    virtual void commandImpl(unsigned long handle, AmbMessage_t &msg);
    // Called by worker thread.  Performs the actual work of sending
    // a command on the CAN bus.

    void flushRead();
    // flush the read buffer

    bool readResponse(boost::asio::ip::tcp::socket &sock, SocketServerResponse &target);
    // read on the socket, waiting up to the specified timeout.
    
    void findChannelNodes(AmbChannel channel);
    // finds and stores info about all nodes node on the channel.
    // This blocks the channel, trashes any pending responses, and takes time so use sparingly.
    // fails if channel is closed.
    
    boost::asio::io_context io_m;
    boost::asio::ip::tcp::resolver::results_type endpoints_m;
    boost::asio::ip::tcp::socket *sock_mp;

    std::vector<unsigned long> monitorTimes_m;
};

#endif
