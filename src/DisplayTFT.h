/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2024 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#if (defined ARDUINO_LILYGO_T_DISPLAY) || (defined ARDUINO_LILYGO_T_DISPLAY_S3)
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>

TFT_eSPI display = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
#define DISPLAY_WIDTH   TFT_HEIGHT                                                     // T-Display = 240; T-Display-S3 = 320
#define DISPLAY_HEIGHT  TFT_WIDTH                                                      // T-Display = 135; T-Display-S3 = 170
#define TOP_HEIGHT      24
#define TOP_BLANK       6
#define BOTTOM_HEIGHT   24
#define CENTER_HEIGHT   (DISPLAY_HEIGHT - TOP_HEIGHT - TOP_BLANK - BOTTOM_HEIGHT)      // T-Display =  81; T-Display-S3 = 116

/*
                                          DISPLAY WIDTH
        +---------------------------------------------------------------------------------+
        |                                                                                 |
        |       TOP_HEIGHT        WiFi/Bluetooth/Profile/Battery Level                    |
        |                                                                                 |
        +---------------------------------------------------------------------------------+   D
        |       TOP_BLANK                                                                 |   I
        +---------------------------------------------------------------------------------+   S
        |                                                                                 |   P
        |                                                                                 |   L
        |                                                                                 |   A
        |                                                                                 |   Y
        |                                                                                 |
        |       CENTER_HEIGHT     Main area                                               |   H
        |                                                                                 |   E
        |                                                                                 |   I
        |                                                                                 |   G
        |                                                                                 |   H
        |                                                                                 |   T
        |                                                                                 |
        +---------------------------------------------------------------------------------+
        |                                                                                 |
        |       BOTTOM_HEIGHT     Progress Bar                                            |
        |                                                                                 |
        +---------------------------------------------------------------------------------+
*/


#endif

#include <WiFi.h>
#include "Version.h"
#include "Images.h"
#include "Fonts.h"

bool          uiUpdate = true;

// Default palette for 4 bit colour sprites
enum tft_default_4bit_palette {
  TFT_INDEX_BLACK,    //  0  ^
  TFT_INDEX_BROWN,    //  1  |
  TFT_INDEX_RED,      //  2  |
  TFT_INDEX_ORANGE,   //  3  |
  TFT_INDEX_YELLOW,   //  4  Colours 0-9 follow the resistor colour code!
  TFT_INDEX_GREEN,    //  5  |
  TFT_INDEX_BLUE,     //  6  |
  TFT_INDEX_PURPLE,   //  7  |
  TFT_INDEX_DARKGREY, //  8  |
  TFT_INDEX_WHITE,    //  9  v
  TFT_INDEX_CYAN,     // 10  Blue+green mix
  TFT_INDEX_MAGENTA,  // 11  Blue+red mix
  TFT_INDEX_MAROON,   // 12  Darker red colour
  TFT_INDEX_DARKGREEN,// 13  Darker green colour
  TFT_INDEX_NAVY,     // 14  Darker blue colour
  TFT_INDEX_PINK      // 15
};


void display_clear()
{
  display.fillScreen(TFT_BLACK);
}

void display_progress_bar_title(const String& title)
{
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setFreeFont(&FreeSans18pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title, display.width() / 2, display.height() / 2);
  DPRINT("%s\n", title);
}

void display_progress_bar_title2(const String& title1, const String& title2)
{
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setFreeFont(&FreeSans18pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title2, display.width() / 2, display.height() / 2);
  uint16_t h = display.fontHeight(1);
  display.setFreeFont(&FreeSans9pt7b);
  display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title1, display.width() / 2, display.height() / 2 - h);
  DPRINT("%s %s\n", title1.c_str(), title2.c_str());
}

void display_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress, bool invert = false, const String& label = "")
{
  height += height % 2;

  uint16_t radius = height / 2;
  uint16_t doubleRadius = 2 * radius;
  uint16_t innerRadius = radius - 2;

  TFT_eSprite bar = TFT_eSprite(&display);

  bar.setColorDepth(4);
  bar.createSprite(width, height);

  bar.drawCircleHelper(radius,             radius, radius, 0b00001001, TFT_INDEX_DARKGREY);
  bar.drawCircleHelper(width - 1 - radius, radius, radius, 0b00000110, TFT_INDEX_DARKGREY);
  bar.drawFastHLine(radius - 1, 0,          width - doubleRadius + 1, TFT_INDEX_DARKGREY);
  bar.drawFastHLine(radius - 1, height - 1, width - doubleRadius + 1, TFT_INDEX_DARKGREY);

  uint16_t maxProgressWidth = (width - doubleRadius - 1) * progress / 100;

  if (invert) {
    bar.fillCircle(width - 1 - radius, radius, innerRadius, TFT_INDEX_ORANGE);
    bar.fillRect(width - 1 - radius - maxProgressWidth, 2, maxProgressWidth, height - 4, TFT_INDEX_ORANGE);
    bar.fillCircle(width - 1 - radius - maxProgressWidth, radius, innerRadius, TFT_INDEX_ORANGE);
  }
  else {
    bar.fillCircle(radius, radius, innerRadius, TFT_INDEX_ORANGE);
    bar.fillRect(radius + 1, 2, maxProgressWidth, height - 4, TFT_INDEX_ORANGE);
    bar.fillCircle(radius + maxProgressWidth, radius, innerRadius, TFT_INDEX_ORANGE);
    if (!label.isEmpty()) {
      bar.setFreeFont(&FreeSansBold9pt7b);
      if (maxProgressWidth - 2 < bar.textWidth(label)) {
        //bar.setTextDatum(MC_DATUM);
        //bar.setTextColor(TFT_INDEX_ORANGE, TFT_INDEX_BLACK);
        //bar.drawString(label, width / 2, radius - 2);
      }
      else {
        bar.setTextDatum(MR_DATUM);
        bar.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_ORANGE);
        bar.drawString(label, radius + maxProgressWidth, radius - 2);
      }
    }
  }

  bar.pushSprite(x, y);

  bar.deleteSprite();
}

void display_progress_bar_sprite(TFT_eSprite &bar, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress, bool invert = false)
{
  height += height % 2;

  uint16_t radius = height / 2;
  uint16_t doubleRadius = 2 * radius;
  uint16_t innerRadius = radius - 2;

  bar.drawCircleHelper(x + radius,             y + radius, radius, 0b00001001, 1);
  bar.drawCircleHelper(x + width - 1 - radius, y + radius, radius, 0b00000110, 1);
  bar.drawFastHLine(x + radius - 1, y,              width - doubleRadius + 1, 1);
  bar.drawFastHLine(x + radius - 1, y + height - 1, width - doubleRadius + 1, 1);

  uint16_t maxProgressWidth = (width - doubleRadius - 1) * progress / 100;

  if (invert) {
    bar.fillCircle(x + width - 1 - radius, y + radius, innerRadius, 1);
    bar.fillRect(x + width - 1 - radius - maxProgressWidth, y + 2, maxProgressWidth, height - 4, 1);
    bar.fillCircle(x + width - 1 - radius - maxProgressWidth, y + radius, innerRadius, 1);
  }
  else {
    bar.fillCircle(x + radius, y + radius, innerRadius, 1);
    bar.fillRect(x + radius + 1, y + 2, maxProgressWidth, height - 4, 1);
    bar.fillCircle(x + radius + maxProgressWidth, y + radius, innerRadius, 1);
  }
}

void display_progress_bar_update(unsigned int progress, unsigned int total)
{
  display_progress_bar(0, display.height() / 2 + 8, display.width() - 1, display.height() / 8, 100*progress/total);
}

void display_progress_bar_2_update(unsigned int progress, unsigned int total)
{
  display_progress_bar(0, display.height() - display.height() / 8, display.width() - 1, display.height() / 8, 100*progress/total);
}

void display_progress_bar_2_label(unsigned int label, unsigned int x)
{
  String l(label);

  TFT_eSprite sprite = TFT_eSprite(&display);

  sprite.setColorDepth(1);
  sprite.createSprite(40, 40);

  sprite.setTextSize(1);
  sprite.setFreeFont(&FreeSans9pt7b);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  if (x <= sprite.textWidth(l) / 2) {
    sprite.setTextDatum(BL_DATUM);
    sprite.drawString(l, 0, 0);
    sprite.pushSprite(0, display.height() - display.height() / 8);
  }
  else if (x >= (display.width() - sprite.textWidth(l) / 2)) {
    sprite.setTextDatum(BR_DATUM);
    sprite.drawString(l, 40, 0);
    sprite.pushSprite(display.width() - 1, display.height() - display.height() / 8);
  }
  else {
    sprite.setTextDatum(BC_DATUM);
    sprite.drawString(l, 20, 0);
    sprite.pushSprite(x, display.height() - display.height() / 8);
  }
  //display.drawFastVLine(x, display.height() - display.height() / 8, x, display.height() - 1);

  sprite.deleteSprite();
}

void topOverlay()
{
  // First 24 lines of display

  TFT_eSprite top = TFT_eSprite(&display);

  top.setColorDepth(4);
  top.createSprite(display.width(), 24);

  if (scrollingMode) {
    static unsigned int frame = 0;

    if (frame < 100) {
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
        top.setFreeFont(&FreeSansBold12pt7b);
        top.setTextDatum(MC_DATUM);
        top.drawString(MODEL, top.width() / 2, top.height() / 2);
        top.setTextFont(1);
        top.setTextDatum(BL_DATUM);
        top.drawString("TM", top.width() - 40, top.height() / 2 - 5);
    } else if (frame < 200) {
        top.setFreeFont(&FreeSans12pt7b);
        top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        top.setTextDatum(TL_DATUM);
        top.drawString("Device:", 0, 0);
        top.setFreeFont(&FreeSans12pt7b);
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
        top.setTextDatum(TR_DATUM);
        top.drawString(host, top.width(), 0);
    } else if (frame < 300) {
#ifdef WIFI
        switch (WiFi.getMode()) {
          case WIFI_AP:
          case WIFI_AP_STA:
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
            top.setTextDatum(TL_DATUM);
            top.drawString("AP:", 0, 0);
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
            top.setTextDatum(TR_DATUM);
            top.drawString(ssidSoftAP, top.width(), 0);
            break;
          case WIFI_STA:
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
            top.setTextDatum(TL_DATUM);
            top.drawString("SSID:", 0, 0);
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
            top.setTextDatum(TR_DATUM);
            top.drawString(wifiSSID, top.width(), 0);
            break;
          case WIFI_MODE_MAX:
          case WIFI_MODE_NULL:
            break;
        }
#endif
    } else if (frame < 400) {
#ifdef WIFI
        switch (WiFi.getMode()) {
          case WIFI_AP:
          case WIFI_AP_STA:
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
            top.setTextDatum(TL_DATUM);
            top.drawString("AP IP:", 0, 0);
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
            top.setTextDatum(TR_DATUM);
            top.drawString(WiFi.softAPIP().toString(), top.width(), 0);
            break;
          case WIFI_STA:
            top.setFreeFont(&FreeSans12pt7b);
            top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
            top.setTextDatum(TL_DATUM);
            top.drawString("IP:", 0, 0);
            top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
            top.setTextDatum(TR_DATUM);
            top.setFreeFont(&FreeSans12pt7b);
            top.drawString(WiFi.localIP().toString(), top.width(), 0);
            break;
          case WIFI_MODE_MAX:
          case WIFI_MODE_NULL:
            break;
        }
#endif
    } else if (frame < 600) {
      top.setFreeFont(&FreeSansBold9pt7b);
      top.setTextDatum(MC_DATUM);

#ifdef BLE
      if (bleEnabled) {
        if (bleMidiConnected) {
          top.fillRect(0, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_WHITE);
          top.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
        }
        else {
          top.fillRect(0, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_BLACK);
          top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        }
        top.drawRoundRect(0, 0, display.width() / 4, TOP_HEIGHT, 0, TFT_INDEX_BLACK);
        top.drawRoundRect(0, 0, display.width() / 4, TOP_HEIGHT, 2, TFT_INDEX_BLACK);
        top.drawRoundRect(0, 0, display.width() / 4, TOP_HEIGHT, 4, TFT_INDEX_BLACK);
        top.drawString("BLE", 1 * display.width() / 8, 11);
      }
#endif

#ifdef WIFI
      if (wifiEnabled) {
        if (appleMidiConnected) {
          top.fillRect(1 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_WHITE);
          top.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
        }
        else {
          top.fillRect(1 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_BLACK);
          top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        }
        top.drawRoundRect(1 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 0, TFT_INDEX_BLACK);
        top.drawRoundRect(1 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 2, TFT_INDEX_BLACK);
        top.drawRoundRect(1 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 4, TFT_INDEX_BLACK);
        top.drawString("MIDI",   3 * display.width() / 8, 11);
        if (interfaces[PED_IPMIDI].midiIn || interfaces[PED_IPMIDI].midiOut) {
          top.fillRect(2 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_WHITE);
          top.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
        }
        else {
          top.fillRect(2 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, TFT_BLACK);
          top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        }
        top.drawRoundRect(2 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 0, TFT_INDEX_BLACK);
        top.drawRoundRect(2 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 2, TFT_INDEX_BLACK);
        top.drawRoundRect(2 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 4, TFT_INDEX_BLACK);
        top.drawString("ipMidi", 5 * display.width() / 8, 11);

        if (interfaces[PED_OSC].midiIn    || interfaces[PED_OSC].midiOut) {
          top.fillRect(3 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_WHITE);
          top.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
        }
        else {
          top.fillRect(3 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, TFT_INDEX_BLACK);
          top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        }
        top.drawRoundRect(3 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 0, TFT_INDEX_BLACK);
        top.drawRoundRect(3 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 2, TFT_INDEX_BLACK);
        top.drawRoundRect(3 * display.width() / 4, 0, display.width() / 4, TOP_HEIGHT, 4, TFT_INDEX_BLACK);
        top.drawString("OSC", 7 * display.width() / 8, 11);
      }
#endif
    } else if (frame < 600) {
        top.setFreeFont(&FreeSans12pt7b);
        top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        top.setTextDatum(TL_DATUM);
        top.drawString("Firmware:", 0, 0);
        top.setFreeFont(&FreeSans12pt7b);
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
        top.setTextDatum(TR_DATUM);
        top.drawString(VERSION, top.width(), 0);
    }
    frame = (frame + 1) % 600;
  }
  else
  if ((millis() >= endMillis2) ||
      (millis() < endMillis2 && MTC.getMode() == MidiTimeCode::SynchroNone)) {

    top.fillRect(0, 0, display.width(), 24, TFT_BLACK);
#ifdef WIFI
    if (wifiEnabled) {
      top.fillCircleHelper(1, 22, 22, 0b00000010, 0, TFT_INDEX_DARKGREY);
      top.fillCircleHelper(1, 22, 18, 0b00000010, 0, TFT_INDEX_BLACK);
      top.fillCircleHelper(1, 22, 16, 0b00000010, 0, TFT_INDEX_DARKGREY);
      top.fillCircleHelper(1, 22, 12, 0b00000010, 0, TFT_INDEX_BLACK);
      top.fillCircleHelper(1, 22, 10, 0b00000010, 0, TFT_INDEX_DARKGREY);
      top.fillCircleHelper(1, 22,  6, 0b00000010, 0, TFT_INDEX_BLACK);
      top.fillCircleHelper(1, 22,  4, 0b00000010, 0, TFT_INDEX_DARKGREY);
      if      (wifiLevel < -80) {
        top.fillCircleHelper(1, 22,  4, 0b00000010, 0, TFT_INDEX_RED);
      }
      else if (wifiLevel < -72) {
        top.fillCircleHelper(1, 22, 10, 0b00000010, 0, TFT_INDEX_ORANGE);
        top.fillCircleHelper(1, 22,  6, 0b00000010, 0, TFT_INDEX_BLACK);
        top.fillCircleHelper(1, 22,  4, 0b00000010, 0, TFT_INDEX_ORANGE);
      }
      else if (wifiLevel < -65) {
        top.fillCircleHelper(1, 22, 16, 0b00000010, 0, TFT_INDEX_YELLOW);
        top.fillCircleHelper(1, 22, 12, 0b00000010, 0, TFT_INDEX_BLACK);
        top.fillCircleHelper(1, 22, 10, 0b00000010, 0, TFT_INDEX_YELLOW);
        top.fillCircleHelper(1, 22,  6, 0b00000010, 0, TFT_INDEX_BLACK);
        top.fillCircleHelper(1, 22,  4, 0b00000010, 0, TFT_INDEX_YELLOW);
      }
      else {
        top.fillCircleHelper(1, 22, 22, 0b00000010, 0, TFT_INDEX_DARKGREEN);
        top.fillCircleHelper(1, 22, 18, 0b00000010, 0, TFT_INDEX_BLACK);
        top.fillCircleHelper(1, 22, 16, 0b00000010, 0, TFT_INDEX_DARKGREEN);
        top.fillCircleHelper(1, 22, 12, 0b00000010, 0, TFT_INDEX_BLACK);
        top.fillCircleHelper(1, 22, 10, 0b00000010, 0, TFT_INDEX_DARKGREEN);
        top.fillCircleHelper(1, 22,  6, 0b00000010, 0, TFT_INDEX_BLACK);
        top.fillCircleHelper(1, 22,  4, 0b00000010, 0, TFT_INDEX_DARKGREEN);
      }
      /*
      top.fillTriangle(1, 22, 30, 22, 30, 2, TFT_DARKGREY);
      int level = constrain(wifiLevel, -90, -60);
      if      (level < -80) top.fillTriangle(1, 22, map2(level, -90, -60, 1, 30), 22, map2(level, -90, -60, 1, 30), map2(level, -90, -60, 22, 1), TFT_RED);
      else if (level < -72) top.fillTriangle(1, 22, map2(level, -90, -60, 1, 30), 22, map2(level, -90, -60, 1, 30), map2(level, -90, -60, 22, 1), TFT_ORANGE);
      else if (level < -65) top.fillTriangle(1, 22, map2(level, -90, -60, 1, 30), 22, map2(level, -90, -60, 1, 30), map2(level, -90, -60, 22, 1), TFT_YELLOW);
      else                  top.fillTriangle(1, 22, map2(level, -90, -60, 1, 30), 22, map2(level, -90, -60, 1, 30), map2(level, -90, -60, 22, 1), TFT_DARKGREEN);
      for (byte c = 1; c < 6; c++)
        top.drawFastVLine(1+c*6, 0, 24, TFT_BLACK);
      */
    }
#endif

#ifdef BLE
    if (bleMidiConnected) {
      top.drawFastVLine(36, 1, 22, TFT_INDEX_BLUE);
      top.drawLine(41,  6, 31, 16, TFT_INDEX_BLUE);
      top.drawLine(31,  6, 41, 16, TFT_INDEX_BLUE);
      top.drawLine(36,  1, 41,  6, TFT_INDEX_BLUE);
      top.drawLine(36, 22, 41, 16, TFT_INDEX_BLUE);
     }
#endif

#ifdef BATTERY
    top.drawRoundRect(display.width() - 50, 1, 44, 20, 4, TFT_INDEX_WHITE);
    top.fillRoundRect(display.width() - 6, 7, 4, 8, 2, TFT_INDEX_WHITE);
    top.fillRoundRect(display.width() - 50 + 2, 1 + 2, map2(constrain(batteryVoltage, 3000, 4100), 3100, 4100, 0, 40), 20 - 4, 4, TFT_INDEX_DARKGREEN);
    if (batteryVoltage > 4400) {
      top.fillTriangle(display.width() - 28,  3, display.width() - 34, 11, display.width() - 28, 11, TFT_INDEX_WHITE);
      top.fillTriangle(display.width() - 28, 18, display.width() - 23, 10, display.width() - 28, 10, TFT_INDEX_WHITE);
    }
#endif

    top.setFreeFont(&FreeSansBold9pt7b);
    top.setTextDatum(MC_DATUM);
    switch (currentProfile) {
      case 0:
        top.fillRoundRect(52 + 24*currentProfile, 1, 22, 22, 4, TFT_INDEX_RED);
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_RED);
        top.drawString("A", 52 + 24*currentProfile + 11, 11);
        break;
      case 1:
        top.fillRoundRect(52 + 24*currentProfile, 1, 22, 22, 4, TFT_INDEX_GREEN);
        top.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_GREEN);
        top.drawString("B", 52 + 24*currentProfile + 11, 11);
        break;
      case 2:
        top.fillRoundRect(52 + 24*currentProfile, 1, 22, 22, 4, TFT_INDEX_BLUE);
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLUE);
        top.drawString("C", 52 + TOP_HEIGHT*currentProfile + 11, 11);
        break;
    }
  }

  if (millis() < endMillis2) {
    if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
        MTC.getMode() == MidiTimeCode::SynchroClockSlave) {
      byte signature = 4;
      switch (timeSignature) {
        case PED_TIMESIGNATURE_2_4:
          signature = 2;
          top.drawRoundRect(0 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          top.drawRoundRect(1 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          break;
        case PED_TIMESIGNATURE_3_4:
        case PED_TIMESIGNATURE_3_8:
        case PED_TIMESIGNATURE_6_8:
        case PED_TIMESIGNATURE_9_8:
        case PED_TIMESIGNATURE_12_8:
          signature = 3;
          top.drawRoundRect(0 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          top.drawRoundRect(1 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          top.drawRoundRect(2 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          break;
        case PED_TIMESIGNATURE_4_4:
          signature = 4;
          top.drawRoundRect(0 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          top.drawRoundRect(1 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          top.drawRoundRect(2 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          top.drawRoundRect(3 * top.width() / signature, 0, top.width() / signature - 2, TOP_HEIGHT, 2, TFT_INDEX_WHITE);
          break;
      }
      switch (MTC.getBeat()) {
        case 0:
          if (MTC.isPlaying())
            top.fillRoundRect(0 * top.width() / signature + 4, 4, top.width() / signature - 10, 16, 2, TFT_INDEX_WHITE);
          else
            top.fillRoundRect(0 * top.width() / signature + top.width() / (2 * signature) - 3, top.height() / 2 - 3, 6, 6, 2, TFT_INDEX_WHITE);
          break;
        case 1:
          if (MTC.isPlaying())
            top.fillRoundRect(1 * top.width() / signature + 4, 4, top.width() / signature - 10, 16, 2, TFT_INDEX_WHITE);
          else
            top.fillRoundRect(1 * top.width() / signature + top.width() / (2 * signature) - 3, top.height() / 2 - 3, 6, 6, 2, TFT_INDEX_WHITE);
          break;
        case 2:
          if (MTC.isPlaying())
            top.fillRoundRect(2 * top.width() / signature + 4, 4, top.width() / signature - 10, 16, 2, TFT_INDEX_WHITE);
          else
            top.fillRoundRect(2 * top.width() / signature + top.width() / (2 * signature) - 3, top.height() / 2 - 3, 6, 6, 2, TFT_INDEX_WHITE);
          break;
        case 3:
          if (MTC.isPlaying())
            top.fillRoundRect(3 * top.width() / signature + 4, 4, top.width() / signature - 10, 16, 2, TFT_INDEX_WHITE);
          else
            top.fillRoundRect(3 * top.width() / signature + top.width() / (2 * signature) - 3, top.height() / 2 - 3, 6, 6, 2, TFT_INDEX_WHITE);
          break;
      }
    }
    else if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
             MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
      char buf[12];
      sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
      switch (currentProfile) {
        case 0:
          top.fillRect(0, 0, top.width(), top.height(), TFT_INDEX_RED);
          top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_RED);
          break;
        case 1:
          top.fillRect(0, 0, top.width(), top.height(), TFT_INDEX_GREEN);
          top.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_GREEN);
          break;
        case 2:
          top.fillRect(0, 0, top.width(), top.height(), TFT_INDEX_BLUE);
          top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLUE);
          break;
      }
      top.fillRect(0, 0, top.width(), top.height(), TFT_INDEX_RED);
      top.setFreeFont(&FreeSans12pt7b);
      top.setTextDatum(MC_DATUM);
      top.drawString(buf, top.width() / 2, top.height() / 2 - 1);
      /*
      top.setFreeFont(&FreeSans9pt7b);
      top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK, true);
      top.setTextDatum(TL_DATUM);
      if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster)
        top.drawString("MTC Master", 0, 0);
      else if (MTC.getMode() == MidiTimeCode::SynchroMTCSlave)
        top.drawString("MTC Slave", 0, 0);
      */
    }
  }

  top.pushSprite(0, 0);
  top.deleteSprite();
}

void bottomOverlay()
{
  // Last 24 lines of display

  if (lastUsed == lastUsedPedal && lastUsed != 0xFF && millis() < endMillis2 && lastPedalName[0] != ':') {
    int p;
    switch (m1) {
      case midi::NoteOn:
      case midi::NoteOff:
        rmin = 0;
        rmax = MIDI_RESOLUTION - 1;

      case midi::ControlChange:
        m3 = constrain(m3, rmin, rmax);
        p = map2(m3, rmin, rmax, 0, 100);
        if (lastPedalName[0] == 0)
          display_progress_bar(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, p);
        else
          display_progress_bar(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, p, false, String(m3));
        break;

      case midi::ProgramChange:
        rmin = 0;
        rmax = MIDI_RESOLUTION - 1;
        m3 = constrain(m2, rmin, rmax);
        p = map2(m2, rmin, rmax, 0, 100);
        if (lastPedalName[0] == 0)
          display_progress_bar(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, p);
        else
          display_progress_bar(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, p, false, String(m3));
        break;

      case midi::PitchBend:
        p = map2(((m3 << 7) | m2) + MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MAX, -100, 100);
        if ( p >= 0 ) {
          display_progress_bar(0,                        display.height() - BOTTOM_HEIGHT, display.width() / 2 + 11, BOTTOM_HEIGHT, 0, true);
          display_progress_bar(display.width() / 2 - 12, display.height() - BOTTOM_HEIGHT, display.width() / 2 + 11, BOTTOM_HEIGHT, p);
        }
        else {
          display_progress_bar(display.width() / 2 - 12, display.height() - BOTTOM_HEIGHT, display.width() / 2 + 11, BOTTOM_HEIGHT, 0);
          display_progress_bar(0,                        display.height() - BOTTOM_HEIGHT, display.width() / 2 + 11, BOTTOM_HEIGHT, -p, true);
        }
        break;

      case midi::AfterTouchChannel:
        m3 = constrain(m2, rmin, rmax);
        p = map2(m3, rmin, rmax, 0, 100);
        display_progress_bar(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, p);
        break;

      case midi::Start:
      case midi::Stop:
      case midi::Continue:
        display_progress_bar(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, 0);
        break;
    }
  }
}


void drawFrame1(int16_t x, int16_t y)
{
  if (millis() < endMillis2 && lastPedalName[0] != ':') {
    if (strlen(lastPedalName) != 0 && lastPedalName[strlen(lastPedalName) - 1] == '.') lastPedalName[strlen(lastPedalName) - 1] = 0;
    if (lastPedalName[0] == 0) {
      display.fillRect(0, TOP_HEIGHT, display.width(), TOP_BLANK, TFT_INDEX_BLACK);   // Clear blank area
      TFT_eSprite sprite = TFT_eSprite(&display);
      sprite.setColorDepth(1);
      sprite.createSprite(display.width(), CENTER_HEIGHT);
      sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);
      sprite.fillRect(0, 0, sprite.width(), sprite.height(), TFT_BLACK);
      switch (m1) {
        case midi::InvalidType:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("Bank", sprite.width() / 2 + x, sprite.height() * 9 / 10 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m2), sprite.width() / 2 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          sprite.drawRoundRect(sprite.width() / 2 - sprite.width() * 2 / 10, 0, sprite.width() * 4 / 10, sprite.height() * 9 / 10 - 24, 8, TFT_WHITE);
          display.fillRect(0, display.height() - BOTTOM_HEIGHT, display.width(), BOTTOM_HEIGHT, TFT_BLACK);   // Clear bottom area
          break;
        case midi::NoteOn:
        case midi::NoteOff:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("Note", sprite.width() / 2 + x, sprite.height() * 9 / 10 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m2), sprite.width() / 2 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          sprite.drawRoundRect(sprite.width() / 2 - sprite.width() * 2 / 10, 0, sprite.width() * 4 / 10, sprite.height() * 9 / 10 - 24, 8, TFT_WHITE);
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("Velocity", sprite.width() * 17 / 20 + x, sprite.height() * 9 / 10  + y);
          sprite.setFreeFont(&FreeSans18pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m3), sprite.width() * 17 / 20 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          break;
        case midi::ControlChange:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("CC", sprite.width() / 2 + x, sprite.height() * 9 / 10 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m2), sprite.width() / 2 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          sprite.drawRoundRect(sprite.width() / 2 - sprite.width() * 2 / 10, 0, sprite.width() * 4 / 10, sprite.height() * 9 / 10 - 24, 8, TFT_WHITE);
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("Value", sprite.width() * 17 / 20 + x, sprite.height() * 9 / 10  + y);
          sprite.setFreeFont(&FreeSans18pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m3), sprite.width() * 17 / 20 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          break;
        case midi::ProgramChange:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("PC", sprite.width() / 2 + x, sprite.height() * 9 / 10 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m2), sprite.width() / 2 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          sprite.drawRoundRect(sprite.width() / 2 - sprite.width() * 2 / 10, 0, sprite.width() * 4 / 10, sprite.height() * 9 / 10 - 24, 8, TFT_WHITE);
          break;
        case midi::PitchBend:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("Pitch", sprite.width() / 2 + x, sprite.height() * 9 / 10 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(((m3 << 7) | m2) + MIDI_PITCHBEND_MIN), sprite.width() / 2 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          sprite.drawRoundRect(sprite.width() / 2 - sprite.width() / 4, 0, sprite.width() / 2, sprite.height() * 9 / 10 - 24, 8, TFT_WHITE);
          break;
        case midi::AfterTouchChannel:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(BC_DATUM);
          sprite.drawString("Pressure", sprite.width() / 2 + x, sprite.height() * 9 / 10 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m2), sprite.width() / 2 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
          sprite.drawRoundRect(sprite.width() / 2 - sprite.width() * 2 / 10, 0, sprite.width() * 4 / 10, sprite.height() * 9 / 10 - 24, 8, TFT_WHITE);
          break;
        case midi::Start:
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Start", sprite.width() / 2 + x, sprite.height() / 2 + y);
          break;
        case midi::Stop:
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Stop", sprite.width() / 2 + x, sprite.height() / 2 + y);
          break;
        case midi::Continue:
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Continue", sprite.width() / 2 + x, sprite.height() / 2 + y);
          break;
      }
      if ((m1 != midi::InvalidType) && (m1 != midi::Start) && (m1 != midi::Stop) && (m1 != midi::Continue)) {
        sprite.setFreeFont(&FreeSans9pt7b);
        sprite.setTextDatum(BC_DATUM);
        sprite.drawString("Channel", sprite.width() * 3 / 20 + x, sprite.height() * 9 / 10  + y);
        sprite.setFreeFont(&FreeSans18pt7b);
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(String(m4), sprite.width() * 3 / 20 + x, (sprite.height() * 9 / 10 - 24) / 2 + y);
      }
      sprite.pushSprite(0, TOP_HEIGHT + TOP_BLANK);
      sprite.deleteSprite();
    }
    else {
      String name = lastPedalName;
      display.fillRect(0, TOP_HEIGHT, display.width(), TOP_BLANK, TFT_INDEX_BLACK);   // Clear blank area
      TFT_eSprite sprite = TFT_eSprite(&display);
      sprite.setColorDepth(1);
      sprite.createSprite(display.width(), CENTER_HEIGHT);
      sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);
      sprite.fillRect(0, 0, sprite.width(), sprite.height(), TFT_BLACK);
      switch (m1) {
        case midi::InvalidType:
          // ?????????????
          sprite.drawRect(64-22, 15, 64+24, 15+23, TFT_WHITE);
          sprite.setTextDatum(TC_DATUM);
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.drawString("Bank", 64, 39);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(m2), 64, 14);
          break;
        case midi::NoteOn:
        case midi::NoteOff:
        case midi::ControlChange:
          name.replace(String("###"), String(m3));
        case midi::ProgramChange:
        case midi::AfterTouchChannel:
          name.replace(String("###"), String(m2));
        default:
          sprite.setTextDatum(MC_DATUM);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(name, sprite.width() / 2, sprite.height() / 2);
          break;
      }
      sprite.pushSprite(0, TOP_HEIGHT + TOP_BLANK);
      sprite.deleteSprite();
    }
  }
  else if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
           MTC.getMode() == MidiTimeCode::SynchroClockSlave) {

    TFT_eSprite sprite = TFT_eSprite(&display);
    sprite.setColorDepth(1);
    sprite.createSprite(display.width(), display.height() - TOP_HEIGHT);
    sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);

    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.setTextDatum(ML_DATUM);
    if (MTC.getMode() == MidiTimeCode::SynchroClockMaster)
      sprite.drawString("Master", 0, sprite.height() / 4);
    else if (MTC.getMode() == MidiTimeCode::SynchroClockSlave)
      sprite.drawString("Slave", 0, sprite.height() / 4);

    sprite.setFreeFont(&FreeSans24pt7b);
    sprite.setTextDatum(TC_DATUM);
    sprite.drawString(String(bpm), sprite.width() / 2, 4);
    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString("BPM", sprite.width() / 2, sprite.height() / 2 - 4);
    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.setTextDatum(ML_DATUM);

    sprite.setFreeFont(&FreeSans18pt7b);
    sprite.setTextDatum(MR_DATUM);
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        sprite.drawString("2/4", sprite.width(), sprite.height() / 4);
        break;
      case PED_TIMESIGNATURE_4_4:
        sprite.drawString("4/4", sprite.width(), sprite.height() / 4);
        break;
      case PED_TIMESIGNATURE_3_4:
        sprite.drawString("3/4", sprite.width(), sprite.height() / 4);
        break;
      case PED_TIMESIGNATURE_3_8:
        sprite.drawString("3/8", sprite.width(), sprite.height() / 4);
        break;
      case PED_TIMESIGNATURE_6_8:
        sprite.drawString("6/8", sprite.width(), sprite.height() / 4);
        break;
      case PED_TIMESIGNATURE_9_8:
        sprite.drawString("9/8", sprite.width(), sprite.height() / 4);
        break;
      case PED_TIMESIGNATURE_12_8:
        sprite.drawString("12/8", sprite.width(), sprite.height() / 4);
        break;
    }
    int shift = 0;
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        sprite.drawRoundRect(5 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        sprite.drawRoundRect(7 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        shift = 1;
        break;
      case PED_TIMESIGNATURE_3_4:
      case PED_TIMESIGNATURE_3_8:
      case PED_TIMESIGNATURE_6_8:
      case PED_TIMESIGNATURE_9_8:
      case PED_TIMESIGNATURE_12_8:
        sprite.drawRoundRect(4 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        sprite.drawRoundRect(6 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        sprite.drawRoundRect(8 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        shift = 0;
        break;
      case PED_TIMESIGNATURE_4_4:
        sprite.drawRoundRect(3 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        sprite.drawRoundRect(5 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        sprite.drawRoundRect(7 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        sprite.drawRoundRect(9 * sprite.width() / 10 - 40, sprite.height() - 40, 40, 40, 2, TFT_WHITE);
        shift = -1;
        break;
    }
    switch (MTC.getBeat()) {
      case 0:
        if (MTC.isPlaying())
          sprite.fillRoundRect((4 + shift) * sprite.width() / 10 - 40 + 2, sprite.height() - 40 + 2, 36, 36, 2, TFT_WHITE);
        else
          sprite.fillRoundRect((4 + shift) * sprite.width() / 10 - 40 + 15, sprite.height() - 40 + 15, 10, 10, 2, TFT_WHITE);
        break;
      case 1:
        if (MTC.isPlaying())
          sprite.fillRoundRect((6 + shift) * sprite.width() / 10 - 40 + 2, sprite.height() - 40 + 2, 36, 36, 2, TFT_WHITE);
        else
          sprite.fillRoundRect((6 + shift) * sprite.width() / 10 - 40 + 15, sprite.height() - 40 + 15, 10, 10, 2, TFT_WHITE);
        break;
      case 2:
        if (MTC.isPlaying())
          sprite.fillRoundRect((8 + shift) * sprite.width() / 10 - 40 + 2, sprite.height() - 40 + 2, 36, 36, 2, TFT_WHITE);
        else
          sprite.fillRoundRect((8 + shift) * sprite.width() / 10 - 40 + 15, sprite.height() - 40 + 15, 10, 10, 2, TFT_WHITE);
        break;
      case 3:
        if (MTC.isPlaying())
          sprite.fillRoundRect((10 + shift) * sprite.width() / 10 - 40 + 2, sprite.height() - 40 + 2, 36, 36, 2, TFT_WHITE);
        else
          sprite.fillRoundRect((10 + shift) * sprite.width() / 10 - 40 + 15, sprite.height() - 40 + 15, 10, 10, 2, TFT_WHITE);
        break;
    }
    sprite.pushSprite(0, TOP_HEIGHT);
    sprite.deleteSprite();
  }
  else if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
           MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {

    char buf[12];
    sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
    TFT_eSprite sprite = TFT_eSprite(&display);
    sprite.setColorDepth(1);
    sprite.createSprite(display.width(), display.height() - BOTTOM_HEIGHT);
    sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);
    sprite.setFreeFont(&FreeSans24pt7b);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(buf, sprite.width() / 2, sprite.height() / 2);
    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.setTextDatum(BL_DATUM);
    sprite.drawString("MTC", 0, sprite.height());
    sprite.setTextDatum(BR_DATUM);
    if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster)
      sprite.drawString("Master", sprite.width(), sprite.height());
    else if (MTC.getMode() == MidiTimeCode::SynchroMTCSlave)
      sprite.drawString("Slave", sprite.width(), sprite.height());
    sprite.pushSprite(0, TOP_HEIGHT);
    sprite.deleteSprite();
  }
  else {
    if (scannerActivated) {
        TFT_eSprite sprite = TFT_eSprite(&display);
        sprite.setColorDepth(4);
        sprite.createSprite(POINTS, display.height() - 28);
        sprite.drawRect(0, 3, sprite.width(), sprite.height() - 6, TFT_INDEX_DARKGREY);
        switch (pedals[scanPedal].mode) {
          case PED_ANALOG:
          case PED_ULTRASONIC:
            for (uint16_t i = 0; i < POINTS; i = i + 4) {
              sprite.drawPixel(i, 3 + (sprite.height() - 6) / 2,     TFT_INDEX_DARKGREY);
              sprite.drawPixel(i, 3 + (sprite.height() - 6) / 4,     TFT_INDEX_DARKGREY);
              sprite.drawPixel(i, 3 + (sprite.height() - 6) / 4 * 3, TFT_INDEX_DARKGREY);
            }
            for (uint16_t i = 0; i < POINTS - 1; i++) {
              uint16_t x = (POINTS + i - scanIndex) % POINTS;
              sprite.drawPixel(x, map2(scan[i],          ADC_RESOLUTION - 1, 0, 3, sprite.height() - 4), TFT_INDEX_BLUE);
              sprite.drawPixel(x, map2(scanProcessed[i], ADC_RESOLUTION - 1, 0, 3, sprite.height() - 4), TFT_INDEX_CYAN);
            }
            sprite.setTextColor(TFT_INDEX_CYAN, TFT_INDEX_BLACK);
            sprite.setTextDatum(BC_DATUM);
            sprite.drawString("Processed", sprite.width() / 4, sprite.height() / 8 + 2);
            sprite.setTextColor(TFT_INDEX_BLUE, TFT_INDEX_BLACK);
            sprite.setTextDatum(TC_DATUM);
            sprite.drawString("Raw", sprite.width() / 4, sprite.height() / 8 + 4);
            sprite.setTextColor(TFT_INDEX_RED, TFT_INDEX_BLACK);
            sprite.setTextDatum(MC_DATUM);
            sprite.drawString("1023", sprite.width() / 2, sprite.height() / 2 - (sprite.height() - 8) / 2);
            sprite.drawString("767",  sprite.width() / 2, sprite.height() / 2 - (sprite.height() - 8) / 4);
            sprite.drawString("511",  sprite.width() / 2, sprite.height() / 2);
            sprite.drawString("255",  sprite.width() / 2, sprite.height() / 2 + (sprite.height() - 8) / 4);
            sprite.drawString("0",    sprite.width() / 2, sprite.height() / 2 + (sprite.height() - 8) / 2);
            sprite.setTextColor(TFT_INDEX_BLUE, TFT_INDEX_BLACK);
            sprite.setTextDatum(ML_DATUM);
            sprite.drawString("3.2V", 3,                  sprite.height() / 2 - (sprite.height() - 8) / 2);
            sprite.drawString("2.4V", 3,                  sprite.height() / 2 - (sprite.height() - 8) / 4);
            sprite.drawString("1.6V", 3,                  sprite.height() / 2);
            sprite.drawString("0.8V", 3,                  sprite.height() / 2 + (sprite.height() - 8) / 4);
            sprite.drawString("0V",   3,                  sprite.height() / 2 + (sprite.height() - 8) / 2);
            break;
          case PED_ANALOG_PAD:
          case PED_ANALOG_PAD_MOMENTARY:
            int t = map2(pedals[scanPedal].expZero, ADC_RESOLUTION - 1, 0, 4, sprite.height() - 4);
            int h = map2(pedals[scanPedal].expMax,  ADC_RESOLUTION - 1, 0, 4, sprite.height() - 4);
            for (uint16_t i = 1; i < pedals[scanPedal].scanCycles; i++)
              sprite.drawLine(i, 4, i, sprite.height() - 4, TFT_INDEX_DARKGREEN);
            for (uint16_t i = pedals[scanPedal].scanCycles; i < pedals[scanPedal].holdCycles; i++)
              sprite.drawLine(i, 4, i, sprite.height() - 4, TFT_INDEX_ORANGE);
            for (uint16_t i = 0; i < POINTS; i++) {
              if (i < pedals[scanPedal].holdCycles) {
                sprite.drawLine(i, map2(scan[i], ADC_RESOLUTION - 1, 0, 3, sprite.height() - 4), i, sprite.height() - 4, (i <= pedals[scanPedal].scanCycles) ? TFT_INDEX_GREEN : (i <= pedals[scanPedal].holdCycles) ? TFT_INDEX_YELLOW : TFT_INDEX_RED);
              }
              else {
                int q = map2(scan[i], ADC_RESOLUTION - 1, 0, 3, sprite.height() - 4);
                sprite.drawLine(i, max(q, t), i, sprite.height() - 4, TFT_INDEX_DARKGREY);
                if (q <= t) sprite.drawLine(i, q, i, t, TFT_INDEX_RED);
              }
            }
            sprite.drawLine(0, t, sprite.width() - 2, t, TFT_INDEX_CYAN);
            sprite.setTextColor(TFT_INDEX_CYAN, TFT_INDEX_BLACK);
            sprite.setTextDatum(MR_DATUM);
            sprite.drawString("Threshold", sprite.width() - 2, t);
            sprite.drawLine(0, h, sprite.width() - 2, h, TFT_INDEX_BLUE);
            sprite.setTextColor(TFT_INDEX_BLUE, TFT_INDEX_BLACK);
            sprite.setTextDatum(MR_DATUM);
            sprite.drawString("High Cut", sprite.width() - 2, h);
            sprite.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
            sprite.setTextDatum(MC_DATUM);
            sprite.drawString("SCAN",  pedals[scanPedal].scanCycles / 2 - 1,                                                                 sprite.height() / 2 - (sprite.height() - 8) / 2);
            sprite.drawString("HOLD",  pedals[scanPedal].scanCycles + (pedals[scanPedal].holdCycles - pedals[scanPedal].scanCycles) / 2 + 1, sprite.height() / 2 - (sprite.height() - 8) / 2);
            sprite.drawString("DECAY", pedals[scanPedal].holdCycles + (sprite.width() - pedals[scanPedal].holdCycles) / 2 - 1,               sprite.height() / 2 - (sprite.height() - 8) / 2);
            break;
        }
        sprite.pushSprite(0, 28);
        sprite.deleteSprite();
    }
    else if (scrollingMode) {
#ifdef DIAGNOSTIC
      TFT_eSprite sprite = TFT_eSprite(&display);

      sprite.setColorDepth(4);
      sprite.createSprite(POINTS, display.height() - 28);
      sprite.drawRect(0, 3, sprite.width(), sprite.height() - 6, TFT_INDEX_DARKGREY);
      for (uint16_t i = 0; i < POINTS; i = i + 4) {
        sprite.drawPixel(i, 3 + (sprite.height() - 6) / 2,     TFT_INDEX_DARKGREY);
        sprite.drawPixel(i, 3 + (sprite.height() - 6) / 4,     TFT_INDEX_DARKGREY);
        sprite.drawPixel(i, 3 + (sprite.height() - 6) / 4 * 3, TFT_INDEX_DARKGREY);
      }
      for (uint16_t i = 3; i < (sprite.height() - 3); i = i + 6) {
        sprite.drawPixel(sprite.width() / 2,     i, TFT_INDEX_DARKGREY);
        sprite.drawPixel(sprite.width() / 4,     i, TFT_INDEX_DARKGREY);
        sprite.drawPixel(sprite.width() / 4 * 3, i, TFT_INDEX_DARKGREY);
      }
      sprite.setTextFont(1);
      sprite.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
      sprite.setTextDatum(ML_DATUM);
      String label0 = "";
      String label1 = "-";
      String label2 = "-";
      String label3 = "-";
      String label4 = "-";
      if (GRAPH_DURATION_QUARTER_HOUR > 0 && (GRAPH_DURATION_QUARTER_MIN % 60) == 0) {
        label0 += String(0*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label1 += String(1*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label2 += String(2*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label3 += String(3*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label4 += String(4*GRAPH_DURATION_QUARTER_HOUR) + "h";
      }
      else if (GRAPH_DURATION_QUARTER_MIN > 0 && (GRAPH_DURATION_QUARTER_SEC % 60) == 0) {
        label0 += String(0*GRAPH_DURATION_QUARTER_MIN) + "m";
        label1 += String(1*GRAPH_DURATION_QUARTER_MIN) + "m";
        label2 += String(2*GRAPH_DURATION_QUARTER_MIN) + "m";
        label3 += String(3*GRAPH_DURATION_QUARTER_MIN) + "m";
        label4 += String(4*GRAPH_DURATION_QUARTER_MIN) + "m";
      }
      else if (GRAPH_DURATION_QUARTER_SEC > 0) {
        label0 += String(0*GRAPH_DURATION_QUARTER_SEC) + "s";
        label1 += String(1*GRAPH_DURATION_QUARTER_SEC) + "s";
        label2 += String(2*GRAPH_DURATION_QUARTER_SEC) + "s";
        label3 += String(3*GRAPH_DURATION_QUARTER_SEC) + "s";
        label4 += String(4*GRAPH_DURATION_QUARTER_SEC) + "s";
      }
      sprite.drawString(label4, 0, sprite.height() / 4);
      sprite.drawString(label4, 0, sprite.height() / 4 * 3 - 3);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString(label3, sprite.width() / 4, sprite.height() / 4);
      sprite.drawString(label3, sprite.width() / 4, sprite.height() / 4 * 3 - 3);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString(label2, sprite.width() / 2, sprite.height() / 4);
      sprite.drawString(label2, sprite.width() / 2, sprite.height() / 4 * 3 - 3);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString(label1, sprite.width() / 4 * 3, sprite.height() / 4);
      sprite.drawString(label1, sprite.width() / 4 * 3, sprite.height() / 4 * 3 - 3);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString(label0, sprite.width() - 1, sprite.height() / 4);
      sprite.drawString(label0, sprite.width() - 1, sprite.height() / 4 * 3 - 3);

      for (uint16_t i = 0; i < POINTS; i++) {
        uint16_t h  = (historyStart + i) % POINTS;
        uint16_t h1 = (h + 1)            % POINTS;
        if (memoryHistory[h]        != 0 && memoryHistory[h1]        != 0) sprite.drawLine(i, map2(memoryHistory[h],        100, 0, 3, sprite.height() - 3), i + 1, map2(memoryHistory[h1],        100, 0, 3, sprite.height() - 3), TFT_INDEX_RED);
        if (fragmentationHistory[h] != 0 && fragmentationHistory[h1] != 0) sprite.drawLine(i, map2(fragmentationHistory[h], 100, 0, 3, sprite.height() - 3), i + 1, map2(fragmentationHistory[h1], 100, 0, 3, sprite.height() - 3), TFT_INDEX_YELLOW);
        if (wifiHistory[h]          != 0 && wifiHistory[h1]          != 0) sprite.drawLine(i, map2(wifiHistory[h],          100, 0, 3, sprite.height() - 3), i + 1, map2(wifiHistory[h1],          100, 0, 3, sprite.height() - 3), TFT_INDEX_BLUE);
        if (batteryHistory[h]       != 0 && batteryHistory[h1]       != 0) sprite.drawLine(i, map2(batteryHistory[h],       100, 0, 3, sprite.height() - 3), i + 1, map2(batteryHistory[h1],       100, 0, 3, sprite.height() - 3), TFT_INDEX_GREEN);
      }

      sprite.setTextFont(1);
      sprite.setTextColor(TFT_INDEX_BLUE, TFT_INDEX_BLACK);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("-10dB", 0, sprite.height() / 2 - (sprite.height() - 8) / 2);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("-50dB", 0, sprite.height() / 2);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("-90dB", 0, sprite.height() / 2 + (sprite.height() - 8) / 2);
      sprite.setTextColor(TFT_INDEX_RED, TFT_INDEX_BLACK);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString("200Kb", sprite.width() / 2, sprite.height() / 2 - (sprite.height() - 8) / 2);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString("100Kb", sprite.width() / 2, sprite.height() / 2);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString("0Kb",  sprite.width() / 2, sprite.height() / 2 + (sprite.height() - 8) / 2);
      sprite.setTextColor(TFT_INDEX_GREEN, TFT_INDEX_BLACK);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString("5.0V", sprite.width() - 1, sprite.height() / 2 - (sprite.height() - 8) / 2);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString("4.0V", sprite.width() - 1, sprite.height() / 2);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString("3.0V", sprite.width() - 1, sprite.height() / 2 + (sprite.height() - 8) / 2);
      sprite.pushSprite(0, 27);
      sprite.deleteSprite();
#endif
    }
    else {
      if (banknames[currentBank][0] == 0) {
        String p;

        display.fillRect(0, TOP_HEIGHT, display.width(), TOP_BLANK, TFT_INDEX_BLACK);   // Clear blank area

        TFT_eSprite sprite = TFT_eSprite(&display);

        sprite.setColorDepth(1);
        sprite.createSprite(display.width(), display.height() - TOP_HEIGHT - TOP_BLANK);
        sprite.setFreeFont(&DSEG14_Classic_Bold_100);
        switch (currentProfile) {
          case 0:
            p = "A." + ((currentBank > 9  ? "" : "0") + String(currentBank));
            sprite.setBitmapColor(TFT_RED, TFT_BLACK);
            break;
          case 1:
            p = "B." + ((currentBank > 9  ? "" : "0") + String(currentBank));
            sprite.setBitmapColor(TFT_GREEN, TFT_BLACK);
            break;
          case 2:
            p = "C." + ((currentBank > 9  ? "" : "0") + String(currentBank));
            sprite.setBitmapColor(TFT_BLUE, TFT_BLACK);
            break;
          default:
            p = "" + ((currentBank > 9  ? "" : "0") + String(currentBank));
            sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);
            break;
        }
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(p, sprite.width() / 2, sprite.height() / 2);
        sprite.pushSprite(0, TOP_HEIGHT + TOP_BLANK);
        sprite.deleteSprite();
      }
      else {
        //const byte pedals2 = PEDALS / 2;
        String name;
        int offsetText       = 0;
        int offsetBackground = 0;
        static unsigned long ms = millis();

        display.fillRect(0, TOP_HEIGHT, display.width(), TOP_BLANK, TFT_INDEX_BLACK);   // Clear blank area
        TFT_eSprite sprite = TFT_eSprite(&display);
        sprite.setColorDepth(1);
        sprite.createSprite(display.width(), display.height() - TOP_HEIGHT - TOP_BLANK);
        sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);

        // Display pedals name
        sprite.setFreeFont(&FreeSans9pt7b);
        for (byte p = 0; p < PEDALS/2; p++) {
          // Top line
          switch (p) {
            case 0:
              sprite.setTextDatum(TL_DATUM);
              offsetText = 1;
              offsetBackground = 0;
              break;
            case PEDALS / 2 - 1:
              sprite.setTextDatum(TR_DATUM);
              offsetText = -1;
              offsetBackground = 2;
              break;
            default:
              sprite.setTextDatum(TC_DATUM);
              offsetText = 0;
              offsetBackground = 1;
              break;
          }
          name = String((banks[currentBank][p].pedalName[0] == ':') ? &banks[currentBank][p].pedalName[1] : banks[currentBank][p].pedalName);
          name.replace(String("###"), String(currentMIDIValue[currentBank][p][0]));
          if (IS_SINGLE_PRESS_ENABLED(pedals[p].pressMode) && currentMIDIValue[currentBank][p][0] == banks[currentBank][p].midiValue2) {
            sprite.fillRect((sprite.width() / (PEDALS / 2 - 1)) * p - offsetBackground * display.textWidth(name) / 2 + offsetText + x,
                              0 + y,
                              display.textWidth(name) + 1,
                              20, 1);
            sprite.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
          }
          else
            sprite.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
          sprite.drawString(name, (sprite.width() / (PEDALS / 2 - 1)) * p + offsetText + x, 0 + y);
          // Bottom line
          switch (p) {
            case 0:
              sprite.setTextDatum(BL_DATUM);
              offsetText = 1;
              offsetBackground = 0;
              break;
            case PEDALS / 2 - 1:
              sprite.setTextDatum(BR_DATUM);
              offsetText = -1;
              offsetBackground = 2;
              break;
            default:
              sprite.setTextDatum(BC_DATUM);
              offsetText = 0;
              offsetBackground = 1;
              break;
          }
          name = String((banks[currentBank][p + PEDALS / 2].pedalName[0] == ':') ? &banks[currentBank][p + PEDALS / 2].pedalName[1] : banks[currentBank][p + PEDALS / 2].pedalName);
          name.replace(String("###"), String(currentMIDIValue[currentBank][p + PEDALS / 2][0]));
          if (IS_SINGLE_PRESS_ENABLED(pedals[p + PEDALS / 2].pressMode) && currentMIDIValue[currentBank][p + PEDALS / 2][0] == banks[currentBank][p + PEDALS / 2].midiValue2) {
            sprite.fillRect((sprite.width() / (PEDALS / 2 - 1)) * p - offsetBackground * display.textWidth(name) / 2 + offsetText + x,
                              sprite.height() - 20 + y,
                              display.textWidth(name) + 1,
                              20, 1);
            sprite.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
          }
          else
            sprite.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
          sprite.drawString(name, (sprite.width() / (PEDALS / 2 - 1)) * p + offsetText + x, sprite.height() - 1 + y);
          sprite.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
        }

        // Center area
        if (((millis() - ms < 4000) && (banknames[currentBank][0] != '.')) || (banknames[currentBank][0] == ':')) {
          // Display bank name
          //sprite.drawRect(0, 20, sprite.width(), sprite.height() - 42, 1);
          name = (banknames[currentBank][0] == ':') ? &banknames[currentBank][1] : banknames[currentBank];
          name.replace(String("##"), String(currentBank));
          sprite.setFreeFont(&FreeSansBold18pt7b);
          sprite.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(name, sprite.width() / 2, sprite.height() / 2);
        }
        else if (((millis() - ms < 8000) || (banknames[currentBank][0] == '.')) && (banknames[currentBank][0] != ':')) {
          // Display pedal values
          /*
          name = (banknames[currentBank][0] == '.') ? &banknames[currentBank][1] : banknames[currentBank];
          name.replace(String("##"), String(currentBank));
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(TR_DATUM);
          sprite.drawString(name, sprite.width() - 1 + x, y);
          */
          for (byte p = 0; p < PEDALS/2; p++) {
            if (IS_SINGLE_PRESS_ENABLED(pedals[p].pressMode) && (banks[currentBank][p].midiMessage != PED_EMPTY)) {
              display_progress_bar_sprite(sprite, (sprite.width() / (PEDALS / 2)) * p + 2 + x, 25 + y, 55, 20, constrain(map2(currentMIDIValue[currentBank][p][0],
                                                                                                       banks[currentBank][p].midiValue1,
                                                                                                       banks[currentBank][p].midiValue2,
                                                                                                       0, 100),
                                                                                                   0, 100));
            }
            if (IS_SINGLE_PRESS_ENABLED(pedals[p + PEDALS / 2].pressMode) && (banks[currentBank][p + PEDALS / 2].midiMessage != PED_EMPTY)) {
              display_progress_bar_sprite(sprite, (sprite.width() / (PEDALS / 2)) * p + 2 + x, sprite.height() - 22 - 25 + y, 55, 20, constrain(map2(currentMIDIValue[currentBank][p + PEDALS / 2][0],
                                                                                                       banks[currentBank][p + PEDALS / 2].midiValue1,
                                                                                                       banks[currentBank][p + PEDALS / 2].midiValue2,
                                                                                                       0, 100),
                                                                                                   0, 100));
            }
          }
        }
        else {
          ms = millis();
        }
        sprite.pushSprite(0, TOP_HEIGHT + TOP_BLANK);
        sprite.deleteSprite();
      }
    }
  }

#ifdef WEBSOCKET
  events.send(MTC.isPlaying() ? "1" : "0", "play");

  if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
      MTC.getMode() == MidiTimeCode::SynchroClockSlave)  {
    char buf[4];
    events.send("", "mtc");
    sprintf(buf, "%3d", bpm);
    events.send(buf, "bpm");
    sprintf(buf, "%d", MTC.getBeat() + 1);
    events.send(buf, "beat");
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        events.send("2/4", "timesignature");
        break;
      case PED_TIMESIGNATURE_4_4:
        events.send("4/4", "timesignature");
        break;
      case PED_TIMESIGNATURE_3_4:
        events.send("3/4", "timesignature");
        break;
      case PED_TIMESIGNATURE_3_8:
        events.send("3/8", "timesignature");
        break;
      case PED_TIMESIGNATURE_6_8:
        events.send("6/8", "timesignature");
        break;
      case PED_TIMESIGNATURE_9_8:
        events.send("9/8", "timesignature");
        break;
      case PED_TIMESIGNATURE_12_8:
        events.send("12/8", "timesignature");
        break;
    }
  }

  if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
      MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
    char buf[12];
    sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
    events.send(buf, "mtc");
    events.send("", "bpm");
    events.send("", "beat");
    events.send("", "timesignature");
  }
#endif
}

void drawFrame2(int16_t x, int16_t y)
{
  if (!scrollingMode || MTC.isPlaying() || MTC.getMode() != PED_MTC_NONE || millis() < endMillis2) return;

  display.fillRect(0, TOP_HEIGHT, display.width(), display.height() - TOP_HEIGHT - BOTTOM_HEIGHT, TFT_BLACK);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_DARKGREY, TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString("Device:", 0 + x, TOP_HEIGHT + TOP_BLANK + y);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  display.drawString(host, display.width() + x, TOP_HEIGHT + TOP_BLANK + y);
#ifdef WIFI
  switch (WiFi.getMode()) {
    case WIFI_AP:
    case WIFI_AP_STA:
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_DARKGREY, TFT_BLACK);
      display.setTextDatum(TL_DATUM);
      display.drawString( "AP:", 0 + x, TOP_HEIGHT + TOP_BLANK + y);
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_WHITE, TFT_BLACK);
      display.setTextDatum(TR_DATUM);
      display.drawString(ssidSoftAP, display.width() + x, TOP_HEIGHT + TOP_BLANK + y);
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_DARKGREY, TFT_BLACK);
      display.setTextDatum(TL_DATUM);
      display.drawString("AP IP:", 0 + x, 58 + y);
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_WHITE, TFT_BLACK);
      display.setTextDatum(TR_DATUM);
      display.drawString(WiFi.softAPIP().toString(), display.width() + x, 58 + y);
      break;
    case WIFI_STA:
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_DARKGREY, TFT_BLACK);
      display.setTextDatum(TL_DATUM);
      display.drawString("SSID:", 0 + x, 58 + y);
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_WHITE, TFT_BLACK);
      display.setTextDatum(TR_DATUM);
      display.drawString(wifiSSID, display.width() + x, 58 + y);
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_DARKGREY, TFT_BLACK);
      display.setTextDatum(TL_DATUM);
      display.drawString("IP:", 0 + x, 86 + y);
      display.setTextColor(TFT_WHITE, TFT_BLACK);
      display.setTextDatum(TR_DATUM);
      display.setFreeFont(&FreeSans12pt7b);
      display.drawString(WiFi.localIP().toString(), display.width() + x, 86 + y);
      break;
    case WIFI_MODE_MAX:
    case WIFI_MODE_NULL:
      break;
  }
#endif
}

void drawFrame3(int16_t x, int16_t y)
{
  if (!scrollingMode || MTC.isPlaying() || MTC.getMode() != PED_MTC_NONE || millis() < endMillis2) return;

  display.fillRect(0, TOP_HEIGHT, display.width(), display.height() - TOP_HEIGHT -BOTTOM_HEIGHT, TFT_BLACK);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_DARKGREY, TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString("Free heap:", 0 + x, TOP_HEIGHT + TOP_BLANK + y);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  display.drawString(ESP.getFreeHeap()/1024 + String(" Kb"), display.width() + x, TOP_HEIGHT + TOP_BLANK + y);

#ifdef BATTERY
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_DARKGREY, TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString("Battery:", 0 + x, 58 + y);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  display.drawString(batteryVoltage / 1000.0F + String(" V"), display.width()+ x, 58 + y);
#endif

  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_DARKGREY, TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString("Uptime:", 0 + x, 86 + y);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  unsigned long sec = (millis() / 1000) % 60;
  unsigned long min = (millis() / 1000 / 60) % 60;
  unsigned long h   = (millis() / 1000 / 3600);
  display.drawString(h + String("h ") + min + String("m ") + sec + String("s"), display.width() + x, 86 + y);
}


void display_init()
{
    display.init();
    flipScreen ? display.setRotation(3) : display.setRotation(1);
    display.fillScreen(TFT_BLACK);
    /*
    if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file ARDUINO_LILYGO_T_DISPLAY.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file ARDUINO_LILYGO_T_DISPLAY.h
    }
    */

    display.fillScreen(TFT_WHITE);
    display.setSwapBytes(true);
    display.pushImage((display.width() - PEDALINO_LOGO_WIDTH) / 2, (display.height() - PEDALINO_LOGO_HEIGHT) / 2, PEDALINO_LOGO_WIDTH, PEDALINO_LOGO_HEIGHT, PedalinoLogo);
    delay(1000);

#ifdef WIFI
  if (wifiEnabled) {
    display.fillScreen(TFT_WHITE);
    display.setSwapBytes(true);
    display.pushImage((display.width() - WIFI_LOGO_WIDTH) / 2, (display.height() - WIFI_LOGO_HEIGHT) / 2, WIFI_LOGO_WIDTH, WIFI_LOGO_HEIGHT, WiFiLogo);
    delay(1000);
  }
#endif

#ifdef BLE
  if (bleEnabled) {
    display.fillScreen(TFT_WHITE);
    display.setSwapBytes(true);
    display.pushImage((display.width() - BLUETOOTH_LOGO_WIDTH) / 2, (display.height() - BLUETOOTH_LOGO_HEIGHT) / 2, BLUETOOTH_LOGO_WIDTH, BLUETOOTH_LOGO_HEIGHT, BluetoothLogo);
    delay(1000);
  }
#endif

    display.fillScreen(TFT_BLACK);
    displayInit = false;
}

void display_ui_update_disable()
{
  uiUpdate = false;
}

void display_ui_update_enable()
{
  uiUpdate = true;
}

void display_off()
{
  digitalWrite(TFT_BL, !TFT_BACKLIGHT_ON);

  display.writecommand(TFT_DISPOFF);
  display.writecommand(TFT_SLPIN);
}

void display_on()
{
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

  display.writecommand(TFT_DISPON);
  display.writecommand(TFT_SLPOUT);
}

void display_update()
{
  static bool off = false;

  if (interruptCounter3 > 0) {

    interruptCounter3 = 0;

    if (displayInit) display_init();

    if (uiUpdate && !reloadProfile) {
      topOverlay();
      drawFrame1(0, 0);
      bottomOverlay();
    }

    displayOff = screenSaverTimeout == 0 ? false : ((millis() - displayOffCountdownStart) > screenSaverTimeout);

    if (!off && displayOff) { display_off(); leds_off();     off = true;  }
    if (off && !displayOff) { display_on();  leds_refresh(); off = false; }
  }
}