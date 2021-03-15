/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2020 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#ifdef NOWIFI
#define ota_begin(...)
#define ota_handle(...)
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
#ifdef TTGO_T_DISPLAY
    display_clear();
    display_progress_bar_title("OTA Update");
#else
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 10, "OTA Update");
    display.display();
#endif
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef TTGO_T_DISPLAY
    display_progress_bar_update(progress, total);
#else
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
#endif
  });

  ArduinoOTA.onEnd([]() {
#ifdef TTGO_T_DISPLAY
    display_clear();
    display_progress_bar_title("Restart");
#else
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
    display.display();
#endif
  });
}

inline void ota_handle() {
  ArduinoOTA.handle();
}

#endif  // NOWIFI
