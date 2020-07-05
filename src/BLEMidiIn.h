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

#undef CLASSIC_BT_ENABLED

#ifdef BLE
#include <BLE-MIDI.h>
#include <hardware/BLE-MIDI_ESP32.h>

void OnBleMidiConnected() {
  bleMidiConnected = true;
  DPRINTLN("BLE MIDI Device Connected");
}

void OnBleMidiDisconnected() {
  bleMidiConnected = false;
  DPRINTLN("BLE MIDI Device Disconnected");
}

void OnBleMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOn(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
  leds_update(midi::NoteOn, channel, note, velocity);
  screen_info(midi::NoteOn, note, velocity, channel);
}

void OnBleMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
  screen_info(midi::NoteOff, note, velocity, channel);
}

void OnBleMidiReceiveAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
  screen_info(midi::AfterTouchPoly, note, pressure, channel);
}

void OnBleMidiReceiveControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
  leds_update(midi::ControlChange, channel, number, value);
  screen_info(midi::ControlChange, number, value, channel);
}

void OnBleMidiReceiveProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
  leds_update(midi::ProgramChange, channel, number, 0);
  screen_info(midi::ProgramChange, number, 0, channel);
}

void OnBleMidiReceiveAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
  screen_info(midi::AfterTouchChannel, pressure, 0, channel);
}

void OnBleMidiReceivePitchBend(byte channel, int bend)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
  screen_info(midi::PitchBend, bend, 0, channel);
}

void OnBleMidiReceiveSystemExclusive(byte *array, unsigned int size)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSysEx(size, array);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, array);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
}

void OnBleMidiReceiveTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTimeCodeQuarterFrame(data);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(data);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
}

void OnBleMidiReceiveSongPosition(unsigned int beats)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongPosition(beats);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnBleMidiReceiveSongSelect(byte songnumber)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongSelect(songnumber);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(songnumber);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnBleMidiReceiveTuneRequest(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTuneRequest();
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTuneRequest();
  if (interfaces[PED_BLEMIDI].midiThru) BLESendTuneRequest();
  AppleMidiSendTuneRequest();
  ipMIDISendTuneRequest();
  OSCSendTuneRequest();
}

void OnBleMidiReceiveClock(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Clock);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendClock();
  AppleMidiSendClock();
  ipMIDISendClock();
  OSCSendClock();
}

void OnBleMidiReceiveStart(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Start);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendStart();
  AppleMidiSendStart();
  ipMIDISendStart();
  OSCSendStart();
}

void OnBleMidiReceiveContinue(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Continue);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendContinue();
  AppleMidiSendContinue();
  ipMIDISendContinue();
  OSCSendContinue();
}

void OnBleMidiReceiveStop(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Stop);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendStop();
  AppleMidiSendStop();
  ipMIDISendStop();
  OSCSendStop();
}

void OnBleMidiReceiveActiveSensing(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::ActiveSensing);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendActiveSensing();
  AppleMidiSendActiveSensing();
  ipMIDISendActiveSensing();
  OSCSendActiveSensing();
}

void OnBleMidiReceiveSystemReset(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::SystemReset);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSystemReset();
  AppleMidiSendSystemReset();
  ipMIDISendSystemReset();
  OSCSendSystemReset();
}

void ble_midi_start_service()
{
  if (!bleEnabled) return;

  // Initiate BLE MIDI communications, listen to all channels
  BLE_MIDI.begin(MIDI_CHANNEL_OMNI);

  BLEBLE_MIDI.onConnected(OnBleMidiConnected);
  BLEBLE_MIDI.onDisconnected(OnBleMidiDisconnected);

  // Connect the handle function called upon reception of a MIDI message from BLE MIDI interface
  BLE_MIDI.setHandleNoteOn(OnBleMidiNoteOn);
  BLE_MIDI.setHandleNoteOff(OnBleMidiNoteOff);
  BLE_MIDI.setHandleAfterTouchPoly(OnBleMidiReceiveAfterTouchPoly);
  BLE_MIDI.setHandleControlChange(OnBleMidiReceiveControlChange);
  BLE_MIDI.setHandleProgramChange(OnBleMidiReceiveProgramChange);
  BLE_MIDI.setHandleAfterTouchChannel(OnBleMidiReceiveAfterTouchChannel);
  BLE_MIDI.setHandlePitchBend(OnBleMidiReceivePitchBend);
  BLE_MIDI.setHandleSystemExclusive(OnBleMidiReceiveSystemExclusive);
  BLE_MIDI.setHandleTimeCodeQuarterFrame(OnBleMidiReceiveTimeCodeQuarterFrame);
  BLE_MIDI.setHandleSongPosition(OnBleMidiReceiveSongPosition);
  BLE_MIDI.setHandleSongSelect(OnBleMidiReceiveSongSelect);
  BLE_MIDI.setHandleTuneRequest(OnBleMidiReceiveTuneRequest);
  BLE_MIDI.setHandleClock(OnBleMidiReceiveClock);
  BLE_MIDI.setHandleStart(OnBleMidiReceiveStart);
  BLE_MIDI.setHandleContinue(OnBleMidiReceiveContinue);
  BLE_MIDI.setHandleStop(OnBleMidiReceiveStop);
  BLE_MIDI.setHandleActiveSensing(OnBleMidiReceiveActiveSensing);
  BLE_MIDI.setHandleSystemReset(OnBleMidiReceiveSystemReset);
}

#endif  // BLE