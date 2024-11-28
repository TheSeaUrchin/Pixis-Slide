#ifndef MIDIQUEUE_H
#define MIDIQUEUE_H

#include <Arduino.h>

struct midiNode {
    uint8_t midiBuf[4];
    midiNode *next;
};
class MidiQueue {
    public:
        midiNode *head = (midiNode*)malloc(sizeof(midiNode));
        midiNode *tail = head;
        int size = 0;

        MidiQueue(){};
        void add(uint8_t buf[4]);
        bool pop(uint8_t buf[4]);
        void printQueue();

        
};






#endif