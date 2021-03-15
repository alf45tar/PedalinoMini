#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HttpsOTAUpdate.h>

static String           latestFirmwareVersion = VERSION;
static String           url;
static HttpsOTAStatus_t otaStatus;

static const char *rootCACertificate = \
  "-----BEGIN CERTIFICATE-----\n"
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
"+OkuE6N36B9K\n"
"-----END CERTIFICATE-----\n";


// Set time via NTP, as required for x.509 validation

void set_clock() {

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

  DPRINT("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    DPRINT(".");
    now = time(nullptr);
  }
  DPRINT("\n");

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  DPRINT("Current time: %s", asctime(&timeinfo));
}


String get_latest_firmware_version(void) {

  String  payload = "";

  if (!wifiEnabled) return payload;

  set_clock();

  WiFiClientSecure *client = new WiFiClientSecure;

  if (client) {

    client->setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;

      url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/firmware/");
      url += xstr(PLATFORMIO_ENV);
      url += F("/version.txt?");
      url += String(rand());

      if (https.begin(*client, url)) {

        DPRINT("[HTTPS] GET... %s\n", url.c_str());
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          DPRINT("[HTTPS] GET... code: %d\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK) {
           payload = https.getString();
            payload.trim();
            if (payload.equals(VERSION)) {
              DPRINT("Device already on latest firmware version: %s\n", VERSION);
            } else {
              DPRINT("New firmware detected: %s\n", payload.c_str());
            }
          }
        } else {
          DPRINT("[HTTP] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        DPRINT("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  }

  return payload;
}


void ota_https_update_event_handler(HttpEvent_t *event) {

    static unsigned int progress;
    static unsigned int total = 2 * 1024 * 1024;

    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            DPRINT("HTTP_EVENT_ERROR\n");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            DPRINT("HTTP_EVENT_ON_CONNECTED\n");
            progress = 0;
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
            break;
        case HTTP_EVENT_HEADER_SENT:
            DPRINT("HTTP_EVENT_HEADER_SENT\n");
            break;
        case HTTP_EVENT_ON_HEADER:
            DPRINT("HTTP_EVENT_ON_HEADER, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            DPRINT("#");
            progress += event->data_len;
#ifdef TTGO_T_DISPLAY
    display_progress_bar_update(progress, total);
#else
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
#endif
            break;
        case HTTP_EVENT_ON_FINISH:
            DPRINT("HTTP_EVENT_ON_FINISH\n");
            break;
        case HTTP_EVENT_DISCONNECTED:
            DPRINT("HTTP_EVENT_DISCONNECTED\n");
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
            break;
    }
}


void ota_http_update(bool spiffs = false)
{
  if (!wifiEnabled) return;

  if (spiffs) {
    url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/firmware/");
    url += xstr(PLATFORMIO_ENV);
    url += F("/");
    url += xstr(PLATFORMIO_ENV);
    url += F("-");
    url += latestFirmwareVersion;
    url += F(".spiffs");
    DPRINT("Starting OTA %s\n", url.c_str());
    HttpsOTA.onHttpEvent(ota_https_update_event_handler);
    HttpsOTA.begin(url.c_str(), rootCACertificate);
  } else {
    url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/firmware/");
    url += xstr(PLATFORMIO_ENV);
    url += F("/");
    url += xstr(PLATFORMIO_ENV);
    url += F("-");
    url += latestFirmwareVersion;
    url += F(".bin");
    DPRINT("Starting OTA %s\n", url.c_str());
    HttpsOTA.onHttpEvent(ota_https_update_event_handler);
    HttpsOTA.begin(url.c_str(), rootCACertificate);
  }
}
