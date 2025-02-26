/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2025
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
*/

/************************************************************************
 * implementation for class SocketClientBusInterface.
 * http://almasw.hq.eso.org/almasw/bin/view/ITS/PsilUserSupport#AmbSocketServer
 *----------------------------------------------------------------------
 */

#include "SocketClientBusInterface.h"
#include <array>
#include <iostream>
#include <chrono>
#include <vector>
#include "string.h"
#include "stringConvert.h"
#include "delayStats.h"
#include "logger.h"
using namespace std;

const size_t REQUEST_LEN(18);

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
    
    void pack(std::array<char, REQUEST_LEN> &target);
};

void SocketServerRequest::pack(std::array<char, REQUEST_LEN> &target) {
    target.fill(0);
    *reinterpret_cast<unsigned short *>(&target[0]) = htons(resourceId);
    *reinterpret_cast<unsigned short *>(&target[2]) = htons(busId);
    *reinterpret_cast<unsigned long *>(&target[4]) = htonl(address);
    target[8] = mode;
    target[9] = dataLength;
    memcpy(&target[10], &data[0], 8);
}

const size_t RESPONSE_LEN(13);

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
    
    void unpack(std::array<char, RESPONSE_LEN> &source);
};

void SocketServerResponse::unpack(std::array<char, RESPONSE_LEN> &source) {
    errorCode = *reinterpret_cast<int*>(&source[0]);
    dataLength = source[4];
    memcpy(data, reinterpret_cast<AmbDataMem_t *>(&source[5]), 8);
}

const size_t NODE_LEN(12);

struct SocketServerNode {
    AmbNodeAddr nodeId; // Byte[0-3] contains the Node ID.
    AmbDataMem_t data[AMB_DATA_MSG_SIZE]; // Byte[4-11] contain the Node Serial Number 
    
    SocketServerNode()
      : nodeId(0)
        { memset(data, 0, AMB_DATA_MSG_SIZE); }
    
    void unpack(std::array<char, NODE_LEN> &source);
};

void SocketServerNode::unpack(std::array<char, NODE_LEN> &source) {
    nodeId = *reinterpret_cast<AmbNodeAddr*>(&source[0]) >> 24;
    memcpy(data, reinterpret_cast<AmbDataMem_t *>(&source[4]), 8);
}

////////////////////////////////////////////////////////////////////////////////////

SocketClientBusInterface::SocketClientBusInterface(const std::string& host, int port)
  : io_m(),
    sock_mp(NULL)
{
    boost::asio::ip::tcp::resolver resolver(io_m);
    endpoints_m = resolver.resolve(host, to_string(port));
    openSocket();
}

SocketClientBusInterface::~SocketClientBusInterface() {
    closeSocket();
}

void SocketClientBusInterface::openSocket() {
    if (endpoints_m.begin() == endpoints_m.end()) {
        LOG(LM_ERROR) << "SocketClientBusInterface::openSocket no valid endpoint" << endl;
        return;
    }
    sock_mp = new boost::asio::ip::tcp::socket(io_m);
    try {
        boost::asio::connect(*sock_mp, endpoints_m);
    }
    catch (boost::system::system_error &e) {
        LOG(LM_ERROR) << "SocketClientBusInterface::openSocket connect error:\n" << e.what() << endl;
        delete sock_mp;
        sock_mp = NULL;
    }
}

void SocketClientBusInterface::closeSocket() {
    if (!sock_mp)
        return;
    try {
        (*sock_mp).close();
    }
    catch (boost::system::system_error &e) {
        LOG(LM_ERROR) << "SocketClientBusInterface::closeSocket close error:\n" << e.what() << endl;
    }
    delete sock_mp;
    sock_mp = NULL;
}

bool SocketClientBusInterface::openChannel(AmbChannel channel) {
    bool success = false;
    unsigned long handle = 0;
    success = true;
    
    // Set the channel connected status:
    if (success) {        
        channelNodeMap_m.setHandle(channel, handle);
        channelNodeMap_m.openChannel(channel);
        findChannelNodes(channel);
    }
    return success;        
}

void SocketClientBusInterface::closeChannel(AmbChannel channel) {
    if (channelNodeMap_m.isOpenChannel(channel)) {
        channelNodeMap_m.closeChannel(channel);
    }
}

// Do the actual work of sending a monitor request,
// and waiting for a reply before returning.
void SocketClientBusInterface::monitorImpl(unsigned long _handle, AmbMessage_t &msg)
{
    if (!sock_mp) {
        LOG(LM_ERROR) << "SocketClientBusInterface::monitorImpl no socket" << endl;
        return;
    }

    flushRead();

    bool error = false;      
    SocketServerRequest req(0, msg.address, msg.dataLen, msg.data);
    SocketServerResponse resp;
    std::array<char, REQUEST_LEN> reqBytes;
    req.pack(reqBytes);
    
    if (enableDebug_m) {
        printf("monitor: %X [ ", (unsigned) req.address);
        for (unsigned index = 0; index < REQUEST_LEN; ++index)
            printf("%02X ", (unsigned char) reqBytes[index]);
        printf("]\n");
        fflush(NULL);
    }

    try {
        boost::system::error_code sock_error;
        boost::asio::write(*sock_mp, boost::asio::buffer(reqBytes), sock_error);
        if (sock_error) {
            LOG(LM_ERROR) << "SocketClientBusInterface::monitorImpl write error: " << sock_error.value() << endl;
            error = true;
        } else if (!readResponse(*sock_mp, resp)) {
            error = true;
        }
    }
    catch (std::exception& e) {
        LOG(LM_ERROR) << "SocketClientBusInterface::monitorImpl:\n" << e.what() << endl;
    }
    if (!error) {
        // Save the received data length into the caller's pointer:
        if (msg.completion_p -> dataLength_p)
            *(msg.completion_p -> dataLength_p) = resp.dataLength;
        // Save the received data:
        if (msg.completion_p -> data_p)
            memcpy(msg.completion_p -> data_p, resp.data, resp.dataLength);
        // Result is good status:
        if (msg.completion_p -> status_p)
            *(msg.completion_p -> status_p) = AMBERR_NOERR;
    } else {
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
    if (!sock_mp) {
        LOG(LM_ERROR) << "SocketClientBusInterface::monitorImpl no socket" << endl;
        return;
    }
    AmbErrorCode_t status = AMBERR_NOERR;

    flushRead();
    
    // Pack and send the command:
    SocketServerRequest req(1, msg.address, msg.dataLen, msg.data);
    std::array<char, REQUEST_LEN> reqBytes;
    req.pack(reqBytes);
    if (enableDebug_m) {
        printf("command: %X [ ", (unsigned) req.address);
        for (unsigned index = 0; index < REQUEST_LEN; ++index)
            printf("%02X ", (unsigned char) reqBytes[index]);
        printf("]\n");
        fflush(NULL);
    }
    try {
        boost::system::error_code sock_error;
        boost::asio::write(*sock_mp, boost::asio::buffer(reqBytes), sock_error);
        if (sock_error) {
            LOG(LM_ERROR) << "SocketClientBusInterface::commandImpl write error " << sock_error.value() << endl;
            status = AMBERR_WRITEERR;
        } else {
            SocketServerResponse resp;
            readResponse(*sock_mp, resp);
        }
    }
    catch (std::exception& e) {
        LOG(LM_ERROR) << "SocketClientBusInterface::commandImpl:\n" << e.what() << endl;
        status = AMBERR_WRITEERR;
    }
    if (msg.completion_p -> status_p) {
       *(msg.completion_p -> status_p) = status;
    }
}                                       

void SocketClientBusInterface::flushRead() {
    std::array<char, 100> buf;
    boost::system::error_code sock_error;
    std::size_t total = 0;
    std::size_t len = (*sock_mp).available(sock_error);
    while (len > 0) {
        total += len;
        len = (*sock_mp).read_some(boost::asio::buffer(buf), sock_error);
        len = (*sock_mp).available(sock_error);
    }
    if (total > 0) {
        LOG(LM_WARNING) << "SocketClientBusInterface::flushRead " << total << " bytes" << endl;
    }
}


bool SocketClientBusInterface::readResponse(boost::asio::ip::tcp::socket &sock, SocketServerResponse &target) {
    // values used for measuring response time.  Enabed by measureLatency_m:
    static delayStats<unsigned long> calc_response_time;
    static std::vector<unsigned long> monitorTimes;
    static unsigned int N = 1000;
    auto start = std::chrono::steady_clock::now();
    unsigned long elapsed;

    std::array<char, RESPONSE_LEN> buf;
    boost::system::error_code sock_error;

    bool error = false;
    try {
        boost::asio::read(sock, boost::asio::buffer(buf), boost::asio::transfer_exactly(RESPONSE_LEN), sock_error);
        if (sock_error) {
            LOG(LM_ERROR) << "SocketClientBusInterface::readResponse read error " << sock_error.value() << endl;
            error = true;
        }
    }
    catch (boost::system::system_error &e) {
        LOG(LM_ERROR) << "SocketClientBusInterface::readResponse read error " << e.what() << endl;
        error = true;
    }
    if (!error) {
        if (measureLatency_m) {
            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
            monitorTimes.push_back(elapsed);
            if (monitorTimes.size() == N) {
                calc_response_time.calculate(monitorTimes);
                LOG(LM_INFO) << "monitor elapsed N=" << N << ", mean=" << calc_response_time.mean_m 
                             << ", stdev=" << calc_response_time.std_m << ", max=" << calc_response_time.max_m << " us" << endl;
                monitorTimes.clear();
            }
        }
        if (enableDebug_m) {
            printf("response: [ ");
            for (unsigned index = 0; index < RESPONSE_LEN; ++index)
                printf("%02X ", (unsigned char) buf[index]);
            printf("]\n");
            fflush(NULL);
        }
        target.unpack(buf);
        return true;
    }
    return false;
}

void SocketClientBusInterface::findChannelNodes(AmbChannel channel) {
    if (!sock_mp) {
        LOG(LM_ERROR) << "SocketClientBusInterface::findChannelNodes no socket" << endl;
        return;
    }
    // Check that the channel is open:
    if (!channelNodeMap_m.isOpenChannel(channel)) {
        LOG(LM_ERROR) << "SocketClientBusInterface::findChannelNodes channel is not open" << endl;
        return;
    }
    // Drop all known nodes:
    channelNodeMap_m.clearNodes(channel);

    bool success = false;

    SocketServerRequest req(2);
    SocketServerResponse resp;
    SocketServerNode node;
    array<char, REQUEST_LEN> reqBytes;
    req.pack(reqBytes);
    if (enableDebug_m) {
        printf("find nodes: [");
        for (unsigned index = 0; index < REQUEST_LEN; ++index)
            printf("%02X ", (unsigned char) reqBytes[index]);
        printf("]\n");
        fflush(NULL);
    }
    try {
        boost::system::error_code sock_error;
        boost::asio::write(*sock_mp, boost::asio::buffer(reqBytes), sock_error);
        if (sock_error) {
            LOG(LM_ERROR) << "SocketClientBusInterface::findChannelNodes write error " << sock_error.value() << endl;
        } else {
            success = readResponse(*sock_mp, resp);
            if (success && resp.errorCode == 0) {
                // Number of nodes found is in dataLength:
                int count = resp.dataLength;
                array<char, NODE_LEN> buf;
                bool error = false;
                while (!error && count > 0) {
                    boost::asio::read(*sock_mp, boost::asio::buffer(buf), boost::asio::transfer_exactly(NODE_LEN), sock_error);
                    if (sock_error) {
                        LOG(LM_ERROR) << "SocketClientBusInterface::findChannelNodes read error " << sock_error.value() << endl;
                        error = true;
                    } else {
                        --count;
                        node.unpack(buf);
                        channelNodeMap_m.addNode(channel, node.nodeId, node.data);
                        if (enableDebug_m) {
                            printf("Node 0x%02X: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                                   static_cast<unsigned int>(node.nodeId),
                                   node.data[0], node.data[1], node.data[2], node.data[3], 
                                   node.data[4], node.data[5], node.data[6], node.data[7]);
                        }
                    }
                }
            }
        }
    }
    catch (std::exception& e) {
        LOG(LM_ERROR) << "SocketClientBusInterface::findChannelNodes: " << e.what() << endl;
        return;
    }
}



