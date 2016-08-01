#ifndef AMBINTERFACE_H
#define AMBINTERFACE_H
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007 
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
 * This file defines the ambInterface class.  This is a singleton class
 * which is the interface between the Obj Oriented world of user space
 * and the CAN bus driver.  It is intended to ease porting of user code
 * from Windows to Linux.
 *----------------------------------------------------------------------
 */

#include "ambDefs.h"

// declare interfaces to which bus implementation classes must conform:
class AmbInterfaceBus {
public:
    AmbInterfaceBus()
      {}
    virtual ~AmbInterfaceBus()
      {}

    virtual void sendMessage(const AmbMessage_t &msg) = 0;
    
    virtual void shutdown() = 0;
};

class AmbInterface {
public:
    static const AmbInterface* getInstance();
    static void deleteInstance();

    static void setBus(AmbInterfaceBus *bus)
      { canBus_mp = bus; }

    void sendMessage(const AmbMessage_t& msg) const;

    AmbErrorCode_t findSN(unsigned char   serialNumber[],
                          AmbChannel      channel,
                          AmbNodeAddr     nodeAddress,
                          Time&           timestamp) const;

    AmbErrorCode_t flush(AmbChannel       channel,
                         Time             flushTime,
                         Time&            timestamp) const
                         { return AMBERR_BADCMD; }

    AmbErrorCode_t flush(AmbChannel      channel,
                         AmbNodeAddr     nodeAddress,
                         Time            flushTime,
                         Time&           timestamp) const
                         { return AMBERR_BADCMD; }

    AmbErrorCode_t flush(AmbChannel      channel,
                         AmbNodeAddr     nodeAddress,
                         AmbRelativeAddr RCA,
                         Time            flushTime,
                         Time&           timestamp) const
                         { return AMBERR_BADCMD; }


private:
   /* Constructor and Destructor are private because 
    * this is a singleton class
    */  
    AmbInterface();
    ~AmbInterface();

   /** 
    * This method is really part of the Constructor, but is moved to a 
    * subroutine to force the initialization in the correct order
    */
    void staticConstructor();

    /* Prevent use of the copy and assignment operator */
    AmbInterface(const AmbInterface&);
    AmbInterface& operator= (const AmbInterface&);

    // Force all monitor and command activity to stop:
    void shutdown();

    // The underlying CAN bus interface:  
    static AmbInterfaceBus *canBus_mp;

    // Static data for implementing singleton behavior:
    static AmbInterface*    instance_mp;
    static pthread_mutex_t  instanceLock_m;
    static sem_t            semCount_m; 
};

inline AmbAddr createAMBAddr(const AmbNodeAddr& nodeAddress,
                             const AmbRelativeAddr& relativeAddress) {
    return (nodeAddress + 1) * 0x40000 + relativeAddress;
    //return ((nodeAddress + 1) << 18) | relativeAddress;
}

#endif /*!AMBINTERFACE_H*/
