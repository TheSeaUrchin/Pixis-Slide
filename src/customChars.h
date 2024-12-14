#include <Arduino.h>

enum waveNames{
    SQUARE,
    TRIANGLE,
    SAWTOOTH,
    SINE
};

byte sine1[] = {
  0b00001,
  0b00010,
  0b00100,
  0b00100,
  0b01000,
  0b01000,
  0b10000,
  0b10000
};

byte sine2[] = {
  0b10000,
  0b01000,
  0b00100,
  0b00100,
  0b00010,
  0b00010,
  0b00001,
  0b00001
};


byte sine3[] = {
  0b10000,
  0b10000,
  0b01000,
  0b01000,
  0b00100,
  0b00100,
  0b00010,
  0b00001
};

byte sine4[] = {
  0b00001,
  0b00001,
  0b00010,
  0b00010,
  0b00100,
  0b00100,
  0b01000,
  0b10000
};
byte* sineChars[4] = {sine1,sine2,sine3,sine4};

byte square1[] = {
  0b11111,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000
};

byte square2[] = {
  0b11111,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001
};


byte square3[] = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b11111
};




byte square4[] = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b11111
};

byte* squareChars[4] = {square1,square2,square3,square4};

byte saw1[] = {
  0x0C,
  0x0A,
  0x09,
  0x08,
  0x08,
  0x08,
  0x08,
  0x08
};

byte saw2[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x10,
  0x08,
  0x04,
  0x02
};

byte saw3[] = {
  0x10,
  0x08,
  0x04,
  0x02,
  0x01,
  0x00,
  0x00,
  0x00
};

byte saw4[] = {
  0x04,
  0x04,
  0x04,
  0x04,
  0x04,
  0x04,
  0x14,
  0x0C
};

byte* sawChars[4] = {saw1,saw2,saw3,saw4};

byte tri1[] = {
  0x02,
  0x02,
  0x04,
  0x04,
  0x08,
  0x08,
  0x10,
  0x10
};

byte tri2[] = {
  0x08,
  0x08,
  0x04,
  0x04,
  0x02,
  0x02,
  0x01,
  0x01
};

byte tri3[] = {
  0x10,
  0x10,
  0x08,
  0x08,
  0x04,
  0x04,
  0x02,
  0x02
};

byte tri4[] = {
  0x01,
  0x01,
  0x02,
  0x02,
  0x04,
  0x04,
  0x08,
  0x08
};

byte* triChars[4] = {tri1,tri2,tri3,tri4};

typedef struct{
    byte** graphBytes;
}Graph;

Graph graphs[4] = {
    {squareChars},
    {triChars},
    {sawChars},
    {sineChars}

};


//Bars

byte bar1[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    31
};
byte bar2[] = {
    0,
    0,
    0,
    0,
    0,
    31,
    31,
    31
};
byte bar3[] = {
    0,
    0,
    0,
    0,
    31,
    31,
    31,
    31
};
byte bar4[] = {
    0,
    0,
    31,
    31,
    31,
    31,
    31,
    31
};

byte* barChars[4] = {bar1,bar2,bar3,bar4};


