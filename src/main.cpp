#include "Channels.h"
#include "SerialComm.h"
#include "waveforms.h"

//ADC setup
uint8_t adc_pins[] = {35};
uint8_t adc_pins_count = sizeof(adc_pins) / sizeof(uint8_t);
volatile bool adc_coversion_done = false;
adc_continuous_data_t *result = NULL;



void ARDUINO_ISR_ATTR adcComplete() {
  adc_coversion_done = true;
}



//Timing variables
#define PRE_SCALER 80 // ABP_CLOCK frequency is 80Mhz, so a prescalar of 80 will make the timing units 1micro second
#define SAMPLING_RATE 44 //44kHz
#define SAMPLING_PERIOD 45 //sampling period in microseconds

int samplingPeriod = float(1/SAMPLING_RATE) * 1000; //in microseconds
Oscillator channels[9];

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
wave WaveForm = triangle;
int pitchBend;


void setupChannels();

//envelope and amplitude
uint8_t mixer(int numActive);
void attack(Oscillator &channel);
void decay(Oscillator &channel);
void release(Oscillator &channel);
void resetChannel(Oscillator &channel);




bool CALC_FLAG = false;

u_int8_t sig = 0;



hw_timer_t *Timer0_Cfg = NULL;

void IRAM_ATTR Timer0_ISR()
{
  CALC_FLAG = false;
  dacWrite(DAC1,sig);
}



void setup() {
  pinMode(35,INPUT);
  Serial.begin(9600);



  setupChannels();

  analogContinuous(adc_pins, adc_pins_count, 1, 20000, &adcComplete);
  analogContinuousStart();
  
  //for now, manually setup channel
  // channels[0].waveform = triangle;
  lfo.waveform = triangle;
  // channels[0].freq = 262;
  lfo.amp = 0;
  lfo.freq = 5;




  setupTask();

  //setup timer
  Timer0_Cfg = timerBegin(1000000); //1MHz
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR);
  timerAlarm(Timer0_Cfg, SAMPLING_PERIOD, true,0);
 
}
int mod = 0;
void loop() {
  if(adc_coversion_done){
    analogContinuousRead(&result, 0);
    mod = float(result[0].avg_read_raw)/409;
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
          // attack(channels[i]);
          //decay(channels[i]);
          channels[i].amp = 255;
        }
        else if(channels[i].state == DECAYING){
          decay(channels[i]);
        }
        else{
          // release(channels[i]);
          channels[i].amp = 0;
          resetChannel(channels[i]);
        }
        calculateWave(channels[i]);
        numActive++;
      }
    }
    //Calculate output
    sig = mixer(numActive);
    // sig = channels[0].output;

    //xSemaphoreGive(xMutex);
    CALC_FLAG = true;
  }


}




u_int8_t calculateWave(Oscillator &channel){

  //calculate modulo
  int modulo = int(lfo.waveform(lfo.freq,lfo.amp,lfo.duty,channel.currentTime));
  modulo+=pitchBend;

  //get phase shift for carrier wave
  float m = float((channel.freq + modulo))/channel.freq;
  channel.shiftCounter += (m*SAMPLING_PERIOD - SAMPLING_PERIOD);
  int newX = channel.currentTime + channel.shiftCounter;
  // Serial.println(carr.freq);

  //return carrier wave
  // wave waveform = *(channel.waveform);
  // channel.output = waveform(channel.freq,channel.amp,channel.duty,newX);
  channel.output = WaveForm(channel.freq,channel.amp,channel.duty,newX);
  return 0;

}

void attack(Oscillator &channel){
  if(channel.currentTime<envelope.attack){
    channel.amp = channel.currentTime * (float(envelope.ampMax)/float(envelope.attack));
  }
  else{
    channel.state = DECAYING;
  }
}

void decay(Oscillator &channel){
  if(channel.amp > envelope.sustain){
    float amp = channel.amp;
    amp -= float(SAMPLING_PERIOD * envelope.ampMax)/envelope.decay;
    // int b = (channel.envelope.sustain -channel.envelope.ampMax) * (channel.prevTime - channel.envelope.attack);
    // channel.carrier.amp = (float(b)/channel.envelope.decay) - channel.envelope.ampMax;
    channel.amp = amp;
  }
}

void release(Oscillator &channel){
  if(channel.amp > 0){
    float amp = channel.amp;
    amp -= float(SAMPLING_PERIOD * envelope.sustain)/envelope.release;
    if(amp < 0){
      amp = 0;
    }
    channel.amp = amp;
  }
  else{
    resetChannel(channel);
  }
}


uint8_t mixer(int numActive){
  //Leave a buffer room of 4 notes
  uint8_t totalOut = 0;
  int divisor = 4;
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
  //Having all channels share a modulation ocillator
  lfo.freq = 10;
  lfo.amp = 0;
  lfo.duty = 0.5;
  lfo.waveform = squareWave;

  //Having all channels share an envelope
  envelope.attack = 500000;
  envelope.decay = 200000;
  envelope.release = 500000;
  envelope.sustain = 100;
  envelope.ampMax = 255;

  for(int i = 0; i < NUM_CHANNELS; i++){
    //Setup structs
    // Channel newChannel;
    Oscillator newOcilator;

    
    //Ocilator
    newOcilator.freq = 440;
    newOcilator.amp = 255;
    newOcilator.duty = 0.5;
    *(newOcilator.waveform) = WaveForm;

    newOcilator.currentTime = 0;
    newOcilator.shiftCounter = 0;
    newOcilator.state = INACTIVE;
    newOcilator.key = 0;
    // Add channel to list
    channels[i] = newOcilator;
  }

}

void resetChannel(Oscillator &channel){
  channel.key = 0;
  channel.shiftCounter = 0;
  channel.currentTime = 0;
  channel.state = INACTIVE;
}

