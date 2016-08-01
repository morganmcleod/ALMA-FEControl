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

/************************************************************************
 *----------------------------------------------------------------------
 */
 
#include "ambQueue.h"

void AmbQueue::flush() {
    pthread_mutex_lock(&mutex);
    AmbMessage_t msg;
    while (!empty()) {
        // Pop each command off the queue and ignore:
        msg = front();
        pop_front();
        // Signal the caller that the command is flushed:
        if (msg.completion_p -> status_p)
            *(msg.completion_p -> status_p) = AMBERR_FLUSHED;   // TODO: is this the right error code?
        if (msg.completion_p -> synchLock_p)
            sem_post(msg.completion_p -> synchLock_p);
        // Destroy the caller's completion structure since at this point nobody else will:
        delete msg.completion_p;
    }
    pthread_mutex_unlock(&mutex);
}    

void AmbQueue::append(const AmbMessage_t& msg) {
    pthread_mutex_lock(&mutex);
    push_back(msg);
    pthread_mutex_unlock(&mutex);
}

bool AmbQueue::getNext(AmbMessage_t& msg) {
    bool ret = false;
    pthread_mutex_lock(&mutex);
    if (!empty()) {
        msg = front();
        pop_front();
        ret = true;
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

