/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___    
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \   
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \  
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  ) 
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /  
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/   
                                                                                   (c) 2018-2019 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

/*
    ESP32 Pedalino Mini

      - Serial MIDI
      - WiFi AppleMIDI a.k.a. RTP-MIDI a.k.a. Network MIDI
      - ipMIDI
      - Bluetooth LE MIDI
      - WiFi OSC
*/

#ifdef NOWIFI
#undef WIFI
#define NOWEBCONFIG
#define NOBLYNK
#else
#define WIFI
#define BOOTSTRAP_LOCAL
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
#ifdef TTGO_T_EIGHT
#include "esp32-hal-psram.h"
#endif

#include <esp_log.h>
#include <string>
#include "Pedalino.h"
#include "TickerTimer.h"
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
#include "WebConfigAsync.h"
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
  bool apmode = false;

  pinMode(WIFI_LED, OUTPUT);

#ifdef ARDUINO_ARCH_ESP32
  pinMode(BLE_LED, OUTPUT);
#endif

#if defined(ARDUINO_ARCH_ESP32) && defined(DEBUG_ESP_PORT)
  //esp_log_level_set("*",      ESP_LOG_ERROR);
  //esp_log_level_set("wifi",   ESP_LOG_WARN);
  //esp_log_level_set("BLE*",   ESP_LOG_ERROR);
  //esp_log_level_set(LOG_TAG,  ESP_LOG_INFO);
#endif

#ifdef SERIALDEBUG
  SERIALDEBUG.begin(115200);
  SERIALDEBUG.setDebugOutput(true);   // enable diagnostic output and printf() output
#endif

  DPRINT("ChipRevision %d, CPU Freq %d MHz, SDK Version %s\n",ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
  DPRINT("Flash Size %d, Flash Speed %d Hz\n",ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  DPRINT("PSRAM Total Heap %d, PSRAM Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram()); 

  DPRINTLN("_________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___ ");
  DPRINTLN("\\______  \\ ____   __| _/____  |  | |__| ____   ____   /     \\ |__| ____ |__|   /  / \\__    ___/     \\   \\  \\ ");
  DPRINTLN("|     ___// __ \\ / __ |\\__  \\ |  | |  |/    \\ /  _ \\ /  \\ /  \\|  |/    \\|  |  /  /    |    | /  \\ /  \\   \\  \\ ");
  DPRINTLN("|    |   \\  ___// /_/ | / __ \\|  |_|  |   |  (  <_> )    Y    \\  |   |  \\  | (  (     |    |/    Y    \\   )  )");
  DPRINTLN("|____|    \\___  >____ |(____  /____/__|___|  /\\____/\\____|__  /__|___|  /__|  \\  \\    |____|\\____|__  /  /  / ");
  DPRINTLN("              \\/     \\/     \\/             \\/               \\/        \\/       \\__\\                 \\/  /__/ ");
  DPRINTLN("                                                                                  (c) 2018-2019 alf45star      ");
  DPRINTLN("                                                                      https://github.com/alf45tar/PedalinoMini");
  DPRINT("\nHostname: %s\n", host.c_str());

  display_init();

  eeprom_init();

  // Reset to factory default if BOOT key is pressed and hold for alt least 8 seconds at power on
  // Enter AP mode if BOOT key is pressed and hold for less than 8 seconds at power on
  pinMode(FACTORY_DEFAULT_PIN, INPUT_PULLUP);
  unsigned long milliStart = millis();
  unsigned long duration = 0;
  lcdClear();
  if (digitalRead(FACTORY_DEFAULT_PIN) == LOW) display_progress_bar_title("Factory reset");
  while ((digitalRead(FACTORY_DEFAULT_PIN) == LOW) && (duration < 8500)) {
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
  if ((digitalRead(FACTORY_DEFAULT_PIN) == HIGH) && (duration > 100 && duration < 8500)) {
    DPRINT("\nSkip connection to last AP and/or SmartConfig/WPS setup\n");
    apmode = true;
  } else if ((digitalRead(FACTORY_DEFAULT_PIN) == LOW) && (duration >= 8500)) {
    DPRINT("\nReset EEPROM to factory default\n");
    lcdSetCursor(0, 1);
    lcdPrint("Factory default ");
    delay(1000);
    eeprom_initialize_to_zero();
    load_factory_default();
    eeprom_update();
    //ESP.eraseConfig();
    //ESP.reset();
  }

#ifdef TTGO_T_EIGHT
  pinMode(PROFILE_A_PIN, INPUT_PULLUP);
  if (digitalRead(PROFILE_A_PIN) == LOW) {
    currentProfile = 0;
    eeprom_update_current_profile(currentProfile);
  }
  pinMode(PROFILE_C_PIN, INPUT_PULLUP);
  if (digitalRead(PROFILE_C_PIN) == LOW) {
    currentProfile = 2;
    eeprom_update_current_profile(currentProfile);
  }
#endif
  eeprom_read();

  // Initiate serial MIDI communications, listen to all channels and turn Thru on/off
  serial_midi_connect();              // On receiving MIDI data callbacks setup
  DPRINT("USB MIDI started\n");
  DPRINT("DIN MIDI started\n");

#ifdef WIFI
  // Write SSID/password to flash only if currently used values do not match what is already stored in flash
  WiFi.persistent(false);
  WiFi.onEvent(WiFiEvent);
  if (apmode)
    ap_mode_start();
  else 
    wifi_connect();
#endif

#ifdef PEDALINO_TELNET_DEBUG
  // Initialize the telnet server of RemoteDebug
  Debug.begin(host);              // Initiaze the telnet server
  Debug.setResetCmdEnabled(true); // Enable the reset command
#endif

#ifdef BLE
  // BLE MIDI service advertising
  ble_midi_start_service();
  DPRINT("BLE MIDI service advertising started\n");
#endif

  autosensing_setup();
  controller_setup();
  mtc_setup();
  blynk_setup();
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

  // Listen to incoming messages
  if (USB_MIDI.read())
    DPRINTMIDI("USB MIDI", USB_MIDI.getType(), USB_MIDI.getChannel(), USB_MIDI.getData1(), USB_MIDI.getData2());
  if (DIN_MIDI.read())
    DPRINTMIDI("Serial MIDI", DIN_MIDI.getType(), DIN_MIDI.getChannel(), DIN_MIDI.getData1(), DIN_MIDI.getData2());

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