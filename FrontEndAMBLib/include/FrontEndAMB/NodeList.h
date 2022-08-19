#ifndef INCLUDE_FRONTENDAMB_NODELIST_H_
#define INCLUDE_FRONTENDAMB_NODELIST_H_
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
*/

#include "ambDefs.h"
#include <vector>

/// Define a structure for containing the node addresses and serial numbers seen on a channel:
struct NodeInfo {
    AmbNodeAddr node;
    unsigned char serialNumber[8];

    NodeInfo(AmbNodeAddr _node = 0, const unsigned char *sn = NULL)
      {init(_node, sn); }

    void init(AmbNodeAddr _node, const unsigned char *sn = NULL)
      { node = _node;
        if (sn) memcpy(serialNumber, sn, 8);
        else memset(serialNumber, 0, 8); }
};
typedef std::vector<NodeInfo> nodeList_t;

#endif /* INCLUDE_FRONTENDAMB_NODELIST_H_ */
