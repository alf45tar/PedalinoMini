/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

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
#include <WebServer.h>
#include <Update.h>

WebServer               httpServer(80);

extern const uint8_t css_bootstrap_min_css_start[]        asm("_binary_data_bootstrap_min_css_gz_start");
extern const uint8_t css_bootstrap_min_css_end[]          asm("_binary_data_bootstrap_min_css_gz_end");
extern const uint8_t js_bootstrap_min_js_start[]          asm("_binary_data_bootstrap_min_js_gz_start");
extern const uint8_t js_bootstrap_min_js_end[]            asm("_binary_data_bootstrap_min_js_gz_end");
extern const uint8_t js_jquery_3_3_1_slim_min_js_start[]  asm("_binary_data_jquery_3_3_1_slim_min_js_gz_start");
extern const uint8_t js_jquery_3_3_1_slim_min_js_end[]    asm("_binary_data_jquery_3_3_1_slim_min_js_gz_end");
extern const uint8_t js_popper_min_js_start[]             asm("_binary_data_popper_min_js_gz_start");
extern const uint8_t js_popper_min_js_end[]               asm("_binary_data_popper_min_js_gz_end");
#endif


#ifdef WEBCONFIG

String  blynk_get_token();
String  blynk_set_token(String);
bool    blynk_cloud_connected();
void    blynk_connect();
void    blynk_disconnect();
void    blynk_refresh();


String  theme     = "bootstrap";
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
    page += F("<link rel='stylesheet' href='/css/bootstrap.min.css' integrity='sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO' crossorigin='anonymous'>");
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
  page += F("<nav class='navbar fixed-bottom navbar-light bg-light'>");
  page += F("<a class='navbar-text' href='https://github.com/alf45tar/PedalinoMini'>https://github.com/alf45tar/PedalinoMini</a>");
  page += F("</nav>");

  page += F("</div>");
#ifdef BOOTSTRAP_LOCAL
  page += F("<script src='/js/jquery-3.3.1.slim.min.js' integrity='sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo' crossorigin='anonymous'></script>");
  page += F("<script src='/js/popper.min.js' integrity='sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49' crossorigin='anonymous'></script>");
  page += F("<script src='/js/bootstrap.min.js' integrity='sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy' crossorigin='anonymous'></script>");
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
  page += F("<dt>SDK Version</dt><dd>");
  page += ESP.getSdkVersion();
  page += F("</dd>");
  //page += F("<dt>Running On Core</dt><dd>");
  //page += xPortGetCoreID();
  page += F("</dd>");
  page += F("<dt>Chip ID</dt><dd>");
#ifdef ARDUINO_ARCH_ESP8266
  page += String(ESP.getChipId(), HEX);
#endif
#ifdef ARDUINO_ARCH_ESP32
  page += String((uint32_t)ESP.getEfuseMac(), HEX);
#endif
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
#ifdef ARDUINO_ARCH_ESP8266
  page += String(ESP.getFlashChipId(), HEX);
#endif
#ifdef ARDUINO_ARCH_ESP32
  page += String((uint32_t)ESP.getEfuseMac(), HEX); // Low 4 bytes of MAC address (6 bytes)
#endif
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
  page += F("</div>");

  page += F("<div class='col-3'>");
  page += F("<h3>Wireless</h3>");
  switch (WiFi.getMode()) {
    case WIFI_STA:
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
      break;
    case WIFI_AP:
      page += F("<dt>AP SSID</dt><dd>");
#ifdef ARDUINO_ARCH_ESP8266
      page += WiFi.softAPSSID();
#endif
#ifdef ARDUINO_ARCH_ESP32
      page += String("Pedalino");
#endif
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
      break;
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

  page += get_top_page(4);

  page += F("<form method='post'>");
  page += F("<div class='form-row'>");
  for (unsigned int i = 1; i <= INTERFACES; i++) {
    page += F("<div class='col-2'>");
    page += F("<span class='badge badge-primary'>");
    page += interfaces[i-1].name + String("            ");
    page += F("</span>");
    page += F("</div>");
  }
  page += F("</div>");
  page += F("<p></p>");

  page += F("<div class='form-row'>");
  for (unsigned int i = 1; i <= INTERFACES; i++) {
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
  for (unsigned int i = 1; i <= INTERFACES; i++) {
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
  for (unsigned int i = 1; i <= INTERFACES; i++) {
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
  for (unsigned int i = 1; i <= INTERFACES; i++) {
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
  for (unsigned int i = 1; i <= INTERFACES; i++) {
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

#ifdef NOBLINK
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
void http_handle_bootstrap_file() {

  size_t bytesOut = 0;

#ifdef ARDUINO_ARCH_ESP8266
  File file;

  SPIFFS.begin();
  if (httpServer.uri() == "/css/bootstrap.min.css") {
    file = SPIFFS.open("bootstrap.min.css.gz", "r"); 
    bytesOut = httpServer.streamFile(file, "text/css");
  }
  if (httpServer.uri() == "/js/jquery-3.3.1.slim.min.js") {
    file = SPIFFS.open("jquery-3.3.1.slim.min.js.gz", "r"); 
    bytesOut = httpServer.streamFile(file, "application/javascript");
  }
  if (httpServer.uri() == "/js/popper.min.js") {
    file = SPIFFS.open("popper.min.js.gz", "r"); 
    bytesOut = httpServer.streamFile(file, "application/javascript");
  }
  if (httpServer.uri() == "/js/bootstrap.min.js") {
    file = SPIFFS.open("bootstrap.min.js.gz", "r"); 
    bytesOut = httpServer.streamFile(file, "application/javascript");
   }
   DPRINT("HTTP Requested %s. Sent %d of %d bytes.\n", httpServer.uri().c_str(), bytesOut, file.size());
   file.close();
   SPIFFS.end();
#endif

#ifdef ARDUINO_ARCH_ESP32
  const char  *file = NULL;
  size_t filesize = 0;

  if (httpServer.uri() == "/css/bootstrap.min.css") {
    file = (const char *)css_bootstrap_min_css_start;
    filesize = strlen(file);
    httpServer.setContentLength(filesize);
    httpServer.sendHeader("Content-Encoding", "gzip");
    httpServer.send(200, "text/css", "");
  }
  if (httpServer.uri() == "/js/jquery-3.3.1.slim.min.js") {
    file = (const char *)js_jquery_3_3_1_slim_min_js_start;
    filesize = strlen(file);
    httpServer.setContentLength(filesize);
    httpServer.sendHeader("Content-Encoding", "gzip");
    httpServer.send(200, "application/javascript", "");
  }
  if (httpServer.uri() == "/js/popper.min.js") {
    file = (const char *)js_popper_min_js_start;
    filesize = strlen(file);
    httpServer.setContentLength(filesize);
    httpServer.sendHeader("Content-Encoding", "gzip");
    httpServer.send(200, "application/javascript", "");
  }
  if (httpServer.uri() == "/js/bootstrap.min.js") {
    file = (const char *)js_bootstrap_min_js_start;
    filesize = strlen(file);
    httpServer.setContentLength(filesize);
    httpServer.sendHeader("Content-Encoding", "gzip");
    httpServer.send(200, "application/javascript", "");
   }
  
  const unsigned int PACKET_SIZE = 2*1460;
  for (unsigned int i = 0; i < filesize / PACKET_SIZE && httpServer.client().connected(); i++) {
    bytesOut += httpServer.client().write(&file[i*PACKET_SIZE], PACKET_SIZE);
  }
  if (httpServer.client().connected())
    bytesOut += httpServer.client().write(&file[filesize / PACKET_SIZE * PACKET_SIZE], filesize % PACKET_SIZE);

  DPRINT("HTTP Requested %s. Sent %d of %d bytes.\n", httpServer.uri().c_str(), bytesOut, filesize);
#endif
}
#endif

void http_handle_globals() {
  
  if (httpServer.hasArg("profile")) {
    uiprofile = httpServer.arg("profile");
    currentProfile = constrain(uiprofile.toInt() - 1, 0, PROFILES - 1);
    eeprom_update_current_profile(currentProfile);
    eeprom_read();
  }

  if (httpServer.hasArg("theme")) {
    theme = httpServer.arg("theme");
    eeprom_update_theme(theme);
  }  
}

void http_handle_root() {
  http_handle_globals();
  httpServer.send(200, "text/html", get_root_page());
}

void http_handle_live() {
  http_handle_globals();
  httpServer.send(200, "text/html", get_live_page());
}

void http_handle_banks() {
  http_handle_globals();
  if (httpServer.hasArg("bank"))  uibank  = httpServer.arg("bank");
  httpServer.send(200, "text/html", get_banks_page());
}

void http_handle_pedals() {
  http_handle_globals();
  httpServer.send(200, "text/html", get_pedals_page());
}

void http_handle_interfaces() {
  http_handle_globals();
  httpServer.send(200, "text/html", get_interfaces_page());
}

void http_handle_options() {
  http_handle_globals();
  httpServer.send(200, "text/html", get_options_page());
}

void http_handle_post_live() {
  
  String a;
  
  a = httpServer.arg("profile");
  currentProfile = a.toInt();

  blynk_refresh();
  alert = "Saved";
  httpServer.send(200, "text/html", get_live_page());
}


void http_handle_post_banks() {
  
  String     a;
  const byte b = constrain(uibank.toInt() - 1, 0, BANKS);
  
  for (unsigned int i = 0; i < PEDALS; i++) {
    a = httpServer.arg(String("message") + String(i+1));
    banks[b][i].midiMessage = a.toInt();

    a = httpServer.arg(String("channel") + String(i+1));
    banks[b][i].midiChannel = a.toInt();
    
    a = httpServer.arg(String("code") + String(i+1));
    banks[b][i].midiCode = a.toInt();

    a = httpServer.arg(String("value1") + String(i+1));
    banks[b][i].midiValue1 = a.toInt();
    
    a = httpServer.arg(String("value2") + String(i+1));
    banks[b][i].midiValue2 = a.toInt();

    a = httpServer.arg(String("value3") + String(i+1));
    banks[b][i].midiValue3 = a.toInt();
  }
  blynk_refresh();
  alert = "Saved";
  httpServer.send(200, "text/html", get_banks_page());
}

void http_handle_post_pedals() {
  
  String       a;
  const String checked("on");
  
  //httpServer.sendHeader("Connection", "close");
  for (unsigned int i = 0; i < PEDALS; i++) {
    a = httpServer.arg(String("autosensing") + String(i+1));
    pedals[i].autoSensing = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = httpServer.arg(String("mode") + String(i+1));
    pedals[i].mode = a.toInt();

    a = httpServer.arg(String("function") + String(i+1));
    pedals[i].function = a.toInt();

    a = httpServer.arg(String("singlepress") + String(i+1));
    pedals[i].pressMode = (a == checked) ? PED_PRESS_1 : 0;

    a = httpServer.arg(String("doublepress") + String(i+1));
    pedals[i].pressMode += (a == checked) ? PED_PRESS_2 : 0;

    a = httpServer.arg(String("longpress") + String(i+1));
    pedals[i].pressMode += (a == checked) ? PED_PRESS_L : 0;

    a = httpServer.arg(String("polarity") + String(i+1));
    pedals[i].invertPolarity += (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = httpServer.arg(String("map") + String(i+1));
    pedals[i].mapFunction = a.toInt();

    a = httpServer.arg(String("min") + String(i+1));
    pedals[i].expZero = a.toInt();

    a = httpServer.arg(String("max") + String(i+1));
    pedals[i].expMax = a.toInt();
  }
  blynk_refresh();
  alert = "Saved";
  httpServer.send(200, "text/html", get_pedals_page());
}

void http_handle_post_interfaces() {
  
  String       a;
  const String checked("on");

  for (unsigned int i = 0; i < INTERFACES; i++) {
    a = httpServer.arg(String("in") + String(i+1));
    interfaces[i].midiIn = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = httpServer.arg(String("out") + String(i+1));
    interfaces[i].midiOut = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = httpServer.arg(String("thru") + String(i+1));
    interfaces[i].midiThru = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = httpServer.arg(String("routing") + String(i+1));
    interfaces[i].midiRouting = (a == checked) ? PED_ENABLE : PED_DISABLE;

    a = httpServer.arg(String("clock") + String(i+1));
    interfaces[i].midiClock = (a == checked) ? PED_ENABLE : PED_DISABLE;
  }
  blynk_refresh();
  alert = "Saved";
  httpServer.send(200, "text/html", get_interfaces_page());
}

void http_handle_post_options() {
  
  http_handle_globals();

  if (httpServer.arg("blynkauthtoken")) {
    blynk_disconnect();
    eeprom_update_blynk_auth_token(httpServer.arg("blynkauthtoken"));
    blynk_set_token(httpServer.arg("blynkauthtoken"));
    blynk_connect();
    blynk_refresh();
    alert = "Saved";
  }

  httpServer.send(200, "text/html", get_options_page());
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

void http_handle_update () {
  if (httpServer.hasArg("theme")) theme = httpServer.arg("theme");
  // The connection will be closed after completion of the response.
  // The connection SHOULD NOT be considered `persistent'.
  // Applications that do not support persistent connections MUST include the "close" connection option in every message.
  //httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", get_update_page());
}

// handler for the /update form POST (once file upload finishes)

void http_handle_update_file_upload_finish () {
  //httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/plain", (Update.hasError()) ? "Update fail!" : "<META http-equiv='refresh' content='15;URL=/'>Update Success! Rebooting...\n");
  ESP.restart();
}

// handler for the file upload, get's the sketch bytes, and writes
// them through the Update object

void http_handle_update_file_upload() {
  
  StreamString  str;
  HTTPUpload&   upload = httpServer.upload();

  switch (upload.status) {

    case UPLOAD_FILE_START:
      {
      // Disconnect, not to interfere with OTA process
      blynk_disconnect();

      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 11, "HTTP Update");
      display.display();
      DPRINT("Filename: %s\n", upload.filename.c_str());
#ifdef ARDUINO_ARCH_ESP8266
      //size of max sketch rounded to a sector
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (Update.begin(maxSketchSpace)) { //start with max available size
#endif
#ifdef ARDUINO_ARCH_ESP32
      if (Update.begin()) {  //start with max available size
#endif
        display.drawProgressBar(4, 32, 120, 8, 0);
        display.display();
        DPRINT("Update start\n");
      }
      else {
        Update.printError(str);
        DPRINT("Update start fail: %s", str.c_str());
      }
      break;
      }
    case UPLOAD_FILE_WRITE:
      if (Update.write(upload.buf, upload.currentSize) == upload.currentSize) {
        if (Update.size()) {
          display.drawProgressBar(4, 32, 120, 8, 100 * Update.progress() / Update.size());
          display.display();
          DPRINT("Progress: %5.1f%%\n", 100.0 * Update.progress() / Update.size());
        }
      }
      else {
        Update.printError(str);
        DPRINT("Update fail: %s", str.c_str());
      }
      break;

    case UPLOAD_FILE_END:
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
      display.display();
      if (Update.end(true)) { //true to set the size to the current progress
        DPRINT("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(str);
        DPRINT("Update fail: %s", str.c_str());
      }
      break;

    case UPLOAD_FILE_ABORTED:
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Update aborted");
      display.display();
      ESP.restart();
      break;
  }
}

void http_handle_not_found() {

  String message = "File Not Found\n\n";

  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";

  for (uint8_t i = 0; i < httpServer.args(); i++) {
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }

  httpServer.send(404, "text/plain", message);
}

void http_setup() {

#ifdef WEBCONFIG
  httpServer.on("/",                        http_handle_root);
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
}