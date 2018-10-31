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

#define NOLCD
#define DEBUG_UPDATER Serial

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

#include <Arduino.h>

#ifdef ARDUINO_ARCH_ESP8266
#undef BLE
#define NOBLE
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <esp_log.h>
#include <string>
#define NOWEBCONFIG   // temporary
#endif

#include "Pedalino.h"
#include "Controller.h"
#include "BlynkRPC.h"
#include "Config.h"
#include "Display.h"
#include "OTAUpdate.h"
#include "BLEMidi.h"
#include "UdpMidiOut.h"
#include "SerialMidi.h"
#include "UdpMidiIn.h"
#include "WebConfig.h"
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
#ifdef ARDUINO_ARCH_ESP32
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

  display_init();

  eeprom_init();

  // Reset to factory default if RIGHT key is pressed and hold for alt least 8 seconds at power on
  // Enter serial passthrough mode if RIGHT key is pressed adn hold for less than 8 seconds at power on
  pinMode(PIN_D(0), INPUT_PULLUP);
  unsigned long milliStart = millis();
  unsigned long duration = 0;
#ifndef NOLCD
  lcd.clear();
#endif
  while ((digitalRead(PIN_D(0)) == LOW) && (duration < 8500)) {
    DPRINT("#");
#ifndef NOLCD
    lcd.setCursor(duration / 500, 0);
    lcd.print("#");
#endif
    delay(100);
    duration = millis() - milliStart;
  }
  DPRINTLN("");
  if ((digitalRead(PIN_D(0)) == HIGH) && (duration > 100 && duration < 8500)) {
    DPRINTLN("Serial passthrough mode for ESP firmware update and monitor");

  } else if ((digitalRead(PIN_D(0)) == LOW) && (duration >= 8500)) {
    DPRINTLN("Reset EEPROM to factory default");
#ifndef NOLCD
    lcd.setCursor(0, 1);
    lcd.print("Factory default ");
    delay(1000);
#endif
    load_factory_default();
    update_eeprom();
  }
  read_eeprom();

#ifdef ARDUINO_ARCH_ESP8266
  SerialMIDI.begin(SERIALMIDI_BAUD_RATE);
#endif
#ifdef ARDUINO_ARCH_ESP32
  SerialMIDI.begin(SERIALMIDI_BAUD_RATE, SERIAL_8N1, SERIALMIDI_RX, SERIALMIDI_TX);
#endif

  serial_midi_connect();              // On receiving MIDI data callbacks setup
  DPRINT("Serial MIDI started\n");

  autosensing_setup();
  controller_setup();
  mtc_setup();
  blynk_config();

  pinMode(WIFI_LED, OUTPUT);

#ifdef ARDUINO_ARCH_ESP32
  pinMode(BLE_LED, OUTPUT);
#endif

#ifndef NOBLE
  // BLE MIDI service advertising
  ble_midi_start_service();
  DPRINT("BLE MIDI service advertising started");
#endif

#ifndef NOWIFI
  // Write SSID/password to flash only if currently used values do not match what is already stored in flash
  WiFi.persistent(false);
  WiFi.onEvent(WiFiEvent);
  wifi_connect();
#endif

#ifndef NOBLYNK
  // Connect to Blynk
  Blynk.config(blynkAuthToken);
  Blynk.connect();
#endif

#ifdef PEDALINO_TELNET_DEBUG
  // Initialize the telnet server of RemoteDebug
  Debug.begin(host);              // Initiaze the telnet server
  Debug.setResetCmdEnabled(true); // Enable the reset command
#endif
}


void loop()
{
  if (!appleMidiConnected) WIFI_LED_OFF();
  if (!bleMidiConnected)  BLE_LED_OFF();
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
#ifndef NOWIFI
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

  // Check whether the input has changed since last time, if so, send the new value over MIDI
  midi_refresh();

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
  //ui.update();

  // Run web server
  httpServer.handleClient();

  // Run OTA update service
  ota_handle();

  // Process Blynk messages
  blynk_run();

#ifdef PEDALINO_TELNET_DEBUG
  // Remote debug over telnet
  Debug.handle();
#endif
}
