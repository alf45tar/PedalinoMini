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

String theme         = "bootstrap";
String httpUsername  = "admin";
String httpPassword  = getChipId();
bool   authenticated = false;

#ifdef NOWIFI
inline void http_run() {};
#else

#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <StreamString.h>
#include <FS.h>
#include <SPIFFS.h>
#include <nvs.h>

AsyncWebServer          httpServer(80);

#ifdef WEBCONFIG

#ifdef WEBSOCKET
AsyncWebSocket               webSocket("/ws");
AsyncEventSource             events("/events");    // EventSource is single direction, text-only protocol.
AsyncWebSocketMessageBuffer *buffer = NULL;
AsyncWebSocketClient        *wsClient = NULL;
#endif

String page         = "";
String alert        = "";
String alertError   = "";
String uiprofile    = "1";
String uibank       = "1";
String uipedal      = "All";
String uisequence   = "1";


void get_top_page(int p = 0) {

  page = "";

  page += F("<!doctype html>");
  page += F("<html lang='en'>");
  page += F("<head>");
  page += F("<title>PedalinoMini&trade;</title>");
  page += F("<meta charset='utf-8'>");
  page += F(" <meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
  if ( theme == "bootstrap" ) {
  #ifdef BOOTSTRAP_LOCAL
    page += F("<link rel='stylesheet' href='/css/bootstrap.min.css' integrity='sha256-aAr2Zpq8MZ+YA/D6JtRD3xtrwpEz2IqOS+pWD/7XKIw=' crossorigin='anonymous'>");
  #else
    page += F("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/css/bootstrap.min.css' integrity='sha384-9aIt2nRpC12Uk9gS9baDl411NQApFmC26EwAOH8WgZl5MYYxFfc+NcPb1dKGj7Sk' crossorigin='anonymous'>");
  #endif
  } else {
    page += F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootswatch/4.5.0/");
    page += theme;
    page += F("/bootstrap.min.css' crossorigin='anonymous'>");
  }
  if (p == -1) page += F("<link rel='stylesheet' href='/css/floating-labels.css'>");
  page += F("</head>");

  page += F("<body>");
  if (p >= 0) {
  page += F("<div class='container-fluid mt-3 mb-3'>");

  page += F("<nav class='navbar navbar-expand-md navbar-light bg-light mb-3'>");
  page += F("<a class='navbar-brand' href='/'>");
  page += F("<img src='/logo.png' width='30' height='30' class='d-inline-block align-top' alt=''></a>");
  page += F("<button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarNavDropdown' aria-controls='navbarNavDropdown' aria-expanded='false' aria-label='Toggle navigation'>");
  page += F("<span class='navbar-toggler-icon'></span>");
  page += F("</button>");
  page += F("<div class='collapse navbar-collapse' id='navbarNavDropdown'>");
  page += F("<ul class='navbar-nav mr-auto'>");
  /*
  page += F("<li class='nav-item");
  page += (p == 1 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/live'>Live</a>");
  page += F("</li>");
  */
  page += F("<li class='nav-item");
  page += (p == 2 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/actions'>Actions</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 4 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/pedals'>Pedals</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 5 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/interfaces'>Interfaces</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 6 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/sequences'>Sequences</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 7 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/options'>Options</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 8 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/configurations'>Configurations</a>");
  page += F("</li>");
  page += F("</ul>");
  }
  if (p != -1 && p != 0 && p != 7)
  {
    page += F("<form class='form-inline my-2 my-lg-0'>");
    page += currentProfile == 0 ? F("<a class='btn btn-primary' href='?profile=1' role='button'>A</a>") : F("<a class='btn btn-outline-primary' href='?profile=1' role='button'>A</a>");
    page += currentProfile == 1 ? F("<a class='btn btn-primary' href='?profile=2' role='button'>B</a>") : F("<a class='btn btn-outline-primary' href='?profile=2' role='button'>B</a>");
    page += currentProfile == 2 ? F("<a class='btn btn-primary' href='?profile=3' role='button'>C</a>") : F("<a class='btn btn-outline-primary' href='?profile=3' role='button'>C</a>");
    page += F("</form>");
  }
  page += F("</div>");
  page += F("</nav>");

  if (alert != "") {
    page += F("<div class='alert alert-success alert-dismissible fade show' role='alert'>");
    page += alert;
    page += F("<button type='button' class='close' data-dismiss='alert' aria-label='Close'>");
    page += F("<span aria-hidden='true'>&times;</span>");
    page += F("</button>");
    page += F("</div>");
    alert = "";
  }
  if (alertError != "") {
    page += F("<div class='alert alert-danger alert-dismissible fade show' role='alert'>");
    page += alertError;
    page += F("<button type='button' class='close' data-dismiss='alert' aria-label='Close'>");
    page += F("<span aria-hidden='true'>&times;</span>");
    page += F("</button>");
    page += F("</div>");
    alertError = "";
  }
}

void get_footer_page() {

  page += F("</div>");
#ifdef BOOTSTRAP_LOCAL
  page += F("<script src='/js/jquery-3.5.1.slim.min.js' integrity='sha256-xG3AUc6BxK8rIJarv4ha5Lp0Z/9dsPAQbO7pKM82WKM=' crossorigin='anonymous'></script>");
  page += F("<script src='/js/popper.min.js' integrity='sha256-x3YZWtRjM8bJqf48dFAv/qmgL68SI4jqNWeSLMZaMGA=' crossorigin='anonymous'></script>");
  page += F("<script src='/js/bootstrap.min.js' integrity='sha256-OFRAJNoaD8L3Br5lglV7VyLRf0itmoBzWUoM+Sji4/8=' crossorigin='anonymous'></script>");
#else
  page += F("<script src='https://code.jquery.com/jquery-3.5.1.slim.min.js' integrity='sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj' crossorigin='anonymous'></script>");
  page += F("<script src='https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js' integrity='sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo' crossorigin='anonymous'></script>");
  page += F("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/js/bootstrap.min.js' integrity='sha384-OgVRvuATP1z7JjHLkuOU7Xw704+h835Lr+6QL9UvYjZE3Ipu6Tp75j7Bh/kR0JKI' crossorigin='anonymous'></script>");
#endif
  page += F("</body>");
  page += F("</html>");
}

void get_login_page() {

  get_top_page(-1);

  page += F("<div class='col-4'>");
  page += F("</div>");

  page += F("<div class='col-4'>");
  page += F("<form class='form-signin'>");
  page += F("<div class='text-center mb-4'>");
  page += F("<img class='mb-4' src='/logo.png' alt='' width='64' height='64'>");
  page += F("<h1 class='h3 mb-3 font-weight-normal'>PedalinoMini&trade;</h1>");
  page += F("<p>Wireless MIDI foot controller <a href='https://github.com/alf45tar/PedalinoMini'>More info</a></p>");
  page += F("</div>");

  page += F("<div class='form-label-group'>");
  page += F("<input type='text' id='username' class='form-control' placeholder='Username' required='' autofocus=''>");
  page += F("<label for='username'>Username</label>");
  page += F("</div>");

  page += F("<div class='form-label-group'>");
  page += F("<input type='password' id='password' class='form-control' placeholder='Password' required=''>");
  page += F("<label for='password'>Password</label>");
  page += F("</div>");

  page += F("<div class='checkbox mb-3'>");
  page += F("<label>");
  page += F("<input type='checkbox' value='remember-me'> Remember me");
  page += F("</label>");
  page += F("</div>");
  page += F("<button class='btn btn-lg btn-primary btn-block' type='submit'>Sign in</button>");
  page += F("<p class='mt-5 mb-3 text-muted text-center'>© 2018-2019</p>");
  page += F("</form>");
  page += F("</div>");

  page += F("<div class='col-4'>");
  page += F("</div>");

  get_footer_page();
}

void get_root_page() {

  get_top_page();

  page += F("<h4 class='display-4'>Wireless MIDI foot controller</h4>");

  page += F("<div class='row' mt-3>");

  page += F("<div class='col-6 col-sm-3'>");
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
  page += F("<dt>Sequences</dt><dd>");
  page += String(SEQUENCES);
  page += F("</dd>");
  page += F("<dt>Boot Mode</dt><dd>");
  switch (bootMode) {
    case PED_BOOT_NORMAL:
      page += F("Normal");
      break;
    case PED_BOOT_BLE:
      page += F("BLE only");
      break;
    case PED_BOOT_WIFI:
      page += F("WiFi only");
      break;
    case PED_BOOT_AP:
      page += F("Access Point and BLE");
      break;
    case PED_BOOT_AP_NO_BLE:
      page += F("Access Point without BLE");
      break;
  }
  page += F("</dd>");
  page += F("<dt>SDK Version</dt><dd>");
  page += ESP.getSdkVersion();
  page += F("</dd>");
  page += F("<dt>PlatformIO Build Env</dt><dd>");
  page += xstr(PLATFORMIO_ENV);
  page += F("</dd>");
  page += F("</div>");

  page += F("<div class='col-6 col-sm-3'>");
  page += F("<h3>Hardware</h3>");
  page += F("<dt>Board</dt><dd>");
  page += ARDUINO_BOARD;
  page += F("</dd>");
  page += F("<dt>Chip</dt><dd>");
  page += String("ESP32");
  page += F("</dd>");
  page += F("<dt>Chip Revision</dt><dd>");
  page += ESP.getChipRevision();
  page += F("</dd>");
  page += F("</dd>");
  page += F("<dt>Chip ID</dt><dd>");
  page += getChipId();
  page += F("</dd>");
  page += F("<dt>CPU Frequency</dt><dd>");
  page += ESP.getCpuFreqMHz();
  page += F(" MHz</dd>");
  page += F("<dt>Flash Chip Frequency</dt><dd>");
  page += ESP.getFlashChipSpeed() / 1000000;
  page += F(" MHz</dd>");
  page += F("<dt>Flash Size</dt><dd>");
  page += ESP.getFlashChipSize() / (1024 * 1024);
  page += F(" MB</dd>");
  page += F("<dt>PSRAM Used/Total</dt><dd>");
  page += (ESP.getPsramSize() - ESP.getFreePsram()) / 1024;
  page += F("/");
  page += ESP.getPsramSize() / 1024;
  page += F(" kB</dd>");
  nvs_stats_t nvs_stats;
  if (nvs_get_stats("nvs", &nvs_stats) == ESP_OK) {
    page += F("<dt>NVS Used/Total</dt><dd>");
    page += nvs_stats.used_entries;
    page += F("/");
    page += nvs_stats.total_entries;
    page += F(" entries</dd>");
  }
  page += F("<dt>SPIFFS Used/Total</dt><dd>");
  page += SPIFFS.usedBytes() / 1024;
  page += F("/");
  page += SPIFFS.totalBytes() / 1024;
  page += F(" kB</dd>");
  page += F("<dt>Free Heap Size</dt><dd>");
  page += ESP.getFreeHeap() / 1024;
  page += F(" kB</dd>");
  //page += F("<dt>Running On Core</dt><dd>");
  //page += xPortGetCoreID();
  //page += F("</dd>");
  page += F("</div>");

  page += F("<div class='col-6  col-sm-3'>");
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
  page += ssidSoftAP;
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
  page += F("<dt>Hostname</dt><dd>");
  page += WiFi.softAPgetHostname();
  page += F("</dd>");
  page += F("<dt>Connected Stations</dt><dd>");
  page += WiFi.softAPgetStationNum();
  page += F("</dd>");
  }
  page += F("</div>");

  page += F("<div class='col-6 col-sm-3'>");
  page += F("<h3>Network</h3>");
  page += F("<dt>Hostname</dt><dd>");
  page += WiFi.getHostname() + String(".local");
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
#ifdef BLINK
  page += F("<dt>Blynk Cloud</dt><dd>");
  if (blynk_cloud_connected()) page += String("Online");
  else page += String("Offline");
  page += F("</dd>");
#endif
  page += F("<dt>MIDI Network</dt><dd>");
  if (appleMidiConnected) page += String("Connected to<br>") + appleMidiSessionName;
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
  get_footer_page();
}

void get_live_page() {

  get_top_page(1);

  page += F("<div aria-live='polite' aria-atomic='true' style='position: relative;'>"
            "<div id='remotedisplay' class='toast' style='position: absolute; top: 0; right: 0; max-width: 600px;' data-autohide='false'>"
            "<div class='toast-header'>"
            "<strong class='mr-auto'>Remote Display</strong>"
            "<button type='button' class='ml-2 mb-1 close' data-dismiss='toast' aria-label='Close'>"
            "<span aria-hidden='true'>&times;</span>"
            "</button>"
            "</div>"
            "<div class='toast-body'>"
            "<canvas id='screen' height='64' width='128'>"
            "Sorry, your browser does not support canvas."
            "</canvas><br><small>"
            "<a id='zoom1' href='#' role='button'>1x</a> "
            "<a id='zoom2' href='#' role='button'>2x</a> "
            "<a id='zoom4' href='#' role='button'>4x</a> "
            "<a id='invert' href='#' role='button'>Invert</a></small>"
            "</div>"
            "</div>"
            "</div>"

            "<div id='live'>"
            "<a id='showremotedisplay' href='#' role='button'>Remote Display</a>"
            "<p></p>"
            "<small>Bank</small><br>"
            "<div class='btn-group btn-group-toggle' data-toggle='buttons'>");
  for (unsigned int i = 1; i <= BANKS; i++) {
    page += F("<label class='btn btn-outline-primary'>"
              "<input type='radio' name='options' autocomplete='off' id='bank");
    page += String(i);
    page += F("'>");
    page += String(i);
    page += F("</label>");
  }
  page += F("</div>"
            "<p></p>"

            "<div class='btn-group'>"
            "<button type='button' class='btn btn-primary dropdown-toggle' data-toggle='dropdown' aria-haspopup='true' aria-expanded='false'>"
            "MIDI Clock</button>"
            "<div class='dropdown-menu'>"
            "<a id='clock-master' class='dropdown-item' href='#'>Master</a>"
            "<a id='clock-slave'  class='dropdown-item' href='#'>Slave</a>"
            "</div>"
            "</div>"

            "<div class='btn-group'>"
            "<button type='button' class='btn btn-outline-primary dropdown-toggle' data-toggle='dropdown' aria-haspopup='true' aria-expanded='false'>"
            "Time Signature</button>"
            "<div class='dropdown-menu'>"
            "<a id='4_4' class='dropdown-item' href='#'>4/4 Common Time</a>"
            "<a id='3_4' class='dropdown-item' href='#'>3/4 Waltz Time</a>"
            "<a id='2_4' class='dropdown-item' href='#'>2/4 March Time</a>"
            "<a id='3_8' class='dropdown-item' href='#'>3/8</a>"
            "<a id='6_8' class='dropdown-item' href='#'>6/8</a>"
            "<a id='9_8' class='dropdown-item' href='#'>9/8</a>"
            "<a id='12_8' class='dropdown-item' href='#'>12/8</a>"
            "</div>"
            "</div>"

            "<div class='btn-group'>"
            "<button type='button' class='btn btn-primary dropdown-toggle' data-toggle='dropdown' aria-haspopup='true' aria-expanded='false'>"
            "MTC</button>"
            "<div class='dropdown-menu'>"
            "<a id='mtc-master' class='dropdown-item' href='#'>Master</a>"
            "<a id='mtc-slave' class='dropdown-item' href='#'>Slave</a>"
            "</div>"
            "</div>"
            "<p></p>"

            "<div>"
            "<h1 id='bpm'></h1> bpm"
            "<h1 id='timesignature'></h1>"
            "<h1 id='beat'></h1>"
            "<h1 id='mtc'></h1>"
            "</div>"
            "<p></p>"

            "<button id='start' type='button' class='btn btn-outline-primary'>Start</button>"
            "<button id='stop' type='button' class='btn btn-outline-primary'>Stop</button>"
            "<button id='continue' type='button' class='btn btn-outline-primary'>Continue</button>"
            "<button id='tap' type='button' class='btn btn-outline-primary'>Tap</button>"
            "</div>"

            "<script>"
            "var isplaying = 0;"
            "var invert = 0;"
            "var zoom = 1;"
            "var con;"
            "var source;"

            "function webSocketConnect() {"
            "con = new WebSocket('ws://' + location.hostname + ':80/ws');"
            "con.binaryType = 'arraybuffer';"
            "con.onopen = function () {"
            "console.log('WebSocket to Pedalino open');"
            "$('#live').find('input, button, submit, textarea, select').removeAttr('disabled');"
            "$('#live').find('a').removeClass('disablehyper').unbind('click');"
            "};"
            "con.onerror = function (error) {"
            "console.log('WebSocket to Pedalino error ', error);"
            "};"
            "con.onmessage = function (e) {"
            "var data = e.data;"
            "var dv = new DataView(data);"
  //          "if (dv.buffer.byteLength != 1024) return;"
            "var canvas=document.getElementById('screen');"
            "var context=canvas.getContext('2d');"
            "var x=0; y=0;"
            "for (y=0; y<64; y++)"
            "  for (x=0; x<128; x++)"
            "    if ((dv.getUint8(x+Math.floor(y/8)*128) & (1<<(y&7))) == 0){"
            "      (invert == 0) ? context.clearRect(x*zoom,y*zoom,zoom,zoom) : context.fillRect(x*zoom,y*zoom,zoom,zoom);"
            "    } else {(invert == 0) ? context.fillRect(x*zoom,y*zoom,zoom,zoom) : context.clearRect(x*zoom,y*zoom,zoom,zoom);}"
            "};"
            "con.onclose = function () {"
            "console.log('WebSocket to Pedalino closed');"
            "$('#live').find('input, button, submit, textarea, select').attr('disabled', 'disabled');"
            "$('#live').find('a').addClass('disablehyper').click(function (e) { e.preventDefault(); });"
            "};"
            "setInterval(keepAliveConnection, 1000);"
            "};"

            "function keepAliveConnection() {"
            "if (con.readyState == WebSocket.CLOSED) webSocketConnect();"
            "if (source.readyState == EventSource.CLOSED) eventSourceConnect();"
            "};"

            "webSocketConnect();"

            "function eventSourceConnect() {"
            "if (!!window.EventSource) {"
            "source = new EventSource('/events');"
            "source.addEventListener('open', function(e) {"
            "console.log('Events Connected');"
            "}, false);"
            "source.addEventListener('error', function(e) {"
            "if (e.target.readyState != EventSource.OPEN) {"
            "console.log('Events Disconnected');"
            "}"
            "}, false);"
            "source.addEventListener('message', function(e) {"
            "console.log('Event: ', e.data);"
            "}, false);"
            "source.addEventListener('play', function(e) { isplaying = e.data; }, false);"
            "source.addEventListener('timesignature', function(e) {"
            "document.getElementById('timesignature').innerHTML = e.data;"
            "}, false);"
            "source.addEventListener('bpm', function(e) {"
            "document.getElementById('bpm').innerHTML = e.data;"
            "}, false);"
            "source.addEventListener('beat', function(e) {"
            "document.getElementById('beat').innerHTML = e.data;"
            "}, false);"
            "source.addEventListener('mtc', function(e) {"
            "document.getElementById('mtc').innerHTML = e.data;"
	          "}, false);"
            "source.addEventListener('screen', function(e) {"
            "}, false);"
            "}"
            "}"

            "eventSourceConnect();"

            "function sendBinary(str) {"
            "if (con.readyState != WebSocket.OPEN || con.bufferedAmount > 0) return;"
            "var buffer = new ArrayBuffer(str.length+1);"
            "var view = new DataView(buffer);"
            "for (i=0; i<str.length; i++)"
            "  view.setUint8(i, str.charCodeAt(i));"
            "view.setUint8(str.length, 0);"
            "con.send(view);"
            "}"

            "document.getElementById('showremotedisplay').onclick = function() {"
            "$('#remotedisplay').toast('show');"
            "setInterval(requestRemoteDisplay, 1000);"
            "return false; };"

            "function requestRemoteDisplay() {sendBinary('.');}"

            "function resizeScreen(z) {"
            "zoom = z;"
            "var canvas=document.getElementById('screen');"
            "var context=canvas.getContext('2d');"
            "context.canvas.width = 128*zoom;"
            "context.canvas.height = 64*zoom;"
            "};");

  for (unsigned int i = 1; i <= BANKS; i++) {
    page += F("document.getElementById('bank");
    page += String(i);
    page += F("').onchange = function() {"
              "sendBinary('bank");
    page += String(i);
    page += F("');"
              "return false; };");
  }

  page += F("document.getElementById('invert').onclick = function() {"
            "if (invert == 0 ) invert = 1; else invert = 0; return false; };"
            "document.getElementById('zoom1').onclick = function() { resizeScreen(1); return false; };"
            "document.getElementById('zoom2').onclick = function() { resizeScreen(2); return false; };"
            "document.getElementById('zoom4').onclick = function() { resizeScreen(4); return false; };"

            "document.getElementById('clock-master').onclick = function() {"
            "sendBinary('clock-master');"
            "return false; };"
            "document.getElementById('clock-slave').onclick = function() {"
            "sendBinary('clock-slave');"
            "return false; };"
            "document.getElementById('mtc-master').onclick = function() {"
            "sendBinary('mtc-master');"
            "return false; };"
            "document.getElementById('mtc-slave').onclick = function() {"
            "sendBinary('mtc-slave');"
            "return false; };"

            "document.getElementById('4_4').onclick = function() {"
            "sendBinary('4/4');"
            "return false; };"
            "document.getElementById('3_4').onclick = function() {"
            "sendBinary('3/4');"
            "return false; };"
            "document.getElementById('2_4').onclick = function() {"
            "sendBinary('2/4');"
            "return false; };"
            "document.getElementById('3_8').onclick = function() {"
            "sendBinary('3/8');"
            "return false; };"
            "document.getElementById('6_8').onclick = function() {"
            "sendBinary('6/8');"
            "return false; };"
            "document.getElementById('9_8').onclick = function() {"
            "sendBinary('9/8');"
            "return false; };"
            "document.getElementById('12_8').onclick = function() {"
            "sendBinary('12/8');"
            "return false; };"

            "document.getElementById('start').onclick = function() {"
            "sendBinary('start');"
            "return false; };"
            "document.getElementById('stop').onclick = function() {"
            "sendBinary('stop');"
            "return false; };"
            "document.getElementById('continue').onclick = function() {"
            "sendBinary('continue');"
            "return false; };"
            "document.getElementById('tap').onclick = function() {"
            "sendBinary('tap');"
            "return false; };"

            "</script>");

  get_footer_page();

  DPRINT("/live %d bytes\n", page.length());
}

void get_actions_page() {

  const byte   b = constrain(uibank.toInt(), 1, BANKS);
  //const byte   p = constrain(uipedal.toInt(), 1, PEDALS);
  action      *act;
  unsigned int i;
  bool         same_pedal;
  byte         maxbutton;

  get_top_page(2);

  page += F("<div class='row mb-3'>");
  page += F("<div class='col-8'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Bank ");
  page += uibank;
  page += F("</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='input-group input-group-sm'>");
  page += F("<div class='btn-group flex-wrap'>");
  for (i = 1; i <= BANKS; i++) {
    page += F("<form method='get'><button type='button submit' class='btn btn-sm btn-block");
    page += (uibank == String(i) ? String(" btn-primary") : String(""));
    page += F("' name='bank' value='");
    page += String(i) + F("'>");
    if (String(banknames[i-1]).isEmpty())
      page += String(i);
    else
      page += String(banknames[i-1]);
    page += F("</button>");
    page += F("</form>");
  }
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='col-4'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Pedal ");
  page += uipedal + F("</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='input-group input-group-sm'>");
  page += F("<div class='btn-group flex-wrap'>");
  page += F("<form method='get'><button type='button submit' class='btn btn-sm");
  page += (uipedal == String("All") ? String(" btn-primary") : String(""));
  page += F("' name='pedal' value='All'>All</button>");
  page += F("</form>");
  for (i = 1; i <= PEDALS; i++) {
    page += F("<form method='get'><button type='button submit' class='btn btn-sm");
    page += (uipedal == String(i) ? String(" btn-primary") : String(""));
    page += F("' name='pedal' value='");
    page += String(i) + F("'>") + String(i) + F("</button>");
    page += F("</form>");
  }
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<form method='post'>");

  page += F("<div class='card mb-3'>");
  page += F("<h5 class='card-header'>Actions</h5>");
  page += F("<div class='card-body'>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-5'>");
  page += F("<div class='input-group input-group-sm'>");
  page += F("<div class='input-group-prepend'>");
  page += F("<div class='input-group-text'>Bank Name</div>");
  page += F("</div>");
  page += F("<input type='text' class='form-control form-control-sm' name='bankname' maxlength='");
  page += String(MAXBANKNAME) + F("' value='");
  page += String(banknames[b-1]) + F("'>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='col-7 text-right'>");
  page += F("<div class='btn-group' role='group'>");
  page += F("<button id='btnGroupNewAction' type='button' class='btn btn-primary btn-sm dropdown-toggle' data-toggle='dropdown' aria-haspopup='true' aria-expanded='false'>New Action</button>");
  page += F("<div class='dropdown-menu' aria-labelledby='btnGroupNewAction'>");
  for (i = 1; i <= PEDALS; i++) {
    page += F("<button type='submit' class='dropdown-item' name='action' value='new");
    page += String(i) + F("'>Pedal ");
    page += String(i) + F("</button>");
  }
  page += F("</div>");
  page += F("</div>");
  if (actions[b-1] != nullptr) {
    page += F(" ");
    page += F("<button type='submit' name='action' value='delete' class='btn btn-danger btn-sm'>Delete Selected Actions</button>");
  }
  page += F("</div>");
  page += F("</div>");

  i = 1;
  act = actions[b-1];
  same_pedal = false;
  while (act != nullptr) {
    if (uipedal != String(act->pedal + 1) && !(uipedal == String("All"))) {
      act = act->next;
      continue;
    }

    if (!same_pedal) {
      page += F("<div class='card mt-3'>");
      page += F("<div class='card-body'>");
      page += F("<h5 class='card-title'>Pedal ");
      page += String(act->pedal + 1) + F("</h5>");
    }
    page += F("<div class='form-row mt-2'>");

    page += F("<div class='col-11'>");

    page += F("<div class='input-group input-group-sm mb-1'>");
    page += F("<div class='input-group-prepend w-25'>");
    page += F("<div class='input-group-text w-100'>On Button</div>");
    page += F("</div>");
    page += F("<select class='custom-select custom-select-sm' name='button");
    page += String(i) + F("'>");
    switch (pedals[act->pedal].mode) {
      case PED_MOMENTARY2:
      case PED_LATCH2:
        maxbutton = 2;
        break;

      case PED_MOMENTARY3:
        maxbutton = 3;
        break;

      case PED_LADDER:
        maxbutton = LADDER_STEPS;
        break;

      default:
        maxbutton = 1;
        break;
    }
    for (unsigned int b = 1; b <= maxbutton; b++) {
      page += F("<option value='");
      page += String(b) + F("'");
      if (act->button == b - 1) page += F(" selected");
      page += F(">");
      page += String(b) + F("</option>");
    }
    page += F("</select>");

    page += F("<select class='custom-select custom-select-sm' name='event");
    page += String(i) + F("'>");
    switch (pedals[act->pedal].mode) {
      case PED_MOMENTARY1:
      case PED_MOMENTARY2:
      case PED_MOMENTARY3:
      case PED_LATCH1:
      case PED_LATCH2:
      case PED_LADDER:
        page += F("<option value='");
        page += String(PED_EVENT_PRESS) + F("'");
        if (act->event == PED_EVENT_PRESS) page += F(" selected");
        page += F(">");
        page += F("Press</option>");
        page += F("<option value='");
        page += String(PED_EVENT_RELEASE) + F("'");
        if (act->event == PED_EVENT_RELEASE) page += F(" selected");
        page += F(">");
        page += F("Release</option>");
        page += F("<option value='");
        page += String(PED_EVENT_CLICK) + F("'");
        if (act->event == PED_EVENT_CLICK) page += F(" selected");
        page += F(">");
        page += F("Click</option>");
        page += F("<option value='");
        page += String(PED_EVENT_DOUBLE_CLICK) + F("'");
        if (act->event == PED_EVENT_DOUBLE_CLICK) page += F(" selected");
        page += F(">");
        page += F("Double Click</option>");
        page += F("<option value='");
        page += String(PED_EVENT_LONG_PRESS) + F("'");
        if (act->event == PED_EVENT_LONG_PRESS) page += F(" selected");
        page += F(">");
        page += F("Long Press</option>");
        break;

      case PED_ANALOG:
        page += F("<option value='");
        page += String(PED_EVENT_MOVE) + F("'");
        if (act->event == PED_EVENT_MOVE) page += F(" selected");
        page += F(">");
        page += F("Move</option>");
        break;

      case PED_JOG_WHEEL:
        page += F("<option value='");
        page += String(PED_EVENT_JOG) + F("'");
        if (act->event == PED_EVENT_JOG) page += F(" selected");
        page += F(">");
        page += F("Jog</option>");
        break;
     }
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='input-group input-group-sm mb-1'>");
    page += F("<div class='input-group-prepend w-25'>");
    page += F("<div class='input-group-text w-100'>Send</div>");
    page += F("</div>");
    page += F("<select class='custom-select custom-select-sm' name='message");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_EMPTY) + F("'");
    if (act->midiMessage == PED_EMPTY) page += F(" selected");
    page += F("></option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE) + F("'");
    if (act->midiMessage == PED_PROGRAM_CHANGE) page += F(" selected");
    page += F(">Program Change</option>");
    page += F("<option value='");
    page += String(PED_CONTROL_CHANGE) + F("'");
    if (act->midiMessage == PED_CONTROL_CHANGE) page += F(" selected");
    page += F(">Control Change</option>");
    page += F("<option value='");
    page += String(PED_NOTE_ON) + F("'");
    if (act->midiMessage == PED_NOTE_ON) page += F(" selected");
    page += F(">Note On</option>");
    page += F("<option value='");
    page += String(PED_NOTE_OFF) + F("'");
    if (act->midiMessage == PED_NOTE_OFF) page += F(" selected");
    page += F(">Note Off</option>");
    page += F("<option value='");
    page += String(PED_BANK_SELECT_INC) + F("'");
    if (act->midiMessage == PED_BANK_SELECT_INC) page += F(" selected");
    page += F(">Bank Select+</option>");
    page += F("<option value='");
    page += String(PED_BANK_SELECT_DEC) + F("'");
    if (act->midiMessage == PED_BANK_SELECT_DEC) page += F(" selected");
    page += F(">Bank Select-</option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE_INC) + F("'");
    if (act->midiMessage == PED_PROGRAM_CHANGE_INC) page += F(" selected");
    page += F(">Program Change+</option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE_DEC) + F("'");
    if (act->midiMessage == PED_PROGRAM_CHANGE_DEC) page += F(" selected");
    page += F(">Program Change-</option>");
    page += F("<option value='");
    page += String(PED_PITCH_BEND) + F("'");
    if (act->midiMessage == PED_PITCH_BEND) page += F(" selected");
    page += F(">Pitch Bend</option>");
    page += F("<option value='");
    page += String(PED_CHANNEL_PRESSURE) + F("'");
    if (act->midiMessage == PED_CHANNEL_PRESSURE) page += F(" selected");
    page += F(">Channel Pressure</option>");
    page += F("<option value='");
    page += String(PED_MIDI_START) + F("'");
    if (act->midiMessage == PED_MIDI_START) page += F(" selected");
    page += F(">Start</option>");
    page += F("<option value='");
    page += String(PED_MIDI_STOP) + F("'");
    if (act->midiMessage == PED_MIDI_STOP) page += F(" selected");
    page += F(">Stop</option>");
    page += F("<option value='");
    page += String(PED_MIDI_CONTINUE) + F("'");
    if (act->midiMessage == PED_MIDI_CONTINUE) page += F(" selected");
    page += F(">Continue</option>");
    page += F("<option value='");
    page += String(PED_SEQUENCE) + F("'");
    if (act->midiMessage == PED_SEQUENCE) page += F(" selected");
    page += F(">Sequence</option>");
    page += F("<option value='");
    page += String(PED_ACTION_BANK_PLUS) + F("'");
    if (act->midiMessage == PED_ACTION_BANK_PLUS) page += F(" selected");
    page += F(">Bank+</option>");
    page += F("<option value='");
    page += String(PED_ACTION_BANK_MINUS) + F("'");
    if (act->midiMessage == PED_ACTION_BANK_MINUS) page += F(" selected");
    page += F(">Bank-</option>");
    page += F("<option value='");
    page += String(PED_ACTION_START) + F("'");
    if (act->midiMessage == PED_ACTION_START) page += F(" selected");
    page += F(">MTC Start</option>");
    page += F("<option value='");
    page += String(PED_ACTION_STOP) + F("'");
    if (act->midiMessage == PED_ACTION_STOP) page += F(" selected");
    page += F(">MTC Stop</option>");
    page += F("<option value='");
    page += String(PED_ACTION_CONTINUE) + F("'");
    if (act->midiMessage == PED_ACTION_CONTINUE) page += F(" selected");
    page += F(">MTC Continue</option>");
    page += F("<option value='");
    page += String(PED_ACTION_TAP) + F("'");
    if (act->midiMessage == PED_ACTION_TAP) page += F(" selected");
    page += F(">Tap</option>");
    page += F("<option value='");
    page += String(PED_ACTION_BPM_PLUS) + F("'");
    if (act->midiMessage == PED_ACTION_BPM_PLUS) page += F(" selected");
    page += F(">BPM+</option>");
    page += F("<option value='");
    page += String(PED_ACTION_BPM_MINUS) + F("'");
    if (act->midiMessage == PED_ACTION_BPM_MINUS) page += F(" selected");
    page += F(">BPM-</option>");
    page += F("</select>");
    page += F("<input type='number' class='form-control form-control-sm' name='code");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(act->midiCode);
    page += F("'>");
    page += F("<select class='custom-select custom-select-sm' name='channel");
    page += String(i) + F("'>");
    for (unsigned int c = 1; c <= 16; c++) {
      page += F("<option value='");
      page += String(c) + F("'");
      if (act->midiChannel == c) page += F(" selected");
      page += F(">Channel ");
      page += String(c) + F("</option>");
    }
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='input-group input-group-sm mb-1'>");
    page += F("<div class='input-group-prepend w-25'>");
    page += F("<div class='input-group-text w-100'>Led</div>");
    page += F("</div>");
    page += F("<input type='number' class='form-control form-control-sm' name='led");
    page += String(i);
    page += F("' min='1' max='");
    page += String(LEDS);
    page += F("' value='");
    page += String(act->led + 1);
    page += F("'>");
    page += F("<input type='color' class='form-control form-control-sm' name='color0-");
    page += String(i);
    page += F("' value='");
    char color[8];
    sprintf(color, "#%06X", act->color0 & 0xFFFFFF);
    page += String(color);
    page += F("'>");
    page += F("<input type='color' class='form-control form-control-sm' name='color1-");
    page += String(i);
    page += F("' value='");
    sprintf(color, "#%06X", act->color1 & 0xFFFFFF);
    page += String(color);
    page += F("'></div>");

    page += F("<div class='input-group input-group-sm mb-1'>");
    page += F("<div class='input-group-prepend w-25'>");
    page += F("<div class='input-group-text w-100'>Tags</div>");
    page += F("</div>");
    page += F("<input type='text' class='form-control form-control-sm' placeholder='Off' name='nameoff");
    page += String(i);
    page += F("' maxlength='");
    page += String(MAXACTIONNAME) + F("' value='");
    page += String(act->tag0);
    page += F("'>");
    page += F("<input type='text' class='form-control form-control-sm' placeholder='On' name='nameon");
    page += String(i);
    page += F("' maxlength='");
    page += String(MAXACTIONNAME) + F("' value='");
    page += String(act->tag1);
    page += F("'></div>");

    page += F("<div class='input-group input-group-sm mb-1'>");
    page += F("<div class='input-group-prepend w-25'>");
    page += F("<div class='input-group-text w-100'>From Off/To On</div>");
    page += F("</div>");
    page += F("<input type='number' class='form-control form-control-sm' name='from");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(act->midiValue1);
    page += F("'>");
    page += F("<input type='number' class='form-control form-control-sm' name='to");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(act->midiValue2);
    page += F("'></div>");
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='form-check'>");
    page += F("<input class='form-check-input position-static' type='checkbox' name='delete");
    page += String(i) + F("'>");
    page += F("</div>");
    page += F("</div>");

    page += F("<div class='w-100'></div>");
    page += F("</div>");

    same_pedal = (act->next != nullptr && act->pedal == act->next->pedal);
    if (!same_pedal) {
      page += F("</div>");
      page += F("</div>");
    }
    act = act->next;
    i++;
  }
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' name='action' value='apply' class='btn btn-primary btn-sm'>Apply</button>");
  page += F(" ");
  page += F("<button type='submit' name='action' value='save' class='btn btn-primary btn-sm'>Save</button>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  get_footer_page();
}

void get_pedals_page() {

  get_top_page(4);

  page += F("<form method='post'>");
  page += F("<div class='form-row'>");

  for (unsigned int i = 1; i <= PEDALS; i++) {
    page += F("<div class='col-sm-6 col-md-4 col-xl-2 col-12 mb-3'>");
    page += F("<div class='card'>");
    page += F("<h5 class='card-header'>Pedal ");
    page += String(i) + F("</h5>");
    page += F("<div class='card-body'>");
    page += F("<div class='input-group input-group-sm mb-2'>");
    page += F("<div class='input-group-prepend' style='width: 40%'>");
    page += F("<div class='input-group-text w-100'>Mode</div>");
    page += F("</div>");
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

    page += F("<div class='input-group input-group-sm mb-2'>");
    page += F("<div class='input-group-prepend' style='width: 40%'>");
    page += F("<div class='input-group-text w-100'>Function</div>");
    page += F("</div>");
    page += F("<select class='custom-select custom-select-sm' name='function");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_NONE) + F("'");
    if (pedals[i-1].function == PED_NONE) page += F(" selected");
    page += F("></option>");
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
    page += F(">MTC Start</option>");
    page += F("<option value='");
    page += String(PED_STOP) + F("'");
    if (pedals[i-1].function == PED_STOP) page += F(" selected");
    page += F(">MTC Stop</option>");
    page += F("<option value='");
    page += String(PED_CONTINUE) + F("'");
    if (pedals[i-1].function == PED_CONTINUE) page += F(" selected");
    page += F(">MTC Continue</option>");
    page += F("<option value='");
    page += String(PED_TAP) + F("'");
    if (pedals[i-1].function == PED_TAP) page += F(" selected");
    page += F(">Tap</option>");
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

    page += F("<div class='input-group input-group-sm mb-2'>");
    page += F("<div class='input-group-prepend' style='width: 40%'>");
    page += F("<div class='input-group-text w-100'>Analog</div>");
    page += F("</div>");
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

    page += F("<div class='input-group input-group-sm mb-2'>");
    page += F("<div class='input-group-prepend' style='width: 40%'>");
    page += F("<div class='input-group-text w-100'>Min</div>");
    page += F("</div>");
    page += F("<input type='number' class='form-control form-control-sm' name='min");
    page += String(i);
    page += F("' min='0' max='");
    page += String(ADC_RESOLUTION - 1) + F("' value='");
    page += String(pedals[i-1].expZero);
    page += F("'></div>");

    page += F("<div class='input-group input-group-sm mb-2'>");
    page += F("<div class='input-group-prepend' style='width: 40%'>");
    page += F("<div class='input-group-text w-100'>Max</div>");
    page += F("</div>");
    page += F("<input type='number' class='form-control form-control-sm' name='max");
    page += String(i);
    page += F("' min='0' max='");
    page += String(ADC_RESOLUTION - 1) + F("' value='");
    page += String(pedals[i-1].expMax);
    page += F("'></div>");

    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='polarityCheck");
    page += String(i) + F("' name='polarity") + String(i) + F("'");
    if (pedals[i-1].invertPolarity) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='polarityCheck");
    page += String(i) + F("'>Invert Polarity</label>");
    page += F("</div>");

    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='singleCheck");
    page += String(i) + F("' name='singlepress") + String(i) + F("'");
    if (pedals[i-1].pressMode == PED_PRESS_1   ||
        pedals[i-1].pressMode == PED_PRESS_1_2 ||
        pedals[i-1].pressMode == PED_PRESS_1_L ||
        pedals[i-1].pressMode == PED_PRESS_1_2_L) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='singleCheck");
    page += String(i) + F("'>Single Press</label>");
    page += F("</div>");

    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='doubleCheck");
    page += String(i) + F("' name='doublepress") + String(i) + F("'");
    if (pedals[i-1].pressMode == PED_PRESS_2   ||
        pedals[i-1].pressMode == PED_PRESS_1_2 ||
        pedals[i-1].pressMode == PED_PRESS_2_L ||
        pedals[i-1].pressMode == PED_PRESS_1_2_L) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='doubleCheck");
    page += String(i) + F("'>Double Press</label>");
    page += F("</div>");

    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='longCheck");
    page += String(i) + F("' name='longpress") + String(i) + F("'");
    if (pedals[i-1].pressMode == PED_PRESS_L   ||
        pedals[i-1].pressMode == PED_PRESS_1_L ||
        pedals[i-1].pressMode == PED_PRESS_2_L ||
        pedals[i-1].pressMode == PED_PRESS_1_2_L) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='longCheck");
    page += String(i) + F("'>Long Press</label>");
    page += F("</div>");

    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='autoCheck");
    page += String(i) + F("' name='autosensing") + String(i) + F("'");
    if (pedals[i-1].autoSensing) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='autoCheck");
    page += String(i) + F("'>Analog Calibration</label>");
    page += F("</div>");

    page += F("</div>");
    page += F("</div>");
    page += F("</div>");

  }
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' name='action' value='apply' class='btn btn-primary btn-sm'>Apply</button>");
  page += F(" ");
  page += F("<button type='submit' name='action' value='save' class='btn btn-primary btn-sm'>Save</button>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  get_footer_page();
}

void get_interfaces_page() {

  get_top_page(5);

  page += F("<form method='post'>");

  page += F("<div class='row'>");
  for (unsigned int i = 1; i <= INTERFACES; i++) {
    page += F("<div class='col-sm-6 col-md-4 col-lg-2 col-12 mb-3'>");
    page += F("<div class='card h-100'>");
    page += F("<h6 class='card-header'>");
    page += interfaces[i-1].name;
    page += F("</h6>");
    page += F("<div class='card-body'>");
    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='inCheck");
    page += String(i) + F("' name='in") + String(i) + F("'");
    if (interfaces[i-1].midiIn) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='inCheck");
    page += String(i) + F("'>In</label>");
    page += F("</div>");
    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='outCheck");
    page += String(i) + F("' name='out") + String(i) + F("'");
    if (interfaces[i-1].midiOut) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='outCheck");
    page += String(i) + F("'>Out</label>");
    page += F("</div>");
    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='thruCheck");
    page += String(i) + F("' name='thru") + String(i) + F("'");
    if (interfaces[i-1].midiThru) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='thruCheck");
    page += String(i) + F("'>Thru</label>");
    page += F("</div>");
    page += F("<div class='custom-control custom-switch'>");
    page += F("<input type='checkbox' class='custom-control-input' id='clockCheck");
    page += String(i) + F("' name='clock") + String(i) + F("'");
    if (interfaces[i-1].midiClock) page += F(" checked");
    page += F(">");
    page += F("<label class='custom-control-label' for='clockCheck");
    page += String(i) + F("'>Clock</label>");
    page += F("</div>");
    page += F("</div>");
    page += F("</div>");
    page += F("</div>");
  }
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' name='action' value='apply' class='btn btn-primary btn-sm'>Apply</button>");
  page += F(" ");
  page += F("<button type='submit' name='action' value='save' class='btn btn-primary btn-sm'>Save</button>");
  page += F("</div>");
  page += F("</div>");

  page += F("</form>");

  get_footer_page();
}

void get_sequences_page() {

  const byte s = constrain(uisequence.toInt(), 0, SEQUENCES);

  get_top_page(6);

  page += F("<div class='btn-group mb-3'>");
  for (unsigned int i = 1; i <= SEQUENCES; i++) {
    page += F("<form method='get'><button type='button submit' class='btn");
    page += (uisequence == String(i) ? String(" btn-primary") : String(""));
    page += F("' name='sequence' value='");
    page += String(i) + F("'>") + String(i) + F("</button></form>");
  }
  page += F("</div>");

  page += F("<form method='post'>");
  page += F("<div class='form-row mb-3'>");
  page += F("<div class='col-1 text-center'>");
  page += F("<span class='badge badge-primary'>Order</span>");
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
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>MIDI Value 1</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>MIDI Value 2</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>MIDI Value 3</span>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row mb-3'>");
  for (unsigned int i = 1; i <= STEPS; i++) {
    page += F("<div class='col-1 mb-3 text-center'>");
    page += String(i);
    page += F("</div>");

    page += F("<div class='col-2'>");
    page += F("<select class='custom-select custom-select-sm' name='message");
    page += String(i);
    page += F("'>");
    page += F("<option value='");
    page += String(PED_EMPTY) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_NONE) page += F(" selected");
    page += F("></option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_PROGRAM_CHANGE) page += F(" selected");
    page += F(">Program Change</option>");
    page += F("<option value='");
    page += String(PED_CONTROL_CHANGE) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_CONTROL_CHANGE) page += F(" selected");
    page += F(">Control Change</option>");
    page += F("<option value='");
    page += String(PED_NOTE_ON) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_NOTE_ON) page += F(" selected");
    page += F(">Note On</option>");
    page += F("<option value='");
    page += String(PED_NOTE_OFF) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_NOTE_OFF) page += F(" selected");
    page += F(">Note Off</option>");
    page += F("<option value='");
    page += String(PED_BANK_SELECT_INC) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_BANK_SELECT_INC) page += F(" selected");
    page += F(">Bank Select+</option>");
    page += F("<option value='");
    page += String(PED_BANK_SELECT_DEC) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_BANK_SELECT_DEC) page += F(" selected");
    page += F(">Bank Select-</option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE_INC) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_PROGRAM_CHANGE_INC) page += F(" selected");
    page += F(">Program Change+</option>");
    page += F("<option value='");
    page += String(PED_PROGRAM_CHANGE_DEC) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_PROGRAM_CHANGE_DEC) page += F(" selected");
    page += F(">Program Change-</option>");
    page += F("<option value='");
    page += String(PED_PITCH_BEND) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_PITCH_BEND) page += F(" selected");
    page += F(">Pitch Bend</option>");
    page += F("<option value='");
    page += String(PED_CHANNEL_PRESSURE) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_CHANNEL_PRESSURE) page += F(" selected");
    page += F(">Channel Pressure</option>");
    page += F("<option value='");
    page += String(PED_MIDI_START) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_MIDI_START) page += F(" selected");
    page += F(">Start</option>");
    page += F("<option value='");
    page += String(PED_MIDI_STOP) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_MIDI_STOP) page += F(" selected");
    page += F(">Stop</option>");
    page += F("<option value='");
    page += String(PED_MIDI_CONTINUE) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_MIDI_CONTINUE) page += F(" selected");
    page += F(">Continue</option>");
    page += F("<option value='");
    page += String(PED_SEQUENCE) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_SEQUENCE) page += F(" selected");
    page += F(">Sequence</option>");
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-1'>");
    page += F("<select class='custom-select custom-select-sm' name='channel");
    page += String(i) + F("'>");
    for (unsigned int c = 1; c <= 16; c++) {
      page += F("<option value='");
      page += String(c) + F("'");
      if (sequences[s-1][i-1].midiChannel == c) page += F(" selected");
      page += F(">");
      page += String(c) + F("</option>");
    }
    page += F("</select>");
    page += F("</div>");

    page += F("<div class='col-2'>");
    page += F("<input type='number' class='form-control form-control-sm' name='code");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(sequences[s-1][i-1].midiCode);
    page += F("'></div>");

    page += F("<div class='col-1'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value1");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(sequences[s-1][i-1].midiValue1);
    page += F("'></div>");

    page += F("<div class='col-1'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value2");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(sequences[s-1][i-1].midiValue2);
    page += F("'></div>");

    page += F("<div class='col-1'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value3");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(sequences[s-1][i-1].midiValue3);
    page += F("'></div>");

    page += F("<div class='w-100'></div>");
  }
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' name='action' value='apply' class='btn btn-primary btn-sm'>Apply</button>");
  page += F(" ");
  page += F("<button type='submit' name='action' value='save' class='btn btn-primary btn-sm'>Save</button>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  get_footer_page();
}

void get_options_page() {

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

  get_top_page(7);

  page += F("<form method='post'>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-md-6 col-12 mb-3'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Device</h5>");
  page += F("<div class='card-body'>");
  page += F("<h5 class='card-title'>Name</h5>");
  page += F("<input class='form-control' type='text' maxlength='32' id='devicename' name='mdnsdevicename' placeholder='' value='");
  page += host + F("'>");
  page += F("<small id='devicenameHelpBlock' class='form-text text-muted'>");
  page += F("Each device must have a different name. Enter the device name without .local. Web UI will be available at http://<i>device_name</i>.local<br>");
  page += F("Pedalino will be restarted if you change it.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='col-md-6 col-12 mb-3'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Boot Mode</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='bootModeWifi' name='bootmodewifi'");
  if (bootMode == PED_BOOT_NORMAL ||
      bootMode == PED_BOOT_WIFI   ||
      bootMode == PED_BOOT_AP     ||
      bootMode == PED_BOOT_AP_NO_BLE) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='bootModeWifi'>WiFi</label>");
  page += F("<small id='bootModeWifiHelpBlock' class='form-text text-muted'>");
  page += F("RTP-MIDI, ipMIDI, OSC and web UI require WiFi.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='bootModeAP' name='bootmodeap'");
  if (bootMode == PED_BOOT_AP ||
      bootMode == PED_BOOT_AP_NO_BLE) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='bootModeAP'>Access Point</label>");
  page += F("<small id='bootModeAPHelpBlock' class='form-text text-muted'>");
  page += F("To enable AP Mode enable WiFi too.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='bootModeBLE' name='bootmodeble'");
  if (bootMode == PED_BOOT_NORMAL ||
      bootMode == PED_BOOT_BLE    ||
      bootMode == PED_BOOT_AP) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='bootModeBLE'>BLE</label>");
  page += F("<small id='bootModeBLEHelpBlock' class='form-text text-muted'>");
  page += F("BLE MIDI requires BLE.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-md-6 col-12 mb-3'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>WiFi Network</h5>");
  page += F("<div class='card-body'>");
  page += F("<label for='wifissid'>SSID</label>");
  //page += F("<input class='form-control' type='text' maxlength='32' id='wifissid' name='wifiSSID' placeholder='SSID' value='");
  //page += wifiSSID + F("'>");
  page += F("<select class='custom-select' id='wifissid' name='wifiSSID'>");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    page += F("<option value='");
    page += WiFi.SSID(i) + F("'");
    if (wifiSSID == WiFi.SSID(i)) page += F(" selected");
    page += F(">");
    page += WiFi.SSID(i) + F("</option>");
  }
  page += F("</select>");
  page += F("<label for='wifipassword'>Password</label>");
  page += F("<input class='form-control' type='password' maxlength='32' id='wifipassword' name='wifiPassword' placeholder='password' value='");
  page += wifiPassword + F("'>");
  page += F("<small class='form-text text-muted'>");
  page += F("Connect to a wifi network using SSID and password.<br>");
  page += F("Pedalino will be restarted if it is connected to a WiFi network and you change them.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='col-md-6 col-12 mb-3'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>AP Mode</h5>");
  page += F("<div class='card-body'>");
  page += F("<label for='wifissid'>SSID</label>");
  page += F("<input class='form-control' type='text' maxlength='32' id='ssidsoftap' name='ssidSoftAP' placeholder='SSID' value='");
  page += ssidSoftAP + F("'>");
  page += F("<label for='passwordsoftap'>Password</label>");
  page += F("<input class='form-control' type='password' maxlength='32' id='passwordsoftap' name='passwordSoftAP' placeholder='password' value='");
  page += passwordSoftAP + F("'>");
  page += F("<small class='form-text text-muted'>");
  page += F("Access Point SSID and password.<br>");
  page += F("Pedalino will be restarted if it is in AP mode and you changed them.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-md-6 col-12 mb-3'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Web UI Theme</h5>");
  page += F("<div class='card-body'>");
  page += F("<select class='custom-select' id='bootstraptheme' name='theme'>");
  for (unsigned int i = 0; i < 22; i++) {
    page += F("<option value='");
    page += bootswatch[i] + F("'");
    if (theme == bootswatch[i]) page += F(" selected");
    page += F(">");
    page += bootswatch[i] + F("</option>");
  }
  page += F("</select>");
  page += F("<small id='bootstrapthemeHelpBlock' class='form-text text-muted'>");
  page += F("Changing default theme require internet connection because themes are served via a CDN network. Only 'bootstrap' theme has been stored into Pedalino flash memory.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='col-md-6 col-12 mb-3'>");
  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Web UI Login</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='input-group input-group-sm mb-3'>");
  page += F("<div class='input-group-prepend w-25'>");
  page += F("<div class='input-group-text w-100'>Username</div>");
  page += F("</div>");
  page += F("<input class='form-control form-control-sm' type='text' maxlength='32' id='httpusername' name='httpUsername' value='");
  page += httpUsername + F("'>");
  page += F("</div>");
  page += F("<div class='input-group input-group-sm mb-3'>");
  page += F("<div class='input-group-prepend w-25'>");
  page += F("<div class='input-group-text w-100'>Password</div>");
  page += F("</div>");
  page += F("<input class='form-control form-control-sm' type='password' maxlength='32' id='httppassword' name='httpPassword' value='");
  page += httpPassword + F("'>");
  page += F("</div>");
  page += F("<small class='form-text text-muted'>");
  page += F("Web UI administrator username and password. Leave username blank for no login request.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='card mb-3'>");
  page += F("<h5 class='card-header'>Momentary Switches</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='form-row'>");
  page += F("<div class='form-group col-md-6 col-12 mb-3'>");
  page += F("<label for='pressTime'>Press Time</label>");
  page += F("<input class='form-control' type='text' maxlength='32' id='pressTime' name='presstime' placeholder='' value='");
  page += String(pressTime) + F("'>");
  page += F("<small id='pressTimeModeHelpBlock' class='form-text text-muted'>");
  page += F("Switch press time in milliseconds. Default value is 200.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='form-group col-md-6 col-12 mb-3'>");
  page += F("<label for='doublePressTime'>Double Press Time</label>");
  page += F("<input class='form-control' type='text' maxlength='32' id='doublePressTime' name='doublepresstime' placeholder='' value='");
  page += String(doublePressTime) + F("'>");
  page += F("<small id='doublePressTimeModeHelpBlock' class='form-text text-muted'>");
  page += F("Set double press detection time between each press time in milliseconds. Default value is 400.<br>");
  page += F("A double press is detected if the switch is released and depressed within this time, measured from when the first press is detected.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='form-row'>");
  page += F("<div class='form-group col-md-6 col-12 mb-3'>");
  page += F("<label for='longPressTime'>Long Press Time</label>");
  page += F("<input class='form-control' type='text' maxlength='32' id='longPressTime' name='longpresstime' placeholder='' value='");
  page += String(longPressTime) + F("'>");
  page += F("<small id='longPressTimeModeHelpBlock' class='form-text text-muted'>");
  page += F("Set the long press time in milliseconds after which a continuous press and release is deemed a long press, measured from when the first press is detected. Default value is 500.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='form-group col-md-6 col-12 mb-3'>");
  page += F("<label for='repeatPressTime'>Repeat Press Time</label>");
  page += F("<input class='form-control' type='text' maxlength='32' id='repeatPressTime' name='repeatpresstime' placeholder='' value='");
  page += String(repeatPressTime) + F("'>");
  page += F("<small id='repeatPressTimeModeHelpBlock' class='form-text text-muted'>");
  page += F("Set the repeat time in milliseconds after which a continuous press and hold is treated as a stream of repeated presses, measured from when the first press is detected. Default value is 500.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row'>");

  page += F("<div class='col-md-6 col-12 mb-3'>");

  page += F("<div class='card h-100'>");
  page += F("<h5 class='card-header'>Resistor Ladder Network</h5>");
  page += F("<div class='card-body'>");
  for (byte i = 1; i <= LADDER_STEPS + 1; i++) {
    page += F("<div class='input-group input-group-sm mb-3'>");
    page += F("<div class='input-group-prepend'>");
    page += F("<div class='input-group-text'>Level ");
    page += String(i);
    page += F("</div>");
    page += F("</div>");
    page += F("<input class='form-control form-control-sm' type='number' id='threshold");
    page += String(i) + F("' name='threshold");
    page += String(i) + F("' min='0' max='");
    page += String(ADC_RESOLUTION-1) + F("' value='");
    page += String(ladderLevels[i-1]) + F("'>");
    page += F("</div>");;
  }
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='col-md-6 col-12 mb-3'>");

  page += F("<div class='card h-40 mb-3'>");
  page += F("<h5 class='card-header'>Encoders</h5>");
  page += F("<div class='card-body'>");
  page += F("<label for='encodersensitivity'>Encoder Sensitivity</label>");
  page += F("<select class='custom-select custom-select-sm' name='encodersensitivity'>");
  for (unsigned int s = 1; s <= 10; s++) {
    page += F("<option value='");
    page += String(s) + F("'");
    if (encoderSensitivity == s) page += F(" selected");
    page += F(">");
    page += String(s) + F("</option>");
  }
  page += F("</select>");
  page += F("<small id='encoderSensitivityHelpBlock' class='form-text text-muted'>");
  page += F("Default value is 5.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='card h-60'>");
  page += F("<h5 class='card-header'>Additional Features</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='tapDanceMode' name='tapdancemode'");
  if (tapDanceMode) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='tapDanceMode'>Tap Dance Mode</label>");
  page += F("<small id='tapDanceModeHelpBlock' class='form-text text-muted'>");
  page += F("The first press of pedal X switch to bank X, the second press of any pedal send the MIDI event.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch mb-4'>");
  page += F("<input type='checkbox' class='custom-control-input' id='repeatOnBankSwitch' name='repeatonbankswitch'");
  if (repeatOnBankSwitch) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='repeatOnBankSwitch'>Bank Switch Repeat</label>");
  page += F("<small id='repeatOnBankSwitchModeHelpBlock' class='form-text text-muted'>");
  page += F("On bank switch repeat the last MIDI message that was sent for that bank");
  page += F("</small>");
  page += F("</div>");
  page += F("<label for='brightnessOn'>Leds On Brightness</label>");
  page += F("<input type='range' class='custom-range' min='0' max='255' id='brightnessOn' name='ledsonbrightness' value='");
  page += String(ledsOnBrightness);
  page += F("'>");
  page += F("<label for='brightnessOff'>Leds Off Brightness</label>");
  page += F("<input type='range' class='custom-range' min='0' max='255' id='brightnessOff' name='ledsoffbrightness' value='");
  page += String(ledsOffBrightness);
  page += F("'>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

#ifdef BLYNK
  page += F("<div class='form-row mb-3'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='blynkCloud' name='blynkcloud'");
  if (blynk_enabled()) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='blynkCloud'>Blynk Cloud</label>");
  page += F("<small id='blynkCloudModeHelpBlock' class='form-text text-muted'>");
  page += F("If Blynk Cloud connection is disabled the app cannot connect to Pedalino.");
  page += F("</small>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row mb-3'>");
  page += F("<label for='authtoken'>Blynk Auth Token</label>");
  page += F("<input class='form-control' type='text' maxlength='32' id='authtoken' name='blynkauthtoken' placeholder='Blynk Auth Token is 32 characters long. Copy and paste from email.' value='");
  page += blynk_get_token() + F("'>");
  page += F("<small id='blynkCloudModeHelpBlock' class='form-text text-muted'>");
  page += F("Auth Token is a unique identifier which is needed to connect your Pedalino to your smartphone. Every Pedalino will have its own Auth Token. You’ll get Auth Token automatically on your email after Pedalino app clone. You can also copy it manually. Click on devices section and selected required device.<br>");
  page += F("Don’t share your Auth Token with anyone, unless you want someone to have access to your Pedalino.");
  page += F("</small>");
  page += F("</div>");
#endif

  page += F("<div class='form-row justify-content-between'>");
  page += F("<div class='col-4'>");
  page += F("<button type='submit' name='action' value='apply' class='btn btn-primary btn-sm'>Apply</button>");
  page += F(" ");
  page += F("<button type='submit' name='action' value='save' class='btn btn-primary btn-sm'>Save</button>");
  page += F("</div>");
  page += F("<div class='col-8 text-right'>");
  page += F("<button type='submit' name='action' value='factorydefault' class='btn btn-primary btn-sm'>Reset to Factory Default</button>");
  page += F(" ");
  page += F("<button type='submit' name='action' value='reboot' class='btn btn-primary btn-sm'>Reboot</button>");
  page += F("</div>");
  page += F("</div>");

  page += F("</form>");

  get_footer_page();
}


void get_configurations_page() {

  get_top_page(8);

  page += F("<form method='post'>");

  page += F("<div class='card mb-3'>");
  page += F("<h5 class='card-header'>New Configuration</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='form-row'>");
  page += F("<div class='col-8'>");
  page += F("<input class='form-control' type='text' maxlength='26' id='newconfiguration' name='newconfiguration' placeholder='' value=''>");
  page += F("<small id='newconfigurationHelpBlock' class='form-text text-muted'>");
  page += F("Type a name, select what to include and press 'Save as configuration' to save current profile with a name. An existing configuration with the same name will be overridden without further notice.");
  page += F("</small>");
  page += F("<br><button type='submit' name='action' value='new' class='btn btn-primary btn-sm'>Save as configuration</button>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("</div>");
  page += F("<div class='col-3'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='actionsCheck1' name='actions1' checked>");
  page += F("<label class='custom-control-label' for='actionsCheck1'>Actions</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='pedalsCheck1' name='pedals1' checked>");
  page += F("<label class='custom-control-label' for='pedalsCheck1'>Pedals</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='interfacesCheck1' name='interfaces1' checked>");
  page += F("<label class='custom-control-label' for='interfacesCheck1'>Interfaces</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='sequencesCheck1' name='sequences1' checked>");
  page += F("<label class='custom-control-label' for='sequencesCheck1'>Sequences</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='optionsCheck1' name='options1' checked>");
  page += F("<label class='custom-control-label' for='optionsCheck1'>Options</label>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("</form>");

  DPRINT("Looking for configuration files on SPIFFS root ...\n");
  int     availableconf = 0;
  String  confoptions;
  File    root = SPIFFS.open("/");
  File    file = root.openNextFile();
  while (file) {
    String c(file.name());

    if (c.length() > 4 && c.lastIndexOf(".cfg") == (c.length() - 4)) {
      availableconf++;
      DPRINT("%s\n", c.c_str());
      confoptions += F("<option value='");
      confoptions += c + F("'>");
      confoptions += c.substring(1, c.length() - 4) + F("</option>");
    }
    file = root.openNextFile();
  }
  DPRINT("done.\n");

  page += F("<form method='post'>");
  page += F("<div class='card mb-3'>");
  page += F("<h5 class='card-header'>Available Configurations</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='form-row'>");
  page += F("<div class='col-8'>");
  page += F("<select class='custom-select' id='filename' name='filename'>");
  page += confoptions;
  page += F("</select>");
  page += F("<small id='filenameHelpBlock' class='form-text text-muted'>");
  page += F("'Apply' to load configuration into current profile.<br>");
  page += F("'Apply & Save' to load configuration into current profile and save the profile.<br>");
  page += F("'Download' to download configuration to local computer.<br>");
  page += F("'Delete' to remove configuration.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("</div>");
  page += F("<div class='col-3'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='actionsCheck2' name='actions2' checked>");
  page += F("<label class='custom-control-label' for='actionsCheck2'>Actions</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='pedalsCheck2' name='pedals2' checked>");
  page += F("<label class='custom-control-label' for='pedalsCheck2'>Pedals</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='interfacesCheck2' name='interfaces2' checked>");
  page += F("<label class='custom-control-label' for='interfacesCheck2'>Interfaces</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='sequencesCheck2' name='sequences2' checked>");
  page += F("<label class='custom-control-label' for='sequencesCheck2'>Sequences</label>");
  page += F("</div>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='optionsCheck2' name='options2' checked>");
  page += F("<label class='custom-control-label' for='optionsCheck2'>Options</label>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-12'>");
  page += F("<button type='submit' name='action' value='apply' class='btn btn-primary btn-sm'>Apply</button> ");
  page += F("<button type='submit' name='action' value='save' class='btn btn-primary btn-sm'>Apply & Save</button> ");
  page += F("<button type='submit' name='action' value='download' class='btn btn-primary btn-sm'>Download</button> ");
  page += F("<button type='submit' name='action' value='delete' class='btn btn-danger btn-sm'>Delete</button> ");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</form>");

  page += F("<form method='post' action='/configurations' enctype='multipart/form-data'>");
  page += F("<div class='card'>");
  page += F("<h5 class='card-header'>Upload Configuration</h5>");
  page += F("<div class='card-body'>");
  page += F("<div class='form-row'>");
  page += F("<div class='col-8'>");
  page += F("<div class='custom-file'>");
  page += F("<input type='file' class='custom-file-input' id='customFile' name='upload'>");
  page += F("<label class='custom-file-label text-truncate' for='customFile'>Choose file</label>");
  page += F("</div>");
  page += F("<small id='uploadHelpBlock' class='form-text text-muted'>");
  page += F("SPIFFS is not a high performance FS. It is designed to balance safety, wear levelling and performance for bare flash devices. ");
  page += F("If you want good performance from SPIFFS keep the % utilisation low.");
  page += F("</small>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("</div>");
  page += F("<div class='col-3'>");
  page += F("<input type='submit' value='Upload' class='btn btn-primary btn-sm'>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  // Add the following code if you want the name of the file appear on select
  //page += F("<script>$('.custom-file-input').on('change', function() { let fileName = $(this).val().split('\\').pop(); $(this).next('.custom-file-label').addClass('selected').html(fileName); });</script>");
  page += F("</form>");

  get_footer_page();
}


size_t get_root_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_root_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_live_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_live_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_actions_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_actions_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_pedals_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_pedals_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_interfaces_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_interfaces_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_sequences_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_sequences_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_options_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_options_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

size_t get_configurations_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_configurations_page();
    DPRINT("HTML page lenght: %d\n", page.length());
    rebuild = false;
  }
  page.getBytes(buffer, maxLen, index);
  buffer[maxLen-1] = 0; // CWE-126
  size_t byteWritten = strlen((const char *)buffer);
  rebuild = (byteWritten == 0);
  if (rebuild) page = "";
  return byteWritten;
}

void http_handle_login(AsyncWebServerRequest *request) {
  if (httpUsername.isEmpty()) {
    authenticated = true;
    request->redirect("/");
  } else {
    authenticated = false;
    get_login_page();
    request->send(200, "text/html", page);
  }
}

void http_handle_post_login(AsyncWebServerRequest *request) {
  if (request->hasArg("username")) {
    if (request->arg("username") == httpUsername)
      if (request->hasArg("password"))
        if (request->arg("password") == httpPassword) {
          authenticated = true;
          return request->redirect("/");
        }
  }
  get_login_page();
  request->send(200, "text/html", page);
}

void http_handle_globals(AsyncWebServerRequest *request) {

  if (request->hasArg("profile")) {
    uiprofile = request->arg("profile");
    currentProfile = constrain(uiprofile.toInt() - 1, 0, PROFILES - 1);
    reloadProfile = true;
    eeprom_read_profile(currentProfile);
  }

  if (request->hasArg("theme") ) {
    if(request->arg("theme") != theme) {
      theme = request->arg("theme");
    }
  }
}

void http_handle_root(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_root_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_live(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_live_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_actions(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  if (request->hasArg("bank"))  uibank   = request->arg("bank");
  if (request->hasArg("pedal")) uipedal  = request->arg("pedal");
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_actions_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_pedals(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_pedals_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_interfaces(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_interfaces_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_sequences(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  if (request->hasArg("sequence"))  uisequence  = request->arg("sequence");
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_sequences_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_options(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_options_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_configurations(AsyncWebServerRequest *request) {
  if (!httpUsername.isEmpty() && !request->authenticate(httpUsername.c_str(), httpPassword.c_str())) return request->requestAuthentication();
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_configurations_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_post_live(AsyncWebServerRequest *request) {

  String a;

  a = request->arg("profile");
  currentProfile = a.toInt();

  blynk_refresh();
  alert = "Saved";

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_live_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_post_actions(AsyncWebServerRequest *request) {

  String     a;
  const byte b = constrain(uibank.toInt() - 1, 0, BANKS - 1);
  const byte p = constrain(uipedal.toInt() - 1, 0, PEDALS - 1);

  strncpy(banknames[b], request->arg(String("bankname")).c_str(), MAXBANKNAME+1);

  if (request->arg("action").equals("new")) {
    action *act = actions[b];
    if (act == nullptr) {
       act = actions[b] = (action*)malloc(sizeof(action));
       assert(act != nullptr);
    }
    else {
      while (act->next != nullptr) act = act->next;
      act->next = (action*)malloc(sizeof(action));
      assert(act->next != nullptr);
      act = act->next;
    }
    act->tag0[0]      = 0;
    act->tag1[0]      = 0;
    act->pedal        = p;
    act->button       = 0;
    act->led          = LEDS - 1;
    act->color0       = CRGB::Black;;
    act->color1       = CRGB::Black;
    act->event        = PED_EVENT_PRESS;
    act->midiMessage  = PED_EMPTY;
    act->midiChannel  = 1;
    act->midiCode     = 0;
    act->midiValue1   = 0;
    act->midiValue2   = 127;
    act->next         = nullptr;
    sort_actions();
    create_banks();
    alert = "";
  }
  else if (request->arg("action").startsWith("new")) {
    action *act = actions[b];
    if (act == nullptr)
       act = actions[b] = (action*)malloc(sizeof(action));
    else {
      while (act->next != nullptr) act = act->next;
      act->next = (action*)malloc(sizeof(action));
      assert(act->next != nullptr);
      act = act->next;
    }
    act->tag0[0]      = 0;
    act->tag1[0]      = 0;
    act->pedal        = constrain(request->arg("action").charAt(3) - '1', 0, PEDALS - 1);
    act->button       = 0;
    act->led          = LEDS - 1;
    act->color0       = CRGB::Black;;
    act->color1       = CRGB::Black;
    act->event        = PED_EVENT_PRESS;
    act->midiMessage  = PED_EMPTY;
    act->midiChannel  = 1;
    act->midiCode     = 0;
    act->midiValue1   = 0;
    act->midiValue2   = 127;
    act->next         = nullptr;
    sort_actions();
    create_banks();
    alert = "";
  }
  else if (request->arg("action").equals("delete")) {
    const String checked("on");
    unsigned int i       = 0;
    action      *act     = actions[b];
    action      *actPrev = nullptr;
    action      *actNext = (act == nullptr) ? nullptr : act->next;

    while (act != nullptr) {
      if (act->pedal == p || uipedal.equals("All")) {
        i++;
        if (request->arg(String("delete") + String(i)) == checked) {
          if (actPrev == nullptr) {         // first
            actions[b] = actNext;
            actNext = (actions[b] == nullptr) ? nullptr : actions[b]->next;
            free(act);
            act = actions[b];
          }
          else if (actNext == nullptr) {    // last
            actPrev->next = nullptr;
            free(act);
            act = nullptr;
          }
          else {                            // in the middle
            actPrev->next = actNext;
            free(act);
            act = actNext;
            actNext = (act == nullptr) ? nullptr : act->next;
          }
        }
        else {                              // next action
          actPrev = act;
          act = act->next;
          actNext = (act == nullptr) ? nullptr : act->next;
        }
      }
      else {
        actPrev = act;
        act = act->next;
        actNext = (act == nullptr) ? nullptr : act->next;
      }
    }
    create_banks();
    alert = F("Selected action(s) deleted.");
  }
  else if (request->arg("action").equals("apply") || request->arg("action").equals("save")) {
    unsigned int i      = 0;
    action      *act    = actions[b];
    while (act != nullptr) {
      if (act->pedal == p || uipedal.equals("All")) {
        i++;
        strncpy(act->tag0,            request->arg(String("nameoff")     + String(i)).c_str(),    MAXACTIONNAME+1);
        strncpy(act->tag1,            request->arg(String("nameon")     + String(i)).c_str(),    MAXACTIONNAME+1);
        act->button       = constrain(request->arg(String("button")   + String(i)).toInt() - 1, 0, LADDER_STEPS - 1);
        act->led          = constrain(request->arg(String("led")      + String(i)).toInt() - 1, 0, LEDS - 1);
        byte r, g, b;
        sscanf(                       request->arg(String("color0-")  + String(i)).c_str(), "#%02x%02x%02x", &r, &g, &b);
        act->color0       = (r << 16) + (g << 8) + b;
        sscanf(                       request->arg(String("color1-")  + String(i)).c_str(), "#%02x%02x%02x", &r, &g, &b);
        act->color1       = (r << 16) + (g << 8) + b;
        act->event        = constrain(request->arg(String("event")    + String(i)).toInt(), 0, 255);
        act->midiMessage  = constrain(request->arg(String("message")  + String(i)).toInt(), 0, 255);
        act->midiCode     = constrain(request->arg(String("code")     + String(i)).toInt(), 0, MIDI_RESOLUTION - 1);
        act->midiValue1   = constrain(request->arg(String("from")     + String(i)).toInt(), 0, MIDI_RESOLUTION - 1);
        act->midiValue2   = constrain(request->arg(String("to")       + String(i)).toInt(), 0, MIDI_RESOLUTION - 1);
        act->midiChannel  = constrain(request->arg(String("channel")  + String(i)).toInt(), 0, MIDI_RESOLUTION - 1);
      }
      act = act->next;
    }
    sort_actions();
    create_banks();
    leds_refresh();
    alert = F("Changes applied. Changes will be lost on next reboot or on profile switch if not saved.");
    if (request->arg("action").equals("save")) {
      eeprom_update_profile();
      eeprom_update_current_profile(currentProfile);
      alert = "Changes saved.";
    }
  }
  blynk_refresh();

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_actions_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
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
    pedals[i].expZero = constrain(a.toInt(), 0, ADC_RESOLUTION - 1);

    a = request->arg(String("max") + String(i+1));
    pedals[i].expMax = constrain(a.toInt(), 0, ADC_RESOLUTION - 1);

    switch (pedals[i].function) {
      case PED_BANK_PLUS:
      case PED_BANK_MINUS:
      case PED_BANK_PLUS_2:
      case PED_BANK_MINUS_2:
      case PED_BANK_PLUS_3:
      case PED_BANK_MINUS_3:
        pedals[i].expZero = constrain(pedals[i].expZero, 1, BANKS);
        pedals[i].expMax  = constrain(pedals[i].expMax,  1, BANKS);
      break;
    }
    if (pedals[i].expMax < pedals[i].expZero) {
      int t;
      t = pedals[i].expMax;
      pedals[i].expMax = pedals[i].expZero;
      pedals[i].expZero = t;
    }

  }
  if (request->arg("action") == String("apply")) {
    loadConfig = true;
    alert = F("Changes applied. Changes will be lost on next reboot or on profile switch if not saved.");
  }
  else if (request->arg("action") == String("save")) {
    eeprom_update_profile();
    eeprom_update_current_profile(currentProfile);
    loadConfig = true;
    alert = "Changes saved.";
  }
  blynk_refresh();

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_pedals_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
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
  if (request->arg("action") == String("apply")) {
    alert = F("Changes applied. Changes will be lost on next reboot or on profile switch if not saved.");
  }
  else if (request->arg("action") == String("save")) {
    eeprom_update_profile();
    eeprom_update_current_profile(currentProfile);
    alert = "Changes saved.";
  }
  blynk_refresh();

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_interfaces_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_post_sequences(AsyncWebServerRequest *request) {

  String     a;
  const byte s = constrain(uisequence.toInt() - 1, 0, SEQUENCES);

  for (unsigned int i = 0; i < STEPS; i++) {
    a = request->arg(String("channel") + String(i+1));
    sequences[s][i].midiChannel = a.toInt();

    a = request->arg(String("message") + String(i+1));
    sequences[s][i].midiMessage = a.toInt();

    a = request->arg(String("code") + String(i+1));
    sequences[s][i].midiCode = a.toInt();

    a = request->arg(String("value1") + String(i+1));
    sequences[s][i].midiValue1 = a.toInt();

    a = request->arg(String("value2") + String(i+1));
    sequences[s][i].midiValue2 = a.toInt();

    a = request->arg(String("value3") + String(i+1));
    sequences[s][i].midiValue3 = a.toInt();
  }
  if (request->arg("action") == String("apply")) {
    alert = F("Changes applied. Changes will be lost on next reboot or on profile switch if not saved.");
  }
  else if (request->arg("action") == String("save")) {
    eeprom_update_profile();
    eeprom_update_current_profile(currentProfile);
    alert = "Changes saved.";
  }
  blynk_refresh();

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_sequences_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_post_options(AsyncWebServerRequest *request) {

  const String checked("on");
  bool  restartRequired = false;

  http_handle_globals(request);

  if (request->arg("mdnsdevicename") != host) {
    host = request->arg("mdnsdevicename");
    eeprom_update_device_name(host);
    // Postpone the restart until after all changes are committed to EEPROM.
    restartRequired = true;
  }

  int newBootMode = bootMode;
  if (request->arg("bootmodewifi") == checked)
    if (request->arg("bootmodeap") == checked)
      if (request->arg("bootmodeble") == checked)
        newBootMode = PED_BOOT_AP;
      else newBootMode = PED_BOOT_AP_NO_BLE;
    else if (request->arg("bootmodeble") == checked)
           newBootMode = PED_BOOT_NORMAL;
         else newBootMode = PED_BOOT_WIFI;
  else if (request->arg("bootmodeble") == checked) newBootMode = PED_BOOT_BLE;
       else newBootMode = PED_BOOT_NORMAL;
  if (newBootMode != bootMode) {
    bootMode = newBootMode;
    //eeprom_update_boot_mode(bootMode);
  }

  if (request->arg("wifiSSID") != wifiSSID || request->arg("wifiPassword") != wifiPassword) {
    wifiSSID      = request->arg("wifiSSID");
    wifiPassword  = request->arg("wifiPassword");
    restartRequired = (bootMode == PED_BOOT_NORMAL || bootMode == PED_BOOT_WIFI);
    if (restartRequired) eeprom_update_sta_wifi_credentials();
  }

  if (request->arg("ssidSoftAP") != ssidSoftAP || request->arg("passwordSoftAP") != passwordSoftAP) {
    ssidSoftAP      = request->arg("ssidSoftAP");
    passwordSoftAP  = request->arg("passwordSoftAP");
    restartRequired = (bootMode == PED_BOOT_AP || bootMode == PED_BOOT_AP_NO_BLE);
    if (restartRequired) eeprom_update_ap_wifi_credentials(ssidSoftAP, passwordSoftAP);
  }

  if (request->arg("httpUsername") != httpUsername || request->arg("httpPassword") != httpPassword) {
    httpUsername  = request->arg("httpUsername");
    httpPassword  = request->arg("httpPassword");
    restartRequired = false;
  }

  if (request->arg("presstime").toInt() != pressTime) {
    pressTime = request->arg("presstime").toInt();
    loadConfig = true;
  }
  if (request->arg("doublepresstime").toInt() != doublePressTime) {
    doublePressTime = request->arg("doublepresstime").toInt();
    loadConfig = true;
  }
  if (request->arg("longpresstime").toInt() != longPressTime) {
    longPressTime = request->arg("longpresstime").toInt();
    loadConfig = true;
  }
  if (request->arg("repeatpresstime").toInt() != repeatPressTime) {
    repeatPressTime = request->arg("repeatpresstime").toInt();
    loadConfig = true;
  }

  //if (pressTimeChanged) eeprom_update_press_time(pressTime, doublePressTime,longPressTime, repeatPressTime);

  bool newTapDanceMode = (request->arg("tapdancemode") == checked);
  if (newTapDanceMode != tapDanceMode) {
    tapDanceBank = newTapDanceMode;
    //eeprom_update_tap_dance(tapDanceMode);
  }

  bool newRepeatOnBankSwitch = (request->arg("repeatonbankswitch") == checked);
  if (newRepeatOnBankSwitch != repeatOnBankSwitch) {
    repeatOnBankSwitch = newRepeatOnBankSwitch;
    //eeprom_update_repeat_on_bank_switch(repeatOnBankSwitch);
  }

  bool newLadder = false;
  for (byte i = 0; i < LADDER_STEPS; i++) {
    String a = request->arg(String("threshold") + String(i+1));
    newLadder = newLadder || ladderLevels[i] != a.toInt();
    ladderLevels[i] = a.toInt();
  }
  // if (newLadder) eeprom_update_ladder();

  if (request->arg("encodersensitivity").toInt() != encoderSensitivity) {
    encoderSensitivity = request->arg("encodersensitivity").toInt();
    //eeprom_update_encoder_sensitivity(encoderSensitivity);
    loadConfig = true;
  }

  if (request->arg("ledsonbrightness").toInt() != ledsOnBrightness) {
    ledsOnBrightness = request->arg("ledsonbrightness").toInt();
    //FastLED.setBrightness(map(ledsOnBrightness, 0, 25, 0, 255));
    //FastLED.show();
  }

  if (request->arg("ledsoffbrightness").toInt() != ledsOffBrightness) {
    ledsOffBrightness = request->arg("ledsoffbrightness").toInt();
  }
#ifdef BLINK
  bool newBlynkCloud = (request->arg("blynkcloud") == checked);
  if (newBlynkCloud & !blynk_enabled()) {
    blynk_enable();
    //eeprom_update_blynk_cloud_enable(true);
    //blynk_connect();
    //blynk_refresh();
  }
  if (!newBlynkCloud & blynk_enabled()) {
    blynk_disconnect();
    blynk_disable();
    //eeprom_update_blynk_cloud_enable(false);
  }

  if (request->arg("blynkauthtoken") != String(blynkAuthToken)) {
    blynk_disconnect();
    //eeprom_update_blynk_auth_token(request->arg("blynkauthtoken"));
    blynk_set_token(request->arg("blynkauthtoken"));
    //blynk_connect();
    //blynk_refresh();
  }
#endif

  if (request->arg("action") == String("apply")) {
    alert = F("Changes applied. Changes will be lost on next reboot or on profile switch if not saved.");
  }
  else if (request->arg("action") == String("save")) {
    eeprom_update_globals();
    alert = "Changes saved.";
  }
  else if (request->arg("action") == String("factorydefault")) {
    eeprom_initialize();
    restartRequired = true;
  }
  else if (request->arg("action") == String("reboot")) {
    restartRequired = true;
  }

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_options_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);

  // Restart only after all changes have been committed to EEPROM, and the response has been sent to the HTTP client.
  if (restartRequired) {
    delay(1000);
    ESP.restart();
  }
}

void http_handle_post_configurations(AsyncWebServerRequest *request) {

  const String checked("on");

  if (request->arg("action") == String("new")) {
    if (request->arg("newconfiguration").isEmpty())  {
      alertError = F("Configuration not saved. No configuration name provided.");
    }
    else {
      String configname("/" + request->arg("newconfiguration") + ".cfg");

      File file = SPIFFS.open(configname, FILE_WRITE);
      if (file) {
        file.close();
        spiffs_save_config(configname,
                           request->arg("actions1")    == checked,
                           request->arg("pedals1")     == checked,
                           request->arg("interfaces1") == checked,
                           request->arg("sequences1")  == checked,
                           request->arg("options1")    == checked);
        alert = F("Current profile setup saved as '");
        alert += request->arg("newconfiguration") + F("'.");
      }
      else {
        alertError = F("Cannot create '");
        alertError += request->arg("newconfiguration") + F("'.");
      }
    }
  }
  else if (request->arg("action") == String("upload")) {
    alertError = F("No file selected. Choose file using Browse button.");
  }
  else if (request->arg("action") == String("apply")) {
    String config = request->arg("filename");
    controller_delete();
    spiffs_load_config(config,
                       request->arg("actions2")    == checked,
                       request->arg("pedals2")     == checked,
                       request->arg("interfaces2") == checked,
                       request->arg("sequences2")  == checked,
                       request->arg("options2")    == checked);
    sort_actions();
    create_banks();
    loadConfig = true;
    config = config.substring(1, config.length() - 4);
    alert = F("Configuration '");
    alert += config + F("' loaded into current profile and running. Profile not saved.");
  }
  else if (request->arg("action") == String("save")) {
    String config = request->arg("filename");
    controller_delete();
    spiffs_load_config(config,
                       request->arg("actions2")    == checked,
                       request->arg("pedals2")     == checked,
                       request->arg("interfaces2") == checked,
                       request->arg("sequences2")  == checked,
                       request->arg("options2")    == checked);
    sort_actions();
    create_banks();
    eeprom_update_globals();
    eeprom_update_profile();
    reloadProfile = true;
    config = config.substring(1, config.length() - 4);
    alert = F("Configuration '");
    alert += config + F("' loaded and saved into current profile.");
  }
  else if (request->arg("action") == String("download")) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, request->arg("filename"), String(), true);
    request->send(response);
    return;
  }
  else if (request->arg("action") == String("delete")) {
    String config = request->arg("filename");
    if (SPIFFS.remove(config)) {
      config = config.substring(1, config.length() - 4);
      alert = F("Configuration '");
      alert += config + F("' deleted.");
    }
    else {
      alertError = F("Cannot delete '");
      alertError += config + F("'.");
    }
  }
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_configurations_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

// handler for the file upload

void http_handle_configuration_file_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

  //Upload handler chunks in data
  if (!index) {
    alert       = "";
    alertError  = "";

    String c = "/" + filename;
    if (!(c.length() > 4 && c.lastIndexOf(".cfg") == (c.length() - 4))) c += ".cfg";   // add .cfg extension if not present

    if (SPIFFS.exists(c)) {
      alertError = F("File '");
      alertError += c +  "' already exists. Delete existing configuration before upload again.";
      request->redirect("/configurations");
      return;
    }
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open(c, FILE_WRITE);

    if (request->_tempFile) {
      DPRINT("Upload start: %s\n", filename.c_str());
    }
    else {
      alertError = F("Cannot upload file ");
      alertError += filename +  ".";
      DPRINT("Upload start fail: %s\n", filename.c_str());
    }
  }

  // stream the incoming chunk to the opened file
  if (!request->_tempFile || request->_tempFile.write(data,len) != len) {
    alertError = F("Upload of '");
    alertError += filename +  "' failed.";
    DPRINT("Upload fail\n");
  }

  // if the final flag is set then this is the last frame of data
  if (request->_tempFile && final) {
    DPRINT("Upload end: %s, %u bytes\n", filename.c_str(), index+len);
    request->_tempFile.close();
    alert = F("Upload of '");
    alert += filename +  "' completed.";
  }
}

#ifdef WEBSOCKET
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

  //static bool connected = false;

  if(type == WS_EVT_CONNECT){
    //client connected
    DPRINT("ws[%s][%u] connect\n", server->url(), client->id());
    //client->printf("Hello Client %u :)", client->id());
    //client->ping();
    //client->keepAlivePeriod(1);
    //connected = true;
    wsClient = client;
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    DPRINT("ws[%s][%u] disconnect\n", server->url(), client->id());
    //connected = false;
    wsClient = NULL;
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
        for (size_t i = 0; i < info->len; i++) {
          DPRINT("%02x ", data[i]);
        }
        data[info->len-1] = 0;
        DPRINT(" %s\n", (char*)data);
        //DPRINT("%d\n", ESP.getFreeHeap());
        if (strcmp((const char *)data, ".") == 0) {
          //AsyncWebSocketMessageBuffer *buffer = webSocket.makeBuffer(128*64);
          //memcpy(buffer->get(), display.buffer, 128*64);
          //if (connected && buffer) {client->binary(buffer); delete buffer; buffer = NULL;}
          //client->binary(display.buffer, 128*64);
        }
        else if (strcmp((const char *)data, "start") == 0)
          mtc_start();
        else if (strcmp((const char *)data, "stop") == 0)
          mtc_stop();
        else if (strcmp((const char *)data, "continue") == 0)
          mtc_continue();
        else if (strcmp((const char *)data, "tap") == 0)
          mtc_tap();
        else if (strcmp((const char *)data, "clock-master") == 0) {
          MTC.setMode(MidiTimeCode::SynchroClockMaster);
          bpm = (bpm == 0) ? 120 : bpm;
          MTC.setBpm(bpm);
          currentMidiTimeCode = PED_MIDI_CLOCK_MASTER;
        }
        else if (strcmp((const char *)data, "clock-slave") == 0) {
          MTC.setMode(MidiTimeCode::SynchroClockSlave);
          currentMidiTimeCode = PED_MIDI_CLOCK_SLAVE;
          bpm = 0;
        }
        else if (strcmp((const char *)data, "mtc-master") == 0) {
          MTC.setMode(MidiTimeCode::SynchroMTCMaster);
          MTC.sendPosition(0, 0, 0, 0);
          currentMidiTimeCode = PED_MTC_MASTER_24;
        }
        else if (strcmp((const char *)data, "mtc-slave") == 0) {
          MTC.setMode(MidiTimeCode::SynchroMTCSlave);
          currentMidiTimeCode = PED_MTC_SLAVE;
        }
        else if (strcmp((const char *)data, "4/4") == 0) {
          timeSignature = PED_TIMESIGNATURE_4_4;
          MTC.setBeat(4);
        }
        else if (strcmp((const char *)data, "3/4") == 0) {
          timeSignature = PED_TIMESIGNATURE_3_4;
          MTC.setBeat(3);
        }
        else if (strcmp((const char *)data, "2/4") == 0) {
          timeSignature = PED_TIMESIGNATURE_2_4;
          MTC.setBeat(2);
        }
        else if (strcmp((const char *)data, "3/8") == 0) {
          timeSignature = PED_TIMESIGNATURE_3_8;
          MTC.setBeat(3);
        }
        else if (strcmp((const char *)data, "6/8") == 0) {
          timeSignature = PED_TIMESIGNATURE_6_8;
          MTC.setBeat(3);
        }
        else if (strcmp((const char *)data, "9/8") == 0) {
          timeSignature = PED_TIMESIGNATURE_9_8;
          MTC.setBeat(3);
        }
        else if (strcmp((const char *)data, "12/8") == 0) {
          timeSignature = PED_TIMESIGNATURE_12_8;
          MTC.setBeat(3);
        }
        else {
          int b;
          if (sscanf((const char *)data, "bank%d", &b) == 1)
            currentBank = constrain(b - 1, 0, BANKS);
            leds_refresh();
        }
      }
      /*
      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
      */
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
          /*
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
          */
        }
      }
    }
  }
}
#endif  // NO_WEBSOCKET


void get_update_page() {

#ifdef WEBCONFIG
  get_top_page();
#else
  page += F("<!doctype html>");
  page += F("<html lang='en'>");
  page += F("<head>");
  page += F("<title>PedalinoMini&trade;</title>");
  page += F("</head>");
  page += F("<body>");
#endif

  page += F("<p></p>");
  page += F("<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");

#ifdef WEBCONFIG
  get_footer_page();
#else
  page += F("</body>");
  page += F("</html>");
#endif
}

// handler for the /update form page

void http_handle_update (AsyncWebServerRequest *request) {
  //if (request->hasArg("theme")) theme = request->arg("theme");
  // The connection will be closed after completion of the response.
  // The connection SHOULD NOT be considered `persistent'.
  // Applications that do not support persistent connections MUST include the "close" connection option in every message.
  //httpServer.sendHeader("Connection", "close");
  //if (!request->authenticate("admin", "password")) {
	//		return request->requestAuthentication();
	//}
  get_update_page();
  request->send(200, "text/html", page);
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

  //Upload handler chunks in data
  if (!index) {
    // Disconnect, not to interfere with OTA process
    blynk_disconnect();
#ifdef WEBSOCKET
    webSocket.enable(false);
    webSocket.closeAll();
#endif
    firmwareUpdate = true;
    leds.setAllLow();
    leds.write();
    DPRINT("Update Start: %s\n", filename.c_str());

    int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
    // Start with max available size
    if (Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      DPRINT("Update start\n");
    }
    else {
      StreamString str;
      Update.printError(str);
      DPRINT("Update start fail: %s", str.c_str());
      firmwareUpdate = false;
    }
  }

  if (!Update.hasError()) {
    // Write chunked data to the free sketch space
    if (Update.write(data, len) == len) {
      if (Update.size()) {
        unsigned int progress = 100 * Update.progress() / Update.size();
        if (progress < 15) {
          leds.setHigh(0);
          leds.write();
        }
        else if (progress < 30) {
          leds.setHigh(1);
          leds.write();
        }
        else if (progress < 45) {
          leds.setHigh(2);
          leds.write();
        }
        else if (progress < 60) {
          leds.setHigh(3);
          leds.write();
        }
        else if (progress < 75) {
          leds.setHigh(4);
          leds.write();
        }
        else if (progress < 90) {
          leds.setHigh(5);
          leds.write();
        }
        DPRINT("Progress: %5.1f%%\n", 100.0 * Update.progress() / Update.size());
      }
    }
    else {
      StreamString str;
      Update.printError(str);
      DPRINT("Update fail: %s", str.c_str());
    }
  }

  // if the final flag is set then this is the last frame of data
  if (final) {
    if (Update.end(true)) {   //true to set the size to the current progress
      DPRINT("Update Success: %uB\n", index+len);
      leds.kittCar();
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

void http_setup() {

#ifdef WEBCONFIG
#ifdef WEBSOCKET
  webSocket.onEvent(onWsEvent);
  httpServer.addHandler(&webSocket);
  //events.setAuthentication("user", "pass");
  httpServer.addHandler(&events);
#endif
  httpServer.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico").setDefaultFile("/favicon.ico").setCacheControl("max-age=600");
  httpServer.serveStatic("/logo.png", SPIFFS, "/logo.png").setDefaultFile("/logo.png").setCacheControl("max-age=600");
  httpServer.serveStatic("/css/bootstrap.min.css", SPIFFS, "/css/bootstrap.min.css").setDefaultFile("/css/bootstrap.min.css").setCacheControl("max-age=600");
  httpServer.serveStatic("/js/bootstrap.min.js", SPIFFS, "/js/bootstrap.min.js").setDefaultFile("/js/bootstrap.min.js").setCacheControl("max-age=600");
  httpServer.serveStatic("/js/jquery-3.5.1.slim.min.js", SPIFFS, "/js/jquery-3.5.1.slim.min.js").setDefaultFile("/js/jquery-3.5.1.slim.min.js").setCacheControl("max-age=600");
  httpServer.serveStatic("/js/popper.min.js", SPIFFS, "/js/popper.min.js").setDefaultFile("/js/popper.min.js").setCacheControl("max-age=600");
  httpServer.serveStatic("/files", SPIFFS, "/").setDefaultFile("").setAuthentication(httpUsername.c_str(), httpPassword.c_str());

  httpServer.on("/",                            http_handle_root);
  httpServer.on("/login",           HTTP_GET,   http_handle_login);
  httpServer.on("/login",           HTTP_POST,  http_handle_post_login);
  httpServer.on("/live",            HTTP_GET,   http_handle_live);
  httpServer.on("/live",            HTTP_POST,  http_handle_post_live);
  httpServer.on("/actions",         HTTP_GET,   http_handle_actions);
  httpServer.on("/actions",         HTTP_POST,  http_handle_post_actions);
  httpServer.on("/pedals",          HTTP_GET,   http_handle_pedals);
  httpServer.on("/pedals",          HTTP_POST,  http_handle_post_pedals);
  httpServer.on("/sequences",       HTTP_GET,   http_handle_sequences);
  httpServer.on("/sequences",       HTTP_POST,  http_handle_post_sequences);
  httpServer.on("/interfaces",      HTTP_GET,   http_handle_interfaces);
  httpServer.on("/interfaces",      HTTP_POST,  http_handle_post_interfaces);
  httpServer.on("/options",         HTTP_GET,   http_handle_options);
  httpServer.on("/options",         HTTP_POST,  http_handle_post_options);
  httpServer.on("/configurations",  HTTP_GET,   http_handle_configurations);
  httpServer.on("/configurations",  HTTP_POST,  http_handle_post_configurations, http_handle_configuration_file_upload);

  httpServer.on("/update",          HTTP_GET,   http_handle_update);
  httpServer.on("/update",          HTTP_POST,  http_handle_update_file_upload_finish, http_handle_update_file_upload);
  httpServer.onNotFound(http_handle_not_found);

  httpServer.begin();

  // Setup a 10Hz timer
  Timer2Attach(100);
#endif  // WEBCONFIG
}

#endif  // WEBCONFIG

inline void http_run() {

  if (interruptCounter2 > 0) {

    interruptCounter2 = 0;

#ifdef WEBSOCKET
    //webSocket.binaryAll(display.buffer, 128*64);
    if (wsClient) wsClient->binary(display.buffer, 128*64);
     // Limits the number of clients by closing the oldest client
     // when the maximum number of clients has been exceeded
    webSocket.cleanupClients();
#endif

/*
    if (!buffer) {
      buffer = webSocket.makeBuffer(128*64);
      memcpy(buffer->get(), display.buffer, 128*64);
    }
    */
  }
}

#endif  // WIFI