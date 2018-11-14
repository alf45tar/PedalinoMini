/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

#ifdef BLE
#include "BleMidi.h"

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
  DPRINT("Received from BLEMIDI  NoteOn  0x%02X   Velocity 0x%02X   Channel %02d\n", note, velocity, channel);

  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendNoteOn(note, velocity, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
}

void OnBleMidiNoteOff(byte channel, byte note, byte velocity)
{
  DPRINT("Received from BLEMIDI  NoteOff 0x%02X   Velocity 0x%02X   Channel %02d\n", note, velocity, channel);

  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendNoteOff(note, velocity, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
}

void OnBleMidiReceiveAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendAfterTouch(note, pressure, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
}

void OnBleMidiReceiveControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendControlChange(number, value, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
}

void OnBleMidiReceiveProgramChange(byte channel, byte number)
{
  DPRINT("Received from BLEMIDI   ProgramChange 0x%02X   Channel %02d\n", number, channel);

  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
}

void OnBleMidiReceiveAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendAfterTouch(pressure, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
}

void OnBleMidiReceivePitchBend(byte channel, int bend)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendPitchBend(bend, channel);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
}

void OnBleMidiReceiveSysEx(const byte *data, uint16_t size)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendSysEx(size, data);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSystemExclusive(data, size);
  AppleMidiSendSystemExclusive(data, size);
  ipMIDISendSystemExclusive(data, size);
  OSCSendSystemExclusive(data, size);
}

void OnBleMidiReceiveTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendTimeCodeQuarterFrame(data);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
}

void OnBleMidiReceiveSongPosition(unsigned short beats)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendSongPosition(beats);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnBleMidiReceiveSongSelect(byte songnumber)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendSongSelect(songnumber);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnBleMidiReceiveTuneRequest(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendTuneRequest();
  if (interfaces[PED_BLEMIDI].midiThru) BLESendTuneRequest();
  AppleMidiSendTuneRequest();
  ipMIDISendTuneRequest();
  OSCSendTuneRequest();
}

void OnBleMidiReceiveClock(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendRealTime(midi::Clock);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendClock();
  AppleMidiSendClock();
  ipMIDISendClock();
  OSCSendClock();
}

void OnBleMidiReceiveStart(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendRealTime(midi::Start);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendStart();
  AppleMidiSendStart();
  ipMIDISendStart();
  OSCSendStart();
}

void OnBleMidiReceiveContinue(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendRealTime(midi::Continue);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendContinue();
  AppleMidiSendContinue();
  ipMIDISendContinue();
  OSCSendContinue();
}

void OnBleMidiReceiveStop(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendRealTime(midi::Stop);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendStop();
  AppleMidiSendStop();
  ipMIDISendStop();
  OSCSendStop();
}

void OnBleMidiReceiveActiveSensing(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendRealTime(midi::ActiveSensing);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendActiveSensing();
  AppleMidiSendActiveSensing();
  ipMIDISendActiveSensing();
  OSCSendActiveSensing();
}

void OnBleMidiReceiveReset(void)
{
  if (!interfaces[PED_BLEMIDI].midiIn) return;

  MIDI.sendRealTime(midi::SystemReset);
  if (interfaces[PED_BLEMIDI].midiThru) BLESendSystemReset();
  AppleMidiSendSystemReset();
  ipMIDISendSystemReset();
  OSCSendSystemReset();
}

void ble_midi_start_service()
{
  // Create a session and wait for a remote host to connect to us
  BleMIDI.begin("Pedalino");

  BleMIDI.onConnected(OnBleMidiConnected);
  BleMIDI.onDisconnected(OnBleMidiDisconnected);

  // Connect the handle function called upon reception of a MIDI message from BLE MIDI interface
  BleMIDI.setHandleNoteOn(OnBleMidiNoteOn);
  BleMIDI.setHandleNoteOff(OnBleMidiNoteOff);
  BleMIDI.setHandleAfterTouchPoly(OnBleMidiReceiveAfterTouchPoly);
  BleMIDI.setHandleControlChange(OnBleMidiReceiveControlChange);
  BleMIDI.setHandleProgramChange(OnBleMidiReceiveProgramChange);
  BleMIDI.setHandleAfterTouchChannel(OnBleMidiReceiveAfterTouchChannel);
  BleMIDI.setHandlePitchBend(OnBleMidiReceivePitchBend);
  BleMIDI.setHandleSysEx(OnBleMidiReceiveSysEx);
  BleMIDI.setHandleTimeCodeQuarterFrame(OnBleMidiReceiveTimeCodeQuarterFrame);
  BleMIDI.setHandleSongPosition(OnBleMidiReceiveSongPosition);
  BleMIDI.setHandleSongSelect(OnBleMidiReceiveSongSelect);
  BleMIDI.setHandleTuneRequest(OnBleMidiReceiveTuneRequest);
  BleMIDI.setHandleClock(OnBleMidiReceiveClock);
  BleMIDI.setHandleStart(OnBleMidiReceiveStart);
  BleMIDI.setHandleContinue(OnBleMidiReceiveContinue);
  BleMIDI.setHandleStop(OnBleMidiReceiveStop);
  BleMIDI.setHandleActiveSensing(OnBleMidiReceiveActiveSensing);
  BleMIDI.setHandleReset(OnBleMidiReceiveReset);
}

#endif  // BLE