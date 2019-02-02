/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266LLMNR.h>
#include <ESP8266HTTPUpdateServer.h>
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <esp_wps.h>
#endif

#define WIFI_CONNECT_TIMEOUT    15
#define SMART_CONFIG_TIMEOUT    15
#define WPS_TIMEOUT             15

#ifdef ARDUINO_ARCH_ESP8266
#define WIFI_LED       2      // Onboard LED on GPIO2 is shared with Serial1 TX
#define WIFI_LED_OFF() digitalWrite(WIFI_LED, HIGH)
#define WIFI_LED_ON()  digitalWrite(WIFI_LED, LOW)
#endif

#ifdef ARDUINO_ARCH_ESP32
#define WIFI_LED        2
#define WIFI_LED_OFF()  digitalWrite(WIFI_LED, LOW)
#define WIFI_LED_ON()   digitalWrite(WIFI_LED, HIGH)
#endif

#ifdef WIFI

void wifi_connect();
void blynk_connect();

void save_wifi_credentials(String ssid, String password)
{
#ifdef ARDUINO_ARCH_ESP32
  eeprom_update();
#endif
}

#ifdef ARDUINO_ARCH_ESP32

static esp_wps_config_t WPS;
int                     wpsStatus = 0;

#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

String translateEncryptionType(wifi_auth_mode_t encryptionType) {

  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
    case (WIFI_AUTH_MAX):
      return "";
  }

  return "";
}
#endif

void stop_services()
{
#ifdef ARDUINO_ARCH_ESP8266
  httpServer.stop();
  ipMIDI.stop();
  oscUDP.stop();
#endif
#ifdef ARDUINO_ARCH_ESP32
  MDNS.end();
  ArduinoOTA.end();
  ipMIDI.stop();
  oscUDP.stop();
#endif
}

void start_services()
{
  stop_services();

#ifdef ARDUINO_ARCH_ESP8266
  // Start LLMNR (Link-Local Multicast Name Resolution) responder
  LLMNR.begin(host.c_str());
  DPRINT("LLMNR responder started\n");

  // Start mDNS (Multicast DNS) responder (ping pedalino.local)
  if (MDNS.begin(host.c_str())) {
    DPRINTLN("mDNS responder started");
    // service name is lower case
    // ESP8266 only: do not add '_' to service name and protocol
    MDNS.addService("apple-midi", "udp", 5004);
    MDNS.addService("osc",        "udp", oscLocalPort);
    MDNS.addService("http",       "tcp", 80);
#ifdef PEDALINO_TELNET_DEBUG
    MDNS.addService("telnet",     "tcp", 23);
#endif
  }
#endif  //  ARDUINO_ARCH_ESP8266

#ifdef ARDUINO_ARCH_ESP32
  // Start mDNS (Multicast DNS) responder
  if (MDNS.begin(host.c_str())) {
    DPRINTLN("mDNS responder started");
    // service name is lower case
    // service name and protocol starts with an '_' e.g. '_udp'
    MDNS.addService("_apple-midi", "_udp", 5004);
    MDNS.addService("_osc",        "_udp", oscLocalPort);
    MDNS.addService("_http",       "_tcp", 80);
#ifdef PEDALINO_TELNET_DEBUG
    MDNS.addService("_telnet",     "_tcp", 23);
#endif
  }
#endif  //  ARDUINO_ARCH_ESP32

  // OTA update init
  ota_begin(host.c_str());
  DPRINT("OTA update started\n");

#ifdef ARDUINO_ARCH_ESP8266
  // Start firmawre update via HTTP (connect to http://pedalino.local/update)
  httpUpdater.setup(&httpServer);
#endif

  http_setup();
  DPRINT("HTTP server started\n");
  DPRINT("Connect to http://%s.local/update for firmware update\n", host.c_str());
#ifdef WEBCONFIG
  DPRINT("Connect to http://%s.local for configuration\n", host.c_str());
#endif

#ifdef ARDUINO_ARCH_ESP8266
  ipMIDI.beginMulticast(WiFi.localIP(), ipMIDImulticast, ipMIDIdestPort);
#endif
#ifdef ARDUINO_ARCH_ESP32
  ipMIDI.beginMulticast(ipMIDImulticast, ipMIDIdestPort);
#endif
  DPRINT("ipMIDI server started\n");

  // RTP-MDI
  apple_midi_start();
  DPRINT("RTP-MIDI started\n");

  // Calculate the broadcast address of local WiFi to broadcast OSC messages
  oscRemoteIp = WiFi.localIP();
  IPAddress localMask = WiFi.subnetMask();
  for (int i = 0; i < 4; i++)
    oscRemoteIp[i] |= (localMask[i] ^ B11111111);

  // Set incoming OSC messages port
  oscUDP.begin(oscLocalPort);
  DPRINT("OSC server started\n");

  // Connect to Blynk Cloud
  blynk_connect();
}


void WiFiEvent(WiFiEvent_t event, system_event_info_t info)
{
  IPAddress localMask;

  /*
      ESP8266 events

    typedef enum WiFiEvent
    {
      WIFI_EVENT_STAMODE_CONNECTED = 0,
      WIFI_EVENT_STAMODE_DISCONNECTED,
      WIFI_EVENT_STAMODE_AUTHMODE_CHANGE,
      WIFI_EVENT_STAMODE_GOT_IP,
      WIFI_EVENT_STAMODE_DHCP_TIMEOUT,
      WIFI_EVENT_SOFTAPMODE_STACONNECTED,
      WIFI_EVENT_SOFTAPMODE_STADISCONNECTED,
      WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED,
      WIFI_EVENT_MAX,
      WIFI_EVENT_ANY = WIFI_EVENT_MAX,
      WIFI_EVENT_MODE_CHANGE
    } WiFiEvent_t;
  */

#ifdef ARDUINO_ARCH_ESP8266
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      uint8_t macAddr[6];
      WiFi.macAddress(macAddr);
      DPRINTLN("BSSID       : %s", WiFi.BSSIDstr().c_str());
      DPRINTLN("RSSI        : %d dBm", WiFi.RSSI());
      DPRINTLN("Channel     : %d", WiFi.channel());
      DPRINTLN("STA         : %02X:%02X:%02X:%02X:%02X:%02X", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
      WiFi.hostname(host);
      break;

    case WIFI_EVENT_STAMODE_GOT_IP:
      DPRINTLN("Hostname    : %s", WiFi.hostname().c_str());
      DPRINTLN("IP address  : %s", WiFi.localIP().toString().c_str());
      DPRINTLN("Subnet mask : %s", WiFi.subnetMask().toString().c_str());
      DPRINTLN("Gataway IP  : %s", WiFi.gatewayIP().toString().c_str());
      DPRINTLN("DNS 1       : %s", WiFi.dnsIP(0).toString().c_str());
      DPRINTLN("DNS 2       : %s", WiFi.dnsIP(1).toString().c_str());
      start_services();
      break;

    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      DPRINTLN("WIFI_EVENT_STAMODE_DHCP_TIMEOUT");
      break;

    case WIFI_EVENT_STAMODE_DISCONNECTED:
      DPRINTLN("WIFI_EVENT_STAMODE_DISCONNECTED");
      stop_services();
      break;

    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      break;

    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      break;

    default:
      DPRINTLN("Event: %d", event);
      break;
  }
#endif

  /*
      ESP32 events

    SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
    SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
    SYSTEM_EVENT_STA_START                < ESP32 station start
    SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
    SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
    SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
    SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
    SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
    SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
    SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
    SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
    SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
    SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
    SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
    SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
    SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
    SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
    SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
    SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
    SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
    SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
    SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
    SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
    SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
    SYSTEM_EVENT_MAX
  */
#ifdef ARDUINO_ARCH_ESP32
  switch (event) {
    case SYSTEM_EVENT_STA_START:
      DPRINT("SYSTEM_EVENT_STA_START\n");
      break;

    case SYSTEM_EVENT_STA_STOP:
      DPRINT("SYSTEM_EVENT_STA_STOP\n");
      break;

    case SYSTEM_EVENT_WIFI_READY:
      DPRINT("SYSTEM_EVENT_WIFI_READY\n");
      break;

    case SYSTEM_EVENT_STA_CONNECTED:
      DPRINT("SYSTEM_EVENT_STA_CONNECTED\n");
      uint8_t macAddr[6];
      WiFi.macAddress(macAddr);
      DPRINT("BSSID       : %s\n", WiFi.BSSIDstr().c_str());
      DPRINT("RSSI        : %d dBm\n", WiFi.RSSI());
      DPRINT("Channel     : %d\n", WiFi.channel());
      DPRINT("STA         : %02X:%02X:%02X:%02X:%02X:%02X\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
      WiFi.setHostname(host.c_str());
      break;

    case SYSTEM_EVENT_STA_GOT_IP:
      DPRINT("SYSTEM_EVENT_STA_GOT_IP\n");
      DPRINT("Hostname    : %s\n", WiFi.getHostname());
      DPRINT("IP address  : %s\n", WiFi.localIP().toString().c_str());
      DPRINT("Subnet mask : %s\n", WiFi.subnetMask().toString().c_str());
      DPRINT("Gataway IP  : %s\n", WiFi.gatewayIP().toString().c_str());
      DPRINT("DNS 1       : %s\n", WiFi.dnsIP(0).toString().c_str());
      DPRINT("DNS 2       : %s\n", WiFi.dnsIP(1).toString().c_str());
      start_services();
      break;

    case SYSTEM_EVENT_STA_LOST_IP:
      DPRINT("SYSTEM_EVENT_STA_LOST_IP\n");
      stop_services();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      DPRINT("SYSTEM_EVENT_STA_DISCONNECTED\n");
      stop_services();
      break;

    case SYSTEM_EVENT_AP_START:
      DPRINT("SYSTEM_EVENT_AP_START\n");
      //IPAddress apIP(192, 168, 1, 1);
      //IPAddress netMsk(255, 255, 255, 0);
      //WiFi.softAPConfig(apIP, apIP, netMsk);
      WiFi.softAPsetHostname(host.c_str());
      //DPRINT("AP SSID     : %s\n", WiFi.softAPSSID().c_str());
      //DPRINT("AP PSK      : %s\n", WiFi.softAPPSK().c_str());
      DPRINT("AP SSID     : %s\n", wifiSoftAP.c_str());
      //DPRINT("AP PSK      : \n");
      DPRINT("AP MAC      : %s\n", WiFi.softAPmacAddress().c_str());
      DPRINT("AP IP       : %s\n", WiFi.softAPIP().toString().c_str());
      DPRINT("Channel     : %d\n", WiFi.channel());
      DPRINT("Connect to %s wireless network with no password\n", wifiSoftAP.c_str());
      start_services();
      break;

    case SYSTEM_EVENT_AP_STOP:
      DPRINT("SYSTEM_EVENT_AP_STOP\n");
      break;

    case SYSTEM_EVENT_AP_STACONNECTED:
      DPRINT("SYSTEM_EVENT_AP_STACONNECTED\n");
      break;

    case SYSTEM_EVENT_AP_STADISCONNECTED:
      DPRINT("SYSTEM_EVENT_AP_STADISCONNECTED\n");
      break;

    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      DPRINT("SYSTEM_EVENT_AP_PROBEREQRECVED\n");
      break;

    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_SUCCESS\n");
      wpsStatus = 1;
      ESP_ERROR_CHECK(esp_wifi_wps_disable());
      DPRINT("WPS successfull\n");
      WiFi.begin();
      break;

    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_FAILED\n");
      wpsStatus = -1;
      ESP_ERROR_CHECK(esp_wifi_wps_disable());
      DPRINT("WPS failed\n");
      break;

    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT\n");
      wpsStatus = -2;
      ESP_ERROR_CHECK(esp_wifi_wps_disable());
      DPRINT("WPS timeout\n");
      break;

    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_PIN\n");
      DPRINT("WPS_PIN = "PINSTR, PIN2STR(info.sta_er_pin.pin_code));
      break;

    default:
      DPRINT("Event: %d\n", event);
      break;
  }
#endif
}


void status_blink()
{
  WIFI_LED_ON();
  delay(50);
  WIFI_LED_OFF();
}

void ap_mode_start()
{
  WIFI_LED_OFF();

  WiFi.disconnect();  // mandatory after the unsuccessful try to connect to an AP
                      // and before setting up the softAP

  WiFi.mode(WIFI_AP);
  
  if (WiFi.softAP(wifiSoftAP.c_str())) {
    DPRINT("AP %s started\n", wifiSoftAP.c_str());
    // Setup the DNS server redirecting all the domains to the apIP
    //dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    //dnsServer.start(53, "*", apIP);
    //start_services();
  }  
  else
    DPRINT("AP mode failed\n");
}

void ap_mode_stop()
{
  WIFI_LED_OFF();

  if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
    if (WiFi.softAPdisconnect()) {
      DPRINT("AP mode disconnected\n");
    }
    else {
      DPRINT("AP mode disconnected failed\n");
    }
  }
}

bool smart_config()
{
  // Return 'true' if SSID and password received within SMART_CONFIG_TIMEOUT seconds

  // Re-establish lost connection to the AP
  WiFi.setAutoReconnect(true);

  // Automatically connect on power on to the last used access point
  WiFi.setAutoConnect(true);

  // Waiting for SSID and password from from mobile app
  // SmartConfig works only in STA mode
  WiFi.mode(WIFI_STA);
  WiFi.beginSmartConfig();

  DPRINT("SmartConfig started\n");
  display_progress_bar_title("SmartConfig");
  for (int i = 0; i < SMART_CONFIG_TIMEOUT && !WiFi.smartConfigDone(); i++) {
    display_progress_bar_update(i, SMART_CONFIG_TIMEOUT-1);
    status_blink();
    delay(950);
  }
  display_progress_bar_update(1, 1);
  
  if (WiFi.smartConfigDone()) {
    // Wait for WiFi to connect to AP
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    wifiSSID = WiFi.SSID();
    wifiPassword = WiFi.psk();

    DPRINT("SSID        : %s\n", WiFi.SSID().c_str());
    DPRINT("Password    : %s\n", WiFi.psk().c_str());

    save_wifi_credentials(WiFi.SSID(), WiFi.psk());
  }
  else
    DPRINT("SmartConfig timeout\n");

  if (WiFi.smartConfigDone())
  {
    WiFi.stopSmartConfig();
    return true;
  }
  else
  {
    WiFi.stopSmartConfig();
    return false;
  }
}

bool wps_config()
{
  wpsStatus = 0;

  WPS.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
  WPS.wps_type = WPS_TYPE_PBC;
  strcpy(WPS.factory_info.manufacturer, "ESPRESSIF");
  strcpy(WPS.factory_info.model_number, "ESP32");
  strcpy(WPS.factory_info.model_name,   "Pedalino(TM)");
  strcpy(WPS.factory_info.device_name,  "PedalinoMini");
  
  ESP_ERROR_CHECK(esp_wifi_wps_enable(&WPS));
  ESP_ERROR_CHECK(esp_wifi_wps_start(0));
  
  DPRINT("WPS started\n");
  display_progress_bar_title("Press WPS button on AP");
  for (int i = 0; i < 10*WPS_TIMEOUT && wpsStatus == 0; i++) {
    display_progress_bar_update(i, 10*WPS_TIMEOUT-1);
    delay(100);
  }
  display_progress_bar_update(1, 1);

  if (wpsStatus == 1) {
    // Wait for WiFi to connect to AP
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    wifiSSID = WiFi.SSID();
    wifiPassword = WiFi.psk();

    DPRINT("SSID        : %s\n", WiFi.SSID().c_str());
    DPRINT("Password    : %s\n", WiFi.psk().c_str());

    save_wifi_credentials(WiFi.SSID(), WiFi.psk());
  }
  else {
    //ESP_ERROR_CHECK(esp_wifi_wps_disable());
    DPRINT("WPS timeout\n");
  }
  return WiFi.isConnected();
}

//bool ap_connect(String ssid = "", String password = "")
bool ap_connect(String ssid, String password)
{
  // Return 'true' if connected to the access point within WIFI_CONNECT_TIMEOUT seconds

  DPRINT("Connecting to\n");
  DPRINT("SSID        : %s\n", ssid.c_str());
  DPRINT("Password    : %s\n", password.c_str());

  if (ssid.length() == 0) return false;

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  display_progress_bar_title2("Connecting to", ssid);
  for (byte i = 0; i < WIFI_CONNECT_TIMEOUT * 2 && !WiFi.isConnected(); i++) {
    display_progress_bar_update(i, WIFI_CONNECT_TIMEOUT*2-1);
    status_blink();
    delay(100);
    status_blink();
    delay(300);
  }
  display_progress_bar_update(1, 1);

  WiFi.isConnected() ? WIFI_LED_ON() : WIFI_LED_OFF();

  return WiFi.isConnected();
}

//bool auto_reconnect(String ssid = "", String password = "")
bool auto_reconnect(String ssid, String password)
{
  // Return 'true' if connected to the (last used) access point within WIFI_CONNECT_TIMEOUT seconds

  if (ssid.length() == 0) {

#ifdef ARDUINO_ARCH_ESP8266
    ssid = WiFi.SSID();
    password = WiFi.psk();
#endif

#ifdef ARDUINO_ARCH_ESP32
    ssid = wifiSSID;
    password = wifiPassword;
#endif
  }

  return (ssid.length() == 0) ? false : ap_connect(ssid, password);
}

void wifi_connect()
{
  if (!auto_reconnect())       // WIFI_CONNECT_TIMEOUT seconds to reconnect to last used access point
    smart_config();            // SMART_CONFIG_TIMEOUT seconds to receive SmartConfig parameters and connect
  
  if (!WiFi.isConnected())
    wps_config();               // WPS_TIMEOUT seconds to receive WPS parameters and connect

  if (!WiFi.isConnected())
    ap_mode_start();           // switch to AP mode until next reboot
}

#endif  // WIFI