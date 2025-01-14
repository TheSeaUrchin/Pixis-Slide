#include "waveforms.h"
#define TABLE_SIZE 4000

int sinTable[TABLE_SIZE];

void makeSineTable(){
  for(int i = 0; i < TABLE_SIZE;i++){
    float point = (float(i)/TABLE_SIZE);
    //floor to 2 decimal places
    sinTable[i] = int(sin((6.28) * (point))*100);
  }
}

u_int8_t squareWave(int freq, u_int8_t amplitude,int duty, int prevTime){
  float d = float(duty)/10;
  if(freq == 0){
    return 0; //prevent division by 0
  }
  int period = 1000000/freq; //Convert frequency to period in microseconds
  int point = prevTime % period; //Calculate the point relative to the period

  int8_t retVal = 0;
  if(point < period * d){
    retVal = amplitude; //Set return value to the amplitude if point is within the duty cycle time
  }
  return retVal;

}



u_int8_t sinWave(int freq, u_int8_t amplitude, int duty, int prevTime){
  int period = 1000000/freq; // convert frequency to period in microseconds
  

  int multiplier = amplitude/2;   //Shift sine wave so minimum is 0
  int point = prevTime % period; //get point in relation to period

  return sin((6.28/period) * (point))*multiplier + multiplier;
}

u_int8_t sinWaveTable(int freq, u_int8_t amplitude, int duty, int prevTime){
  int startTime = micros();
  int period = 1000000/freq; // convert frequency to period in microsecond
  int multiplier = amplitude/2;   //Shift sine wave so minimum is 0

  int point = prevTime % period; //get point in relation to period
  float retVal = sinTable[(TABLE_SIZE*point)/period]*multiplier;
  // int retVal = 0;
  return retVal/100 + multiplier;
}


u_int8_t sawtooth(int freq, u_int8_t amplitude,int duty, int prevTime){
  int period = 1000000/freq;
  int point = prevTime % period;

  //Multiply by 100 to avoid floating point calculation
  float amp = amplitude * 100;

  return  ( (amp/period) * point) /100;

}

u_int8_t triangle(int freq, u_int8_t amplitude, int duty, int prevTime){
  
  int period = 1000000/freq; // convert frequency to period in microseconds
  int point = prevTime % period; // get point relative to cycle
  
  float amp = amplitude; //convert amplitude to a float for calculations
  int peak = period/2;
  int ret = 0;


  if(point <= peak){
    //Positive slope
    ret = ( float(amp/peak) * point);

  }
  else{
    //Negative slope
    ret =  amplitude*2 - ((float(amp/peak) * point));
  }

  //Prevent integer overflow in case of small integer division error
  if(ret > amplitude){
    return amplitude;
  }
  else{
    return ret;
  }
}