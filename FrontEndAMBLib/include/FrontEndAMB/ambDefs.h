#ifndef AMBDEFS_H
#define AMBDEFS_H
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
* "@(#) $Id: ambDefs.h,v 1.8 2005/07/12 06:30:52 jkern Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* jkern  2003-12-17  created
* mmcleod 2006-04-14 copied and modified for Windows usage.
*/

/************************************************************************
 * brief Modified from the file of the same name from the CAmbServer 
 * project implementing the real time server.
 * 
 * On windows we share a lot of the same typedefs and data structures
 * but of course there is no realtime kernel module handling things.
 *----------------------------------------------------------------------
 */

#include "timeDef.h"
#include <pthread.h>
#include <semaphore.h>
#include <string>

#define AMB_DATA_MSG_SIZE 8
typedef unsigned long AmbChannel;
typedef unsigned long AmbNodeAddr;
typedef unsigned long AmbNodeType;  
typedef unsigned long AmbRelativeAddr;
typedef unsigned long AmbAddr;
typedef unsigned long long Time;
typedef unsigned short AmbDataLength_t;
typedef unsigned char AmbDataMem_t;

/// ACS::Time is declared here to aid portability to Linux/ACS.
namespace ACS {
    typedef unsigned long long Time;
};



typedef enum {
    AMB_MONITOR,
    AMB_MONITOR_NEXT,
    AMB_CONTROL,
    AMB_CONTROL_NEXT,
    AMB_RESET,
    AMB_GET_NODES,
    AMB_GET_NO_CHANNELS,
    AMB_SHUTDOWN,
    AMB_FLUSH,
    AMB_FLUSH_NODE,
    AMB_FLUSH_RCA,
    AMB_BLOCK_READ,
    AMB_BLOCK_REQUEST
} AmbTransaction_t;

typedef enum {
    AMBERR_NOERR,
    AMBERR_CONTINUE,
    AMBERR_BADCMD,
    AMBERR_BADCHAN,
    AMBERR_UNKNOWNDEV,
    AMBERR_INITFAILED,
    AMBERR_WRITEERR,
    AMBERR_READERR,
    AMBERR_FLUSHED,
    AMBERR_TIMEOUT,
    AMBERR_RESPFIFO,  // not able to put received AMB message in response fifo
    AMBERR_NOMEM,     // Unable to allocate memory to receive message
    AMBERR_PENDING,   // Designats that the value has not been filled yet
    AMBERR_ADDRERR    // There was an error in the address.
} AmbErrorCode_t;

/* Node types that may be in a CAN bus */
typedef enum {
    UNKNOWN_NODE_TYPE = 0,	// Older correlator firmware and AMB do not support node types
    MIN_NODE_TYPE   = 0x0f, // Non-inclusive lower bound of slave types
    LTA             = 0x10,	// Long term accumulator card
    SCC             = 0x11,	// Station control card
    QCC             = 0x12,	// Quadrant control card
    FINAL_ADDER     = 0x13,	// Final adder card
    HPDI            = 0x14,	// Data port interface card
    SCC_TESTFIX     = 0x15,	// SCC test fixture -- not found in real correlator
    DTS_RCVR        = 0x16,	// DTS receiver
    MAX_NODE_TYPE   = 0x17  // Non-inclusive upper bound of slave types
} AmbNodeType_t;

  /**
   * This is a structure which stores the memory location to load the
   * data into when a message completes.  Not used by kernel module but
   * passed through for simplicity
   */
typedef struct {
    AmbDataLength_t* dataLength_p; // Length of returned data
    AmbDataMem_t*    data_p;       // Returned Data
    AmbChannel*      channel_p;    // Channel response comes from
    AmbAddr*         address_p;    // Address response comes from
    Time*            timestamp_p;  // Time Command was executed
    AmbNodeType*     type_p;	   // Type of node responding 
    AmbErrorCode_t*  status_p;     // Status Value
    sem_t*           synchLock_p;  // Semaphore for blocking processes
    sem_t*           contLock_p;   // Semaphore for Extended read
} AmbCompletion_t;

  /**
   * This is passed back to the users space process when a
   * operation has completed
   */

typedef struct {
    AmbCompletion_t* completion_p; //   Completion Address
    AmbChannel       channel;      //  Channel Address
    AmbAddr          address;      //  AMB Address
    AmbNodeType	     type;	   //  Type of node responding (when applicable, ignored otherwise)
    AmbErrorCode_t   status;       //  Status flag 0 if successful
    AmbDataLength_t  dataLen;      //  Length of returned data
    AmbDataMem_t     data[AMB_DATA_MSG_SIZE];      //  The Returned data
    Time             timestamp;    //  Timestamp of completion
    unsigned int     magic;        // Magic word to Verify Structure is complete
} AmbResponse_t;

  /**
   * This is the request sent from user space to be executed
   */

typedef struct {
    AmbTransaction_t   requestType;  //  Transaction Type
    AmbChannel         channel;      //  Channel Address
    AmbAddr            address;   //  AMB Address
    AmbDataLength_t    dataLen;      //  Length of commanded data
    AmbDataMem_t       data[AMB_DATA_MSG_SIZE];      //  Data to transmit
    AmbCompletion_t*   completion_p; //  Response Address
    Time               targetTE;     // Requested execution time
} AmbMessage_t;

#endif /*!AMBDEFS_H*/
