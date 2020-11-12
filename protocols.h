#ifndef PROTOCOL_DLL
#define PROTOCOL_DLL

#define PKT_LEN 8 

typedef unsigned char Byte;

struct Packet {
    Byte data[PKT_LEN];
};

struct Frame {
    Byte seq, ack;
    Packet pkt;

    Frame() {}

    Frame(Byte seq, Byte ack, Packet &pkt) {
        this->seq = seq;
        this->ack = ack;
        this->pkt = pkt;
    }
};

Packet* fromNetworkLayer();
void toNetworkLayer(Packet *pkt);

void toPhysicalLayer(Frame *frame, int writeKey);
Frame* fromPhysicalLayer(int readKey);

int waitForEvent(Frame *frame, int readKey);

#endif