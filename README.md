# InAs Nanowire Resistance Measurement

Arduino Uno firmware for reading two resistance-measurement inputs with a ProtoCentral ADS1220 24-bit ADC breakout. The firmware reads the ADS1220 single-ended channels AIN1 and AIN2 and reports calculated resistance values over serial.

## Hardware

Connect the ADS1220 breakout to the Arduino Uno:

| ADS1220 | Arduino Uno |
| --- | --- |
| DRDY | D6 |
| MISO | D12 |
| MOSI | D11 |
| SCLK | D13 |
| CS | D7 |
| DVDD | +5 V |
| DGND | GND |

Connect the measurement circuit outputs to ADS1220 inputs AIN1 and AIN2. The resistance calculation uses a 402 kΩ reference resistor (`R1`) and assumes a 3.301 V reference (`VREF`). Adjust these constants in `src/main.cpp` to match the actual circuit and ADC reference.

## Build and upload

This is a PlatformIO project targeting an Arduino Uno. Install PlatformIO Core or the PlatformIO IDE, connect the board, then run from the project directory:

```sh
pio run
pio run --target upload
pio device monitor --baud 9600
