#ifndef INCLUDE_FRONTENDAMB_MESSAGEPACKUNPACK_H_
#define INCLUDE_FRONTENDAMB_MESSAGEPACKUNPACK_H_
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

// pack/unpack support between CAN payloads and data types:
#include "ambDefs.h"
#include "femcDefs.h"

class MessagePackUnpack {
public:
    virtual FEMC_ERROR unpack(unsigned long &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< unpack a CAN payload into an unsigned long.  Return value is the FEStatus byte.
    virtual void pack(unsigned long value, AmbDataLength_t &dataLength, AmbDataMem_t *data);
    ///< pack an unsigned long into a CAN payload.

    virtual FEMC_ERROR unpack(unsigned short &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< unpack a CAN payload into an unsigned short.  Return value is the FEStatus byte.
    virtual void pack(unsigned short value, AmbDataLength_t &dataLength, AmbDataMem_t *data);
    ///< pack an unsigned short into a CAN payload.

    virtual FEMC_ERROR unpack(short &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< unpack a 2s complement CAN payload into a signed short.  Return value is the FEStatus byte.
    virtual void pack(short value, AmbDataLength_t &dataLength, AmbDataMem_t *data);
    ///< pack a signed short into a 2s complement CAN payload.

    virtual FEMC_ERROR unpack(unsigned char &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< unpack a CAN payload into an unsigned char.  Return value is the FEStatus byte.
    virtual void pack(unsigned char value, AmbDataLength_t &dataLength, AmbDataMem_t *data);
    ///< pack an unsigned char into a CAN payload.

    virtual FEMC_ERROR unpack(bool &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
        unsigned char tmp;
        FEMC_ERROR ret = unpack(tmp, dataLength, data);
        target = (tmp) ? true : false;
        return ret;
    }
    ///< unpack an unsigned char CAN payload into a bool.  Return value is the FEStatus byte.

    virtual void pack(bool value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
        unsigned char tmp = (value) ? 1 : 0;
        pack(tmp, dataLength, data);
    }
    ///< pack an bool into a CAN payload as unsigned char.

    virtual FEMC_ERROR unpack(float &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< unpack a CAN payload into a float.  Return value is the FEStatus byte.
    virtual void pack(float value, AmbDataLength_t &dataLength, AmbDataMem_t *data);
    ///< pack a float into a CAN payload.

    virtual FEMC_ERROR unpack(std::string &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< unpack a CAN payload into an std::string.  Return value is the FEStatus byte.

    virtual FEMC_ERROR unpack(AmbDataMem_t *target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
    ///< copy a CAN payload into an array of bytes.  Returns FEMC_UNPACK_ERROR on error, else FEMC_NO_ERROR.
};
#endif /* INCLUDE_FRONTENDAMB_MESSAGEPACKUNPACK_H_ */
