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

#ifndef _PEDALINO_H
#define _PEDALINO_H

//#define LOG_LOCAL_LEVEL   ESP_LOG_VERBOSE

#include <vector>
#include <Arduino.h>

#define MODEL           "PedalinoMini™"

#define INTERFACES        6
#define PROFILES          3
#define BANKS            21   // 20 banks + 1 bank for global actions
#define PEDALS            6   // real number of pedals is board specific (see below)
#define CONTROLS        100
#define SEQUENCES        16
#define STEPS            10   // number of steps for each sequence
#define LADDER_STEPS      6   // max number of controls in a resistor ladder
#define LEDS             10   // number of WS2812B leds (254 max)
#define LED_RGB_ORDER   RGB   // do not change it, RGB order is managed by program because FastLED library does not support changing RGB order at runtime
#define SLOTS_ROWS        2
#define SLOTS_COLS        3
#define SLOTS             SLOTS_ROWS * SLOTS_COLS


#define MAXACTIONNAME    16
#define MAXBANKNAME      16

// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// GPIOs 34 to 39 are GPIs – input only pins.
// These pins don’t have internal pull-ups or pull-down resistors.
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html

#ifdef HELTEC_WIFI_KIT_32
#undef  PEDALS
#define PEDALS                6
const byte pinD[] = {GPIO_NUM_0, GPIO_NUM_2, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12};
const byte pinA[] = {GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_38, GPIO_NUM_39, GPIO_NUM_34, GPIO_NUM_35};
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0    // PRG button
#define USB_MIDI_IN_PIN       GPIO_NUM_18
#define USB_MIDI_OUT_PIN      GPIO_NUM_19
#define DIN_MIDI_IN_PIN       GPIO_NUM_23
#define DIN_MIDI_OUT_PIN      GPIO_NUM_22
#define BATTERY_PIN           GPIO_NUM_13   // Pin connected to VBAT
#define BATTERY_ADC_EN        GPIO_NUM_21   // ADC_EN is the ADC detection enable port
#define FASTLEDS_DATA_PIN     GPIO_NUM_5
#elif defined ARDUINO_LILYGO_T_DISPLAY
#undef  PEDALS
#define PEDALS                8
const byte pinD[] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_17, GPIO_NUM_35, GPIO_NUM_0};
const byte pinA[] = {GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_38, GPIO_NUM_39, GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_35, GPIO_NUM_0};
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0    // Button 2
#define USB_MIDI_IN_PIN       GPIO_NUM_21   // SDA
#define USB_MIDI_OUT_PIN      GPIO_NUM_22   // SCL
#define DIN_MIDI_IN_PIN       GPIO_NUM_15
#define DIN_MIDI_OUT_PIN      GPIO_NUM_2
#define BATTERY_PIN           GPIO_NUM_34   // Pin connected to BAT (BAT is not VBAT)
#define BATTERY_ADC_EN        GPIO_NUM_14   // ADC_EN is the ADC detection enable port
#define FASTLEDS_DATA_PIN     GPIO_NUM_15
#elif defined TTGO_T_EIGHT
#undef  PEDALS
#define PEDALS                9
const byte pinD[] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_37, GPIO_NUM_38, GPIO_NUM_39};
const byte pinA[] = {GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_37, GPIO_NUM_38, GPIO_NUM_39};
#define FACTORY_DEFAULT_PIN   GPIO_NUM_38   // Right 37   Center 38   Left 39
#define LATCH_PIN             GPIO_NUM_2
#define USB_MIDI_IN_PIN       GPIO_NUM_18   // Used by SD
#define USB_MIDI_OUT_PIN      GPIO_NUM_19   // Used by SD
#define DIN_MIDI_IN_PIN       GPIO_NUM_15
#define DIN_MIDI_OUT_PIN      GPIO_NUM_4
#define BATTERY_PIN           GPIO_NUM_36   // GPIO_NUM_32 to GPIO_NUM_39 only
#define FASTLEDS_DATA_PIN     GPIO_NUM_5
#elif defined ARDUINO_BPI_LEAF_S3           // https://wiki.banana-pi.org/BPI-Leaf-S3
#undef  PEDALS
#define PEDALS                9
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/gpio.html
// Strapping pins: 1, 3, 45, 46
// ADC1:    1-10
// ADC2:   11-20 (19=D- 20=D+)
// SPI0/1: 26-32 (usually used for SPI flash and PSRAM)
const byte pinD[] = {GPIO_NUM_10, GPIO_NUM_9,  GPIO_NUM_8,  GPIO_NUM_7,  GPIO_NUM_6,  GPIO_NUM_5,  GPIO_NUM_4,  GPIO_NUM_2,  GPIO_NUM_0};
const byte pinA[] = {GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_0};
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0    // Button BOOT
#define DIN_MIDI_IN_PIN       GPIO_NUM_44
#define DIN_MIDI_OUT_PIN      GPIO_NUM_43
#define BATTERY_PIN           GPIO_NUM_14   // Pin connected to +BATT
#define FASTLEDS_DATA_PIN     GPIO_NUM_48
#undef  SDA
#undef  SCL
#define SDA                   GPIO_NUM_15
#define SCL                   GPIO_NUM_16
#elif defined ARDUINO_LILYGO_T_DISPLAY_S3   // https://github.com/Xinyuan-LilyGO/T-Display-S3
#undef  PEDALS
#define PEDALS                8
const byte pinD[] = {GPIO_NUM_10, GPIO_NUM_2,  GPIO_NUM_43, GPIO_NUM_44, GPIO_NUM_21, GPIO_NUM_3,  GPIO_NUM_14, GPIO_NUM_0};
const byte pinA[] = {GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_14, GPIO_NUM_0};
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0    // Button BOOT
#define DIN_MIDI_IN_PIN       GPIO_NUM_44
#define DIN_MIDI_OUT_PIN      GPIO_NUM_43
#define BATTERY_PIN           GPIO_NUM_4    // Pin connected to BAT (BAT is not VBAT)
#define FASTLEDS_DATA_PIN     GPIO_NUM_3
#else
#undef  PEDALS
#define PEDALS                7
const byte pinD[] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_0};
const byte pinA[] = {GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_0};
#define FACTORY_DEFAULT_PIN   GPIO_NUM_0
#define USB_MIDI_IN_PIN       GPIO_NUM_18
#define USB_MIDI_OUT_PIN      GPIO_NUM_19
#define DIN_MIDI_IN_PIN       GPIO_NUM_15
#define DIN_MIDI_OUT_PIN      GPIO_NUM_4
#define BATTERY_PIN           GPIO_NUM_36   // GPIO_NUM_32 to GPIO_NUM_39 only
#define FASTLEDS_DATA_PIN     GPIO_NUM_5
#endif

#define PIN_D(x)          pinD[x]
#define PIN_A(x)          pinA[x]

#include <FastLED.h>                    // https://github.com/FastLED/FastLED
CRGB    fastleds[LEDS+1];               // fastleds[LEDS] not used
EOrder  rgbOrder = RGB;

typedef uint8_t   byte;

#include <HCSR04.h>                     // https://github.com/d03n3rfr1tz3/HC-SR04
#include <ResponsiveAnalogRead.h>       // https://github.com/dxinteractive/ResponsiveAnalogRead
#include <MD_REncoder.h>                // https://github.com/MajicDesigns/MD_REncoder
#include <AceButton.h>                  // https://github.com/bxparks/AceButton
#include <hellodrum.h>                  // https://github.com/RyoKosaka/HelloDrum-arduino-Library
//#include "AnalogPad.h"
using namespace ace_button;

#define DEBOUNCE_INTERVAL        5
#define PED_PRESS_TIME         200
#define PED_DOUBLE_PRESS_TIME  400
#define PED_LONG_PRESS_TIME    500
#define PED_REPEAT_PRESS_TIME 1000
#define PED_SIMULTANEOUS_GAP    50

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

#define PED_UPDATE_NONE         0
#define PED_UPDATE_ARDUINO_OTA  1
#define PED_UPDATE_HTTP         2
#define PED_UPDATE_CLOUD        3

#define PED_EMPTY                     midi::InvalidType
#define PED_PROGRAM_CHANGE            midi::ProgramChange
#define PED_CONTROL_CHANGE            midi::ControlChange
#define PED_NOTE_ON                   midi::NoteOn
#define PED_NOTE_OFF                  midi::NoteOff
#define PED_BANK_SELECT_INC           4
#define PED_BANK_SELECT_DEC           5
#define PED_PROGRAM_CHANGE_INC        6
#define PED_PROGRAM_CHANGE_DEC        7
#define PED_CONTROL_CHANGE_SNAP       8
#define PED_PITCH_BEND                midi::PitchBend
#define PED_CHANNEL_PRESSURE          midi::AfterTouchChannel
#define PED_MIDI_START                midi::Start
#define PED_MIDI_STOP                 midi::Stop
#define PED_MIDI_CONTINUE             midi::Continue
#define PED_SEQUENCE                  20
#define PED_ACTION_BANK               19
#define PED_ACTION_BANK_PLUS          21
#define PED_ACTION_BANK_MINUS         22
#define PED_ACTION_START              23
#define PED_ACTION_STOP               24
#define PED_ACTION_CONTINUE           25
#define PED_ACTION_TAP                26
#define PED_ACTION_BPM_PLUS           27
#define PED_ACTION_BPM_MINUS          29
#define PED_ACTION_PROFILE_PLUS       30
#define PED_ACTION_PROFILE_MINUS      31
#define PED_ACTION_DEVICE_INFO        32
#define PED_ACTION_LED_COLOR          33
#define PED_ACTION_REPEAT             34
#define PED_ACTION_REPEAT_OVERWRITE   35
#define PED_SEQUENCE_STEP_BY_STEP_FWD 36
#define PED_SEQUENCE_STEP_BY_STEP_REV 37
#define PED_ACTION_MIDI_CLOCK_MASTER  40
#define PED_ACTION_MIDI_CLOCK_SLAVE   41
#define PED_ACTION_MIDI_CLOCK_OFF     42
#define PED_ACTION_MTC_MASTER         43
#define PED_ACTION_MTC_SLAVE          44
#define PED_ACTION_MTC_OFF            45
#define PED_ACTION_MTC_TIME_SIGNATURE 46
#define PED_OSC_MESSAGE               50
#define PED_ACTION_SCAN               98
#define PED_ACTION_POWER_ON_OFF       99

#define PED_NONE                  1
#define PED_MOMENTARY1            2
#define PED_LATCH1                3
#define PED_ANALOG                4
#define PED_JOG_WHEEL             5
#define PED_MOMENTARY2            6
#define PED_MOMENTARY3            7
#define PED_LATCH2                8
#define PED_LADDER                9
#define PED_ULTRASONIC            10    // HC-SR04
#define PED_ANALOG_MOMENTARY      11
#define PED_ANALOG_LATCH          12
#define PED_ANALOG_PAD            13    // Piezo
#define PED_ANALOG_PAD_MOMENTARY  14    // Piezo + switch

const char *pedalModeName[] = {"", "None", "Momentary 1", "Latch", "Analog", "Jog Wheel", "Momentary 2", "Momentary 3", "Latch 2", "Ladder", "Ultrasonic", "Analog+Momentary", "Analog+Latch", "Analog Pad", "Analog Pad+Momentary"};

#define PED_PRESS_1             1
#define PED_PRESS_2             2
#define PED_PRESS_L             4
#define PED_PRESS_1_2           PED_PRESS_1 + PED_PRESS_2
#define PED_PRESS_1_L           PED_PRESS_1 + PED_PRESS_L
#define PED_PRESS_2_L           PED_PRESS_2 + PED_PRESS_L
#define PED_PRESS_1_2_L         PED_PRESS_1 + PED_PRESS_2 + PED_PRESS_L

#define IS_SINGLE_PRESS_ENABLED(x)         (((x)&1)==1)
#define IS_DOUBLE_PRESS_ENABLED(x)         (((x)&2)==2)
#define IS_LONG_PRESS_ENABLED(x)           (((x)&4)==4)

const char *pedalPressModeName[] = {"None", "1", "2", "12", "L", "1L","2L", "12L"};

//#define PED_MIDI                1
//#define PED_ACTIONS             PED_MIDI
#define PED_BANK_PLUS           2
#define PED_BANK_MINUS          3
#define PED_START               4
#define PED_STOP                5
#define PED_CONTINUE            6
#define PED_TAP                 7
#define PED_BPM_PLUS            8
#define PED_BPM_MINUS           9

#define PED_EVENT_PRESS           AceButton::kEventPressed
#define PED_EVENT_RELEASE         AceButton::kEventReleased
#define PED_EVENT_CLICK           AceButton::kEventClicked
#define PED_EVENT_DOUBLE_CLICK    AceButton::kEventDoubleClicked
#define PED_EVENT_LONG_PRESS      AceButton::kEventLongPressed
#define PED_EVENT_REPEAT          AceButton::kEventRepeatPressed
#define PED_EVENT_LONG_RELEASED   AceButton::kEventLongReleased
#define PED_EVENT_MOVE            7
#define PED_EVENT_JOG             8
#define PED_EVENT_NONE            9
#define PED_EVENT_OSC             10
#define PED_EVENT_PRESS_RELEASE   11
#define PED_EVENT_SHOT            12
#define PED_EVENT_LAST            13

const char *eventName[] = {"Press", "Release", "Click", "Double Click", "Long Press", "Repeat Pressed", "Long Released", "Move", "Jog", "None", "OSC", "Press & Release"};

#define PED_LINEAR              0
#define PED_LOG                 1
#define PED_ANTILOG             2

const char *pedalAnalogResponse[] = {"Linear", "Log", "Antilog"};

#define PED_USBMIDI             0
#define PED_DINMIDI             1
#define PED_RTPMIDI             2
#define PED_IPMIDI              3
#define PED_BLEMIDI             4
#define PED_OSC                 5

#define PED_DISABLE             0
#define PED_ENABLE              1
#define PED_SHOW                2

#define IS_INTERFACE_ENABLED(x)   (((x) & PED_ENABLE) == PED_ENABLE)
#define IS_SHOW_ENABLED(x)        (((x) & PED_SHOW  ) == PED_SHOW  )

#define PED_MTC_NONE            0
#define PED_MTC_SLAVE           1
#define PED_MTC_MASTER_24       2
#define PED_MTC_MASTER_25       3
#define PED_MTC_MASTER_30DF     4
#define PED_MTC_MASTER_30       5
#define PED_MIDI_CLOCK_SLAVE    6
#define PED_MIDI_CLOCK_MASTER   7

#define PED_TIMESIGNATURE_2_4   0
#define PED_TIMESIGNATURE_4_4   1
#define PED_TIMESIGNATURE_3_4   2
#define PED_TIMESIGNATURE_3_8   3
#define PED_TIMESIGNATURE_6_8   4
#define PED_TIMESIGNATURE_9_8   5
#define PED_TIMESIGNATURE_12_8  6
#define PED_TIMESIGNATURE_LAST  7

#define MIDI_RESOLUTION         128       // MIDI 7-bit CC resolution
#define ADC_RESOLUTION         1024
#define ADC_RESOLUTION_BITS      10       // hardware 9 to 12-bit ADC converter resolution
                                          // software 1 to 16-bit resolution

struct action {
  char                   tag0[MAXACTIONNAME+1];
  union {
    char                 tag1[MAXACTIONNAME+1];
    char                 name[MAXACTIONNAME+1];
  };
  byte                   control;
  byte                   led;             // 0..LEDS-1 existing leds, if equal to LEDS no led assigned to action, 255 = Use default led defined by control
  uint32_t               color0;
  uint32_t               color1;
  byte                   event;
  byte                   midiMessage;
  byte                   midiChannel;     /* MIDI channel 1-16 */
  byte                   midiCode;        /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue1;
  byte                   midiValue2;
  char                   oscAddress[51];
  byte                   slot;            // 0..SLOTS-1 real slots. If equal to SLOTS no slot selected
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
                                             9 = ladder
                                             10 = ultrasonic */
  byte                   pressMode;       /* 1 = single click
                                             2 = double click
                                             4 = long click
                                             3 = single and double click
                                             5 = single and long click
                                             6 = double and long click
                                             7 = single, double and long click */
  byte                   invertPolarity;  /* 0 = disable
                                             1 = enable   */
  byte                   latchEmulation;  /* 0 = disable
                                             1 = enable   */
  byte                   analogResponse;
  int                    expZero;           // [0, ADC_RESOLUTION-1]
  int                    expMax;            // [0, ADC_RESOLUTION-1]
  float                  snapMultiplier;    // a value from 0 to 1 that controls the amount of easing
                                            // increase this to lessen the amount of easing (such as 0.1)
                                            // and make the responsive values more responsive,
                                            // but doing so may cause more noise to seep through
                                            // when sleep is not enabled.
  float                  activityThreshold; // the amount of movement that must take place for it
                                            // to register as activity and start moving the output value
  float                  gain;
  float                  retrigger;
  int                    holdCycles;
  int                    scanCycles;
  int                    crosstalk;
  float                  crosstalkRatio;
  int                    pedalValue[2];     // [0, ADC_RESOLUTION-1]
  unsigned long          lastUpdate[2];     // last time the value is changed
  byte                   latchStatus[LADDER_STEPS]; // latch emulation status: 0 = off, 1 = on
  AceButton             *button[LADDER_STEPS];
  ButtonConfig          *buttonConfig;
  MD_REncoder           *jogwheel;
  ResponsiveAnalogRead  *analogPedal;
  //AnalogPad             *analogPad;
  HelloDrum             *analogPad;
};

struct control {
  byte                   pedal1;
  byte                   button1;
  byte                   pedal2;
  byte                   button2;
  byte                   led;              // 0..LEDS-1 existing leds, LEDS = disabled
};

struct interface {
  char                   name[13];
  byte                   midiIn;           // 0 = disable, 1 = enable
  byte                   midiOut;          // 0 = disable, 1 = enable
  byte                   midiThru;         // 0 = disable, 1 = enable
  byte                   midiClock;        // 0 = disable, 1 = enable
};

struct message {
  byte                   midiMessage;
  byte                   midiCode;         /* Program Change, Control Code, Note or Pitch Bend value to send */
  byte                   midiValue;        /* Control Code value, Note velocity */
  byte                   midiChannel;      /* MIDI channel 1-16, 0 = None, 17 = All */
  byte                   led;              // 0..LEDS-1 existing leds, if equal to LEDS ...
  uint32_t               color;
};

char      banknames[BANKS][MAXBANKNAME+1];        // Bank Names
action   *actions[BANKS];                         // Actions
bank      banks[BANKS][PEDALS];                   // The first action of every pedal
pedal     pedals[PEDALS];                         // Pedals Setup
control   controls[CONTROLS];                     // Controls Setup
message   sequences[SEQUENCES][STEPS];            // Sequences Setup
byte      currentMIDIValue[BANKS][PEDALS][LADDER_STEPS];
message   lastMIDIMessage[BANKS];
CRGB      lastColor0;
CRGB      lastColor1;
byte      lastProgramChange[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t  lastBankSelect[16]    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
CRGB      lastLedColor[BANKS][LEDS];
std::vector<byte> ultrasonicEcho(PEDALS);         // Echo pins
byte      ultrasonicTrigger;                      // Trigger pin

interface interfaces[] = { "USB MIDI   ", 0,          PED_ENABLE + PED_SHOW, 0, 0,
                           "Legacy MIDI", 0,          PED_ENABLE + PED_SHOW, 0, 0,
                           "RTP-MIDI   ", PED_ENABLE, PED_ENABLE + PED_SHOW, 0, 0,
                           "ipMIDI     ", PED_ENABLE, PED_ENABLE + PED_SHOW, 0, 0,
                           "BLE MIDI   ", PED_ENABLE, PED_ENABLE + PED_SHOW, 0, 0,
                           "OSC        ", PED_ENABLE, PED_ENABLE + PED_SHOW, 0, 0
                         };                       // Interfaces Setup

AceButton       bootButton;
ButtonConfig    bootButtonConfig;
uint16_t        ladderLevels[LADDER_STEPS+1] = {497, 660, 752, 816, 876, 945, ADC_RESOLUTION - 1};  // TC-Helicon Switch 6

char  slots[SLOTS_ROWS][SLOTS_COLS][MAXBANKNAME+1];

bool  tapDanceMode            = false;
bool  repeatOnBankSwitch      = false;
bool  tapDanceBank            = true;

byte  bootMode                = PED_BOOT_NORMAL;
bool  flipScreen              = false;
bool  displayOff              = false;
unsigned long displayOffCountdownStart = 0;
unsigned long screenSaverTimeout = 1000*60*60;   // 1 hour

volatile byte currentProfile  = 0;
volatile bool reloadProfile   = true;
volatile bool saveProfile     = false;
volatile bool loadConfig      = false;
volatile bool scrollingMode   = false;  // Display scrolling mode
volatile bool displayInit     = false;
volatile bool webServerStart  = false;

byte  currentBank             = 1;
byte  currentPedal            = 0;
byte  currentInterface        = PED_USBMIDI;
byte  lastUsedSwitch          = 0xFF;
byte  lastUsedPedal           = 0xFF;
byte  lastUsed                = 0xFF;   // Pedal or switch
byte  lastSlot                = SLOTS;
char  lastPedalName[MAXACTIONNAME+1] = "";
bool  selectBank              = true;
byte  currentMidiTimeCode     = PED_MTC_NONE;
byte  timeSignature           = PED_TIMESIGNATURE_4_4;
long  debounceInterval        = DEBOUNCE_INTERVAL;
long  simultaneousGapTime     = PED_SIMULTANEOUS_GAP;
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

uint32_t freeMemory;
uint32_t maxAllocation;

String wifiSSID     = "";
String wifiPassword = "";
int    wifiLevel    = 0;

uint16_t  batteryVoltage = 4200;  // mV

#ifdef DIAGNOSTIC
#ifdef ARDUINO_LILYGO_T_DISPLAY_S3
#define POINTS                        320             // Logged data points
#else
#define POINTS                        240             // Logged data points
#endif // ARDUINO_LILYGO_T_DISPLAY_S3
#define SECONDS_BETWEEN_SAMPLES       1
#define GRAPH_DURATION                POINTS * SECONDS_BETWEEN_SAMPLES
#define GRAPH_DURATION_QUARTER_SEC    GRAPH_DURATION / 4
#define GRAPH_DURATION_QUARTER_MIN    GRAPH_DURATION_QUARTER_SEC / 60
#define GRAPH_DURATION_QUARTER_HOUR   GRAPH_DURATION_QUARTER_MIN / 60
RTC_DATA_ATTR uint16_t historyStart = 0;
RTC_DATA_ATTR byte memoryHistory[POINTS];         // 0% =   0Kb    100% = 200Kb
RTC_DATA_ATTR byte fragmentationHistory[POINTS];  // 0% =   0Kb    100% = 200Kb
RTC_DATA_ATTR byte wifiHistory[POINTS];           // 0% = -90dB    100% = -10dB
RTC_DATA_ATTR byte batteryHistory[POINTS];        // 0% =  3.0V    100% =  5.0V
#endif // DIAGNOSTIC

uint16_t  scan[POINTS];
uint16_t  scanProcessed[POINTS];
uint16_t  scanIndex = 0;
bool      scannerActivated = false;
byte      scanPedal = 0;

bool powersaver = false;
byte firmwareUpdate = PED_UPDATE_NONE;

uint32_t vref = 1100;

#ifdef DEBUG_ESP_PORT
#include <esp_log.h>
#define SERIALDEBUG       Serial
#define LOG_TAG           "PedalinoESP"
//#define DPRINT(...)       ESP_LOGI(LOG_TAG, __VA_ARGS__)
//#define DPRINTLN(...)     ESP_LOGI(LOG_TAG, __VA_ARGS__)
#define DPRINT(...)       SERIALDEBUG.printf(__VA_ARGS__)
#define DPRINTLN(...)     { SERIALDEBUG.printf( __VA_ARGS__ ); SERIALDEBUG.println(); }
#endif

#ifndef DPRINT
#define DPRINT(...)
#endif

#ifndef DPRINTLN
#define DPRINTLN(...)
#define DPRINTMIDI(...)
#endif

long map2(long x, long in_min, long in_max, long out_min, long out_max) {
    const long dividend = out_max - out_min;
    const long divisor = in_max - in_min;
    const long delta = x - in_min;

    if (x == in_min) return out_min;
    if (x == in_max) return out_max;

    return (divisor == 0 ? (x <= in_min ? out_min : out_max) : (delta * dividend + (divisor / 2)) / divisor + out_min);
}

String getChipId() {
  char chipId[9];
  snprintf(chipId, 9, "%08X", (uint32_t)ESP.getEfuseMac()); // Low 4 bytes of MAC address (6 bytes)
  return String(chipId);
}

String host           = getChipId();
String ssidSoftAP     = String("Pedalino-") + getChipId();
String passwordSoftAP = getChipId();

#include <AsyncTCP.h>
#define WEBSERVER_H           // to not redefine WebRequestMethod (HTTP_GET, HTTP_POST, ...)
#define HTTP_ANY  0b01111111  // not defined otherwise
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
void   http_setup();

int m1, m2, m3, m4, rmin, rmax;
unsigned long endMillis2 = 0;

void screen_info(int b1, int b2, int b3, int b4, int mi = 0, int ma = MIDI_RESOLUTION - 1)
{
  m1 = b1;
  m2 = b2;
  m3 = b3;
  m4 = b4;
  rmin = min(mi, ma);
  rmax = max(mi, ma);
  endMillis2 = millis() + 1500;
}

void   leds_off();
void   leds_refresh();
void   leds_update(byte, byte, byte, byte);
void   eeprom_update_current_profile(byte);
bool   auto_reconnect(const String& ssid = "", const String& password = "");
bool   smart_config();
bool   ap_connect(const String& ssid = "", const String& password = "");
#ifdef WIFI
String translateEncryptionType(wifi_auth_mode_t);
#endif

void display_clear();
void display_progress_bar_title(const String&);
void display_progress_bar_title2(const String&, const String&);
void display_progress_bar_update(unsigned int, unsigned int);
void display_progress_bar_2_update(unsigned int, unsigned int);
void display_progress_bar_2_label(unsigned int, unsigned int);
void display_off();

void switch_profile_or_bank(byte, byte, byte);

void mtc_start();
void mtc_stop();
void mtc_continue();
void mtc_tap();

#endif // _PEDALINO_H