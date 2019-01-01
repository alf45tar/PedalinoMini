/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */


/*
    ESP8266/ESP32 Pedalino Mini

      - Serial MIDI
      - WiFi AppleMIDI a.k.a. RTP-MIDI a.k.a. Network MIDI
      - ipMIDI
      - Bluetooth LE MIDI
      - WiFi OSC
*/

#ifdef NOWIFI
#undef WIFI
#else
#define WIFI
#endif

#ifdef NOBLE
#undef BLE
#else
#define BLE
#endif

#ifdef NOWEBCONFIG
#undef WEBCONFIG
#else
#define WEBCONFIG
#endif

#ifdef NOBLYNK
#undef BLYNK
#else
#define BLYNK
#endif

#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP8266
#undef BLE
#define NOBLE
#undef BOOTSTRAP_LOCAL
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <esp_log.h>
#include <string>
#endif

#include "Pedalino.h"
#include "Config.h"
#include "UdpMidiOut.h"
#include "BLEMidiOut.h"
#include "SerialMidi.h"
#include "UdpMidiIn.h"
#include "BLEMidiIn.h"
#include "Controller.h"
#include "BlynkRPC.h"
#include "DisplayLCD.h"
#include "DisplayOLED.h"
#ifdef ASYNC_WEB_SERVER
#include "WebConfigAsync.h"
#else
#include "WebConfig.h"
#endif
#include "OTAUpdate.h"
#include "WifiConnect.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN    2
#endif

#ifndef WIFI_LED
#define WIFI_LED        LED_BUILTIN  // onboard LED, used as status indicator
#endif

#ifndef BLE_LED
#define BLE_LED         LED_BUILTIN  // onboard LED, used as status indicator
#endif


void setup()
{
  pinMode(WIFI_LED, OUTPUT);

#ifdef ARDUINO_ARCH_ESP32
  pinMode(BLE_LED, OUTPUT);
#endif

#if defined(ARDUINO_ARCH_ESP32) && defined(DEBUG_ESP_PORT)
  esp_log_level_set("*",      ESP_LOG_ERROR);
  //esp_log_level_set("wifi",   ESP_LOG_WARN);
  //esp_log_level_set("BLE*",   ESP_LOG_ERROR);
  esp_log_level_set(LOG_TAG,  ESP_LOG_INFO);
#endif

#ifdef SERIALDEBUG
  SERIALDEBUG.begin(115200);
  SERIALDEBUG.setDebugOutput(true);   // enable diagnostic output and printf() output
#endif

  DPRINTLN("  __________           .___      .__  .__                   ___ ________________    ___");
  DPRINTLN("  \\______   \\ ____   __| _/____  |  | |__| ____   ____     /  / \\__    ___/     \\   \\  \\");
  DPRINTLN("   |     ___// __ \\ / __ |\\__  \\ |  | |  |/    \\ /  _ \\   /  /    |    | /  \\ /  \\   \\  \\");
  DPRINTLN("   |    |   \\  ___// /_/ | / __ \\|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \\   )  )");
  DPRINTLN("   |____|    \\___  >____ |(____  /____/__|___|  /\\____/   \\  \\    |____|\\____|__  /  /  /");
  DPRINTLN("                 \\/     \\/     \\/             \\/           \\__\\                 \\/  /__/");
  DPRINTLN("                                                                (c) 2018 alf45star");
  DPRINTLN("                                                        https://github.com/alf45tar/Pedalino");
  DPRINT("\nHostname: %s\n", host.c_str());

  display_init();

  eeprom_init();

  // Reset to factory default if RIGHT key is pressed and hold for alt least 8 seconds at power on
  // Enter serial passthrough mode if RIGHT key is pressed adn hold for less than 8 seconds at power on
  pinMode(PIN_D(0), INPUT_PULLUP);
  unsigned long milliStart = millis();
  unsigned long duration = 0;
  lcdClear();
  if (digitalRead(PIN_D(0)) == LOW) display_progress_bar_title("Factory reset");
  while ((digitalRead(PIN_D(0)) == LOW) && (duration < 8500)) {
    DPRINT("#");
    lcdSetCursor(duration / 500, 0);
    lcdPrint("#");
    display_progress_bar_update(duration, 8500);
    WIFI_LED_ON();
    delay(50);
    WIFI_LED_OFF();
    delay(50);
    duration = millis() - milliStart;
  }
  //display_clear();
  if ((digitalRead(PIN_D(0)) == HIGH) && (duration > 100 && duration < 8500)) {
    DPRINT("\nSerial passthrough mode for ESP firmware update and monitor\n");

  } else if ((digitalRead(PIN_D(0)) == LOW) && (duration >= 8500)) {
    DPRINT("\nReset EEPROM to factory default\n");
    lcdSetCursor(0, 1);
    lcdPrint("Factory default ");
    delay(1000);
    //eeprom_initialize_to_zero();
    load_factory_default();
    eeprom_update();
    //ESP.eraseConfig();
    //ESP.reset();
  }
  eeprom_read();

#ifdef ARDUINO_ARCH_ESP8266
  SerialMIDI.begin(SERIALMIDI_BAUD_RATE);
#endif
#ifdef ARDUINO_ARCH_ESP32
  SerialMIDI.begin(SERIALMIDI_BAUD_RATE, SERIAL_8N1, SERIALMIDI_RX, SERIALMIDI_TX);
#endif

  serial_midi_connect();              // On receiving MIDI data callbacks setup
  DPRINT("Serial MIDI started\n");

#ifdef BLE
  // BLE MIDI service advertising
  ble_midi_start_service();
  DPRINT("BLE MIDI service advertising started");
#endif

#ifdef WIFI
  // Write SSID/password to flash only if currently used values do not match what is already stored in flash
  WiFi.persistent(false);
  WiFi.onEvent(WiFiEvent);
  wifi_connect();
#endif

#ifdef PEDALINO_TELNET_DEBUG
  // Initialize the telnet server of RemoteDebug
  Debug.begin(host);              // Initiaze the telnet server
  Debug.setResetCmdEnabled(true); // Enable the reset command
#endif

  autosensing_setup();
  controller_setup();
  mtc_setup();
}


void loop()
{
#ifdef WIFI
  if (!appleMidiConnected) WIFI_LED_OFF();
#endif
#ifdef BLE
  if (!bleMidiConnected)  BLE_LED_OFF();
#endif
  if (appleMidiConnected ||  bleMidiConnected) {
    // led fast blinking (5 times per second)
    if (millis() - wifiLastOn > 200) {
      if (bleMidiConnected) BLE_LED_ON();
      if (appleMidiConnected) WIFI_LED_ON();
      wifiLastOn = millis();
    }
    else if (millis() - wifiLastOn > 100) {
      BLE_LED_OFF();
      WIFI_LED_OFF();
    }
  }
#ifdef WIFI
  else
    // led always on if connected to an AP or one or more client connected the the internal AP
    switch (WiFi.getMode()) {
      case WIFI_STA:
        WiFi.isConnected() ? WIFI_LED_ON() : WIFI_LED_OFF();
        break;
      case WIFI_AP:
        WiFi.softAPgetStationNum() > 0 ? WIFI_LED_ON() : WIFI_LED_OFF();
        break;
      default:
        WIFI_LED_OFF();
        break;
    }
#endif

  screen_update();

  MTC.loop();

  // Check whether the input has changed since last time, if so, send the new value over MIDI
  controller_run();

  // Listen to incoming messages from Arduino
  if (MIDI.read())
    DPRINTMIDI("Serial MIDI", MIDI.getType(), MIDI.getChannel(), MIDI.getData1(), MIDI.getData2());

  // Listen to incoming AppleMIDI messages from WiFi
  rtpMIDI_listen();

  // Listen to incoming ipMIDI messages from WiFi
  ipMIDI_listen();

  // Listen to incoming OSC UDP messages from WiFi
  oscUDP_listen();

  // Update display
  display_update();

  http_run();

  // Run OTA update service
  ota_handle();

  // Process Blynk messages
  blynk_run();

#ifdef PEDALINO_TELNET_DEBUG
  // Remote debug over telnet
  Debug.handle();
#endif
}