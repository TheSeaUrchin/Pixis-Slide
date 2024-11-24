
#include "SerialComm.h"
void checkMemory(void);
int noteToF(int note);
USB Usb;
USBH_MIDI  Midi(&Usb);

void mainTask(void *params){


    //Channel* channels = ((Params*) params)->channels;
    // SemaphoreHandle_t xMutex = ((Params*) params)->xMutex;
    //Setup
    while (Usb.Init() == -1) {
        delay( 200 );
        Serial.println("Connection Failed");
    }



    while(true){
        Usb.Task();
        if ( Midi ) {
            uint8_t bufMidi[4];
            uint16_t  rcvd;
            //If we recieved data from the midi controller
            if (Midi.RecvData( &rcvd,  bufMidi) == 0 ) {
                //Handle Midi Message
                
                if(bufMidi[0] == NOTE_ON){
                    int note = noteToF(bufMidi[2]);
                    xSemaphoreTake (xMutex, portMAX_DELAY);
                    channels[0].carrier.freq = note;
                    // Serial.println(channels[0].carrier.freq);
                    xSemaphoreGive (xMutex);

                }
                else if(bufMidi[0] == CONTROL_CHANGE){
                    if(bufMidi[2] == MOD_WHEEL){
                        xSemaphoreTake (xMutex, portMAX_DELAY);
                        channels[0].modulator.amp = bufMidi[3];
                        xSemaphoreGive (xMutex);
                    }
                }
                else if(bufMidi[0] == PITCH_BEND){
                    int bend = bufMidi[3];
                    bend-=64;
                    xSemaphoreTake (xMutex, portMAX_DELAY);
                    channels[0].pitchBend = bend;
                    xSemaphoreGive (xMutex);
                }
            }
    
        }
        delay(1);
        yield();
            
    }

}


void setupTask(Params* params){
      xTaskCreatePinnedToCore (
        mainTask,
        "SerialTask",
        5000,
        params,
        10,
        NULL,
        0
    );
}

void checkMemory() {

  size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

  Serial.print("Free heap memory: "); 

  Serial.println(free_heap); 

}

int noteToF(int note){
    float n = (float(note)-69)/12;
    return (pow(2,n) * 440);
}



// #include "SerialComm.h"

// void mainTask(void *params){
//     USB Usb;
//     USBHub Hub(&Usb);
//     USBH_MIDI  Midi(&Usb);

//     Channel* channels = ((Params*) params)->channels;
//     SemaphoreHandle_t xMutex = ((Params*) params)->xMutex;
    
//     while(true){
//         if(Serial.available()!=0){
//             int input = Serial.parseInt();
//             Serial.println(input);
//             xSemaphoreTake (xMutex, portMAX_DELAY);
//             channels[0].carrier.freq = input;
//             xSemaphoreGive (xMutex);
//         }
//         delay(50);
//     }
// }

// void setupTask(Params* params){
//       xTaskCreatePinnedToCore (
//         mainTask,
//         "SerialTask",
//         5000,
//         params,
//         10,
//         NULL,
//         0
//     );
// }