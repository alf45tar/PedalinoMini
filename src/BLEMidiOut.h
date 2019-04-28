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

#ifdef BLE
#include "BleMidi.h"
#endif

#ifdef ARDUINO_ARCH_ESP8266
#define BLE_LED_OFF()
#define BLE_LED_ON()
#endif

#ifdef ARDUINO_ARCH_ESP32
#define BLE_LED         2
#define BLE_LED_OFF()   digitalWrite(BLE_LED, LOW)
#define BLE_LED_ON()    digitalWrite(BLE_LED, HIGH)
#endif

// Bluetooth LE MIDI interface

#if defined(ARDUINO_ARCH_ESP32) && defined(BLE)
BLEMIDI_CREATE_INSTANCE(BleMIDI);
#endif

bool                  bleMidiConnected = false;
unsigned long         bleLastOn        = 0;

#ifdef NOBLE
#define BLEMidiReceive(...)
#define BLESendNoteOn(...)
#define BLESendNoteOff(...)
#define BLESendAfterTouchPoly(...)
#define BLESendControlChange(...)
#define BLESendProgramChange(...)
#define BLESendAfterTouch(...)
#define BLESendPitchBend(...)
#define BLESendSystemExclusive(...)
#define BLESendTimeCodeQuarterFrame(...)
#define BLESendSongPosition(...)
#define BLESendSongSelect(...)
#define BLESendTuneRequest(...)
#define BLESendClock(...)
#define BLESendStart(...)
#define BLESendContinue(...)
#define BLESendStop(...) {}
#define BLESendActiveSensing(...)
#define BLESendSystemReset(...)
#else

void BLESendNoteOn(byte note, byte velocity, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendNoteOn(note, velocity, channel);
}

void BLESendNoteOff(byte note, byte velocity, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendNoteOff(note, velocity, channel);
}

void BLESendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendAfterTouch(note, pressure, channel);
}

void BLESendControlChange(byte number, byte value, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendControlChange(number, value, channel);
}

void BLESendProgramChange(byte number, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendProgramChange(number, channel);
}

void BLESendAfterTouch(byte pressure, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendAfterTouch(pressure, channel);
}

void BLESendPitchBend(int bend, byte channel)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendPitchBend(bend, channel);
}

void BLESendSystemExclusive(const byte* array, unsigned size)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendSysEx(array, size);
}

void BLESendTimeCodeQuarterFrame(byte data)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendTimeCodeQuarterFrame(data);
}

void BLESendSongPosition(unsigned int beats)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendSongPosition(beats);
}

void BLESendSongSelect(byte songnumber)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendSongSelect(songnumber);
}

void BLESendTuneRequest(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendTuneRequest();
}

void BLESendClock(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendClock();
}

void BLESendStart(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendStart();
}

void BLESendContinue(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendContinue();
}

void BLESendStop(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendStop();
}

void BLESendActiveSensing(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendActiveSensing();
}

void BLESendSystemReset(void)
{
  if (interfaces[PED_BLEMIDI].midiOut) BleMIDI.sendReset();
}

#endif  // NOBLE