#include "Channels.h"
#include "SerialComm.h"
#include "waveforms.h"
#include "UI.h"

//ADC setup
uint8_t adc_pins[] = {36};
uint8_t adc_pins_count = sizeof(adc_pins) / sizeof(uint8_t);
volatile bool adc_coversion_done = false;
adc_continuous_data_t *result = NULL;
int majorScale[] = {2,2,1,2,2,2,1};
int majorScale2[] = {0,2,4,5,7,9,11,12};
int minorScale[] = {2,2,2,1,2,2,1};
int pentaMajor[] = {2,2,3,2,3};
int pentaMajor2[] = {0,2,4,7,9,12};





void ARDUINO_ISR_ATTR adcComplete() {
  adc_coversion_done = true;
}



//Timing variables
#define PRE_SCALER 80 // ABP_CLOCK frequency is 80Mhz, so a prescalar of 80 will make the timing units 1micro second
// #define SAMPLING_RATE 44 //44kHz
#define SAMPLING_PERIOD 60 //sampling period in microseconds

// int samplingPeriod = float(1/SAMPLING_RATE) * 1000; //in microseconds
Oscillator channels[9];
Strip strip;


// put function declarations here:

int sampling = 22000;
int potPin = 35;

int modulo = 0;
int modChange = 0;
float shiftCounter = 0;
int lastTime = 0;




u_int8_t calculateWave(Oscillator &channel);

// Modifications that apply to all channels
LFO lfo;
Envelope envelope;
GlobalVals globalVals;

void setupChannels();

//envelope and amplitude
uint8_t mixer(int numActive);
void attack(Oscillator &channel);
void decay(Oscillator &channel);
void release(Oscillator &channel);
void resetChannel(Oscillator &channel);
int calculateGliss(int val);
void setupScreens();
void attachVar(Screen &screen,int pos,int *var,int min,int max);

bool CALC_FLAG = false;
bool loopStart = false;

u_int8_t sig = 0;
int offCount = 0;



hw_timer_t *Timer0_Cfg = NULL;


wave waves[] = {squareWave,triangle,sawtooth,sinWave};

//linking variables with UI


void IRAM_ATTR Timer0_ISR()
{
  // if(!CALC_FLAG && loopStart){

  // }
  CALC_FLAG = false;
  dacWrite(DAC1,sig);
}



void setup() {
  pinMode(36,INPUT);
  Serial.begin(9600);



  setupChannels();
  //for now, manually setup channel
  lfo.waveform = TRIANGLE;
  lfo.amp = 0;
  lfo.freq = 5;
  lfo.duty = 5;
  // channels[0].state = ATTACKING;
  //channels[0].freq = 262;
  channels[0].key = 60;

  setupScreens();
  setupUI();
  setupTask();


  analogContinuous(adc_pins, adc_pins_count, 1, 20000, &adcComplete);
  analogContinuousStart();
  //setup timer
  Timer0_Cfg = timerBegin(1000000); //1MHz
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR);
  timerAlarm(Timer0_Cfg, SAMPLING_PERIOD, true,0);
 
}

void loop() {
  loopStart = true;
  if(adc_coversion_done){
    analogContinuousRead(&result, 0);
    int val = float(result[0].avg_read_raw);
    if(val == 0){
      if(offCount < 5){
        val = strip.currentVal;
        offCount++;
      }

    }
    else{
      offCount = 0;
    }
    if(abs(strip.currentVal - val) > 40){
      if(strip.currentOut == 0){
        channels[0].state = ATTACKING;
      }
      int out = calculateGliss(val);
      //Serial.println(out);
      if(out == 0){
        channels[0].state = RELEASING;
      }
      else{
        channels[0].freq = out;
      }

      strip.currentVal = val;
      strip.currentOut = out;

      // pitchBend = val;
    }
    // else{
    //   pitchBend = 0;
    // }
    adc_coversion_done = false;
  }

  if(!CALC_FLAG){
    
    getMidi();


    int numActive = 0;
    //Calculate all active channels
    for(int i = 0; i < NUM_CHANNELS; i++){
      if(channels[i].state != INACTIVE){
        channels[i].currentTime+=SAMPLING_PERIOD;
        if(channels[i].state == ATTACKING){
          attack(channels[i]);
          //decay(channels[i]);
          // channels[i].amp = 255;
        }
        else if(channels[i].state == DECAYING){
          decay(channels[i]);
        }
        else{
          release(channels[i]);
          // channels[i].amp = 0;
          //resetChannel(channels[i]);
        }
        calculateWave(channels[i]);
        numActive++;
      }
    }
    //Calculate output
    sig = mixer(numActive);
    // sig = channels[0].output;

    //xSemaphoreGive(xMutex);
    // int timeStart = micros();
    updateUI();
    CALC_FLAG = true;
    // Serial.println(micros()-timeStart);

  }
  



}




u_int8_t calculateWave(Oscillator &channel){

  //calculate modulo
  int modulo = int((waves[lfo.waveform])(lfo.freq,lfo.amp,lfo.duty,channel.currentTime));
  modulo -= (lfo.amp/2); // decrease modulo to be above and below 0
  modulo+=globalVals.pitchBend;

  //get phase shift for carrier wave
  float m = float((channel.freq + modulo))/channel.freq;
  channel.shiftCounter += (m*SAMPLING_PERIOD - SAMPLING_PERIOD);
  int newX = channel.currentTime + channel.shiftCounter;
  channel.output = (waves[globalVals.waveform])(channel.freq,channel.amp,globalVals.duty,newX);
  
  return 0;

}

void attack(Oscillator &channel){
  if(envelope.attack == 0){
    channel.amp = globalVals.ampMax;
    channel.state = DECAYING;
    return;
  }
  if(channel.amp < globalVals.ampMax){
    channel.amp +=  float(globalVals.ampMax*SAMPLING_PERIOD)/(envelope.attack*1000);
    if(channel.amp>globalVals.ampMax){
      channel.amp = globalVals.ampMax;
    }
  }
  else{
    channel.state = DECAYING;
  }
}

void decay(Oscillator &channel){
  if(envelope.decay == 0){
    channel.amp = envelope.sustain;
    return;
  }
  if(channel.amp > envelope.sustain){
    channel.amp -= float(SAMPLING_PERIOD * globalVals.ampMax)/(envelope.decay*1000);
    // int b = (channel.envelope.sustain -channel.envelope.ampMax) * (channel.prevTime - channel.envelope.attack);
    // channel.carrier.amp = (float(b)/channel.envelope.decay) - channel.envelope.ampMax;
    if(channel.amp < envelope.sustain){
      channel.amp = envelope.sustain;
    }
  }
}

void release(Oscillator &channel){
  if(envelope.release == 0){
    resetChannel(channel);
    return;
  }
  if(channel.amp > 0){
    channel.amp -= float(SAMPLING_PERIOD * channel.relAmp)/(envelope.release*1000);
    if(channel.amp < 0){
      channel.amp = 0;
    }
  }
  else{
    resetChannel(channel);
  }
}


uint8_t mixer(int numActive){
  //Leave a buffer room of 4 notes
  uint8_t totalOut = 0;
  int divisor = 4;
  
  //if there are more than 4 active notes
  if(numActive > divisor){
    divisor = numActive;
  }
  for(int i =0; i < NUM_CHANNELS; i++){
    if(channels[i].state != INACTIVE){
      totalOut += channels[i].output/divisor;
    }
  }
  return totalOut;
}

void setupChannels(){
  globalVals.ampMax = 255;
  globalVals.pitchBend = 0;
  globalVals.waveform = SQUARE;
  globalVals.duty = 5;
  //Having all channels share a modulation ocillator
  lfo.freq = 10;
  lfo.amp = 0;
  lfo.duty = 5;
  lfo.waveform = SQUARE;

  //Having all channels share an envelope
  envelope.attack = 200;
  envelope.decay = 1000;
  envelope.release = 1000;
  envelope.sustain = 100;


  for(int i = 0; i < NUM_CHANNELS; i++){
    //Setup structs
    // Channel newChannel;
    Oscillator newOcilator;

    
    //Ocilator
    newOcilator.freq = 440;
    newOcilator.amp = 255;
    newOcilator.duty = 0.5;
    *(newOcilator.waveform) = squareWave;

    newOcilator.currentTime = 0;
    newOcilator.shiftCounter = 0;
    newOcilator.state = INACTIVE;
    newOcilator.key = 0;
    // Add channel to list
    channels[i] = newOcilator;
  }

  strip.mode = 0;
  strip.numNotes = 10;
  strip.spacing = 20;
  strip.currentVal = 0;
  strip.currentOut = 0;



}

void resetChannel(Oscillator &channel){
  channel.key = 0;
  channel.shiftCounter = 0;
  channel.currentTime = 0;
  channel.state = INACTIVE;
}

int calculateGliss(int val){
  float noteSection = (4096/strip.numNotes);
  int noteSize = noteSection - strip.spacing;
  if(val < 50){
    return 0;
  }
  if(val % int(noteSection) <= noteSize && val % int(noteSection) != 0){
    int origin = 60;
    if(channels[0].key != 0){
      origin = channels[0].key;
    }

    int noteNum = val/noteSection;
    int note = channels[0].key + ((noteNum/5)*12) + pentaMajor2[noteNum % 5];
    // return noteToF(channels[0].key +(val/noteSection) + 1 );
    return noteToF(note);

    
  }
  else{
    return 0;
  }
  
}
void attachVar(Screen &screen,int pos,int *var,int min,int max){
  screen.vars[pos] = var;
  screen.min[pos] = min;
  screen.max[pos] = max;
}
void setupScreens(){

  
  //Oscillator screen
  attachVar(OSCscr,0,&globalVals.waveform,0,4);
  attachVar(OSCscr,1,&globalVals.ampMax,0,255);
  attachVar(OSCscr,2,&globalVals.duty,0,10);

  //LFO Screen
  attachVar(LFOscr,0,&lfo.waveform,0,4);
  attachVar(LFOscr,1,&lfo.freq,0,20);
  attachVar(LFOscr,2,&lfo.amp,0,255);
  attachVar(LFOscr,3,&lfo.duty,0,10);

  //Env Screen
  attachVar(ENVscr,0,&envelope.attack,0,1000);
  attachVar(ENVscr,1,&envelope.decay,0,3000);
  attachVar(ENVscr,2,&envelope.sustain,0,255);
  attachVar(ENVscr,3,&envelope.release,0,3000);

}

// bool stripPressed(int reading){
//   if(strip.currentVal != 0 && reading == 0){
//     strip.currentVal
//   }
// }
