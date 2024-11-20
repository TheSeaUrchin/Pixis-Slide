#include <Arduino.h>
#include "Channels.h"

typedef struct {
    Channel* channels;
    SemaphoreHandle_t xMutex;
}Params;
void setupTask(Params* params);
void mainTask(void *params);