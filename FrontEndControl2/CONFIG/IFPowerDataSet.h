#ifndef IFPOWERDATASET_H_
#define IFPOWERDATASET_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2012
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

#include <vector>

class IFPowerDataSet : public std::vector<float> {
public:
    IFPowerDataSet()
      : std::vector<float>(16, float(0.0))
      {}

    enum Field {
        PHOT0_IF0,
        PHOT0_IF1,
        PHOT0_IF2,
        PHOT0_IF3,
        PHOT15_IF0,
        PHOT15_IF1,
        PHOT15_IF2,
        PHOT15_IF3,
        PCOLD_IF0,
        PCOLD_IF1,
        PCOLD_IF2,
        PCOLD_IF3,
        YFACTOR_IF0,
        YFACTOR_IF1,
        YFACTOR_IF2,
        YFACTOR_IF3
    };
};


#endif /* IFPOWERDATASET_H_ */
