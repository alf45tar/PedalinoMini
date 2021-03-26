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
"MIIEsTCCA5mgAwIBAgIQBOHnpNxc8vNtwCtCuF0VnzANBgkqhkiG9w0BAQsFADBs\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
"ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowcDEL\n"
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
"LmRpZ2ljZXJ0LmNvbTEvMC0GA1UEAxMmRGlnaUNlcnQgU0hBMiBIaWdoIEFzc3Vy\n"
"YW5jZSBTZXJ2ZXIgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2\n"
"4C/CJAbIbQRf1+8KZAayfSImZRauQkCbztyfn3YHPsMwVYcZuU+UDlqUH1VWtMIC\n"
"Kq/QmO4LQNfE0DtyyBSe75CxEamu0si4QzrZCwvV1ZX1QK/IHe1NnF9Xt4ZQaJn1\n"
"itrSxwUfqJfJ3KSxgoQtxq2lnMcZgqaFD15EWCo3j/018QsIJzJa9buLnqS9UdAn\n"
"4t07QjOjBSjEuyjMmqwrIw14xnvmXnG3Sj4I+4G3FhahnSMSTeXXkgisdaScus0X\n"
"sh5ENWV/UyU50RwKmmMbGZJ0aAo3wsJSSMs5WqK24V3B3aAguCGikyZvFEohQcft\n"
"bZvySC/zA/WiaJJTL17jAgMBAAGjggFJMIIBRTASBgNVHRMBAf8ECDAGAQH/AgEA\n"
"MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIw\n"
"NAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2Vy\n"
"dC5jb20wSwYDVR0fBEQwQjBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQuY29t\n"
"L0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENBLmNybDA9BgNVHSAENjA0MDIG\n"
"BFUdIAAwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQ\n"
"UzAdBgNVHQ4EFgQUUWj/kK8CB3U8zNllZGKiErhZcjswHwYDVR0jBBgwFoAUsT7D\n"
"aQP4v0cB1JgmGggC72NkK8MwDQYJKoZIhvcNAQELBQADggEBABiKlYkD5m3fXPwd\n"
"aOpKj4PWUS+Na0QWnqxj9dJubISZi6qBcYRb7TROsLd5kinMLYBq8I4g4Xmk/gNH\n"
"E+r1hspZcX30BJZr01lYPf7TMSVcGDiEo+afgv2MW5gxTs14nhr9hctJqvIni5ly\n"
"/D6q1UEL2tU2ob8cbkdJf17ZSHwD2f2LSaCYJkJA69aSEaRkCldUxPUd1gJea6zu\n"
"xICaEnL6VpPX/78whQYwvwt/Tv9XBZ0k7YXDK/umdaisLRbvfXknsuvCnQsH6qqF\n"
"0wGjIChBWUMo0oHjqvbsezt3tkBigAVBRQHvFwY+3sAzm2fTYS5yh+Rp/BIAV0Ae\n"
"cPUeybQ=\n"
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
  url += F(".bin");
  DPRINT("Starting OTA %s\n", url.c_str());
  HttpsOTA.onHttpEvent(ota_https_update_event_handler);
  HttpsOTA.begin(url.c_str(), rootCACertificate);
}


void get_file_from_cloud(String url, String filename) {

  if (!wifiEnabled) return;

  WiFiClientSecure *client = new WiFiClientSecure;

  if (client) {

    client->setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;

      url += F("?");
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