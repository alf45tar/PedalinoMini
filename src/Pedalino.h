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

#include <Arduino.h>

#ifndef _PEDALINO_H
#define _PEDALINO_H

#define MODEL           "PedalinoMini™"
#define INTERFACES        6
#define PROFILES          3
#define BANKS            20
#define PEDALS            6
#define SEQUENCES        16
#define STEPS            10   // number of steps for each sequence
#define LADDER_STEPS      6   // max number of switches in a resistor ladder
#define LEDS             18

#define MAXACTIONNAME    10
#define MAXBANKNAME      10

// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// GPIOs 34 to 39 are GPIs – input only pins.
// These pins don’t have internal pull-ups or pull-down resistors.
const byte pinD[] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13};
const byte pinA[] = {GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32, GPIO_NUM_33};

#define PIN_D(x)          pinD[x]
#define PIN_A(x)          pinA[x]

#ifdef TTGO_T_EIGHT
#define FACTORY_DEFAULT_PIN   GPIO_NUM_38   // Right 37   Center 38   Left 39
#define RIGHT_PIN             GPIO_NUM_37
#define CENTER_PIN            GPIO_NUM_38
#define LEFT_PIN              GPIO_NUM_39   // Shared with A2
#define SERIAL_DATA_PIN       GPIO_NUM_2    // DS
#define CLOCK_PIN             GPIO_NUM_2    // SH_CP
#define LATCH_PIN             GPIO_NUM_2    // ST_CP
#define USB_MIDI_IN_PIN       GPIO_NUM_18   // Used by SD
#define USB_MIDI_OUT_PIN      GPIO_NUM_19   // Used by SD
#define DIN_MIDI_IN_PIN       GPIO_NUM_15
#define DIN_MIDI_OUT_PIN      GPIO_NUM_4
#define BATTERY_PIN           GPIO_NUM_36   // GPIO_NUM_32 to GPIO_NUM_39 only
#else
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0
#define RIGHT_PIN             GPIO_NUM_23
#define CENTER_PIN            GPIO_NUM_0
#define LEFT_PIN              GPIO_NUM_23
#define SERIAL_DATA_PIN       GPIO_NUM_16   // DS
#define CLOCK_PIN             GPIO_NUM_5    // SH_CP
#define LATCH_PIN             GPIO_NUM_17   // ST_CP
#define USB_MIDI_IN_PIN       GPIO_NUM_18
#define USB_MIDI_OUT_PIN      GPIO_NUM_19
#define DIN_MIDI_IN_PIN       GPIO_NUM_15
#define DIN_MIDI_OUT_PIN      GPIO_NUM_4
#define BATTERY_PIN           GPIO_NUM_36   // GPIO_NUM_32 to GPIO_NUM_39 only
#endif

#include "FastLED.h"
#define FASTLEDS_DATA_PIN  GPIO_NUM_5
CRGB fastleds[LEDS];

#include "ShiftOut.h"

#define NUMBER_OF_SHIFT_REGISTERS 1

ShiftOut<NUMBER_OF_SHIFT_REGISTERS> leds;

// Serial MIDI interfaces

#include <MIDI.h>                       // https://github.com/FortySevenEffects/arduino_midi_library

#define MIDI_BAUD_RATE                  31250
#define HIGH_SPEED_SERIAL_BAUD_RATE     1000000

#define SERIAL_MIDI_USB   Serial1
#define SERIAL_MIDI_DIN   Serial2

MIDI_CREATE_INSTANCE(HardwareSerial, SERIAL_MIDI_USB, USB_MIDI);
MIDI_CREATE_INSTANCE(HardwareSerial, SERIAL_MIDI_DIN, DIN_MIDI);


typedef uint8_t   byte;

#include <ResponsiveAnalogRead.h>       // https://github.com/dxinteractive/ResponsiveAnalogRead
#include <MD_REncoder.h>                // https://github.com/MajicDesigns/MD_REncoder
#include <AceButton.h>                  // https://github.com/bxparks/AceButton
using namespace ace_button;

#define DEBOUNCE_INTERVAL      20
#define PED_PRESS_TIME        200
#define PED_DOUBLE_PRESS_TIME 400
#define PED_LONG_PRESS_TIME   500
#define PED_REPEAT_PRESS_TIME 500

#include "MidiTimeCode.h"

#define PED_BOOT_UNKNOWN        0
#define PED_BOOT_NORMAL         1
#define PED_BOOT_BLE            2
#define PED_BOOT_WIFI           3
#define PED_BOOT_AP             4
#define PED_BOOT_AP_NO_BLE      5
#define PED_BOOT_RESET_WIFI     6
#define PED_BOOT_LADDER_CONFIG  7
#define PED_FACTORY_DEFAULT     8

#define PED_EMPTY               midi::InvalidType
#define PED_PROGRAM_CHANGE      midi::ProgramChange
#define PED_CONTROL_CHANGE      midi::ControlChange
#define PED_NOTE_ON             midi::NoteOn
#define PED_NOTE_OFF            midi::NoteOff
#define PED_BANK_SELECT_INC     4
#define PED_BANK_SELECT_DEC     5
#define PED_PROGRAM_CHANGE_INC  6
#define PED_PROGRAM_CHANGE_DEC  7
#define PED_PITCH_BEND          midi::PitchBend
#define PED_CHANNEL_PRESSURE    midi::AfterTouchChannel
#define PED_MIDI_START          midi::Start
#define PED_MIDI_STOP           midi::Stop
#define PED_MIDI_CONTINUE       midi::Continue
#define PED_SEQUENCE            20
#define PED_ACTION_BANK_PLUS    21
#define PED_ACTION_BANK_MINUS   22
#define PED_ACTION_START        23
#define PED_ACTION_STOP         24
#define PED_ACTION_CONTINUE     25
#define PED_ACTION_TAP          26
#define PED_ACTION_BPM_PLUS     27
#define PED_ACTION_BPM_MINUS    29

#define PED_NONE                1
#define PED_MOMENTARY1          2
#define PED_LATCH1              3
#define PED_ANALOG              4
#define PED_JOG_WHEEL           5
#define PED_MOMENTARY2          6
#define PED_MOMENTARY3          7
#define PED_LATCH2              8
#define PED_LADDER              9

#define PED_PRESS_1             1
#define PED_PRESS_2             2
#define PED_PRESS_L             4
#define PED_PRESS_1_2           PED_PRESS_1 + PED_PRESS_2
#define PED_PRESS_1_L           PED_PRESS_1 + PED_PRESS_L
#define PED_PRESS_2_L           PED_PRESS_2 + PED_PRESS_L
#define PED_PRESS_1_2_L         PED_PRESS_1 + PED_PRESS_2 + PED_PRESS_L

#define PED_MIDI                1
#define PED_BANK_PLUS           2
#define PED_BANK_MINUS          3
#define PED_START               4
#define PED_STOP                5
#define PED_CONTINUE            6
#define PED_TAP                 7
#define PED_BPM_PLUS            8
#define PED_BPM_MINUS           9
#define PED_BANK_PLUS_2        10
#define PED_BANK_MINUS_2       11
#define PED_BANK_PLUS_3        12
#define PED_BANK_MINUS_3       13

#define PED_EVENT_PRESS         AceButton::kEventPressed
#define PED_EVENT_RELEASE       AceButton::kEventReleased
#define PED_EVENT_CLICK         AceButton::kEventClicked
#define PED_EVENT_DOUBLE_CLICK  AceButton::kEventDoubleClicked
#define PED_EVENT_LONG_PRESS    AceButton::kEventLongPressed
#define PED_EVENT_MOVE          6
#define PED_EVENT_JOG           7

#define PED_LINEAR              0
#define PED_LOG                 1
#define PED_ANTILOG             2

#define PED_USBMIDI             0
#define PED_DINMIDI             1
#define PED_RTPMIDI             2
#define PED_IPMIDI              3
#define PED_BLEMIDI             4
#define PED_OSC                 5

#define PED_DISABLE             0
#define PED_ENABLE              1

#define PED_MTC_NONE            0
#define PED_MTC_SLAVE           1
#define PED_MTC_MASTER_24       2
#define PED_MTC_MASTER_25       3
#define PED_MTC_MASTER_30DF     4
#define PED_MTC_MASTER_30       5
#define PED_MIDI_CLOCK_SLAVE    6
#define PED_MIDI_CLOCK_MASTER   7

#define PED_TIMESIGNATURE_4_4   0
#define PED_TIMESIGNATURE_3_4   1
#define PED_TIMESIGNATURE_2_4   2
#define PED_TIMESIGNATURE_3_8   3
#define PED_TIMESIGNATURE_6_8   4
#define PED_TIMESIGNATURE_9_8   5
#define PED_TIMESIGNATURE_12_8  6

#define MIDI_RESOLUTION         128       // MIDI 7-bit CC resolution
#define ADC_RESOLUTION         1024
#define ADC_RESOLUTION_BITS      10       // hardware 9 to 12-bit ADC converter resolution
                                          // software 1 to 16-bit resolution
#define CALIBRATION_DURATION   8000       // milliseconds

struct action {
  char                   tag0[MAXACTIONNAME+1];
  union {
    char                 tag1[MAXACTIONNAME+1];
    char                 name[MAXACTIONNAME+1];
  };
  byte                   pedal;
  byte                   button;
  byte                   led;
  uint32_t               color0;
  uint32_t               color1;
  byte                   event;
  byte                   midiMessage;     /*  1 = Program Change,
                                              2 = Control Code
                                              3 = Note On/Note Off
                                              4 = Bank Select+
                                              5 = Bank Select-
                                              6 = Program Change+
                                              7 = Program Change-
                                              8 = Pitch Bend
                                              9 = Channel Pressure
                                             10 = Start
                                             11 = Stop
                                             12 = Continue
                                             13 = Sequence */
  byte                   midiChannel;     /* MIDI channel 1-16 */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue1;
  byte                   midiValue2;
  action                *next;
};

struct bank {
  char                   pedalName[MAXACTIONNAME+1];
  byte                   midiMessage;
  byte                   midiChannel;
  byte                   midiCode;
  byte                   midiValue1;
  byte                   midiValue2;
};

struct pedal {
  byte                   function;        /*  1 = None (use Actions)
                                              2 = Bank+
                                              3 = Bank-
                                              4 = Start
                                              5 = Stop
                                              6 = Continue
                                              7 = Tap
                                              8 = BPM+
                                              9 = BPM-
                                             10 = Bank+2
                                             11 = Bank-2
                                             12 = Bank+3
                                             13 = Bank-3 */
  byte                   autoSensing;     /* 0 = disable
                                             1 = enable   */
  byte                   mode;            /* 1 = none
                                             2 = momentary
                                             3 = latch
                                             4 = analog
                                             5 = jog wheel
                                             6 = momentary 2
                                             7 = momentary 3
                                             8 = latch 2
                                             9 = ladder */
  byte                   pressMode;       /* 1 = single click
                                             2 = double click
                                             4 = long click
                                             3 = single and double click
                                             5 = single and long click
                                             6 = double and long click
                                             7 = single, double and long click */
  byte                   invertPolarity;
  byte                   mapFunction;
  int                    expZero;           // [0, ADC_RESOLUTION-1]
  int                    expMax;            // [0, ADC_RESOLUTION-1]
  int                    pedalValue[2];     // [0, ADC_RESOLUTION-1]
  unsigned long          lastUpdate[2];     // last time the value is changed
  AceButton             *button[LADDER_STEPS];
  ButtonConfig          *buttonConfig;
  MD_REncoder           *jogwheel;
  ResponsiveAnalogRead  *analogPedal;
};

struct interface {
  char                   name[12];
  byte                   midiIn;          // 0 = disable, 1 = enable
  byte                   midiOut;         // 0 = disable, 1 = enable
  byte                   midiThru;        // 0 = disable, 1 = enable
  byte                   midiRouting;     // 0 = disable, 1 = enable
  byte                   midiClock;       // 0 = disable, 1 = enable
};

struct sequence {
  byte                   midiMessage;     /*  1 = Program Change,
                                              2 = Control Code
                                              3 = Note On/Note Off
                                              4 = Bank Select+
                                              5 = Bank Select-
                                              6 = Program Change+
                                              7 = Program Change-
                                              8 = Pitch Bend
                                              9 = Channel Pressure
                                             10 = Start
                                             11 = Stop
                                             12 = Continue
                                             13 = Sequence */
  byte                   midiChannel;     /* MIDI channel 1-16 */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue1;      /* Single click */
  byte                   midiValue2;      /* Double click */
  byte                   midiValue3;      /* Long click */
};

struct message {
  byte                   midiMessage;     /*  1 = Program Change,
                                              2 = Control Code
                                              3 = Note On/Note Off
                                              4 = Bank Select+
                                              5 = Bank Select-
                                              6 = Program Change+
                                              7 = Program Change-
                                              8 = Pitch Bend
                                              9 = Channel Pressure
                                             10 = Start
                                             11 = Stop
                                             12 = Continue
                                             13 = Sequence */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue;       /* Control Code value, Note velocity */
  byte                   midiChannel;     /* MIDI channel 1-16 */
};

char      banknames[BANKS][MAXBANKNAME+1];        // Bank Names
action   *actions[BANKS];                         // Actions
bank      banks[BANKS][PEDALS];                   // The first action of every pedal
pedal     pedals[PEDALS];                         // Pedals Setup
sequence  sequences[SEQUENCES][STEPS];            // Sequences Setup
byte      currentMIDIValue[BANKS][PEDALS][LADDER_STEPS];
message   lastMIDIMessage[BANKS];
byte      lastProgramChange[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t  lastBankSelect[16]    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
CRGB      lastLedColor[BANKS][LEDS];

interface interfaces[] = {
                           "USB MIDI   ", 0, 1, 0, 0, 0,
                           "Legacy MIDI", 0, 1, 0, 0, 0,
                           "RTP-MIDI   ", 1, 1, 0, 0, 0,
                           "ipMIDI     ", 1, 1, 0, 0, 0,
                           "BLE MIDI   ", 1, 1, 0, 0, 0,
                           "OSC        ", 1, 1, 0, 0, 0
                          };                       // Interfaces Setup

AceButton       bootButton;
ButtonConfig    bootButtonConfig;
uint16_t        ladderLevels[LADDER_STEPS+1] = {497, 660, 752, 816, 876, 945, ADC_RESOLUTION - 1};  // TC-Helicon Switch 6

bool  tapDanceMode            = false;
bool  repeatOnBankSwitch      = false;
bool  tapDanceBank            = true;

byte  bootMode                = PED_BOOT_NORMAL;
volatile byte currentProfile  = 0;
volatile bool reloadProfile   = true;
volatile bool saveProfile     = false;
volatile bool loadConfig      = false;
volatile bool scrollingMode   = false;  // Display scrolling mode
byte  currentBank             = 0;
byte  currentPedal            = 0;
byte  currentInterface        = PED_USBMIDI;
byte  lastUsedSwitch          = 0xFF;
byte  lastUsedPedal           = 0xFF;
byte  lastUsed                = 0xFF;   // Pedal or switch
char  lastPedalName[MAXACTIONNAME+1] = "";
bool  selectBank              = true;
byte  currentMidiTimeCode     = PED_MTC_NONE;
byte  timeSignature           = PED_TIMESIGNATURE_4_4;
long  pressTime               = PED_PRESS_TIME;
long  doublePressTime         = PED_DOUBLE_PRESS_TIME;
long  longPressTime           = PED_LONG_PRESS_TIME;
long  repeatPressTime         = PED_REPEAT_PRESS_TIME;
byte  encoderSensitivity      = 5;    // 1..10
byte  ledsOnBrightness        = 50;   // 0..255
byte  ledsOffBrightness       = 10;   // 0..255
byte  tapLed                  = 0;
CRGB  tapColor0               = 0;
CRGB  tapColor1               = 0;


MidiTimeCode  MTC;
unsigned int  bpm             = 120;

#ifdef WIFI
bool  wifiEnabled             = true;
#else
bool  wifiEnabled             = false;
#endif
#ifdef BLE
bool  bleEnabled              = true;
#else
bool  bleEnabled              = false;
#endif
bool  wifiConnected           = false;
bool  bleConnected            = false;

String wifiSSID("");
String wifiPassword("");

bool powersaver = false;
bool firmwareUpdate = false;

#ifdef DEBUG_ESP_PORT
#include <esp_log.h>
#define SERIALDEBUG       Serial
#define LOG_TAG           "PedalinoESP"
//#define DPRINT(...)       ESP_LOGI(LOG_TAG, __VA_ARGS__)
//#define DPRINTLN(...)     ESP_LOGI(LOG_TAG, __VA_ARGS__)
#define DPRINT(...)       SERIALDEBUG.printf(__VA_ARGS__)
#define DPRINTLN(...)     { SERIALDEBUG.printf( __VA_ARGS__ ); SERIALDEBUG.println(); }
#endif

#ifdef PEDALINO_TELNET_DEBUG
#include <RemoteDebug.h>          // Remote debug over telnet - not recommended for production, only for development
RemoteDebug Debug;
#define DPRINT(...)       rdebugI(__VA_ARGS__)
#define DPRINTLN(...)     rdebugIln(__VA_ARGS__)
#endif

#ifndef DPRINT
#define DPRINT(...)
#endif

#ifndef DPRINTLN
#define DPRINTLN(...)
#define DPRINTMIDI(...)
#endif

#define xstr(s) sstr(s)  // stringize the result of expansion of a macro argument
#define sstr(s) #s

String getChipId() {
  char chipId[9];
  snprintf(chipId, 9, "%08X", (uint32_t)ESP.getEfuseMac()); // Low 4 bytes of MAC address (6 bytes)
  return String(chipId);
}

String host(getChipId());
String ssidSoftAP("Pedalino-" + getChipId());
String passwordSoftAP(getChipId());


#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern String           theme;
extern bool             appleMidiConnected;
#ifdef WEBSOCKET
extern AsyncWebSocket   webSocket;
extern AsyncEventSource events;
#endif

void   controller_delete();
void   delete_actions();
void   sort_actions();
void   create_banks();
void   wifi_connect();
void   blynk_enable();
void   blynk_disable();
bool   blynk_enabled();
bool   blynk_cloud_connected();
String blynk_get_token();
String blynk_set_token(String);
void   blynk_connect();
void   blynk_disconnect();
void   blynk_refresh();

void   screen_update(bool);
void   screen_info(int, int, int, int, int = 0, int = MIDI_RESOLUTION - 1);
void   leds_refresh();
void   leds_update(byte, byte, byte, byte);
void   eeprom_update_current_profile(byte);
bool   auto_reconnect(String ssid = "", String password = "");
bool   smart_config();
bool   ap_connect(String ssid = "", String password = "");
#ifdef WIFI
#include <BlynkSimpleEsp32.h>
String translateEncryptionType(wifi_auth_mode_t);
#endif

void display_clear();
void display_progress_bar_title(String);
void display_progress_bar_title2(String, String);
void display_progress_bar_update(unsigned int, unsigned int);
void display_progress_bar_2_update(unsigned int, unsigned int);
void display_progress_bar_2_label(unsigned int, unsigned int);

void mtc_start();
void mtc_stop();
void mtc_continue();
void mtc_tap();

#endif // _PEDALINO_H