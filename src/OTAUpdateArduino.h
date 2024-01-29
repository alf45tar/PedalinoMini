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

#if !defined(ARDUINOOTA) || defined(NOWIFI)
#define ota_begin(...)
#define ota_handle(...)
#define ota_end(...)
#else

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

void ota_begin(const char *hostname) {

  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.begin();

  ArduinoOTA.onStart([]() {
      firmwareUpdate = PED_UPDATE_ARDUINO_OTA;
#ifdef WEBSOCKET
    webSocket.enable(false);
    webSocket.closeAll();
#endif
#if defined(ARDUINO_LILYGO_T_DISPLAY) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
    display_clear();
    display_progress_bar_title("OTA Update");
#else
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 10, "OTA Update");
    display.display();
#endif
    fill_solid(fastleds, LEDS, CRGB::Black);
    FastLED.show();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#if defined(ARDUINO_LILYGO_T_DISPLAY) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
    display_progress_bar_update(progress, total);
#else
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
#endif
    byte ledProgress = (progress / (total / (LEDS * 100))) / 100;
    fill_solid(fastleds, ledProgress + 1, swap_rgb_order(CRGB::Red, rgbOrder));
    byte ledDim = (progress / (total / (LEDS * 100))) % 100;
    fastleds[ledProgress].nscale8(ledDim);
    FastLED.show();
  });

  ArduinoOTA.onEnd([]() {
#if defined(ARDUINO_LILYGO_T_DISPLAY) || defined(ARDUINO_LILYGO_T_DISPLAY_S3)
    display_clear();
    display_progress_bar_title("Restart");
#else
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
    display.display();
#endif
    fill_solid(fastleds, LEDS, swap_rgb_order(CRGB::Green, rgbOrder));
    FastLED.show();
  });

  DPRINT("OTA update started\n");
}

inline void ota_handle() {
  ArduinoOTA.handle();
}

inline void ota_end() {
  ArduinoOTA.End();
}

#endif  // NOWIFI