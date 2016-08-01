#ifndef DS1820_H
#define DS1820_H

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

/************************************************************************
 * Helper to convert the output CAN message from the DS1820 temperature 
 * probe into degrees C.  Returns 0 if no error, -1 otherwise. 
 *----------------------------------------------------------------------
 */

#include "ambDefs.h"

int unpackDS1820Temperature(float &target, AmbDataLength_t dataLength, const AmbDataMem_t *data);
 
#endif
