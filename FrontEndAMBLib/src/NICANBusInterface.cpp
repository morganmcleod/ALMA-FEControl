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

#include "NICANBusInterface.h"
#include <stdio.h>
#include <nican.h>

// --------------------------------------------------------------------------
//private:

void printNCStatus(NCTYPE_STATUS status, const char *source) {
    char statusString[400];
    if(status !=0) {
        ncStatusToString(status, sizeof(statusString), statusString);
        printf("\n%s\nSource = %s\n", statusString, source);
    }
}



bool NICANBusInterface::openChannel(AmbChannel channel) {
    bool success = false;
    
    // Setup to initialize NI-CAN:
    NCTYPE_ATTRID           AttrIdList[8];
    NCTYPE_UINT32           AttrValueList[8];
    NCTYPE_BAUD_RATE        BaudRate = NC_BAUD_1000K;
    char                    ObjName[] = "CAN0";
    ObjName[3] = (char) channel + 48;

    // Configure the CAN Network Interface Object:
    AttrIdList[0] =         NC_ATTR_BAUD_RATE;
    AttrValueList[0] =      BaudRate;
    AttrIdList[1] =         NC_ATTR_START_ON_OPEN;
    AttrValueList[1] =      NC_TRUE;
    AttrIdList[2] =         NC_ATTR_READ_Q_LEN;
    AttrValueList[2] =      50;
    AttrIdList[3] =         NC_ATTR_WRITE_Q_LEN;
    AttrValueList[3] =      50;
    AttrIdList[4] =         NC_ATTR_CAN_COMP_STD;
    AttrValueList[4] =      0;
    AttrIdList[5] =         NC_ATTR_CAN_MASK_STD;
    AttrValueList[5] =      NC_CAN_MASK_STD_DONTCARE;
    AttrIdList[6] =         NC_ATTR_CAN_COMP_XTD;
    AttrValueList[6] =      0;
    AttrIdList[7] =         NC_ATTR_CAN_MASK_XTD;
    AttrValueList[7] =      NC_CAN_MASK_XTD_DONTCARE;

    // initialize the CAN interface:

    NCTYPE_STATUS status = ncConfig(ObjName, 8, AttrIdList, AttrValueList);
    success = (status >= 0);
    
    if (!success) {
        printf("NICANBusInterface: ncConfig failed. obj=%s status=%d\n", ObjName, (int) status);
        printNCStatus(status, "NICANBusInterface::openChannel");
    }
        
    NCTYPE_OBJH handle = 0;
    if (success) {
        status = ncOpenObject(ObjName, &handle);
        success = (status >= 0);
        if (!success) {
            printf("NICANBusInterface: ncOpenObject failed. obj=%s status=%d\n", ObjName, (int) status);
            printNCStatus(status, "NICANBusInterface::openChannel");
        }
    }

    
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

void NICANBusInterface::closeChannel(AmbChannel channel) {
    if (channelNodeMap_m.isOpenChannel(channel)) {
        // Close the CAN object:
        NCTYPE_STATUS status = ncCloseObject(channelNodeMap_m.getHandle(channel));
        // Mark the channel closed in the map:
        channelNodeMap_m.closeChannel(channel);        
    }
}

// Do the actual work of clearing the read buffer, sending a monitor request,
// and waiting for a reply before returning.
void NICANBusInterface::monitorImpl(unsigned long _handle, AmbMessage_t &msg)
{
    NCTYPE_OBJH handle = _handle; 
        
    // Read and discard any stale data in the read buffer:
    flushReadBuffer(handle);

    // Request the monitor parameter:
    NCTYPE_STATUS status;
    NCTYPE_STATE currentState = 0;
    NCTYPE_CAN_FRAME request;
    memset(&request, 0, sizeof(NCTYPE_CAN_FRAME));
    NCTYPE_CAN_STRUCT response;
    memset(&response, 0, sizeof(NCTYPE_CAN_STRUCT));
    request.ArbitrationId = NC_FL_CAN_ARBID_XTD + msg.address;
    request.IsRemote = 0;
    request.DataLength = 0;
    memset(request.Data, 0, AMB_DATA_MSG_SIZE);

    status = ncWrite(handle, sizeof(NCTYPE_CAN_FRAME), &request);

    bool success = false;
    bool timeout = false;
    while (!success && !timeout) {
        // Wait for data ready to read:
        status = ncWaitForState(handle, NC_ST_READ_AVAIL, monitorTimeout_m, &currentState);
        if (status >= 0) {
            status = ncRead(handle,  sizeof(NCTYPE_CAN_STRUCT), &response);
        
            if (enableDebug_m) {        
                printf("monitorImpl: %x -> %x [ ", (unsigned) request.ArbitrationId, (unsigned) response.ArbitrationId);
                for (int index = 0; index < response.DataLength; ++index)
                    printf("%02X ", response.Data[index]);
                printf("]\n");
            }

            if (response.ArbitrationId == request.ArbitrationId)
                success = true;
        } else
            timeout = true;
    }
    if (success) {
        // Save the received data length into the caller's pointer:
        if (msg.completion_p -> dataLength_p)
            *(msg.completion_p -> dataLength_p) = response.DataLength;
        // Save the received data:
        if (msg.completion_p -> data_p)
            memcpy(msg.completion_p -> data_p, response.Data, response.DataLength);
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
void NICANBusInterface::commandImpl(unsigned long _handle, AmbMessage_t &msg)
{
    NCTYPE_OBJH handle = _handle; 

    // Send the command:
    NCTYPE_STATUS status;
    NCTYPE_STATE currentState = 0;
    NCTYPE_CAN_FRAME command;
    command.ArbitrationId = NC_FL_CAN_ARBID_XTD + msg.address;
    command.IsRemote = 0;
    command.DataLength = msg.dataLen;
    memset(command.Data, 0, AMB_DATA_MSG_SIZE);
    memcpy(command.Data, msg.data, msg.dataLen);

    if (enableDebug_m) {
        printf("commandImpl: %x [ ", (unsigned) command.ArbitrationId);
        for (int index = 0; index < command.DataLength; ++index)
            printf("%02X ", command.Data[index]);
        printf("]\n");
    }

    status = ncWrite(handle, sizeof(NCTYPE_CAN_FRAME), &command);
    
    status = ncWaitForState(handle, NC_ST_WRITE_SUCCESS, monitorTimeout_m, &currentState);
    
    if (msg.completion_p -> status_p) {
        if (status >= 0)
            *(msg.completion_p -> status_p) = AMBERR_NOERR;
        else
            *(msg.completion_p -> status_p) = AMBERR_WRITEERR;
    }
}

void NICANBusInterface::findChannelNodes(AmbChannel channel) {
    // Check that the channel is open:
    if (channelNodeMap_m.isOpenChannel(channel)) {
        // Drop all known nodes:
        channelNodeMap_m.clearNodes(channel);
        // Get the NICAN object handle:
        NCTYPE_OBJH handle = channelNodeMap_m.getHandle(channel);  
        // clear the CAN read buffer of any garbage:
        flushReadBuffer(handle);

        NCTYPE_CAN_FRAME request;
        request.ArbitrationId = NC_FL_CAN_ARBID_XTD;
        request.IsRemote = 0;
        request.DataLength = 0;
        memset(request.Data, 0, AMB_DATA_MSG_SIZE);
    
        // Write a broadcast request for all nodes to announce themselves:
        NCTYPE_STATUS status = ncWrite(handle, sizeof(NCTYPE_CAN_FRAME), &request);

        unsigned nodeId;
        int node_count = 0;
        NCTYPE_STATE currentState = 0;
        NCTYPE_CAN_STRUCT response;

        // Gather info on all nodes which reply:
        bool done = false;
        while (!done) {
            status = ncWaitForState(handle, NC_ST_READ_AVAIL, monitorTimeout_m, &currentState);
            if (status < 0)
                done = true;
            else {
                status = ncRead(handle,  sizeof(NCTYPE_CAN_STRUCT), &response);
                if (status >= 0) {
                    ++node_count;
                    nodeId = (response.ArbitrationId - NC_FL_CAN_ARBID_XTD) / 0x40000U - 1U;
                    channelNodeMap_m.addNode(channel, nodeId, response.Data);
                    if (enableDebug_m) {
                        printf("Node %02X (arb. %X):\n"
                               "  SN: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                               static_cast<unsigned int>(nodeId),
                               static_cast<unsigned int>(response.ArbitrationId),
                               response.Data[0], response.Data[1], response.Data[2], response.Data[3],
                               response.Data[4], response.Data[5], response.Data[6], response.Data[7]);
                    }
                }
            }
        }
    }
}

void NICANBusInterface::flushReadBuffer(unsigned long _handle) {
    NCTYPE_OBJH handle = _handle;
    NCTYPE_STATUS status;
    NCTYPE_UINT32 attrValue;
    NCTYPE_CAN_STRUCT response;
        
    // Read and discard any stale data in the read buffer:
    bool done = false;
    while (!done) {
        status = ncGetAttribute(handle, NC_ATTR_STATE, sizeof(NCTYPE_UINT32), &attrValue);
        if (attrValue == NC_ST_READ_AVAIL) {
            status = ncRead(handle,  sizeof(NCTYPE_CAN_STRUCT), &response);
            printf("flushed one %d\n", (int) status);
        } else
            done = true;
    }
}

