# Climate Monitoring & Control System

This project simulates a simple Arduino-based climate monitoring and control system. It measures temperature, humidity, and ambient light using a DHT22 sensor and an LDR (photoresistor). Based on those readings, it automatically adjusts:
- A **servo motor** (simulating heating or cooling valves)  
- **Five LEDs** (representing artificial lighting intensity)  

All data is displayed on a **20×4 I2C LCD**. The entire setup is written in C++ for the Arduino platform and is simulated in Wokwi.

---

## Features

- **Sensors**  
  - **DHT22**: Reads temperature (°C) and humidity (%) every 2 s.  
  - **LDR (Photoresistor)**: Reads ambient light level (analog 0–1023) and converts it to a real-world lux value via a log–resistance formula (`γ = 0.7, RL10 = 50 kΩ`).  

- **Actuators**  
  - **Servo Motor**:  
    - **0°** = Open “heating” valve  
    - **90°** = Valve closed (idle)  
    - **180°** = Open “cooling” valve  
    Uses a three-position ON-OFF control with a 2 °C dead zone around the 25 °C setpoint.  
  - **Five LEDs**: Represent five discrete lighting levels. Based on the computed lux, LEDs turn ON/OFF in stages (from very dark → all 5 LEDs ON, to very bright → 0 LEDs).  

- **Display**  
  - **20×4 I2C LCD** shows:  
    1. **Line 1**: Current temperature (°C) and humidity (%)  
    2. **Line 2**: Calculated lux and number of LEDs lit  
    3. **Line 3**: Current servo action (`CALENTAR`, `REPOSO`, or `ENFRIAR`)  
    4. **Line 4**: Status of heating and cooling valves (`ON`/`OFF`)  

---

## How to Simulate in Wokwi

1. **Open the Project**  
   - Visit the following Wokwi link to load the simulation:  
     https://wokwi.com/projects/431813915036075009  

2. **Adjust Sensor Values**  
   - **DHT22**: Click on the DHT22 icon in the Wokwi canvas. In the Properties panel, drag the “Temperature” and “Humidity” sliders to simulate changes. The LCD and servo will react accordingly.  
   - **LDR**: Use the slider attached to the photoresistor icon. Moving it simulates changing ambient light—watch how the LEDs turn ON/OFF and how the LCD updates the lux value.

3. **Observe Actuators**  
   - **Servo Motor**: Will rotate to 0°, 90°, or 180° based on the measured temperature (below 23 °C → heating; above 27 °C → cooling; otherwise idle).  
   - **Heating/Cooling Valves**: Two LEDs (red for heating, blue for cooling) turn ON when their respective valves are activated.  
   - **Lighting LEDs**: Five yellow LEDs arranged on the canvas light up progressively as ambient lux decreases (darker environment).

4. **View Data on LCD**  
   - The **20×4 LCD** continuously shows live sensor readings, control actions, and valve/LED statuses. You can confirm that the ON-OFF logic and lux thresholds are working as intended.

---

## Code Overview

- **Language**: C++ (Arduino framework)  
- **Main Libraries**:  
  - `DHT.h` (for DHT22)  
  - `LiquidCrystal_I2C.h` (for 20×4 I2C LCD)  
  - `Servo.h` (for servo motor control)  

- **Control Logic**:  
  - **Temperature**:  
    ```cpp
    if (temp < 23.0) { // below dead zone
      // activate heating
    }
    else if (temp > 27.0) { // above dead zone
      // activate cooling
    }
    else {
      // idle (zone dead)
    }
    ```
  - **Lighting** (lux thresholds):  
    ```cpp
    if (lux < 100)      level = 5;
    else if (lux < 500) level = 4;
    else if (lux < 2000) level = 3;
    else if (lux < 10000) level = 2;
    else if (lux < 50000) level = 1;
    else level = 0;
    ```
---

## Files and Structure

- **`README.md`**: This document.  
- **`main.c`**: Main Arduino sketch containing setup and loop with all sensor readings, control logic, and LCD updates.  
- **`diaram.json`**: Main Arduino diagram containing all the conections and setup of components.  
- **Simulation Quad**:  
  - **Arduino Uno**  
  - **DHT22 Sensor**  
  - **Photoresistor Sensor**  
  - **20×4 I2C LCD**  
  - **Servo Motor**  
  - **7 LEDs**: 5 for lighting levels, 2 for heating/cooling valves  

---

## Usage Notes

- To demo **automatic regulation** of temperature, adjust the DHT22 temperature slider and watch the servo and valves respond to bring the temperature back into the dead-zone.  
- To demo **lighting control**, move the LDR slider from bright to dark and observe the number of LEDs illuminating progressively.  
- All values update every 2 seconds on the LCD, allowing you to verify correct ON-OFF actions and thresholds.

---

## License

This project is released under the MIT License. Feel free to copy, modify, and share!

---
