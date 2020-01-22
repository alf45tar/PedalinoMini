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
#define BANKS            10
#define PEDALS            6
#define SEQUENCES        16
#define STEPS            10   // number of steps for each sequence

#define MAXPEDALNAME     10

// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// GPIOs 34 to 39 are GPIs – input only pins.
// These pins don’t have internal pull-ups or pull-down resistors. 
const byte pinD[] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13};
const byte pinA[] = {GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32, GPIO_NUM_33};

#define PIN_D(x)          pinD[x]
#define PIN_A(x)          pinA[x]

#ifdef TTGO_T_EIGHT
#define FACTORY_DEFAULT_PIN   GPIO_NUM_38  // Right 37   Center 38   Left 39
#define RIGHT_PIN             GPIO_NUM_37
#define CENTER_PIN            GPIO_NUM_38
#define LEFT_PIN              GPIO_NUM_39  // Shared with A2
#define SERIAL_DATA_PIN       GPIO_NUM_2   // DS
#define CLOCK_PIN             GPIO_NUM_2   // SH_CP
#define LATCH_PIN             GPIO_NUM_2   // ST_CP
#define USB_MIDI_IN_PIN       GPIO_NUM_18  // Used by SD
#define USB_MIDI_OUT_PIN      GPIO_NUM_19  // Used by SD
#define DIN_MIDI_IN_PIN       GPIO_NUM_15
#define DIN_MIDI_OUT_PIN      GPIO_NUM_4
#define BATTERY_PIN           GPIO_NUM_15
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
#define BATTERY_PIN           GPIO_NUM_23
#endif

#include "ShiftOut.h"

#define NUMBER_OF_SHIFT_REGISTERS 1

ShiftOut<NUMBER_OF_SHIFT_REGISTERS> leds;

// Serial MIDI interfaces

#include <MIDI.h>                       // https://github.com/FortySevenEffects/arduino_midi_library

#define MIDI_BAUD_RATE                  31250
#define HIGH_SPEED_SERIAL_BAUD_RATE     1000000

struct Serial1MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = MIDI_BAUD_RATE;
  static const int8_t RxPin  = USB_MIDI_IN_PIN;
  static const int8_t TxPin  = USB_MIDI_OUT_PIN;
};

struct Serial2MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = MIDI_BAUD_RATE;
  static const int8_t RxPin  = DIN_MIDI_IN_PIN;
  static const int8_t TxPin  = DIN_MIDI_OUT_PIN;
};

#define SERIAL_MIDI_USB   Serial1
#define SERIAL_MIDI_DIN   Serial2

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, SERIAL_MIDI_USB, USB_MIDI, Serial1MIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, SERIAL_MIDI_DIN, DIN_MIDI, Serial2MIDISettings);


typedef uint8_t   byte;

#include <ResponsiveAnalogRead.h>       // https://github.com/dxinteractive/ResponsiveAnalogRead
#include <MD_UISwitch.h>                // https://github.com/MajicDesigns/MD_UISwitch
#include <MD_REncoder.h>                // https://github.com/MajicDesigns/MD_REncoder

#define DEBOUNCE_INTERVAL 20
#define BOUNCE_LOCK_OUT                 // This method is a lot more responsive, but does not cancel noise.
//#define BOUNCE_WITH_PROMPT_DETECTION  // Report accurate switch time normally with no delay. Use when accurate switch transition timing is important.
#include <Bounce2.h>                    // https://github.com/thomasfredericks/Bounce2


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
#define PED_FACTORY_DEFAULT     7

#define PED_EMPTY               0
#define PED_PROGRAM_CHANGE      1
#define PED_CONTROL_CHANGE      2
#define PED_NOTE_ON_OFF         3
#define PED_BANK_SELECT_INC     4
#define PED_BANK_SELECT_DEC     5
#define PED_PROGRAM_CHANGE_INC  6
#define PED_PROGRAM_CHANGE_DEC  7
#define PED_PITCH_BEND          8
#define PED_CHANNEL_PRESSURE    9
#define PED_MIDI_START          10
#define PED_MIDI_STOP           11
#define PED_MIDI_CONTINUE       12
#define PED_SEQUENCE            20

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
#define PED_PRESS_1_2           3
#define PED_PRESS_1_L           5
#define PED_PRESS_2_L           6
#define PED_PRESS_1_2_L         7

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
#define ADC_RESOLUTION          128       
#define ADC_RESOLUTION_BITS       7       // hardware 9 to 12-bit ADC converter resolution
                                          // software 1 to 16-bit resolution
#define CALIBRATION_DURATION   8000       // milliseconds

struct bank {
  char                   pedalName[MAXPEDALNAME+1];
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

struct pedal {
  byte                   function;        /*  1 = MIDI
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
  int                    pedalValue[2];     // [0, MIDI_RESOLUTION-1]
  unsigned long          lastUpdate[2];     // last time the value is changed
  Bounce                *debouncer[2];
  MD_UISwitch           *footSwitch[2];
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

MD_UISwitch_Digital bootButton(CENTER_PIN);

#ifdef __BOARD_HAS_PSRAM__
bank**      banks;
pedal*      pedals;
sequence**  sequences;
message*    lastMIDIMessage;
#else
bank      banks[BANKS][PEDALS];                   // Banks Setup
pedal     pedals[PEDALS];                         // Pedals Setup
sequence  sequences[SEQUENCES][STEPS];            // Sequences Setup
message   lastMIDIMessage[BANKS]; 
#endif

interface interfaces[] = {
                           "USB MIDI   ", 1, 1, 0, 1, 0,
                           "Legacy MIDI", 1, 1, 0, 1, 0,
                           "RTP-MIDI   ", 1, 1, 0, 1, 0,
                           "ipMIDI     ", 1, 1, 0, 1, 0,
                           "BLE MIDI   ", 1, 1, 0, 1, 0,
                           "OSC        ", 1, 1, 0, 1, 0
                          };                       // Interfaces Setup

bool  tapDanceMode            = false;
bool  repeatOnBankSwitch      = false;
bool  tapDanceBank            = true;

byte  bootMode                = PED_BOOT_NORMAL;
volatile byte currentProfile  = 0;
volatile bool reloadProfile   = true;
volatile bool saveProfile     = false;
volatile bool scrollingMode   = false;  // Display scrolling mode
byte  currentBank             = 0;
byte  currentPedal            = 0;
byte  currentInterface        = PED_USBMIDI;
byte  lastUsedSwitch          = 0xFF;
byte  lastUsedPedal           = 0xFF;
byte  lastUsed                = 0xFF;   // Pedal or switch
char  lastPedalName[MAXPEDALNAME+1] = "";
bool  selectBank              = true;
byte  currentMidiTimeCode     = PED_MTC_NONE;
byte  timeSignature           = PED_TIMESIGNATURE_4_4;
long  pressTime               = PED_PRESS_TIME;
long  doublePressTime         = PED_DOUBLE_PRESS_TIME;
long  longPressTime           = PED_LONG_PRESS_TIME;
long  repeatPressTime         = PED_REPEAT_PRESS_TIME;


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


// The keys value that works for most LCD Keypad Shield

MD_UISwitch_Analog::uiAnalogKeys_t kt[] =
{
  {  30, 30, 'R' },  // Right
  { 130, 50, 'U' },  // Up
  { 305, 50, 'D' },  // Down
  { 475, 50, 'L' },  // Left
  { 720, 90, 'S' },  // Select
};

#define PED_LADDER_1  'R'
#define PED_LADDER_2  'U'
#define PED_LADDER_3  'D'
#define PED_LADDER_4  'L'
#define PED_LADDER_5  'S'

bool powersaver = false;

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
void   screen_info(int, int, int, int);
void   eeprom_update_current_profile(byte);
bool   auto_reconnect(String ssid = "", String password = "");
bool   smart_config();
bool   ap_connect(String ssid = "", String password = "");
#ifdef WIFI
#include <BlynkSimpleEsp32.h>
String translateEncryptionType(wifi_auth_mode_t);
#endif

#endif // _PEDALINO_H