
#include "SerialComm.h"

void mainTask(void *params){
    Channel* channels = ((Params*) params)->channels;
    SemaphoreHandle_t xMutex = ((Params*) params)->xMutex;
    
    while(true){
        if(Serial.available()!=0){
            int input = Serial.parseInt();
            Serial.println(input);
            xSemaphoreTake (xMutex, portMAX_DELAY);
            channels[0].carrier.freq = input;
            xSemaphoreGive (xMutex);
        }
        delay(50);
    }
}

void setupTask(Params* params){
      xTaskCreatePinnedToCore (
        mainTask,
        "SerialTask",
        1000,
        params,
        10,
        NULL,
        0
    );
}

