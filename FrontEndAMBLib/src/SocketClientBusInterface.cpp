/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2005
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
 * implementation for class SocketClientBusInterface.
 * http://almasw.hq.eso.org/almasw/bin/view/ITS/PsilUserSupport#AmbSocketServer
 *----------------------------------------------------------------------
 */

#include "SocketClientBusInterface.h"
#include "Socket.h"
#include <stdio.h>
#include <string.h> //for memset
#include <string>
using namespace std;

struct SocketServerRequest {
    // The Request Structure consists of 18 bytes (Byte Numbers 0-17)
    short resourceId;   // Byte[0,1] are the Resource identifier. Valid values are -1 to 64 inclusive
                        //      1-64 refer to the Antenna Bus master on antenna ALMA001 - ALMA064 respectivly
                        //      0 refers to the ARTM
                        //      -1 refers to the DMC.
    int busId;          // Byte[2,3] are the bus identifier, bus numbering starts at 0,
    AmbAddr address;    // Byte[4,7] Are the Complete 29 bit Can Identifier, The top 11 bits of the 29 bit address 
                        //  are the Node Number and the remaining 18 bits are the Relative Can Address.
    unsigned char mode; // Byte[8] is an integer Identifing the requested mode:
                        //      0 is a monitor    
                        //      1 is a conmmand
                        //      2 is a getNodes
                        //      3 is a reset 
    AmbDataLength_t dataLength;// Byte[9] is the Length of the valid data. 8 Bytes of data are always sent, 
                               //  but only the first <Byte[9]> of them are valid. For all modes but command this value should be 0.
    AmbDataMem_t data[AMB_DATA_MSG_SIZE]; // Byte[10-17] contain the data used in a command, this field is ignored in all other cases. 

    SocketServerRequest(int _mode = 0, 
                        AmbAddr _address = 0,            
                        AmbDataLength_t _dataLength = 0, 
                        AmbDataMem_t *_data = NULL)
      : resourceId(1),
        busId(1),
        address(_address),
        mode(_mode),
        dataLength((_data) ? _dataLength : 0)
        { memset(data, 0, AMB_DATA_MSG_SIZE);
          if (dataLength > 8) dataLength = 8;
          if (_data && dataLength) memcpy(data, _data, dataLength);
        }
    
    void pack(string &target);
    
    void asText(string &target);
};

void SocketServerRequest::pack(string &target) {
    target.clear();
    unsigned short stmp;
    unsigned long ltmp;    
    stmp = htons(resourceId);
    target.append(reinterpret_cast<char *>(&stmp), 2);
    stmp = htons(busId);    
    target.append(reinterpret_cast<char *>(&stmp), 2);
    ltmp = htonl(address);
    target.append(reinterpret_cast<char *>(&ltmp), 4);
    target.append(reinterpret_cast<char *>(&mode), 1);
    target.append(reinterpret_cast<char *>(&dataLength), 1);
    target.append(reinterpret_cast<char *>(&data[0]), 8);
}

struct SocketServerResponse {
    //The Reponse Structure consists of 13 bytes (Byte Numbers 0-12) 
    
    int errorCode;// Byte[0-3] contain the error code. In the case of a successful completion 0 is returned, 
                  //  otherwise a negative number is returned. The meaning of the error codes is specified on the above web page.
    AmbDataLength_t dataLength; // Byte[4] contains the number of valid data bytes in the data field, 
                                // in case of error, or a command this will be 0. 
                                // In the case of getNodes (mode 2) this byte has special meaning (see Get Nodes Protocol).
    AmbDataMem_t data[AMB_DATA_MSG_SIZE]; // Byte[5-12] contain the returned data, 8 bytes are always returned but only the first <Byte[4]> of these bytes are valid. 
    
    SocketServerResponse()
      : errorCode(0),
        dataLength(0)
        { memset(data, 0, AMB_DATA_MSG_SIZE); }
    
    bool unpack(string &source);
};

bool SocketServerResponse::unpack(string &source) {
    if (source.length() < 13)
        return false;
    unsigned long ltmp;
    memcpy(reinterpret_cast<char *>(&ltmp), source.c_str() + 0, 4);
    errorCode = ntohl(ltmp);
    memcpy(reinterpret_cast<char *>(&dataLength), source.c_str() + 4, 1);
    memcpy(reinterpret_cast<char *>(&data[0]), source.c_str() + 5, 8);
    source.erase(0, 13);
    return true;
}

struct SocketServerNode {
    AmbNodeAddr nodeId; // Byte[0-3] contains the Node ID.
    AmbDataMem_t data[AMB_DATA_MSG_SIZE]; // Byte[4-11] contain the Node Serial Number 
    
    SocketServerNode()
      : nodeId(0)
        { memset(data, 0, AMB_DATA_MSG_SIZE); }
    
    bool unpack(string &source);
};

bool SocketServerNode::unpack(string &source) {
    if (source.length() < 12)
        return false;
    unsigned long ltmp;
    memcpy(reinterpret_cast<char *>(&ltmp), source.c_str(), 4);
    nodeId = ntohl(ltmp);
    memcpy(reinterpret_cast<char *>(&data[0]), source.c_str() + 4, 8);
    source.erase(0, 12);
    return true;
} 

SocketClientBusInterface::SocketClientBusInterface(const std::string& host, int port)
  : socket_mp(NULL)
{
    try {
        socket_mp = new SocketClient(host, port);
    }
    catch(std::string &s) {
        delete socket_mp;
        socket_mp = NULL;
        printf("SocketClientBusInterface: %s\n", s.c_str());
    }
}

SocketClientBusInterface::~SocketClientBusInterface() {
    delete socket_mp;
}

bool SocketClientBusInterface::openChannel(AmbChannel channel) {
    bool success = false;
    
    // initialize the socket interface:
    
    unsigned long handle = 0;
    success = true;
    
    // Set the channel connected status:
    if (success) {        
        channelNodeMap_m.setHandle(channel, handle);
        channelNodeMap_m.openChannel(channel);
    }

    // Find all the nodes on the channel:
    //if (success)
    //    findChannelNodes(channel);

    return success;        
}

void SocketClientBusInterface::closeChannel(AmbChannel channel) {
    if (channelNodeMap_m.isOpenChannel(channel)) {
        channelNodeMap_m.closeChannel(channel);
        // Close the CAN object:
        unsigned long handle = channelNodeMap_m.getHandle(channel);
    }
}

// Do the actual work of clearing the read buffer, sending a monitor request,
// and waiting for a reply before returning.
void SocketClientBusInterface::monitorImpl(unsigned long _handle, AmbMessage_t &msg)
{
    bool success = false;
    bool timeout = false;
    SocketServerRequest req(0, msg.address, msg.dataLen, msg.data);
    string reqBytes, respBytes;
    req.pack(reqBytes);
    
    SocketServerResponse resp;
                
    if (socket_mp) {
        // Request the monitor parameter:

        if (enableDebug_m) {
            printf("monRequ: %X [ ", (unsigned) req.address);
            for (unsigned index = 0; index < reqBytes.length(); ++index)
                printf("%02X ", (unsigned char) reqBytes[index]);
            printf("]\n");
            fflush(NULL);
        }
        
        try {
            socket_mp -> SendBytes(reqBytes);
            if (readWithTimeout(respBytes, monitorTimeout_m)) {

                if (enableDebug_m) {
                    printf("monResp: [ ");
                    for (unsigned index = 0; index < respBytes.length(); ++index)
                        printf("%02X ", (unsigned char) respBytes[index]);
                    printf("]\n");
                    fflush(NULL);
                }

                success = resp.unpack(respBytes);
            } else
                timeout = true;
        }
        catch(const string &s) {
            printf("SocketClientBusInterface::monitorImpl: %s\n", s.c_str());
        }
    }
    if (success) {
        // Save the received data length into the caller's pointer:
        if (msg.completion_p -> dataLength_p)
            *(msg.completion_p -> dataLength_p) = resp.dataLength;
        // Save the received data:
        if (msg.completion_p -> data_p)
            memcpy(msg.completion_p -> data_p, resp.data, resp.dataLength);
        // Result is good status:
        if (msg.completion_p -> status_p)
            *(msg.completion_p -> status_p) = AMBERR_NOERR;
    
    } else if (timeout) {
        // Read timed out or other error.  Return zero data:
        if (msg.completion_p -> dataLength_p)
            *(msg.completion_p -> dataLength_p) = 0;
        if (msg.completion_p -> data_p)
            memset(msg.completion_p -> data_p, 0, AMB_DATA_MSG_SIZE);
        if (msg.completion_p -> status_p)
            *(msg.completion_p -> status_p) = AMBERR_TIMEOUT;
    }
}                                       
  
// Do the actual work of sending a command:
void SocketClientBusInterface::commandImpl(unsigned long _handle, AmbMessage_t &msg)
{
    int status = -1;
    
    if (socket_mp) {
        // Pack and send the command:
        SocketServerRequest req(1, msg.address, msg.dataLen, msg.data);
        string reqBytes;
        req.pack(reqBytes);
        
        if (enableDebug_m) {
            printf("command: %X [ ", (unsigned) req.address);
            for (unsigned index = 0; index < reqBytes.length(); ++index)
                printf("%02X ", (unsigned char) reqBytes[index]);
            printf("]\n");
            fflush(NULL);
        }
        
        try {
            socket_mp -> SendBytes(reqBytes);
            status = 0;
        }
        catch(const string &s) {
            printf("SocketClientBusInterface::commandImpl: %s\n", s.c_str());
        }
    }
    if (msg.completion_p -> status_p) {
        if (status >= 0)
            *(msg.completion_p -> status_p) = AMBERR_NOERR;
        else
            *(msg.completion_p -> status_p) = AMBERR_WRITEERR;
    }
}                                       

bool SocketClientBusInterface::readWithTimeout(std::string &target, unsigned long timeout) {
    string tmp;
    unsigned long startTime = GETTIMER();
    unsigned long stopTime = startTime + timeout;
    while (GETTIMER() < stopTime) {
        tmp = socket_mp -> ReceiveBytes();
        if (tmp.length()) {
            target.append(tmp);
            return true;
        }
        SLEEP(1);
    }
    return false;
}

void SocketClientBusInterface::findChannelNodes(AmbChannel channel) {
    if (!socket_mp)
        return;
    
    // Check that the channel is open:
    if (channelNodeMap_m.isOpenChannel(channel)) {
        // Drop all known nodes:
        channelNodeMap_m.clearNodes(channel);
        // Get the channel object handle:
        unsigned long handle = channelNodeMap_m.getHandle(channel);  

        bool success = false;

        SocketServerRequest req(2);
        SocketServerResponse resp;
        SocketServerNode node;
               
        string reqBytes, respBytes;
        req.pack(reqBytes);
        
        if (enableDebug_m) {
            printf("getNods: [");
            for (unsigned index = 0; index < reqBytes.length(); ++index)
                printf("%02X ", (unsigned char) reqBytes[index]);
            printf("]\n");
            fflush(NULL);
        }
        
        try {
            socket_mp -> SendBytes(reqBytes);
            if (readWithTimeout(respBytes, monitorTimeout_m)) {

                if (enableDebug_m) {
                    printf("nodResp: [");
                    for (unsigned index = 0; index < respBytes.length(); ++index)
                        printf("%02X ", (unsigned char) respBytes[index]);
                    printf("]\n");
                    fflush(NULL);
                }

                success = resp.unpack(respBytes);
            }
        }
        catch(const string &s) {
            printf("SocketClientBusInterface::findChannelNodes: %s\n", s.c_str());
            return;
        }
        
        if (success && resp.errorCode == 0) {
            int count = resp.dataLength;
            bool timeout = false;
            while (!timeout && count > 0) {                        
                if (!node.unpack(respBytes)) {
                    if (!readWithTimeout(respBytes, monitorTimeout_m))
                        timeout = true;
                    else {
                        if (enableDebug_m) {
                            printf("nodResp: [");
                            for (unsigned index = 0; index < respBytes.length(); ++index)
                                printf("%02X ", (unsigned char) respBytes[index]);
                            printf("]\n");
                            fflush(NULL);
                        }
                    }
                } else {
                    --count;
                    channelNodeMap_m.addNode(channel, node.nodeId, node.data);
                    if (enableDebug_m) {
                        printf("Node %02X:\n"
                               "  SN: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                               static_cast<unsigned int>(node.nodeId),
                               node.data[0], node.data[1], node.data[2], node.data[3], 
                               node.data[4], node.data[5], node.data[6], node.data[7]);
                    }
                }
            }
        }    
    }
}



