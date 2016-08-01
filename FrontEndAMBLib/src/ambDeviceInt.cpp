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
* "@(#) $Id: ambDeviceInt.cpp,v 1.7 2005/07/19 20:23:11 jkern Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* jkern  2003-12-19  created 
*/

/************************************************************************
*    This is the baseclass for all AMB devices, the AMBDevIO class also
* makes use of this class.  This abstraction is the layer which interfaces
* directly to the Interface
*------------------------------------------------------------------------
*/

#include "ambDeviceInt.h"
#include "ambInterface.h"

static const char *rcsId="@(#) $Id: ambDeviceInt.cpp,v 1.7 2005/07/19 20:23:11 jkern Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

AmbDeviceInt::AmbDeviceInt(){
  interface_mp = AmbInterface::getInstance();
}

AmbDeviceInt::~AmbDeviceInt(){
  interface_mp->deleteInstance();
}

void AmbDeviceInt::monitor(AmbRelativeAddr  RCA,
               AmbDataLength_t& dataLength,
               AmbDataMem_t*    data,
               sem_t*           synchLock,
               Time*            timestamp,
               AmbErrorCode_t*  status) {
  return(monitorTE(0, RCA, dataLength, data, synchLock, timestamp, status));
}

void AmbDeviceInt::command(AmbRelativeAddr      RCA,
               AmbDataLength_t      dataLength,
               const AmbDataMem_t*  data,
               sem_t*               synchLock,
               Time*                timestamp,
               AmbErrorCode_t*      status) {
  return(commandTE(0, RCA, dataLength, data, synchLock, timestamp, status));
}

void AmbDeviceInt::monitorTE(ACS::Time        TimeEvent,
                 AmbRelativeAddr  RCA,
                 AmbDataLength_t& dataLength, 
                 AmbDataMem_t*    data,
                 sem_t*           synchLock,
                 Time*            timestamp,
                 AmbErrorCode_t*  status){

  /* Call to get a monitor point at a specific time*/
  AmbCompletion_t* completion = new AmbCompletion_t;
  AmbMessage_t     message;

  /* Build the completion block */
  completion->dataLength_p = &dataLength;
  completion->data_p       = data;
  completion->channel_p    = NULL;
  completion->address_p    = NULL;
  completion->timestamp_p  = timestamp;
  completion->status_p     = status;
  completion->synchLock_p  = synchLock;
  completion->contLock_p   = NULL;
  completion->type_p       = NULL;

  /* Build the message block */
  message.requestType  = AMB_MONITOR;
  message.channel      = m_channel;
  message.address      = createAMBAddr(m_nodeAddress,RCA);
  message.dataLen      = 0;
  message.completion_p = completion;
  message.targetTE     = TimeEvent;

  /* Send the message and wait for a return */
  interface_mp->sendMessage(message);
}  

void AmbDeviceInt::commandTE(ACS::Time           TimeEvent,
                 AmbRelativeAddr     RCA,
                 AmbDataLength_t     dataLength, 
                 const AmbDataMem_t* data,
                 sem_t*              synchLock,
                 Time*               timestamp,
                 AmbErrorCode_t*     status){

  AmbCompletion_t* completion = new AmbCompletion_t;
  AmbMessage_t     message;

  /* Build the completion block */
  completion->dataLength_p = NULL;
  completion->data_p       = NULL;
  completion->channel_p    = NULL;
  completion->address_p    = NULL;
  completion->timestamp_p  = timestamp;
  completion->status_p     = status;
  completion->synchLock_p  = synchLock;
  completion->contLock_p   = NULL;
  completion->type_p       = NULL;

  /* Build the message block */
  message.requestType  = AMB_CONTROL;
  message.channel      = m_channel;
  message.address      = createAMBAddr(m_nodeAddress, RCA);
  message.dataLen      = dataLength;
  for (int idx = 0; idx < message.dataLen; idx++)
    message.data[idx] = data[idx];
  message.completion_p = completion;
  message.targetTE     = TimeEvent;

  /* Send the message and wait for a return */
  interface_mp->sendMessage(message);
}

void AmbDeviceInt::monitorNextTE(AmbRelativeAddr     RCA,
                 AmbDataLength_t&    dataLength, 
                 AmbDataMem_t*       data,
                 sem_t*              synchLock,
                 Time*               timestamp,
                 AmbErrorCode_t*     status){


  /* Call to get a monitor point at a specific time*/
  AmbCompletion_t* completion = new AmbCompletion_t;
  AmbMessage_t     message;

  /* Build the completion block */
  completion->dataLength_p = &dataLength;
  completion->data_p       = data;
  completion->channel_p    = NULL;
  completion->address_p    = NULL;
  completion->timestamp_p  = timestamp;
  completion->status_p     = status;
  completion->synchLock_p  = synchLock;
  completion->contLock_p   = NULL;
  completion->type_p       = NULL;

  /* Build the message block */
  message.requestType  = AMB_MONITOR_NEXT;
  message.channel      = m_channel;
  message.address      = createAMBAddr(m_nodeAddress,RCA);
  message.dataLen      = 0;
  message.completion_p = completion;
  message.targetTE     = 0;

  /* Send the message and wait for a return */
  interface_mp->sendMessage(message);
}

void  AmbDeviceInt::commandNextTE(AmbRelativeAddr RCA,
                  AmbDataLength_t            dataLength, 
                  const AmbDataMem_t*        data,
                  sem_t*                     synchLock,
                  ACS::Time*                 timestamp,
                  AmbErrorCode_t*            status){
  AmbCompletion_t* completion = new AmbCompletion_t;
  AmbMessage_t     message;

  /* Build the completion block */
  completion->dataLength_p = NULL;
  completion->data_p       = NULL;
  completion->channel_p    = NULL;
  completion->address_p    = NULL;
  completion->timestamp_p  = timestamp;
  completion->status_p     = status;
  completion->synchLock_p  = synchLock;
  completion->contLock_p   = NULL;
  completion->type_p       = NULL;

  /* Build the message block */
  message.requestType  = AMB_CONTROL_NEXT;
  message.channel      = m_channel;
  message.address      = createAMBAddr(m_nodeAddress, RCA);
  message.dataLen      = dataLength;
  for (int idx = 0; idx < message.dataLen; idx++)
    message.data[idx] = data[idx];

  message.completion_p = completion;
  message.targetTE     = 0;

  /* Send the message and wait for a return */
  interface_mp->sendMessage(message);
}

void AmbDeviceInt::flushNode(ACS::Time                      TimeEvent,
                 ACS::Time*                     timestamp,
                 AmbErrorCode_t*                status){
  if (interface_mp == NULL) {
    *status = AMBERR_WRITEERR;
    return;
  }

  /* Send the message and wait for a return */
  *status = interface_mp->flush(m_channel, m_nodeAddress, TimeEvent, 
                *timestamp);
}

void AmbDeviceInt::flushRCA(ACS::Time                      TimeEvent,
                AmbRelativeAddr                RCA,
                ACS::Time*                     timestamp,
                AmbErrorCode_t*                status){
  if (interface_mp == NULL) {
    *status = AMBERR_WRITEERR;
    return;
  }

  /* Send the message and wait for a return */
  *status = interface_mp->flush(m_channel, m_nodeAddress, RCA, TimeEvent, 
                   *timestamp);
}



/*___oOo___*/
