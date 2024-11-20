#include <Arduino.h>

#ifndef CHANNELS_H
#define CHANNELS_H
typedef u_int8_t (*wave)(int,u_int8_t,float,int);
typedef u_int8_t (*calc)();


//Channel struct definitions

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
} Envelope;

typedef struct {
  Ocillator carrier;
  Ocillator modulator;
  Envelope envelope;
  int prevTime;
  float shiftCounter;
} Channel;

#endif