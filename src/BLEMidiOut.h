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

bool                  bleMidiConnected = false;
unsigned long         bleLastOn        = 0;
String                bleServer        = "";

#ifdef BLE
#include <BLEMIDI_Transport.h>
#ifdef BLECLIENT
#include <hardware/BLEMIDI_Client_ESP32.h>
#else
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#endif
#endif

// Bluetooth LE MIDI interface

#ifdef BLE
#ifdef BLECLIENT
BLEMIDI_CREATE_INSTANCE(bleServer.c_str(), BLE_MIDI);
#else
BLEMIDI_CREATE_INSTANCE(host.c_str(), BLE_MIDI);
#endif
#endif

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
#define BLESendRealTimeMessage(...)
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

void BLESendRealTimeMessage(byte type)
{
  switch (type) {

      case midi::TuneRequest:
        BLESendTuneRequest();
        break;

      case midi::Clock:
        BLESendClock();
        break;

      case midi::Start:
        BLESendStart();
        break;

      case midi::Continue:
        BLESendContinue();
        break;

      case midi::Stop:
        BLESendStop();
        break;

      case midi::ActiveSensing:
        BLESendActiveSensing();
        break;

      case midi::SystemReset:
        BLESendSystemReset();
        break;
    }
}

#endif  // NOBLE