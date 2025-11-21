# 🌿 Algae Cooling System

An Arduino-based dual temperature monitoring system that demonstrates the thermal regulation effects of algae-covered roofing on indoor climate control across different seasons.

## 📋 Project Overview

This project explores **bio-thermal insulation** using algae as a natural temperature regulator for building roofs. By monitoring both room temperature and algae surface temperature in real-time, we can observe how living algae coating impacts indoor thermal comfort throughout the year.

### The Hypothesis
- **Summer**: Algae acts as a cooling layer, reducing heat absorption
- **Winter**: Algae provides insulation, reducing heat loss
- **Result**: Year-round improved thermal comfort and potential energy savings

## 🎯 Features

- **Dual Temperature Monitoring**: Simultaneous tracking of room and algae surface temperatures
- **Real-time LCD Display**: 16x2 I2C LCD shows live temperature readings
- **Mock Mode**: Simulate temperature scenarios for testing and demonstrations
- **Serial Control Interface**: Configure and debug via serial commands
- **Data Logging Ready**: Easy integration with data logging systems
- **Realistic Fluctuations**: Mock mode includes natural temperature variations

## 🔧 Hardware Requirements

### Components
- Arduino Uno/Nano (any ATmega328P board)
- 2x LM35 Temperature Sensors
- 16x2 I2C LCD Display
- Jumper wires
- Breadboard (optional)

### Connections

| Component | Pin | Arduino Pin |
|-----------|-----|-------------|
| Room Temp Sensor (LM35) | OUTPUT | A0 |
| Algae Temp Sensor (LM35) | OUTPUT | A1 |
| LCD Display | SDA | A4 |
| LCD Display | SCL | A5 |

**LM35 Pinout** (flat side facing you):
```
[VCC] [OUTPUT] [GND]
  5V    Signal   GND
```

## 📚 Software Requirements

### Arduino Libraries
Install via Arduino Library Manager:
- `Wire` (built-in)
- `LiquidCrystal_I2C`

### Installation
1. Clone this repository:
```bash
   git clone https://github.com/UtkarshTheDev/algae-cooling-system.git
```
2. Open `algae_temp_monitor.ino` in Arduino IDE
3. Install required libraries
4. Upload to your Arduino board
5. Open Serial Monitor (9600 baud)

## 🎮 Usage

### Serial Commands

| Command | Description | Example |
|---------|-------------|---------|
| `scan` | Detect I2C devices and test sensors | `scan` |
| `fake on` | Enable mock temperature readings | `fake on` |
| `fake off` | Use real sensor data | `fake off` |
| `set room 25.5` | Set mock room temperature | `set room 25.5` |
| `set algae 22.0` | Set mock algae temperature | `set algae 22.0` |
| `status` | Show current readings and mode | `status` |
| `debug on` | Enable detailed debug output | `debug on` |
| `debug off` | Disable debug output | `debug off` |
| `calibrate` | Show sensor calibration data | `calibrate` |
| `help` | Display all available commands | `help` |

### LCD Display Format
```
Room: 24.3°C
Algae: 22.1°C  
```

## 🔬 Project Applications

- **Research**: Study thermal effects of bio-insulation
- **Education**: Demonstrate sustainable building concepts
- **Prototyping**: Test algae cultivation systems
- **Energy Analysis**: Measure potential HVAC savings
- **Climate Science**: Understand passive cooling/heating

## 📊 Expected Results

### Summer Scenario
- **Without Algae**: Room temp follows outdoor temp closely
- **With Algae**: Room temp reduced by 2-5°C due to evaporative cooling

### Winter Scenario
- **Without Algae**: Greater heat loss through roof
- **With Algae**: Improved insulation, reduced heating needs

## 🐛 Troubleshooting

### Sensor Issues
- **Temp shows ~49°C indoors**: Sensor connected backwards
- **Temp shows 0°C**: OUTPUT pin disconnected
- **Temp shows 100+°C**: VCC/GND wiring incorrect
- Use `calibrate` command to verify sensor readings

### LCD Issues
- **No display**: Check I2C address (try 0x27 or 0x3F)
- **Garbled text**: Check SDA/SCL connections
- Use `scan` command to detect I2C devices

## 📖 Scientific Background

Algae photosynthesis and evapotranspiration create a natural cooling effect. The living biomass also provides:
- **Thermal mass**: Reduces temperature fluctuations
- **Evaporative cooling**: Converts heat to water vapor
- **UV protection**: Reduces roof material degradation
- **Insulation**: Biomass layer reduces heat transfer

## 🤝 Contributing

Contributions are welcome! Areas for improvement:
- Additional sensor support
- Data analysis scripts
- Documentation improvements
- Hardware design optimizations

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👤 Author

**Utkarsh Tiwari**
- GitHub: [@UtkarshTheDev](https://github.com/UtkarshTheDev)
- Project Link: [https://github.com/UtkarshTheDev/algae-cooling-system](https://github.com/UtkarshTheDev/algae-cooling-system)

## 🙏 Acknowledgments

- Inspired by green roof and living wall technologies
- Built with Arduino and open-source libraries
- Thanks to the sustainable architecture community

**⭐ If you find this project interesting, please consider giving it a star!**

---
