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

#include "messagePackUnpack.h"
#include "logger.h"
#include <iomanip>
using namespace std;

// Pack and unpack functions for unsigned long:

typedef union { // A union for unsigned long/can conversion
    unsigned char chr_val[4];
    unsigned long long_val;
} AmbULConv;

FEMC_ERROR MessagePackUnpack::unpack(unsigned long &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1 || dataLength == 5)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 4)
        ret = FEMC_UNPACK_ERROR;
    else {
        for (int index = 0; index < dataLength; ++index) {
            target *= 0xFF;
            target += data[index];
        }
    }
    return ret;
}

void MessagePackUnpack::pack(unsigned long value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    AmbULConv conv;
    conv.long_val = value;
    dataLength = 4;
    data[0] = conv.chr_val[3];
    data[1] = conv.chr_val[2];
    data[2] = conv.chr_val[1];
    data[3] = conv.chr_val[0];
}

// Pack and unpack functions for unsigned short:

FEMC_ERROR MessagePackUnpack::unpack(unsigned short &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1 || dataLength == 3)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 2)
        ret = FEMC_UNPACK_ERROR;
    else {
        target = (data[0] & 0xff) << 8;
        target += data[1];
    }
    return ret;
}

void MessagePackUnpack::pack(unsigned short value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    dataLength = 2;
    data[0] = (value >> 8) & 0xff;  //shift and set upper byte
    data[1] = value & 0xff;         //set lower byte
}

// Pack and unpack functions for signed short:

FEMC_ERROR MessagePackUnpack::unpack(short &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1 || dataLength == 3)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 2)
        ret = FEMC_UNPACK_ERROR;
    else {
        unsigned short temp = data[0] & 0xFF;
        temp <<= 8;
        temp += data[1];
        if (data[0] & 0x80) {
            // negative 2s complement number:
            temp -= 1;
            temp ^= 0xFFFF;
            target = (short) -temp;
        } else
            target = (short) temp;
        LOG(LM_DEBUG) << "unpack(short &target) data="
                      << uppercase << hex << setw(2) << setfill('0') << (int) data[0] << " " << (int) data[1] << dec << setw(0)
                      << " temp=" << temp << " target=" << target << endl;
    }
    return ret;
}

void MessagePackUnpack::pack(short value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    dataLength = 2;
    unsigned short temp = (unsigned short) (value);
    if (value < 0) {
        // Convert to negative 2s compliment:
        temp = (unsigned short) (-value);
        temp ^= 0xFFFF;
        temp += 1;
    }
    data[0] = (temp >> 8) & 0xff;  //shift and set upper byte
    data[1] = temp & 0xff;         //set lower byte
}


// Pack and unpack functions for unsigned char:

FEMC_ERROR MessagePackUnpack::unpack(unsigned char &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0;
    if (dataLength == 1) {
        // Ambiguous whether this is just the requested data or just the status byte.
        // Treat it as a status byte if it matches the range of FEMC_ERROR, otherwise as data:
        target = data[0];
        signed char retC((signed char) target);
        if (retC < 0 && retC >= FEMC_LAST_ERROR_CODE) {
            target = 0;
            ret = (FEMC_ERROR) retC;
        }
    } else if (dataLength == 2) {
        ret = (FEMC_ERROR) ((signed char) data[dataLength - 1]);
        if (ret == FEMC_NO_ERROR)
            target = data[0];
    } else
        ret = FEMC_UNPACK_ERROR;
    return ret;
}

void MessagePackUnpack::pack(unsigned char value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    dataLength = 1;
    data[0] = value;
}

// Pack and unpack functions for float:

typedef union { // A union for float/can conversion
    unsigned char chr_val[4];
    float flt_val;
} AmbFloatConv;

FEMC_ERROR MessagePackUnpack::unpack(float &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    FEMC_ERROR ret(FEMC_NO_ERROR);
    target = 0.0;
    if (dataLength == 1 || dataLength == 5)
        ret = (FEMC_ERROR) ((signed char) data[--dataLength]);
    if (dataLength < 4)
        ret = FEMC_UNPACK_ERROR;
    else {
        // Convert to float the incoming CAN message:
        AmbFloatConv conv;
        conv.chr_val[0] = data[3];
        conv.chr_val[1] = data[2];
        conv.chr_val[2] = data[1];
        conv.chr_val[3] = data[0];
        target = conv.flt_val;
    }
    return ret;
}

void MessagePackUnpack::pack(float value, AmbDataLength_t &dataLength, AmbDataMem_t *data) {
    /* Set a float value to the CAN message*/
    AmbFloatConv conv;
    conv.flt_val = value;

    dataLength = 4;
    data[0] = conv.chr_val[3];
    data[1] = conv.chr_val[2];
    data[2] = conv.chr_val[1];
    data[3] = conv.chr_val[0];
}

// unpack function for std::string.  No pack is required for this type:
FEMC_ERROR MessagePackUnpack::unpack(std::string &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    target = "";
    for (int index = 0; index < dataLength; ++index)
        target += (char) data[index];
    return FEMC_NO_ERROR;
}

// unpack function to copy a CAN payload into an array of bytes. No pack is required for this type:
FEMC_ERROR MessagePackUnpack::unpack(AmbDataMem_t *target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    if (!target)
        return FEMC_UNPACK_ERROR;
    memcpy(target, data, dataLength);
    return FEMC_NO_ERROR;
}

