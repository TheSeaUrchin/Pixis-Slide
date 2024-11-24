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

// USB Usb;
// USBHub Hub(&Usb);
// USBH_MIDI  Midi(&Usb);

typedef struct {
    Channel* channels;
    SemaphoreHandle_t xMutex;
}Params;
void setupTask(Params* params);
void mainTask(void *params);

extern SemaphoreHandle_t xMutex;
extern Channel channels[9];

#endif