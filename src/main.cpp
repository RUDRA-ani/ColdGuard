#include <Arduino.h>
#include "app/application.h"

void setup()
{
    Serial.begin(115200);
    application_init();
}

void loop()
{
    application_update();
}
