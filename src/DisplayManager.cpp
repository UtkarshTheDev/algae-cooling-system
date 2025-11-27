// src/DisplayManager.cpp
#include "DisplayManager.h"
#include "Config.h"

DisplayManager::DisplayManager(SystemState& state) : _state(state), _lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS) {}

void DisplayManager::begin() {
    _lcd.init();
    _lcd.backlight();
}

void DisplayManager::showWelcomeMessage() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("Algae Cooling System");
    _lcd.setCursor(0, 1);
    _lcd.print("Starting...");
}

void DisplayManager::update() {
    _lcd.clear();
    
    // Line 1: Room Temperature
    _lcd.setCursor(0, 0);
    _lcd.print("Room:");
    _lcd.setCursor(6, 0);
    if (_state.roomTemp >= 0 && _state.roomTemp < 150) {
        _lcd.print(_state.roomTemp, 1);
        _lcd.print((char)223);  // Degree symbol
        _lcd.print("C");
    } else {
        _lcd.print("ERROR");
    }
    
    // Line 2: Algae Temperature
    _lcd.setCursor(0, 1);
    _lcd.print("Algae:");
    _lcd.setCursor(6, 1);
    if (_state.algaeTemp >= 0 && _state.algaeTemp < 150) {
        _lcd.print(_state.algaeTemp, 1);
        _lcd.print((char)223);  // Degree symbol
        _lcd.print("C");
    } else {
        _lcd.print("ERROR");
    }
}
