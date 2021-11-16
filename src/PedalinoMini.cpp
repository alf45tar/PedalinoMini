/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2021 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

/*
    ESP32 PedalinoMini™ Mini

      - Serial MIDI
      - WiFi AppleMIDI a.k.a. RTP-MIDI a.k.a. Network MIDI
      - ipMIDI
      - Bluetooth LE MIDI
      - WiFi OSC
*/

#ifdef NOWIFI
#undef WIFI
#define NOWEBCONFIG
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
#undef WEBSOCKET
#else
#define WEBCONFIG
#endif

#include <Arduino.h>
#ifdef TTGO_T_EIGHT
#include "esp32-hal-psram.h"
#endif

#include <esp32-hal-log.h>
#include <esp_adc_cal.h>
#include <esp_partition.h>
#include <esp_bt_main.h>
#include <string>
#include "Pedalino.h"
#include "TickerTimer.h"
#include "Config.h"
#include "UdpMidiOut.h"
#include "BLEMidiOut.h"
#include "SerialMidiIn.h"
#include "UdpMidiIn.h"
#include "BLEMidiIn.h"
#include "LedsEffects.h"
#include "DisplayLCD.h"
#ifdef TTGO_T_DISPLAY
#include "DisplayTFT.h"
#else
#include "DisplayOLED.h"
#endif
#include "Controller.h"
#include "OTAHTTPS.h"
#include "WebConfigAsync.h"
#include "OTAUpdateArduino.h"
#include "WifiConnect.h"


void IRAM_ATTR onButtonLeft()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    if (reloadProfile) return;
    currentProfile = (currentProfile == 0 ? PROFILES - 1 : currentProfile - 1);
    reloadProfile = true;
  }
  last_interrupt_time = interrupt_time;
}

void IRAM_ATTR onButtonRight()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    if (reloadProfile) return;
    currentProfile = (currentProfile == (PROFILES - 1) ? 0 : currentProfile + 1);
    reloadProfile = true;
  }
  last_interrupt_time = interrupt_time;
}

void boot_button_event_handler(AceButton* button, uint8_t eventType, uint8_t buttonState)
{
  DPRINT("Pedal: Boot    Button: %d    EventType: %d     ButtonState: %d\n", button->getId(), eventType, buttonState);

  switch (eventType) {

    case AceButton::kEventClicked:
      /*
      if (!reloadProfile) {
        currentProfile = (currentProfile == (PROFILES - 1) ? 0 : currentProfile + 1);
        reloadProfile = true;
      }
      */
      currentBank = (currentBank == BANKS - 1 ? 0 : currentBank + 1);
      leds_refresh();
      break;

    case AceButton::kEventDoubleClicked:
      /*
      if (!reloadProfile) {
        currentProfile = (currentProfile == 0 ? PROFILES - 1 : currentProfile - 1);
        reloadProfile = true;
      }
      */
      currentBank = (currentBank == 0 ? BANKS - 1 : currentBank - 1);
      leds_refresh();
      break;

    case AceButton::kEventLongPressed:
      scrollingMode = !scrollingMode;
      leds_refresh();
      break;
  }
}

void wifi_and_battery_level() {

  if (interruptCounter1 > 0) {

    interruptCounter1 = 0;

    freeMemory = ESP.getFreeHeap();

#ifdef WIFI
    if (wifiEnabled) wifiLevel = (3 * wifiLevel + WiFi.RSSI()) / 4;
#endif

#ifdef BATTERY
#ifdef BATTERY_ADC_EN
    /*
      BATTERY_ADC_EN is the ADC detection enable port
      If the USB port is used for power supply, it is turned on by default.
      If it is powered by battery, it needs to be set to high level
    */
    pinMode(BATTERY_ADC_EN, OUTPUT);
    digitalWrite(BATTERY_ADC_EN, HIGH);
    delay(10);
#endif
    uint16_t v = analogRead(BATTERY_PIN);
#ifdef BATTERY_ADC_EN
    digitalWrite(BATTERY_ADC_EN, LOW);
#endif
    uint16_t voltage = ((uint32_t)v * 2 * 33 * vref) / 10240;   //  float voltage = ((float)v / 1024.0) * 2.0 * 3.3 * (vref / 1000.0);
    uint16_t difference = abs(voltage - batteryVoltage);
    //DPRINT("%d %d %d\n", voltage, batteryVoltage, difference);
    if (difference > 200)
      batteryVoltage = (uint16_t)voltage;
    else
      batteryVoltage = (7 * batteryVoltage + voltage) / 8;
#endif

#ifdef DIAGNOSTIC
    static byte sec = 0;

    if (sec == 0) {
      memoryHistory[historyStart]  = map2(freeMemory, 0, 200*1024, 0, 100);
      wifiHistory[historyStart]    = map2(constrain(wifiLevel, -90, -10), -90, -10, 0, 100);
      batteryHistory[historyStart] = map2(constrain(batteryVoltage, 3000, 5000), 3000, 5000, 0, 100);
      historyStart = (historyStart + 1) % POINTS;
    }
    sec = (sec + 1) % SECONDS_BETWEEN_SAMPLES;
#endif
  }
}


void setup()
{
  //esp_spiram_add_to_heapalloc();

  display_init();

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
#ifdef TTGO_T_DISPLAY
  //Check type of calibration value used to characterize ADC for BATTERY_PIN (GPIO34)
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t           val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    vref = adc_chars.vref;
    DPRINT("eFuse Vref:%u mV\n", adc_chars.vref);
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    DPRINT("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  } else {
    DPRINT("Default Vref: 1100mV\n");
  }
#endif

  // Setup a 1Hz timer for wifi and battery level monitoring and logging
  Timer1Attach(1000);

  // Setup a 25Hz timer for display update
  Timer3Attach(1000 / 25);

  esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
  if (pi != NULL) {
    const esp_partition_t* nvs = esp_partition_get(pi);
    DPRINT("%s Total %d\n", nvs->label, nvs->size);
    esp_partition_iterator_release(pi);
  }
  DPRINT("PlatformIO Built Environment: %s\n", xstr(PLATFORMIO_ENV));

  DPRINTLN("_________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___ ");
  DPRINTLN("\\______  \\ ____   __| _/____  |  | |__| ____   ____   /     \\ |__| ____ |__|   /  / \\__    ___/     \\   \\  \\ ");
  DPRINTLN("|     ___// __ \\ / __ |\\__  \\ |  | |  |/    \\ /  _ \\ /  \\ /  \\|  |/    \\|  |  /  /    |    | /  \\ /  \\   \\  \\ ");
  DPRINTLN("|    |   \\  ___// /_/ | / __ \\|  |_|  |   |  (  <_> )    Y    \\  |   |  \\  | (  (     |    |/    Y    \\   )  )");
  DPRINTLN("|____|    \\___  >____ |(____  /____/__|___|  /\\____/\\____|__  /__|___|  /__|  \\  \\    |____|\\____|__  /  /  / ");
  DPRINTLN("              \\/     \\/     \\/             \\/               \\/        \\/       \\__\\                 \\/  /__/ ");
  DPRINTLN("                                                                                  (c) 2018-2020 alf45star      ");
  DPRINTLN("                                                                      https://github.com/alf45tar/PedalinoMini");
  DPRINT("\nHostname: %s\n", host.c_str());
  DPRINT("PSRAM%sfound\n", psramFound() ? " " : " not ");

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

  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  DPRINT("PSRAM Total Heap %d, PSRAM Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());

  if (!SPIFFS.begin()) {
      DPRINT("SPIFFS mount FAILED\n");
  }
  else {
    DPRINT("SPIFFS mount OK\n");
  }

  eeprom_init_or_erase();
  eeprom_read_global();

  FastLED.addLeds<NEOPIXEL, FASTLEDS_DATA_PIN>(fastleds, LEDS);
  fill_solid(fastleds, LEDS, CRGB::Black);
  FastLED.show();
  lastColor = CRGB::Black;
  for (byte b = 0; b < BANKS; b++)
    for (byte l = 0; l < LEDS; l++)
      lastLedColor[b][l] = CRGB::Black;

  // Reset to factory default if BOOT key is pressed and hold for alt least 15 seconds at power on

  pinMode(FACTORY_DEFAULT_PIN, INPUT_PULLUP);
  unsigned long milliStart = millis();
  unsigned long duration = 0;
  lcdClear();
  byte newBootMode = PED_BOOT_UNKNOWN;
  if (digitalRead(FACTORY_DEFAULT_PIN) == LOW) {
    newBootMode = PED_BOOT_NORMAL;
    display_progress_bar_title2("Release button for", "Normal Boot");
  }
  while ((digitalRead(FACTORY_DEFAULT_PIN) == LOW) && (duration < 16000)) {
    if (duration > 1000 && duration < 3000 && newBootMode != PED_BOOT_BLE) {
      newBootMode = PED_BOOT_BLE;
      display_progress_bar_title2("Release button for", "Bluetooth Only");
    }
    else if (duration > 3000 && duration < 5000 && newBootMode != PED_BOOT_WIFI) {
      newBootMode = PED_BOOT_WIFI;
      display_progress_bar_title2("Release button for", "WiFi Only");
    }
    else if (duration > 5000 && duration < 7000 && newBootMode != PED_BOOT_AP) {
      newBootMode = PED_BOOT_AP;
      display_progress_bar_title2("Release button for", "Access Point");
    }
    else if (duration > 7000 && duration < 9000 && newBootMode != PED_BOOT_AP_NO_BLE) {
      newBootMode = PED_BOOT_AP_NO_BLE;
      display_progress_bar_title2("Release button for", "AP without BLE");
    }
    else if (duration > 9000 && duration < 11000 && newBootMode != PED_BOOT_RESET_WIFI) {
      newBootMode = PED_BOOT_RESET_WIFI;
      display_progress_bar_title2("Release button for", "WiFi Reset");
    }
    else if (duration > 11000 && duration < 13000 && newBootMode != PED_BOOT_LADDER_CONFIG) {
      newBootMode = PED_BOOT_LADDER_CONFIG;
      display_progress_bar_title2("Release button for", "Ladder Config");
    }
    else if (duration > 13000 && duration < 16000 && newBootMode != PED_FACTORY_DEFAULT) {
      newBootMode = PED_FACTORY_DEFAULT;
      display_progress_bar_title2("Hold button for", "Factory Default");
    }
    DPRINT("#");
    lcdSetCursor(duration / 500, 0);
    lcdPrint("#");
    display_progress_bar_update(duration, 16000);
    duration = millis() - milliStart;
  }

  //display_clear();

  DPRINT("\n");
  switch ((newBootMode == PED_BOOT_UNKNOWN) ? bootMode : newBootMode) {
    case PED_BOOT_NORMAL:     DPRINT("Boot NORMAL\n");  break;
    case PED_BOOT_BLE:        DPRINT("Boot BLE\n");     break;
    case PED_BOOT_WIFI:       DPRINT("Boot WIFI\n");    break;
    case PED_BOOT_AP:         DPRINT("Boot AP+BLE\n");  break;
    case PED_BOOT_AP_NO_BLE:  DPRINT("Boot AP\n");      break;
  }

  if (newBootMode != PED_BOOT_UNKNOWN && newBootMode != bootMode) {
    bootMode = newBootMode;
    switch (bootMode) {
      case PED_BOOT_NORMAL:
      case PED_BOOT_BLE:
      case PED_BOOT_WIFI:
      case PED_BOOT_AP:
      case PED_BOOT_AP_NO_BLE:
        eeprom_update_boot_mode(bootMode);
        break;
      default:
        break;
    }
  }
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
#ifdef BLE
      bleEnabled = true;
#else
      bleEnabled = false;
#endif
#ifdef WIFI
      wifiEnabled = true;
#else
      wifiEnabled = false;
#endif
      break;

    case PED_BOOT_RESET_WIFI:
      DPRINT("\nReset WiFi credentials\n");
      eeprom_update_sta_wifi_credentials("", "");
      eeprom_read_global();
      break;

    case PED_BOOT_LADDER_CONFIG:
      DPRINT("\nLadder Config\n");
      eeprom_read_profile();
      ladder_config();
      ESP.restart();
      break;

    case PED_FACTORY_DEFAULT:
      if (duration > 15000) {
        DPRINT("\nReset EEPROM to factory default\n");
        lcdSetCursor(0, 1);
        lcdPrint("Factory default ");
        eeprom_initialize();
        //eeprom_read_global();
        ESP.restart();
      }
      else
        bootMode = PED_BOOT_NORMAL;
      break;

  }

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

#ifdef BLUFI
  blufi_config();
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
  }
#endif

#ifdef LEFT_PIN
  pinMode(LEFT_PIN, INPUT_PULLUP);
  attachInterrupt(LEFT_PIN, onButtonLeft, CHANGE);
#endif
#ifdef CENTER_PIN
  pinMode(CENTER_PIN, INPUT_PULLUP);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureClick);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureDoubleClick);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureRepeatPress);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  bootButtonConfig.setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
  bootButtonConfig.setDebounceDelay(DEBOUNCE_INTERVAL);
  bootButtonConfig.setClickDelay(pressTime);
  bootButtonConfig.setDoubleClickDelay(doublePressTime);
  bootButtonConfig.setLongPressDelay(longPressTime);
  bootButtonConfig.setRepeatPressDelay(repeatPressTime);
  bootButtonConfig.setRepeatPressInterval(repeatPressTime);
  bootButton.init(&bootButtonConfig, CENTER_PIN);
  bootButton.setEventHandler(boot_button_event_handler);
#endif
#ifdef RIGHT_PIN
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  attachInterrupt(RIGHT_PIN, onButtonRight, CHANGE);
#endif

  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
}


void loop()
{
  switch (firmwareUpdate) {
    case PED_UPDATE_ARDUINO_OTA:
    case PED_UPDATE_HTTP:
      return;

    case PED_UPDATE_CLOUD:
      //set_clock();
      url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/data/css/bootstrap.min.css.gz");
      get_file_from_cloud(url, "/css/bootstrap.min.css.gz");
      url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/data/js/bootstrap.bundle.min.js.gz");
      get_file_from_cloud(url, "/js/bootstrap.bundle.min.js.gz");
      url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/data/schema.json");
      get_file_from_cloud(url, "/schema.json");

      latestFirmwareVersion = get_latest_firmware_version();
      ota_http_update();
      while (firmwareUpdate == PED_UPDATE_CLOUD) {
        otaStatus = HttpsOTA.status();
        switch (otaStatus) {
          case HTTPS_OTA_IDLE:
            //DPRINT("OTA upgrade have not started yet.\n");
            break;
          case HTTPS_OTA_UPDATING:
            //DPRINT("OTA upgrade is in progress.\n");
            break;
          case HTTPS_OTA_SUCCESS:
            DPRINT("OTA upgrade is successful.\n");
            DPRINT("Restart!\n");
            ESP.restart();
            break;
          case HTTPS_OTA_FAIL:
            DPRINT("OTA upgrade failed.\n");
            firmwareUpdate = PED_UPDATE_NONE;
            break;
          case HTTPS_OTA_ERR:
            DPRINT("Error occured while creating xEventGroup().\n");
            firmwareUpdate = PED_UPDATE_NONE;
            break;
        }
        yield();
      }
      return;
  }

  bootButton.check();

  screen_update();

  MTC.loop();

  // Check whether the input has changed since last time, if so, send the new value over MIDI
  controller_run();

  // Listen to incoming messages
  if (interfaces[PED_USBMIDI].midiIn && USB_MIDI.read())
    DPRINTMIDI("USB MIDI", USB_MIDI.getType(), USB_MIDI.getChannel(), USB_MIDI.getData1(), USB_MIDI.getData2());

  if (interfaces[PED_DINMIDI].midiIn && DIN_MIDI.read())
    DPRINTMIDI("DIN MIDI", DIN_MIDI.getType(), DIN_MIDI.getChannel(), DIN_MIDI.getData1(), DIN_MIDI.getData2());

#ifdef BLE
  if (bleEnabled) {
    if (interfaces[PED_BLEMIDI].midiIn && BLE_MIDI.read())
      DPRINTMIDI("BLE MIDI", BLE_MIDI.getType(), BLE_MIDI.getChannel(), BLE_MIDI.getData1(), BLE_MIDI.getData2());
  }
#endif

#ifdef WIFI
  if (wifiEnabled) {
    //if (WiFi.isConnected())
    {
      if (interfaces[PED_RTPMIDI].midiIn && RTP_MIDI.read())
        DPRINTMIDI("RTP MIDI", RTP_MIDI.getType(), RTP_MIDI.getChannel(), RTP_MIDI.getData1(), RTP_MIDI.getData2());

      if (interfaces[PED_IPMIDI].midiIn && IP_MIDI.read())
        DPRINTMIDI("IP  MIDI", IP_MIDI.getType(), IP_MIDI.getChannel(), IP_MIDI.getData1(), IP_MIDI.getData2());
    }

    http_run();

    // Run OTA update service
    ota_handle();
  }
#endif // WIFI

  wifi_and_battery_level();

  // Update display
  display_update();

  if (scrollingMode) {
    switch (currentProfile) {
      case 0:
        //blendwave();
        ease2();
        break;
      case 1:
        pacifica_loop();
        break;
      case 2:
        pride();
        break;
    }
    FastLED.show();
  }
}
