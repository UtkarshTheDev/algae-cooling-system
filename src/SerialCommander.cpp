// src/SerialCommander.cpp
#include "SerialCommander.h"
#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

SerialCommander::SerialCommander(SystemState& state, SensorManager& sensorManager) : _state(state), _sensorManager(sensorManager) {}

void SerialCommander::process() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toLowerCase();
        
        if (cmd == "scan") {
            scanI2CDevices();
            _sensorManager.test();
        }
        else if (cmd == "fake on") {
            _state.fakeMode = true;
            Serial.println(F("✓ Fake mode ENABLED"));
        }
        else if (cmd == "fake off") {
            _state.fakeMode = false;
            Serial.println(F("✓ Fake mode DISABLED - Using real sensors"));
        }
        else if (cmd.startsWith("set room ")) {
            float temp = cmd.substring(9).toFloat();
            if (temp > -50 && temp < 100) {
                // This is a bit of a hack, we should have a setter in SensorManager
                // _sensorManager.setFakeRoomTemp(temp);
                Serial.print(F("✓ Room temp set to: "));
                Serial.print(temp, 1);
                Serial.println(F("°C"));
            } else {
                Serial.println(F("✗ Invalid temperature (-50 to 100°C)"));
            }
        }
        else if (cmd.startsWith("set algae ")) {
            float temp = cmd.substring(10).toFloat();
            if (temp > -50 && temp < 100) {
                // This is a bit of a hack, we should have a setter in SensorManager
                // _sensorManager.setFakeAlgaeTemp(temp);
                Serial.print(F("✓ Algae temp set to: "));
                Serial.print(temp, 1);
                Serial.println(F("°C"));
            } else {
                Serial.println(F("✗ Invalid temperature (-50 to 100°C)"));
            }
        }
        else if (cmd == "status") {
            printStatus();
        }
        else if (cmd == "debug on") {
            _state.debugMode = true;
            Serial.println(F("✓ Debug mode ENABLED - Showing ADC values"));
        }
        else if (cmd == "debug off") {
            _state.debugMode = false;
            Serial.println(F("✓ Debug mode DISABLED"));
        }
        else if (cmd == "calibrate") {
            _sensorManager.calibrate();
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd.length() > 0) {
            Serial.println(F("✗ Unknown command. Type 'help' for commands."));
        }
    }
}

void SerialCommander::printHelp() {
  Serial.println(F("\n=== AVAILABLE COMMANDS ==="));
  Serial.println(F("scan              - Scan I2C and test LM35 sensors"));
  Serial.println(F("fake on           - Enable mock/fake readings"));
  Serial.println(F("fake off          - Use real sensor readings"));
  Serial.println(F("set room 25.5     - Set fake room temp to 25.5°C"));
  Serial.println(F("set algae 22.0    - Set fake algae temp to 22.0°C"));
  Serial.println(F("status            - Show current temperatures"));
  Serial.println(F("debug on          - Show ADC values and voltages"));
  Serial.println(F("debug off         - Disable debug output"));
  Serial.println(F("calibrate         - Show detailed sensor readings"));
  Serial.println(F("help              - Show this help menu"));
  Serial.println(F("=========================\n"));
}

void SerialCommander::printStatus() {
  Serial.println(F("\n=== SYSTEM STATUS ==="));
  Serial.print(F("Mode: "));
  Serial.println(_state.fakeMode ? F("FAKE/MOCK") : F("REAL SENSORS"));
  Serial.print(F("Debug: "));
  Serial.println(_state.debugMode ? F("ON") : F("OFF"));
  Serial.print(F("Room Temp: "));
  Serial.print(_state.roomTemp, 1);
  Serial.println(F("°C"));
  Serial.print(F("Algae Temp: "));
  Serial.print(_state.algaeTemp, 1);
  Serial.println(F("°C"));
  Serial.println(F("====================\n"));
}

void SerialCommander::scanI2CDevices() {
  Serial.println(F("\n--- I2C Device Scanner ---"));
  byte error, address;
  int devices = 0;
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print(F("I2C device found at 0x"));
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(F(" ("));
      Serial.print(address);
      Serial.println(F("))"));
      devices++;
      
      if (address == LCD_ADDRESS) {
        Serial.println(F("  → LCD Display"));
      }
    }
  }
  
  if (devices == 0) {
    Serial.println(F("No I2C devices found!"));
    Serial.println(F("Check wiring: SDA->A4, SCL->A5"));
  }
  Serial.println(F("--- Scan Complete ---\n"));
}
