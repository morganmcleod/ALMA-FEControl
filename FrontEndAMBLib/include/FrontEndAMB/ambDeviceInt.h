#ifndef FE_CONTROL_AMBDEVICEINT_H
#define FE_CONTROL_AMBDEVICEINT_H
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
* "@(#) $Id: ambDeviceInt.h,v 1.3 2005/07/15 21:34:46 mmcleod Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* mmcleod  2005-7-5   modified for Windows
*/

/************************************************************************
 * This is the baseclass for device drivers.  It defines the class AmbDeviceInt
 * which provides methods for monitor and control interactions with the Amb
 * as well as providing the interface which AmbDevIO uses.  This is a pure
 * virtual class, as there is no way to specify the channel and address
 * thus the constructor is protected.
 * 
 * This version:  Minimally modified by MMcLeod for porting to Windows.
 * The low-level Front End control interface will be developed first as
 * a Windows DLL to be called from LabVIEW.   The goal of this development
 * is to get an early start on the software for FE control which will be
 * required in both the FE integration center and in the ABM.   The core
 * of the resulting code should be readily portable back to Linux.
 * 
 * The interface layering scheme for both platforms is as follows:
 * 
 *              Windows                             Linux
 *              -------                             -----
 *                                  |
 *  Test system LabVIEW software    |   ABM software
 * ---------------------------------+-----------------------------------   
 *  Windows DLL interface           |   ABM wrapper 
 *  (lv_wrapper.cpp)                |   to be developed by computing
 * ---------------------------------+-----------------------------------
 * 
 *              Common code implementing FE low-level control.
 *              (FECartridgeBias, FEMonitorComtrol, etc.)
 * 
 * ---------------------------------+-----------------------------------
 *  ambDeviceInt.h - Windows        |   ambDeviceInt.h - Linux
 *  (this file)                     |   (as provided by computing)
 * ---------------------------------+-----------------------------------
 *  Windows implementation          |   Linux implementation
 *  (AmbInterface, etc.)            |
 * ---------------------------------+-----------------------------------
 *  NI-CAN driver - Windows         |   Linux CAN driver
 * ---------------------------------+-----------------------------------
 * 
 *                               CAN bus
 * 
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "ambDefs.h"
#include <pthread.h>            // On Windows we are using Pthreads-w32,
#include <semaphore.h>          // a Windows port of Posix threads.

class AmbInterface;

// The public class declaration is identical to the Linux version, with one
// exception:  Each of the functions is declared pure here with "= 0" appended.

class AmbDeviceInt {
 public:
  virtual void monitor(AmbRelativeAddr  RCA,
		       AmbDataLength_t& dataLength, 
		       AmbDataMem_t*    data,
		       sem_t*           synchLock,
		       Time*            timestamp,
		       AmbErrorCode_t*  status);
  
  virtual void command(AmbRelativeAddr     RCA,
		       AmbDataLength_t     dataLength,
		       const AmbDataMem_t* data,
		       sem_t*              synchLock,
		       Time*               timestamp,
		       AmbErrorCode_t*     status);

  virtual void monitorTE(ACS::Time        TimeEvent,
			 AmbRelativeAddr  RCA,
			 AmbDataLength_t& dataLength, 
			 AmbDataMem_t*    data,
			 sem_t*           synchLock,
			 Time*            timestamp,
			 AmbErrorCode_t*  status);

  virtual void commandTE(ACS::Time           TimeEvent,
			 AmbRelativeAddr     RCA,
			 AmbDataLength_t     dataLength, 
			 const AmbDataMem_t* data,
			 sem_t*              synchLock,
			 Time*               timestamp,
			 AmbErrorCode_t*     status);

  virtual void monitorNextTE(AmbRelativeAddr     RCA,
			     AmbDataLength_t&    dataLength, 
			     AmbDataMem_t*       data,
			     sem_t*              synchLock,
			     Time*               timestamp,
			     AmbErrorCode_t*     status);

  virtual void commandNextTE(AmbRelativeAddr RCA,
			     AmbDataLength_t            dataLength, 
			     const AmbDataMem_t*        data,
			     sem_t*                     synchLock,
			     ACS::Time*                 timestamp,
			     AmbErrorCode_t*            status);

  virtual void flushNode(ACS::Time                      TimeEvent,
             ACS::Time*                     timestamp,
             AmbErrorCode_t*                status);

  virtual void flushRCA(ACS::Time                      TimeEvent,
            AmbRelativeAddr                RCA,
            ACS::Time*                     timestamp,
            AmbErrorCode_t*                status);
  
 protected:
  AmbDeviceInt();
  virtual ~AmbDeviceInt();

  AmbChannel             m_channel;
  AmbNodeAddr            m_nodeAddress;
  const AmbInterface*    interface_mp;
};

#endif /*!FE_CONTROL_AMBDEVICEINT_H*/
