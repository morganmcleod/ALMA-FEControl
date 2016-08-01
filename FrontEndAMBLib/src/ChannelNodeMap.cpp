/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2003 
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
#include "ChannelNodeMap.h"
#include "logger.h"
using namespace std;

bool ChannelNodeMap::isOpenChannel(AmbChannel channel) const {
    // This method is called for every message sent so there's no time to lock the mutex.
    // Should be ok because we're just checking a range and a boolean.
    return (channel < channels.size() && channels[channel].isOpen);
}

void ChannelNodeMap::openChannel(AmbChannel channel) {
    if (channel < channels.size()) {
        pthread_mutex_lock(&mutex_m);
        channels[channel].isOpen = true;
        pthread_mutex_unlock(&mutex_m);
    } else
        LOG(LM_ERROR) << "ChannelNodeMap::openChannel(" << channel << ") out of range." << endl;
}

void ChannelNodeMap::closeChannel(AmbChannel channel) {
    if (channel < channels.size()) {
        pthread_mutex_lock(&mutex_m);
        channels[channel].isOpen = false;
        channels[channel].nodes.clear();
        pthread_mutex_unlock(&mutex_m);
    } else
        LOG(LM_ERROR) << "ChannelNodeMap::closeChannel(" << channel << ") out of range." << endl;
}

bool ChannelNodeMap::getFirstOpenChannel(AmbChannel &channel) const {
    bool found = false;
    unsigned index = 0;
    pthread_mutex_lock(&mutex_m);
    while (!found && index < channels.size()) {
        if (channels[index].isOpen) {
            channel = index;
            found = true;
        }
        ++index;
    }
    pthread_mutex_unlock(&mutex_m);
    return found;
}

void ChannelNodeMap::closeAllChannels() {
    pthread_mutex_lock(&mutex_m);
    for (unsigned channel = 0; channel < channels.size(); ++channel) {
        channels[channel].isOpen = false;
        channels[channel].nodes.clear();
    }
    pthread_mutex_unlock(&mutex_m);
}

unsigned long ChannelNodeMap::getHandle(AmbChannel channel) const {
    unsigned long ret = 0;
    if (channel < channels.size()) {
        pthread_mutex_lock(&mutex_m);
        ret = channels[channel].objH;
        pthread_mutex_unlock(&mutex_m);
    } else
        LOG(LM_ERROR) << "ChannelNodeMap::getHandle(" << channel << ") out of range." << endl;
    return ret;
}

void ChannelNodeMap::setHandle(AmbChannel channel, unsigned long handle) {
    if (channel < channels.size()) {
        pthread_mutex_lock(&mutex_m);
        channels[channel].objH = handle;
        pthread_mutex_unlock(&mutex_m);
    } else
        LOG(LM_ERROR) << "ChannelNodeMap::setHandle(" << channel << ", handle) out of range." << endl;
}

void ChannelNodeMap::clearNodes(AmbChannel channel) {
    if (channel < channels.size()) {
        pthread_mutex_lock(&mutex_m);
        channels[channel].nodes.clear();
        pthread_mutex_unlock(&mutex_m);
    } else
        LOG(LM_ERROR) << "ChannelNodeMap::clearNodes(" << channel << ") out of range." << endl;
}

void ChannelNodeMap::addNode(AmbChannel channel, AmbNodeAddr node, unsigned char *serialNumber) {
    if (channel < channels.size()) {
        pthread_mutex_lock(&mutex_m);
        channels[channel].nodes.push_back(NodeInfo(node, serialNumber));
        pthread_mutex_unlock(&mutex_m);
    } else
        LOG(LM_ERROR) << "ChannelNodeMap::addNode(" << channel << ", node, serialNumber) out of range." << endl;
}



