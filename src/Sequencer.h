#include <Arduino.h>
typedef struct{
    uint8_t data[4];
    int time;
}MIDI;

typedef struct{
    int numNotes;
    MIDI* midi;
}Song;


//song structure
MIDI AxelFMidi[] = {
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 0},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 499},
{ .data = {0x9, 0x92, 0x44, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x44, 0x00}, .time = 374},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 124},
{ .data = {0x9, 0x92, 0x46, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x46, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x3F, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x3F, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 499},
{ .data = {0x9, 0x92, 0x48, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x48, 0x00}, .time = 374},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 124},
{ .data = {0x9, 0x92, 0x49, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x49, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x48, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x48, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x44, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x44, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x48, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x48, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x4D, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x4D, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 124},
{ .data = {0x9, 0x92, 0x3F, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x3F, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x3F, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x3F, 0x00}, .time = 124},
{ .data = {0x9, 0x92, 0x3C, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x3C, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x43, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x43, 0x00}, .time = 249},
{ .data = {0x9, 0x92, 0x41, 0x50}, .time = 1},
{ .data = {0x8, 0x82, 0x41, 0x00}, .time = 1249}
};
Song AxelF = {46 , AxelFMidi};



int MIDITime = 0;
int midiNote = 0;
void beginSequence(){
    MIDITime = millis();
    midiNote = 0;
}

bool sequenceNext(){
    return (midiNote < AxelF.numNotes && millis() - MIDITime > AxelFMidi[midiNote].time);
}
void sequence(uint8_t* dataptr){
    for(int i = 0; i < 4; i++){
        dataptr[i] = AxelFMidi[midiNote].data[i];
    }
    MIDITime = millis();
    midiNote++;

}