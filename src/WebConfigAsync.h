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

String theme = "bootstrap";

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

AsyncWebServer          httpServer(80);

#ifdef WEBCONFIG

#ifdef WEBSOCKET
AsyncWebSocket               webSocket("/ws");
AsyncEventSource             events("/events");    // EventSource is single direction, text-only protocol.
AsyncWebSocketMessageBuffer *buffer = NULL;
#endif

String page       = "";
String alert      = "";
String uiprofile  = "1";
String uibank     = "1";
String uisequence = "1";


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
    page += F("<link rel='stylesheet' href='/css/bootstrap.min.css' integrity='sha256-L/W5Wfqfa0sdBNIKN9cG6QA5F2qx4qICmU2VgLruv9Y=' crossorigin='anonymous'>");
  #else
    page += F("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css' integrity='sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh' crossorigin='anonymous'>");
  #endif
  } else {
    page += F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootswatch/4.4.1/");
    page += theme;
    page += F("/bootstrap.min.css' crossorigin='anonymous'>");
  }
  if (p == -1) page += F("<link rel='stylesheet' href='/css/floating-labels.css'>");
  page += F("</head>");

  page += F("<body>");
  if (p >= 0) {
  page += F("<p></p>");
  page += F("<div class='container-fluid'>");

  page += F("<nav class='navbar navbar-expand navbar-light bg-light'>");
  page += F("<a class='navbar-brand' href='/'>");
  page += F("<img src='/logo.png' width='30' height='30' class='d-inline-block align-top' alt=''>Pedalino&trade;</a>");
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
  page += F("<a class='nav-link' href='/sequences'>Sequences</a>");
  page += F("</li>");
  page += F("<li class='nav-item");
  page += (p == 6 ? F(" active'>") : F("'>"));
  page += F("<a class='nav-link' href='/options'>Options</a>");
  page += F("</li>");
  page += F("</ul>");
  }
  //if (p > 1)
  {
    page += F("<form class='form-inline my-2 my-lg-0'>");
    page += currentProfile == 0 ? F("<a class='btn btn-primary' href='?profile=1' role='button'>A</a>") : F("<a class='btn btn-outline-primary' href='?profile=1' role='button'>A</a>");
    page += currentProfile == 1 ? F("<a class='btn btn-primary' href='?profile=2' role='button'>B</a>") : F("<a class='btn btn-outline-primary' href='?profile=2' role='button'>B</a>");
    page += currentProfile == 2 ? F("<a class='btn btn-primary' href='?profile=3' role='button'>C</a>") : F("<a class='btn btn-outline-primary' href='?profile=3' role='button'>C</a>");

    //page += F("<button class='btn btn-primary my-2 my-sm-0' type='button'>Save</button>");
/*
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
*/
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
}

void get_footer_page() {

  //page += F("<nav class='navbar align-items-end navbar-light bg-light'>");
  //page += F("<a class='navbar-text' href='https://github.com/alf45tar/PedalinoMini'>https://github.com/alf45tar/PedalinoMini</a>");
  //page += F("</nav>");

  page += F("<p></p>");
  page += F("</div>");
#ifdef BOOTSTRAP_LOCAL
  page += F("<script src='/js/jquery-3.4.1.slim.min.js' integrity='sha256-pasqAKBDmFT4eHoN2ndd6lN370kFiGUFyTiUHWhU7k8=' crossorigin='anonymous'></script>");
  page += F("<script src='/js/popper.min.js' integrity='sha256-x3YZWtRjM8bJqf48dFAv/qmgL68SI4jqNWeSLMZaMGA=' crossorigin='anonymous'></script>");
  page += F("<script src='/js/bootstrap.min.js' integrity='sha256-WqU1JavFxSAMcLP2WIOI+GB2zWmShMI82mTpLDcqFUg=' crossorigin='anonymous'></script>");
#else
  page += F("<script src='https://code.jquery.com/jquery-3.4.1.slim.min.js' integrity='sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n' crossorigin='anonymous'></script>");
  page += F("<script src='https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js' integrity='sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo' crossorigin='anonymous'></script>");
  page += F("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js' integrity='sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6' crossorigin='anonymous'></script>");
#endif
  page += F("</body>");
  page += F("</html>");
}

void get_login_page() {

  get_top_page(-1);
  
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

  get_footer_page();
}

void get_root_page() {

  get_top_page();

  page += F("<h4 class='display-4'>Wireless MIDI foot controller</h4>");
  page += F("<p></p>");

  page += F("<div class='row'>");

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
  page += F("<dt>PSRAM Free/Total</dt><dd>");
  page += ESP.getFreePsram() / 1024;
  page += F("/");
  page += ESP.getPsramSize() / 1024;
  page += F(" kB</dd>");
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
            "setInterval(requestRemoteDisplay, 100);"
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

void get_banks_page() {

  const byte b = constrain(uibank.toInt(), 0, BANKS);

  get_top_page(2);

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
  page += F("<span class='badge badge-primary'>Name</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>MIDI Message</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary'>MIDI Channel</span>");
  page += F("</div>");
  page += F("<div class='col-2'>");
  page += F("<span class='badge badge-primary'>Control Change/Note</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary' data-toggle='tooltip' title='Single Press/From'>MIDI Value 1</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary' data-toggle='tooltip' title='Double Press'>MIDI Value 2</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary' data-toggle='tooltip' title='Long Press/To'>MIDI Value 3</span>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  for (unsigned int i = 1; i <= PEDALS; i++) {
    page += F("<div class='col-1 mb-3 text-center'>");
    page += String(i);
    page += F("</div>");

    page += F("<div class='col-2'>");
    page += F("<input type='text' class='form-control form-control-sm' name='name");
    page += String(i);
    page += F("' maxlength='");
    page += String(MAXPEDALNAME) + F("' value='");
    page += String(banks[b-1][i-1].pedalName);
    page += F("'></div>");

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
    page += F("<option value='");
    page += String(PED_PITCH_BEND) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_PITCH_BEND) page += F(" selected");
    page += F(">Pitch Bend</option>");
    page += F("<option value='");
    page += String(PED_CHANNEL_PRESSURE) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_CHANNEL_PRESSURE) page += F(" selected");
    page += F(">Channel Pressure</option>");
    page += F("<option value='");
    page += String(PED_MIDI_START) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_MIDI_START) page += F(" selected");
    page += F(">Start</option>");
    page += F("<option value='");
    page += String(PED_MIDI_STOP) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_MIDI_STOP) page += F(" selected");
    page += F(">Stop</option>");
    page += F("<option value='");
    page += String(PED_MIDI_CONTINUE) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_MIDI_CONTINUE) page += F(" selected");
    page += F(">Continue</option>");
    page += F("<option value='");
    page += String(PED_SEQUENCE) + F("'");
    if (banks[b-1][i-1].midiMessage == PED_SEQUENCE) page += F(" selected");
    page += F(">Sequence</option>");
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

    page += F("<div class='col-1'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value1");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(banks[b-1][i-1].midiValue1);
    page += F("'></div>");

    page += F("<div class='col-1'>");
    page += F("<input type='number' class='form-control form-control-sm' name='value2");
    page += String(i);
    page += F("' min='0' max='127' value='");
    page += String(banks[b-1][i-1].midiValue2);
    page += F("'></div>");

    page += F("<div class='col-1'>");
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

  get_footer_page();
}

void get_pedals_page() {

  get_top_page(3);

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
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary' data-toggle='tooltip' title='Analog Min/First Bank/BPM Min'>Min</span>");
  page += F("</div>");
  page += F("<div class='col-1'>");
  page += F("<span class='badge badge-primary' data-toggle='tooltip' title='Analog Max/Last Bank/BPM Max'>Max</span>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  for (unsigned int i = 1; i <= PEDALS; i++) {
    page += F("<div class='col-1 mb-3 text-center'>");
    page += String(i);
    page += F("</div>");

    page += F("<div class='col-1 text-center'>");
    page += F("<div class='custom-control custom-switch'>");
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
    page += F(">Bank+1</option>");
    page += F("<option value='");
    page += String(PED_BANK_PLUS_2) + F("'");
    if (pedals[i-1].function == PED_BANK_PLUS_2) page += F(" selected");
    page += F(">Bank+2</option>");
    page += F("<option value='");
    page += String(PED_BANK_PLUS_3) + F("'");
    if (pedals[i-1].function == PED_BANK_PLUS_3) page += F(" selected");
    page += F(">Bank+3</option>");
    page += F("<option value='");
    page += String(PED_BANK_MINUS) + F("'");
    if (pedals[i-1].function == PED_BANK_MINUS) page += F(" selected");
    page += F(">Bank-1</option>");
    page += F("<option value='");
    page += String(PED_BANK_MINUS_2) + F("'");
    if (pedals[i-1].function == PED_BANK_MINUS_2) page += F(" selected");
    page += F(">Bank-2</option>");
    page += F("<option value='");
    page += String(PED_BANK_MINUS_3) + F("'");
    if (pedals[i-1].function == PED_BANK_MINUS_3) page += F(" selected");
    page += F(">Bank-3</option>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<input type='number' class='form-control form-control-sm' name='min");
    page += String(i);
    page += F("' min='0' max='");
    page += String(ADC_RESOLUTION - 1) + F("' value='");
    page += String(pedals[i-1].expZero);
    page += F("'></div>");

    page += F("<div class='col-1'>");
    page += F("<input type='number' class='form-control form-control-sm' name='max");
    page += String(i);
    page += F("' min='0' max='");
    page += String(ADC_RESOLUTION - 1) + F("' value='");
    page += String(pedals[i-1].expMax);
    page += F("'></div>");

/*
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
*/
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

  get_footer_page();
}

void get_interfaces_page() {

  get_top_page(4);

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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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
    page += F("<div class='custom-control custom-switch'>");
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

  get_footer_page();
}

void get_sequences_page() {

  const byte s = constrain(uisequence.toInt(), 0, SEQUENCES);
  
  get_top_page(5);

  page += F("<div class='btn-group'>");
  for (unsigned int i = 1; i <= SEQUENCES; i++) {
    page += F("<form method='get'><button type='button submit' class='btn");
    page += (uisequence == String(i) ? String(" btn-primary") : String(""));
    page += F("' name='sequence' value='");
    page += String(i) + F("'>") + String(i) + F("</button></form>");
  }
  page += F("</div>");
  
  page += F("<p></p>");

  page += F("<form method='post'>");
  page += F("<div class='form-row'>");
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

  page += F("<p></p>");

  page += F("<div class='form-row'>");
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
    page += String(PED_NOTE_ON_OFF) + F("'");
    if (sequences[s-1][i-1].midiMessage == PED_NOTE_ON_OFF) page += F(" selected");
    page += F(">Note On/Off</option>");
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

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<div class='col-auto'>");
  page += F("<button type='submit' class='btn btn-primary'>Save</button>");
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
  
  get_top_page(6);

  page += F("<form method='post'>");

  page += F("<div class='form-row'>");
  page += F("<label for='devicename' class='col-2 col-form-label'>Device Name</label>");
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

  page += F("<div class='form-row'>");
  page += F("<label for='tapDanceMode' class='col-2 col-form-label'>Tap Dance Mode</label>");
  page += F("<div class='col-10'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='tapDanceMode' name='tapdancemode'");
  if (tapDanceMode) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='tapDanceMode'>One press for bank followed by one press for MIDI event</label>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>The first press of pedal X switch to bank X, the second press of any pedal send the MIDI event.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<label for='repeatOnBankSwitch' class='col-2 col-form-label'>Bank Switch</label>");
  page += F("<div class='col-10'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='repeatOnBankSwitch' name='repeatonbankswitch'");
  if (repeatOnBankSwitch) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='repeatOnBankSwitch'>Send last MIDI message on bank switch</label>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>On bank switch repeat the last MIDI message that was sent for that bank.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<label for='pressTime' class='col-2 col-form-label'>Press Time</label>");
  page += F("<div class='col-10'>");
  page += F("<input class='form-control' type='text' maxlength='32' id='pressTime' name='presstime' placeholder='' value='");
  page += String(pressTime) + F("'>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>Switch press time in milliseconds.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<label for='doublePressTime' class='col-2 col-form-label'>Double Press Time</label>");
  page += F("<div class='col-10'>");
  page += F("<input class='form-control' type='text' maxlength='32' id='doublePressTime' name='doublepresstime' placeholder='' value='");
  page += String(doublePressTime) + F("'>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>Set double press detection time between each press time in milliseconds.</p>");
  page += F("<p>A double press is detected if the switch is released and depressed within this time, measured from when the first press is detected.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<label for='longPressTime' class='col-2 col-form-label'>Long Press Time</label>");
  page += F("<div class='col-10'>");
  page += F("<input class='form-control' type='text' maxlength='32' id='longPressTime' name='longpresstime' placeholder='' value='");
  page += String(longPressTime) + F("'>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("Set the long press time in milliseconds after which a continuous press and release is deemed a long press, measured from when the first press is detected.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

  page += F("<div class='form-row'>");
  page += F("<label for='repeatPressTime' class='col-2 col-form-label'>Repeat Press Time</label>");
  page += F("<div class='col-10'>");
  page += F("<input class='form-control' type='text' maxlength='32' id='repeatPressTime' name='repeatpresstime' placeholder='' value='");
  page += String(repeatPressTime) + F("'>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>Set the repeat time in milliseconds after which a continuous press and hold is treated as a stream of repeated presses, measured from when the first press is detected.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");

  page += F("<p></p>");

#ifdef BLYNK
  page += F("<div class='form-row'>");
  page += F("<label for='blynkCloud' class='col-2 col-form-label'>Blynk Cloud</label>");
  page += F("<div class='col-10'>");
  page += F("<div class='custom-control custom-switch'>");
  page += F("<input type='checkbox' class='custom-control-input' id='blynkCloud' name='blynkcloud'");
  if (blynk_enabled()) page += F(" checked");
  page += F(">");
  page += F("<label class='custom-control-label' for='blynkCloud'>Enable/disable connection to Blynk Cloud</label>");
  page += F("</div>");
  page += F("</div>");
  page += F("<div class='w-100'></div>");
  page += F("<div class='col-2'>");
  page += F("</div>");
  page += F("<div class='col-10'>");
  page += F("<div class='shadow p-3 bg-white rounded'>");
  page += F("<p>If Blynk Cloud connection is disabled the app cannot connect to Pedalino.</p>");
  page += F("</div>");
  page += F("</div>");
  page += F("</div>");
  page += F("<p></p>");
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

size_t get_banks_page_chunked(uint8_t *buffer, size_t maxLen, size_t index) {

  static bool rebuild = true;

  if (rebuild) {
    get_banks_page();
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

void http_handle_login(AsyncWebServerRequest *request) {
  get_login_page();
  request->send(200, "text/html", page);
}

void http_handle_post_login(AsyncWebServerRequest *request) {
  if (request->hasArg("username")) {
    if (request->arg("username") == String("admin")) 
      if (request->hasArg("password")) 
        if (request->arg("password") == host)
          request->redirect("/");
  }
  get_login_page();
  request->send(200, "text/html", page);
}

void http_handle_globals(AsyncWebServerRequest *request) {
  
  if (request->hasArg("profile")) {
    uiprofile = request->arg("profile");
    currentProfile = constrain(uiprofile.toInt() - 1, 0, PROFILES - 1);
    eeprom_read_profile(currentProfile);
    autosensing_setup();
    controller_setup();
    mtc_setup();
  }

  if (request->hasArg("theme")) {
    theme = request->arg("theme");
    eeprom_update_theme(theme);
  }  
}

void http_handle_root(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_root_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_live(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_live_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_banks(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  if (request->hasArg("bank"))  uibank  = request->arg("bank");
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_banks_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_pedals(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_pedals_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_interfaces(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_interfaces_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_sequences(AsyncWebServerRequest *request) {
  http_handle_globals(request);
  if (request->hasArg("sequence"))  uisequence  = request->arg("sequence");
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_sequences_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_options(AsyncWebServerRequest *request) {
  http_handle_globals(request);

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_options_page_chunked);
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


void http_handle_post_banks(AsyncWebServerRequest *request) {
  
  String     a;
  const byte b = constrain(uibank.toInt() - 1, 0, BANKS);
  
  for (unsigned int i = 0; i < PEDALS; i++) {
    a = request->arg(String("name") + String(i+1));
    strncpy(banks[b][i].pedalName, a.c_str(), MAXPEDALNAME+1);
    banks[b][i].pedalName[MAXPEDALNAME] = 0;

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
  eeprom_update_profile();
  blynk_refresh();
  alert = "Saved";

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_banks_page_chunked);
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
  eeprom_update_profile();
  autosensing_setup();
  controller_setup();
  blynk_refresh();
  alert = "Saved";

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
  eeprom_update_profile();
  blynk_refresh();
  alert = "Saved";

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
  eeprom_update_profile();
  blynk_refresh();
  alert = "Saved";

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_sequences_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);
}

void http_handle_post_options(AsyncWebServerRequest *request) {

  const String checked("on");
  bool  restartRequired = false;
  
  http_handle_globals(request);

  tapDanceMode = (request->arg("tapdancemode") == checked);
  eeprom_update_tap_dance(tapDanceMode);
  tapDanceBank = true;
  
  repeatOnBankSwitch = (request->arg("repeatonbankswitch") == checked);
  eeprom_update_repeat_on_bank_switch(repeatOnBankSwitch);

  if (request->arg("blynkcloud") == checked) {
    blynk_enable();
    eeprom_update_blynk_cloud_enable(true);
    //blynk_connect();
    //blynk_refresh();
  }
  else {
    blynk_disconnect();
    blynk_disable();
    eeprom_update_blynk_cloud_enable(false);
  }

  if (request->arg("blynkauthtoken")) {
    blynk_disconnect();
    eeprom_update_blynk_auth_token(request->arg("blynkauthtoken"));
    blynk_set_token(request->arg("blynkauthtoken"));
    //blynk_connect();
    //blynk_refresh();
  }

  if (request->arg("mdnsdevicename") != host) {
    host = request->arg("mdnsdevicename");
    eeprom_update_device_name(host);
    // Postpone the restart until after all changes are committed to EEPROM.
    restartRequired = true;
  }

  bool pressTimeChanged = false;

  if (request->arg("presstime").toInt() != pressTime) {
    pressTime = request->arg("presstime").toInt();
    pressTimeChanged = true;
    controller_setup();
  }
  if (request->arg("doublepresstime").toInt() != doublePressTime) {
    doublePressTime = request->arg("doublepresstime").toInt();
    pressTimeChanged = true;
    controller_setup();
  }
  if (request->arg("longpresstime").toInt() != longPressTime) {
    longPressTime = request->arg("longpresstime").toInt();
    pressTimeChanged = true;
    controller_setup();
  }
  if (request->arg("repeatpresstime").toInt() != repeatPressTime) {
    repeatPressTime = request->arg("repeatpresstime").toInt();
    pressTimeChanged = true;
    controller_setup();
  }

  if (pressTimeChanged)
    eeprom_update_press_time(pressTime, doublePressTime,longPressTime, repeatPressTime);

  alert = "Saved";
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/html", get_options_page_chunked);
  response->addHeader("Connection", "close");
  request->send(response);

  // Restart only after all changes have been committed to EEPROM, and the response has been sent to the HTTP client.
  if (restartRequired) {
    delay(1000);
    ESP.restart();
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
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    DPRINT("ws[%s][%u] disconnect\n", server->url(), client->id());
    //connected = false;
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
  page += "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

#ifdef WEBCONFIG
  get_footer_page();
#else
  page += F("</body>");
  page += F("</html>");
#endif
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

  if (!index) {
    // Disconnect, not to interfere with OTA process
    blynk_disconnect();
#ifdef WEBSOCKET
    webSocket.enable(false);
    webSocket.textAll("Web Update Started");
    webSocket.closeAll();
#endif

    DPRINT("Update Start: %s\n", filename.c_str());
    display_ui_update_disable();
    display_progress_bar_title("HTTP Update");
    if (Update.begin()) {  //start with max available size
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

void http_setup() {

#ifdef WEBCONFIG
#ifdef WEBSOCKET
  webSocket.onEvent(onWsEvent);
  httpServer.addHandler(&webSocket);
  //events.setAuthentication("user", "pass");
  httpServer.addHandler(&events);
#endif
  if (!SPIFFS.begin()) {
      DPRINT("SPIFFS mount FAILED\n");
  }
  else {
    DPRINT("SPIFFS mount OK\n");
  }
  httpServer.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico").setDefaultFile("/favicon.ico").setCacheControl("max-age=600");
  httpServer.serveStatic("/logo.png", SPIFFS, "/logo.png").setDefaultFile("/logo.png").setCacheControl("max-age=600");
  httpServer.serveStatic("/css/bootstrap.min.css", SPIFFS, "/css/bootstrap.min.css").setDefaultFile("/css/bootstrap.min.css").setCacheControl("max-age=600");
  httpServer.serveStatic("/js/bootstrap.min.js", SPIFFS, "/js/bootstrap.min.js").setDefaultFile("/js/bootstrap.min.js").setCacheControl("max-age=600");
  httpServer.serveStatic("/js/jquery-3.4.1.slim.min.js", SPIFFS, "/js/jquery-3.4.1.slim.min.js").setDefaultFile("/js/jquery-3.4.1.slim.min.js").setCacheControl("max-age=600");
  httpServer.serveStatic("/js/popper.min.js", SPIFFS, "/js/popper.min.js").setDefaultFile("/js/popper.min.js").setCacheControl("max-age=600");

  httpServer.on("/",                        http_handle_root);
  httpServer.on("/login",       HTTP_GET,   http_handle_login);
  httpServer.on("/login",       HTTP_POST,  http_handle_post_login);
  httpServer.on("/live",        HTTP_GET,   http_handle_live);
  httpServer.on("/live",        HTTP_POST,  http_handle_post_live);
  httpServer.on("/banks",       HTTP_GET,   http_handle_banks);
  httpServer.on("/banks",       HTTP_POST,  http_handle_post_banks);
  httpServer.on("/pedals",      HTTP_GET,   http_handle_pedals);
  httpServer.on("/pedals",      HTTP_POST,  http_handle_post_pedals);
  httpServer.on("/sequences",   HTTP_GET,   http_handle_sequences);
  httpServer.on("/sequences",   HTTP_POST,  http_handle_post_sequences);
  httpServer.on("/interfaces",  HTTP_GET,   http_handle_interfaces);
  httpServer.on("/interfaces",  HTTP_POST,  http_handle_post_interfaces);
  httpServer.on("/options",     HTTP_GET,   http_handle_options);
  httpServer.on("/options",     HTTP_POST,  http_handle_post_options);
  //httpServer.on("/css/floating-labels.css", http_handle_bootstrap_file);

  httpServer.on("/update",      HTTP_GET,   http_handle_update);
  httpServer.on("/update",      HTTP_POST,  http_handle_update_file_upload_finish, http_handle_update_file_upload);
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
    webSocket.binaryAll(display.buffer, 128*64);
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