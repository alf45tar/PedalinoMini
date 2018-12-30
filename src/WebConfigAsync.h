/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

String  theme     = "bootstrap";

#ifdef NOWIFI 
inline void http_run() {};
#else

#include <StreamString.h>
#include <FS.h>

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer        httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>

AsyncWebServer          httpServer(80);
AsyncWebSocket          webSocket("/ws");
AsyncEventSource        events("/events");    // EventSource is single direction, text-only protocol.

extern const uint8_t bootstrap_min_css_start[]        asm("_binary_data_bootstrap_min_css_gz_start");
extern const uint8_t bootstrap_min_css_end[]          asm("_binary_data_bootstrap_min_css_gz_end");
extern const uint8_t bootstrap_min_js_start[]         asm("_binary_data_bootstrap_min_js_gz_start");
extern const uint8_t bootstrap_min_js_end[]           asm("_binary_data_bootstrap_min_js_gz_end");
extern const uint8_t jquery_3_3_1_slim_min_js_start[] asm("_binary_data_jquery_3_3_1_slim_min_js_gz_start");
extern const uint8_t jquery_3_3_1_slim_min_js_end[]   asm("_binary_data_jquery_3_3_1_slim_min_js_gz_end");
extern const uint8_t popper_min_js_start[]            asm("_binary_data_popper_min_js_gz_start");
extern const uint8_t popper_min_js_end[]              asm("_binary_data_popper_min_js_gz_end");

#define FAVICON_ICO_GZ_LEN  621
const uint8_t favicon_ico_gz[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x08, 0x54, 0x41, 0x29, 0x5c, 0x02, 0x00, 0x66, 0x61, 0x76, 0x69, 0x63, 0x6f, 
  0x6e, 0x2e, 0x69, 0x63, 0x6f, 0x00, 0xad, 0x53, 0x4d, 0x6f, 0x12, 0x61, 0x10, 0x7e, 0xf9, 0xd8, 
  0x5d, 0x92, 0x26, 0xb4, 0xc2, 0x62, 0xb1, 0x58, 0x41, 0xd6, 0x76, 0xbf, 0x17, 0x58, 0x58, 0xc2, 
  0x52, 0x16, 0x30, 0x42, 0x23, 0x85, 0xd4, 0x10, 0xd3, 0xa6, 0xd2, 0x83, 0x50, 0x35, 0xa0, 0x31, 
  0x1e, 0x4b, 0x82, 0x72, 0xd1, 0x93, 0x89, 0x47, 0x7b, 0x31, 0x26, 0x5e, 0xd5, 0x83, 0xf1, 0xe6, 
  0xc1, 0x98, 0x78, 0x31, 0x6a, 0xa2, 0x69, 0x1a, 0x13, 0x8d, 0x17, 0x7f, 0x83, 0xff, 0x40, 0x1f, 
  0x16, 0xbc, 0x08, 0x45, 0x4d, 0x9c, 0xcd, 0x93, 0xd9, 0xbc, 0x33, 0xcf, 0xcc, 0xbc, 0xcf, 0xec, 
  0x12, 0xe2, 0xc0, 0x33, 0x37, 0x37, 0xf0, 0x11, 0xd2, 0x71, 0x13, 0x72, 0x94, 0x10, 0x22, 0x00, 
  0x38, 0xc2, 0xc9, 0xf0, 0xdc, 0x36, 0xc4, 0x38, 0xcd, 0xc6, 0x44, 0xa3, 0x28, 0xca, 0xf6, 0x6c, 
  0x20, 0x40, 0x05, 0x02, 0x01, 0xda, 0xe5, 0x72, 0x11, 0x86, 0x61, 0xc8, 0x34, 0x73, 0xbb, 0xdd, 
  0xa4, 0xbc, 0xba, 0x4a, 0x54, 0x55, 0x9d, 0x5d, 0xab, 0x56, 0xf3, 0xaa, 0xa6, 0xb1, 0x09, 0x5d, 
  0xdf, 0xd2, 0x93, 0xc9, 0x4b, 0x08, 0x3b, 0x7d, 0x3e, 0xdf, 0x54, 0xbe, 0xc7, 0xe3, 0x21, 0x17, 
  0x1a, 0x0d, 0x72, 0xa6, 0x54, 0xca, 0xf5, 0xfb, 0xfd, 0x6f, 0xd5, 0x5a, 0x6d, 0x1d, 0xfc, 0x3b, 
  0xc9, 0x54, 0x6a, 0x6f, 0x50, 0x1e, 0x73, 0xfc, 0x91, 0x9f, 0xb3, 0x2c, 0x82, 0xbe, 0xb1, 0x7c, 
  0xa1, 0x70, 0xa0, 0xc5, 0x62, 0x45, 0xf4, 0xfe, 0x6b, 0x3e, 0x4d, 0xd3, 0xc4, 0x30, 0x0c, 0x22, 
  0x49, 0x92, 0x6c, 0xa4, 0xd3, 0x1f, 0x44, 0x49, 0x5a, 0x19, 0xf1, 0xef, 0x23, 0xec, 0xf2, 0xfb, 
  0xfd, 0xff, 0xc2, 0xff, 0x28, 0x0d, 0xf9, 0xb7, 0xc1, 0x7f, 0x2c, 0x88, 0xa2, 0x2e, 0x2b, 0x8a, 
  0x02, 0x1f, 0x3d, 0x11, 0x0e, 0x7b, 0x07, 0xab, 0x40, 0xfe, 0x74, 0xbe, 0x2c, 0x67, 0xc1, 0xbf, 
  0x81, 0xf7, 0x2f, 0xc0, 0xbb, 0x11, 0xde, 0xa6, 0x0c, 0xe3, 0x79, 0x3c, 0x91, 0xd8, 0x08, 0x06, 
  0x83, 0x1e, 0xd4, 0x3b, 0x94, 0xaf, 0xa8, 0x6a, 0x36, 0x12, 0x89, 0xf8, 0x78, 0x9e, 0x57, 0x80, 
  0xb8, 0x20, 0x08, 0x3a, 0x6a, 0x9e, 0x86, 0x3e, 0xb7, 0x10, 0xdf, 0x87, 0xb6, 0x2d, 0x68, 0x46, 
  0x45, 0xa3, 0x51, 0xe2, 0x70, 0x38, 0xc6, 0xf9, 0x8a, 0x62, 0xa2, 0x86, 0x82, 0x19, 0x6e, 0x02, 
  0x7d, 0x5d, 0xd7, 0x77, 0x33, 0x99, 0x4c, 0xb9, 0xdd, 0x6e, 0xf7, 0x2b, 0x95, 0xca, 0xc3, 0xdd, 
  0x6e, 0xf7, 0xab, 0x69, 0x9a, 0x62, 0xa9, 0x5c, 0x26, 0xf8, 0x3e, 0xc6, 0xf8, 0xb2, 0x2c, 0x67, 
  0x30, 0x67, 0x13, 0xdc, 0xa7, 0xb1, 0x78, 0xfc, 0x22, 0xde, 0xb7, 0xd2, 0xe9, 0xf4, 0x5a, 0x63, 
  0x7b, 0xfb, 0x55, 0xb1, 0x58, 0x3c, 0xdf, 0x6c, 0xb5, 0xae, 0xe0, 0x9c, 0xb5, 0xf2, 0x79, 0xe2, 
  0x74, 0x3a, 0xc7, 0xf8, 0xd0, 0xdf, 0x44, 0xbf, 0xab, 0x19, 0xd3, 0xbc, 0x1b, 0x0e, 0x87, 0x23, 
  0x3c, 0x0c, 0xf9, 0x1d, 0xcc, 0xfd, 0xe0, 0xf8, 0xe2, 0x62, 0x08, 0x77, 0x3b, 0xe9, 0x67, 0x59, 
  0xea, 0xd8, 0xc2, 0xc2, 0xc4, 0xfb, 0x9f, 0x5a, 0x5a, 0xca, 0xb6, 0x3b, 0x9d, 0x6e, 0x6b, 0x67, 
  0xe7, 0x19, 0x7a, 0xef, 0x61, 0x0f, 0x8f, 0x80, 0x7b, 0xa2, 0x28, 0xa6, 0x50, 0xa7, 0x09, 0x1d, 
  0x9f, 0x20, 0xe7, 0x08, 0x34, 0x99, 0x74, 0xff, 0x7d, 0xec, 0x6b, 0x1d, 0x79, 0xc2, 0x4a, 0x2e, 
  0x77, 0x4e, 0xd3, 0xb4, 0xf2, 0x32, 0xcf, 0x67, 0xa1, 0x61, 0x16, 0xb5, 0xae, 0x0d, 0xea, 0x63, 
  0x8e, 0x0e, 0xda, 0xba, 0xe6, 0xe7, 0xe7, 0x6d, 0x3e, 0xfe, 0x99, 0x5f, 0xfc, 0x65, 0xc4, 0x3f, 
  0x01, 0x07, 0xf8, 0x17, 0xae, 0xf7, 0x7a, 0xbd, 0x17, 0x1b, 0x9b, 0x9b, 0x9f, 0xd1, 0xfb, 0xcd, 
  0x68, 0x87, 0x2f, 0xc1, 0xbd, 0x1c, 0x0a, 0x85, 0x66, 0x2c, 0xcb, 0xb2, 0xb9, 0x30, 0xdb, 0xe3, 
  0x9b, 0x25, 0x51, 0x8e, 0x9b, 0x81, 0x2f, 0xa0, 0x77, 0x0d, 0xfb, 0x95, 0xea, 0xf5, 0xfa, 0x59, 
  0xe8, 0x55, 0xc3, 0x3c, 0x26, 0x34, 0x55, 0x39, 0x8e, 0x63, 0x19, 0x9a, 0x76, 0x7a, 0xbd, 0x5e, 
  0xf2, 0x3f, 0xed, 0xfb, 0x0f, 0x42, 0x5e, 0xff, 0x8e, 0x59, 0x80, 0x01, 0x5c, 0x87, 0x80, 0x19, 
  0xe6, 0xbc, 0x47, 0xee, 0x4f, 0xac, 0x9d, 0x13, 0xe3, 0x7e, 0x04, 0x00, 0x00
};
#endif


#ifdef WEBCONFIG

String  blynk_get_token();
String  blynk_set_token(String);
bool    blynk_cloud_connected();
void    blynk_connect();
void    blynk_disconnect();
void    blynk_refresh();

String  alert     = "";
String  uiprofile = "1";
String  uibank    = "1";


String get_top_page(byte p = 0) {

  String page = "";

  page += F("<!doctype html>");
  page += F("<html lang='en'>");
  page += F("<head>");
  page += F("<title>Pedalino&trade;</title>");
  page += F("<meta charset='utf-8'>");
  page += F(" <meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
  if ( theme == "bootstrap" ) {
  #ifdef BOOTSTRAP_LOCAL
    page += F("<link rel='stylesheet' href='/css/bootstrap.min.css' integrity='sha256-eSi1q2PG6J7g7ib17yAaWMcrr5GrtohYChqibrV7PBE=' crossorigin='anonymous'>");
  #else
    page += F("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css' integrity='sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO' crossorigin='anonymous'>");
  #endif
  } else {
    page += F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootswatch/4.1.3/");
    page += theme;
    page += F("/bootstrap.min.css' crossorigin='anonymous'>");
  }
  page += F("</head>");

  page += F("<body>");
  page += F("<p></p>");
  page += F("<div class='container-fluid'>");

  page += F("<nav class='navbar navbar-expand navbar-light bg-light'>");
  page += F("<a class='navbar-brand' href='/'>Pedalino&trade;</a>");
  page += F("<button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarNavDropdown' aria-controls='navbarNavDropdown' aria-expanded='false' aria-label='Toggle navigation'>");
  page += F("<span class='navbar-toggler-icon'></span>");
  page += F("</button>");
  page += F("<div class='collapse navbar-collapse' id='navbarNavDropdown'>");
  page += F("<ul class='navbar-nav mr-auto'>");
  page += F("<li class='nav-item");
  page += (p == 1 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/live'>Live</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 2 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/banks'>Banks</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 3 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/pedals'>Pedals</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 4 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/interfaces'>Interfaces</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 5 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/options'>Options</a>");
  page += F("</li>");
  page += F("</ul>");
  if (p > 1) {
    page += F("<form class='form-inline my-2 my-lg-0'>");
    //page += F("<button class='btn btn-primary my-2 my-sm-0' type='button'>Save</button>");
    page += F("<div class='btn-group my-2 my-sm-0'>");
    page += F("<button type='button' class='btn btn-info'>Profile ");
    uiprofile = String(currentProfile + 1);
    page += String((char)('A' + uiprofile.toInt() - 1));
    page += F("</button>");
    page += F("<button type='button' class='btn btn-info dropdown-toggle dropdown-toggle-split' data-toggle='dropdown' aria-haspopup='true' aria-expanded='false'>");
    page += F("<span class='sr-only'>Toggle Dropdown</span>");
    page += F("</button>");
    page += F("<div class='dropdown-menu' aria-labelledby='navbarDropdownMenuLink'>");
    page += F("<a class='dropdown-item' href='?profile=1'>A</a>");
    page += F("<a class='dropdown-item' href='?profile=2'>B</a>");
    page += F("<a class='dropdown-item' href='?profile=3'>C</a>");
    page += F("</div>");
    page += F("</div>");
    page += F("</form>");
  }
  page += F("</div>");
  page += F("</nav>");

  if (alert != "") {
    page += F("<p></p>");
    page += F("<div class='alert alert-success alert-dismissible fade show' role='alert'>");
    page += alert;
    page += F("<button type='button' class='close' data-dismiss='alert' aria-label='Close'>");
    page += F("<span aria-hidden='true'>&times;</span>");
    page += F("</button>");
    page += F("</div>");
    alert = "";
  }

  page += F("<p></p>");

  return page;
}

String get_footer_page() {

  String page = "";
  //page += F("<nav class='navbar align-items-end navbar-light bg-light'>");
  //page += F("<a class='navbar-text' href='https://github.com/alf45tar/PedalinoMini'>https://github.com/alf45tar/PedalinoMini</a>");
  //page += F("</nav>");

  page += F("</div>");
#ifdef BOOTSTRAP_LOCAL
  page += F("<script src='/js/jquery-3.3.1.slim.min.js' integrity='sha256-o3xvfVoAnalAlD3CPebt5QWZ3yLdooNGruu0ZJvZy0U=' crossorigin='anonymous'></script>");
  page += F("<script src='/js/popper.min.js' integrity='sha256-0rnynqH0KmCovrHAT3aGgofypI1uxQ+znWuIhYSgPEk=' crossorigin='anonymous'></script>");
  page += F("<script src='/js/bootstrap.min.js' integrity='sha256-VsEqElsCHSGmnmHXGQzvoWjWwoznFSZc6hs7ARLRacQ=' crossorigin='anonymous'></script>");
#else
  page += F("<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js' integrity='sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo' crossorigin='anonymous'></script>");
  page += F("<script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js' integrity='sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49' crossorigin='anonymous'></script>");
  page += F("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js' integrity='sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy' crossorigin='anonymous'></script>");
#endif
  page += F("</body>");
  page += F("</html>");

  return page;
}

String get_root_page() {

  String page = "";

  page += get_top_page();

  page += F("<h4 class='display-4'>Smart wireless MIDI foot controller</h4>");
  page += F("<p></p>");

  page += F("<div class='row'>");

  page += F("<div class='col-3'>");
  page += F("<h3>Product</h3>");
  page += F("<dt>Model</dt><dd>");
  page += String(MODEL);
  page += F("</dd>");
  page += F("<dt>Profiles</dt><dd>");
  page += String(PROFILES);
  page += F("</dd>");
  page += F("<dt>Banks</dt><dd>");
  page += String(BANKS);
  page += F("</dd>");
  page += F("<dt>Pedals</dt><dd>");
  page += String(PEDALS);
  page += F("</dd>");
  page += F("</div>");

  page += F("<div class='col-3'>");
  page += F("<h3>Hardware</h3>");
  page += F("<dt>Chip</dt><dd>");
#ifdef ARDUINO_ARCH_ESP8266
  page += String("ESP8266");
#endif
#ifdef ARDUINO_ARCH_ESP32
  page += String("ESP32");
#endif
  page += F("</dd>");
#ifdef ARDUINO_ARCH_ESP32
  page += F("<dt>Chip Revision</dt><dd>");
  page += ESP.getChipRevision();
  page += F("</dd>");
#endif
  page += F("</dd>");
  page += F("<dt>Chip ID</dt><dd>");
  page += getChipId();
  page += F("</dd>");
  page += F("<dt>Chip Speed</dt><dd>");
  page += ESP.getFlashChipSpeed() / 1000000;
  page += F(" MHz</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize() / (1024 * 1024);
  page += F(" MB</dd>");
#ifdef ARDUINO_ARCH_ESP8266
  page += F("<dt>Real Flash Size</dt><dd>");
  page += ESP.getFlashChipRealSize() / (1024 * 1024);
  page += F(" MB</dd>");
#endif
  page += F("<dt>Free Heap Size</dt><dd>");
  page += ESP.getFreeHeap() / 1024;
  page += F(" kB</dd>");
  page += F("<dt>SDK Version</dt><dd>");
  page += ESP.getSdkVersion();
  page += F("</dd>");
  //page += F("<dt>Running On Core</dt><dd>");
  //page += xPortGetCoreID();
  //page += F("</dd>");
  page += F("</div>");

  page += F("<div class='col-3'>");
  if (WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA) {
  page += F("<h3>Wireless STA</h3>");
  page += F("<dt>SSID</dt><dd>");
  page += wifiSSID;
  page += F("</dd>");
  page += F("<dt>BSSID</dt><dd>");
  page += WiFi.BSSIDstr();
  page += F("</dd>");
  page += F("<dt>RSSI</dt><dd>");
  page += String(WiFi.RSSI());
  page += F(" dBm</dd>");
  page += F("<dt>Channel</dt><dd>");
  page += String(WiFi.channel());
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  }

  if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
  page += F("<h3>Wireless AP</h3>");
  page += F("<dt>AP SSID</dt><dd>");
  page += wifiSoftAP;
  page += F("</dd>");
  page += F("<dt>AP MAC Address</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>AP IP Address</dt><dd>");
  page += WiFi.softAPIP().toString();
  page += F("</dd>");
  page += F("<dt>Channel</dt><dd>");
  page += String(WiFi.channel());
  page += F("</dd>");
#ifdef ARDUINO_ARCH_ESP32  
  page += F("<dt>Hostname</dt><dd>");
  page += WiFi.softAPgetHostname();
  page += F("</dd>");
#endif
  page += F("<dt>Connected Stations</dt><dd>");
  page += WiFi.softAPgetStationNum();
  page += F("</dd>");
  }
  page += F("</div>");

  page += F("<div class='col-3'>");
  page += F("<h3>Network</h3>");
  page += F("<dt>Hostname</dt><dd>");
#ifdef ARDUINO_ARCH_ESP8266
  page += WiFi.hostname() + String(".local");
#endif
#ifdef ARDUINO_ARCH_ESP32
  page += WiFi.getHostname() + String(".local");
#endif
  page += F("</dd>");
  page += F("<dt>IP address</dt><dd>");
  page += WiFi.localIP().toString();
  page += F("</dd>");
  page += F("<dt>Subnet mask</dt><dd>");
  page += WiFi.subnetMask().toString();
  page += F("</dd>");
  page += F("<dt>Gataway IP</dt><dd>");
  page += WiFi.gatewayIP().toString();
  page += F("</dd>");
  page += F("<dt>DNS 1</dt><dd>");
  page += WiFi.dnsIP(0).toString();
  page += F("</dd>");
  page += F("<dt>DNS 2</dt><dd>");
  page += WiFi.dnsIP(1).toString();
  page += F("</dd>");
  page += F("<dt>Blynk Cloud</dt><dd>");
  if (blynk_cloud_connected()) page += String("Online");
  else page += String("Offline");
  page += F("</dd>");
  page += F("<dt>MIDI Network</dt><dd>");
  if (appleMidiConnected) page += String("Connected");
  else page += String("Disconnected");
  page += F("</dd>");
#ifdef BLE
  page += F("<dt>Bluetooth LE MIDI</dt><dd>");
  if (bleMidiConnected) page += String("Connected");
  else page += String("Disconnected");
  page += F("</dd>");
#endif
  page += F("</div>");

  page += F("</div>");
  page += get_footer_page();

  return page;
}

String get_live_page() {

  String page = "";

  page += get_top_page(1);

  page += F("<canvas id='screen' height='128' width='256'>");
  page += F("Sorry, your browser does not support canvas.");
  page += F("</canvas>");
  page += F("<div id='myDiv'>");
  page += F("</div>");

  page += F("<script>");
  page += F("var con = new WebSocket('ws://' + location.hostname + ':80/ws');");
  page += F("con.binaryType = 'arraybuffer';");
  page += F("con.onopen = function () {");
  page += F("console.log('WebSocket connection open');");  
  page += F("};");
  page += F("con.onerror = function (error) {");
  page += F("console.log('WebSocket Error ', error);");
  page += F("};");
  page += F("con.onmessage = function (e) {");
  page += F("var data = e.data;");
  page += F("var dv = new DataView(data);");
  page += F("var canvas=document.getElementById('screen');");
  page += F("var context=canvas.getContext('2d');");
  page += F("var x=0; y=0;");
  page += F("for (y=0; y<64; y++)");
  page += F("  for (x=0; x<128; x++)");
  page += F("    ((dv.getUint8(x+Math.floor(y/8)*128) & (1<<(y&7))) == 0) ? context.clearRect(x*2,y*2,2,2) : context.fillRect(x*2,y*2,2,2);");
  page += F("};");
  page += F("con.onclose = function () {");
  page += F("console.log('WebSocket connection closed');");
  page += F("};");

  page += F("if (!!window.EventSource) {");
  page += F("var source = new EventSource('/events');");
  page += F("source.addEventListener('open', function(e) {");
  page += F("console.log('Events Connected');");
  page += F("}, false);");
  page += F("source.addEventListener('error', function(e) {");
  page += F("if (e.target.readyState != EventSource.OPEN) {");
  page += F("console.log('Events Disconnected');");
  page += F("}");
  page += F("}, false);");
  page += F("source.addEventListener('message', function(e) {");
  page += F("console.log('Event: ', e.data);");
  page += F("}, false);");
  page += F("source.addEventListener('mtc', function(e) {");
  page += F("var myDiv = document.getElementById('myDiv');");
	page += F("myDiv.innerHTML = e.data;");
  page += F("}, false);");
  page += F("source.addEventListener('screen', function(e) {");
  page += F("}, false);");
  page += F("}");
  page += F("</script>");

  page += get_footer_page();

  return page;
}

String get_banks_page() {

  String page = "";
  const byte b = constrain(uibank.toInt(), 0, BANKS);

  page += get_top_page(2);

  page += F("<div class='btn-group'>");
  for (unsigned int i = 1; i <= BANKS; i++) {
    page += F("<form method='get'><button type='button submit' class='btn");
    page += (uibank == String(i) ? String(" btn-primary") : String(""));
    page += F("' name='bank' value='");
    page += String(i) + F("'>") + String(i) + F("</button></form>");
  }
  page += F("</div>");
  
  page += F("<p></p>");

  page += F("<form method='post'>");
  page += F("<div class='form-row'>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Pedal</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>MIDI Message</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>MIDI Channel</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>MIDI Code/Note</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>MIDI Value 1</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>MIDI Value 2</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>MIDI Value 3</span>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  for (unsigned int i = 1; i <= PEDALS; i++) {
    page += F("<div class='col-1 mb-3 text-center'>");
    page += String(i);
    page += F("</div>");

    page += F("<div class='col-2'>");
    page += F("<select class='custom-select custom-select-sm' name='message");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_PROGRAM_CHANGE) page += F(" selected");
    page += F(">Program Change</option>");
    page += F("<option value='");
    page += String(PED_CONTROL_CHANGE) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_CONTROL_CHANGE) page += F(" selected");
    page += F(">Control Change</option>");
    page += F("<option value='");
    page += String(PED_NOTE_ON_OFF) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_NOTE_ON_OFF) page += F(" selected");
    page += F(">Note On/Off</option>");
    page += F("<option value='");
    page += String(PED_PITCH_BEND) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_PITCH_BEND) page += F(" selected");
    page += F(">Pitch Bend</option>");
    page += F("<option value='");
    page += String(PED_BANK_SELECT_INC) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_BANK_SELECT_INC) page += F(" selected");
    page += F(">Bank Select+</option>");
    page += F("<option value='");
    page += String(PED_BANK_SELECT_DEC) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_BANK_SELECT_DEC) page += F(" selected");
    page += F(">Bank Select-</option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE_INC) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_PROGRAM_CHANGE_INC) page += F(" selected");
    page += F(">Program Change+</option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE_DEC) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_PROGRAM_CHANGE_DEC) page += F(" selected");
    page += F(">Program Change-</option>");
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-1'>");
    page += F("<select class='custom-select custom-select-sm' name='channel");
    page += String(i) + F("'>");
    for (unsigned int c = 1; c <= 16; c++) {
      page += F("<option value='");
      page += String(c) + F("'");
      if (banks[b-1][i-1].midiChannel == c) page += F(" selected");
      page += F(">");
      page += String(c) + F("</option>");
    }
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-2'>");
    page += F("<input type='number' class='form-control form-control-sm' name='code");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(banks[b-1][i-1].midiCode);
    page += F("'></div>");

    page += F("<div class='col-2'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value1");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(banks[b-1][i-1].midiValue1);
    page += F("'></div>");

    page += F("<div class='col-2'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value2");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(banks[b-1][i-1].midiValue2);
    page += F("'></div>");

    page += F("<div class='col-2'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value3");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(banks[b-1][i-1].midiValue3);
    page += F("'></div>");

    page += F("<div class='w-100'></div>");
  }
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' class='btn btn-primary'>Save</button>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  page += get_footer_page();

  return page;
}

String get_pedals_page() {

  String page = "";

  page += get_top_page(3);

  page += F("<form method='post'>");
  page += F("<div class='form-row'>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Pedal<br>#</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Auto<br>Sensing</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>Pedal<br>Mode</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>Pedal<br>Function</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Single<br>Press</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Double<br>Press</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Long<br>Press</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Invert<br>Polarity</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>Analog<br>Map</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Analog<br>Zero</span>");
  page += F("</div>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Analog<br>Max</span>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  for (unsigned int i = 1; i <= PEDALS; i++) {
    page += F("<div class='col-1 text-center'>");
    page += String(i);
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='autoCheck");
    page += String(i) + F("' name='autosensing") + String(i) + F("'");
    if (pedals[i-1].autoSensing) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='autoCheck");
    page += String(i) + F("'></p></label>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='col-2'>");
    page += F("<select class='custom-select custom-select-sm' name='mode");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_NONE) + F("'");
    if (pedals[i-1].mode == PED_NONE) page += F(" selected");
    page += F(">None</option>");
    page += F("<option value='");
    page += String(PED_MOMENTARY1) + F("'");
    if (pedals[i-1].mode == PED_MOMENTARY1) page += F(" selected");
    page += F(">Momentary</option>");
    page += F("<option value='");
    page += String(PED_LATCH1) + F("'");
    if (pedals[i-1].mode == PED_LATCH1) page += F(" selected");
    page += F(">Latch</option>");
    page += F("<option value='");
    page += String(PED_ANALOG) + F("'");
    if (pedals[i-1].mode == PED_ANALOG) page += F(" selected");
    page += F(">Analog</option>");
    page += F("<option value='");
    page += String(PED_JOG_WHEEL) + F("'");
    if (pedals[i-1].mode == PED_JOG_WHEEL) page += F(" selected");
    page += F(">Jog Wheel</option>");
    page += F("<option value='");
    page += String(PED_MOMENTARY2) + F("'");
    if (pedals[i-1].mode == PED_MOMENTARY2) page += F(" selected");
    page += F(">Momentary 2</option>");
    page += F("<option value='");
    page += String(PED_MOMENTARY3) + F("'");
    if (pedals[i-1].mode == PED_MOMENTARY3) page += F(" selected");
    page += F(">Momentary 3</option>");
    page += F("<option value='");
    page += String(PED_LATCH2) + F("'");
    if (pedals[i-1].mode == PED_LATCH2) page += F(" selected");
    page += F(">Latch 2</option>");
    page += F("<option value='");
    page += String(PED_LADDER) + F("'");
    if (pedals[i-1].mode == PED_LADDER) page += F(" selected");
    page += F(">Ladder</option>");
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-1'>");
    page += F("<select class='custom-select custom-select-sm' name='function");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_MIDI) + F("'");
    if (pedals[i-1].function == PED_MIDI) page += F(" selected");
    page += F(">MIDI</option>");
    page += F("<option value='");
    page += String(PED_BANK_PLUS) + F("'");
    if (pedals[i-1].function == PED_BANK_PLUS) page += F(" selected");
    page += F(">Bank+</option>");
    page += F("<option value='");
    page += String(PED_BANK_MINUS) + F("'");
    if (pedals[i-1].function == PED_BANK_MINUS) page += F(" selected");
    page += F(">Bank-</option>");
    page += F("<option value='");
    page += String(PED_START) + F("'");
    if (pedals[i-1].function == PED_START) page += F(" selected");
    page += F(">Start</option>");
    page += F("<option value='");
    page += String(PED_STOP) + F("'");
    if (pedals[i-1].function == PED_STOP) page += F(" selected");
    page += F(">Stop</option>");
    page += F("<option value='");
    page += String(PED_CONTINUE) + F("'");
    if (pedals[i-1].function == PED_CONTINUE) page += F(" selected");
    page += F(">Continue</option>");
    page += F("<option value='");
    page += String(PED_TAP) + F("'");
    if (pedals[i-1].function == PED_TAP) page += F(" selected");
    page += F(">Tap</option>");
    page += F("<option value='");
    page += String(PED_MENU) + F("'");
    if (pedals[i-1].function == PED_MENU) page += F(" selected");
    page += F(">Menu</option>");
    page += F("<option value='");
    page += String(PED_CONFIRM) + F("'");
    if (pedals[i-1].function == PED_CONFIRM) page += F(" selected");
    page += F(">Confirm</option>");
    page += F("<option value='");
    page += String(PED_ESCAPE) + F("'");
    if (pedals[i-1].function == PED_ESCAPE) page += F(" selected");
    page += F(">Excape</option>");
    page += F("<option value='");
    page += String(PED_NEXT) + F("'");
    if (pedals[i-1].function == PED_NEXT) page += F(" selected");
    page += F(">Next</option>");
    page += F("<option value='");
    page += String(PED_PREVIOUS) + F("'");
    if (pedals[i-1].function == PED_PREVIOUS) page += F(" selected");
    page += F(">Previous</option>");
    page += F("<option value='");
    page += String(PED_BPM_PLUS) + F("'");
    if (pedals[i-1].function == PED_BPM_PLUS) page += F(" selected");
    page += F(">BPM+</option>");
    page += F("<option value='");
    page += String(PED_BPM_MINUS) + F("'");
    if (pedals[i-1].function == PED_BPM_MINUS) page += F(" selected");
    page += F(">BPM-</option>");
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='singleCheck");
    page += String(i) + F("' name='singlepress") + String(i) + F("'");
    if (pedals[i-1].pressMode == PED_PRESS_1   ||
        pedals[i-1].pressMode == PED_PRESS_1_2 ||
        pedals[i-1].pressMode == PED_PRESS_1_L ||
        pedals[i-1].pressMode == PED_PRESS_1_2_L) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='singleCheck");
    page += String(i) + F("'></p></label>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='doubleCheck");
    page += String(i) + F("' name='doublepress") + String(i) + F("'");
    if (pedals[i-1].pressMode == PED_PRESS_2   ||
        pedals[i-1].pressMode == PED_PRESS_1_2 ||
        pedals[i-1].pressMode == PED_PRESS_2_L || 
        pedals[i-1].pressMode == PED_PRESS_1_2_L) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='doubleCheck");
    page += String(i) + F("'></p></label>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='longCheck");
    page += String(i) + F("' name='longpress") + String(i) + F("'");
    if (pedals[i-1].pressMode == PED_PRESS_L   ||
        pedals[i-1].pressMode == PED_PRESS_1_L ||
        pedals[i-1].pressMode == PED_PRESS_2_L ||
        pedals[i-1].pressMode == PED_PRESS_1_2_L) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='longCheck");
    page += String(i) + F("'></p></label>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='polarityCheck");
    page += String(i) + F("' name='polarity") + String(i) + F("'");
    if (pedals[i-1].invertPolarity) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='polarityCheck");
    page += String(i) + F("'></p></label>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='col-1'>");
    page += F("<select class='custom-select custom-select-sm' name='map");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_LINEAR) + F("'");
    if (pedals[i-1].mapFunction == PED_LINEAR) page += F(" selected");
    page += F(">Linear</option>");
    page += F("<option value='");
    page += String(PED_LOG) + F("'");
    if (pedals[i-1].mapFunction == PED_LOG) page += F(" selected");
    page += F(">Log</option>");
    page += F("<option value='");
    page += String(PED_ANTILOG) + F("'");
    if (pedals[i-1].mapFunction == PED_ANTILOG) page += F(" selected");
    page += F(">Antilog</option>");  
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-1'>");
    page += F("<div class='form-group'>");
    page += F("<label for='minControlRange");
    page += String(i) + F("'></label>");
    page += F("<input type='range' class='custom-range' id='minControlRange");
    page += String(i) + F("' name='min");
    page += String(i) + F("' value='");
    page += String(pedals[i-1].expZero);
    page += String("' min='0' max='");
    page += String(ADC_RESOLUTION) + F("'>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='col-1'>");
    page += F("<div class='form-group'>");
    page += F("<label for='maxControlRange");
    page += String(i) + F("'></label>");
    page += F("<input type='range' class='custom-range' id='maxControlRange");
    page += String(i) + F("' name='max");
    page += String(i) + F("' value='");
    page += String(pedals[i-1].expMax);
    page += String("' min='0' max='");
    page += String(ADC_RESOLUTION) + F("'>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='w-100'></div>");
  }
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' class='btn btn-primary'>Save</button>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  page += get_footer_page();

  return page;
}

String get_interfaces_page() {

  String page = "";
#ifdef PEDALINO_MINI
  const int firstInterface = 3;
#else
  const int firstInterface = 1;
#endif

  page += get_top_page(4);

  page += F("<form method='post'>");
  page += F("<div class='form-row'>");
  for (unsigned int i = firstInterface; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<span class='badge badge-primary'>");
    page += interfaces[i-1].name + String("            ");
    page += F("</span>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<p></p>");

  page += F("<div class='form-row'>");
  for (unsigned int i = firstInterface; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='inCheck");
    page += String(i) + F("' name='in") + String(i) + F("'");
    if (interfaces[i-1].midiIn) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='inCheck");
    page += String(i) + F("'>In</label>");
    page += F("</div>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<div class='form-row'>");
  for (unsigned int i = firstInterface; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='outCheck");
    page += String(i) + F("' name='out") + String(i) + F("'");
    if (interfaces[i-1].midiOut) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='outCheck");
    page += String(i) + F("'>Out</label>");
    page += F("</div>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<div class='form-row'>");
  for (unsigned int i = firstInterface; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='thruCheck");
    page += String(i) + F("' name='thru") + String(i) + F("'");
    if (interfaces[i-1].midiThru) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='thruCheck");
    page += String(i) + F("'>Thru</label>");
    page += F("</div>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<div class='form-row'>");
  for (unsigned int i = firstInterface; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='routingCheck");
    page += String(i) + F("' name='routing") + String(i) + F("'");
    if (interfaces[i-1].midiRouting) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='routingCheck");
    page += String(i) + F("'>Routing</label>");
    page += F("</div>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<div class='form-row'>");
  for (unsigned int i = firstInterface; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<div class='custom-control custom-checkbox'>");
    page += F("<input type='checkbox' class='custom-control-input' id='clockCheck");
    page += String(i) + F("' name='clock") + String(i) + F("'");
    if (interfaces[i-1].midiClock) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='clockCheck");
    page += String(i) + F("'>Clock</label>");
    page += F("</div>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' class='btn btn-primary'>Save</button>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  page += get_footer_page();

  return page;
}

String get_options_page() {

  String        page = "";
  const String  bootswatch[] = { "bootstrap",
                                 "cerulean",
                                 "cosmo",
                                 "cyborg",
                                 "darkly",
                                 "flatly",
                                 "litera",
                                 "journal",
                                 "lumen",
                                 "lux",
                                 "materia",
                                 "minty",
                                 "pulse",
                                 "sandstone",
                                 "simplex",
                                 "sketchy",
                                 "slate",
                                 "solar",
                                 "spacelab",
                                 "superhero",
                                 "united",
                                 "yeti"};

  page += get_top_page(5);

  page += F("<form method='post'>");

  page += F("<div class='form-row'>");
  page += F("<label for='bootstraptheme' class='col-2 col-form-label'>Device Name</label>");
  page += F("<div class='col-10'>");
  page += F("<input class='form-control' type='text' maxlength='32' id='devicename' name='mdnsdevicename' placeholder='' value='");
  page += host + F("'>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>Each device must have a different name. Enter the device name without .local. Web UI will be available at http://<i>device_name</i>.local</p>");
  page += F("<p>Pedalino will be restarted if you change it.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<label for='bootstraptheme' class='col-2 col-form-label'>Web UI Theme</label>");
  page += F("<div class='col-10'>");
  page += F("<select class='custom-select' id='bootstraptheme' name='theme'>");
  for (unsigned int i = 0; i < 22; i++) {
    page += F("<option value='");
    page += bootswatch[i] + F("'");
    if (theme == bootswatch[i]) page += F(" selected");
    page += F(">");
    page += bootswatch[i] + F("</option>");
  }
  page += F("</select>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>Changing default theme require internet connection because themes are served via a CDN network. Only default 'bootstrap' theme has been stored into Pedalino flash memory.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

#ifdef BLYNK
  page += F("<div class='form-row'>");
  page += F("<label for='authtoken' class='col-2 col-form-label'>Blynk Auth Token</label>");
  page += F("<div class='col-10'>");
  page += F("<input class='form-control' type='text' maxlength='32' id='authtoken' name='blynkauthtoken' placeholder='Blynk Auth Token is 32 characters long. Copy and paste from email.' value='");
  page += blynk_get_token() + F("'>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>Auth Token is a unique identifier which is needed to connect your Pedalino to your smartphone. Every Pedalino will have its own Auth Token. You’ll get Auth Token automatically on your email after Pedalino app clone. You can also copy it manually. Click on devices section and selected required device.</p>");
  page += F("<p>Don’t share your Auth Token with anyone, unless you want someone to have access to your Pedalino.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
#endif

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' class='btn btn-primary'>Save</button>");
  page += F("</div>");
  page += F("</div>");

  page += F("</form>");

  page += get_footer_page();

  return page;
}

#ifdef BOOTSTRAP_LOCAL

void http_handle_bootstrap_file(AsyncWebServerRequest *request) {

#ifdef ARDUINO_ARCH_ESP8266
  if (request->url() == "/css/bootstrap.min.css") {
    request->send(SPIFFS, "bootstrap.min.css.gz", "text/css");
  }
  if (request->url() == "/js/jquery-3.3.1.slim.min.js") {
    request->send(SPIFFS, "jquery-3.3.1.slim.min.js.gz", "application/javascript");
  }
  if (request->url() == "/js/popper.min.js") {
    request->send(SPIFFS, "popper.min.js.gz", "application/javascript");
  }
  if (request->url() == "/js/bootstrap.min.js") {
    request->send(SPIFFS, "bootstrap.min.js.gz", "application/javascript");
   }
#endif

#ifdef ARDUINO_ARCH_ESP32
  const uint8_t *file = NULL;
  size_t filesize = 0;

  if (request->url() == "/css/bootstrap.min.css") {
    file = bootstrap_min_css_start;
    filesize = bootstrap_min_css_end - bootstrap_min_css_start;
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", file, filesize);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
  if (request->url() == "/js/jquery-3.3.1.slim.min.js") {
    file = jquery_3_3_1_slim_min_js_start;
    filesize = jquery_3_3_1_slim_min_js_end - jquery_3_3_1_slim_min_js_start;
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", file, filesize);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
  if (request->url() == "/js/popper.min.js") {
    file = popper_min_js_start;
    filesize = popper_min_js_end - popper_min_js_start;
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", file, filesize);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
  if (request->url() == "/js/bootstrap.min.js") {
    file = bootstrap_min_js_start;
    filesize = bootstrap_min_js_end - bootstrap_min_js_start;
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", file, filesize);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
   }
#endif
}
#endif

void http_handle_favicon(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon_ico_gz, FAVICON_ICO_GZ_LEN);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

void http_handle_globals(AsyncWebServerRequest *request) {
  
  if (request->hasArg("profile")) {
    uiprofile = request->arg("profile");
    currentProfile = constrain(uiprofile.toInt() - 1, 0, PROFILES - 1);
    eeprom_update_current_profile(currentProfile);
    eeprom_read();
  }

  if (request->hasArg("theme")) {
    theme = request->arg("theme");
    eeprom_update_theme(theme);
  }  
}

void http_handle_root(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  request->send(200, "text/html", get_root_page());
}

void http_handle_live(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  request->send(200, "text/html", get_live_page());
}

void http_handle_banks(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  if (request->hasArg("bank"))  uibank  = request->arg("bank");
  request->send(200, "text/html", get_banks_page());
}

void http_handle_pedals(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  request->send(200, "text/html", get_pedals_page());
}

void http_handle_interfaces(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  request->send(200, "text/html", get_interfaces_page());
}

void http_handle_options(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  request->send(200, "text/html", get_options_page());
}

void http_handle_post_live(AsyncWebServerRequest *request) {
  
  String a;
  
  a = request->arg("profile");
  currentProfile = a.toInt();

  blynk_refresh();
  alert = "Saved";
  request->send(200, "text/html", get_live_page());
}


void http_handle_post_banks(AsyncWebServerRequest *request) {
  
  String     a;
  const byte b = constrain(uibank.toInt() - 1, 0, BANKS);
  
  for (unsigned int i = 0; i < PEDALS; i++) {
    a = request->arg(String("message") + String(i+1));
    banks[b][i].midiMessage = a.toInt();

    a = request->arg(String("channel") + String(i+1));
    banks[b][i].midiChannel = a.toInt();
    
    a = request->arg(String("code") + String(i+1));
    banks[b][i].midiCode = a.toInt();

    a = request->arg(String("value1") + String(i+1));
    banks[b][i].midiValue1 = a.toInt();
    
    a = request->arg(String("value2") + String(i+1));
    banks[b][i].midiValue2 = a.toInt();

    a = request->arg(String("value3") + String(i+1));
    banks[b][i].midiValue3 = a.toInt();
  }
  eeprom_update();
  blynk_refresh();
  alert = "Saved";
  request->send(200, "text/html", get_banks_page());
}

void http_handle_post_pedals(AsyncWebServerRequest *request) {
  
  String       a;
  const String checked("on");
  
  //httpServer.sendHeader("Connection", "close");
  for (unsigned int i = 0; i < PEDALS; i++) {
    a = request->arg(String("autosensing") + String(i+1));
    pedals[i].autoSensing = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = request->arg(String("mode") + String(i+1));
    pedals[i].mode = a.toInt();

    a = request->arg(String("function") + String(i+1));
    pedals[i].function = a.toInt();

    a = request->arg(String("singlepress") + String(i+1));
    pedals[i].pressMode = (a == checked) ? PED_PRESS_1 : 0;

    a = request->arg(String("doublepress") + String(i+1));
    pedals[i].pressMode += (a == checked) ? PED_PRESS_2 : 0;

    a = request->arg(String("longpress") + String(i+1));
    pedals[i].pressMode += (a == checked) ? PED_PRESS_L : 0;

    a = request->arg(String("polarity") + String(i+1));
    pedals[i].invertPolarity = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = request->arg(String("map") + String(i+1));
    pedals[i].mapFunction = a.toInt();

    a = request->arg(String("min") + String(i+1));
    pedals[i].expZero = a.toInt();

    a = request->arg(String("max") + String(i+1));
    pedals[i].expMax = a.toInt();
  }
  eeprom_update();
  blynk_refresh();
  alert = "Saved";
  request->send(200, "text/html", get_pedals_page());
}

void http_handle_post_interfaces(AsyncWebServerRequest *request) {
  
  String       a;
  const String checked("on");

  for (unsigned int i = 0; i < INTERFACES; i++) {
    a = request->arg(String("in") + String(i+1));
    interfaces[i].midiIn = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = request->arg(String("out") + String(i+1));
    interfaces[i].midiOut = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = request->arg(String("thru") + String(i+1));
    interfaces[i].midiThru = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = request->arg(String("routing") + String(i+1));
    interfaces[i].midiRouting = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = request->arg(String("clock") + String(i+1));
    interfaces[i].midiClock = (a == checked) ? PED_ENABLE : PED_DISABLE;
  }
  eeprom_update();
  blynk_refresh();
  alert = "Saved";
  request->send(200, "text/html", get_interfaces_page());
}

void http_handle_post_options(AsyncWebServerRequest *request) {
  
  http_handle_globals(request);

  if (host != request->arg("mdnsdevicename")) {
    host = request->arg("mdnsdevicename");
    eeprom_update_device_name(host);
    delay(1000);
    ESP.restart();
    alert = "Saved";
  }

  if (request->arg("blynkauthtoken")) {
    blynk_disconnect();
    eeprom_update_blynk_auth_token(request->arg("blynkauthtoken"));
    blynk_set_token(request->arg("blynkauthtoken"));
    blynk_connect();
    blynk_refresh();
    alert = "Saved";
  }

  request->send(200, "text/html", get_options_page());
}

#endif  // WEBCONFIG


String get_update_page() {

  String page = "";

  page += get_top_page(5);

  page += F("<p></p>");
  page += "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

  page += get_footer_page();

  return page;
}

 // handler for the /update form page

void http_handle_update (AsyncWebServerRequest *request) {
  if (request->hasArg("theme")) theme = request->arg("theme");
  // The connection will be closed after completion of the response.
  // The connection SHOULD NOT be considered `persistent'.
  // Applications that do not support persistent connections MUST include the "close" connection option in every message.
  //httpServer.sendHeader("Connection", "close");
  if (!request->authenticate("admin", "password")) {
			return request->requestAuthentication();
	}
  request->send(200, "text/html", get_update_page());
}

// handler for the /update form POST (once file upload finishes)

void http_handle_update_file_upload_finish (AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (Update.hasError()) ? "Update fail!" : "<META http-equiv='refresh' content='15;URL=/'>Update Success! Rebooting...\n");
  response->addHeader("Connection", "close");
  request->send(response);
  delay(1000);
  ESP.restart();
}

// handler for the file upload, get's the sketch bytes, and writes
// them through the Update object

void http_handle_update_file_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

  if (!index) {
    // Disconnect, not to interfere with OTA process
    blynk_disconnect();

    DPRINT("Update Start: %s\n", filename.c_str());
    display_ui_update_disable();
    display_progress_bar_title("HTTP Update");
#ifdef ARDUINO_ARCH_ESP8266
    Update.runAsync(true);
    //size of max sketch rounded to a sector
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (Update.begin(maxSketchSpace)) { //start with max available size
#endif
#ifdef ARDUINO_ARCH_ESP32
    if (Update.begin()) {  //start with max available size
#endif
      DPRINT("Update start\n");
      display_progress_bar_update(0, 100);
    }
    else {
      StreamString str;
      Update.printError(str);
      DPRINT("Update start fail: %s", str.c_str());
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Update aborted");
      display.display();
      ESP.restart();
    }
  }
    
  if (!Update.hasError()) {
    if (Update.write(data, len) == len) {
      if (Update.size()) {
        DPRINT("Progress: %5.1f%%\n", 100.0 * Update.progress() / Update.size());
        display_progress_bar_update(Update.progress(), Update.size());
      }
    }  
    else {
      StreamString str;
      Update.printError(str);
      DPRINT("Update fail: %s", str.c_str());
    }
  }

  if (final) {
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
    display.display();
    if (Update.end(true)) {
      DPRINT("Update Success: %uB\n", index+len);
    } else {
      StreamString str;
      Update.printError(str);
      DPRINT("Update fail: %s", str.c_str());
    }
  }
}

void http_handle_not_found(AsyncWebServerRequest *request) {

  String message = "File Not Found\n\n";

  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";

  for (uint8_t i = 0; i < request->args(); i++) {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

  request->send(404, "text/plain", message);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    DPRINT("ws[%s][%u] connect\n", server->url(), client->id());
    //client->printf("Hello Client %u :)", client->id());
    //client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    DPRINT("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    //error was received from the other end
    DPRINT("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    //pong message was received (in response to a ping request maybe)
    DPRINT("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    //data packet
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      DPRINT("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
      if(info->opcode == WS_TEXT){
        data[len] = 0;
        DPRINT("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < info->len; i++){
          DPRINT("%02x ", data[i]);
        }
        DPRINT("\n");
      }
      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          DPRINT("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        DPRINT("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      DPRINT("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
      if(info->message_opcode == WS_TEXT){
        data[len] = 0;
        DPRINT("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < len; i++){
          DPRINT("%02x ", data[i]);
        }
        DPRINT("\n");
      }

      if((info->index + len) == info->len){
        DPRINT("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          DPRINT("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void http_setup() {

  webSocket.onEvent(onWsEvent);
  httpServer.addHandler(&webSocket);
  //events.setAuthentication("user", "pass");
  httpServer.addHandler(&events);

#ifdef WEBCONFIG
  httpServer.on("/",                        http_handle_root);
  httpServer.on("/favicon.ico", HTTP_GET,   http_handle_favicon);
  httpServer.on("/live",        HTTP_GET,   http_handle_live);
  httpServer.on("/live",        HTTP_POST,  http_handle_post_live);
  httpServer.on("/banks",       HTTP_GET,   http_handle_banks);
  httpServer.on("/banks",       HTTP_POST,  http_handle_post_banks);
  httpServer.on("/pedals",      HTTP_GET,   http_handle_pedals);
  httpServer.on("/pedals",      HTTP_POST,  http_handle_post_pedals);
  httpServer.on("/interfaces",  HTTP_GET,   http_handle_interfaces);
  httpServer.on("/interfaces",  HTTP_POST,  http_handle_post_interfaces);
  httpServer.on("/options",     HTTP_GET,   http_handle_options);
  httpServer.on("/options",     HTTP_POST,  http_handle_post_options);

#ifdef BOOTSTRAP_LOCAL
  httpServer.on("/css/bootstrap.min.css",        http_handle_bootstrap_file);
  httpServer.on("/js/jquery-3.3.1.slim.min.js",  http_handle_bootstrap_file);
  httpServer.on("/js/popper.min.js",             http_handle_bootstrap_file);
  httpServer.on("/js/bootstrap.min.js",          http_handle_bootstrap_file);
#endif  // BOOTSTRAP_LOCAL
#endif  // WEBCONFIG
  httpServer.on("/update",      HTTP_GET,   http_handle_update);
  httpServer.on("/update",      HTTP_POST,  http_handle_update_file_upload_finish, http_handle_update_file_upload);
  httpServer.onNotFound(http_handle_not_found);

  httpServer.begin();
}



inline void http_run() {
  if (millis() % 250 == 0)
    webSocket.binaryAll(display.buffer, 128*64);
}

#endif  // WIFI