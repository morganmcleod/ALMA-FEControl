#ifndef CANCHANNELNODEMAP_H_
#define CANCHANNELNODEMAP_H_
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
#include "NodeList.h"
#include <vector>

/// A data structure with an entry for each CAN channel:
/// Whether it is open or closed, and an opaque 'handle' for referencing the channel in the underlying network driver.
/// Also for each channel, a list of CAN nodes seen.
class ChannelNodeMap {
public:

    ChannelNodeMap(int maxChannels)
      : channels(maxChannels)
        { pthread_mutex_init(&mutex_m, NULL); }
    
    ~ChannelNodeMap()
      { closeAllChannels();
        pthread_mutex_destroy(&mutex_m); }

    bool isOpenChannel(AmbChannel channel) const;
    ///< check whether the specified channel is already open.

    void openChannel(AmbChannel channel);
    ///< mark the specified CAN channel as open.  No effect if already open.

    void closeChannel(AmbChannel channel);
    ///< close the specified CAN channel.  No effect if already closed.

    void closeAllChannels();
    ///< close all CAN channels.

    bool getFirstOpenChannel(AmbChannel &channel) const;
    ///< get the channel number for the first open channel.
    ///< returns false if no channels are open.
    
    unsigned long getHandle(AmbChannel channel) const;
    ///< get the object handle for a channel.
    
    void setHandle(AmbChannel channel, unsigned long handle);
    ///< set the object handle for a channel.

    void clearNodes(AmbChannel channel);
    ///< drop all known CAN nodes for the given channel.
    
    void addNode(AmbChannel channel, AmbNodeAddr node, unsigned char *serialNumber);
    ///< add a node to the given channel.
    
    const nodeList_t &getNodes(AmbChannel channel) const
      { return channels[channel].nodes; }
    ///< peek at the list of nodes on the channel.

private:
    // forbid copy construct and assignment:
    ChannelNodeMap(const ChannelNodeMap &other);
    ChannelNodeMap &operator =(const ChannelNodeMap &other);

    mutable pthread_mutex_t mutex_m;  ///< Mutex to protect access to the data structures.

    /// Define a structure for holding the state of each can controller channel:
    struct ChannelState {
        nodeList_t nodes;       ///< Vector of CAN nodes on the channel.
        unsigned long objH;     ///< The handle to for the channel.
        bool isOpen;            ///< True if the channel has been successfully opened.

        ChannelState()
          : objH(0),
            isOpen(false)
            {}
    };
    std::vector<ChannelState> channels;  ///< The list of channel state structures.
};

#endif /*CANCHANNELNODEMAP_H_*/
