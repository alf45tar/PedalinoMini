/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2024 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
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
#include <esp_wifi.h>
#include <esp_bt_main.h>
#include <driver/rtc_io.h>
#include <string>
#include "Pedalino.h"
#include "TickerTimer.h"
#include "LedsEffects.h"
#include "SerialMidiOut.h"
#include "USBMidiOut.h"
#include "UdpMidiOut.h"
#include "BLEMidiOut.h"
#include "SerialMidiIn.h"
#include "USBMidiIn.h"
#include "UdpMidiIn.h"
#include "BLEMidiIn.h"
#include "Config.h"
#if defined(ARDUINO_LILYGO_T_DISPLAY) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
#include "DisplayTFT.h"
#else
#include "DisplayOLED.h"
#endif
#include "Controller.h"
#include "OTAHTTPS.h"
#include "ImprovSerial.h"
#include "ImprovBLE.h"
#include "OTAUpdateArduino.h"
#include "WifiConnect.h"
#include "WebConfigAsync.h"


#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
#if CONFIG_IDF_TARGET_ESP32
#include <esp32/rom/rtc.h>
#elif CONFIG_IDF_TARGET_ESP32S2
#include <esp32s2/rom/rtc.h>
#elif CONFIG_IDF_TARGET_ESP32C3
#include <esp32c3/rom/rtc.h>
#elif CONFIG_IDF_TARGET_ESP32S3
#include <esp32s3/rom/rtc.h>
#else
#error Target CONFIG_IDF_TARGET is not supported
#endif
#else // ESP32 Before IDF 4.0
#include <rom/rtc.h>
#endif

TaskHandle_t loopCore0;   // loop0() on core 0
TaskHandle_t loopCore1;   // loop1() on core 1
void loop0(void * pvParameters);
void loop1(void * pvParameters);

void verbose_print_reset_reason(int reason)
{
  switch (reason)
  {
    case 1  : DPRINTLN("POWERON_RESET Vbat power on reset");                                break;
    case 3  : DPRINTLN("SW_RESET Software reset digital core");                             break;
    case 4  : DPRINTLN("OWDT_RESET Legacy watch dog reset digital core");                   break;
    case 5  : DPRINTLN("DEEPSLEEP_RESET Deep Sleep reset digital core");                    break;
    case 6  : DPRINTLN("SDIO_RESET Reset by SLC module, reset digital core");               break;
    case 7  : DPRINTLN("TG0WDT_SYS_RESET Timer Group0 Watch dog reset digital core");       break;
    case 8  : DPRINTLN("TG1WDT_SYS_RESET Timer Group1 Watch dog reset digital core");       break;
    case 9  : DPRINTLN("RTCWDT_SYS_RESET RTC Watch dog Reset digital core");                break;
    case 10 : DPRINTLN("INTRUSION_RESET Instrusion tested to reset CPU");                   break;
    case 11 : DPRINTLN("TGWDT_CPU_RESET Time Group reset CPU");                             break;
    case 12 : DPRINTLN("SW_CPU_RESET Software reset CPU");                                  break;
    case 13 : DPRINTLN("RTCWDT_CPU_RESET RTC Watch dog Reset CPU");                         break;
    case 14 : DPRINTLN("EXT_CPU_RESET for APP CPU, reseted by PRO CPU");                    break;
    case 15 : DPRINTLN("RTCWDT_BROWN_OUT_RESET Reset when the vdd voltage is not stable");  break;
    case 16 : DPRINTLN("RTCWDT_RTC_RESET RTC Watch dog reset digital core and rtc module"); break;
    default : DPRINTLN("NO_MEAN");
  }
}

void wifi_and_battery_level() {

  if (interruptCounter1 > 0) {

    interruptCounter1 = 0;

    freeMemory = ESP.getFreeHeap();
    maxAllocation = ESP.getMaxAllocHeap();

#ifdef WIFI
    if (wifiEnabled) wifiLevel = (3 * wifiLevel + WiFi.RSSI()) / 4;
#endif // WIFI

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
#endif // BATTERY_ADC_EN
    //analogReadResolution(12);
    uint16_t voltage = analogReadMilliVolts(BATTERY_PIN) * 2; // Read the ADC voltage using the built in factory calibration, no conversion needed.
                                                              // Battery voltage is measured using a voltage divider
    //analogReadResolution(ADC_RESOLUTION_BITS);
#ifdef BATTERY_ADC_EN
    digitalWrite(BATTERY_ADC_EN, LOW);
#endif // BATTERY_ADC_EN
    uint16_t difference = abs(voltage - batteryVoltage);
    //DPRINT("%d %d %d %d\n", voltage, batteryVoltage, difference, map2(constrain(batteryVoltage, 3000, 5000), 3000, 5000, 0, 100));
    if (difference > 100)
      batteryVoltage = (uint16_t)voltage;
    else
      batteryVoltage = (7 * batteryVoltage + voltage) / 8;
#endif // BATTERY

#ifdef DIAGNOSTIC
    static byte sec = 0;

    if (sec == 0) {
      memoryHistory[historyStart]        = map2(freeMemory, 0, 120*1024, 0, 100);
      fragmentationHistory[historyStart] = map2(maxAllocation, 0, 120*1024, 0, 100);
      wifiHistory[historyStart]          = map2(constrain(wifiLevel, -90, -10), -90, -10, 0, 100);
      batteryHistory[historyStart]       = map2(constrain(batteryVoltage, 3000, 5000), 3000, 5000, 0, 100);
      scanLoopHistory[historyStart]      = map2(constrain(scanLoop, 0, 10000), 0, 10000, 0, 100);
      serviceLoopHistory[historyStart]   = map2(constrain(serviceLoop, 0, 10000), 0, 10000, 0, 100);
      historyStart = (historyStart + 1) % POINTS;
    }
    sec = (sec + 1) % SECONDS_BETWEEN_SAMPLES;
#endif // DIAGNOSTIC
  }
}


void setup()
{
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

  DPRINT("%s - Version %d.%d.%d\n", MODEL, PEDALINO_VERSION_MAJOR, PEDALINO_VERSION_MINOR, PEDALINO_VERSION_PATCH);
  DPRINT("CPU0 reset reason: ");
  verbose_print_reset_reason(rtc_get_reset_reason(0));
  DPRINT("CPU1 reset reason: ");
  verbose_print_reset_reason(rtc_get_reset_reason(1));

  if (psramFound()) {
    if (psramInit()) {
      DPRINT("The PSRAM is correctly initialized\n");
    } else {
      DPRINT("PSRAM found but PSRAM init fail");
    }
  } else {
    DPRINT("PSRAM not found\n");
  }

  DPRINT("ChipRevision %d, CPU Freq %d MHz, SDK Version %s\n",ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
  DPRINT("Flash Size %d, Flash Speed %d Hz\n",ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  DPRINT("PSRAM Total Heap %d, PSRAM Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());

  adc_power_acquire();

  // Reset the pin used for power off
  // If the specified GPIO is a valid RTC GPIO, init as digital GPIO
  for (byte p = 0; p < PEDALS; p++) {
    if (rtc_gpio_is_valid_gpio((gpio_num_t)PIN_D(p))) rtc_gpio_deinit((gpio_num_t)PIN_D(p));
  }

#ifdef BATTERY_PIN
  //Check type of calibration value used to characterize ADC for BATTERY_PIN
  esp_adc_cal_characteristics_t adc_chars = {};
#if defined ARDUINO_LILYGO_T_DISPLAY
  // GPIO34
  esp_adc_cal_value_t           val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
#elif defined ARDUINO_BPI_LEAF_S3
  // GPIO14
  esp_adc_cal_value_t           val_type = esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
#elif defined ARDUINO_LILYGO_T_DISPLAY_S3
  // GPIO04
  esp_adc_cal_value_t           val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
#else
  // GPIO__
  esp_adc_cal_value_t           val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
#endif
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    vref = adc_chars.vref;
    DPRINT("eFuse Vref:%u mV\n", adc_chars.vref);
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    DPRINT("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  } else {
    DPRINT("Default Vref: 1100mV\n");
  }
#endif // BATTERY_PIN

  // Setup a 1Hz timer for wifi and battery level monitoring and logging
  Timer1Attach(1000);

  // Setup a 25Hz timer for display update
  Timer3Attach(1000 / 25);

  esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
  if (pi != NULL) {
    const esp_partition_t* nvs = esp_partition_get(pi);
    DPRINT("%s Total %d\n", nvs->label, nvs->size);
    esp_partition_iterator_release(pi);
  } else {
    DPRINT("'nvs' partition not found\n");
  }

  DPRINT("PlatformIO Built Environment: %s\n", xstr(PLATFORMIO_ENV));

  DPRINTLN("_________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___ ");
  DPRINTLN("\\______  \\ ____   __| _/____  |  | |__| ____   ____   /     \\ |__| ____ |__|   /  / \\__    ___/     \\   \\  \\ ");
  DPRINTLN("|     ___// __ \\ / __ |\\__  \\ |  | |  |/    \\ /  _ \\ /  \\ /  \\|  |/    \\|  |  /  /    |    | /  \\ /  \\   \\  \\ ");
  DPRINTLN("|    |   \\  ___// /_/ | / __ \\|  |_|  |   |  (  <_> )    Y    \\  |   |  \\  | (  (     |    |/    Y    \\   )  )");
  DPRINTLN("|____|    \\___  >____ |(____  /____/__|___|  /\\____/\\____|__  /__|___|  /__|  \\  \\    |____|\\____|__  /  /  / ");
  DPRINTLN("              \\/     \\/     \\/             \\/               \\/        \\/       \\__\\                 \\/  /__/ ");
  DPRINTLN("                                                                                  (c) 2018-2024 alf45star     ");
  DPRINTLN("                                                                      https://github.com/alf45tar/PedalinoMini");
  DPRINT("\nHostname: %s\n", host.c_str());
  DPRINT("PSRAM%sfound\n", psramFound() ? " " : " not ");
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

  display_boot();

  sketchSize = ESP.getSketchSize();
  sketchMD5  = ESP.getSketchMD5();

  DPRINT("Sketch Size %d bytes, Firmware Hash %s\n", sketchSize, sketchMD5.c_str());

  FastLED.addLeds<WS2812B, FASTLEDS_DATA_PIN, LED_RGB_ORDER>(fastleds, LEDS);
  fill_solid(fastleds, LEDS, CRGB::Black);
  FastLED.show();
  lastColor0 = CRGB::Black;
  lastColor1 = CRGB::Black;
  for (byte b = 0; b < BANKS; b++)
    for (byte l = 0; l < LEDS; l++)
      lastLedColor[b][l] = CRGB::Black;

  // Reset to factory default if BOOT key is pressed and hold for alt least 15 seconds at power on

  pinMode(FACTORY_DEFAULT_PIN, INPUT_PULLUP);
  unsigned long milliStart = millis();
  unsigned long duration = 0;
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
    display_progress_bar_update(duration, 16000);
    duration = millis() - milliStart;
  }

  //display_clear();

  if (newBootMode != PED_BOOT_UNKNOWN && newBootMode != bootMode) {
    switch (newBootMode) {
      case PED_BOOT_NORMAL:
      case PED_BOOT_BLE:
      case PED_BOOT_WIFI:
      case PED_BOOT_AP:
      case PED_BOOT_AP_NO_BLE:
        bootMode = newBootMode;
        eeprom_update_boot_mode(bootMode);
        break;
    }
  }
  newBootMode = (newBootMode == PED_BOOT_UNKNOWN) ? bootMode : newBootMode;
  DPRINT("\n");
  switch (newBootMode) {
    case PED_BOOT_NORMAL:     DPRINT("Boot NORMAL\n");  break;
    case PED_BOOT_BLE:        DPRINT("Boot BLE\n");     break;
    case PED_BOOT_WIFI:       DPRINT("Boot WIFI\n");    break;
    case PED_BOOT_AP:         DPRINT("Boot AP+BLE\n");  break;
    case PED_BOOT_AP_NO_BLE:  DPRINT("Boot AP\n");      break;
  }
  switch (newBootMode) {
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
        eeprom_initialize();
        ESP.restart();
      }
      else
        bootMode = PED_BOOT_NORMAL;
      break;

  }

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

  reloadProfile = true;
  controller_run();

  // Initiate USB Device MIDI communications, listen to all channels and turn Thru on/off
  usb_midi_connect();                 // On receiving MIDI data callbacks setup
  DPRINT("USB MIDI started\n");

  // Initiate serial MIDI communications, listen to all channels and turn Thru on/off
  serial_midi_connect();              // On receiving MIDI data callbacks setup
  DPRINT("DIN MIDI started\n");

#ifdef BLUFI
  blufi_config();
#endif

#ifdef WIFI
  if (wifiEnabled) {
    WiFi.persistent(false);
#if defined(ARDUINO_BPI_LEAF_S3) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
    WiFi.useStaticBuffers(true);
#endif
    WiFi.onEvent(WiFiEvent);
    if (bootMode == PED_BOOT_AP || bootMode == PED_BOOT_AP_NO_BLE) {
      DPRINT("Skipped connection to last AP and/or SmartConfig/WPS setup\n");
      ap_mode_start();
    }
    else
      wifi_connect();

    improv_serial::global_improv_serial.setup(String("PedalinoMini (TM)"),
                                              String(VERSION),
                                              String(xstr(PLATFORMIO_ENV)),
                                              String("Device name: ") + host);

    networks = WiFi.scanNetworks(true); // Async scan
  }
#endif

#ifdef BLE
  if (bleEnabled) {
    // BLE MIDI service advertising
    ble_midi_start_service();
#ifdef BLECLIENT
    DPRINT("BLE MIDI service connection started (client mode)\n");
#else
    DPRINT("BLE MIDI service advertising started (server mode)\n");
#endif
  }
#endif

  set_initial_led_color();

  xTaskCreatePinnedToCore(
                    loop1,       /* Task function. */
                    "loopTask1", /* name of task. */
                    8192,        /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &loopCore1,  /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */

  xTaskCreatePinnedToCore(
                    loop0,       /* Task function. */
                    "loopTask0", /* name of task. */
                    8192,        /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &loopCore0,  /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */

  DPRINT("Internal Total Heap %d, Internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
}


void loop()
{
  // High priority activities run on Core 1

  unsigned long startClock = micros();

  // Check whether the input has changed since last time, if so, send the new value over MIDI
  controller_run();

  // Send MTC or MIDI CLock
  MTC.loop();

  // It is not a priority task but it only works on the same core where FastLED init run
  // https://github.com/davidlmorris/FastLED_Hang_Fix_Demo
  // It has no effect when not in diagnostic screen

    if (scrollingMode && !displayOff) {
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

  scanLoop = micros() - startClock;
}


void loop1(void * pvParameters)
{
  // High priority activities run on Core 1

  unsigned long startClock = micros();
  unsigned long endClock;

  for(;;) {

    // Feed the watchdog of FreeRTOS
    vTaskDelay(1);

    // Listen to incoming messages
    if (interfaces[PED_USBMIDI].midiIn && USB_MIDI.read())
      DPRINTMIDI("USB MIDI", USB_MIDI.getType(), USB_MIDI.getChannel(), USB_MIDI.getData1(), USB_MIDI.getData2());

    if (interfaces[PED_DINMIDI].midiIn && DIN_MIDI.read())
      DPRINTMIDI("DIN MIDI", DIN_MIDI.getType(), DIN_MIDI.getChannel(), DIN_MIDI.getData1(), DIN_MIDI.getData2());

#if defined(BLE) && !defined(BLECLIENT)
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
    }
#endif // WIFI

    endClock = micros();
    serviceLoop = endClock - startClock;
    startClock = endClock;
  }
}

void loop0(void * pvParameters)
{
  // Low priority activities run on Core 0 within WiFi and Bluetooth

  for(;;) {

    // Feed the watchdog of FreeRTOS
    vTaskDelay(10);

#ifdef WIFI
    if (wifiEnabled) {

      http_run();

      // Run OTA update service
      ota_handle();

      // WiFi Provisioning
      if (improv_serial::global_improv_serial.loop()) {
        wifiSSID     = improv_serial::global_improv_serial.get_ssid();
        wifiPassword = improv_serial::global_improv_serial.get_password();

        DPRINT("SSID        : %s\n", wifiSSID.c_str());
        DPRINT("Password    : %s\n", wifiPassword.c_str());

        eeprom_update_sta_wifi_credentials(wifiSSID, wifiPassword);
      }
    }
#endif // WIFI

    wifi_and_battery_level();

    // Update display
    display_update();

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
          delay(1);
        }
        return;
    }
  }
}