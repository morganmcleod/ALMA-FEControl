#ifndef OPTIMIZEBASE_H_
#define OPTIMIZEBASE_H_
/*******************************************************************************
* ALMA - Atacama Large Millimeter Array
* (c) Associated Universities Inc., 2007
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
/// \brief A generic base class which defines a common interface and worker thread for optimization procedures.

#include "FEMCEventQueue.h"
#include <string>

class OptimizeBase {
public:
    OptimizeBase(const char *name = "");
    ///< construct with an object name to use for logging

    virtual ~OptimizeBase();
    ///< destructor forces worker thread to stop.

    bool busy() const
      { return data_m.start_m; }
    ///< true if the thread was started and it has not finished or or been stopped.

    bool finished() const;
    ///< true if setFinished() has been called by the derived class' optimizeAction()
    
    bool success() const;
    ///< true if setFinished(success=true) has been called by the derived class' optimizeAction()
     
    virtual void requestStop();
    ///< main thread can call this to request worker thread to stop.

    bool stopRequested() const
      { return data_m.stop_m; }
    ///< main or worker thread can use this to check whether the thread has been told to stop.

protected:
    const std::string &logDir_m;///< directory to use for logs and results files.
    
    /// Data structure shared between the main thread and the worker thread:
    /// worker accesses it through owner->data_m or via helper functions.
    struct optimizeData {
        bool start_m;           ///< main thread sets this to true to cause initialization.
        bool stop_m;            ///< main thread sets this to true to signal worker thread to stop.
        bool stopped_m;         ///< worker thread sets this to true before exiting.
        bool finished_m;        ///< worker thread sets this to true when the procedure completes.
        bool success_m;         ///< if finished_m is true, this indicates success or failure.
        mutable pthread_mutex_t lock_m;///< mutex to protect this shared data.
        
        optimizeData()
          { reset();
            pthread_mutex_init(&lock_m, NULL); }
            
        ~optimizeData()
          { pthread_mutex_destroy(&lock_m); }

        void reset() 
          { start_m = stop_m = stopped_m = finished_m = success_m = false; }
           
    } data_m;   ///< single instance of shared data structure.

    bool startWorkerThread();
    ///< start the worker thread.  Call from derived class after all preconditions are set.

    virtual void optimizeAction() = 0;
    ///< derived classes must declare an optimizeAction method for the worker thread to call.

    virtual void exitAction(bool success)
      {}
    ///< derived classes may declare an exitAction which gets called just prior to the thread exiting.
    ///< whatever value is passed to setFinished(bool) by optimizeAction() is passed in here as success.

    void setFinished(bool success = true);
    ///< derived class calls to report that the optimization procedure is finished.

// event reporting interface for derived classes to use:

    void setEvent(short eventCode, 
                  short band = 0, 
                  short pol = -1,
                  short param = 0,
                  short progress = 0, 
                  const std::string &message = std::string())
      { FEMCEventQueue::addEvent(FEMCEventQueue::Event(eventCode, band, pol, param, progress, message)); }
    ///< send an event to the client via the FEMCEventQueue.

    void setStatusMessage(bool success, const std::string &message)
      { FEMCEventQueue::addStatusMessage(success, message); }
    ///< send a status message event to the client via FEMCEventQueue

    void setProgress(short progress) 
      { setEvent(FEMCEventQueue::EVENT_PROGRESS, 0, -1, 0, progress); } 
    ///< helper for reporting progress.
    
private:
    OptimizeBase(const OptimizeBase &other);
    ///< forbid copy constructor
    
    OptimizeBase &operator =(const OptimizeBase &other);
    ///< forbid assignment

    std::string name_m;             ///< the name of this OptimizeBase object.

    pthread_t thread_m;             ///< the worker thread handle.

    bool stopWorkerThread();
    ///< force the worker thread to stop.  Only called from destructor so private.
    
    static void *optimizeThread(OptimizeBase *owner);
    ///< the worker thread runner function.  Calls derived class' optimizeAction().
};

#endif /*OPTIMIZEBASE_H_*/
