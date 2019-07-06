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

#define LCD_ROWS  2
#define LCD_COLS  16

#define LCD_LINE1_PERSISTENCE   1500;

#define lcdClear(...)
#define lcdSetCursor(...)
#define lcdPrint(...)
#define lcdCreateChar(...)
#define lcdWrite(...)
#define lcdCursor(...)
#define lcdNoCursor(...)
/*
#define lcdClear(...)       lcd.clear( __VA_ARGS__ )
#define lcdSetCursor(...)   lcd.setCursor( __VA_ARGS__ )
#define lcdPrint(...)       lcd.print( __VA_ARGS__ )
#define lcdCreateChar(...)  lcd.createChar( __VA_ARGS__ )
#define lcdWrite(...)       lcd.write( __VA_ARGS__ )
#define lcdCursor(...)      lcd.cursor( __VA_ARGS__ )
#define lcdNoCursor(...)    lcd.noCursor( __VA_ARGS__ )
*/

bool powerPlug     = false;
bool batteryLow    = false;

#define POWERPLUG     (byte)4
#define BATTERYLEVEL  (byte)5
#define WIFIICON      (byte)6
#define BLUETOOTHICON (byte)7

byte partial_bar[4][8] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,

  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,

  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,

  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte battery[7][8] = {
  B01110,
  B11011,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,

  B01110,
  B11011,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,

  B01110,
  B11011,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,

  B01110,
  B11011,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,

  B01110,
  B11011,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,

  B01110,
  B11011,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,

  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte power_plug[] = {
  B01010,
  B01010,
  B11111,
  B10001,
  B10001,
  B01110,
  B00100,
  B00100
};

byte bluetooth_icon[] = {
  B00100,
  B00110,
  B10101,
  B01110,
  B01110,
  B10101,
  B00110,
  B00100
};

byte wifi_icon[] = {
  B00001,
  B00001,
  B00001,
  B00101,
  B00101,
  B10101,
  B10101,
  B10101
};

const char bar1[]  = {49, 50, 51, 52, 53, 54, 55, 56, 57, 58};
const char bar2[]  = {'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o'};
//const char bar2[]  = {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'};

int m1, m2, m3, m4;
unsigned long endMillis2;


void screen_info(int b1, int b2, int b3, int b4)
{
  m1 = b1;
  m2 = b2;
  m3 = b3;
  m4 = b4;
  endMillis2 = millis() + LCD_LINE1_PERSISTENCE;
}


char foot_char(byte footswitch)
{
  footswitch = constrain(footswitch, 0, PEDALS - 1);
  if (pedals[footswitch].function != PED_MIDI) return ' ';
  if ((footswitch == lastUsedPedal) ||

      ((pedals[footswitch].mode == PED_MOMENTARY1 ||
        pedals[footswitch].mode == PED_LATCH1) && pedals[footswitch].pedalValue[0] == LOW) ||

      ((pedals[footswitch].mode == PED_MOMENTARY2 ||
        pedals[footswitch].mode == PED_MOMENTARY3 ||
        pedals[footswitch].mode == PED_LATCH2) && pedals[footswitch].pedalValue[0] == LOW) ||

      pedals[footswitch].pedalValue[1] == LOW) return bar1[footswitch % 10];
  return ' ';
}


void screen_update(bool force = false) {

  static char screen1[LCD_COLS + 1];
  static char screen2[LCD_COLS + 1];
  static int  analog;
  static byte batteryLevel = 0;

  byte        f = 0, p = 0;

  if (!powersaver) {

    char buf[LCD_COLS + 1];

    // Line 1
    memset(buf, 0, sizeof(buf));
    if (millis() < endMillis2) {
      switch (m1) {
        case midi::NoteOn:
        case midi::NoteOff:
          sprintf(&buf[strlen(buf)], "Note %3d Ch%2d", m2, m4);
          break;
        case midi::ControlChange:
          sprintf(&buf[strlen(buf)], "CC%3d/%3dCh%2d", m2, m3, m4);
          break;
        case midi::ProgramChange:
          sprintf(&buf[strlen(buf)], "PC%3d    Ch%2d", m2, m4);
          break;
        case midi::PitchBend:
          sprintf(&buf[strlen(buf)], "Pitch%3d Ch%2d", m2, m4);
          break;
      }
    }
    else if ( MidiTimeCode::getMode() == MidiTimeCode::SynchroClockMaster || MidiTimeCode::getMode() == MidiTimeCode::SynchroClockSlave) {
      sprintf(&buf[strlen(buf)], "%3dBPM", bpm);
      for (byte i = 0; i < (LCD_COLS - 9); i++)
        if (MTC.isPlaying())
          buf[6 + i] = (MTC.getBeat() == i) ? '>' : ' ';
        else
          buf[6 + i] = (MTC.getBeat() == i) ? '.' : ' ';
    }
    else if ( MidiTimeCode::getMode() == MidiTimeCode::SynchroMTCMaster || MidiTimeCode::getMode() == MidiTimeCode::SynchroMTCSlave) {
      sprintf(&buf[strlen(buf)], "%02d:%02d:%02d:%02d    ", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
    }
    else {
      for (byte i = 0; i < (LCD_COLS - 3); i++) {
        //buf[i] = foot_char(i);
        buf[i] = ' ';
      }
    }
    if (force || strcmp(screen1, buf) != 0) {     // do not update if not changed
      memset(screen1, 0, sizeof(screen1));
      strncpy(screen1, buf, LCD_COLS);
      lcdSetCursor(0, 0);
      lcdPrint(buf);
#ifdef BLYNK
      blynkLCD.print(0, 0, buf);
#endif
    }

    if (bleConnected) {
      lcdSetCursor(13, 0);
      lcdWrite(BLUETOOTHICON);
    }
    if (wifiConnected || true) {
      lcdSetCursor(14, 0);
      lcdWrite(WIFIICON);
    }
    if (powerPlug) {
      lcdSetCursor(15, 0);
      lcdWrite(POWERPLUG);
    }

    byte newLevel = (millis() % 3500) / 500;
    newLevel = 3;
    if (force || batteryLevel != newLevel) {
      batteryLevel = newLevel;
      lcdCreateChar(BATTERYLEVEL, battery[batteryLevel]);
      lcdSetCursor(15, 0);
      lcdWrite(BATTERYLEVEL);
    }

    // Line 2
    memset(buf, 0, sizeof(buf));
    sprintf(&buf[strlen(buf)], "Bank%2d", currentBank + 1);
    if (lastUsedPedal >= 0 && lastUsedPedal < PEDALS) {
      //strncpy(&buf[strlen(buf)], &bar2[0], map(pedals[lastUsedPedal].pedalValue[0], 0, MIDI_RESOLUTION - 1, 0, 10));
      //strncpy(&buf[strlen(buf)], "          ", 10 - map(pedals[lastUsedPedal].pedalValue[0], 0, MIDI_RESOLUTION - 1, 0, 10));
      f = map(pedals[lastUsedPedal].pedalValue[0], 0, MIDI_RESOLUTION - 1, 0, 50);
      p = f % 5;
      f = f / 5;
      strncpy(&buf[strlen(buf)], &bar2[0], f);
    }
    if (force || strcmp(screen2, buf) != 0 || analog != pedals[lastUsedPedal].pedalValue[0]) {     // do not update if not changed
      memset(screen2, 0, sizeof(screen2));
      strncpy(screen2, buf, LCD_COLS);
      analog = pedals[lastUsedPedal].pedalValue[0];
      lcdSetCursor(0, 1);
      lcdPrint(buf);
      if (p > 0) lcdWrite((byte)(p - 1));
      for (byte i = 0; i < 10 - f ; i++) {
        lcdPrint(" ");
        buf[strlen(buf)] = ' ';
      } 
      // replace unprintable chars
      for (byte i = 0; i < LCD_COLS; i++)
        buf[i] = (buf[i] == -1) ? '#' : buf[i];
#ifdef BLYNK
      blynkLCD.print(0, 1, buf);
#endif
    }

    if (selectBank) {
      lcdSetCursor(5, 1);
      lcdCursor();
    }
    else
      lcdNoCursor();
  }
}