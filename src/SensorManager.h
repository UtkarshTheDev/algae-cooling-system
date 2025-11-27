// src/SensorManager.h
#pragma once
#include "State.h"

class SensorManager {
public:
    SensorManager(SystemState& state);
    void begin();
    void update();
    void test();
    void calibrate();
private:
    SystemState& _state;
    float _fakeRoomTemp = 24.0;
    float _fakeAlgaeTemp = 22.0;
    float readLM35(int pin);
    void addRealisticFluctuation();
};
