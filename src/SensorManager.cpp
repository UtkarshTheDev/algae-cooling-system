// src/SensorManager.cpp
#include "SensorManager.h"
#include "Config.h"
#include <Arduino.h>

SensorManager::SensorManager(SystemState& state) : _state(state) {}

void SensorManager::begin() {
    // Initialization if needed
}

void SensorManager::update() {
    if (_state.fakeMode) {
        addRealisticFluctuation();
        _state.roomTemp = _fakeRoomTemp;
        _state.algaeTemp = _fakeAlgaeTemp;
    } else {
        _state.roomTemp = readLM35(ROOM_TEMP_PIN);
        _state.algaeTemp = readLM35(ALGAE_TEMP_PIN);
    }
}

float SensorManager::readLM35(int pin) {
  long sum = 0;
  for (int i = 0; i < SAMPLES_PER_READ; i++) {
    sum += analogRead(pin);
    delay(10);
  }
  
  float avgReading = sum / (float)SAMPLES_PER_READ;
  float voltage = (avgReading / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
  float temperature = voltage * 100.0;
  
  if (_state.debugMode) {
    Serial.print(F("  [Pin "));
    Serial.print(pin);
    Serial.print(F("] ADC: "));
    Serial.print(avgReading, 1);
    Serial.print(F(" | Voltage: "));
    Serial.print(voltage, 3);
    Serial.print(F("V | Temp: "));
    Serial.print(temperature, 2);
    Serial.println(F("°C"));
  }
  
  return temperature;
}

void SensorManager::addRealisticFluctuation() {
  float roomVariation = random(-50, 51) / 100.0;
  float algaeVariation = random(-50, 51) / 100.0;
  
  _fakeRoomTemp += roomVariation;
  _fakeAlgaeTemp += algaeVariation;
  
  static float baseRoom = 24.0;
  static float baseAlgae = 22.0;
  static bool baseSet = false;
  
  if (!baseSet) {
    baseRoom = _fakeRoomTemp;
    baseAlgae = _fakeAlgaeTemp;
    baseSet = true;
  }
  
  if (abs(_fakeRoomTemp - baseRoom) > 2.0) {
    _fakeRoomTemp = baseRoom + random(-200, 201) / 100.0;
  }
  if (abs(_fakeAlgaeTemp - baseAlgae) > 2.0) {
    _fakeAlgaeTemp = baseAlgae + random(-200, 201) / 100.0;
  }
}

void SensorManager::test() {
  Serial.println(F("--- LM35 Sensor Test ---"));
  Serial.print(F("Room Sensor (Pin A0): "));
  float roomTemp = readLM35(ROOM_TEMP_PIN);
  Serial.print(F("T="));
  Serial.print(roomTemp, 1);
  Serial.println(F("°C"));

  Serial.print(F("Algae Sensor (Pin A1): "));
  float algaeTemp = readLM35(ALGAE_TEMP_PIN);
  Serial.print(F("T="));
  Serial.print(algaeTemp, 1);
  Serial.println(F("°C"));
  Serial.println(F("--- Test Complete ---\n"));
}

void SensorManager::calibrate() {
  Serial.println(F("\n=== LM35 CALIBRATION INFO ==="));
  Serial.println(F("Current Readings (averaged over 50 samples):"));
  
  long roomSum = 0;
  for (int i = 0; i < 50; i++) {
    roomSum += analogRead(ROOM_TEMP_PIN);
    delay(10);
  }
  float roomAvg = roomSum / 50.0;
  float roomVolt = (roomAvg / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
  float roomTemp = roomVolt * 100.0;
  
  Serial.print(F("Room (A0): ADC="));
  Serial.print(roomAvg, 1);
  Serial.print(F(", Voltage="));
  Serial.print(roomVolt, 3);
  Serial.print(F("V, Temp="));
  Serial.print(roomTemp, 2);
  Serial.println(F("°C"));
  
  long algaeSum = 0;
  for (int i = 0; i < 50; i++) {
    algaeSum += analogRead(ALGAE_TEMP_PIN);
    delay(10);
  }
  float algaeAvg = algaeSum / 50.0;
  float algaeVolt = (algaeAvg / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
  float algaeTemp = algaeVolt * 100.0;
  
  Serial.print(F("Algae (A1): ADC="));
  Serial.print(algaeAvg, 1);
  Serial.print(F(", Voltage="));
  Serial.print(algaeVolt, 3);
  Serial.print(F("V, Temp="));
  Serial.print(algaeTemp, 2);
  Serial.println(F("°C"));
  Serial.println(F("================================\n"));
}
