#ifndef AMBDEVICEIMPL_H_
#define AMBDEVICEIMPL_H_

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
* "@(#) $Id: $"
*
*/

/************************************************************************
 * class AmbDeviceImpl is a do-nothing wrapper around AmbDeviceInt.
 * I'm not sure if AmbDeviceImpl should have any role on Windows
 * as it does on the Linux ABM.
 * 
 *----------------------------------------------------------------------
 */

#include "ambDeviceInt.h"

class AmbDeviceImpl : public AmbDeviceInt {
public:
    AmbDeviceImpl(const std::string& name);
    virtual ~AmbDeviceImpl();

protected:
    std::string name_m;
};


#endif /*AMBDEVICEIMPL_H_*/
