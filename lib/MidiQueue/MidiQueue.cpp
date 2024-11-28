#include <MidiQueue.h>

void MidiQueue::add(uint8_t buf[4]){
    for(int i = 0; i < 4; i++){
        tail -> midiBuf[i] = buf[i];
    }
    size++;
    midiNode *newNode = (midiNode*)malloc(sizeof(midiNode));
    tail -> next = newNode;
    tail = newNode;

}

bool MidiQueue::pop(uint8_t buf[4]){
    if(size == 0){
        return false;
    }
    for(int i = 0; i < 4; i++){
        buf[i] = head ->midiBuf[i];
    }
    midiNode *prevHead = head;
    head = head ->next;
    size--;
    free(prevHead);
    return true;
}

void MidiQueue::printQueue(){
    Serial.println(size);
    midiNode *node = head;
    for(int i = 0; i < size; i++){
        for(int j = 0; j<4; j++){
            Serial.print(node -> midiBuf[i]);
        }
        Serial.println("");
        node = node -> next;
    }
}