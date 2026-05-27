# ESP32 Home Automation System

An advanced Smart Home Automation System built using ESP32, Bluetooth communication, IR Remote control, DHT11 temperature & humidity sensor, TM1637 display, and relay modules.

This project allows users to control home appliances wirelessly using Bluetooth and IR remote while also monitoring environmental conditions in real time.

---

## Features

- Bluetooth-based appliance control
- IR Remote appliance control
- Real-time temperature monitoring
- Real-time humidity monitoring
- TM1637 digital display integration
- Automatic cooler control based on temperature
- Relay-based appliance switching
- FreeRTOS multitasking support
- ESP32 restart functionality
- Bluetooth connection status LED indication

---

## Technologies Used

- ESP32
- Arduino IDE
- Embedded C++
- FreeRTOS
- BluetoothSerial Library
- IRremote Library
- DHT Sensor Library
- TM1637 Display Library

---

## Components Required

| Component | Quantity |
|---|---|
| ESP32 Board | 1 |
| DHT11 Sensor | 1 |
| TM1637 Display | 1 |
| 4 Channel Relay Module | 1 |
| IR Receiver Module | 1 |
| Bluetooth-enabled Mobile | 1 |
| LEDs | 1 |
| Jumper Wires | Multiple |

---

## Pin Configuration

| Component | GPIO Pin |
|---|---|
| DHT11 Sensor | GPIO 4 |
| TM1637 CLK | GPIO 18 |
| TM1637 DIO | GPIO 5 |
| IR Receiver | GPIO 15 |
| Relay 1 | GPIO 23 |
| Relay 2 | GPIO 22 |
| Relay 3 | GPIO 19 |
| Relay 4 | GPIO 21 |
| Bluetooth Status LED | GPIO 2 |

---

## Functionalities

### Bluetooth Controls

| Command | Action |
|---|---|
| 1 | Toggle TV |
| 2 | Toggle Cooler |
| 3 | Toggle Music System |
| 4 | Toggle Light |
| 5 | Restart ESP32 |
| 6 | Turn OFF IR Receiver |
| 7 | Turn ON IR Receiver |
| 8 | Toggle Auto Cooler Mode |

---

## Auto Cooler Feature

The system automatically controls the cooler based on room temperature:

- Cooler turns ON when temperature > 29°C
- Cooler turns OFF when temperature <= 25°C

---

## Display Information

TM1637 display shows:

- Temperature in Celsius
- Humidity percentage
- Device status
- Automation status
- IR status

---

## FreeRTOS Multitasking

The project uses FreeRTOS task scheduling for:

- Continuous DHT11 sensor monitoring
- Display updates
- Real-time automation handling

---

## How to Run

1. Install Arduino IDE
2. Install ESP32 board package
3. Install required libraries:
   - DHT Sensor Library
   - TM1637Display
   - IRremote
   - BluetoothSerial
4. Upload code to ESP32
5. Connect components according to pin configuration
6. Pair Bluetooth device with ESP32

---

## Future Improvements

- WiFi control using Blynk or MQTT
- Mobile application integration
- Voice assistant support
- Firebase cloud integration
- Energy monitoring system

---

## Author

Krishabh Rangare

---

## License

This project is open-source and available under the MIT License.
