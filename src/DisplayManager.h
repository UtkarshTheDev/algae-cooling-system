// src/DisplayManager.h
#pragma once
#include <LiquidCrystal_I2C.h>
#include "State.h"

class DisplayManager {
public:
    DisplayManager(SystemState& state);
    void begin();
    void showWelcomeMessage();
    void update();
private:
    SystemState& _state;
    LiquidCrystal_I2C _lcd;
};
