#ifndef CANBUSINTERFACE_H_
#define CANBUSINTERFACE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2003 
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

#include "ambInterface.h"
#include "ambQueue.h"
#include "ChannelNodeMap.h"
#include <vector>

/// This class implements the abstract AmbInterfaceBus, providing a worker thread and policy
/// for queueing of messages, and a data structure for tracking CAN channels and nodes.
/// Further derived classes must implement the particulars of bus opening, closing, and communication.
class CANBusInterface : public AmbInterfaceBus {
public:
    static int maxChannels_m;
    ///< Max number of CAN channels to support simultaneously.  Default is 6.

    static bool enableDebug_m;
    ///< Enables debug logging of CAN traffic to stdout.  Default is false.

    static bool enableDebugLifecycle_m;
    ///< Enables debug logging of startup/shutdown to stdout.  Default is false.
    
    static bool noTransmit_m;
    ///< For debugging with no CAN hardware or front end.
    ///< If true, all messages are timed out immediately rather than sent.

    static int monitorTimeout_m;
    ///< Timeout in ms while waiting for CAN responses.

    CANBusInterface();
    ///< Constructor for abstract class.

    virtual ~CANBusInterface();
    ///< Destructor for abstract class.
    
    virtual const nodeList_t* findNodes(AmbChannel channel) = 0;
    ///< Find all nodes on the specified channel.  Opens the channel if necessary.

    virtual void sendMessage(const AmbMessage_t &msg);
    ///< Send a message on the CAN bus.  Opens the msg.channel if necessary.
    ///< Implementation of method required by AmbInterfaceBus.
    
    virtual void shutdown();
    ///< Shut down and disconenct the bus.  Implementation of method required by AmbInterfaceBus.

protected:
    ChannelNodeMap channelNodeMap_m;
    ///< The map of channels to nodes and the state of which channels are open. Shared with derived bus classes. 

private:    
    virtual bool openChannel(AmbChannel channel) = 0;
    ///< derived bus class must initialize a CAN interface channel.
      
    virtual void closeChannel(AmbChannel channel) = 0;
    ///< derived bus class must shut down a CAN interface channel.

    virtual void monitorImpl(unsigned long handle, AmbMessage_t &msg) = 0;  
    // Called by worker thread.  Performs the actual work of requesting 
    // and recieving a monitor point on the CAN bus.
    
    virtual void commandImpl(unsigned long handle, AmbMessage_t &msg) = 0;
    // Called by worker thread.  Performs the actual work of sending
    // a command on the CAN bus.
    
private:    
    // forbid copy construct, assignment:
    CANBusInterface(const CANBusInterface &other);
    CANBusInterface &operator =(const CANBusInterface &other);
    
    static void *queueHandlerThread(CANBusInterface *owner);
    ///< The function which the worker thread runs.
    
    AmbQueue queue_m;       ///< The message queue.
    pthread_t thread_m;     ///< handle for the worker thread.   
    bool dieNow;            ///< True signals the worker thread to die.
    bool deadNow;           ///< True when death complete.
};


#endif /*CANBUSINTERFACE_H_*/
