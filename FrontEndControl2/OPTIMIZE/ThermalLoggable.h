#ifndef ThermalLoggable_H_
#define ThermalLoggable_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2008
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

/// \file
/// \brief Abstract interface for things which the ThermalLogger can call logging functions on.

#include <string>

/// Abstract interface for things which the ThermalLogger can call logging functions on.
class ThermalLoggable {
public:
    ThermalLoggable()
      {}
    virtual ~ThermalLoggable()
      {}

    virtual void appendThermalLog(std::string &target) const = 0;
    ///< append thermal information to a logging string

    virtual void appendThermalLogHeader(std::string &target) const = 0;
    ///< append thermal header information to a logging string
};

#endif /*ThermalLoggable_H_*/
