#include "SerialComm.h"
#include <MidiQueue.h>
#include "Sequencer.h"

#define BTN_SIDE 39
#define DEBUG_DBNC 1000

void printKeyLog();
USB Usb;
USBH_MIDI  Midi(&Usb);

struct logging{
    bool noteOn;
    int key;
    int channelNum;
    struct logging *next;

};
struct logging *keyLogHead = (struct logging *)malloc(sizeof(struct logging));
struct logging *keyLog = keyLogHead;
int logLen = 0;

SemaphoreHandle_t xMutex = NULL;
extern bool mono;


MidiQueue midiQueue = MidiQueue();
void interpertMidi(uint8_t bufMidi[4]);
void buttonISR();

bool usbConnected = false;
int t = millis();
bool sequencing = false;
int pTime = 0;
void mainTask(void *params){

    attachInterrupt(BTN_SIDE,buttonISR,RISING);


    //try to connect to the USB device
    if (Usb.Init() == -1) {
        usbConnected = false;
    }
    else{
        usbConnected = true;
    }


    while(true){
        if(usbConnected){
            Usb.Task(); //poll the USB device
            if(Midi){
                uint8_t bufMidi[4];
                uint16_t  rcvd;
                //If we recieved data from the midi controller
                if (Midi.RecvData( &rcvd,  bufMidi) == 0 ){
                    //critical section
                    xSemaphoreTake (xMutex, portMAX_DELAY);
                    midiQueue.add(bufMidi);
                    xSemaphoreGive (xMutex);
                }
            }
            
        }
        else if(sequencing){
            if(sequenceNext()){
                uint8_t bufMidi[4];
                sequence(bufMidi);
                //critical section
                xSemaphoreTake (xMutex, portMAX_DELAY);
                midiQueue.add(bufMidi);
                xSemaphoreGive (xMutex);


            }
        }
        delay(1);
        yield(); //Prevent Crash 
    }
}

void setupTask(){
      xMutex = xSemaphoreCreateMutex();
      xTaskCreatePinnedToCore (
        mainTask,
        "SerialTask",
        5000,
        NULL,
        10,
        NULL,
        0
    );
}




void getMidi(){
    int arrSize = 0;
    xSemaphoreTake (xMutex, portMAX_DELAY);
    //Update up to two messages at a time
    
    if(midiQueue.size == 0){
        xSemaphoreGive (xMutex);
        return;
    }

    if(midiQueue.size < 3){
        arrSize = midiQueue.size;
    }
    else{
        arrSize = 2;
    }

    uint8_t MidiBuffer[arrSize][4];
    for(int i = 0; i < arrSize; i++){
        midiQueue.pop(MidiBuffer[i]);
    }
    xSemaphoreGive (xMutex);

    
    for(int i = 0; i < arrSize; i++){
        interpertMidi(MidiBuffer[i]);
    }
}

void interpertMidi(uint8_t bufMidi[4]){
    //Handle Midi Message
                
    if(bufMidi[0] == NOTE_ON && bufMidi[3] != 0){
        
        int note = noteToF(bufMidi[2]);
        int channelNum = 0;

        //Select note from notes to activate
        for(int i = 0; i < NUM_CHANNELS; i++){
            if(channels[i].state == INACTIVE || mono){
                channels[i].state = ATTACKING;
                channels[i].freq = note;
                channels[i].key = bufMidi[2];
                channelNum = i;
                break;
            }
        }
        if(bufMidi[3] == 0){
            Serial.println("Velocity 0");
        }
        
        #ifdef KEY_LOGGING
            keyLog->noteOn = true;
            keyLog->key = bufMidi[2];
            keyLog->channelNum = channelNum;
            struct logging *nextKey = (struct logging *)malloc(sizeof(struct logging));
            keyLog->next = nextKey;
            keyLog = nextKey;
            logLen++;
        #endif

    }

    //Some midi Controllers set velocity to 0 to turn off note, checking 3rd byte for velocity

    else if(bufMidi[0] == NOTE_OFF || bufMidi[3] == 0){ 

        int channelNum = 0;
        //If mono, just turn off note 0
        if(mono){
            channels[0].state = RELEASING;
            channels[0].relAmp = channels[0].amp;
            channelNum = 0;
            return;
        }
        
        for(int i = 0; i < NUM_CHANNELS; i++){
            if((channels[i].key == bufMidi[2]) && (channels[i].state != INACTIVE && channels[i].state != RELEASING)){
                channels[i].state = RELEASING;
                channels[i].relAmp = channels[i].amp;
                channelNum = i;
            }
        }
        #ifdef KEY_LOGGING
            keyLog->noteOn = false;
            keyLog->key = bufMidi[2];
            keyLog->channelNum = channelNum;
            struct logging *nextKey = (struct logging *)malloc(sizeof(struct logging));
            keyLog->next = nextKey;
            keyLog = nextKey;
            logLen++;
            if(logLen > 20){
                struct logging *tempHead = keyLogHead;
                keyLogHead = keyLogHead -> next;
                free(tempHead);
                logLen--;
            }
        #endif

    }
    else if(bufMidi[0] == CONTROL_CHANGE){
        
        if(bufMidi[2] == MOD_WHEEL){
            lfo.amp = bufMidi[3]; //all channels share the same modulator
        }

        else if(bufMidi[2] == DEBUG_BTN && bufMidi[3] > 0){
            xSemaphoreTake (xMutex, portMAX_DELAY);
            midiQueue.printQueue();
            xSemaphoreGive (xMutex);
            #ifdef KEY_LOGGING
                printKeyLog();
            #endif
        }
    }
    else if(bufMidi[0] == PITCH_BEND){
        int bend = bufMidi[3];
        bend-=64;
        globalVals.pitchBend = bend; //TODO: make pitchbend shared
    }
}



int noteToF(int note){
    float n = (float(note)-69)/12;
    return (pow(2,n) * 440);
}

void printKeyLog(){
    struct logging *keyNode = keyLogHead;
    while(1){
        if(keyNode->noteOn){
            Serial.print("Note On: ");
        }
        else{
            Serial.print("Note Off: ");
        }
        Serial.print(keyNode->key);
        Serial.print(" --> ");
        Serial.println(keyNode->channelNum);
        if(keyNode->next == NULL){
            break;
        }
        keyNode = keyNode->next;
    }
}

//For Debugging
void IRAM_ATTR buttonISR(){
    if(millis() - pTime < DEBUG_DBNC){
        return;
    }
    Serial.println("Interrupt");
    pTime = millis();
    sequencing = true;
    beginSequence();
    
}

