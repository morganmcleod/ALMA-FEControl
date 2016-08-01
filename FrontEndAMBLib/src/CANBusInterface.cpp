/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
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

#include "CANBusInterface.h"
#include "setTimeStamp.h"
#include <stdio.h>
using namespace std;

int CANBusInterface::maxChannels_m = 6;
// Max number of CAN channels to support simultaneously. 

bool CANBusInterface::enableDebug_m = false;
// Enables debug logging of CAN traffic to stdout.

bool CANBusInterface::enableDebugLifecycle_m = false;
///< Enables debug logging of startup/shutdown to stdout.  Default is false.

bool CANBusInterface::noTransmit_m = false;
// If true, all messages are timed out immediately rather than sent.
    
int CANBusInterface::monitorTimeout_m = 1500;
// Timeout in ms while waiting for CAN responses.
// TODO: This should be controlled by whether we are working with debug firmware or not.

CANBusInterface::CANBusInterface()
  : channelNodeMap_m(maxChannels_m),
    dieNow(false),
    deadNow(false)
{
    if (enableDebugLifecycle_m)
        printf("Creating CANBusInterface\n");
    // Create the worker thread, passing this as the thread argument:
    pthread_create(&thread_m, NULL, reinterpret_cast<void*(*)(void*)> (queueHandlerThread), this);
    pthread_detach(thread_m);
}

CANBusInterface::~CANBusInterface() {
    if (enableDebugLifecycle_m)
        printf("Destroying CANBusInterface\n");
}

void CANBusInterface::sendMessage(const AmbMessage_t& msg) {
    bool doSend = true;
    // Before enqueueing the message, check whether we are shutting down:    
    if (dieNow) {
        printf("CANBusInterface::sendMessage no send in shutting down state\n");
        doSend = false;
    }
    
    // Not shutting down.  Is the channel open?
    else if (!channelNodeMap_m.isOpenChannel(msg.channel))
        // no. Try opening the channel:
        if (!openChannel(msg.channel)) {
            doSend = false; // Opening channel failed.
            printf("CANBusInterface::sendMessage cannot open channel to send\n");
        }
    
    // If not sending...
    if (!doSend) {
        // Reject the command and signal the caller that we're done with the message:
        if (msg.completion_p -> status_p)
            *(msg.completion_p -> status_p) = AMBERR_INITFAILED;    // TODO: is this the right error code?
        if (msg.completion_p -> synchLock_p)                    
            sem_post(msg.completion_p -> synchLock_p);
        // Destroy the caller's completion structure:
        delete msg.completion_p;
        return;
    }
    // At this point it's safe to queue the command for sending:
    queue_m.append(msg);
}

void CANBusInterface::shutdown() {
    if (enableDebugLifecycle_m)
        printf("CANBusInterface::shutdown()...\n");

    // Tell the worker thread to die:
    dieNow = true;

    // Spin until the thread sets deadNow to true:
    while (!deadNow)
        SLEEP(20);

    // Flush the message queue:
    queue_m.flush();
    
    // Close all open channels:
    AmbChannel channel;
    while (channelNodeMap_m.getFirstOpenChannel(channel))
        closeChannel(channel);

    if (enableDebugLifecycle_m)
        printf("CANBusInterface::shutdown() finished\n");

}

// --------------------------------------------------------------------------
//private:

// The worker thread function pulls commands out of the queue and dispatches them:                                        
void *CANBusInterface::queueHandlerThread(CANBusInterface *owner) {
    if (!owner)
        return NULL;

    if (enableDebugLifecycle_m)
        printf("CANBusInterface queueHandlerThread started\n");

    AmbMessage_t msg;               // place to copy the next queue item.
    unsigned long channelHandle;    // handle to the channel to use.
    bool handleItem;                // flag to indicate a message was pulled from queue.
    
    // Infinite loop until the thread is told to die:
    while (true) {

        handleItem = false;
        channelHandle = 0;
        
        // Check whether the thread has been told to die:
        if (owner -> dieNow) {
            if (enableDebugLifecycle_m)
                printf("CANBusInterface queueHandlerThread stopping\n");
            // Signal that we're dead now and exit:
            owner -> deadNow = true;
            pthread_exit(NULL);
        }
        // Oh goody, we're still alive.  See if there's anything in the queue:
        handleItem = owner -> queue_m.getNext(msg);
        if (handleItem)
            channelHandle = owner -> channelNodeMap_m.getHandle(msg.channel);    
        
        // If nothing to do, sleep a millisecond:   
        if (!handleItem) {
            // Might be a bit much for "real time" but it will do for Windows.
            SLEEP(1);
        
        // Dispatch the message:
        } else {        
            if (!noTransmit_m) {            
                if (msg.requestType == AMB_MONITOR || msg.requestType == AMB_MONITOR_NEXT) 
                    owner -> monitorImpl(channelHandle, msg);
                else if (msg.requestType == AMB_CONTROL || msg.requestType == AMB_CONTROL_NEXT)
                    owner -> commandImpl(channelHandle, msg);
            } else {
                // Debugging code to treat all commands as timed out:
                if (msg.completion_p -> dataLength_p)
                    *(msg.completion_p -> dataLength_p) = 0;
                if (msg.completion_p -> data_p)
                    memset(msg.completion_p -> data_p, 0, AMB_DATA_MSG_SIZE);
                if (msg.completion_p -> status_p)
                    *(msg.completion_p -> status_p) = AMBERR_TIMEOUT;
            }
                
            // Set the time stamp completed:
            setTimeStamp(msg.completion_p -> timestamp_p);
            // Signal the caller that the request completed:
            if (msg.completion_p -> synchLock_p)
                sem_post(msg.completion_p -> synchLock_p);
            // Destroy the caller's completion structure:
            delete msg.completion_p;
        }
    }
}
