// src/SerialCommander.h
#pragma once
#include "State.h"
#include "SensorManager.h" // To access test/calibrate

class SerialCommander {
public:
    SerialCommander(SystemState& state, SensorManager& sensorManager);
    void process();
private:
    SystemState& _state;
    SensorManager& _sensorManager;
    void printHelp();
    void printStatus();
    void scanI2CDevices();
};
