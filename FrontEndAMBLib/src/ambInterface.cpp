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

/************************************************************************
*   NAME  
*     ambInterface
* 
*   SYNOPSIS
*
*     This singleton class provides the interface between user objects
*   the CAN bus driver.   Modified from the file of the same name in 
*   the CAmbServer package to work on Windows in a non- realtime way.
*   Removed all exception try/catch/throw.
*------------------------------------------------------------------------
*/

#include "ambInterface.h"
#include <pthread.h>
#include <set>
#include <stdio.h>

//---------------------------------------------------------------------------
// Static Member Definitions:
AmbInterfaceBus *AmbInterface::canBus_mp = NULL;

AmbInterface* AmbInterface::instance_mp = NULL;

pthread_mutex_t AmbInterface::instanceLock_m = PTHREAD_MUTEX_INITIALIZER;

sem_t AmbInterface::semCount_m; 


AmbInterface::AmbInterface() {
    staticConstructor();
}

/**
   This function is used to move the initialization of global variables
   out of the constructor.  This ensures that they are created before
   they are used.
   See: http://websqa.hq.eso.org/common/CodingStandards/manuals/ucs_30.htm
   for more info.
*/
void AmbInterface::staticConstructor() {
    /* Initialize the semaphore which counts references */
    printf("AmbInterface starting\n");
    if (sem_init(&semCount_m, 0, 0) != 0) {
    	printf("AmbInterface: Error initializing semaphore\n");
    }
    pthread_mutex_init(&instanceLock_m, NULL);
    pthread_mutex_lock(&instanceLock_m);
    canBus_mp = NULL;
    pthread_mutex_unlock(&instanceLock_m);
}

AmbInterface::~AmbInterface(){
    shutdown();
    canBus_mp = NULL;
    if (sem_destroy(&semCount_m) != 0) {
        printf("AmbInterface: Error destroying semaphore\n");
    }
    int status = pthread_mutex_destroy(&instanceLock_m); 
    if (status != 0) {
        printf("AmbInterface: Error destroying mutex\n");
    }
}

const AmbInterface* AmbInterface::getInstance() {
    pthread_mutex_lock(&instanceLock_m);
    if (instance_mp == NULL) {
        instance_mp = new AmbInterface();
    }
    sem_post(&semCount_m);
    int status = pthread_mutex_unlock(&instanceLock_m);
    return instance_mp;
}

void AmbInterface::deleteInstance() {
    int currentValue;

    pthread_mutex_lock(&instanceLock_m);

    if (sem_trywait(&semCount_m) == 0) {
        sem_getvalue(&semCount_m, &currentValue);
        if (currentValue == 0) {
            /* No clients left, delete the instance */
            pthread_mutex_unlock(&instanceLock_m);
            if (instance_mp) {
                delete instance_mp;
                instance_mp = NULL;
            }
            return;
        }
    }
    pthread_mutex_unlock(&instanceLock_m);
}


void AmbInterface::shutdown() {    
    if (canBus_mp)
        canBus_mp -> shutdown();

    printf("AmbInterface stopped.\n");
}


AmbErrorCode_t AmbInterface::findSN(AmbDataMem_t serialNumber[],
                                    AmbChannel channel,
                                    AmbNodeAddr nodeAddress,
                                    Time& timestamp) const 
{
  AmbCompletion_t* completion_p = new AmbCompletion_t;
  AmbMessage_t     message;

  AmbDataLength_t  dataLength;
  AmbErrorCode_t   status;
  sem_t            synchLock;
  sem_init(&synchLock, 0, 0);

  /* Build the completion block */
  completion_p->dataLength_p = &dataLength;
  completion_p->data_p       = serialNumber;
  completion_p->channel_p    = NULL;
  completion_p->address_p    = NULL;
  completion_p->timestamp_p  = &timestamp;
  completion_p->status_p     = &status;
  completion_p->synchLock_p  = &synchLock;
  completion_p->contLock_p   = NULL;
  completion_p->type_p       = NULL;

  /* Build the message block */
  message.requestType  = AMB_MONITOR;
  message.channel      = channel;
  message.address      = createAMBAddr(nodeAddress,0);
  message.dataLen      = 0;
  message.completion_p = completion_p;
  message.targetTE     = 0;

  /* Send the message and wait for a return */
  sendMessage(message);
  sem_wait(&synchLock);
  sem_destroy(&synchLock);

  return status;
}

void AmbInterface::sendMessage(const AmbMessage_t& msg) const {
    if (canBus_mp) 
        canBus_mp -> sendMessage(msg);
}


