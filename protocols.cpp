#include "protocols.h"
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>



Packet* fromNetworkLayer() {
    Packet *pkt = new Packet();
    if(pkt!=0) {
        for(int i=0; i<PKT_LEN; i++)
            pkt->data[i] = rand()%256;
    }

    return pkt;
}

void toNetworkLayer(Packet *pkt) {
    if(pkt!=0) {
        memset(pkt, 0, PKT_LEN);
        delete pkt;
    }
}

Frame* fromPhysicalLayer(int readKey) {
    Frame *frame = new Frame();
    if(frame!=0) {
        int bytesRead = read(readKey, frame, sizeof(frame));
        if(bytesRead!=sizeof(frame)) return 0;
    }
    return frame;
}

void toPhysicalLayer(Frame *frame, int writeKey) {
    if(frame!=0) {
        write(writeKey, frame, sizeof(frame));
    }
}

int waitForEvent(Frame *frame, int readKey) {
    int t = time(0);
    while(time(0)-t < 3) {
        Frame *frm = fromPhysicalLayer(readKey);
        if(frm!=0) {
            if(frm->ack==frame->seq) return 1;
            else return 0;
        }
    }
    return -1;
}