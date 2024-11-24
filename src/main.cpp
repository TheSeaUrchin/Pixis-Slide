#include "Channels.h"
#include "SerialComm.h"

//ADC setup
uint8_t adc_pins[] = {35};
uint8_t adc_pins_count = sizeof(adc_pins) / sizeof(uint8_t);
volatile bool adc_coversion_done = false;
adc_continuous_data_t *result = NULL;

SemaphoreHandle_t xMutex = NULL;


void ARDUINO_ISR_ATTR adcComplete() {
  adc_coversion_done = true;
}



//Timing variables
#define PRE_SCALER 80 // ABP_CLOCK frequency is 80Mhz, so a prescalar of 80 will make the timing units 1micro second
#define SAMPLING_RATE 44 //44kHz

int samplingPeriod = float(1/SAMPLING_RATE) * 1000; //in microseconds
Channel channels[9];

// put function declarations here:

int prevTime;
int sampling = 22000;
int potPin = 35;

int modulo = 0;
int modChange = 0;
float shiftCounter = 0;




u_int8_t sinWave(int freq, u_int8_t amplitude,float duty, int prevTime);
u_int8_t sawtooth(int freq, u_int8_t amplitude,float duty, int prevTime);
u_int8_t triangle(int freq, u_int8_t amplitude,float duty, int prevTime);
u_int8_t squareWave(int freq, u_int8_t amplitude,float duty, int prevTime);
u_int8_t calculateWave(Channel &channel);
void setupChannels();



bool CALC_FLAG = false;

u_int8_t sig = 0;



hw_timer_t *Timer0_Cfg = NULL;

void IRAM_ATTR Timer0_ISR()
{
  CALC_FLAG = false;
  dacWrite(DAC1,sig);
}



void setup() {
  prevTime = micros();
  pinMode(35,INPUT);
  Serial.begin(9600);



  setupChannels();

  analogContinuous(adc_pins, adc_pins_count, 1, 20000, &adcComplete);
  analogContinuousStart();
  
  //for now, manually setup channel
  channels[0].carrier.waveform = squareWave;
  channels[0].modulator.waveform = triangle;
  channels[0].carrier.freq = 262;
  channels[0].modulator.amp = 0;
  channels[0].modulator.freq = 5;

  xMutex = xSemaphoreCreateMutex();

  Params p;
  p.channels = channels;
  p.xMutex = xMutex;

  setupTask(&p);

  //setup timer
  Timer0_Cfg = timerBegin(1000000); //1MHz
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR);
  timerAlarm(Timer0_Cfg, 45, true,0);
 
}
int mod = 0;
void loop() {
  if(adc_coversion_done){
    analogContinuousRead(&result, 0);
    mod = float(result[0].avg_read_raw)/409;
    adc_coversion_done = false;
  }

  if(!CALC_FLAG){
    xSemaphoreTake(xMutex, portMAX_DELAY);
    channels[0].prevTime+=45;
    //channels[0].modulator.amp = mod;
    sig = calculateWave(channels[0]);
    xSemaphoreGive(xMutex);
    CALC_FLAG = true;
  }

  // if(micros()- prevTime >= 45){
  //   prevTime = micros();
  //   channels[0].prevTime = prevTime;
  //   CALC_FLAG = false;
  //   //dacWrite(DAC1,sawtooth(262,255,0.5,prevTime));
  //   dacWrite(DAC1,sig);
    
    
  // }

}

u_int8_t squareWave(int freq, u_int8_t amplitude,float duty, int prevTime){
  if(freq == 0){
    return 0;
  }
  int period = 1000000/freq;
  int point = prevTime % period;

  int8_t retVal = 0;
  if(point < period * duty){
    retVal = amplitude;
  }
  return retVal;

}



u_int8_t sinWave(int freq, u_int8_t amplitude, float duty, int prevTime){
  int period = 1000000/(freq);
  
  //Shift sine wave so minimum is 0
  int multiplier = amplitude/2;
  int point = (prevTime) % (period);

  return sin((6.28/period) * (point))*multiplier + multiplier;
}

u_int8_t sawtooth(int freq, u_int8_t amplitude,float duty, int prevTime){
  int period = 1000000/freq;
  int point = prevTime % period;

  //Multiply by 100 to avoid floating point calculation
  float amp = amplitude * 100;

  return  ( (amp/period) * point) /100;

}

u_int8_t triangle(int freq, u_int8_t amplitude, float duty, int prevTime){
  int period = 1000000/freq;
  int point = prevTime % period;
  
  //Multpiply by 100 to avoid floating point calculations
  float amp = amplitude * 100;
  int peak = period/2;
  int ret = 0;
  // Serial.println(float(amp/peak)*point);


  if(point <= peak){
    //Positive slope
    ret = ( float(amp/peak) * point) /100;

  }
  else{
    //Negative slope
    ret =  amplitude*2 - ((float(amp/peak) * point) /100);
  }

  //Prevent integer overflow in case of small integer division error
  if(ret > amplitude){
    return amplitude;
  }
  else{
    return ret;
  }
}


u_int8_t calculateWave(Channel &channel){
  auto& modu = channel.modulator;
  auto& carr = channel.carrier;
  //calculate modulo
  int modulo = int(channel.modulator.waveform(modu.freq,modu.amp,modu.duty,channel.prevTime));
  modulo+=channel.pitchBend;

  //get phase shift for carrier wave
  float m = float((carr.freq + modulo))/carr.freq;
  channel.shiftCounter += (m*45 - 45);
  int newX = channel.prevTime + channel.shiftCounter;
  // Serial.println(carr.freq);

  //return carrier wave
  channel.output = channel.carrier.waveform(carr.freq,carr.amp,carr.duty,newX);
  return 0;

}

uint8_t attack(){

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
}

void setupChannels(){
  //Having all channels share a modulation ocillator
  Ocillator modOcilator;
  modOcilator.freq = 10;
  modOcilator.amp = 0;
  modOcilator.duty = 0.5;
  modOcilator.waveform = squareWave;

  //Having all channels share an envelope
  Envelope envelope;
  envelope.attack = 0;
  envelope.decay = 0;
  envelope.release = 0;
  envelope.sustain = 0;
  envelope.ampMax = 0;

  for(int i = 0; i < NUM_CHANNELS; i++){
    //Setup structs
    Channel newChannel;
    Ocillator newOcilator;

    
    //Ocilator
    newOcilator.freq = 440;
    newOcilator.amp = 255;
    newOcilator.duty = 0.5;
    newOcilator.waveform = squareWave;

    //Channel
    newChannel.carrier = newOcilator;
    newChannel.modulator = modOcilator;
    newChannel.envelope = envelope;
    newChannel.prevTime = 0;
    newChannel.shiftCounter = 0;
    newChannel.pitchBend = 0;
    newChannel.state = INACTIVE;
    // Add channel to list
    channels[i] = newChannel;
  }

  //Make sure mixer has its own mod
  // Ocillator mixerMod;
  // mixerMod.freq = 10;
  // mixerMod.amp = 0;
  // mixerMod.duty = 0.5;
  // mixerMod.waveform = squareWave;
  // channels[MIXER_CHANNEL].modulator = mixerMod;

}

