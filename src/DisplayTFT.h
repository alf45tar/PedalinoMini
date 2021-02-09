/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2021 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#ifdef TTGO_T_DISPLAY
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>

TFT_eSPI    display  = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
#define DISPLAY_WIDTH   display.width()
#define DISPLAY_HEIGHT  display.height()

#endif

#include <WiFi.h>
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

void display_progress_bar_title(String title)
{
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setFreeFont(&FreeSans18pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title, display.width() / 2, display.height() / 2);
}

void display_progress_bar_title2(String title1, String title2)
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

}

void display_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress, bool invert = false)
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
  const String l(label);

  display.setTextSize(1);
  display.setFreeFont(&FreeSans9pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  if (x <= display.textWidth(l) / 2) {
    display.setTextDatum(BL_DATUM);
    display.drawString(l, 0, display.height() - display.height() / 8);
  }
  else if (x >= (display.width() - display.textWidth(l) / 2)) {
    display.setTextDatum(BR_DATUM);
    display.drawString(l, display.width() + 1, display.height() - display.height() / 8);
  }
  else {
    display.setTextDatum(BC_DATUM);
    display.drawString(l, x, display.height() - display.height() / 8);
  }
  display.drawFastVLine(x, display.height() - display.height() / 8, x, display.height() - 1);
}


void topOverlay()
{
  TFT_eSprite top = TFT_eSprite(&display);

  top.setColorDepth(4);
  top.createSprite(display.width(), 24);

  if (scrollingMode) {
    static byte frame = 0;

    if (frame < 40) {
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
        top.setFreeFont(&FreeSansBold12pt7b);
        top.setTextDatum(MC_DATUM);
        top.drawString(MODEL, top.width() / 2, top.height() / 2);
        top.setTextFont(1);
        top.setTextDatum(BL_DATUM);
        top.drawString("TM", top.width() - 40, top.height() / 2 - 5);
    } else if (frame < 80) {
        top.setFreeFont(&FreeSans12pt7b);
        top.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
        top.setTextDatum(TL_DATUM);
        top.drawString("Device:", 0, 0);
        top.setFreeFont(&FreeSans12pt7b);
        top.setTextColor(TFT_INDEX_WHITE, TFT_INDEX_BLACK);
        top.setTextDatum(TR_DATUM);
        top.drawString(host, top.width(), 0);
    } else if (frame < 120) {
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
    } else if (frame < 160) {
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
    }
    frame= (frame + 1) % (5*8*4);
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
      if      (level < -80) top.fillTriangle(1, 22, map(level, -90, -60, 1, 30), 22, map(level, -90, -60, 1, 30), map(level, -90, -60, 22, 1), TFT_RED);
      else if (level < -72) top.fillTriangle(1, 22, map(level, -90, -60, 1, 30), 22, map(level, -90, -60, 1, 30), map(level, -90, -60, 22, 1), TFT_ORANGE);
      else if (level < -65) top.fillTriangle(1, 22, map(level, -90, -60, 1, 30), 22, map(level, -90, -60, 1, 30), map(level, -90, -60, 22, 1), TFT_YELLOW);
      else                  top.fillTriangle(1, 22, map(level, -90, -60, 1, 30), 22, map(level, -90, -60, 1, 30), map(level, -90, -60, 22, 1), TFT_DARKGREEN);
      for (byte c = 1; c < 6; c++)
        top.drawFastVLine(1+c*6, 0, 24, TFT_BLACK);
      */
      /*
      top.setSwapBytes(true);
      if      (wifiLevel < -90) top.pushImage(1, 1, 32, 22, w0);
      else if (wifiLevel < -80) top.pushImage(1, 1, 32, 22, w25);
      else if (wifiLevel < -70) top.pushImage(1, 1, 32, 22, w50);
      else if (wifiLevel < -60) top.pushImage(1, 1, 32, 22, w75);
      else                   top.pushImage(1, 1, 32, 22, w100);
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
    /*
    top.setSwapBytes(true);
    if (bleMidiConnected)
      if (wifiEnabled) top.pushImage(40, 1, 16, 22, bt);
      else top.pushImage(1, 1, 16, 22, bt);
    else if (wifiEnabled) top.fillRect(40, 1, 16, 22, TFT_BLACK);
      else top.fillRect(1, 1, 16, 22, TFT_BLACK);
    */
#endif

#ifdef BATTERY
    top.drawRoundRect(display.width() - 50, 1, 44, 20, 4, TFT_INDEX_WHITE);
    top.fillRoundRect(display.width() - 6, 7, 4, 8, 2, TFT_INDEX_WHITE);
    top.fillRoundRect(display.width() - 50 + 2, 1 + 2, map(constrain(batteryVoltage, 3000, 4000), 3000, 4000, 0, 40), 20 - 4, 4, TFT_INDEX_DARKGREEN);
    if (batteryVoltage > 4200) {
      top.fillTriangle(display.width() - 26,  3, display.width() - 34, 13, display.width() - 28, 12, TFT_INDEX_WHITE);
      top.fillTriangle(display.width() - 30, 18, display.width() - 22,  9, display.width() - 28, 12, TFT_INDEX_WHITE);
    }
/*
    top.setSwapBytes(true);
    if      (batteryVoltage > 4200) top.pushImage(display.width() - 50, 1, 50, 22, bcharge);
    else if (batteryVoltage > 4000) top.pushImage(display.width() - 50, 1, 50, 22, b100);
    else if (batteryVoltage > 3800) top.pushImage(display.width() - 50, 1, 50, 22, b75);
    else if (batteryVoltage > 3600) top.pushImage(display.width() - 50, 1, 50, 22, b50);
    else if (batteryVoltage > 3400) top.pushImage(display.width() - 50, 1, 50, 22, b25);
    else if (batteryVoltage > 3200) top.pushImage(display.width() - 50, 1, 50, 22, b10);
    else top.pushImage(display.width() - 50, 1, 50, 22, b0);
*/
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
        top.drawString("C", 52 + 24*currentProfile + 11, 11);
        break;
    }
  }

/*
  if (millis() < endMillis2) {
    if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
        MTC.getMode() == MidiTimeCode::SynchroClockSlave) {

      display->setFont(ArialMT_Plain_10);
      display->setTextAlignment(TEXT_ALIGN_CENTER);
      display->drawString(64, 0, String(bpm) + "BPM");
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->setFont(block10x10);
      switch (timeSignature) {
        case PED_TIMESIGNATURE_2_4:
          display->drawString( 98, 0, String(0));
          display->drawString(108, 0, String(0));
          break;
        case PED_TIMESIGNATURE_4_4:
          display->drawString( 98, 0, String(0));
          display->drawString(108, 0, String(0));
          display->drawString(118, 0, String(0));
          display->drawString(128, 0, String(0));
          break;
        case PED_TIMESIGNATURE_3_4:
        case PED_TIMESIGNATURE_3_8:
        case PED_TIMESIGNATURE_6_8:
        case PED_TIMESIGNATURE_9_8:
        case PED_TIMESIGNATURE_12_8:
          display->drawString( 98, 0, String(0));
          display->drawString(108, 0, String(0));
          display->drawString(118, 0, String(0));
          break;
      }
      switch (MTC.getBeat()) {
        case 0:
          if (MTC.isPlaying())
            display->drawString(98, 0, String(2));
          else
            display->drawString(98, 0, String(1));
          break;
        case 1:
          if (MTC.isPlaying())
            display->drawString(108, 0, String(2));
          else
            display->drawString(108, 0, String(1));
          break;
        case 2:
          if (MTC.isPlaying())
            display->drawString(118, 0, String(2));
          else
            display->drawString(118, 0, String(1));
          break;
        case 3:
          if (MTC.isPlaying())
            display->drawString(128, 0, String(2));
          else
            display->drawString(128, 0, String(1));
          break;
      }

      //MTC.isPlaying() ? display->setColor(WHITE) : display->setColor(BLACK);
      switch (timeSignature) {
        case PED_TIMESIGNATURE_2_4:
          display->fillRect(64 * MTC.getBeat(), 0, 64, 10);
          break;
        case PED_TIMESIGNATURE_4_4:
          display->drawRect(77 + 13 * 0, 0, 12, 10);
          display->drawRect(77 + 13 * 1, 0, 12, 10);
          display->drawRect(77 + 13 * 2, 0, 12, 10);
          display->drawRect(77 + 13 * 3, 0, 12, 10);
          if (MTC.isPlaying())
            display->fillRect(79 + 13 * MTC.getBeat(), 2,  8,  6);
          else
            display->drawRect(81 + 13 * MTC.getBeat(), 3,  4,  4);
          break;
        case PED_TIMESIGNATURE_3_4:
        case PED_TIMESIGNATURE_3_8:
        case PED_TIMESIGNATURE_6_8:
        case PED_TIMESIGNATURE_9_8:
        case PED_TIMESIGNATURE_12_8:
          display->fillRect(43 * MTC.getBeat(), 0, 42, 10);
          break;
      }
      //display->setColor(WHITE);
    }
    else if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
             MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
      char buf[12];
      sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
      display->setFont(ArialMT_Plain_10);
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->drawString(128, 0, buf);
    }
  }
  */

  top.pushSprite(0, 0);

  top.deleteSprite();
}

void bottomOverlay()
{
  if (scrollingMode) return;

  if (lastUsed == lastUsedPedal && lastUsed != 0xFF && millis() < endMillis2 && lastPedalName[0] != ':') {
    int p;
    switch (m1) {
      case midi::NoteOn:
      case midi::NoteOff:
        rmin = 0;
        rmax = MIDI_RESOLUTION - 1;

      case midi::ControlChange:
        m3 = constrain(m3, rmin, rmax);
        p = map(m3, rmin, rmax, 0, 100);
        display_progress_bar(0, display.height() - 24, display.width(), 24, p);
        if (lastPedalName[0] != 0) display_progress_bar_2_label(m3, map(p, 0, 100, 3, 124));
        break;

      case midi::ProgramChange:
        rmin = 0;
        rmax = MIDI_RESOLUTION - 1;
        m3 = constrain(m2, rmin, rmax);
        p = map(m2, rmin, rmax, 0, 100);
        display_progress_bar(0, display.height() - 24, display.width(), 24, p);
        if (lastPedalName[0] != 0) display_progress_bar_2_label(m3, map(p, 0, 100, 3, 124));
        break;

      case midi::PitchBend:
        p = map(((m3 << 7) | m2) + MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MAX, -100, 100);
        if ( p >= 0 ) {
          display_progress_bar(0,                        display.height() - 24, display.width() / 2 + 11, 24, 0, true);
          display_progress_bar(display.width() / 2 - 12, display.height() - 24, display.width() / 2 + 11, 24, p);
        }
        else {
          display_progress_bar(display.width() / 2 - 12, display.height() - 24, display.width() / 2 + 11, 24, 0);
          display_progress_bar(0,                        display.height() - 24, display.width() / 2 + 11, 24, -p, true);
        }
        break;

      case midi::AfterTouchChannel:
        m3 = constrain(m2, rmin, rmax);
        p = map(m3, rmin, rmax, 0, 100);
        display_progress_bar(0, display.height() - 24, display.width(), 24, p);
        break;
    }
  }
  else if (scrollingMode || MTC.getMode() != MidiTimeCode::SynchroNone) {

    TFT_eSprite bottom = TFT_eSprite(&display);

    bottom.setColorDepth(4);
    bottom.createSprite(display.width(), 24);
    bottom.fillRect(0, 0, display.width(), 24, TFT_INDEX_BLACK);

    bottom.setFreeFont(&FreeSansBold9pt7b);
    bottom.setTextDatum(MC_DATUM);

#ifdef BLE
    if (bleEnabled) {
       if (bleMidiConnected) {
        bottom.fillRect(0, 0, display.width() / 4, 24, TFT_INDEX_WHITE);
        bottom.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
      }
      else {
        bottom.fillRect(0, 0, display.width() / 4, 24, TFT_INDEX_BLACK);
        bottom.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
      }
      bottom.drawRoundRect(0, 0, display.width() / 4, 24, 0, TFT_INDEX_BLACK);
      bottom.drawRoundRect(0, 0, display.width() / 4, 24, 2, TFT_INDEX_BLACK);
      bottom.drawRoundRect(0, 0, display.width() / 4, 24, 4, TFT_INDEX_BLACK);
      bottom.drawString("BLE", 1 * display.width() / 8, 11);
    }
#endif

#ifdef WIFI
    if (wifiEnabled) {
      if (appleMidiConnected) {
        bottom.fillRect(1 * display.width() / 4, 0, display.width() / 4, 24, TFT_INDEX_WHITE);
        bottom.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
      }
      else {
        bottom.fillRect(1 * display.width() / 4, 0, display.width() / 4, 24, TFT_INDEX_BLACK);
        bottom.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
      }
      bottom.drawRoundRect(1 * display.width() / 4, 0, display.width() / 4, 24, 0, TFT_INDEX_BLACK);
      bottom.drawRoundRect(1 * display.width() / 4, 0, display.width() / 4, 24, 2, TFT_INDEX_BLACK);
      bottom.drawRoundRect(1 * display.width() / 4, 0, display.width() / 4, 24, 4, TFT_INDEX_BLACK);
      bottom.drawString("MIDI",   3 * display.width() / 8, 11);
      if (interfaces[PED_IPMIDI].midiIn || interfaces[PED_IPMIDI].midiOut) {
        bottom.fillRect(2 * display.width() / 4, 0, display.width() / 4, 24, TFT_INDEX_WHITE);
        bottom.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
      }
      else {
        bottom.fillRect(2 * display.width() / 4, 0, display.width() / 4, 24, TFT_BLACK);
        bottom.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
      }
      bottom.drawRoundRect(2 * display.width() / 4, 0, display.width() / 4, 24, 0, TFT_INDEX_BLACK);
      bottom.drawRoundRect(2 * display.width() / 4, 0, display.width() / 4, 24, 2, TFT_INDEX_BLACK);
      bottom.drawRoundRect(2 * display.width() / 4, 0, display.width() / 4, 24, 4, TFT_INDEX_BLACK);
      bottom.drawString("ipMidi", 5 * display.width() / 8, 11);

      if (interfaces[PED_OSC].midiIn    || interfaces[PED_OSC].midiOut) {
        bottom.fillRect(3 * display.width() / 4, 0, display.width() / 4, 24, TFT_INDEX_WHITE);
        bottom.setTextColor(TFT_INDEX_BLACK, TFT_INDEX_WHITE);
      }
      else {
        bottom.fillRect(3 * display.width() / 4, 0, display.width() / 4, 24, TFT_INDEX_BLACK);
        bottom.setTextColor(TFT_INDEX_DARKGREY, TFT_INDEX_BLACK);
      }
      bottom.drawRoundRect(3 * display.width() / 4, 0, display.width() / 4, 24, 0, TFT_INDEX_BLACK);
      bottom.drawRoundRect(3 * display.width() / 4, 0, display.width() / 4, 24, 2, TFT_INDEX_BLACK);
      bottom.drawRoundRect(3 * display.width() / 4, 0, display.width() / 4, 24, 4, TFT_INDEX_BLACK);
      bottom.drawString("OSC", 7 * display.width() / 8, 11);
    }
#endif

    bottom.pushSprite(0, display.height() - 24);
  }
}


void drawFrame1(int16_t x, int16_t y)
{
  if (millis() < endMillis2 && lastPedalName[0] != ':') {

    //ui.disableAutoTransition();
    //ui.switchToFrame(0);
    if (strlen(lastPedalName) != 0 && lastPedalName[strlen(lastPedalName) - 1] == '.') lastPedalName[strlen(lastPedalName) - 1] = 0;
    if (!scrollingMode && lastPedalName[0] == 0) {
      display.fillRect(0, 24, display.width(), 6, TFT_INDEX_BLACK);
      TFT_eSprite sprite = TFT_eSprite(&display);
      sprite.setColorDepth(1);
      sprite.createSprite(display.width(), display.height() - 30 - 24);
      sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);
      sprite.fillRect(0, 0, sprite.width(), sprite.height(), TFT_BLACK);
      switch (m1) {
        case midi::InvalidType:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Bank", sprite.width() / 2 + x, 62 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(m2), sprite.width() / 2 + x, 22 + y);
          sprite.drawRoundRect(display.width() / 2 - 50, 0, 100, 50, 8, TFT_WHITE);
          display.fillRect(0, display.height() - 24, display.width(), 24, TFT_INDEX_BLACK);
          break;
        case midi::NoteOn:
        case midi::NoteOff:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Note", sprite.width() / 2 + x, 62 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(m2), sprite.width() / 2 + x, 22 + y);
          sprite.drawRoundRect(display.width() / 2 - 50, 0, 100, 50, 8, TFT_WHITE);
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MR_DATUM);
          sprite.drawString("Velocity", sprite.width() + x, 62 + y);
          sprite.setFreeFont(&FreeSans18pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m3), sprite.width() - display.width() / 8 + x, 22 + y);
          break;
        case midi::ControlChange:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("CC", sprite.width() / 2 + x, 62 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(m2), sprite.width() / 2 + x, 22 + y);
          sprite.drawRoundRect(display.width() / 2 - 50, 0, 100, 50, 8, TFT_WHITE);
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MR_DATUM);
          sprite.drawString("Value", sprite.width() + x, 62 + y);
          sprite.setFreeFont(&FreeSans18pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString(String(m3), sprite.width() - display.width() / 8 + x, 22 + y);
          break;
        case midi::ProgramChange:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("PC", sprite.width() / 2 + x, 62 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(m2), sprite.width() / 2 + x, 22 + y);
          sprite.drawRoundRect(display.width() / 2 - 50, 0, 100, 50, 8, TFT_WHITE);
          break;
        case midi::PitchBend:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Pitch", sprite.width() / 8 * 5 + x, 62 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(((m3 << 7) | m2) + MIDI_PITCHBEND_MIN), sprite.width() / 8 * 5 + x, 22 + y);
          sprite.drawRoundRect(display.width() / 8 * 5 - 70, 0, 140, 50, 8, TFT_WHITE);
          break;
        case midi::AfterTouchChannel:
          sprite.setFreeFont(&FreeSans9pt7b);
          sprite.setTextDatum(MC_DATUM);
          sprite.drawString("Pressure", sprite.width() / 2 + x, 62 + y);
          sprite.setFreeFont(&FreeSans24pt7b);
          sprite.drawString(String(m2), sprite.width() / 2 + x, 22 + y);
          sprite.drawRoundRect(display.width() / 2 - 50, 0, 100, 50, 8, TFT_WHITE);
          break;
      }
      if (m1 != midi::InvalidType) {
        sprite.setFreeFont(&FreeSans9pt7b);
        sprite.setTextDatum(ML_DATUM);
        sprite.drawString("Channel", 0 + x, 62 + y);
        sprite.setFreeFont(&FreeSans18pt7b);
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(String(m4), display.width() / 8 + x, 22 + y);
      }
      sprite.pushSprite(0, 30);
      sprite.deleteSprite();
    }
    else {
      String name = lastPedalName;
      switch (m1) {
        case midi::InvalidType:
          display.drawRect(64-22, 15, 64+24, 15+23, TFT_WHITE);
          display.setTextDatum(TC_DATUM);
          display.setFreeFont(&FreeSans9pt7b);
          display.drawString("Bank", 64 + x, 39 + y);
          display.setFreeFont(&FreeSans24pt7b);
          display.drawString(String(m2), 64 + x, 14 + y);
          break;
        case midi::NoteOn:
        case midi::NoteOff:
        case midi::ControlChange:
          name.replace(String("###"), String(m3));
        case midi::ProgramChange:
        case midi::AfterTouchChannel:
          name.replace(String("###"), String(m2));
        default:
          display.setTextDatum(MC_DATUM);
          display.setFreeFont(&FreeSans24pt7b);
          display.drawString(name, 64, 32);
          break;
      }
    }
  }
  else if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
           MTC.getMode() == MidiTimeCode::SynchroClockSlave) {
    /*
    display->setFont(ArialMT_Plain_24);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(22 + x, 12 + y, String(bpm));
    display->setFont(ArialMT_Plain_10);
    display->drawString(22 + x, 36 + y, "BPM");
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        display->drawString(128 + x, 36 + y, "2/4");
        break;
      case PED_TIMESIGNATURE_4_4:
        display->drawString(128 + x, 36 + y, "4/4");
        break;
      case PED_TIMESIGNATURE_3_4:
        display->drawString(128 + x, 36 + y, "3/4");
        break;
      case PED_TIMESIGNATURE_3_8:
        display->drawString(128 + x, 36 + y, "3/8");
        break;
      case PED_TIMESIGNATURE_6_8:
        display->drawString(128 + x, 36 + y, "6/8");
        break;
      case PED_TIMESIGNATURE_9_8:
        display->drawString(128 + x, 36 + y, "9/8");
        break;
      case PED_TIMESIGNATURE_12_8:
        display->drawString(128 + x, 36 + y, "12/8");
        break;
    }
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    if (MTC.getMode() == MidiTimeCode::SynchroClockMaster)
      display->drawString(68 + x, 36 + y, "Master");
    else if (MTC.getMode() == MidiTimeCode::SynchroClockSlave)
      display->drawString(68 + x, 36 + y, "Slave");

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(block);
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        display->drawString( 68 + x, 16 + x, String(0));
        display->drawString( 88 + x, 16 + x, String(0));
        break;
      case PED_TIMESIGNATURE_4_4:
        display->drawString( 68 + x, 16 + x, String(0));
        display->drawString( 88 + x, 16 + x, String(0));
        display->drawString(108 + x, 16 + x, String(0));
        display->drawString(128 + x, 16 + x, String(0));
        break;
      case PED_TIMESIGNATURE_3_4:
      case PED_TIMESIGNATURE_3_8:
      case PED_TIMESIGNATURE_6_8:
      case PED_TIMESIGNATURE_9_8:
      case PED_TIMESIGNATURE_12_8:
        display->drawString( 68 + x, 16 + x, String(0));
        display->drawString( 88 + x, 16 + x, String(0));
        display->drawString(108 + x, 16 + x, String(0));
        break;
    }
    switch (MTC.getBeat()) {
      case 0:
        if (MTC.isPlaying())
          display->drawString( 68 + x, 16 + x, String(2));
        else
          display->drawString( 68 + x, 16 + x, String(1));
        break;
      case 1:
        if (MTC.isPlaying())
          display->drawString( 88 + x, 16 + x, String(2));
        else
          display->drawString( 88 + x, 16 + x, String(1));
        break;
      case 2:
        if (MTC.isPlaying())
          display->drawString(108 + x, 16 + x, String(2));
        else
          display->drawString(108 + x, 16 + x, String(1));
        break;
      case 3:
        if (MTC.isPlaying())
          display->drawString(128 + x, 16 + x, String(2));
        else
          display->drawString(128 + x, 16 + x, String(1));
        break;
    }
    ui.disableAutoTransition();
    */
  }
  else if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
           MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
    /*
    char buf[12];
    sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
    display->setFont(ArialMT_Plain_24);
    display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display->drawString(64 + x, 32 + y, buf);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster)
      display->drawString(128 + x, 0 + y, "Master");
    else if (MTC.getMode() == MidiTimeCode::SynchroMTCSlave)
      display->drawString(128 + x, 0 + y, "Slave");
    ui.disableAutoTransition();
    */
  }
  else {
    if (scrollingMode) {
      /*
      display.fillRect(0, 24, display.width(), display.height() - 48, TFT_BLACK);
      display.setTextColor(TFT_WHITE, TFT_BLACK);
      display.setFreeFont(&FreeSansBold12pt7b);
      display.setTextDatum(MC_DATUM);
      display.drawString(MODEL, display.width() / 2 + x, display.height() / 2 + y);
      display.setTextFont(1);
      display.setTextDatum(BL_DATUM);
      display.drawString("TM", display.width() - 40 + x, display.height() / 2 - 5 + y);
      */
#ifdef DIAGNOSTIC
      TFT_eSprite sprite = TFT_eSprite(&display);

      sprite.setColorDepth(4);
      sprite.createSprite(POINTS, 108);
      sprite.drawRect(0, 3, sprite.width(), 102, TFT_INDEX_DARKGREY);
      for (byte i = 0; i < POINTS; i = i + 4) {
        sprite.drawPixel(i, 3 + 102 / 2,     TFT_INDEX_DARKGREY);
        sprite.drawPixel(i, 3 + 102 / 4,     TFT_INDEX_DARKGREY);
        sprite.drawPixel(i, 3 + 102 / 4 * 3, TFT_INDEX_DARKGREY);
      }
      for (byte i = 3; i < 105; i = i + 6) {
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
      if (GRAPH_DURATION_QUARTER_HOUR > 0) {
        label0 += String(0*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label1 += String(1*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label2 += String(2*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label3 += String(3*GRAPH_DURATION_QUARTER_HOUR) + "h";
        label4 += String(4*GRAPH_DURATION_QUARTER_HOUR) + "h";
      }
      else if (GRAPH_DURATION_QUARTER_MIN > 0) {
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

      for (byte i = 0; i < POINTS - 1; i++) {
        byte h = (historyStart + i) % POINTS;
        if (memoryHistory[h]  != 0 && memoryHistory[h+1]  != 0) sprite.drawLine(i, 104 - memoryHistory[h],  i + 1, 104 - memoryHistory[h+1],  TFT_INDEX_RED);
        if (wifiHistory[h]    != 0 && wifiHistory[h+1]    != 0) sprite.drawLine(i, 104 - wifiHistory[h],    i + 1, 104 - wifiHistory[h+1],    TFT_INDEX_BLUE);
        if (batteryHistory[h] != 0 && batteryHistory[h+1] != 0) sprite.drawLine(i, 104 - batteryHistory[h], i + 1, 104 - batteryHistory[h+1], TFT_INDEX_GREEN);
      }

      sprite.setTextFont(1);
      sprite.setTextColor(TFT_INDEX_BLUE, TFT_INDEX_BLACK);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("-10dB", 0, sprite.height() / 2 - 50);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("-50dB", 0, sprite.height() / 2);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("-90dB", 0, sprite.height() / 2 + 50);
      sprite.setTextColor(TFT_INDEX_RED, TFT_INDEX_BLACK);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString("200Kb", sprite.width() / 2, sprite.height() / 2 - 50);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString("100Kb", sprite.width() / 2, sprite.height() / 2);
      sprite.setTextDatum(MC_DATUM);
      sprite.drawString("0Kb",  sprite.width() / 2, sprite.height() / 2 + 50);
      sprite.setTextColor(TFT_INDEX_GREEN, TFT_INDEX_BLACK);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString("5.0V", sprite.width() - 1, sprite.height() / 2 - 50);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString("4.0V", sprite.width() - 1, sprite.height() / 2);
      sprite.setTextDatum(MR_DATUM);
      sprite.drawString("3.0V", sprite.width() - 1, sprite.height() / 2 + 50);
      sprite.pushSprite(0, 27);
      sprite.deleteSprite();
      //ui.enableAutoTransition();
#endif
    }
    else {
      if (banknames[currentBank][0] == 0) {
        String p;

        display.fillRect(0, 24, display.width(), 6, TFT_INDEX_BLACK);

        TFT_eSprite sprite = TFT_eSprite(&display);

        sprite.setColorDepth(1);
        sprite.createSprite(display.width(), display.height() - 30);
        sprite.setFreeFont(&DSEG14_Classic_Bold_100);
        switch (currentProfile) {
          case 0:
            p = "A." + ((currentBank >= 9  ? "" : "0") + String(currentBank + 1));
            sprite.setBitmapColor(TFT_RED, TFT_BLACK);
            break;
          case 1:
            p = "B." + ((currentBank >= 9  ? "" : "0") + String(currentBank + 1));
            sprite.setBitmapColor(TFT_GREEN, TFT_BLACK);
            break;
          case 2:
            p = "C." + ((currentBank >= 9  ? "" : "0") + String(currentBank + 1));
            sprite.setBitmapColor(TFT_BLUE, TFT_BLACK);
            break;
          default:
            p = "" + ((currentBank >= 9  ? "" : "0") + String(currentBank + 1));
            sprite.setBitmapColor(TFT_WHITE, TFT_BLACK);
            break;
        }
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(p, sprite.width() / 2, sprite.height() / 2);
        sprite.pushSprite(0, 30);
        sprite.deleteSprite();
      }
      else {
        const byte pedals2 = PEDALS / 2;
        String name;
        int offsetText       = 0;
        int offsetBackground = 0;
        static unsigned long ms = millis();

        TFT_eSprite sprite = TFT_eSprite(&display);

        sprite.setColorDepth(1);
        sprite.createSprite(display.width(), display.height() - 30);
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
          if (pedals[p].function1 == PED_MIDI && currentMIDIValue[currentBank][p][0] == banks[currentBank][p].midiValue2) {
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
          if (pedals[p + PEDALS / 2].function1 == PED_MIDI && currentMIDIValue[currentBank][p + PEDALS / 2][0] == banks[currentBank][p + PEDALS / 2].midiValue2) {
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
          sprite.drawRect(0, 20, sprite.width(), sprite.height() - 42, 1);
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
            if ((pedals[p].function1 == PED_MIDI) && (banks[currentBank][p].midiMessage != PED_EMPTY)) {
              display_progress_bar_sprite(sprite, (sprite.width() / (PEDALS / 2)) * p + 2 + x, 25 + y, 55, 20, constrain(map(currentMIDIValue[currentBank][p][0],
                                                                                                       banks[currentBank][p].midiValue1,
                                                                                                       banks[currentBank][p].midiValue2,
                                                                                                       0, 100),
                                                                                                   0, 100));
            }
            if ((pedals[p + PEDALS / 2].function1 == PED_MIDI) && (banks[currentBank][p + PEDALS / 2].midiMessage != PED_EMPTY)) {
              display_progress_bar_sprite(sprite, (sprite.width() / (PEDALS / 2)) * p + 2 + x, sprite.height() - 22 - 25 + y, 55, 20, constrain(map(currentMIDIValue[currentBank][p + PEDALS / 2][0],
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
        sprite.pushSprite(0, 30);
        sprite.deleteSprite();
      }
      //ui.disableAutoTransition();
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
    //ui.switchToFrame(0);

  display.fillRect(0, 24, display.width(), display.height() - 48, TFT_BLACK);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_DARKGREY, TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString("Device:", 0 + x, 30 + y);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  display.drawString(host, display.width() + x, 30 + y);
#ifdef WIFI
  switch (WiFi.getMode()) {
    case WIFI_AP:
    case WIFI_AP_STA:
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_DARKGREY, TFT_BLACK);
      display.setTextDatum(TL_DATUM);
      display.drawString( "AP:", 0 + x, 30 + y);
      display.setFreeFont(&FreeSans12pt7b);
      display.setTextColor(TFT_WHITE, TFT_BLACK);
      display.setTextDatum(TR_DATUM);
      display.drawString(ssidSoftAP, display.width() + x, 30 + y);
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
    //ui.switchToFrame(0);

  display.fillRect(0, 24, display.width(), display.height() - 48, TFT_BLACK);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_DARKGREY, TFT_BLACK);
  display.setTextDatum(TL_DATUM);
  display.drawString("Free heap:", 0 + x, 30 + y);
  display.setFreeFont(&FreeSans12pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  display.drawString(ESP.getFreeHeap()/1024 + String(" Kb"), display.width() + x, 30 + y);

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
  long sec = (millis() / 1000) % 60;
  long min = (millis() / 1000 / 60) % 60;
  long h   = (millis() / 1000 / 3600);
  display.drawString(h + String("h ") + min + String("m ") + sec + String("s"), display.width() + x, 86 + y);
}


void display_init()
{
    display.init();
    display.setRotation(1);
    display.fillScreen(TFT_BLACK);
    /*
    if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
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
}

void display_ui_update_disable()
{
  uiUpdate = false;
}

void display_ui_update_enable()
{
  uiUpdate = true;
}

void display_update()
{
  if (interruptCounter3 > 0) {

    interruptCounter3 = 0;

    if (uiUpdate) {
      topOverlay();
      drawFrame1(0, 0);
      bottomOverlay();
    }
  }
}

void display_off()
{
  digitalWrite(TFT_BL, !TFT_BACKLIGHT_ON);

  display.writecommand(TFT_DISPOFF);
  display.writecommand(TFT_SLPIN);
}