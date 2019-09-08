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

#ifdef TTGO_T_EIGHT
#define FACTORY_DEFAULT_PIN   GPIO_NUM_38   // Right 37   Center 38   Left 39
#define RIGHT_PIN             GPIO_NUM_37
#define CENTER_PIN            GPIO_NUM_38
#define LEFT_PIN              GPIO_NUM_39
#else
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0
#define RIGHT_PIN             GPIO_NUM_37
#define CENTER_PIN            GPIO_NUM_0
#define LEFT_PIN              GPIO_NUM_38
#endif
#define BATTERY_PIN           GPIO_NUM_34

#define PIN_D(x)          pinD[x]
#define PIN_A(x)          pinA[x]

typedef uint8_t   byte;

#include <MIDI.h>                       // https://github.com/FortySevenEffects/arduino_midi_library
#include <ResponsiveAnalogRead.h>       // https://github.com/dxinteractive/ResponsiveAnalogRead
#include <MD_UISwitch.h>                // https://github.com/MajicDesigns/MD_UISwitch

#define DEBOUNCE_INTERVAL 20
#define BOUNCE_LOCK_OUT                 // This method is a lot more responsive, but does not cancel noise.
//#define BOUNCE_WITH_PROMPT_DETECTION  // Report accurate switch time normally with no delay. Use when accurate switch transition timing is important.
#include <Bounce2.h>                    // https://github.com/thomasfredericks/Bounce2

#include "MidiTimeCode.h"

#define PED_BOOT_NORMAL         0
#define PED_BOOT_BLE            1
#define PED_BOOT_WIFI           2
#define PED_BOOT_AP             3
#define PED_BOOT_AP_NO_BLE      4
#define PED_FACTORY_DEFAULT     5

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

#define PED_NONE                0
#define PED_MOMENTARY1          1
#define PED_LATCH1              2
#define PED_ANALOG              3
#define PED_JOG_WHEEL           4
#define PED_MOMENTARY2          5
#define PED_MOMENTARY3          6
#define PED_LATCH2              7
#define PED_LADDER              8

#define PED_PRESS_1             1
#define PED_PRESS_2             2
#define PED_PRESS_L             4
#define PED_PRESS_1_2           3
#define PED_PRESS_1_L           5
#define PED_PRESS_2_L           6
#define PED_PRESS_1_2_L         7

#define PED_MIDI                0
#define PED_BANK_PLUS           1
#define PED_BANK_MINUS          2
#define PED_START               3
#define PED_STOP                4
#define PED_CONTINUE            5
#define PED_TAP                 6
#define PED_MENU                7
#define PED_CONFIRM             8
#define PED_ESCAPE              9
#define PED_NEXT               10
#define PED_PREVIOUS           11
#define PED_BPM_PLUS           12
#define PED_BPM_MINUS          13

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

#define PED_LEGACY_MIDI_OUT     0
#define PED_LEGACY_MIDI_IN      1
#define PED_LEGACY_MIDI_THRU    2

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
#define ADC_RESOLUTION         1024       // hardware 9 to 12-bit ADC converter resolution
#define ADC_RESOLUTION_BITS      10       // software 1 to 16-bit resolution
#define CALIBRATION_DURATION   8000       // milliseconds

struct bank {
  char                   pedalName[MAXPEDALNAME+1];
  byte                   midiMessage;     /* 1 = Program Change,
                                             2 = Control Code
                                             3 = Note On/Note Off
                                             4 = Pitch Bend 
                                             5 = Bank Select+
                                             6 = Bank Select-
                                             7 = Program Change+
                                             8 = Program Change-
                                             9 = Sequence */
  byte                   midiChannel;     /* MIDI channel 1-16 */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue1;      /* Single click */
  byte                   midiValue2;      /* Double click */
  byte                   midiValue3;      /* Long click */
};

struct pedal {
  byte                   function;        /* 0 = MIDI
                                             1 = bank+
                                             2 = bank-
                                             3 = menu
                                             4 = confirm
                                             5 = escape
                                             6 = next
                                             7 = previous */
  byte                   autoSensing;     /* 0 = disable
                                             1 = enable   */
  byte                   mode;            /* 0 = none
                                             1 = momentary
                                             2 = latch
                                             3 = analog
                                             4 = jog wheel
                                             5 = momentary 2
                                             6 = momentary 3
                                             7 = latch 2
                                             8 = ladder */
  byte                   pressMode;       /* 1 = single click
                                             2 = double click
                                             4 = long click
                                             3 = single and double click
                                             5 = single and long click
                                             6 = double and long click
                                             7 = single, double and long click */
  byte                   invertPolarity;
  byte                   mapFunction;
  int                    expZero;
  int                    expMax;
  int                    pedalValue[2];
  unsigned long          lastUpdate[2];         // last time the value is changed
  Bounce                *debouncer[2];
  MD_UISwitch           *footSwitch[2];
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
  byte                   midiMessage;     /* 0 = None
                                             1 = Program Change,
                                             2 = Control Code
                                             3 = Note On/Note Off
                                             4 = Pitch Bend 
                                             5 = Bank Select+
                                             6 = Bank Select-
                                             7 = Program Change+
                                             8 = Program Change-
                                             9 = Sequence */
  byte                   midiChannel;     /* MIDI channel 1-16 */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue1;      /* Single click */
  byte                   midiValue2;      /* Double click */
  byte                   midiValue3;      /* Long click */
};

struct message {
  byte                   midiMessage;     /* 0 = None
                                             1 = Program Change,
                                             2 = Control Code
                                             3 = Note On/Note Off
                                             4 = Pitch Bend 
                                             5 = Bank Select+
                                             6 = Bank Select-
                                             7 = Program Change+
                                             8 = Program Change-
                                             9 = Sequence */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue;       /* Control Code value, Note velocity */
  byte                   midiChannel;     /* MIDI channel 1-16 */
};

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
bool  selectBank              = true;
byte  currentMidiTimeCode     = PED_MTC_NONE;
byte  timeSignature           = PED_TIMESIGNATURE_4_4;
long  pressTime               = 100;
long  doublePressTime         = 300;
long  longPressTime           = 500;
long  repeatPressTime         = 500;

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

// Serial MIDI interface to comunicate with Arduino

#define MIDI_BAUD_RATE                  31250
#define HIGH_SPEED_SERIAL_BAUD_RATE     1000000

struct Serial1MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = MIDI_BAUD_RATE;
  static const int8_t RxPin  = 18;
  static const int8_t TxPin  = 19;
};

struct Serial2MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = MIDI_BAUD_RATE;
  static const int8_t RxPin  = 15;
  static const int8_t TxPin  = 4;
};

#define SERIAL_MIDI_USB   Serial1
#define SERIAL_MIDI_DIN   Serial2

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, SERIAL_MIDI_USB, USB_MIDI, Serial1MIDISettings);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, SERIAL_MIDI_DIN, DIN_MIDI, Serial2MIDISettings);

// The keys value that works for most LCD Keypad Shield

MD_UISwitch_Analog::uiAnalogKeys_t kt[] =
{
  {  10, 10, 'R' },  // Right
  { 130, 15, 'U' },  // Up
  { 305, 15, 'D' },  // Down
  { 475, 15, 'L' },  // Left
  { 720, 15, 'S' },  // Select
};

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
  String id((uint32_t)ESP.getEfuseMac(), HEX); // Low 4 bytes of MAC address (6 bytes)
  id.toUpperCase();
  return id;
}

String host(getChipId());
String wifiSoftAP("Pedalino-" + getChipId());


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
void   eeprom_update_current_profile(byte);
bool   auto_reconnect(String ssid = "", String password = "");
bool   smart_config();
bool   ap_connect(String ssid = "", String password = "");
#ifdef WIFI
String translateEncryptionType(wifi_auth_mode_t);
#endif

#endif // _PEDALINO_H