
#include <LiquidCrystal_I2C.h>
#include "customChars.h"
#include "SerialComm.h"
#include "UI.h"

#define BUTTON_UP 32
#define BUTTON_DOWN 34
#define BUTTON_LEFT 33
#define BUTTON_RIGHT 35
#define DEBOUNCE 100


 
// #define CLK 12 // CLK ENCODER 
// #define DT 13 // DT ENCODER 
 
ESP32Encoder encoder1;
ESP32Encoder encoder2;
ESP32Encoder encoder3;
ESP32Encoder encoder4;




LiquidCrystal_I2C lcd(0x27, 21, 22);
int currentScreen = 1;
extern SemaphoreHandle_t encoderMutex;




Item items[4] = {
  {GRAPH,0,1,"WAVE",&encoder1,0,0},
  {BAR,6,1,"FRQ",&encoder2,0,0},
  {BAR,11,1,"AMP",&encoder3,0,0},
  {BAR,16,1,"PWM",&encoder4,0,0}
};


Screen LFOscr;
Screen OSCscr;
Screen ENVscr;

Screen *screens[] = {&OSCscr,&LFOscr,&ENVscr};

long encoderCnt = 0;
long value = 0;
bool newScreen = false;
int pressTime = 0;
int encoderTimer = 0;
void makeGraph(int name);
void printGraph(int curx, int cury, int name,String caption);
void makeBar();
void printBar(int curx, int cury,int num,String caption);
int updateValue(struct Item &item);
void ScreenSetup();
void bindUI(Screen &screen,int element);
int convertToUI(int min, int max, int value);
int convertFromUI(int min, int max, int value);
void drawScreen(Screen &screen);
void setScreen(Screen &screen,int pos,ItemType itemType,String caption);
void buttonInt();
void printHeader();
void setupUI(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  //top menu


  makeBar();
  ScreenSetup();
  pinMode(BUTTON_DOWN,INPUT);
  pinMode(BUTTON_UP,INPUT);
  pinMode(BUTTON_LEFT,INPUT);
  pinMode(BUTTON_RIGHT,INPUT);
  


  lcd.setCursor(0,0);
  lcd.cursor_on();
  attachInterrupt(BUTTON_LEFT,buttonInt,RISING);
  attachInterrupt(BUTTON_RIGHT,buttonInt,RISING);


  //encoder Stuff

  //encoder1.useInternalWeakPullResistors = puType::up;
  // encoder1.isrServiceCpuCore = 0;
  encoder1.attachHalfQuad ( 16, 4);
  encoder1.setCount ( 0 );


  encoder2.useInternalWeakPullResistors = puType::up;
  // encoder2.isrServiceCpuCore = 0;
  encoder2.attachHalfQuad ( 13, 12);
  encoder2.setCount ( 0 );


  encoder3.useInternalWeakPullResistors = puType::up;
  // encoder3.isrServiceCpuCore = 0;
  encoder3.attachHalfQuad ( 15, 14);
  encoder3.setCount ( 0 );


  encoder4.useInternalWeakPullResistors = puType::up;
  // encoder4.isrServiceCpuCore = 0;
  encoder4.attachHalfQuad ( 27, 26);
  encoder4.setCount ( 0 );



  drawScreen(*screens[currentScreen]);
  lcd.setCursor(4*currentScreen,0);
}

void updateUI(){
  if(newScreen){
    drawScreen(*screens[currentScreen]);
    Serial.println(items[0].value);
    Serial.println(*items[0].valueBind);
    newScreen = false;
  }
  if(millis()-encoderTimer < 100){
      return;
  }
  encoderTimer = millis();
  for(int i = 0; i < screens[currentScreen]->numElements; i++){
    int update = updateValue(items[i]);
    //int update = 0;
    if(update != 0){

      if(items[i].itemType == GRAPH){
        if(update > 0 ){
          if(items[i].value == 3){
            items[i].value = 0;
          }
          else{
            items[i].value++;
          }
        }
        else{
          if(items[i].value == 0){
            items[i].value = 3; //change to max later
          }
          else{
            items[i].value-=1;
          }
        }

        printGraph(items[i].x,items[i].y,items[i].value,items[i].caption);
        *items[i].valueBind = items[i].value;
      }

      if(items[i].itemType == BAR){
        if(update > 0 && items[i].value < 10){
          items[i].value++;
        }
        else if(update < 0 && items[i].value > 0){
          items[i].value-=1;
        }
        printBar(items[i].x,items[i].y,items[i].value,items[i].caption);
        *items[i].valueBind = convertFromUI(items[i].min,items[i].max,items[i].value);
      }
      lcd.setCursor(4*currentScreen,0);
    }
  }
}


void makeGraph(int name){
  for(int i=0; i<4 ; i++){
    lcd.createChar(i,graphs[name].graphBytes[i]);
  }
}
void makeBar(){
  for(int i=0; i<4 ; i++){
    lcd.createChar(i+4,barChars[i]);
  }
}


void printHeader(){
  lcd.setCursor(0,0);
  lcd.print("OSC ");
  lcd.print("LFO ");
  lcd.print("ENV ");
  lcd.print("FX ");
  lcd.print("ETC");
}
void printGraph(int curx, int cury, int name,String caption){
  makeGraph(name);
  lcd.setCursor(curx,cury);
  lcd.write(0);
  lcd.write(1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.setCursor(curx,cury+1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(2);
  lcd.write(3);
  lcd.setCursor(curx,cury+2);
  lcd.print(caption);

}

void printBar(int curx, int cury,int num,String caption){
  lcd.setCursor(curx,cury);
  lcd.print("|");
  lcd.setCursor(curx,cury+1);
  lcd.print("|");
  lcd.setCursor(curx+1,cury);
  if(num>9){
    lcd.write(0xFF);
    lcd.setCursor(curx+1,cury+1);
    lcd.write(0xFF);
  }
  else if(num> 5){
    lcd.write(num-5+3);
    lcd.setCursor(curx+1,cury+1);
    lcd.write(0xFF);
  }
  else if (num == 5){
    lcd.print(" ");
    lcd.setCursor(curx+1,cury+1);
    lcd.write(0xFF);
  }
  else if(num == 0){
    lcd.print(" ");
    lcd.setCursor(curx+1,cury+1);
    lcd.print(" ");
  }
  else{
    lcd.print(" ");
    lcd.setCursor(curx+1,cury+1);
    lcd.write(num+3);
  }

  lcd.setCursor(curx+2,cury);
  lcd.print("|");
  lcd.setCursor(curx+2,cury+1);
  lcd.print("|");
  lcd.setCursor(curx,cury+2);
  lcd.print(caption);
  lcd.setCursor(curx+3,cury);


}

int updateValue(struct Item &item){
  int retval = 0;
  //int currentEncoder = item.prevEncoderVal;
  // xSemaphoreTake (encoderMutex, portMAX_DELAY);
  int currentEncoder =   item.encoder->getCount()/2;
  // xSemaphoreGive(encoderMutex);


  if(item.prevEncoderVal != currentEncoder){
    if(item.prevEncoderVal > currentEncoder){
      retval -=1;
    }
    else if(item.prevEncoderVal < currentEncoder){
      retval++;
    }
    item.prevEncoderVal = currentEncoder;

  }
  return retval;
}
void setScreen(Screen &screen,int pos,ItemType itemType,String caption){
  screen.Type[pos] = itemType;
  screen.caption[pos] = caption;
}
void ScreenSetup(){
  //setup stuff
  setScreen(LFOscr,0,GRAPH,"WAVE");
  setScreen(LFOscr,1,BAR,"FRQ");
  setScreen(LFOscr,2,BAR,"AMP");
  setScreen(LFOscr,3,BAR,"PW");

  LFOscr.numElements = 4;

  setScreen(ENVscr,0,BAR,"ATK");
  setScreen(ENVscr,1,BAR,"DEC");
  setScreen(ENVscr,2,BAR,"STN");
  setScreen(ENVscr,3,BAR,"REL");
  ENVscr.numElements = 4;

  setScreen(OSCscr,0,GRAPH,"WAVE");
  setScreen(OSCscr,1,BAR,"AMP");
  setScreen(OSCscr,2,BAR,"PW");

  OSCscr.numElements = 3;


}



void drawScreen(Screen &screen){
    lcd.clear();
    printHeader();
    for(int i = 0; i < screen.numElements; i++){
        items[i].itemType = screen.Type[i];
        items[i].caption = screen.caption[i];
        
        bindUI(screen,i);

        if(items[i].itemType == BAR){
            items[i].value = convertToUI(items[i].min,items[i].max,*items[i].valueBind);
        }
        else{
            items[i].value = *items[i].valueBind;
        }


        if(items[i].itemType == GRAPH){
            printGraph(items[i].x,items[i].y,items[i].value,items[i].caption);
        }
        else if(items[i].itemType == BAR){
            printBar(items[i].x,items[i].y,items[i].value,items[i].caption);
        }
    }
    lcd.setCursor(currentScreen*4,0);
}


void bindUI(Screen &screen, int element){
    items[element].valueBind = screen.vars[element];
    items[element].min = screen.min[element];
    items[element].max = screen.max[element];
}

int convertFromUI(int min, int max, int value){

    int retVal = (((max-min) * value)/10 ) + min;
    if(retVal > max){
        return max;
    }
    else{
        return retVal;
    }
}

int convertToUI(int min, int max, int value){
    int retVal = ((value - min) * 10)/(max-min);
    if(retVal > 10){
        return 10;
    }
    else{
        return retVal;
    }
}

void IRAM_ATTR buttonInt(){
  if(millis() - pressTime < DEBOUNCE){
    return;
  }
  pressTime = millis();
  if(digitalRead(BUTTON_LEFT)){
    if(currentScreen >0){
        newScreen = true;
        currentScreen--;
    }

  }
  else if(digitalRead(BUTTON_RIGHT)){
    if(currentScreen < NUM_SCREENS-1){
      newScreen = true;
      currentScreen++;
    }

  }
}
//OSC
//  wave
//  pwm
//  mono/polly

/*
LFO:
  wave
  frq
  amp
  pwm
*/

/*
ENV:
  Atk
  Dec
  Sus
  Rel
*/

/*
FX:
  LPF
  HPF
  ECO
*/

/*
ETC:
  Sld/Gls
  None/Note/Pent/blue
  Major/Minor
*/
