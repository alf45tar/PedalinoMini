[![Build Status](https://travis-ci.org/alf45tar/PedalinoMini.svg?branch=master)](https://travis-ci.org/alf45tar/PedalinoMini)

[![](https://github.com/alf45tar/PedalinoMini/blob/master/images/youtube-video.jpg)](https://www.youtube.com/watch?v=ZqCsKDWJVQs)

# PedalinoMini™

Open source wireless MIDI foot controller for guitarists and more.

- Plug-and-play with any MIDI-compatible app on iOS 8 and above as well as OS X Yosemite and above.
- High customizable using iOS or Android app
- Bluetooth, WiFI, USB and legacy MIDI interfaces
- No extra drivers to connect Windows, macOS, iOS (iPad/iPhone) and Android
- Bluetooth LE MIDI (iOS and macOS compatible)
- Network MIDI (aka AppleMIDI or RTP-MIDI)
- ipMIDI
- Open Sound Control (OSC)
- IEEE 802.11 b/g/n Wi-Fi 2.4 GHZ with WPA/WPA2 authentication
- Bluetooth Low Energy 4.0
- DIN MIDI IN and MIDI OUT connectors
- MIDI routing
- MIDI clock master and slave
- MIDI Time Code (MTC) master and slave
- OSC to MIDI and vicecersa
- Any number of pedals of any type in any order
- Auto-sensing footswitches and expression pedals

## Features

- Support for digital foot switches (momentary or latch), analog expression pedals and jog wheels (rotary encoders)
- 10 banks of 6 controllers each
- 3 user configuration profiles
- Each port can connect 1 expression pedal or up to 3 foot switches for a maximum of 18 foot switches.
- MIDI output via AppleMIDI (also known as RTP-MIDI) or ipMIDI via Wi-Fi
- Send the following MIDI events: Program Change, Control Code, Note On/Off or Pitch Bend
- MIDI channel, MIDI note, MIDI control code, MIDI program change can be configured by each pedal and by each bank
- Switch debouncing and analog noise suppression without decreasing responsiveness
- Invert polarity via software
- Individual automatic calibration of expression pedals. Manual fine tuning is not usually requested.
- Transform a linear expression pedal into log expression pedal and vice versa
- Responsive and mobile-first configuration web interface (http://pedalino.local)
- Smart Config technology to help users connect to a Wi-Fi network through simple app on a smartphone.
- Firmware update via HTTP (<http://pedalino.local/update>)

## iOS and Android App

The app made with [Blynk](https://www.blynk.cc) is on the way.

## Bill of materials

The shortest bill of materials ever: an ESP32 board and a OLED display. That's it.

  - Any ESP32 board supported by [Arduino core for ESP32 WiFi chip](https://github.com/espressif/arduino-esp32)
    - Tested on [DOIT ESP32 DevKit V1](https://github.com/SmartArduino/SZDOITWiKi/wiki/ESP8266---ESP32) 4M dual-mode Wi-Fi and Bluetooth module
  - OLED I2C 0.96"/1.3" display 128x64 pixels SSD1306/SH1106 based

Not enough short?

  - An all-in-one [TTGO T-Eight ESP32](http://www.lilygo.cn/claprod_view.aspx?TypeId=21&Id=987&FId=t28:21:28) with a bigger OLED display (1.3"), 4MB PSRAM and lithium battery interface

USB MIDI and DIN MIDI connection requires additional hardware.

## Schematic

![Schematic1](https://github.com/alf45tar/PedalinoMini/blob/master/images/Schematic_PedalinoMini_Sheet-1.svg "Schematic1")
![Schematic2](https://github.com/alf45tar/PedalinoMini/blob/master/images/Schematic_PedalinoMini_Sheet-2.svg "Schematic2")

## USB MIDI

The cheapest way to implement an USB MIDI connection is using an Arduino Uno R3 (or Mega) as USB-to-Serial converter with a USB class-compliant MIDI firmware for ATmega16U2.

PedalinoMini™ is tested with [mocoLUFA](https://github.com/kuwatay/mocolufa). mocoLUFA supports dual mode boot (USB-MIDI or Arduino-Serial) and high-speed mode (1 Mbps) even if we don't need them. More information can be obtained [here](https://github.com/tttapa/MIDI_controller).

No sketch is needed for ATmega328P. ATmega328P is disabled connecting RESET to GND.

Serial1 of ESP32 (re-mapped to pin 18 RX and 19 TX) is connected to Serial (pin 0 and 1) of Arduino Uno R3. ESP32 is usually 5V tolerant.

Arduino Uno is powered by the USB MIDI connection. 

IMPORTANT: ESP32 board and Arduino Uno R3 must share GND.

## OLED display screenshots

Home screen|Device info|MIDI message
-----|-----|-----
![OLED Screen 1](https://github.com/alf45tar/PedalinoMini/blob/master/images/oled-home.png "Screen 1")|![OLED Screen 2](https://github.com/alf45tar/PedalinoMini/blob/master/images/oled-device.png "Screen 2")|![OLED Screen 3](https://github.com/alf45tar/PedalinoMini/blob/master/images/oled-midi.png "Screen 3")

MIDI Clock|MIDI Time Code
-----|-----
![OLED Screen 4](https://github.com/alf45tar/PedalinoMini/blob/master/images/oled-midi-clock.png "Screen 4")|![OLED Screen 5](https://github.com/alf45tar/PedalinoMini/blob/master/images/oled-mtc.png "Screen 5")

## WEB user interface

![WEB Screen 1](https://github.com/alf45tar/PedalinoMini/blob/master/images/web-home.png "Screen 1")

![WEB Screen 2](https://github.com/alf45tar/PedalinoMini/blob/master/images/web-banks.png "Screen 2")

![WEB Screen 3](https://github.com/alf45tar/PedalinoMini/blob/master/images/web-pedals.png "Screen 3")

![WEB Screen 4](https://github.com/alf45tar/PedalinoMini/blob/master/images/web-interfaces.png "Screen 4")

![WEB Screen 5](https://github.com/alf45tar/PedalinoMini/blob/master/images/web-options.png "Screen 5")

## How to connect PedalinoMini™ to a WiFi network

PedalinoMini™ supports IEEE 802.11 b/g/n WiFi with WPA/WPA2 authentication (only 2.4 GHz).

PedalinoMini™ implements Wi-Fi Protected Setup (WPS) and Smart Config technology ([Espressif’s ESP-TOUCH protocol](https://www.espressif.com/en/products/software/esp-touch/overview)). WPS needs access to the WPS button on the WiFi router. Smart Config requires a smartphone with one the following apps:

- [ESP8266 SmartConfig](https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch) for Android
- [Espressif Esptouch](https://itunes.apple.com/us/app/espressif-esptouch/id1071176700?mt=8) for iOS

If the WiFi network is not available PedalinoMini™ will create an hotspot for you. Connect all your device to it.

- On power on Pedalino will try to connect to the last know access point
- If it cannot connect to the last used access point within 15 seconds it enters into Smart Config mode
- Start one of the suggested apps to configure SSID and password
- If it doesn't receive any SSID and password during the next 15 seconds it enters into WPS mode
- Press WPS button on your WiFi router
- If it doesn't receive any SSID and password during the next 15 seconds it switch to AP mode
- In AP mode PedalinoMini™ create a WiFi network called 'Pedalino-XXXXXXXX' waiting connection from clients. The required password is XXXXXXXX (uppercase). XXXXXXXX is a variable string.
- Reboot Pedalino to restart the procedure.

```
void wifi_connect()
{
  auto_reconnect();           // WIFI_CONNECT_TIMEOUT seconds to reconnect to last used access point
  if (!WiFi.isConnected())
    smart_config();           // SMART_CONFIG_TIMEOUT seconds to receive SmartConfig parameters and connect
  if (!WiFi.isConnected())
    wps_config();             // WPS_TIMEOUT seconds to receive WPS parameters and connect
  if (!WiFi.isConnected())
    ap_mode_start();          // switch to AP mode until next reboot
}
```

## How to start PedalinoMini™ in AP mode (skipping last know access point, SmartConfig and WPS)

See [How to reset PedalinoMini™ to factory default](https://github.com/alf45tar/PedalinoMini/blob/master/README.md#how-to-reset-pedalinomini-to-factory-default)

## How to start PedalinoMini™ with a specific profile

### TTGO T-Eight ESP32 procedure

- Press and hold navigation LEFT button for profile A or navigation RIGHT button for profile C
- Press and release POWER button
- Release LEFT or RIGHT button when the WiFi/Bluetooth logo disappear

## How to reset PedalinoMini™ to factory default

### DOIT ESP32 DevKit V1 procedure

- Press and release EN button and immediately after press and hold BOOT button
- When PedalinoMini™ reboot (after about 10 seconds) release BOOT button
- PedalinoMini™ will start in AP mode if you release CENTER button before reboot

### TTGO T-Eight ESP32 procedure

- Press and hold navigation CENTER button
- Press and release POWER button
- When PedalinoMini™ reboot (after about 10 seconds) release CENTER button 
- PedalinoMini™ will start in AP mode if you release CENTER button before reboot
