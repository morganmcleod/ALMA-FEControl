#ifndef INCLUDE_TRIMSTR_H_
#define INCLUDE_TRIMSTR_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2019
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

#include <string>

inline std::string TrimStr(const std::string& src, const std::string& c = " \t\r\n") {
    int p2 = src.find_last_not_of(c);
    if (p2 == std::string::npos) return std::string();
    int p1 = src.find_first_not_of(c);
    if (p1 == std::string::npos) p1 = 0;
    return src.substr(p1, (p2-p1)+1);
}

#endif /* INCLUDE_TRIMSTR_H_ */
