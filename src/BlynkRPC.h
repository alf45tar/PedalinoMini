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

#ifdef NOBLYNK
inline String blynk_get_token() { return String("                                "); }
inline String blynk_set_token(String token) { return token; }
inline void blynk_setup() {}
inline void blynk_enable() {}
inline void blynk_disable() {}
inline bool blynk_enabled() { return false; }
inline bool blynk_cloud_connected() { return false; }
inline void blynk_config() {}
inline void blynk_connect() {}
inline void blynk_disconnect() {}
inline void blynk_run() {}
inline void blynk_refresh() {}
#else

#define BLYNK_RETRY_CONNECTION    60      // If fail retry Blynk Cloud connection after 60 seconds
#define BLYNK_NO_BUILTIN                  // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT                    // Disable float operations

#ifdef SERIALDEBUG
#define BLYNK_PRINT SERIALDEBUG           // Defines the object that is used for printing
//#define BLYNK_DEBUG                     // Optional, this enables more detailed prints
#endif

#ifdef WIFI
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#endif

#define BLYNK_PROFILE               V1
#define BLYNK_CLOCK_START           V11
#define BLYNK_CLOCK_STOP            V12
#define BLYNK_CLOCK_CONTINUE        V13
#define BLYNK_MIDI_TIME_CODE        V14
#define BLYNK_CLOCK_MASTER_SLAVE    V15
#define BLYNK_BPM                   V16
#define BLYNK_TAP_TEMPO             V17

#define BLYNK_BANK                  V20
#define BLYNK_MIDIMESSAGE           V21
#define BLYNK_MIDICHANNEL           V22
#define BLYNK_MIDICODE              V23
#define BLYNK_MIDIVALUE1            V24
#define BLYNK_MIDIVALUE2            V25
#define BLYNK_MIDIVALUE3            V26

#define BLYNK_PEDAL                 V30
#define BLYNK_PEDAL_MODE1           V31
#define BLYNK_PEDAL_MODE2           V32
#define BLYNK_PEDAL_FUNCTION        V33
#define BLYNK_PEDAL_AUTOSENSING     V34
#define BLYNK_PEDAL_SINGLEPRESS     V35
#define BLYNK_PEDAL_DOUBLEPRESS     V36
#define BLYNK_PEDAL_LONGPRESS       V37
#define BLYNK_PEDAL_POLARITY        V38
#define BLYNK_PEDAL_CALIBRATE       V39
#define BLYNK_PEDAL_ANALOGZERO      V51
#define BLYNK_PEDAL_ANALOGMAX       V52

#define BLYNK_INTERFACE             V40
#define BLYNK_INTERFACE_MIDIIN      V41
#define BLYNK_INTERFACE_MIDIOUT     V42
#define BLYNK_INTERFACE_MIDITHRU    V43
#define BLYNK_INTERFACE_MIDIROUTING V44
#define BLYNK_INTERFACE_MIDICLOCK   V45

#define BLYNK_SCANWIFI              V91
#define BLYNK_SSID                  V92
#define BLYNK_PASSWORD              V93
#define BLYNK_WIFICONNECT           V94
#define BLYNK_SMARTCONFIG   		    V95

#define PRINT_VIRTUAL_PIN(vPin)     { DPRINT("WRITE VirtualPIN %d", vPin); }

#define BLYNK_AUTHTOKEN_LEN          32

bool blynkEnabled = false;
char blynkAuthToken[BLYNK_AUTHTOKEN_LEN+1] = { 0 }; // all elements 0;

WidgetLCD  blynkLCD(V0);
String     ssid("");
String     password("");


void blynk_setup()
{
  // Setup a 1Hz timer
  Timer3Attach(1000);
}

void blynk_enable()
{
  blynkEnabled = true;
}

void blynk_disable()
{
  blynkEnabled = false;
}

bool blynk_enabled()
{
  return blynkEnabled;
}

String blynk_get_token()
{
  return String(blynkAuthToken);
}

String blynk_set_token(String token)
{
  if (token.length() == BLYNK_AUTHTOKEN_LEN || token == "")
    strncpy(blynkAuthToken, token.c_str(), BLYNK_AUTHTOKEN_LEN);

  return String(blynkAuthToken);
}

bool blynk_cloud_connected()
{
  return Blynk.connected();
}

void blynk_connect()
{
  static unsigned long lastFail = 0;

  if (!blynkEnabled || ((lastFail > 0) && (millis() - lastFail < BLYNK_RETRY_CONNECTION*1000))) return;

#ifdef WIFI
  // Connect to Blynk Cloud
  if (WiFi.getMode() != WIFI_AP && strlen(blynkAuthToken) == BLYNK_AUTHTOKEN_LEN) {
    Blynk.config(blynkAuthToken);
    lastFail = Blynk.connect() ? 0 : millis();
  }
#endif
#ifdef BLE
  // Connect to Blynk Cloud
  /*
  blynkEnabled = true;
  Blynk.begin("522b382dbdfc411891aac707d106b293");
  lastFail = 0;
  */
#endif
}

void blynk_disconnect()
{
  // Disconnect to Blynk Cloud
  if (Blynk.connected())
    Blynk.disconnect();
}

inline void blynk_run()
{
  if (interruptCounter3 > 0) {

    interruptCounter3 = 0;

    if (!blynkEnabled) return;

#ifdef WIFI
    if (Blynk.connected()) {
      Blynk.run();
    }
    else if (WiFi.isConnected())
      blynk_connect();
#endif
  }
}

void blynk_refresh_bank()
{
  if (Blynk.connected())
  {
    Blynk.virtualWrite(BLYNK_PROFILE,               currentProfile + 1);
    Blynk.virtualWrite(BLYNK_BANK,                  currentBank + 1);
    Blynk.virtualWrite(BLYNK_PEDAL,                 currentPedal + 1);
    Blynk.virtualWrite(BLYNK_MIDIMESSAGE,           banks[currentBank][currentPedal].midiMessage + 1);
    Blynk.virtualWrite(BLYNK_MIDICHANNEL,           banks[currentBank][currentPedal].midiChannel);
    Blynk.virtualWrite(BLYNK_MIDICODE,              banks[currentBank][currentPedal].midiCode);
    Blynk.virtualWrite(BLYNK_MIDIVALUE1,            banks[currentBank][currentPedal].midiValue1);
    Blynk.virtualWrite(BLYNK_MIDIVALUE2,            banks[currentBank][currentPedal].midiValue2);
    Blynk.virtualWrite(BLYNK_MIDIVALUE3,            banks[currentBank][currentPedal].midiValue3);
  }
}

void blynk_refresh_pedal()
{
  if (Blynk.connected())
  {
    Blynk.virtualWrite(BLYNK_PEDAL_FUNCTION,        pedals[currentPedal].function + 1);
    switch (pedals[currentPedal].mode) {
      case PED_MOMENTARY1:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           1);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           2);
        break;
      case PED_MOMENTARY2:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           1);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           3);
        break;
      case PED_MOMENTARY3:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           1);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           4);
        break;
      case PED_LATCH1:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           2);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           2);
        break;
      case PED_LATCH2:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           2);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           3);
        break;
      case PED_ANALOG:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           3);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           1);
        break;
      case PED_JOG_WHEEL:
        Blynk.virtualWrite(BLYNK_PEDAL_MODE1,           4);
        Blynk.virtualWrite(BLYNK_PEDAL_MODE2,           1);
        break;
    }
    Blynk.virtualWrite(BLYNK_PEDAL_AUTOSENSING,     pedals[currentPedal].autoSensing);
    Blynk.virtualWrite(BLYNK_PEDAL_POLARITY,        pedals[currentPedal].invertPolarity);
    if (pedals[currentPedal].mode == PED_ANALOG) {
      Blynk.virtualWrite(BLYNK_PEDAL_ANALOGZERO,    pedals[currentPedal].expZero);
      Blynk.virtualWrite(BLYNK_PEDAL_ANALOGMAX,     pedals[currentPedal].expMax);
    }
    else {
      Blynk.virtualWrite(BLYNK_PEDAL_ANALOGZERO,    0);
      Blynk.virtualWrite(BLYNK_PEDAL_ANALOGMAX,     1023);
    }
  }
}

void blynk_refresh_interface()
{
  if (Blynk.connected())
  {
    Blynk.virtualWrite(BLYNK_INTERFACE,             currentInterface + 1);
    Blynk.virtualWrite(BLYNK_INTERFACE_MIDIIN,      interfaces[currentInterface].midiIn);
    Blynk.virtualWrite(BLYNK_INTERFACE_MIDIOUT,     interfaces[currentInterface].midiOut);
    Blynk.virtualWrite(BLYNK_INTERFACE_MIDITHRU,    interfaces[currentInterface].midiThru);
    Blynk.virtualWrite(BLYNK_INTERFACE_MIDIROUTING, interfaces[currentInterface].midiRouting);
    Blynk.virtualWrite(BLYNK_INTERFACE_MIDICLOCK,   interfaces[currentInterface].midiClock);
  }
}

void blynk_refresh_tempo()
{
  if (Blynk.connected())
  {
    switch (currentMidiTimeCode) {

      case PED_MTC_NONE:
        Blynk.virtualWrite(BLYNK_MIDI_TIME_CODE,          1);
        Blynk.virtualWrite(BLYNK_CLOCK_MASTER_SLAVE,      1);
        break;

      case PED_MIDI_CLOCK_MASTER:
        Blynk.virtualWrite(BLYNK_MIDI_TIME_CODE,          2);
        Blynk.virtualWrite(BLYNK_CLOCK_MASTER_SLAVE,      1);
        break;

      case PED_MIDI_CLOCK_SLAVE:
        Blynk.virtualWrite(BLYNK_MIDI_TIME_CODE,          2);
        Blynk.virtualWrite(BLYNK_CLOCK_MASTER_SLAVE,      2);
        break;

      case PED_MTC_MASTER_24:
      case PED_MTC_MASTER_25:
      case PED_MTC_MASTER_30DF:
      case PED_MTC_MASTER_30:
        Blynk.virtualWrite(BLYNK_MIDI_TIME_CODE,          3);
        Blynk.virtualWrite(BLYNK_CLOCK_MASTER_SLAVE,      1);
        break;

      case PED_MTC_SLAVE:
        Blynk.virtualWrite(BLYNK_MIDI_TIME_CODE,          3);
        Blynk.virtualWrite(BLYNK_CLOCK_MASTER_SLAVE,      2);
        break;
    }
    Blynk.virtualWrite(BLYNK_BPM,                   bpm);
  }
}

void blynk_refresh()
{
  if (Blynk.connected())
  {
    blynk_refresh_bank();
    blynk_refresh_pedal();
    blynk_refresh_interface();
    blynk_refresh_tempo();
  }
}

BLYNK_CONNECTED() {
  // This function is called when hardware connects to Blynk Cloud or private server.
  DPRINTLN("Connected to Blynk Cloud");
  blynk_refresh();
  blynkLCD.clear();
  Blynk.virtualWrite(BLYNK_WIFICONNECT, 0);
  Blynk.virtualWrite(BLYNK_SCANWIFI, 0);
  Blynk.setProperty(BLYNK_SSID, "labels", "");
  Blynk.virtualWrite(BLYNK_PASSWORD, "");
  Blynk.virtualWrite(BLYNK_SMARTCONFIG, 0);
}

BLYNK_APP_CONNECTED() {
  //  This function is called every time Blynk app client connects to Blynk server.
  DPRINTLN("Blink App connected");
  blynkLCD.clear();
  screen_update(true);
  blynk_refresh();
}

BLYNK_APP_DISCONNECTED() {
  // This function is called every time the Blynk app disconnects from Blynk Cloud or private server.
  DPRINTLN("Blink App disconnected");
}

BLYNK_WRITE(BLYNK_PROFILE) {
  int profile = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Profile %d\n", profile);
  currentProfile = constrain(profile - 1, 0, PROFILES - 1);
  eeprom_update_current_profile(currentProfile);
  DPRINTLN("Switching profile");
  ESP.restart();
}

BLYNK_WRITE(BLYNK_MIDI_TIME_CODE) {
  int mtc = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MTC %d\n", mtc);
  switch (currentMidiTimeCode) {

    case PED_MTC_NONE:
    case PED_MTC_MASTER_24:
    case PED_MTC_MASTER_25:
    case PED_MTC_MASTER_30DF:
    case PED_MTC_MASTER_30:
    case PED_MIDI_CLOCK_MASTER:
      switch (mtc) {
        case 1:
          MTC.setMode(MidiTimeCode::SynchroNone);
          currentMidiTimeCode = PED_MTC_NONE;
          break;
        case 2:
          MTC.setMode(MidiTimeCode::SynchroClockMaster);
          bpm = (bpm == 0) ? 120 : bpm;
          MTC.setBpm(bpm);
          currentMidiTimeCode = PED_MIDI_CLOCK_MASTER;
          Blynk.virtualWrite(BLYNK_BPM, bpm);
          break;
        case 3:
          MTC.setMode(MidiTimeCode::SynchroMTCMaster);
          MTC.sendPosition(0, 0, 0, 0);
          currentMidiTimeCode = PED_MTC_MASTER_24;
          break;
      }
      break;

    case PED_MTC_SLAVE:
    case PED_MIDI_CLOCK_SLAVE:
      switch (mtc) {
        case 1:
          MTC.setMode(MidiTimeCode::SynchroNone);
          currentMidiTimeCode = PED_MTC_NONE;
          break;
        case 2:
          MTC.setMode(MidiTimeCode::SynchroClockSlave);
          currentMidiTimeCode = PED_MIDI_CLOCK_SLAVE;
          bpm = 0;
          Blynk.virtualWrite(BLYNK_BPM, bpm);
          break;
        case 3:
          MTC.setMode(MidiTimeCode::SynchroMTCSlave);
          currentMidiTimeCode = PED_MTC_SLAVE;
          break;
      }
      break;
  }
}

BLYNK_WRITE(BLYNK_CLOCK_MASTER_SLAVE) {
  int master_slave = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - CLOCK %d\n", master_slave);
  switch (currentMidiTimeCode) {

    case PED_MTC_SLAVE:
    case PED_MTC_MASTER_24:
    case PED_MTC_MASTER_25:
    case PED_MTC_MASTER_30DF:
    case PED_MTC_MASTER_30:
      if (master_slave == 1) {
        MTC.setMode(MidiTimeCode::SynchroMTCMaster);
        MTC.sendPosition(0, 0, 0, 0);
        currentMidiTimeCode = PED_MTC_MASTER_24;
      }
      else
        MTC.setMode(MidiTimeCode::SynchroMTCSlave);
      currentMidiTimeCode = PED_MTC_SLAVE;
      break;

    case PED_MIDI_CLOCK_SLAVE:
    case PED_MIDI_CLOCK_MASTER:
      if (master_slave == 1) {
        MTC.setMode(MidiTimeCode::SynchroClockMaster);
        bpm = (bpm == 0) ? 120 : bpm;
        MTC.setBpm(bpm);
        currentMidiTimeCode = PED_MIDI_CLOCK_MASTER;
      }
      else {
        MTC.setMode(MidiTimeCode::SynchroClockSlave);
        bpm = 0;
        currentMidiTimeCode = PED_MIDI_CLOCK_SLAVE;
      }
      Blynk.virtualWrite(BLYNK_BPM, bpm);
      break;
  }
}

BLYNK_WRITE(BLYNK_BPM) {
  int beatperminute = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - BPM %d\n", beatperminute);
  switch (currentMidiTimeCode) {
    case PED_MIDI_CLOCK_MASTER:
      bpm = constrain(beatperminute, 40, 300);
      MTC.setBpm(bpm);
      break;
  }
}

BLYNK_WRITE(BLYNK_CLOCK_START) {
  int pressed = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Clock Start %d\n", pressed);
  if (pressed) {
    MTC.sendPosition(0, 0, 0, 0);
    MTC.sendPlay();
  }
}

BLYNK_WRITE(BLYNK_CLOCK_STOP) {
  int pressed = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Clock Stop %d\n", pressed);
  if (pressed) MTC.sendStop();
}

BLYNK_WRITE(BLYNK_CLOCK_CONTINUE) {
  int pressed = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Clock Continue %d\n", pressed);
  if (pressed) MTC.sendContinue();
}

BLYNK_WRITE(BLYNK_TAP_TEMPO) {
  int pressed = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Tap Tempo %d\n", pressed);
  if (pressed) {
    bpm = MTC.tapTempo();
    if (bpm > 0) {
      MTC.setBpm(bpm);
      Blynk.virtualWrite(BLYNK_BPM, bpm);
    }
  }
}

BLYNK_WRITE(BLYNK_BANK) {
  int bank = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Bank %d\n", bank);
  currentBank = constrain(bank - 1, 0, BANKS - 1);
  blynk_refresh_bank();
}

BLYNK_WRITE(BLYNK_MIDIMESSAGE) {
  int msg = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - ");
  switch (msg) {
    case 1:
      DPRINTLN("Program Change");
      break;
    case 2:
      DPRINTLN("Control Change");
      break;
    case 3:
      DPRINTLN("Note On/Off");
      break;
    case 4:
      DPRINTLN("Pitch Bend");
      break;
  }
  banks[currentBank][currentPedal].midiMessage = constrain(msg - 1, 0, 3);
}

BLYNK_WRITE(BLYNK_MIDICHANNEL) {
  int channel = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Channel %d\n", channel);
  banks[currentBank][currentPedal].midiChannel = constrain(channel, 1, 16);
}

BLYNK_WRITE(BLYNK_MIDICODE) {
  int code = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Code %d\n", code);
  banks[currentBank][currentPedal].midiCode = constrain(code, 0, 127);
}

BLYNK_WRITE(BLYNK_MIDIVALUE1) {
  int code = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Single Press %d\n", code);
  banks[currentBank][currentPedal].midiValue1 = constrain(code, 0, 127);
}

BLYNK_WRITE(BLYNK_MIDIVALUE2) {
  int code = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Double Press %d\n", code);
  banks[currentBank][currentPedal].midiValue2 = constrain(code, 0, 127);
}

BLYNK_WRITE(BLYNK_MIDIVALUE3) {
  int code = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Long Press %d\n", code);
  banks[currentBank][currentPedal].midiValue3 = constrain(code, 0, 127);
}


BLYNK_WRITE(BLYNK_PEDAL) {
  int pedal = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Pedal %d\n", pedal);
  currentPedal = constrain(pedal - 1, 0, PEDALS - 1);
  blynk_refresh_bank();
  blynk_refresh_pedal();
}

BLYNK_WRITE(BLYNK_PEDAL_MODE1) {
  int mode = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Mode %d\n", mode);
  switch (mode) {
    case 1:
      pedals[currentPedal].mode = PED_MOMENTARY1;
      break;
    case 2:
      pedals[currentPedal].mode = PED_LATCH1;
      break;
    case 3:
      pedals[currentPedal].mode = PED_ANALOG;
      break;
    case 4:
      pedals[currentPedal].mode = PED_JOG_WHEEL;
      break;
  }
}

BLYNK_WRITE(BLYNK_PEDAL_MODE2) {
  int mode = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Mode %d\n", mode);
  switch (mode) {
    case 1:
      pedals[currentPedal].mode = PED_MOMENTARY1;
      break;
    case 2:
      pedals[currentPedal].mode = PED_LATCH1;
      break;
    case 3:
      pedals[currentPedal].mode = PED_ANALOG;
      break;
    case 4:
      pedals[currentPedal].mode = PED_JOG_WHEEL;
      break;
  }
}

BLYNK_WRITE(BLYNK_PEDAL_FUNCTION) {
  int function = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Function %d\n", function);
  pedals[currentPedal].function = function - 1;
}

BLYNK_WRITE(BLYNK_PEDAL_AUTOSENSING) {
  int autosensing = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Autosensing %d\n", autosensing);
  pedals[currentPedal].autoSensing = autosensing;
}

BLYNK_WRITE(BLYNK_PEDAL_POLARITY) {
  int polarity = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Polarity %d\n", polarity);
  pedals[currentPedal].invertPolarity = polarity;
}

BLYNK_WRITE(BLYNK_PEDAL_ANALOGZERO) {
  int analogzero = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Analog Zero %d\n", analogzero);
  pedals[currentPedal].expZero = analogzero;
  pedals[currentPedal].expMax = _max(pedals[currentPedal].expMax, analogzero);
  Blynk.virtualWrite(BLYNK_PEDAL_ANALOGMAX, pedals[currentPedal].expMax);
}

BLYNK_WRITE(BLYNK_PEDAL_ANALOGMAX) {
  int analogmax = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - Analog Max %d\n", analogmax);
  pedals[currentPedal].expZero = _min(pedals[currentPedal].expZero, analogmax);
  pedals[currentPedal].expMax = analogmax;
  Blynk.virtualWrite(BLYNK_PEDAL_ANALOGZERO, pedals[currentPedal].expZero);
}


BLYNK_WRITE(BLYNK_INTERFACE) {
  int interface = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - ");
  switch (interface) {
    case 1:
      DPRINT("USB");
      break;
    case 2:
      DPRINT("DIN");
      break;
    case 3:
      DPRINT("RTP");
      break;
    case 4:
      DPRINT("BLE");
      break;
  }
  DPRINTLN(" MIDI interface");
  currentInterface = constrain(interface - 1, 0, INTERFACES);
  blynk_refresh_interface();
}

BLYNK_WRITE(BLYNK_INTERFACE_MIDIIN) {
  int onoff = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI IN %d\n", onoff);
  interfaces[currentInterface].midiIn = onoff;
}

BLYNK_WRITE(BLYNK_INTERFACE_MIDIOUT) {
  int onoff = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI OUT %d\n", onoff);
  interfaces[currentInterface].midiOut = onoff;
}

BLYNK_WRITE(BLYNK_INTERFACE_MIDITHRU) {
  int onoff = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI THRU %d\n", onoff);
  interfaces[currentInterface].midiThru = onoff;
}

BLYNK_WRITE(BLYNK_INTERFACE_MIDIROUTING) {
  int onoff = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Routing %d\n", onoff);
  interfaces[currentInterface].midiRouting = onoff;
}

BLYNK_WRITE(BLYNK_INTERFACE_MIDICLOCK) {
  int onoff = param.asInt();
  PRINT_VIRTUAL_PIN(request.pin);
  DPRINT(" - MIDI Clock %d\n", onoff);
  interfaces[currentInterface].midiClock = onoff;
}

#ifdef WIFI

BLYNK_WRITE(BLYNK_WIFICONNECT) {
  WiFi.scanDelete();
  if (ap_connect(ssid, password))
    Blynk.connect();
  else if (auto_reconnect())
    Blynk.connect();
  Blynk.virtualWrite(BLYNK_WIFICONNECT, 0);
}

/*
  BLYNK_WRITE(BLYNK_WIFISTATUS) {
  int wifiOnOff = param.asInt();
  switch (wifiOnOff) {
    case 0: // OFF
      WiFi.mode(WIFI_OFF);
      DPRINTLN("WiFi Off Mode");
      break;

    case 1: // ON
      wifi_connect();
      Blynk.connect();
      break;
  }
  }
*/

BLYNK_WRITE(BLYNK_SCANWIFI) {
  int scan = param.asInt();
  if (scan) {
    DPRINTLN("WiFi Scan started");
    int networksFound = WiFi.scanNetworks();
    DPRINTLN("WiFi Scan done");
    if (networksFound == 0) {
      DPRINTLN("No networks found");
    } else {
      DPRINTLN("%d network(s) found", networksFound);
      BlynkParamAllocated items(512); // list length, in bytes
      for (int i = 0; i < networksFound; i++) {
        DPRINTLN("%2d.\n BSSID: %s\n SSID: %s\n Channel: %d\n Signal: %d dBm\n Auth Mode: %s", i + 1, WiFi.BSSIDstr(i).c_str(), WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), translateEncryptionType(WiFi.encryptionType(i)).c_str());
        items.add(WiFi.SSID(i).c_str());
      }
      Blynk.setProperty(BLYNK_SSID, "labels", items);
      if (networksFound > 0) {
        Blynk.virtualWrite(BLYNK_SSID, 1);
        ssid = WiFi.SSID(0);
      }
      Blynk.virtualWrite(BLYNK_SCANWIFI, 0);
    }
  }
}

BLYNK_WRITE(BLYNK_SSID) {
  int i = param.asInt();
  ssid = WiFi.SSID(i - 1);
  DPRINTLN("SSID     : %s", ssid.c_str());
}

BLYNK_WRITE(BLYNK_PASSWORD) {
  password = param.asStr();
  DPRINTLN("Password : %s", password.c_str());
}

BLYNK_WRITE(BLYNK_SMARTCONFIG) {
  int smartconfig = param.asInt();
  if (smartconfig) {
    if (smart_config()) blynk_connect();
  }
}

#endif  // WIFI
#endif  // NOBLYNK
