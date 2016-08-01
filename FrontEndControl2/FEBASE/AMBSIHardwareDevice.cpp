/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2011
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

/************************************************************************
 * intermediate class derived from FEHardwareDevice which declares
 *   and implements the mandatory AMBSI interface monitor points.
 *----------------------------------------------------------------------
 */

#include "AMBSIHardwareDevice.h"
#include "MonCtrlMacros.h"
#include <FrontEndAMB/ds1820.h>
using namespace std;

AMBSIHardwareDevice::AMBSIHardwareDevice(const std::string &name)
  : FEHardwareDevice(name),
    AMBSINumErrors_value(0),
    AMBSINumTransactions_value(0),
    AMBSITemperature_value(0)
{
}

std::string AMBSIHardwareDevice::AMBSISerialNum() {
    FEHardwareDevice::syncMonitorEightByteESN(AMBSI_SERIAL_NUMBER, AMBSISerialNum_value, false);
    return AMBSISerialNum_value;
}

std::string AMBSIHardwareDevice::AMBSIFirmwareRev() {
    FEHardwareDevice::syncMonitorThreeByteRevLevel(AMBSI_BASE + AMBSI_FIRMWARE_REV, AMBSIFirmwareRev_value);
    return AMBSIFirmwareRev_value;
}

unsigned long AMBSIHardwareDevice::AMBSINumErrors() {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(AMBSI_BASE + AMBSI_NUM_ERRORS, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        AMBSINumErrors_value = 0;
    else
        unpack(AMBSINumErrors_value, dataLength, data);
    return AMBSINumErrors_value;
}

unsigned long AMBSIHardwareDevice::AMBSINumTransactions() {
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(AMBSI_BASE + AMBSI_NUM_TRANSACTIONS, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        AMBSINumTransactions_value = 0;
    else
        unpack(AMBSINumTransactions_value, dataLength, data);
    return AMBSINumTransactions_value;
}

float AMBSIHardwareDevice::AMBSITemperature() {
    AMBSITemperature_value = 0;
    int feStatus = FEMC_NO_ERROR;
    AmbDataLength_t dataLength;
    AmbDataMem_t data[8];
    sem_t synchLock;
    Time timestamp;
    AmbErrorCode_t status = AMBERR_NOERR;
    sem_init(&synchLock, 0, 0);
    monitor(AMBSI_BASE + AMBSI_TEMPERATURE, dataLength, data, &synchLock, &timestamp, &status);
    sem_wait(&synchLock);
    sem_destroy(&synchLock);
    if (status != AMBERR_NOERR)
        feStatus = FEMC_UNPACK_ERROR;
    else if (0 != unpackDS1820Temperature(AMBSITemperature_value, dataLength, data))
        feStatus = FEMC_UNPACK_ERROR;
    return AMBSITemperature_value;
}

