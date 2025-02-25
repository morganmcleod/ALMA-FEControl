#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_

#include <string>
#include <deque>
#include <pthread.h>

class FEMCEventQueue {
public:
    static void createInstance();
    ///< Create the singleton instance of the event queue.
    ///< Since clients only insert and remove events and don't hold references, there's no need to do reference counting.

    static void destroyInstance();
    ///< Destroy the singleton instance of the event queue.

    enum EventCodes {
        EVENT_NONE              = 0,    // the do-nothing event
        EVENT_PROGRESS          = 1,    // report % progress of a background method 
        EVENT_STATUS_MESSAGE    = 2,    // report a status message to display to the user
        EVENT_IVCURVE_DONE      = 3,    // finished measuring I-V curve.  caller can collect the data.
        EVENT_PA_ADJUST_DONE    = 4,    // finished adjusting the PA drain voltage to get a target SIS current
        EVENT_GET_VJVD          = 5,    // tells the caller to request updated junction voltage and PA drain voltage settings.
        EVENT_GET_MAGNET        = 6,    // tells the caller to request updated magnet current settings.
        EVENT_REQUEST_IFPOWER   = 7,    // asks the caller to start providing IF power readings.
        EVENT_STOP_IFPOWER      = 8,    // asks the caller to stop IF power readings.
        EVENT_CRYO_PUMP_DONE    = 9,    // indicates that the cryo-pumping procedure has finished.
        EVENT_OPTIMIZE_DONE     = 10,   // indicates that an optimization-related measurement is complete.  caller can collect the data.
        EVENT_CHOPPER_MOVE      = 11,   // asks the caller to move the chopper to hot or cold position.
        EVENT_INITIALIZE        = 12,   // sent after successful connection and querying of FE state.
        EVENT_CARTHC_DONE       = 13,   // sent when all cart health check measurements have finished.
        EVENT_ALL_REPS_DONE     = 14    // sent when all requested IVCURVE or OPTIMIZE repetitions are complete.
    };

    struct Event {
        unsigned long seq_m;            ///< auto-incrementing sequence number.
        short eventCode_m;              ///< from EventCodes enum
        short band_m;                   ///< what band this event applies to
        short pol_m;                    ///< what pol channel this applies to
        short param_m;                  ///< additional paramter for the event
        short progress_m;               ///< progress bar percentage
        std::string message_m;          ///< status message for the user.
        
        Event(short eventCode = EVENT_NONE,
              short band = 0,
              short pol = -1,
              short param = 0,
              short progress = 0,
              const std::string &message = std::string())
          : seq_m(0),
            eventCode_m(eventCode),
            band_m(band),
            pol_m(pol),
            param_m(param),
            progress_m(progress),
            message_m(message)
            {}
        ///< default and in-place constructor.
            
        void reset() {
            seq_m = 0;
            eventCode_m = EVENT_NONE;
            band_m = pol_m = param_m = progress_m = 0;
            message_m.clear();
        }
        ///< reset event structure to default state
        
        void print() const;
        ///< log the event to the standard logger

    };

    static void subscribe(bool doSubscribe);
    ///< subscribe/unsubscribe to pull events from the queue.
    ///< if there are no subscribers then events are discarded rather than queued.

    static void addEvent(const Event &ev);
    ///< add an event to the queue.
    
    static void addStatusMessage(bool success, const std::string &message)
      { addEvent(Event(EVENT_STATUS_MESSAGE, 0, -1, (success ? 1 : 0), 0, message)); }
    ///< shorthand for adding a status message to the queue.
    
    static void addProgressEvent(short progress)
      { addEvent(Event(EVENT_PROGRESS, 0, -1, 0, progress)); }
    ///< shorthand for reporting progress.

    static bool getNextEvent(Event &target);
    ///< get the next event from the queue.
    
    static bool getNextEventSeqNo(unsigned long &target);
    ///< get the next sequence number the queue will assign.

private:
    // Constructor and Destructor are private because this is a singleton class
    FEMCEventQueue();
    ~FEMCEventQueue();

    // forbid copy and assignment
    FEMCEventQueue(const FEMCEventQueue&);
    FEMCEventQueue& operator= (const FEMCEventQueue&);

    // Static data for implementing singleton behavior:
    static FEMCEventQueue* instance_mp;
    static pthread_mutex_t instanceLock_m;
    
    // The queue and its mutex:
    std::deque<Event> queue_m;      ///< the queue implementation
    unsigned long nextSeq_m;        ///< the next sequence number to assign
    pthread_mutex_t queueLock_m;    ///< mutex to protect queue_m and nextSeq_m

    // the number of subscribers:
    int subcribers_m;
};

#endif /*EVENTQUEUE_H_*/
