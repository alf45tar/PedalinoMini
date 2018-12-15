/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

#ifdef NOLCD
#define menu_setup(...)
#define menu_run(...)
#else
#include "NoteNumbers.h"
#include "ControlChange.h"

const bool      AUTO_START    = true;  // auto start the menu, manual detect and start if false
const uint16_t  MENU_TIMEOUT  = 5000;  // in milliseconds


// Function prototypes for Navigation/Display
bool display(MD_Menu::userDisplayAction_t, char* = nullptr);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);

// Function prototypes for variable get/set functions
MD_Menu::value_t *mnuValueRqst(MD_Menu::mnuId_t id, bool bGet);

#define M_ROOT            10
#define M_BANKSETUP       11
#define M_PEDALSETUP      12
#define M_INTERFACESETUP  13
#define M_TEMPO           14
#define M_PROFILE         15
#define M_OPTIONS         16

#define II_BANK           20
#define II_PEDAL          21
#define II_MIDICHANNEL    22
#define II_MIDIMESSAGE    23
#define II_MIDICODE       24
#define II_MIDINOTE       25
#define II_MIDIVALUE1     26
#define II_MIDIVALUE2     27
#define II_MIDIVALUE3     28
#define II_FUNCTION       29
#define II_AUTOSENSING    30
#define II_MODE           31
#define II_PRESS_MODE     32
#define II_VALUE_SINGLE   33
#define II_VALUE_DOUBLE   34
#define II_VALUE_LONG     35
#define II_POLARITY       36
#define II_CALIBRATE      37
#define II_ZERO           38
#define II_MAX            39
#define II_RESPONSECURVE  40
#define II_INTERFACE      41
#define II_MIDI_IN        42
#define II_MIDI_OUT       43
#define II_MIDI_THRU      44
#define II_MIDI_ROUTING   45
#define II_MIDI_CLOCK     46
#define II_PROFILE_LOAD   48
#define II_PROFILE_COPY   49
#define II_BACKLIGHT      50
#define II_IRLEARN        51
#define II_IRCLEAR        52
#define II_WIFIRESET      53
#define II_MIDITIMECODE   54
#define II_BPM            55
#define II_TIMESIGNATURE  56
#define II_SERIALPASS     57
#define II_DEFAULT        58

// Global menu data and definitions

MD_Menu::value_t vBuf;  // interface buffer for values

// Menu Headers --------
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { M_ROOT,           SIGNATURE,         10, 15, 0 },
  { M_BANKSETUP,      "Banks Setup",     20, 37, 0 },
  { M_PEDALSETUP,     "Pedals Setup",    40, 49, 0 },
  { M_INTERFACESETUP, "Interface Setup", 60, 65, 0 },
  { M_TEMPO,          "Tempo",           70, 72, 0 },
  { M_PROFILE,        "Profiles",        80, 81, 0 },
  { M_OPTIONS,        "Options",         90, 95, 0 }
};

// Menu Items ----------
const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  // Starting (Root) menu
  { 10, "Banks Setup",     MD_Menu::MNU_MENU,  M_BANKSETUP },
  { 11, "Pedals Setup",    MD_Menu::MNU_MENU,  M_PEDALSETUP },
  { 12, "Interface Setup", MD_Menu::MNU_MENU,  M_INTERFACESETUP },
  { 13, "Tempo",           MD_Menu::MNU_MENU,  M_TEMPO },
  { 14, "Profiles",        MD_Menu::MNU_MENU,  M_PROFILE },
  { 15, "Options",         MD_Menu::MNU_MENU,  M_OPTIONS },
  // Banks Setup
  { 20, "Select Bank",     MD_Menu::MNU_INPUT, II_BANK },
  { 30, "Select Pedal",    MD_Menu::MNU_INPUT, II_PEDAL },
  { 31, "MIDI Channel",    MD_Menu::MNU_INPUT, II_MIDICHANNEL },
  { 32, "MIDI Message",    MD_Menu::MNU_INPUT, II_MIDIMESSAGE },
  { 33, "MIDI Code",       MD_Menu::MNU_INPUT, II_MIDICODE },
  { 34, "MIDI Note",       MD_Menu::MNU_INPUT, II_MIDINOTE },
  { 35, "Value 1",         MD_Menu::MNU_INPUT, II_MIDIVALUE1 },
  { 36, "Value 2",         MD_Menu::MNU_INPUT, II_MIDIVALUE2 },
  { 37, "Value 3",         MD_Menu::MNU_INPUT, II_MIDIVALUE3 },
  // Pedals Setup
  { 40, "Select Pedal",    MD_Menu::MNU_INPUT, II_PEDAL },
  { 41, "Auto Sensing",    MD_Menu::MNU_INPUT, II_AUTOSENSING },
  { 42, "Mode",            MD_Menu::MNU_INPUT, II_MODE },
  { 43, "Function",        MD_Menu::MNU_INPUT, II_FUNCTION },
  { 44, "Press Mode",      MD_Menu::MNU_INPUT, II_PRESS_MODE },
  { 45, "Polarity",        MD_Menu::MNU_INPUT, II_POLARITY },
  { 46, "Calibrate",       MD_Menu::MNU_INPUT, II_CALIBRATE },
  { 47, "Zero",            MD_Menu::MNU_INPUT, II_ZERO },
  { 48, "Max",             MD_Menu::MNU_INPUT, II_MAX },
  { 49, "Response Curve",  MD_Menu::MNU_INPUT, II_RESPONSECURVE },
  // Interface Setup
  { 60, "Select Interf.",  MD_Menu::MNU_INPUT, II_INTERFACE },
  { 61, "MIDI IN",         MD_Menu::MNU_INPUT, II_MIDI_IN },
  { 62, "MIDI OUT",        MD_Menu::MNU_INPUT, II_MIDI_OUT },
  { 63, "MIDI THRU",       MD_Menu::MNU_INPUT, II_MIDI_THRU },
  { 64, "MIDI Routing",    MD_Menu::MNU_INPUT, II_MIDI_ROUTING },
  { 65, "MIDI Clock",      MD_Menu::MNU_INPUT, II_MIDI_CLOCK },
  // Tempo
  { 70, "MIDI Time Code",  MD_Menu::MNU_INPUT, II_MIDITIMECODE },
  { 71, "Time Signature",  MD_Menu::MNU_INPUT, II_TIMESIGNATURE },
  { 72, "BPM",             MD_Menu::MNU_INPUT, II_BPM },
  // Profiles Setup
  { 80, "Load Profile",    MD_Menu::MNU_INPUT, II_PROFILE_LOAD },
  { 81, "Copy To Profile", MD_Menu::MNU_INPUT, II_PROFILE_COPY },
  // Options
  { 90, "IR RC Learn",     MD_Menu::MNU_INPUT, II_IRLEARN },
  { 91, "IR RC Clear",     MD_Menu::MNU_INPUT, II_IRCLEAR },
//  { 92, "LCD Backlight",   MD_Menu::MNU_INPUT, II_BACKLIGHT },
  { 93, "WiFi Reset",      MD_Menu::MNU_INPUT, II_WIFIRESET },
  { 94, "Firmware upload", MD_Menu::MNU_INPUT, II_SERIALPASS },
  { 95, "Factory default", MD_Menu::MNU_INPUT, II_DEFAULT }
};

// Input Items ---------
const PROGMEM char listMidiMessage[]     = "Program Change| Control Code |  Note On/Off |  Pitch Bend  ";
const PROGMEM char listPedalFunction[]   = "     MIDI     |    Bank +    |    Bank -    |     Start    |     Stop     |   Continue   |     Tap      |     Menu     |    Confirm   |    Escape    |     Next     |   Previous   ";
const PROGMEM char listPedalMode[]       = "   Momentary  |     Latch    |    Analog    |   Jog Wheel  |  Momentary 2 |  Momentary 3 |    Latch 2   |    Ladder    ";
const PROGMEM char listPedalPressMode[]  = "    Single    |    Double    |     Long     |      1+2     |      1+L     |     1+2+L    |      2+L     ";
const PROGMEM char listPolarity[]        = " No|Yes";
const PROGMEM char listResponseCurve[]   = "    Linear    |      Log     |   Anti-Log   ";
const PROGMEM char listInterface[]       = "     USB      |  Legacy MIDI |   AppleMIDI  |    ipMIDI    |   Bluetooth  |     OSC      ";
const PROGMEM char listEnableDisable[]   = "   Disable    |    Enable    ";
const PROGMEM char listMidiTimeCode[]    = "    None      |   MTC Slave  |    MTC 24    |    MTC 25    |   MTC 30 DF  |    MTC 30    |  Clock Slave | Clock Master ";
const PROGMEM char listTimeSignature[]   = "     2/4      |     4/4      |     3/4      |     3/8      |     6/8      |     9/8      |     12/8     ";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { II_BANK,          ">1-10:      ", MD_Menu::INP_INT,   mnuValueRqst,  2, 1, 0,  BANKS, 0, 10, nullptr },
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  { II_PEDAL,         ">1-8:       ", MD_Menu::INP_INT,   mnuValueRqst,  2, 1, 0, PEDALS, 0, 10, nullptr },
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  { II_PEDAL,         ">1-16:      ", MD_Menu::INP_INT,   mnuValueRqst,  2, 1, 0, PEDALS, 0, 10, nullptr },
#endif
  { II_MIDICHANNEL,   ">1-16:      ", MD_Menu::INP_INT,   mnuValueRqst,  2, 1, 0,                 16, 0, 10, nullptr },
  { II_MIDIMESSAGE,   ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listMidiMessage },
  { II_MIDICODE,      ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listMidiControlChange },
  { II_MIDINOTE,      ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listMidiNoteNumbers },
  { II_MIDIVALUE1,    ">0-127:    " , MD_Menu::INP_INT,   mnuValueRqst,  3, 0, 0,                127, 0, 10, nullptr },
  { II_MIDIVALUE2,    ">0-127:    " , MD_Menu::INP_INT,   mnuValueRqst,  3, 0, 0,                127, 0, 10, nullptr },
  { II_MIDIVALUE3,    ">0-127:    " , MD_Menu::INP_INT,   mnuValueRqst,  3, 0, 0,                127, 0, 10, nullptr },
  { II_FUNCTION,      ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listPedalFunction },
  { II_AUTOSENSING,   ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listEnableDisable },
  { II_MODE,          ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listPedalMode },
  { II_PRESS_MODE,    ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listPedalPressMode },
  { II_VALUE_SINGLE,  ">0-127:    " , MD_Menu::INP_INT,   mnuValueRqst,  3, 0, 0,                127, 0, 10, nullptr },
  { II_VALUE_DOUBLE,  ">0-127:    " , MD_Menu::INP_INT,   mnuValueRqst,  3, 0, 0,                127, 0, 10, nullptr },
  { II_VALUE_LONG,    ">0-127:    " , MD_Menu::INP_INT,   mnuValueRqst,  3, 0, 0,                127, 0, 10, nullptr },
  { II_POLARITY,      "Invert:    " , MD_Menu::INP_LIST,  mnuValueRqst,  3, 0, 0,                  0, 0,  0, listPolarity },
  { II_CALIBRATE,     "Confirm"     , MD_Menu::INP_RUN,   mnuValueRqst,  0, 0, 0,                  0, 0,  0, nullptr },
  { II_ZERO,          ">0-1023:  "  , MD_Menu::INP_INT,   mnuValueRqst,  4, 0, 0, ADC_RESOLUTION - 1, 0, 10, nullptr },
  { II_MAX,           ">0-1023:  "  , MD_Menu::INP_INT,   mnuValueRqst,  4, 0, 0, ADC_RESOLUTION - 1, 0, 10, nullptr },
  { II_RESPONSECURVE, ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listResponseCurve },
  { II_INTERFACE,     ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listInterface },
  { II_MIDI_IN,       ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listEnableDisable },
  { II_MIDI_OUT,      ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listEnableDisable },
  { II_MIDI_THRU,     ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listEnableDisable },
  { II_MIDI_ROUTING,  ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listEnableDisable },
  { II_MIDI_CLOCK,    ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listEnableDisable },
  { II_PROFILE_LOAD,  ">1-3:        ", MD_Menu::INP_INT,   mnuValueRqst,  1, 1, 0,                  3, 1, 10, nullptr },
  { II_PROFILE_COPY,  ">1-3:        ", MD_Menu::INP_INT,   mnuValueRqst,  1, 1, 0,                  3, 1, 10, nullptr },
  { II_BACKLIGHT,     ">1-10:      ", MD_Menu::INP_INT,   mnuValueRqst,  2, 1, 0,                 10, 0, 10, nullptr },
  { II_IRLEARN,       "Confirm"     , MD_Menu::INP_RUN,   mnuValueRqst,  0, 0, 0,                  0, 0,  0, nullptr },
  { II_IRCLEAR,       "Confirm"     , MD_Menu::INP_RUN,   mnuValueRqst,  0, 0, 0,                  0, 0,  0, nullptr },
  { II_WIFIRESET,     "Confirm"     , MD_Menu::INP_RUN,   mnuValueRqst,  0, 0, 0,                  0, 0,  0, nullptr },
  { II_MIDITIMECODE,  ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listMidiTimeCode },
  { II_BPM,           ">40-300:   " , MD_Menu::INP_INT,   mnuValueRqst,  3, 1, 0,                300, 40, 10, nullptr },
  { II_TIMESIGNATURE, ""            , MD_Menu::INP_LIST,  mnuValueRqst, 14, 0, 0,                  0, 0,  0, listTimeSignature },
  { II_SERIALPASS,    "Confirm"     , MD_Menu::INP_RUN,   mnuValueRqst,  0, 0, 0,                  0, 0,  0, nullptr },
  { II_DEFAULT,       "Confirm"     , MD_Menu::INP_RUN,   mnuValueRqst,  0, 0, 0,                  0, 0,  0, nullptr }
};

// bring it all together in the global menu object
MD_Menu M(navigation, display,        // user navigation and display
          mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
          mnuItm, ARRAY_SIZE(mnuItm), // menu item data
          mnuInp, ARRAY_SIZE(mnuInp));// menu input data

// Callback code for menu set/get input values

MD_Menu::value_t *mnuValueRqst(MD_Menu::mnuId_t id, bool bGet)
{
  MD_Menu::value_t *r = &vBuf;

  switch (id)
  {
    case II_PROFILE_LOAD:
      if (bGet) vBuf.value = currentProfile + 1;
      else {
        currentProfile = vBuf.value - 1;
        update_current_profile_eeprom();
        DPRINTLNF("Switching profile");
        Reset_AVR();
        //setup();
      }
      break;

    case II_PROFILE_COPY:
      if (bGet) vBuf.value = currentProfile + 1;
      else currentProfile = vBuf.value - 1;
      break;

    case II_BANK:
      if (bGet) vBuf.value = currentBank + 1;
      else currentBank = vBuf.value - 1;
      break;

    case II_PEDAL:
      if (bGet) vBuf.value = currentPedal + 1;
      else currentPedal = vBuf.value - 1;
      break;

    case II_MIDICHANNEL:
      if (bGet) vBuf.value = banks[currentBank][currentPedal].midiChannel;
      else {
        banks[currentBank][currentPedal].midiChannel = vBuf.value;
        serialize_bank();
      }
      break;

    case II_MIDIMESSAGE:
      if (bGet) vBuf.value = banks[currentBank][currentPedal].midiMessage;
      else {
        banks[currentBank][currentPedal].midiMessage = vBuf.value;
        serialize_bank();
      }
      break;

    case II_MIDICODE:
    case II_MIDINOTE:
      if (bGet) vBuf.value = banks[currentBank][currentPedal].midiCode;
      else {
        banks[currentBank][currentPedal].midiCode = vBuf.value;
        serialize_bank();
      }
      break;

    case II_MIDIVALUE1:
      if (bGet) vBuf.value = banks[currentBank][currentPedal].midiValue1;
      else {
        banks[currentBank][currentPedal].midiValue1 = vBuf.value;
        serialize_bank();
      }
      break;

    case II_MIDIVALUE2:
      if (bGet) vBuf.value = banks[currentBank][currentPedal].midiValue2;
      else {
        banks[currentBank][currentPedal].midiValue2 = vBuf.value;
        serialize_bank();
      }
      break;

    case II_MIDIVALUE3:
      if (bGet) vBuf.value = banks[currentBank][currentPedal].midiValue3;
      else {
        banks[currentBank][currentPedal].midiValue3 = vBuf.value;
        serialize_bank();
      }
      break;

    case II_FUNCTION:
      if (bGet) vBuf.value = pedals[currentPedal].function;
      else {
        pedals[currentPedal].function = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_AUTOSENSING:
      if (bGet) vBuf.value = pedals[currentPedal].autoSensing;
      else {
        pedals[currentPedal].autoSensing = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_MODE:
      if (bGet) vBuf.value = pedals[currentPedal].mode;
      else {
        pedals[currentPedal].mode = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_PRESS_MODE:
      if (bGet) vBuf.value = pedals[currentPedal].pressMode;
      else {
        pedals[currentPedal].pressMode = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_POLARITY:
      if (bGet) vBuf.value = pedals[currentPedal].invertPolarity;
      else {
        pedals[currentPedal].invertPolarity = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_CALIBRATE:
      if (!bGet && pedals[currentPedal].mode == PED_ANALOG) calibrate();
      r = nullptr;
      break;

    case II_ZERO:
      if (bGet)
        if (pedals[currentPedal].mode == PED_ANALOG)
          vBuf.value = pedals[currentPedal].expZero;
        else r = nullptr;
      else {
        pedals[currentPedal].expZero = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_MAX:
      if (bGet)
        if (pedals[currentPedal].mode == PED_ANALOG)
          vBuf.value = pedals[currentPedal].expMax;
        else r = nullptr;
      else {
        pedals[currentPedal].expMax = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_RESPONSECURVE:
      if (bGet)
        if (pedals[currentPedal].mode == PED_ANALOG)
          vBuf.value = pedals[currentPedal].mapFunction;
        else r = nullptr;
      else {
        pedals[currentPedal].mapFunction = vBuf.value;
        serialize_pedal();
      }
      break;

    case II_INTERFACE:
      if (bGet) vBuf.value = currentInterface;
      else {
        currentInterface = vBuf.value;
        serialize_interface();
      }
      break;

    case II_MIDI_IN:
      if (bGet) vBuf.value = interfaces[currentInterface].midiIn;
      else {
        interfaces[currentInterface].midiIn = vBuf.value;
        serialize_interface();
      }
      break;

    case II_MIDI_OUT:
      if (bGet) vBuf.value = interfaces[currentInterface].midiOut;
      else {
        interfaces[currentInterface].midiOut = vBuf.value;
        serialize_interface();
      }
      break;

    case II_MIDI_THRU:
      if (bGet) vBuf.value = interfaces[currentInterface].midiThru;
      else {
        interfaces[currentInterface].midiThru = vBuf.value;
        interfaces[PED_USBMIDI].midiThru ? USB_MIDI.turnThruOn() : USB_MIDI.turnThruOff();
        interfaces[PED_DINMIDI].midiThru ? DIN_MIDI.turnThruOn() : DIN_MIDI.turnThruOff();
        interfaces[PED_RTPMIDI].midiThru ? ESP_MIDI.turnThruOn() : ESP_MIDI.turnThruOff();
        serialize_interface();
      }
      break;

    case II_MIDI_ROUTING:
      if (bGet) vBuf.value = interfaces[currentInterface].midiRouting;
      else {
        interfaces[currentInterface].midiRouting = vBuf.value;
        serialize_interface();
      }
      break;

    case II_MIDI_CLOCK:
      if (bGet) vBuf.value = interfaces[currentInterface].midiClock;
      else {
        interfaces[currentInterface].midiClock = vBuf.value;
        serialize_interface();
      }
      break;

    case II_MIDITIMECODE:
      if (bGet) vBuf.value = currentMidiTimeCode;
      else {
        currentMidiTimeCode = vBuf.value;
        mtc_setup();
      }
      break;
    
    case II_BPM:
      if (bGet) vBuf.value = bpm;
      else bpm = vBuf.value;
      break;

    case II_TIMESIGNATURE:
      if (bGet) vBuf.value = timeSignature;
      else {
        timeSignature = vBuf.value;
        switch (timeSignature) {
          case PED_TIMESIGNATURE_2_4:
            MTC.setBeat(2);
            break;
          case PED_TIMESIGNATURE_4_4:
            MTC.setBeat(4);
            break;
          case PED_TIMESIGNATURE_3_4:
            MTC.setBeat(3);
            break;
          case PED_TIMESIGNATURE_3_8:
            MTC.setBeat(3);
            break;
          case PED_TIMESIGNATURE_6_8:
            MTC.setBeat(6);
            break;
          case PED_TIMESIGNATURE_9_8:
            MTC.setBeat(9);
            break;
          case PED_TIMESIGNATURE_12_8:
            MTC.setBeat(12);
            break;
          default:
            MTC.setBeat(4);
            break;
        }
      }
      break;

    case II_BACKLIGHT:
      if (bGet) vBuf.value = backlight / 25;
      else {
        backlight = vBuf.value * 25;
        //lcd.setBacklight(backlight);
        analogWrite(LCD_BACKLIGHT, backlight);
      }
      break;

    case II_IRLEARN:
      if (!bGet) {
        unsigned long startMillis;
        unsigned long elapsedTime;
        for (byte c = 0; c < IR_CUSTOM_CODES; c++)
        {
          // empty IR buffer
          startMillis = millis();
          elapsedTime = 0;
          while (elapsedTime <= 100) {
            if (irrecv.decode(&results)) irrecv.resume();
            elapsedTime = millis() - startMillis;
          }

          lcd.clear();
          lcd.setCursor(0, 0);
          switch (c) {
            case IRC_ON_OFF:
              lcd.print("Press ON/OFF");
              break;
            case IRC_OK:
              lcd.print("Press OK");
              break;
            case IRC_ESC:
              lcd.print("Press ESC");
              break;
            case IRC_LEFT:
              lcd.print("Press LEFT");
              break;
            case IRC_RIGHT:
              lcd.print("Press RIGHT");
              break;
            case IRC_UP:
              lcd.print("Press UP");
              break;
            case IRC_DOWN:
              lcd.print("Press DOWN");
              break;
            case IRC_SWITCH:
              lcd.print("Press SWITCH");
              break;
            case IRC_KEY_1:
              lcd.print("Press 1");
              break;
            case IRC_KEY_2:
              lcd.print("Press 2");
              break;
            case IRC_KEY_3:
              lcd.print("Press 3");
              break;
            case IRC_KEY_4:
              lcd.print("Press 4");
              break;
            case IRC_KEY_5:
              lcd.print("Press 5");
              break;
            case IRC_KEY_6:
              lcd.print("Press 6");
              break;
            case IRC_KEY_7:
              lcd.print("Press 7");
              break;
            case IRC_KEY_8:
              lcd.print("Press 8");
              break;
            case IRC_KEY_9:
              lcd.print("Press 9");
              break;
            case IRC_KEY_0:
              lcd.print("Press 0");
              break;
          }
          startMillis = millis();
          elapsedTime  = 0;
          while (!irrecv.decode(&results) && elapsedTime <= 10000) {            // wait an IR code or timeout
            lcd.setCursor(map(elapsedTime, 0, 10000, 0, LCD_COLS - 1), 1);
            lcd.print(char(B10100101));
            elapsedTime = millis() - startMillis;
          }
          if (elapsedTime <= 10000) {
            ircustomcode[c] = results.value;
            lcd.setCursor(0, 1);
            lcd.print(results.value, HEX);
            delay(300);
            irrecv.resume();
          }
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Remote control");
        lcd.setCursor(0, 1);
        lcd.print("   saving...");
        update_eeprom();
        lcd.setCursor(0, 1);
        lcd.print("    saved");
        delay(1000);
      }
      r = nullptr;
      break;

    case II_IRCLEAR:
      if (!bGet) {
        for (byte c = 0; c < IR_CUSTOM_CODES; c++) ircustomcode[c] = 0xFFFFFE;
      }
      r = nullptr;
      break;

    case II_WIFIRESET:
      if (!bGet) {
        serialize_factory_default();
      }
      r = nullptr;
      break;

    case II_SERIALPASS:
      if (!bGet) {
        serialPassthrough = true;
        Serial.end();
        Serial3.end();
        Serial.begin(115200);
        Serial3.begin(115200);
      }
      r = nullptr;
      break;

    case II_DEFAULT:
      if (!bGet) {
        lcd.clear();
        // Sets all of the bytes of the EEPROM to 0.
        for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
          EEPROM.write(i, 0);
          lcd.setCursor(map(i, 0, EEPROM.length(), 0, LCD_COLS - 1), 0);
          lcd.print(char(B10100101));
        }
        Reset_AVR();
      }

    default:
      r = nullptr;
      break;
  }

  if (!bGet && id != II_PROFILE_LOAD && id != II_IRLEARN && id != II_WIFIRESET) {
    update_eeprom();
    controller_setup();
  }

  return (r);
}

bool display(MD_Menu::userDisplayAction_t action, char *msg)
{
  char line[LCD_COLS + 1];

  switch (action)
  {
    case MD_Menu::DISP_INIT:
      lcd.begin(LCD_COLS, LCD_ROWS);
      for (byte i = 0; i < 4; i++)
        lcd.createChar(i, partial_bar[i]);
      lcd.createChar(POWERPLUG, power_plug);
      lcd.createChar(BATTERYLEVEL, battery[0]);
      lcd.createChar(WIFIICON, wifi_icon);
      lcd.createChar(BLUETOOTHICON, bluetooth_icon);
      lcd.clear();
      lcd.noCursor();
#ifndef NOBLYNK
      blynkLCD.clear();
#endif
      break;

    case MD_Menu::DISP_CLEAR:
      lcd.clear();
#ifndef NOBLYNK
      blynkLCD.clear();
#endif
      break;

    case MD_Menu::DISP_L0:
      memset(line, 0, LCD_COLS + 1);
      if (strcmp(msg, "Pedals Setup") == 0)
        sprintf(line, "%s %2d", "Pedal", currentPedal + 1);
      else if (strcmp(msg, "Banks Setup") == 0)
        sprintf(line, "%s %2d %s %2d", "Bank", currentBank + 1, "Pedal", currentPedal + 1);
      else if (strcmp(msg, "Interface Setup") == 0)
        switch (currentInterface) {
          case PED_USBMIDI:
            strcpy(line, "USB MIDI");
            break;
          case PED_DINMIDI:
            strcpy(line, "Legacy MIDI");
            break;
          case PED_RTPMIDI:
            strcpy(line, "AppleMIDI");
            break;
          case PED_IPMIDI:
            strcpy(line, "ipMIDI");
            break;
          case PED_BLEMIDI:
            strcpy(line, "Bluetooth MIDI");
            break;
          case PED_OSC:
            strcpy(line, "OSC");
            break;
        }
      else
        strcpy(line, msg);
      for (int i = strlen(line); i < LCD_COLS; i++)
        line[i] = ' ';
      lcd.setCursor(0, 0);
      lcd.print(line);
#ifndef NOBLYNK
      blynkLCD.print(0, 0, line);
#endif
      break;

    case MD_Menu::DISP_L1:
      memset(line, 0, LCD_COLS + 1);
      strcpy(line, msg);
      for (int i = strlen(line); i < LCD_COLS; i++)
        line[i] = ' ';
      lcd.setCursor(0, 1);
      lcd.print(line);
#ifndef NOBLYNK
      blynkLCD.print(0, 1, line);
#endif
      break;
  }

  return (true);
}


MD_Menu::userNavAction_t navigation(uint16_t &incDelta)
{
  incDelta = 1;
  static unsigned long  previous_value;
  static byte           count = 0;
  unsigned long         ircode;
  byte                  numberPressed = 0;

  MD_UISwitch::keyResult_t k1;    // Close status between T and S
  MD_UISwitch::keyResult_t k2;    // Close status between R and S
  byte                     k;     /*       k1      k2
                                     0 =  Open    Open
                                     1 = Closed   Open
                                     2 =  Open   Closed
                                     3 = Closed  Closed */

  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].function == PED_MIDI) continue;

    k = 0;
    k1 = MD_UISwitch::KEY_NULL;
    k2 = MD_UISwitch::KEY_NULL;
    if (pedals[i].footSwitch[0] != nullptr) k1 = pedals[i].footSwitch[0]->read();
    if (pedals[i].footSwitch[1] != nullptr) k2 = pedals[i].footSwitch[1]->read();
    if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) && k2 == MD_UISwitch::KEY_NULL) k = 1;
    if ((k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS) && k1 == MD_UISwitch::KEY_NULL) k = 2;
    if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) &&
        (k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS)) k = 3;
    if (k > 0 && (k1 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_PRESS)) {
      // Single press
      if (pedals[i].mode == PED_LADDER) {
        if (k1 != MD_UISwitch::KEY_NULL) {
          switch (pedals[i].footSwitch[0]->getKey()) {

            case 'S':
              return MD_Menu::NAV_SEL;
              break;

            case 'L':
              if (M.isInMenu())
                return MD_Menu::NAV_ESC;
              else if (MTC.isPlaying()) 
                MTC.sendStop();
              else
                MTC.sendPosition(0, 0, 0, 0);
              return MD_Menu::NAV_NULL;
              break;

            case 'U':
              if (M.isInMenu())
                return MD_Menu::NAV_INC;
              else if (currentBank < BANKS - 1) currentBank++;
              return MD_Menu::NAV_NULL;
              break;

            case 'D':
              if (M.isInMenu())
                return MD_Menu::NAV_DEC;
              else if (currentBank > 0) currentBank--;
              return MD_Menu::NAV_NULL;
              break;

            case 'R':
              if (M.isInMenu())
                return MD_Menu::NAV_NULL;
              else if (MTC.isPlaying())
                MTC.sendStop();
              else if (MTC.getFrames() == 0 && MTC.getSeconds() == 0 && MTC.getMinutes() == 0 && MTC.getHours() == 0)
                MTC.sendPlay();
              else
                MTC.sendContinue();
              return MD_Menu::NAV_NULL;
              break;
          }
        }
      }
      else {
        switch (pedals[i].function) {
        case PED_BANK_PLUS:
          switch (k) {
            case 1:
              if (currentBank < BANKS - 1) currentBank++;
              break;
            case 2:
              if (currentBank > 0) currentBank--;
              break;
            case 3:
              break;
          }
          break;

        case PED_BANK_MINUS:
          switch (k) {
            case 1:
              if (currentBank > 0) currentBank--;
              break;
            case 2:
              if (currentBank < BANKS - 1) currentBank++;
              break;
            case 3:
              break;
          }
          break;

        case PED_START:
          switch (k) {
            case 1:
              MTC.sendPosition(0, 0, 0, 0);
              MTC.sendPlay();
              break;
            case 2:
              bpm = MTC.tapTempo();
              break;
            case 3:
              break;
          }
          break;


        case PED_STOP:
          switch (k) {
            case 1:
              MTC.sendStop();
              break;
            case 2:
              bpm = MTC.tapTempo();
              if (bpm > 0) MTC.setBpm(bpm);
              break;
            case 3:
              break;
          }
          break;

        case PED_CONTINUE:
          switch (k) {
            case 1:
              MTC.sendContinue();
              break;
            case 2:
              bpm = MTC.tapTempo();
              break;
            case 3:
              break;
          }
          break;

        case PED_TAP:
          switch (k) {
            case 1:
              bpm = MTC.tapTempo();
              if (bpm > 0) MTC.setBpm(bpm);
              break;
            case 2:
              MTC.sendPlay();
              break;
            case 3:
              MTC.sendStop();
              break;
          }
          break;

        case PED_MENU:
          switch (k) {
            case 1:
              return MD_Menu::NAV_INC;
            case 2:
              return MD_Menu::NAV_DEC;
            case 3:
              return MD_Menu::NAV_ESC;
          }
          break;

        case PED_CONFIRM:
          return MD_Menu::NAV_SEL;
          break;
        case PED_ESCAPE:
          return MD_Menu::NAV_ESC;
          break;
        case PED_NEXT:
          return MD_Menu::NAV_INC;
          break;
        case PED_PREVIOUS:
          return MD_Menu::NAV_DEC;
          break;
        }
      }
    }

    // Double press, long press and repeat
    if (pedals[i].footSwitch[0] != nullptr)
      switch (k1) {
        case MD_UISwitch::KEY_NULL:
          pedals[i].footSwitch[0]->setDoublePressTime(300);
          pedals[i].footSwitch[0]->setLongPressTime(500);
          pedals[i].footSwitch[0]->setRepeatTime(500);
          pedals[i].footSwitch[0]->enableDoublePress(true);
          pedals[i].footSwitch[0]->enableLongPress(true);
          break;
        case MD_UISwitch::KEY_RPTPRESS:
          pedals[i].footSwitch[0]->setDoublePressTime(0);
          pedals[i].footSwitch[0]->setLongPressTime(0);
          pedals[i].footSwitch[0]->setRepeatTime(10);
          pedals[i].footSwitch[0]->enableDoublePress(false);
          pedals[i].footSwitch[0]->enableLongPress(false);
          break;
        case MD_UISwitch::KEY_DPRESS:
          if (pedals[i].function == PED_MENU) return MD_Menu::NAV_SEL;
          break;
        case MD_UISwitch::KEY_LONGPRESS:
          if (pedals[i].function == PED_MENU) return MD_Menu::NAV_ESC;
          break;
        case MD_UISwitch::KEY_PRESS:
          break;
      }

    if (pedals[i].footSwitch[1] != nullptr)
      switch (k2) {
        case MD_UISwitch::KEY_NULL:
          pedals[i].footSwitch[1]->setDoublePressTime(300);
          pedals[i].footSwitch[1]->setLongPressTime(500);
          pedals[i].footSwitch[1]->setRepeatTime(500);
          pedals[i].footSwitch[1]->enableDoublePress(true);
          pedals[i].footSwitch[1]->enableLongPress(true);
          break;
        case MD_UISwitch::KEY_RPTPRESS:
          pedals[i].footSwitch[1]->setDoublePressTime(0);
          pedals[i].footSwitch[1]->setLongPressTime(0);
          pedals[i].footSwitch[1]->setRepeatTime(10);
          pedals[i].footSwitch[1]->enableDoublePress(false);
          pedals[i].footSwitch[1]->enableLongPress(false);
          break;
        case MD_UISwitch::KEY_DPRESS:
          if (pedals[i].function == PED_MENU) return MD_Menu::NAV_SEL;
          break;
        case MD_UISwitch::KEY_LONGPRESS:
          if (pedals[i].function == PED_MENU) return MD_Menu::NAV_ESC;
          break;
        case MD_UISwitch::KEY_PRESS:
          break;
      }
  }

  // IR Remote Control navigation

  if (irrecv.decode(&results)) {
    ircode = results.value;
    irrecv.resume();
    if (ircode == REPEAT) count++;
    else {
      count = 0;
      previous_value = ircode;
    }
    if (count > REPEAT_TO_SKIP) ircode = previous_value;

    switch (results.decode_type) {
      default:
      case UNKNOWN:      DPRINTF("UNKNOWN");       break ;
      case NEC:          DPRINTF("NEC");           break ;
      case SONY:         DPRINTF("SONY");          break ;
      case RC5:          DPRINTF("RC5");           break ;
      case RC6:          DPRINTF("RC6");           break ;
      case DISH:         DPRINTF("DISH");          break ;
      case SHARP:        DPRINTF("SHARP");         break ;
      case JVC:          DPRINTF("JVC");           break ;
      case SANYO:        DPRINTF("SANYO");         break ;
      case MITSUBISHI:   DPRINTF("MITSUBISHI");    break ;
      case SAMSUNG:      DPRINTF("SAMSUNG");       break ;
      case LG:           DPRINTF("LG");            break ;
      case WHYNTER:      DPRINTF("WHYNTER");       break ;
      case AIWA_RC_T501: DPRINTF("AIWA_RC_T501");  break ;
      case PANASONIC:    DPRINTF("PANASONIC");     break ;
      case DENON:        DPRINTF("Denon");         break ;
    }
    DPRINTF(" IR Code: 0x");
    DPRINTLN2(ircode, HEX);

    if      (ircode == ircustomcode[IRC_ON_OFF])  ircode = IR_ON_OFF;
    else if (ircode == ircustomcode[IRC_OK])      ircode = IR_OK;
    else if (ircode == ircustomcode[IRC_ESC])     ircode = IR_ESC;
    else if (ircode == ircustomcode[IRC_LEFT])    ircode = IR_LEFT;
    else if (ircode == ircustomcode[IRC_RIGHT])   ircode = IR_RIGHT;
    else if (ircode == ircustomcode[IRC_UP])      ircode = IR_UP;
    else if (ircode == ircustomcode[IRC_DOWN])    ircode = IR_DOWN;
    else if (ircode == ircustomcode[IRC_SWITCH])  ircode = IR_SWITCH;
    else if (ircode == ircustomcode[IRC_KEY_1])   ircode = IR_KEY_1;
    else if (ircode == ircustomcode[IRC_KEY_2])   ircode = IR_KEY_2;
    else if (ircode == ircustomcode[IRC_KEY_3])   ircode = IR_KEY_3;
    else if (ircode == ircustomcode[IRC_KEY_4])   ircode = IR_KEY_4;
    else if (ircode == ircustomcode[IRC_KEY_5])   ircode = IR_KEY_5;
    else if (ircode == ircustomcode[IRC_KEY_6])   ircode = IR_KEY_6;
    else if (ircode == ircustomcode[IRC_KEY_7])   ircode = IR_KEY_7;
    else if (ircode == ircustomcode[IRC_KEY_8])   ircode = IR_KEY_8;
    else if (ircode == ircustomcode[IRC_KEY_9])   ircode = IR_KEY_9;
    else if (ircode == ircustomcode[IRC_KEY_0])   ircode = IR_KEY_0;

    switch (ircode) {
      case IR_ON_OFF:
        (powersaver) ? lcd.on() : lcd.off();
        powersaver = !powersaver;
        return MD_Menu::NAV_NULL;

      case IR_OK:
        return MD_Menu::NAV_SEL;

      case IR_ESC:
        return MD_Menu::NAV_ESC;

      case IR_RIGHT:
      case IR_DOWN:
        return MD_Menu::NAV_DEC;

      case IR_LEFT:
      case IR_UP:
        return MD_Menu::NAV_INC;

      case IR_KEY_1:
        numberPressed = 1;
        break;
      case IR_KEY_2:
        numberPressed = 2;
        break;
      case IR_KEY_3:
        numberPressed = 3;
        break;
      case IR_KEY_4:
        numberPressed = 4;
        break;
      case IR_KEY_5:
        numberPressed = 5;
        break;
      case IR_KEY_6:
        numberPressed = 6;
        break;
      case IR_KEY_7:
        numberPressed = 7;
        break;
      case IR_KEY_8:
        numberPressed = 8;
        break;
      case IR_KEY_9:
        numberPressed = 9;
        break;
      case IR_KEY_0:
        numberPressed = 10;
        break;

      case IR_SWITCH:
        selectBank = !selectBank;
        break;
    }

    if (numberPressed != 0) {                                               // number pressed
      if (selectBank) {                                                     // select the bank
        if (numberPressed >= 1 && numberPressed <= BANKS) {
          currentBank = numberPressed - 1;
          update_eeprom();
          controller_setup();
        }
      }
      else if (numberPressed >= 1 && numberPressed <= PEDALS) {             // simulate pedal push
        lastUsedSwitch = numberPressed - 1;
        midi_send(banks[currentBank][lastUsedSwitch].midiMessage,
                  banks[currentBank][lastUsedSwitch].midiCode,
                  banks[currentBank][lastUsedSwitch].midiValue1,
                  banks[currentBank][lastUsedSwitch].midiChannel);
        pedals[lastUsedSwitch].pedalValue[0] = LOW;
        pedals[lastUsedSwitch].lastUpdate[0] = millis();
        screen_update();
        delay(10);
        midi_send(banks[currentBank][lastUsedSwitch].midiMessage,
                  banks[currentBank][lastUsedSwitch].midiCode,
                  banks[currentBank][lastUsedSwitch].midiValue2,
                  banks[currentBank][lastUsedSwitch].midiChannel,
                  pedals[lastUsedSwitch].mode == PED_LATCH1 || pedals[lastUsedSwitch].mode == PED_LATCH2);
        pedals[lastUsedSwitch].pedalValue[0] = HIGH;
        pedals[lastUsedSwitch].lastUpdate[0] = millis();
        screen_update();
      }
    }
  }

  return MD_Menu::NAV_NULL;
}


void menu_setup() 
{
  irrecv.enableIRIn();                            // Start the IR receiver
  irrecv.blink13(true);
  
  pinMode(LCD_BACKLIGHT, OUTPUT);
  analogWrite(LCD_BACKLIGHT, backlight);

  display(MD_Menu::DISP_INIT);
  M.begin();
  M.setMenuWrap(true);
  M.setAutoStart(AUTO_START);
#ifdef MENU_TIMEOUT
  M.setTimeout(MENU_TIMEOUT);
#endif
}


void menu_run() 
{
  static bool prevMenuRun = true;

  // Detect if we need to initiate running normal user code
  if (prevMenuRun && !M.isInMenu())
    screen_update(true);
  prevMenuRun = M.isInMenu();

  // If we are not running and not autostart
  // check if there is a reason to start the menu
  if (!M.isInMenu() && !AUTO_START)
  {
    uint16_t dummy;

    if (navigation(dummy) == MD_Menu::NAV_SEL)
      M.runMenu(true);
  }
  if (!M.isInMenu())
    screen_update();
  else
    lcd.noCursor();

  M.runMenu();   // just run the menu code
}


#endif  // NOLCD

void menu_navigation()
{
  MD_UISwitch::keyResult_t k1;    // Close status between T and S
  MD_UISwitch::keyResult_t k2;    // Close status between R and S
  byte                     k;     /*       k1      k2
                                     0 =  Open    Open
                                     1 = Closed   Open
                                     2 =  Open   Closed
                                     3 = Closed  Closed */

  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].function == PED_MIDI) continue;

    k = 0;
    k1 = MD_UISwitch::KEY_NULL;
    k2 = MD_UISwitch::KEY_NULL;
    if (pedals[i].footSwitch[0] != nullptr) k1 = pedals[i].footSwitch[0]->read();
    if (pedals[i].footSwitch[1] != nullptr) k2 = pedals[i].footSwitch[1]->read();
    if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) && k2 == MD_UISwitch::KEY_NULL) k = 1;
    if ((k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS) && k1 == MD_UISwitch::KEY_NULL) k = 2;
    if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) &&
        (k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS)) k = 3;
    if (k > 0 && (k1 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_PRESS)) {
      DPRINTLN("Press %d %d %d\n", k, k1, k2)
      // Single press
      if (pedals[i].mode == PED_LADDER) {
        if (k1 != MD_UISwitch::KEY_NULL) {
          switch (pedals[i].footSwitch[0]->getKey()) {
/*
            case 'S':
              return MD_Menu::NAV_SEL;
              break;

            case 'L':
              if (M.isInMenu())
                return MD_Menu::NAV_ESC;
              else if (MTC.isPlaying()) 
                MTC.sendStop();
              else
                MTC.sendPosition(0, 0, 0, 0);
              return MD_Menu::NAV_NULL;
              break;

            case 'U':
              if (M.isInMenu())
                return MD_Menu::NAV_INC;
              else if (currentBank < BANKS - 1) currentBank++;
              return MD_Menu::NAV_NULL;
              break;

            case 'D':
              if (M.isInMenu())
                return MD_Menu::NAV_DEC;
              else if (currentBank > 0) currentBank--;
              return MD_Menu::NAV_NULL;
              break;

            case 'R':
              if (M.isInMenu())
                return MD_Menu::NAV_NULL;
              else if (MTC.isPlaying())
                MTC.sendStop();
              else if (MTC.getFrames() == 0 && MTC.getSeconds() == 0 && MTC.getMinutes() == 0 && MTC.getHours() == 0)
                MTC.sendPlay();
              else
                MTC.sendContinue();
              return MD_Menu::NAV_NULL;
              break;
*/
          }
        }
      }
      else {
        switch (pedals[i].function) {
        case PED_BANK_PLUS:
          switch (k) {
            case 1:
              currentBank = (currentBank + 1) % BANKS;
              break;
            case 2:
              if (currentBank > 0) currentBank--;
              else currentBank = BANKS - 1;
              break;
            case 3:
              currentBank = 0;
              break;
          }
          break;

        case PED_BANK_MINUS:
          switch (k) {
            case 1:
              if (currentBank > 0) currentBank--;
              else currentBank = BANKS - 1;
              break;
            case 2:
              currentBank = (currentBank + 1) % BANKS;
              break;
            case 3:
              currentBank = BANKS - 1;
              break;
          }
          break;

        case PED_START:
          switch (k) {
            case 1:
              MTC.sendPosition(0, 0, 0, 0);
              MTC.sendPlay();
              break;
            case 2:
              bpm = MTC.tapTempo();
              break;
            case 3:
              break;
          }
          break;


        case PED_STOP:
          switch (k) {
            case 1:
              MTC.sendStop();
              break;
            case 2:
              bpm = MTC.tapTempo();
              if (bpm > 0) MTC.setBpm(bpm);
              break;
            case 3:
              break;
          }
          break;

        case PED_CONTINUE:
          switch (k) {
            case 1:
              MTC.sendContinue();
              break;
            case 2:
              bpm = MTC.tapTempo();
              break;
            case 3:
              break;
          }
          break;

        case PED_TAP:
          switch (k) {
            case 1:
              bpm = MTC.tapTempo();
              if (bpm > 0) MTC.setBpm(bpm);
              break;
            case 2:
              MTC.sendPlay();
              break;
            case 3:
              MTC.sendStop();
              break;
          }
          break;
/*
        case PED_MENU:
          switch (k) {
            case 1:
              return MD_Menu::NAV_INC;
            case 2:
              return MD_Menu::NAV_DEC;
            case 3:
              return MD_Menu::NAV_ESC;
          }
          break;

        case PED_CONFIRM:
          return MD_Menu::NAV_SEL;
          break;
        case PED_ESCAPE:
          return MD_Menu::NAV_ESC;
          break;
        case PED_NEXT:
          return MD_Menu::NAV_INC;
          break;
        case PED_PREVIOUS:
          return MD_Menu::NAV_DEC;
          break;
*/
        }
      }
    }

    // Double press, long press and repeat
    if (pedals[i].footSwitch[0] != nullptr)
      switch (k1) {
        case MD_UISwitch::KEY_NULL:
          pedals[i].footSwitch[0]->setDoublePressTime(300);
          pedals[i].footSwitch[0]->setLongPressTime(500);
          pedals[i].footSwitch[0]->setRepeatTime(500);
          pedals[i].footSwitch[0]->enableDoublePress(true);
          pedals[i].footSwitch[0]->enableLongPress(true);
          break;
        case MD_UISwitch::KEY_RPTPRESS:
          pedals[i].footSwitch[0]->setDoublePressTime(0);
          pedals[i].footSwitch[0]->setLongPressTime(0);
          pedals[i].footSwitch[0]->setRepeatTime(10);
          pedals[i].footSwitch[0]->enableDoublePress(false);
          pedals[i].footSwitch[0]->enableLongPress(false);
          break;
        case MD_UISwitch::KEY_DPRESS:
          //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_SEL;
          break;
        case MD_UISwitch::KEY_LONGPRESS:
          //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_ESC;
          break;
        case MD_UISwitch::KEY_PRESS:
          break;
      }

    if (pedals[i].footSwitch[1] != nullptr)
      switch (k2) {
        case MD_UISwitch::KEY_NULL:
          pedals[i].footSwitch[1]->setDoublePressTime(300);
          pedals[i].footSwitch[1]->setLongPressTime(500);
          pedals[i].footSwitch[1]->setRepeatTime(500);
          pedals[i].footSwitch[1]->enableDoublePress(true);
          pedals[i].footSwitch[1]->enableLongPress(true);
          break;
        case MD_UISwitch::KEY_RPTPRESS:
          pedals[i].footSwitch[1]->setDoublePressTime(0);
          pedals[i].footSwitch[1]->setLongPressTime(0);
          pedals[i].footSwitch[1]->setRepeatTime(10);
          pedals[i].footSwitch[1]->enableDoublePress(false);
          pedals[i].footSwitch[1]->enableLongPress(false);
          break;
        case MD_UISwitch::KEY_DPRESS:
          //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_SEL;
          break;
        case MD_UISwitch::KEY_LONGPRESS:
          //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_ESC;
          break;
        case MD_UISwitch::KEY_PRESS:
          break;
      }
  }
}
