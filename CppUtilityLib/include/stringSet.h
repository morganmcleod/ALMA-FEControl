#ifndef INCLUDE_STRINGSET_H_
#define INCLUDE_STRINGSET_H_
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

#include <set>
#include <string>

class StringSet : public std::set<std::string> {
    typedef std::set<std::string> base_type;
public:
    StringSet()
      : base_type()
        {}

    virtual ~StringSet() {
    }

    typedef base_type::iterator iterator;               ///< declare iterator types
    typedef base_type::const_iterator const_iterator;

private:
};

#endif /* INCLUDE_STRINGSET_H_ */
