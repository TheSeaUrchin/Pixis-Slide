#include <Arduino.h>

#ifndef CHANNELS_H
#define CHANNELS_H

#define NUM_CHANNELS 9
#define MIXER_CHANNEL (NUM_CHANNELS - 1) //Main output channel

typedef u_int8_t (*wave)(int,u_int8_t,int,int);




//Channel struct definitions
enum noteState{
  ATTACKING, 
  DECAYING,
  RELEASING,
  INACTIVE //becomes inactive when note release is finished
};
typedef struct{
  int freq;
  float amp;
  int duty;
  wave *waveform;
  int currentTime;
  float shiftCounter;
  // int pitchBend;
  noteState state;
  int output;
  int key;
  float relAmp;
} Oscillator;

typedef struct{
  int attack;
  int decay;
  int sustain;
  int release;
} Envelope;

typedef struct{
  int freq;
  int amp;
  int duty;
  int waveform;
} LFO;

typedef struct{
  int waveform;
  int ampMax;
  int pitchBend;
  int duty;
} GlobalVals;

typedef struct{
  int numNotes;
  int spacing;
  int mode;
  int currentVal;
  int currentOut;
} Strip;

// typedef struct {
//   Ocillator carrier;
//   Ocillator modulator;
//   Envelope envelope;
//   int prevTime;
//   float shiftCounter;
//   int pitchBend;
//   noteState state;
//   int output;
//   int key;

// } Channel;



#endif