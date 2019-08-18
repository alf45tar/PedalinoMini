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
#include <esp_bt_main.h>
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


void IRAM_ATTR onButtonLeft()
{
  if (reloadProfile) return;
  currentProfile = (currentProfile == 0 ? PROFILES - 1 : currentProfile - 1);
  reloadProfile = true;
}

void IRAM_ATTR onButtonRight()
{
  if (reloadProfile) return;
  currentProfile = (currentProfile == (PROFILES - 1) ? 0 : currentProfile + 1);
  reloadProfile = true;
}

void IRAM_ATTR onButtonCenter()
{
  if (saveProfile) return;
  saveProfile = true;
}

void setup()
{
  pinMode(WIFI_LED, OUTPUT);
  pinMode(BLE_LED, OUTPUT);

#ifdef DEBUG_ESP_PORT
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

#ifdef TTGO_T_EIGHT
  pinMode(LEFT_PIN, INPUT_PULLUP);
  if (digitalRead(LEFT_PIN) == LOW) {
    //currentProfile = 0;
    //eeprom_update_current_profile(currentProfile);
    bleEnabled = false;
  }
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  if (digitalRead(RIGHT_PIN) == LOW) {
    //currentProfile = 2;
    //eeprom_update_current_profile(currentProfile);
    wifiEnabled = false;
  }
#endif

#ifdef BLE
  if (bleEnabled) {
    // Release Bluetooth Classic memory
    long before = ESP.getFreeHeap();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_CLASSIC_BT);
    long after = ESP.getFreeHeap();
    DPRINT("Bluetooth Classic disabled: %ld bytes released\n", after - before);
  }
  else {
    // Release Bluetooth memory
    long before = ESP.getFreeHeap();
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_BTDM);
    long after = ESP.getFreeHeap();
    DPRINT("Bluetooth disabled: %ld bytes released\n", after - before);
  }
#endif

#ifdef WIFI
  if (!wifiEnabled) {
    // Release WiFi memory
    long before = ESP.getFreeHeap();
    WiFi.mode(WIFI_OFF);
    long after = ESP.getFreeHeap();
    DPRINT("WiFi disabled: %ld bytes released\n", after - before);
  }
#endif

#if __BOARD_HAS_PSRAM__
  banks = (bank**)heap_caps_malloc(BANKS*sizeof(bank*), MALLOC_CAP_SPIRAM);
  for (byte b = 0; b < BANKS; b++)
    banks[b] = (bank*)heap_caps_malloc(PEDALS*sizeof(bank), MALLOC_CAP_SPIRAM);

  pedals = (pedal*)heap_caps_malloc(PEDALS*sizeof(pedal), MALLOC_CAP_SPIRAM);
  
  sequences = (sequence**)heap_caps_malloc(SEQUENCES*sizeof(sequence*), MALLOC_CAP_SPIRAM);
  for (byte s = 0; s < SEQUENCES; s++)
    sequences[s] = (sequence*)heap_caps_malloc(STEPS*sizeof(sequence), MALLOC_CAP_SPIRAM);

  lastMIDIMessage = (message*)heap_caps_malloc(BANKS*sizeof(message), MALLOC_CAP_SPIRAM);
#endif

  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  DPRINT("PSRAM Total Heap %d, PSRAM Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram()); 

  display_init();

  eeprom_init_or_erase();

  // Reset to factory default if BOOT key is pressed and hold for alt least 12 seconds at power on
  
  pinMode(FACTORY_DEFAULT_PIN, INPUT_PULLUP);
  unsigned long milliStart = millis();
  unsigned long duration = 0;
  lcdClear();
  bootMode = PED_BOOT_NORMAL;
  if (digitalRead(FACTORY_DEFAULT_PIN) == LOW)
    display_progress_bar_title2("Release button for", "Normal Boot");
  while ((digitalRead(FACTORY_DEFAULT_PIN) == LOW) && (duration < 12000)) {
    if (duration > 1000 && duration < 3000 && bootMode != PED_BOOT_BLE) {
      bootMode = PED_BOOT_BLE;
      display_progress_bar_title2("Release button for", "Bluetooth Only");
    }
    else if (duration > 3000 && duration < 5000 && bootMode != PED_BOOT_WIFI) {
      bootMode = PED_BOOT_WIFI;
      display_progress_bar_title2("Release button for", "WiFi Only");
    }
    else if (duration > 5000 && duration < 7000 && bootMode != PED_BOOT_AP) {
      bootMode = PED_BOOT_AP;
      display_progress_bar_title2("Release button for", "Access Point");
    }
    else if (duration > 7000 && duration < 9000 && bootMode != PED_BOOT_AP_NO_BLE) {
      bootMode = PED_BOOT_AP_NO_BLE;
      display_progress_bar_title2("Release button for", "AP without BLE");
    }
    else if (duration > 9000 && duration < 12000 && bootMode != PED_FACTORY_DEFAULT) {
      bootMode = PED_FACTORY_DEFAULT;
      display_progress_bar_title2("Hold button for", "Factory Default");
    }
    DPRINT("#");
    lcdSetCursor(duration / 500, 0);
    lcdPrint("#");
    display_progress_bar_update(duration, 12000);
    WIFI_LED_ON();
    delay(50);
    WIFI_LED_OFF();
    delay(50);
    duration = millis() - milliStart;
  }
  //display_clear();
  switch (bootMode) { 
    case PED_BOOT_NORMAL:
      break;

    case PED_BOOT_BLE:
#ifdef BLE
      bleEnabled = true;
#else
      bleEnabled = false;
#endif
      wifiEnabled = false;    
      break;

    case PED_BOOT_WIFI:
    case PED_BOOT_AP_NO_BLE:
      bleEnabled = false;
#ifdef WIFI
      wifiEnabled = true;
#else
      wifiEnabled = false;
#endif
      break;

    case PED_BOOT_AP:
      break;

    case PED_FACTORY_DEFAULT:
      if (duration > 12000) {
        DPRINT("\nReset EEPROM to factory default\n");
        lcdSetCursor(0, 1);
        lcdPrint("Factory default ");
        delay(1000);
        eeprom_initialize();
        //ESP.restart();
      }
      else
        bootMode = PED_BOOT_NORMAL;
      break;
      
  }
  
  eeprom_read_global();

  // Initiate serial MIDI communications, listen to all channels and turn Thru on/off
  serial_midi_connect();              // On receiving MIDI data callbacks setup
  DPRINT("USB MIDI started\n");
  DPRINT("DIN MIDI started\n");

#ifdef BLE
  if (bleEnabled) {
    // BLE MIDI service advertising
    ble_midi_start_service();
    DPRINT("BLE MIDI service advertising started\n");
  }
 #endif

#ifdef WIFI
  if (wifiEnabled) {
    WiFi.persistent(false);
    WiFi.onEvent(WiFiEvent);
    if (bootMode == PED_BOOT_AP || bootMode == PED_BOOT_AP_NO_BLE) {
      DPRINT("Skipped connection to last AP and/or SmartConfig/WPS setup\n");
      ap_mode_start();
    }
    else 
      wifi_connect();

    blynk_setup();

#ifdef PEDALINO_TELNET_DEBUG
    // Initialize the telnet server of RemoteDebug
    Debug.begin(host);              // Initiaze the telnet server
    Debug.setResetCmdEnabled(true); // Enable the reset command
#endif
  }
#endif

  attachInterrupt(LEFT_PIN,   onButtonLeft,   FALLING);
  attachInterrupt(CENTER_PIN, onButtonCenter, FALLING);
  attachInterrupt(RIGHT_PIN,  onButtonRight,  FALLING);

  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
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

  if (wifiEnabled) {
    // Listen to incoming AppleMIDI messages from WiFi
    rtpMIDI_listen();

    // Listen to incoming ipMIDI messages from WiFi
    ipMIDI_listen();

    // Listen to incoming OSC UDP messages from WiFi
    oscUDP_listen();

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

  // Update display
  display_update();
}