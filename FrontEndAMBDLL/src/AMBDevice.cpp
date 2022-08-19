/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2022
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
*
*/

#include "AMBDevice.h"
#include <iomanip>

AmbErrorCode_t AMBDevice::command(AmbNodeAddr nodeAddr,
                                  AmbRelativeAddr RCA,
                                  AmbDataLength_t dataLength,
                                  const AmbDataMem_t *data)
{
    // We are treating the ambDeviceInt like a bus interface rather than a single CAN device.
    // So cache the current node address
    AmbNodeAddr tempNodeAddr = AmbDeviceInt::m_nodeAddress;
    AmbDeviceInt::m_nodeAddress = nodeAddr;

    AmbErrorCode_t status(AMBERR_NOERR);
    Time timestamp;
    // send command with a semaphore:
    sem_t synchLock;
    sem_init(&synchLock, 0, 0);
    AmbDeviceInt::command(RCA, dataLength, data, &synchLock, &timestamp, &status);
    // restore the previous node address:
    AmbDeviceInt::m_nodeAddress = tempNodeAddr;
    // wait on the semaphore:
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    return status;
}

AmbErrorCode_t AMBDevice::monitor(AmbNodeAddr nodeAddr,
                                  AmbRelativeAddr RCA,
                                  AmbDataLength_t &dataLength,
                                  AmbDataMem_t *data)
{
    // We are treating the ambDeviceInt like a bus interface rather than a single CAN device.
    // So cache the current node address
    AmbNodeAddr tempNodeAddr = AmbDeviceInt::m_nodeAddress;
    AmbDeviceInt::m_nodeAddress = nodeAddr;

    AmbErrorCode_t status(AMBERR_NOERR);
    Time timestamp;
    // send monitor request with the semaphore:
    sem_t synchLock;
    sem_init(&synchLock, 0, 0);
    AmbDeviceInt::monitor(RCA, dataLength, data, &synchLock, &timestamp, &status);
    // restore the previous node address:
    AmbDeviceInt::m_nodeAddress = tempNodeAddr;
    // wait on the semaphore:
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    // check for errors and return result:
    if (status != AMBERR_NOERR) {
        errorCount_m++;
        if (logAmbErrors_m)
            LOG(LM_ERROR) << "FEHardwareDevice(0x" << std::uppercase << std::hex << m_nodeAddress << "): AMB error=" << status << " RCA=" << " 0x" << std::uppercase << std::hex << std::setw(6) << std::setfill('0') << RCA << std::endl;
    }
    return status;
}





