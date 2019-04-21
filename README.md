[![Build Status](https://travis-ci.org/alf45tar/PedalinoMini.svg?branch=master)](https://travis-ci.org/alf45tar/PedalinoMini)

[![](https://github.com/alf45tar/PedalinoMini/blob/master/images/youtube-video.jpg)](https://www.youtube.com/watch?v=ZqCsKDWJVQs)

# PedalinoMini™

Open source wireless MIDI foot controller for guitarists and more.

- Plug-and-play with any MIDI-compatible app on iOS 8 and above as well as OS X Yosemite and above.
- High customizable using iOS or Android app
- Bluetooth and WiFI MIDI interface
- No extra drivers to connect Windows, macOS, iOS (iPad/iPhone) and Android
- Bluetooth LE MIDI (iOS and macOS compatible)
- Network MIDI (aka AppleMIDI or RTP-MIDI)
- ipMIDI
- Open Sound Control (OSC)
- IEEE 802.11 b/g/n Wi-Fi 2.4 GHZ with WPA/WPA2 authentication
- Bluetooth Low Energy 4.0
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

## PedalinoMini™ Breadboard Prototype example

The breadboard shows the connection (4 wires only) between the ESP32 board and the OLED display. Momentary switches are just an example.

![Breadboard](https://github.com/alf45tar/PedalinoMini/blob/master/images/PedalinoMini_bb.png "Breadboard")

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

## <a name="wifi"></a>How to connect PedalinoMini™ to a WiFi network

AppleMIDI, ipMIDI and Open Sound Control (OSC) protocol requires a network connection. PedalinoMini™ support IEEE 802.11 b/g/n WiFi with WPA/WPA2 authentication (only 2.4 GHz).

PedalinoMini™ implements Smart Config technology via [Espressif’s ESP-TOUCH protocol](https://www.espressif.com/en/products/software/esp-touch/overview) to help users connect embedded devices to a WiFi network through simple configuration on a smartphone.

Tested apps for configure SSID and password are:

- [ESP8266 SmartConfig](https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch) for Android

- [Espressif Esptouch](https://itunes.apple.com/us/app/espressif-esptouch/id1071176700?mt=8) for iOS

### Boot procedure

- On power on Pedalino will try to connect to the last know access point
- If it cannot connect to the last used access point within 15 seconds it enters into Smart Config mode
- Start one of the tested apps to configure SSID and password
- If it doesn't receive any SSID and password during the next 15 seconds it enters into WPS mode
- Press WPS button on your WiFi router
- If it doesn't receive any SSID and password during the next 15 seconds it switch to AP mode
- In AP mode PedalinoMini™ create a WiFi network called 'Pedalino-XXXXXXXX' waiting connection from clients. The required password is XXXXXXXX (uppercase). XXXXXXXX is a variable string.
- Reboot Pedalino to restart the procedure.

## How to reset PedalinoMini™ to factory default

### TTGO T-Eight ESP32 procedure

- Press and hold navigation CENTER button
- Press and release POWER button
- Release CENTER button when PedalinoMini™ reboot (after about 10 seconds)