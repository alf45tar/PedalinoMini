#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HttpsOTAUpdate.h>

#define OTA_PARTITION_SIZE    0x1D0000      // 1900544 bytes

String            latestFirmwareVersion = VERSION;
String            url;
HttpsOTAStatus_t  otaStatus;
unsigned long     otaProgress;


//  To get public SSL certificate of a server, execute the next command:
//  openssl s_client -showcerts -connect raw.githubusercontent.com:443

static const char *rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n"
"MIIEvjCCA6agAwIBAgIQBtjZBNVYQ0b2ii+nVCJ+xDANBgkqhkiG9w0BAQsFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaME8xCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxKTAnBgNVBAMTIERpZ2lDZXJ0IFRMUyBS\n"
"U0EgU0hBMjU2IDIwMjAgQ0ExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n"
"AQEAwUuzZUdwvN1PWNvsnO3DZuUfMRNUrUpmRh8sCuxkB+Uu3Ny5CiDt3+PE0J6a\n"
"qXodgojlEVbbHp9YwlHnLDQNLtKS4VbL8Xlfs7uHyiUDe5pSQWYQYE9XE0nw6Ddn\n"
"g9/n00tnTCJRpt8OmRDtV1F0JuJ9x8piLhMbfyOIJVNvwTRYAIuE//i+p1hJInuW\n"
"raKImxW8oHzf6VGo1bDtN+I2tIJLYrVJmuzHZ9bjPvXj1hJeRPG/cUJ9WIQDgLGB\n"
"Afr5yjK7tI4nhyfFK3TUqNaX3sNk+crOU6JWvHgXjkkDKa77SU+kFbnO8lwZV21r\n"
"eacroicgE7XQPUDTITAHk+qZ9QIDAQABo4IBgjCCAX4wEgYDVR0TAQH/BAgwBgEB\n"
"/wIBADAdBgNVHQ4EFgQUt2ui6qiqhIx56rTaD5iyxZV2ufQwHwYDVR0jBBgwFoAU\n"
"A95QNVbRTLtm8KPiGxvDl7I90VUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQG\n"
"CCsGAQUFBwMBBggrBgEFBQcDAjB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGG\n"
"GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBABggrBgEFBQcwAoY0aHR0cDovL2Nh\n"
"Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNydDBCBgNV\n"
"HR8EOzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRH\n"
"bG9iYWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwCwYJYIZIAYb9bAIBMAcGBWeBDAEB\n"
"MAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEBCwUAA4IB\n"
"AQCAMs5eC91uWg0Kr+HWhMvAjvqFcO3aXbMM9yt1QP6FCvrzMXi3cEsaiVi6gL3z\n"
"ax3pfs8LulicWdSQ0/1s/dCYbbdxglvPbQtaCdB73sRD2Cqk3p5BJl+7j5nL3a7h\n"
"qG+fh/50tx8bIKuxT8b1Z11dmzzp/2n3YWzW2fP9NsarA4h20ksudYbj/NhVfSbC\n"
"EXffPgK2fPOre3qGNm+499iTcc+G33Mw+nur7SpZyEKEOxEXGlLzyQ4UfaJbcme6\n"
"ce1XR2bFuAJKZTRei9AqPCCcUZlM51Ke92sRKw2Sfh3oius2FkOH6ipjv3U/697E\n"
"A7sKPPcw7+uvTPyLNhBzPvOk\n"
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
          DPRINT("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
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

    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            DPRINT("HTTP_EVENT_ERROR\n");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            DPRINT("HTTP_EVENT_ON_CONNECTED\n");
            otaProgress = 0;
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
            otaProgress += event->data_len;
#ifdef TTGO_T_DISPLAY
    display_progress_bar_update(otaProgress, OTA_PARTITION_SIZE);
#else
    display.drawProgressBar(4, 32, 120, 8, otaProgress / (OTA_PARTITION_SIZE / 100) );
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


void ota_http_update()
{
  if (!wifiEnabled) return;

  url = F("https://raw.githubusercontent.com/alf45tar/PedalinoMini/master/firmware/");
  url += xstr(PLATFORMIO_ENV);
  url += F("/");
  url += xstr(PLATFORMIO_ENV);
  url += F("-");
  url += latestFirmwareVersion;
  url += F("-firmware.bin");
  DPRINT("Starting OTA %s\n", url.c_str());
  HttpsOTA.onHttpEvent(ota_https_update_event_handler);
  HttpsOTA.begin(url.c_str(), rootCACertificate);
}


void get_file_from_cloud(const String& url, const String& filename) {

  String unique_url = url;

  if (!wifiEnabled) return;

  WiFiClientSecure *client = new WiFiClientSecure;

  if (client) {

    client->setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;

      unique_url += F("?");
      unique_url += String(rand());

      if (https.begin(*client, unique_url)) {

        DPRINT("[HTTPS] GET... %s\n", url.c_str());
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          DPRINT("[HTTPS] GET... code: %d\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK) {
            DPRINT("Writing %s ... ", filename.c_str());
            File f = SPIFFS.open(filename, "w");
            https.writeToStream(&f);
            f.close();
            DPRINT("done.\n");
          }
        } else {
          DPRINT("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        DPRINT("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  }
}