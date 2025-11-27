// src/Config.h
#pragma once

#include <Arduino.h>

// Pin definitions
#define ROOM_TEMP_PIN A0
#define ALGAE_TEMP_PIN A1

// LCD Configuration
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// LM35 Configuration
#define SAMPLES_PER_READ 10
#define ADC_RESOLUTION 1024.0
#define REFERENCE_VOLTAGE 5.0
#define MV_PER_DEGREE 10.0

// Timing
const unsigned long UPDATE_INTERVAL = 2000;
const unsigned long FLUCTUATION_INTERVAL = 1000;
