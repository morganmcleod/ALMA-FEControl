
#include <vector>
#include "FrontEndAMB/ambDefs.h"
#include "AMBDevice.h"


class Sequencer {
public:
    Sequencer(AMBDevice &ambDevice, AmbNodeAddr nodeAddr, int reserve = 500)
      : ambDevice_mp(&ambDevice),
        nodeAddr_m(nodeAddr),
        sequence_m()
    {   
        sequence_m.reserve(reserve);
        clear(); 
    } 

    struct Message {
        AmbRelativeAddr RCA;
        AmbDataLength_t dataLength;
        AmbDataMem_t data[8];
    };

    void add(const Message &msg) {
        sequence_m.push_back(msg);
    }

    void run(vector<Message> &replies, bool clearAfter = true);

    void clear() {
        sequence_m.clear();
    }

private:
    AMBDevice *ambDevice_mp;
    AmbNodeAddr nodeAddr_m;
    vector<Message> sequence_m;
};

void Sequencer::run(vector<Message> &replies, bool clearAfter) {
    Message reply;

    replies.clear();
    for (vector<Message>::const_iterator it = sequence_m.begin(); it != sequence_m.end(); it++) {
        const Message &msg = *it;
        if (msg.dataLength) {
            ambDevice_mp -> command(nodeAddr_m, msg.RCA, msg.dataLength, msg.data);
        } else {
            ambDevice_mp -> monitor(nodeAddr_m, msg.RCA, reply.dataLength, reply.data);
            reply.RCA = msg.RCA;
            replies.push_back(reply);
        }
    }
}
