# 💧 ESP32 Water Tank Controller

An **ESP32-based smart water tank controller** using the **ESP-IDF framework**.  
This system monitors water levels, detects overflow, and controls two irrigation pumps.  
It features built-in **Ethernet connectivity via the WT32-ETH01** module.

## 🔌 Components Overview

- **ESP32 (WT32-ETH01)** – Main controller with Ethernet PHY  
- **DS18B20** – Temperature sensor (1-Wire)  
- **A02YYUW** – Ultrasonic water level sensor (UART)  
- **Overflow Sensor** – Digital input (with external pull-up recommended)  
- **Pump Relays** – Control for two irrigation pumps  

## 🌐 Ethernet – WT32-ETH01

The WT32-ETH01 has an onboard **LAN8720 PHY** connected to the ESP32 via RMII.  
Ethernet is handled internally and does **not** require SPI configuration.

## 🌡️ Temperature Sensor (DS18B20 – 1-Wire)

| GPIO | Function |
|------|----------|
| 4    | Data     |

> ⚠️ **Note:** Ensure a **4.7 kΩ pull-up resistor** is connected on the 1-Wire line.

## 🌊 Ultrasonic Water Sensor (A02YYUW – UART)

| GPIO | Function |
|------|----------|
| 5    | RX       |
| 17   | TX       |

## 🛑 Overflow Sensor

| GPIO | Function |
|------|----------|
| 2    | Input    |

> ⚠️ **Note:** Connect the other pin of the sensor to **GND**. Use an **external pull-up resistor** if needed.

## ⚙️ Pump Relays

| GPIO | Relay        |
|------|--------------|
| 32   | Pump Relay 1 |
| 33   | Pump Relay 2 |

## 📦 Dependencies

This project uses components from [UncleRus/esp-idf-lib](https://github.com/UncleRus/esp-idf-lib):

- `ds18x20`
- `esp_idf_lib_helpers`
- `onewire`

> 📁 Components have been copied into the project directory for convenience.

## 🛠️ Development Environment

This project is developed using **Visual Studio Code** with the **ESP-IDF extension**.

### Setup Instructions

1. **Install VS Code**: [Visual Studio Code](https://code.visualstudio.com)  
2. **Install ESP-IDF Extension**: `espressif.esp-idf-extension`  
3. **Follow the ESP-IDF Setup Wizard** to install:
   - ESP-IDF framework  
   - Python dependencies  
   - Required tools (CMake, Ninja, etc.)  

4. Once setup is complete, you can:
   - **Build** the project using `ESP-IDF: Build`  
   - **Flash** the firmware with `ESP-IDF: Flash`  
   - **Monitor** serial output using `ESP-IDF: Monitor`  

> ⚠️ **Make sure the correct ESP32 target** (e.g., `esp32`, `esp32s3`) is selected in your workspace settings.

## 🧰 Deployment & Serial Communication

The **WT32-ETH01 does not include a USB-to-serial interface**, so you need a **USB-UART adapter** for flashing and serial monitoring.

### 🔌 Recommended Adapter

- **FT232RL-M-USB-C**  
  A reliable USB-C UART adapter with **3.3V logic levels**, compatible with ESP32 devices.

### ⚙️ Auto-Reset Wiring for Easy Flashing

To enable automatic bootloader mode (no button pressing), wire **EN** and **IO0** to the adapter’s **RTS** and **DTR** pins:

| WT32-ETH01 Pin | USB-UART Adapter Pin |
|----------------|----------------------|
| `TX` (GPIO1)   | `RX`                 |
| `RX` (GPIO3)   | `TX`                 |
| `GND`          | `GND`                |
| `EN`           | `RTS`                |
| `IO0`          | `DTR` *(via 0.1 µF cap)* |
| `3.3V`         | `VCC`                |
| `5V`           | `VCC` *(3.3V may not supply enough power)* |