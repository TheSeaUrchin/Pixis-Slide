#pragma once
#include <Arduino.h>
u_int8_t sinWave(int freq, u_int8_t amplitude,int duty, int prevTime);
u_int8_t sawtooth(int freq, u_int8_t amplitude,int duty, int prevTime);
u_int8_t triangle(int freq, u_int8_t amplitude,int duty, int prevTime);
u_int8_t squareWave(int freq, u_int8_t amplitude,int duty, int prevTime);

enum waveforms{
    SQUARE,
    TRIANGLE,
    SAWTOOTH,
    SINE
};