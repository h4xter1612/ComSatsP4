# ComSatsP4

**Author:** Juan Pablo Solís Ruiz | **Date:** 4/26/2025

**Coauthors:** Eduardo Franco Ortega, Elrick Eduardo Haces Gil Sotelo

## Overview

**ComSatsP4** - Embedded Communication System

**ComSatsP4** is an embedded system project designed for satellite communications.
It focuses on implementing a lightweight, reliable communication system using a microcontroller and a custom-designed antenna, enabling data transmission between a satellite and a ground station.

The system is built for efficiency, robustness, and low-power consumption — ideal for nanosatellite missions like Earth observation, climate monitoring, and telemetry transmission.

## How to use

Before using **ComSatsP4**, you need to have:

- A compatible microcontroller (e.g., Arduino, STM32, ESP32)
- Embedded C/C++ development environment (Arduino IDE, PlatformIO, STM32CubeIDE, etc.)
- Basic electronic components (antenna, transceiver module like SX1280 or CC2500, capacitors, resistors)
- UART or SPI interface configured between MCU and transceiver
- (Optional) Oscilloscope or SDR for signal analysis

### To deploy ComSatsP4:

1. **Download** the source code to your local machine.
2. **Open** the project in your preferred IDE.
3. **Connect** your microcontroller and transceiver module following the provided pinout diagram.
4. **Flash** the firmware to the microcontroller.
5. **Test** communication by sending and receiving data packets using the provided functions.

A basic communication test sketch (`ComSatsP4_Test.ino`) is included to verify hardware functionality.

## Versions

- **Development Environment:** Windows 11 / Linux (Ubuntu 22.04 supported)
- **Firmware Language:** C/C++
- **Current Hardware:** Arduino Uno / STM32 Nucleo (expandable)
- **Release:** Alpha version 0.1 (first deployment)

## Features of ComSatsP4:

### 🔹 Embedded Communication Core

The system handles basic packet transmission and reception over S-band frequencies, ensuring low latency and high reliability.

### 🔹 Pre-configured Communication Drivers

ComSatsP4 includes pre-written drivers for popular transceivers like SX1280 and CC2500, supporting easy integration and flexible switching between modules.

### 🔹 Low-Power Management

Designed for nanosatellite missions, the firmware includes low-power modes and efficient resource management to extend operational time in orbit.

### 🔹 Expandable Modular Architecture

Easily adapt the project to support additional sensors or telemetry data by plugging into a modular data handling framework.

### 🔹 Basic Error Detection

ComSatsP4 implements basic checksum verification to ensure the integrity of transmitted and received messages.

## Hardware Requirements

- Microcontroller with SPI or UART interface
- S-band transceiver module (e.g., SX1280 or CC2500)
- PCB-mounted custom S-band antenna
- Power supply (regulated 3.3V or 5V depending on MCU and transceiver)
- Basic components (resistors, capacitors)

## Future Improvements (Roadmap 🚀)

- AES encryption for secure communication
- Dynamic frequency hopping
- Redundant communication links
- Integration with onboard sensors for real-time telemetry
- Full integration into CubeSat onboard computer (OBC)

## License

This project is open source under the MIT License.
Feel free to use it, modify it, and adapt it to your own satellite missions!

## Extra Resources

- [SX1280 Datasheet](https://www.semtech.com/products/wireless-rf/ultra-low-power-sx1280)
- [CC2500 Datasheet](https://www.ti.com/product/CC2500)
- [CubeSat Design Specification](https://www.cubesat.org/s/cds_rev13_final2.pdf)
- [Guide to Embedded Systems for Satellites](https://ntrs.nasa.gov/api/citations/20160011378/downloads/20160011378.pdf)
