#include <Arduino.h>

#ifndef CHANNELS_H
#define CHANNELS_H

#define NUM_CHANNELS 9
#define MIXER_CHANNEL (NUM_CHANNELS - 1) //Main output channel

typedef u_int8_t (*wave)(int,u_int8_t,float,int);




//Channel struct definitions
enum noteState{
  ON, //becomes on when key is hit and assigned to channel
  OFF, //becomes off when physical note is released and remains on while the sound is releasing
  INACTIVE //becomes inactive when note release is finished
};
typedef struct{
  int freq;
  u_int8_t amp;
  float duty;
  wave waveform;
} Ocillator;

typedef struct{
  int attack;
  int decay;
  int sustain;
  int release;
  int ampMax;
} Envelope;

typedef struct {
  Ocillator carrier;
  Ocillator modulator;
  Envelope envelope;
  int prevTime;
  float shiftCounter;
  int pitchBend;
  noteState state;
  int output;

} Channel;

#endif