// src/main.cpp
#include <Arduino.h>
#include "Config.h"
#include "State.h"
#include "SensorManager.h"
#include "DisplayManager.h"
#include "SerialCommander.h"

SystemState state;
SensorManager sensorManager(state);
DisplayManager displayManager(state);
SerialCommander serialCommander(state, sensorManager);

unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(9600);
    sensorManager.begin();
    displayManager.begin();
    displayManager.showWelcomeMessage();
    delay(1000);
    sensorManager.test();
}

void loop() {
    serialCommander.process();

    if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = millis();
        sensorManager.update();
        displayManager.update();
    }
}
