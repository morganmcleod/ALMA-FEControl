#ifndef SRC_AMBDEVICE_H_
#define SRC_AMBDEVICE_H_
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

#include "FrontEndAMB/ambDeviceInt.h"
#include "FrontEndAMB/femcDefs.h"
#include "logger.h"
using namespace std;

class AMBDevice : public AmbDeviceInt {
public:
    AMBDevice(AmbChannel channel, AmbNodeAddr nodeAddress, const AmbInterface* interface_p)
      : logAmbErrors_m(true),
        errorCount_m(0)
    {
        AmbDeviceInt::m_channel = channel;
        AmbDeviceInt::m_nodeAddress = nodeAddress;
        AmbDeviceInt::interface_mp = interface_p;
    }

    using AmbDeviceInt::command;
    AmbErrorCode_t command(AmbNodeAddr nodeAddr,
                           AmbRelativeAddr RCA,
                           AmbDataLength_t dataLength,
                           const AmbDataMem_t *data);

    using AmbDeviceInt::monitor;
    AmbErrorCode_t monitor(AmbNodeAddr nodeAddr,
                           AmbRelativeAddr RCA,
                           AmbDataLength_t &dataLength,
                           AmbDataMem_t *data);

private:
    AMBDevice(const AMBDevice &other);
    ///< forbid copy construct.

    inline bool monitorIgnorableError(FEMC_ERROR &err)
      { return (err == FEMC_NO_ERROR || err == FEMC_HARDW_UPDATE_WARN || err == FEMC_HARDW_RETRY_WARN); }
    ///< returns true if the data associated with the error can perhaps be used anyway.

    bool logAmbErrors_m;
    unsigned long errorCount_m;
};

#endif /* SRC_AMBDEVICE_H_ */
