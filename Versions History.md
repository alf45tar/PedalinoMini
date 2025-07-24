# Versions history

## 4.0.0 - July 28th, 2025

- Added delayed actions (3.5.1)
- Fixed "Step by Step -" [#457](https://github.com/alf45tar/PedalinoMini/issues/547) (3.5.2)
- Added BLE keyboard emulation (3.6.0)
- Added USB keyboard emulation (3.6.0)
- Updated onboard Bootstrap to 5.3.7 (3.6.0)
- Moved to close-source with licensing and activation (4.0.0)

## 3.5.0 - October 25th, 2024 - Bonus release for 500 stars

- Fixed simultaneous button press
- Fixed reboot on configuration load
- Fixed reboot on change profile via WebUI
- Fixed bank 0 Program Change leds
- Fixed LILYGO® T-Display-S3 display off on battery
- Fixed LILYGO® T-Display-S3 leds strip and midi out pin overlap
- Increased sequences to 20
- Add support for ADS1115 16-bit analog-to-digital converter for 4 additional Analog pedals
- Modified the voltage detection
- Removed default credentials to connect to WebUI
- Enabled PSRAM for LILYGO® T-Display-S3
- Updated onboard Bootstrap to 5.3.3
- Updated SorttableJS to 1.15.3
- Updated to ArduinoJson v7
- Updated to Adafruit TinyUSB Library version 3 (configuration description override)

## 3.2.2 - February 3st, 2024

- BPI Leaf S3 new pins assignment
- ESPAsyncWebServer from esphome

## 3.2.1 - February 1st, 2024

- BPI Leaf S3 and LILYGO® T-Display-S3 USB MIDI device mode
- LILYGO® T-Display-S3 display support
- Fixed sequence led in configuration file
- Added MIDI Channel "None" (do not send any message but recorded as last message sent to be used with Repeat Overwrite) and "All"
- Fixed sequences loop when last slot is not empty
- Fixed an error preventing AP mode with ESP32 S3
- Added "MIDI Clock Master", "MIDI CLock Slave", "MIDI Clock Off", "Repeat", "Repeat Overwrite" to configuration file
- Updated onboard Bootstrap to 5.3.2
- Better battery voltage detection for BPI Leaf S3
- Updated SortableJS to 1.15.2
- Moved low priority tasks to Core 0
- Added `lilygo-t-display` target board with Espressif 6.5.0

## 3.1.6 - May 1st, 2023

- Fixed power off action

## 3.1.5 - April 30th, 2023

The following features:

- Bootstrap 5.2.2 onboard and latest via internet
- SortableJS 1.15.0
- Latest JSON Editor
- Better buttons placement in WebUI
- Fixed RGB order in cross led refresh
- Fixed BLE boot mode disabled when disabled in Options
- BLE client mode (define BLECLIENT in platformio.ini)
- Redesign of Pedals logic with Controls
- Simultaneous buttons press
- Control Change Snap (thanks to potul)
- OLED display bottom line fix (thanks to potul)
- Tag name truncated fix
- Led color on boot fix
- Add Debounce Interval and Simultaneous Gap Time in Options
- Press & Release event for streamline actions
- Inactivity timeout switch off display and leds
- Added Default as led option in Sequences
- Added "Set Bank" action in Sequences
- Added "Step by Step+" and "Step by Step-" to run sequence step by step forward and backward
- Added latch emulation for momentary switches
- Initial suppport for BPI Leaf S3 (no USB MIDI device/host)
- Increased Controls to 100
- Configuration file up to 256Kb when PSRAM is available
- Configuration file can be appended to current profile (only Actions)
- Switch profile (CC 00 [01-03] on channel 16) and bank (CC 32 [00-20] on channel 1&) via MIDI
- MIDI Clock and MIDI Time Code (MTC) fixes and improvements

has been released to public on April 30th, 2023. Thanks to the new sponsors: SrMorris, FelixMuellCode, MiqViq, serhatsoyyigit, potul, AndreySom, C*********, jimhiggs, A***********, Ratterbass, TarFilarek, S*********, bobvc133, itsptadeu, mosswind,,b*******, m*****, m*******, m********, TheNothingMan.

## 2.5.2 - September 23rd, 2022

Bonus version for doubling the stars on September 2nd, 2022.

- RGB Order saved in NVS
- Minor bug fixes

## 2.5.1 - August 2nd, 2022

The following features:

- Fixed reboot on Options page with a long list of visible WiFi network
- Fixed Note velocity always zero
- Added display flip vertically on TTGO T-Display
- Updated to latest JSON Editor 9.7.4
- Added flip display and leds RGB order in Options
- Fixed memory fragmentation during webpage creation
- A new pedal type to connect an expression pedal and a switch (momentary or ladder) to one port only
- Disabled WiFi power saving to reduce latency
- Added OSC local port, remote host and remote port in Options
- New "OSC Message" action sending integer and float values
- Experimental: replace NVS with JSON files

has been released to public on August 2nd, 2022 thanks to the following sponsors: b*******, basst22778, dbosnyak, d***********, slapukas, Samantha-uk, Poznik, FelixMuellCode, yusufrahadika and s*****.

## 2.4.0 - February 26th, 2022

The following features:

- Installing firmware and WiFi provisioning directly from browser
- Support for ultrasonic ranging module HC-SR04
- Adjusting easing and threshold for analog and ultrasonic sensor
- Fixed a bug that avoid to reach max value when analog calibration is on
- Improved Pedals web UI
- Fixed Program Change leds brightness
- Link each button to a led in Pedals setup in order to streamline Actions definition

has been released to public on February 26th, 2022. Thanks to the new sponsors: ClintR1984, pstechsolutions, mknerr, radioactivetoy, y0m1g, joesuspense, andeee, jsleroy, Viser, anssir, mattzzw.

## 2.3.2 - November 12th, 2021

The following features:

- Bootstrap 5.1.3
- JSON Editor 9.5.6
- Reduced debounce interval from 20ms to 5ms
- Added "Set Led Color" action
- Redesigned Sequences (new web UI, new configuration file) - EEPROM breaking change
- Improved Actions web UI
- Bank duplication

has been released to public on November 12th, 2021. Thanks to the new sponsors: RomanKlein777, michaelhauser, bobvc133, bmarshall91, p*************.

## August 9th, 2021

The following features/fixes:

- Bootstrap 5.0.2
- Fixed BOOT button actions in default configuration
- Fixed compilation error when BATTERY flag defined for board without battery support
- Fixed battery indicator for generic ESP32 board without battery support
- Fixed TTGO T-Display glitches
- Enable/disable incoming and outcoming MIDI messages display
- Universal expression pedal
- Fixed normal banks actions are not triggered when there are no actions in global bank
- Fixed actions disappering on WebUI after profile switch
- Drag & drop banks reorder
- Current bank saved before profile switch (only via pedal)
- Fixed Network MIDI/AppleMIDI/RTP-MIDI not working in AP mode
- Leds effects

has been released to public on August 9th, 2021 thanks to the following sponsors: richardjs, P********, j*****, TarFilarek. wespac001 x 2, DR-Mello, DWSQUIRES, e36910, itsptadeu.

## May 12th, 2021

The following features:

- Detection and actions triggered on REPEAT PRESSED and LONG RELEASE event
- Latest Bootstrap 5 release and WebUI themes

has been released to public on May 12th, 2021 thanks to the following sponsors: @MaxZak2020, @Alt Shift Creative, @serhatsoyyigit, @Kubbik1, @rigr, @jwyse, @teopost, @davidin73, @juani13973, @ba********, @MiqViq, @jisv48.
