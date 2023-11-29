# ADS1220 ADC for InAs Nanowire Resistance Measurement

## Overview
This Arduino-based code uses the ADS1220 24-bit ADC breakout board to measure the resistance of InAs nanowires. It's designed to read 2 channels in continuous conversion mode.

## Hardware Setup
- **Arduino Connections:**
  - DRDY to D2
  - MISO to D12
  - MOSI to D11
  - SCLK to D13
  - CS to D7
  - DVDD to +5V
  - DGND to Ground
  - AN0-AN3 to Analog Input

## Software Dependencies
- Protocentral_ADS1220.h
- SPI.h
- ArduinoLowPower.h
- arduino_secrets.h

## License
This project is licensed under the MIT License.
