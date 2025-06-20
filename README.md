Collecting workspace information# IoT Smart Garden Project

## Overview

The **IoT Smart Garden** is an automated plant monitoring and watering system designed to help users maintain optimal soil moisture, temperature, and humidity for their plants. The system uses an ESP32 microcontroller, various sensors (DHT11 for temperature/humidity, soil moisture sensor, and a liquid level sensor), and a servo motor to control watering. Data is sent to the VOne IoT platform for monitoring and workflow automation.

---

## Features

- **Automated Watering:** Waters plants based on real-time soil moisture readings.
- **Environmental Monitoring:** Tracks temperature, humidity, and soil moisture.
- **Remote Control:** Supports manual override and automation via VOne IoT platform.
- **Alerts & Notifications:** Sends warnings when thresholds are crossed (e.g., low moisture, high temperature, low humidity).
- **Failsafes:** Detects empty water tank and stops watering, with buzzer alert.

---

## Hardware Components

- ESP32 microcontroller
- DHT11 temperature & humidity sensor
- Soil moisture sensor
- Liquid level sensor
- Servo motor (for valve control)
- Buzzer (for water tank alert)
- WiFi connectivity

---

## Folder Structure

```
IoT-SmartGarden-Report.pdf      # Project report
arduino-code/
    arduino-code.ino           # ESP32 firmware (main logic)
vone-workflow-code/
    humidity.py                # VOne workflow: humidity alert logic
    moisture.py                # VOne workflow: soil moisture alert logic
    temperature.py             # VOne workflow: temperature alert logic
```

---

## How It Works

### ESP32 Firmware (arduino-code/arduino-code.ino)

- Reads sensor data (temperature, humidity, soil moisture, water level).
- Publishes telemetry to VOne IoT platform.
- Receives actuator commands (manual servo control).
- Automates watering based on soil moisture and water availability.
- Triggers buzzer if water tank is empty.

### VOne Workflow Scripts (vone-workflow-code/)

- **moisture.py:** Checks if soil moisture is below threshold and triggers watering alert.
- **humidity.py:** Checks if humidity is below threshold and triggers alert.
- **temperature.py:** Checks if temperature is above threshold and triggers alert.

---

## Getting Started

### 1. Hardware Setup

- Connect sensors and actuators to ESP32 as per pin assignments in arduino-code.ino.
- Ensure WiFi credentials and VOne device IDs are set in the firmware.

### 2. Firmware

- Open arduino-code.ino in Arduino IDE.
- Install required libraries: `ESP32Servo`, `DHT`, and VOne MQTT client.
- Upload the code to your ESP32.

### 3. VOne IoT Platform

- Register your devices (DHT11, Servo, Moisture Sensor) on VOne.
- Set up workflows using the scripts in vone-workflow-code.
- Configure thresholds as needed in the Python scripts.

---

## Customization

- **Thresholds:** Adjust soil moisture, temperature, and humidity thresholds in the respective Python scripts.
- **Servo Angles:** Modify `SERVO_OPEN_ANGLE` and `SERVO_CLOSED_ANGLE` in the firmware for your valve.
- **Watering Duration:** Change `MIN_WATERING_TIME` and `MAX_WATERING_TIME` as needed.

---

## Project Report

For detailed design, implementation, and results, see IoT-SmartGarden-Report.pdf.

---

## License

This project is for educational purposes. See the report for references and acknowledgments.

---

## Authors

- See project report for contributors.

---

## References

- [VOne IoT Platform](https://vonecloud.com/)
- Arduino, ESP32, DHT11, and related documentation.

---

**Note:** For any issues or questions, please refer to the project report or contact the authors.