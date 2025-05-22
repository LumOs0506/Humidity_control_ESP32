# Humidity Control System using ESP32

![Humidity Control System](https://via.placeholder.com/800x400?text=Humidity+Control+System)

## Overview

This project is an automated humidity control system built on the ESP32 platform. It monitors ambient temperature and humidity levels and automatically maintains optimal humidity through a combination of humidifiers, fans, and drying mechanisms. The system features both automatic and manual operation modes, with Bluetooth control capabilities via the Blinker app.

## Features

- **Automatic Humidity Control**: Maintains humidity levels between 40-70%
- **Multi-mode Operation**:
  - Auto mode: System automatically manages humidity based on thresholds
  - Bluetooth mode: Manual control via smartphone app
  - TW mode: External hardware trigger control
- **Water Level Monitoring**: Ultrasonic sensor for water level detection
- **LCD Display**: Real-time temperature and humidity readings
- **Bluetooth Control**: Remote operation of all components
- **Components Control**:
  - Humidifier: Increases ambient humidity
  - Fan: Reduces humidity and improves air circulation
  - Dry Servo: Opens drying mechanism when humidity is too high
  - Pump: Refills the humidifier water tank
  - Light: Auxiliary lighting system

## Hardware Requirements

- ESP32 Development Board (UPesy WROOM model)
- DHT11 Temperature and Humidity Sensor
- ST7735 TFT Display
- Ultrasonic Distance Sensor (SR04)
- Servo Motor
- Relay Module or MOSFETs for controlling:
  - Humidifier
  - Fan
  - Water Pump
  - LED Light
- Jumper Wires and Breadboard
- Power Supply

## Pin Configuration

| Component      | ESP32 Pin |
|----------------|-----------|
| TFT CS         | 16        |
| TFT DC         | 17        |
| TFT RST        | 5         |
| DHT11          | 25        |
| Dry Servo      | 26        |
| Fan            | 32        |
| Humidifier     | 13        |
| Pump           | 15        |
| Light          | 2         |
| SR04 Trigger   | 22        |
| SR04 Echo      | 21        |
| TW Humidifier  | 27        |
| TW Fan         | 33        |
| TW Light       | 12        |

## Software Requirements

- PlatformIO IDE (recommended) or Arduino IDE
- Required Libraries:
  - Adafruit GFX Library
  - Adafruit ST7735 Library
  - ESP32Servo
  - DHT Sensor Library
  - Blinker Library
  - SR04 Library

## Installation

1. Clone this repository
   ```
   git clone https://github.com/LumOs0506/Humidity_control_ESP32.git
   ```

2. Open the project in PlatformIO IDE

3. Install the required libraries through PlatformIO Library Manager or using the platformio.ini file

4. Connect your ESP32 board

5. Upload the code to your board

## Operation Modes

### Automatic Mode
- The system monitors humidity levels using the DHT11 sensor
- When humidity drops below 40%, the humidifier activates
- When humidity exceeds 70%, the fan and drying mechanism activate
- Water level is continuously monitored to prevent the humidifier from running dry

### Bluetooth Control Mode
- Connect to the device via the Blinker app
- Manually control each component:
  - Button 1: Toggle humidifier
  - Button 2: Toggle fan
  - Button 3: Toggle light
  - Button 4: Toggle drying mechanism
  - Button 5: Toggle water pump
  - Button 6: Change operation mode

### TW Mode (External Trigger)
- External hardware triggers can control the system
- Digital input pins are monitored for high/low states to control various components

## Circuit Diagram

[Insert circuit diagram here]

## Usage Instructions

1. Power on the device
2. The display will show the current temperature and humidity
3. By default, the system starts in automatic mode
4. To switch to Bluetooth control:
   - Connect to the device via the Blinker app
   - Use Button 6 to toggle between different modes
5. Monitor the water level and refill as needed

## Troubleshooting

- **Humidifier not working**: Check water level and pump operation
- **Inaccurate readings**: Ensure the DHT11 sensor is properly connected
- **Display issues**: Verify the ST7735 connections and power supply
- **Servo not moving**: Check servo connections and power supply

## Future Improvements

- Add web interface for remote monitoring
- Implement data logging capabilities
- Add more sensors for comprehensive environmental monitoring
- Develop a custom PCB design for more reliable connections
- Add power management features
