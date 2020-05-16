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

#ifdef BLE
#include <BLE-MIDI.h>
#include <hardware/BLE-MIDI_ESP32.h>
#endif

#define BLE_LED         2
#define BLE_LED_OFF()   digitalWrite(BLE_LED, LOW)
#define BLE_LED_ON()    digitalWrite(BLE_LED, HIGH)

// Bluetooth LE MIDI interface

#ifdef BLE
BLEMIDI_CREATE_INSTANCE(BLEMIDI_NAMESPACE::BLEMIDI_ESP32, host.c_str(), BLE_MIDI);
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
#define BLESendStop(...)
#define BLESendActiveSensing(...)
#define BLESendSystemReset(...)
#else

void BLESendNoteOn(byte note, byte velocity, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendNoteOn(note, velocity, channel);
}

void BLESendNoteOff(byte note, byte velocity, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendNoteOff(note, velocity, channel);
}

void BLESendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendAfterTouch(note, pressure, channel);
}

void BLESendControlChange(byte number, byte value, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendControlChange(number, value, channel);
}

void BLESendProgramChange(byte number, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendProgramChange(number, channel);
}

void BLESendAfterTouch(byte pressure, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendAfterTouch(pressure, channel);
}

void BLESendPitchBend(int bend, byte channel)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendPitchBend(bend, channel);
}

void BLESendSystemExclusive(const byte* array, unsigned size)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendSysEx(size, array);
}

void BLESendTimeCodeQuarterFrame(byte data)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendTimeCodeQuarterFrame(data);
}

void BLESendSongPosition(unsigned int beats)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendSongPosition(beats);
}

void BLESendSongSelect(byte songnumber)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendSongSelect(songnumber);
}

void BLESendTuneRequest(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendTuneRequest();
}

void BLESendClock(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendClock();
}

void BLESendStart(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendStart();
}

void BLESendContinue(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendContinue();
}

void BLESendStop(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendStop();
}

void BLESendActiveSensing(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendActiveSensing();
}

void BLESendSystemReset(void)
{
  if (bleEnabled && interfaces[PED_BLEMIDI].midiOut) BLE_MIDI.sendSystemReset();
}

#endif  // NOBLE