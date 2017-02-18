#include "FEMCEventQueue.h"
#include "logger.h"
using namespace std;

//---------------------------------------------------------------------------
// Static Member Definitions:
FEMCEventQueue* FEMCEventQueue::instance_mp = NULL;
pthread_mutex_t FEMCEventQueue::instanceLock_m = PTHREAD_MUTEX_INITIALIZER;


//---------------------------------------------------------------------------
// create and destroy singleton instance:
void FEMCEventQueue::createInstance() {
// Create the singleton instance of the event queue.
    pthread_mutex_lock(&instanceLock_m);
    if (instance_mp == NULL)
        instance_mp = new FEMCEventQueue();
    pthread_mutex_unlock(&instanceLock_m);
}

void FEMCEventQueue::destroyInstance() {
// Destroy the singleton instance of the event queue.
    pthread_mutex_lock(&instanceLock_m);
    delete instance_mp;
    instance_mp = NULL;
    pthread_mutex_unlock(&instanceLock_m);
}

//---------------------------------------------------------------------------
// private constructor and desctructor:

FEMCEventQueue::FEMCEventQueue()
  : nextSeq_m(1),       // initialize next sequence number to 1
    subcribers_m(0)     // no subscribers.
{
    pthread_mutex_init(&queueLock_m, NULL);
    LOG(LM_INFO) << "FEMCEventQueue starting." << endl;
}

FEMCEventQueue::~FEMCEventQueue(){
    pthread_mutex_destroy(&queueLock_m);
    LOG(LM_INFO) << "FEMCEventQueue stopped." << endl;
}

//---------------------------------------------------------------------------
// implementation:

void FEMCEventQueue::Event::print() const {
    LOG(LM_INFO) << "FEMCEvent: seq=" << seq_m << " code=" << eventCode_m << " band=" << band_m << " pol=" << pol_m
        << " param=" << param_m << " progress=" << progress_m << " '" << message_m << "'" << std::endl;
}

void FEMCEventQueue::subscribe(bool doSubscribe) {
    if (!instance_mp)
        return;
    bool wasNone(instance_mp -> subcribers_m == 0);
    instance_mp -> subcribers_m += doSubscribe ? 1 : -1;
    if (instance_mp -> subcribers_m > 0 and wasNone)
        LOG(LM_INFO) << "FEMCEventQueue: got first subscriber." << endl;
    else if (instance_mp -> subcribers_m <= 0)
        LOG(LM_INFO) << "FEMCEventQueue: no more subscribers." << endl;
}

void FEMCEventQueue::addEvent(const Event &source) {
    if (!instance_mp)
        return;

    // Make a copy of the source event:
    Event copy(source);

    // Lock the instance object:
    pthread_mutex_lock(&(instance_mp -> queueLock_m));

    // Assign the next sequence number and increment:
    copy.seq_m = instance_mp -> nextSeq_m;
    instance_mp -> nextSeq_m++;

    // only add it to the queue if there are subscribers:
    if (instance_mp -> subcribers_m > 0)
        instance_mp -> queue_m.push_back(copy);

    // Unlock the instance object:
    pthread_mutex_unlock(&(instance_mp -> queueLock_m));

    // Log all events except NONE and PROGRESS:
    if (copy.eventCode_m > EVENT_PROGRESS)
        copy.print();
}
    
bool FEMCEventQueue::getNextEvent(Event &target) {
    if (!instance_mp)
        return false;
    target.reset();
    bool ret = false;
    pthread_mutex_lock(&(instance_mp -> queueLock_m));
    if (!instance_mp -> queue_m.empty()) {
        target = instance_mp -> queue_m.front();
        instance_mp -> queue_m.pop_front();
        ret = true;
    }
    pthread_mutex_unlock(&(instance_mp -> queueLock_m));
    return ret; 
}

bool FEMCEventQueue::getNextEventSeqNo(unsigned long &target) {
    target = 0;
    if (!instance_mp)
        return false;
    pthread_mutex_lock(&(instance_mp -> queueLock_m));
    target = instance_mp -> nextSeq_m;
    pthread_mutex_unlock(&(instance_mp -> queueLock_m));
    return true;
}
