/*
 * Dual Temperature Monitor with LCD Display and Debug Features
 * Using LM35 Analog Temperature Sensors
 * 
 * HARDWARE CONNECTIONS:
 * ---------------------
 * LM35 Temp Sensors (Analog):
 *   - Room Temp Sensor: Connect to Analog Pin A0
 *   - Algae Temp Sensor: Connect to Analog Pin A1
 *   - Each sensor: VCC(left) to 5V, GND(right) to GND, OUTPUT(middle) to analog pin
 *   - NO resistor needed for LM35!
 *   - Output: 10mV per °C (0°C = 0V, 100°C = 1V)
 * 
 * LCD Display (I2C):
 *   - SDA to A4 (on Uno/Nano)
 *   - SCL to A5 (on Uno/Nano)
 *   - VCC to 5V, GND to GND
 * 
 * SERIAL COMMANDS:
 * ----------------
 * scan           - Scan for I2C devices (LCD)
 * fake on        - Enable fake/mock temperature readings
 * fake off       - Disable fake readings (use real sensors)
 * set room 25.5  - Set fake room temperature to 25.5°C
 * set algae 22.0 - Set fake algae temperature to 22.0°C
 * status         - Show current mode and temperatures
 * debug on       - Enable detailed debug output (shows raw ADC values)
 * debug off      - Disable debug output
 * calibrate      - Show calibration info for LM35 sensors
 * help           - Show all commands
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
#define ROOM_TEMP_PIN A0    // Room temperature sensor on Analog Pin A0
#define ALGAE_TEMP_PIN A1   // Algae temperature sensor on Analog Pin A1

// LCD Configuration (try 0x27 or 0x3F if not working)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD at address 0x27

// LM35 Configuration
#define SAMPLES_PER_READ 10  // Number of samples to average for stability
#define ADC_RESOLUTION 1024.0  // 10-bit ADC
#define REFERENCE_VOLTAGE 5.0  // Arduino reference voltage
#define MV_PER_DEGREE 10.0     // LM35: 10mV per degree Celsius

// Global variables
bool fakeMode = false;
bool debugMode = false;
float fakeRoomTemp = 24.0;
float fakeAlgaeTemp = 22.0;
float lastRoomTemp = 0.0;
float lastAlgaeTemp = 0.0;
unsigned long lastUpdate = 0;
unsigned long lastFluctuation = 0;
const unsigned long UPDATE_INTERVAL = 2000;  // Update every 2 seconds
const unsigned long FLUCTUATION_INTERVAL = 1000;  // Fluctuate every 1 second

void setup() {
  Serial.begin(9600);
  Serial.println(F("\n===Room/Algae Temperature Monitor ==="));
  Serial.println(F("Initializing..."));
  
  // Set ADC reference to default (5V)
  analogReference(DEFAULT);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Algae Cooling System");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  
  delay(1000);
  
  // Initial sensor test
  scanI2CDevices();
  testLM35Sensors();
  
  lcd.clear();
  Serial.println(F("\nSystem Ready!"));
  Serial.println(F("Type 'help' for commands"));
  printHelp();
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    processSerialCommand();
  }
  
  // Update temperatures
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    updateTemperatures();
    displayTemperatures();
  }
  
  // Add fluctuation in fake mode
  if (fakeMode && (millis() - lastFluctuation >= FLUCTUATION_INTERVAL)) {
    lastFluctuation = millis();
    addRealisticFluctuation();
  }
}

float readLM35Temperature(int pin) {
  // Take multiple samples and average for stability
  long sum = 0;
  for (int i = 0; i < SAMPLES_PER_READ; i++) {
    sum += analogRead(pin);
    delay(10);  // Small delay between readings
  }
  
  float avgReading = sum / (float)SAMPLES_PER_READ;
  
  // Convert ADC reading to voltage
  // ADC value / 1024 * 5V = voltage
  float voltage = (avgReading / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
  
  // Convert voltage to temperature
  // LM35: 10mV per degree, so 1V = 100°C
  float temperature = voltage * 100.0;
  
  if (debugMode) {
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

void updateTemperatures() {
  if (fakeMode) {
    // Use fake temperatures (already fluctuating)
    lastRoomTemp = fakeRoomTemp;
    lastAlgaeTemp = fakeAlgaeTemp;
  } else {
    // Read real LM35 sensor data
    lastRoomTemp = readLM35Temperature(ROOM_TEMP_PIN);
    lastAlgaeTemp = readLM35Temperature(ALGAE_TEMP_PIN);
    
    // Sanity check (LM35 range: 0-100°C typically)
    if (lastRoomTemp < 0 || lastRoomTemp > 150) {
      if (debugMode) Serial.println(F("WARNING: Room sensor reading out of range!"));
    }
    if (lastAlgaeTemp < 0 || lastAlgaeTemp > 150) {
      if (debugMode) Serial.println(F("WARNING: Algae sensor reading out of range!"));
    }
  }
  
  if (debugMode) {
    Serial.print(F("Room: "));
    Serial.print(lastRoomTemp, 1);
    Serial.print(F("°C | Algae: "));
    Serial.print(lastAlgaeTemp, 1);
    Serial.print(F("°C | Mode: "));
    Serial.println(fakeMode ? F("FAKE") : F("REAL"));
  }
}

void displayTemperatures() {
  lcd.clear();
  
  // Line 1: Room Temperature
  lcd.setCursor(0, 0);
  lcd.print("Room:");
  lcd.setCursor(6, 0);
  if (lastRoomTemp >= 0 && lastRoomTemp < 150) {
    lcd.print(lastRoomTemp, 1);
    lcd.print((char)223);  // Degree symbol
    lcd.print("C");
  } else {
    lcd.print("ERROR");
  }
  
  // Line 2: Algae Temperature
  lcd.setCursor(0, 1);
  lcd.print("Algae:");
  lcd.setCursor(6, 1);
  if (lastAlgaeTemp >= 0 && lastAlgaeTemp < 150) {
    lcd.print(lastAlgaeTemp, 1);
    lcd.print((char)223);  // Degree symbol
    lcd.print("C");
  } else {
    lcd.print("ERROR");
  }
  
  // Show fake mode indicator
  if (fakeMode) {
    lcd.setCursor(15, 0);
    lcd.print("F");
  }
}

void addRealisticFluctuation() {
  // Add random fluctuation ±0.5°C for realism
  float roomVariation = random(-50, 51) / 100.0;  // -0.5 to +0.5
  float algaeVariation = random(-50, 51) / 100.0;
  
  fakeRoomTemp += roomVariation;
  fakeAlgaeTemp += algaeVariation;
  
  // Keep within reasonable bounds (±2°C from base)
  static float baseRoom = 24.0;
  static float baseAlgae = 22.0;
  static bool baseSet = false;
  
  if (!baseSet) {
    baseRoom = fakeRoomTemp;
    baseAlgae = fakeAlgaeTemp;
    baseSet = true;
  }
  
  if (abs(fakeRoomTemp - baseRoom) > 2.0) {
    fakeRoomTemp = baseRoom + random(-200, 201) / 100.0;
  }
  if (abs(fakeAlgaeTemp - baseAlgae) > 2.0) {
    fakeAlgaeTemp = baseAlgae + random(-200, 201) / 100.0;
  }
}

void processSerialCommand() {
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();
  
  if (cmd == "scan") {
    scanI2CDevices();
    testLM35Sensors();
  }
  else if (cmd == "fake on") {
    fakeMode = true;
    Serial.println(F("✓ Fake mode ENABLED"));
  }
  else if (cmd == "fake off") {
    fakeMode = false;
    Serial.println(F("✓ Fake mode DISABLED - Using real sensors"));
  }
  else if (cmd.startsWith("set room ")) {
    float temp = cmd.substring(9).toFloat();
    if (temp > -50 && temp < 100) {
      fakeRoomTemp = temp;
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
      fakeAlgaeTemp = temp;
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
    debugMode = true;
    Serial.println(F("✓ Debug mode ENABLED - Showing ADC values"));
  }
  else if (cmd == "debug off") {
    debugMode = false;
    Serial.println(F("✓ Debug mode DISABLED"));
  }
  else if (cmd == "calibrate") {
    calibrateLM35();
  }
  else if (cmd == "help") {
    printHelp();
  }
  else if (cmd.length() > 0) {
    Serial.println(F("✗ Unknown command. Type 'help' for commands."));
  }
}

void scanI2CDevices() {
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
      Serial.println(F(")"));
      devices++;
      
      // Try to identify device
      if (address == 0x27 || address == 0x3F) {
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

void testLM35Sensors() {
  Serial.println(F("--- LM35 Sensor Test ---"));
  Serial.println(F("LM35 Pinout: [VCC] [OUTPUT] [GND]"));
  Serial.println(F("             (left) (middle) (right)"));
  Serial.println();
  
  // Test Room sensor
  Serial.print(F("Room Sensor (Pin A0): "));
  int roomADC = analogRead(ROOM_TEMP_PIN);
  float roomVolt = (roomADC / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
  float roomTemp = roomVolt * 100.0;
  
  Serial.print(F("ADC="));
  Serial.print(roomADC);
  Serial.print(F(", V="));
  Serial.print(roomVolt, 3);
  Serial.print(F("V, T="));
  Serial.print(roomTemp, 1);
  Serial.println(F("°C"));
  
  if (roomADC < 10) {
    Serial.println(F("  ⚠ Very low reading - Check if sensor is connected!"));
  } else if (roomADC > 1000) {
    Serial.println(F("  ⚠ Very high reading - Check wiring!"));
  } else {
    Serial.println(F("  ✓ Sensor appears to be working"));
  }
  
  // Test Algae sensor
  Serial.print(F("Algae Sensor (Pin A1): "));
  int algaeADC = analogRead(ALGAE_TEMP_PIN);
  float algaeVolt = (algaeADC / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
  float algaeTemp = algaeVolt * 100.0;
  
  Serial.print(F("ADC="));
  Serial.print(algaeADC);
  Serial.print(F(", V="));
  Serial.print(algaeVolt, 3);
  Serial.print(F("V, T="));
  Serial.print(algaeTemp, 1);
  Serial.println(F("°C"));
  
  if (algaeADC < 10) {
    Serial.println(F("  ⚠ Very low reading - Check if sensor is connected!"));
  } else if (algaeADC > 1000) {
    Serial.println(F("  ⚠ Very high reading - Check wiring!"));
  } else {
    Serial.println(F("  ✓ Sensor appears to be working"));
  }
  
  Serial.println(F("--- Test Complete ---\n"));
}

void calibrateLM35() {
  Serial.println(F("\n=== LM35 CALIBRATION INFO ==="));
  Serial.println(F("LM35 outputs 10mV per °C"));
  Serial.println(F("At 25°C: ~250mV (0.25V)"));
  Serial.println(F("At 30°C: ~300mV (0.30V)"));
  Serial.println();
  
  Serial.println(F("Current Readings (averaged over 50 samples):"));
  
  // Room sensor detailed reading
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
  
  // Algae sensor detailed reading
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
  
  Serial.println(F("\nTroubleshooting:"));
  Serial.println(F("- If temp shows ~49°C indoors → Sensor backwards!"));
  Serial.println(F("- If temp shows 0°C → Check OUTPUT pin connection"));
  Serial.println(F("- If temp shows 100+°C → Check VCC/GND wiring"));
  Serial.println(F("- LM35 pinout (flat side facing you): VCC | OUT | GND"));
  Serial.println(F("================================\n"));
}

void printStatus() {
  Serial.println(F("\n=== SYSTEM STATUS ==="));
  Serial.print(F("Mode: "));
  Serial.println(fakeMode ? F("FAKE/MOCK") : F("REAL SENSORS"));
  Serial.print(F("Debug: "));
  Serial.println(debugMode ? F("ON") : F("OFF"));
  Serial.print(F("Room Temp: "));
  Serial.print(lastRoomTemp, 1);
  Serial.println(F("°C"));
  Serial.print(F("Algae Temp: "));
  Serial.print(lastAlgaeTemp, 1);
  Serial.println(F("°C"));
  if (fakeMode) {
    Serial.print(F("Fake Base Room: "));
    Serial.print(fakeRoomTemp, 1);
    Serial.println(F("°C"));
    Serial.print(F("Fake Base Algae: "));
    Serial.print(fakeAlgaeTemp, 1);
    Serial.println(F("°C"));
  }
  Serial.println(F("====================\n"));
}

void printHelp() {
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