#include "protocols.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void sender(int*);
void receiver(int*);
void f();

int main() {
    int fd1[2], fd2[2];
    if(pipe(fd1)<0) {
        cout << "Pipe can not be created." << endl;
        exit(1);
    }
    if(pipe(fd2)<0) {
        cout << "Pipe can not be created." << endl;
        exit(1);
    }
    int *keysForSender = new int[2];
    int *keysForReceiver = new int[2];

    keysForSender[0] = fd1[0];
    keysForSender[1] = fd2[1];
    keysForReceiver[0] = fd2[0];
    keysForReceiver[1] = fd1[1];

    int pid = fork();
    if(pid<0) {
        cout << "Fork failed." << endl;
        exit(1);
    }
    else if(pid>0) {
        /* PARENT PROCESS --> Sender */
        sender(keysForSender);
        wait(NULL);
    }
    else {
        /* CHILD PROCESS --> Receiver */
        receiver(keysForReceiver);
    }

    return 0;
}

void sender(int *keys) {
    Byte nextFrame = 0;
    while(1) {
        Packet *pkt = fromNetworkLayer();
        if(pkt==0) continue;
        Frame *frame = new Frame(nextFrame, 0, *pkt);
        if(frame!=0) {
            toPhysicalLayer(frame, keys[1]);
            int event = waitForEvent(frame, keys[0]);
            if(event>0) {
                cout << ("[S] Frame [" + to_string(frame->seq) + "] Acknowledged") << '\n';
                delete pkt;
                delete frame;
                if(nextFrame==0) nextFrame++;
                else nextFrame = 0;
            }
            else {
                if(event==0) {
                    cout << ("[S] Frame [" + to_string(frame->seq) + "] Not Acknowledged") << '\n';
                }
                else {
                    cout << ("[S] Timer Expired for Frame [" + to_string(frame->seq) + "]")  << '\n';
                }
                while(waitForEvent(frame, keys[0])<0) {
                    toPhysicalLayer(frame, keys[1]);
                }
                delete pkt;
                delete frame;
                nextFrame = (++nextFrame)%2;
            }
        }
        f();
    }
}

void receiver(int *keys) {
    Byte frameExpected = 0;
    while(1) {
        Frame *frame = fromPhysicalLayer(keys[0]);
        if(frame!=0) {
            if(frame->seq==frameExpected) {
                cout << ("[R] Frame [" + to_string(frameExpected) + "] Received") << '\n';
                Packet *pkt = new Packet(frame->pkt);
                toNetworkLayer(pkt);
                Frame *ackframe = new Frame(0, frameExpected, *(new Packet()));
                toPhysicalLayer(ackframe, keys[1]);
                frameExpected = (++frameExpected)%2;
            }
        }
        f();
    }
}

void f() {
    int tmp = time(0);
    for(int i=0; i<1000000; i++) {
        long long tmp2 = tmp*tmp*tmp;
        long long tmp3 = tmp2*tmp2*tmp2;
    }
}