#ifndef AMBQUEUE_H_
#define AMBQUEUE_H_
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

#include "ambDefs.h"
#include <list>

/// declare an STL base class for the queue:
typedef std::list<AmbMessage_t> AmbQueue_t;

/// AmbQueue is a thread-safe list of messages waiting to be sent on the CAN bus.
/// It stands in for the FIFOs used by the AmbServer and AmbInterface classes in the real ABM. 
class AmbQueue : public AmbQueue_t {

public:
    AmbQueue()
      { pthread_mutex_init(&mutex, NULL); }
    ~AmbQueue()
      { flush();
        pthread_mutex_destroy(&mutex); }    

    void flush();
        
    void append(const AmbMessage_t& msg);
    
    bool getNext(AmbMessage_t& msg);    
    
private:
    pthread_mutex_t mutex;  // Mutex to protect shared data.
};

#endif /*AMBQUEUE_H_*/
