/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2006
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

#include "ds1820.h"

static const int FEMC_UNPACK_ERROR = -9; 

int unpackDS1820Temperature(float &target, AmbDataLength_t dataLength, const AmbDataMem_t *data) {
    // Convert the data bytes from the DS1820 to a temperature in Celcius:
    // The least significant bit represents 1/2 degree.
    // The most significant byte, if nonzero indicates negative.
    // The bits in between contain the value, two's compliment if negative.
    
    target = 0.0;
    int ret = 0;
    if (dataLength == 1 || dataLength == 5)
        ret = data[--dataLength];
    if (dataLength != 4)
        ret = FEMC_UNPACK_ERROR;
    else {
        int valueBits = data[0] >> 1;
        if (data[1] != 0) {
            valueBits *= -1;
            valueBits += -1;
        }
        
        target = valueBits;
        bool add_half = (data[0]) & 0x01;
        if (add_half)
            target += 0.5;
    }
    return ret;
}
