#pragma once

#include <Arduino.h>
#include <ESP32Encoder.h> 

void setupUI();
void updateUI();
#define NUM_SCREENS 3
enum ItemType{
  GRAPH,
  BAR,
  NONE
};

struct Item{
  ItemType itemType;
  int x;
  int y;
  String caption;
  ESP32Encoder *encoder;
  int prevEncoderVal;
  int value;
  int *valueBind;
  int min;
  int max;
  int currentEncoderCnt = 0;
};


typedef struct {
  ItemType Type[4];
  String caption[4];
  int numElements;
  int *vars[4];
  int min[4] = {0,0,0,0};
  int max[4];
}Screen;



extern Item items[4];

extern Screen LFOscr;
extern Screen OSCscr;
extern Screen ENVscr;

void setupUI();
void updateUI();