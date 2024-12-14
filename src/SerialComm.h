#ifndef SERIAL_H
#define SERIAL_H
#include <Arduino.h>
#include "Channels.h"

#include <usbh_midi.h>
#include <usbhub.h>

//Status bits for channel voice messages:
#define NOTE_OFF 0x8
#define NOTE_ON  0x9
#define AFTERTOUCH 0xA
#define CONTROL_CHANGE 0xB
#define PROGRAM_CHANGE 0xC
#define CHANNEL_PRESSURE 0xD
#define PITCH_BEND 0xE
#define PACKET_SIZE 4

// Control Change Controller Numbers
#define MOD_WHEEL 0x01
#define DEBUG_BTN 0x71

//#define KEY_LOGGING //uncomment for key logging

// USB Usb;
// USBHub Hub(&Usb);
// USBH_MIDI  Midi(&Usb);


void setupTask();
void mainTask();
void getMidi();

extern Oscillator channels[9];
extern LFO lfo;
extern GlobalVals globalVals;

int noteToF(int note);





#endif