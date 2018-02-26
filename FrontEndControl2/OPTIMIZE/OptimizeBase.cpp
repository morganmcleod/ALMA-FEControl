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

#include "OptimizeBase.h"
#include "logger.h"
#include <LOGGER/logDir.h>
using namespace std;

OptimizeBase::OptimizeBase(const char *name)
  : logDir_m(FEConfig::getLogDir()),
    name_m(name)
    {} 

OptimizeBase::~OptimizeBase() {
    stopWorkerThread();
}

bool OptimizeBase::finished() const {
    pthread_mutex_lock(&data_m.lock_m);
    bool ret = data_m.finished_m;
    pthread_mutex_unlock(&data_m.lock_m);
    return ret;
}

bool OptimizeBase::success() const {
    pthread_mutex_lock(&data_m.lock_m);
    bool ret = data_m.success_m;
    pthread_mutex_unlock(&data_m.lock_m);
    return ret;
}

void OptimizeBase::requestStop() {
    data_m.stop_m = true;
}

//protected:

bool OptimizeBase::startWorkerThread() {
    LOG(LM_INFO) << "OptimizeBase(" << name_m << "): starting worker thread..." << endl;
    data_m.reset();    
    data_m.start_m = true;
    pthread_create(&thread_m, NULL, reinterpret_cast<void*(*)(void*)>(optimizeThread), this);
    pthread_detach(thread_m);
    return true;
}

void OptimizeBase::setFinished(bool success) {
    pthread_mutex_lock(&data_m.lock_m);
    data_m.success_m = success;
    data_m.finished_m = true;
    data_m.start_m = false;
    pthread_mutex_unlock(&data_m.lock_m);
}

bool OptimizeBase::stopWorkerThread() {
    if (!data_m.start_m)
        return true;
    if (data_m.stopped_m)
        return true;
    LOG(LM_INFO) << "OptimizeBase(" << name_m << "): stopping worker thread..." << endl;
    int retry = 250;
    data_m.stop_m = true;
    do {
        SLEEP(100);
    } while (!data_m.stopped_m && retry--);
    if (!data_m.stopped_m) {
        int elapsed = (250 - retry) / 10;
        LOG(LM_INFO) << "OptimizeBase(" << name_m << "): NOT stopped after " << elapsed << " seconds." << endl;
    }
    data_m.reset();
    return data_m.stopped_m;
}

void *OptimizeBase::optimizeThread(OptimizeBase *owner) {
    // refuse to run if not passed an owner pointer:
    if (!owner)
        return NULL;
    
    optimizeData &data_m = owner -> data_m;
    // TESTING:  try caching the name pointer to avoid access violation on exit.
    std::string name(owner -> name_m);
    
    bool done = false;
    while (!done) {
        // if told to stop exit the loop:
        if (data_m.stop_m)
            done = true;
        else {
            // If not stopped, call the derived class' optimizeAction:
            owner -> optimizeAction();
            // If the optimization is done, stop:
            if (owner -> finished()) {
                LOG(LM_INFO) << "OptimizeBase(" << name << "): optimizeAction finished." << endl;
                done = true;
            }
        }
    }
    // call the exitAction with the result indicating successful finish.
    owner -> exitAction(owner -> success());

    // time to stop; acknowledge with stopped_m and exit the thread:
    LOG(LM_DEBUG) << "OptimizeBase(" << name << "): exiting worker thread." << endl;
    pthread_mutex_lock(&data_m.lock_m);
    data_m.stopped_m = true;
    pthread_mutex_unlock(&data_m.lock_m);
    return NULL;
}

