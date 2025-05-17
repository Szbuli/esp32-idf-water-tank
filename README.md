# 💧 ESP32 Water Tank Controller

An **ESP32-based smart water tank controller** using the **ESP-IDF framework**.  
This system monitors water levels, detects overflow, and controls two irrigation pumps. It features built-in **Ethernet connectivity via the WT32-ETH01** module.

---

## 🔌 Components Overview

- **ESP32 (WT32-ETH01)** – Main controller with Ethernet PHY
- **DS18B20** – Temperature sensor (1-Wire)
- **A02YYUW** – Ultrasonic water level sensor (UART)
- **Overflow Sensor** – Digital input (with external pull-up recommended)
- **Pump Relays** – Control for two irrigation pumps

---

## 🌐 Ethernet – WT32-ETH01

The WT32-ETH01 has an onboard **LAN8720 PHY** connected to the ESP32 via RMII.  
Ethernet is handled internally and does **not** require SPI configuration.

---

## 🌡️ Temperature Sensor (DS18B20 – 1-Wire)

| GPIO | Function |
|------|----------|
| 4    | Data     |

> ⚠️ Note: Ensure a **pull-up resistor(4.7K).** is connected on the 1-Wire line 
---

## 🌊 Ultrasonic Water Sensor (A02YYUW – UART)

| GPIO | Function |
|------|----------|
| 5    | RX       |
| 17   | TX       |

---

## 🛑 Overflow Sensor

| GPIO | Function |
|------|----------|
| 2    | Input    |

> ⚠️ Note: Connect the other pin of the sensor to **GND**. Use an **external pull-up resistor** if needed.

---

## ⚙️ Pump Relays

| GPIO | Relay        |
|------|--------------|
| 32   | Pump Relay 1 |
| 33   | Pump Relay 2 |

---

## 📦 Dependencies

This project uses components from [UncleRus/esp-idf-lib](https://github.com/UncleRus/esp-idf-lib):

- `ds18x20`
- `esp_idf_lib_helpers`
- `onewire`

> Components have been copied into the project directory for convenience.

---
