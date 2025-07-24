
>The **ESP32-S3** is the preferred standard board for all new deployments of PedalinoMini™ which include a built-in support for USB MIDI.
>Please note that some of the newest features are only supported on the ESP32-S3 and **not** on the original ESP32.
>
>If you still want to add USB MIDI to any ESP32 board here in the following a couples of methods.

## USB MIDI using Raspberry Pi Pico (RP2040)

Raspberry Pi Pico is a generally available cost-effective board that can be used to add an USB MIDI connection.

### Method 1

Thanks to [Sthopeless](https://github.com/Sthopeless)

- Flash Pico with the .UF2 binary file provided here https://github.com/rsta2/pico/releases/tag/v1.0
- Connect ESP32 RX1 to Pico GP0 and ESP32 TX1 to Pico GP1 and GND to GND.

### Method 2

- Flash Pico with CircuitPhyton .UF2 binary file provided here https://circuitpython.org/board/raspberry_pi_pico. Tutorial available [here](https://learn.adafruit.com/getting-started-with-raspberry-pi-pico-circuitpython/circuitpython).
- Copy the CIRCUITPY folder files under the CIRCUITPY Pico drive. The CIRCUITPY folder contains the boot.py file and the [Adafruit CircuitPhyton MIDI](https://github.com/adafruit/Adafruit_CircuitPython_MIDI) library.
- Connect ESP32 RX1 to Pico GP0 and ESP32 TX1 to Pico GP1 and GND to GND.

For both methods:

ESP32 Pin|Pico Pin
---------|--------
RX1|GP0
TX1|GP1
GND|GND

- If the Pico is powered via USB, the VSYS must NOT be connected to other power source.
- [Optional] Power ESP32 board just feeding the power from the VBUS port on the Pico to VIN (if available) of the ESP32.

IMPORTANT: ESP32 board and Pico must share GND.

## USB MIDI using Arduino Pro Micro

The cheapest and compact way to implement an USB MIDI connection is using an Arduino Pro Micro and the [BlokasLabs/USBMIDI](https://github.com/BlokasLabs/USBMIDI) library. Upload the [UsbMidiConverter](https://github.com/BlokasLabs/USBMIDI/blob/master/examples/UsbMidiConverter/UsbMidiConverter.ino) example into the Arduino Pro Micro.

Serial1 of ESP32 (re-mapped to pin 18 RX and 19 TX) is connected to Serial1 (pin 1 TX0 and pin 2 RX1) of Arduino Pro Micro. Use a 3.3V Pro Micro board. A 5V Pro Micro board works too because the ESP32 is usually 5V tolerant.

Arduino Pro Micro is powered by the USB MIDI connection.

IMPORTANT: ESP32 board and Arduino Pro Micro must share GND.
