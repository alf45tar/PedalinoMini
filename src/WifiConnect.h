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


#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <esp_wifi.h>
#include <esp_wps.h>
#ifdef BLUFI
#include "BluFi.h"
#endif

#define WIFI_CONNECT_TIMEOUT    15
#define SMART_CONFIG_TIMEOUT    15
#define WPS_TIMEOUT             15

#define WIFI_LED        2
#define WIFI_LED_OFF()  digitalWrite(WIFI_LED, LOW)
#define WIFI_LED_ON()   digitalWrite(WIFI_LED, HIGH)

#ifdef WIFI

void wifi_connect();

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

void stop_services()
{
  MDNS.end();
  ArduinoOTA.end();
  oscUDPin.close();
  oscUDPout.close();
}

void start_services()
{
  stop_services();

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

  // OTA update init
  ota_begin(host.c_str());
  DPRINT("OTA update started\n");

#ifdef WEBCONFIG
  switch (bootMode) {
    case PED_BOOT_WIFI:
    case PED_BOOT_AP_NO_BLE:
      http_setup();
      DPRINT("HTTP server started\n");
      DPRINT("Connect to http://%s.local/update for firmware update\n", host.c_str());
      DPRINT("Connect to http://%s.local for configuration\n", host.c_str());
      break;
    default:
      DPRINT("HTTP server not started. Use 'WiFi Only' or 'AP without BLE' boot mode for web config ui.\n");
      break;
  }
#endif

  // ipMIDI
  ip_midi_start();
  DPRINT("ipMIDI started\n");

  // RTP-MIDI
  apple_midi_start();
  DPRINT("RTP-MIDI started\n");

  // Set incoming OSC messages port
  //oscUDPin.listen(WiFi.localIP(), oscLocalPort);
  oscUDPin.listen(oscLocalPort);
  oscUDPin.onPacket(oscOnPacket);

  DPRINT("OSC server started\n");

  // Calculate the broadcast address of local WiFi to broadcast OSC messages
  oscRemoteIp = WiFi.localIP();
  IPAddress localMask = WiFi.subnetMask();
  for (int i = 0; i < 4; i++)
    oscRemoteIp[i] |= (localMask[i] ^ B11111111);

  // Set outcoming OSC broadcast ip/port
  oscUDPout.connect(oscRemoteIp, oscRemotePort);

  // Connect to Blynk Cloud
  blynk_connect();
}


void WiFiEvent(WiFiEvent_t event, system_event_info_t info)
{
  IPAddress localMask;

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
      WiFi.softAPsetHostname((host + String(".local")).c_str());
      //DPRINT("AP SSID     : %s\n", WiFi.softAPSSID().c_str());
      //DPRINT("AP PSK      : %s\n", WiFi.softAPPSK().c_str());
      //DPRINT("AP SSID     : %s\n", ssidSoftAP.c_str());
      //DPRINT("AP PSK      : %s\n", host.c_str());
      //DPRINT("AP MAC      : %s\n", WiFi.softAPmacAddress().c_str());
      //DPRINT("AP IP       : %s\n", WiFi.softAPIP().toString().c_str());
      //DPRINT("Channel     : %d\n", WiFi.channel());
      //DPRINT("Connect to %s wireless network with no password\n", ssidSoftAP.c_str());
      //start_services();
#ifdef BLUFI
      {
        wifi_mode_t            mode;
        wifi_config_t          config;
        esp_blufi_extra_info_t info;

        esp_wifi_get_mode(&mode);
        if (mode == WIFI_MODE_AP) {
          esp_wifi_get_config(WIFI_IF_AP, &config);
          memset(&info, 0, sizeof(esp_blufi_extra_info_t));
          info.softap_ssid     = config.ap.ssid;
          info.softap_ssid_len = config.ap.ssid_len;
          esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
        } else {
          esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
        }
      }
#endif
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
      if (WiFi.getMode() == WIFI_STA) ESP_ERROR_CHECK(esp_wifi_wps_disable());
      //WiFi.begin();
      ESP_ERROR_CHECK(esp_wifi_connect());
      break;

    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_FAILED\n");
      wpsStatus = -1;
      if (WiFi.getMode() == WIFI_STA) ESP_ERROR_CHECK(esp_wifi_wps_disable());
      break;

    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT\n");
      wpsStatus = -2;
      if (WiFi.getMode() == WIFI_STA) ESP_ERROR_CHECK(esp_wifi_wps_disable());
      break;

    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_PIN\n");
      DPRINT("WPS_PIN = " PINSTR, PIN2STR(info.sta_er_pin.pin_code));
      break;

    case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
      DPRINT("SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP\n");
      break;

#ifdef BLUFI
    case SYSTEM_EVENT_SCAN_DONE: {
      DPRINT("SYSTEM_EVENT_SCAN_DONE\n");
      uint16_t apCount = 0;
      esp_wifi_scan_get_ap_num(&apCount);
      if (apCount == 0) {
        BLUFI_INFO("Nothing AP found");
        break;
      }
      wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
      if (!ap_list) {
        BLUFI_ERROR("malloc error, ap_list is NULL");
        break;
      }
      ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, ap_list));
      esp_blufi_ap_record_t * blufi_ap_list = (esp_blufi_ap_record_t *)malloc(apCount * sizeof(esp_blufi_ap_record_t));
      if (!blufi_ap_list) {
        if (ap_list) {
          free(ap_list);
        }
        BLUFI_ERROR("malloc error, blufi_ap_list is NULL");
        break;
      }
      for (int i = 0; i < apCount; ++i) {
        blufi_ap_list[i].rssi = ap_list[i].rssi;
        memcpy(blufi_ap_list[i].ssid, ap_list[i].ssid, sizeof(ap_list[i].ssid));
      }
      esp_blufi_send_wifi_list(apCount, blufi_ap_list);
      esp_wifi_scan_stop();
      free(ap_list);
      free(blufi_ap_list);
      break;
    }
#endif

    default:
      DPRINT("Event: %d\n", event);
      break;
  }
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

  if (WiFi.softAP(ssidSoftAP.c_str(), passwordSoftAP.c_str())) {
    DPRINT("AP %s started with password %s\n", ssidSoftAP.c_str(), passwordSoftAP.c_str());
    // Setup the DNS server redirecting all the domains to the apIP
    //dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    //dnsServer.start(53, "*", apIP);
    //if (!WiFi.softAPsetHostname(host.c_str())) {
    //  DPRINT("WiFi.softAPsetHostname(%s) failed\n", host.c_str());
    //}
    DPRINT("AP SSID     : %s\n", ssidSoftAP.c_str());
    DPRINT("AP PSK      : %s\n", passwordSoftAP.c_str());
    DPRINT("AP MAC      : %s\n", WiFi.softAPmacAddress().c_str());
    DPRINT("AP IP       : %s\n", WiFi.softAPIP().toString().c_str());
    DPRINT("Channel     : %d\n", WiFi.channel());
    DPRINT("Connect to %s wireless network with password %s\n", ssidSoftAP.c_str(), passwordSoftAP.c_str());
    start_services();
  }
  else
    DPRINT("AP mode failed\n");
}

void ap_mode_stop()
{
  WIFI_LED_OFF();

  stop_services();

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
  leds.setAllLow();
  leds.write();
  for (int i = 0; i < SMART_CONFIG_TIMEOUT && !WiFi.smartConfigDone(); i++) {
    display_progress_bar_update(i, SMART_CONFIG_TIMEOUT-1);
    status_blink();
    delay(950);
    leds.setHigh(map(i, 0, SMART_CONFIG_TIMEOUT-2, 0, 5));
    leds.write();
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

    eeprom_update_sta_wifi_credentials(WiFi.SSID(), WiFi.psk());
  }
  else
    DPRINT("SmartConfig timeout\n");

  if (WiFi.smartConfigDone())
  {
    WiFi.stopSmartConfig();
    leds.kittCar();
    return true;
  }
  else
  {
    WiFi.stopSmartConfig();
    leds.setAllLow();
    leds.write();
    return false;
  }
}

bool wps_config()
{
  wpsStatus = 0;

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

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
  leds.setAllLow();
  leds.write();
  for (int i = 0; i < 10*WPS_TIMEOUT && wpsStatus == 0; i++) {
    display_progress_bar_update(i, 10*WPS_TIMEOUT-1);
    leds.setHigh(map(i, 0, 10*WPS_TIMEOUT-10, 5, 0));
    leds.write();
    delay(100);
  }
  display_progress_bar_update(1, 1);
  leds.setAllLow();
  leds.write();

  if (wpsStatus == 1) {
    // Wait for WiFi to connect to AP
    display_progress_bar_title2("Connecting to", WiFi.SSID());
    for (byte i = 0; i < WIFI_CONNECT_TIMEOUT * 2 && !WiFi.isConnected(); i++) {
      display_progress_bar_update(i, WIFI_CONNECT_TIMEOUT*2-1);
      status_blink();
      delay(100);
      status_blink();
      delay(300);
    }
    display_progress_bar_update(1, 1);

    WiFi.isConnected() ? WIFI_LED_ON() : WIFI_LED_OFF();

    if (WiFi.isConnected()) {
      wifiSSID = WiFi.SSID();
      wifiPassword = WiFi.psk();

      DPRINT("SSID        : %s\n", WiFi.SSID().c_str());
      DPRINT("Password    : %s\n", WiFi.psk().c_str());

      eeprom_update_sta_wifi_credentials(WiFi.SSID(), WiFi.psk());
    }
  }
  else {
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
  leds.setAllLow();
  leds.write();
  for (byte i = 0; i < WIFI_CONNECT_TIMEOUT * 2 && !WiFi.isConnected(); i++) {
    display_progress_bar_update(i, WIFI_CONNECT_TIMEOUT*2-1);
    status_blink();
    delay(100);
    status_blink();
    delay(300);
    leds.setHigh(map(i, 0, WIFI_CONNECT_TIMEOUT*2-2, 0, 5));
    leds.write();
  }
  display_progress_bar_update(1, 1);

  if (WiFi.isConnected()) {
    WIFI_LED_ON();
    leds.setAllHigh();
    leds.write();
    delay(100);
  }
  else WIFI_LED_OFF();
  leds.setAllLow();
  leds.write();

  return WiFi.isConnected();
}

//bool auto_reconnect(String ssid = "", String password = "")
bool auto_reconnect(String ssid, String password)
{
  // Return 'true' if connected to the (last used) access point within WIFI_CONNECT_TIMEOUT seconds

  if (ssid.length() == 0) {
    ssid = wifiSSID;
    password = wifiPassword;
  }

  return (ssid.length() == 0) ? false : ap_connect(ssid, password);
}

void wifi_connect()
{
  if (auto_reconnect())       // WIFI_CONNECT_TIMEOUT seconds to reconnect to last used access point
    return;

#ifndef NOSMARTCONFIG
  if (!WiFi.isConnected())
    smart_config();           // SMART_CONFIG_TIMEOUT seconds to receive SmartConfig parameters and connect
#endif

#ifndef NOWPS
  if (!WiFi.isConnected())
    wps_config();             // WPS_TIMEOUT seconds to receive WPS parameters and connect
#endif

  if (!WiFi.isConnected())
    ap_mode_start();          // switch to AP mode until next reboot
}

#endif  // WIFI