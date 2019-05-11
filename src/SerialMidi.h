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

#include <MIDI.h>

// Forward messages received from serial MIDI interface to WiFI interface

void OnSerialMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
}

void OnSerialMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
}

void OnSerialMidiAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
}

void OnSerialMidiControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
}

void OnSerialMidiProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
}

void OnSerialMidiAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
}

void OnSerialMidiPitchBend(byte channel, int bend)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
}

void OnSerialMidiSystemExclusive(byte* array, unsigned size)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
}

void OnSerialMidiTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
}

void OnSerialMidiSongPosition(unsigned int beats)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnSerialMidiSongSelect(byte songnumber)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnSerialMidiTuneRequest(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendTuneRequest();
  ipMIDISendTuneRequest();
  AppleMidiSendTuneRequest();
  OSCSendTuneRequest();
}

void OnSerialMidiClock(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendClock();
  ipMIDISendClock();
  AppleMidiSendClock();
  OSCSendClock();
}

void OnSerialMidiStart(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendStart();
  ipMIDISendStart();
  AppleMidiSendStart();
  OSCSendStart();
}

void OnSerialMidiContinue(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendContinue();
  ipMIDISendContinue();
  AppleMidiSendContinue();
  OSCSendContinue();
}

void OnSerialMidiStop(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendStop();
  ipMIDISendStop();
  AppleMidiSendStop();
  OSCSendStop();
}

void OnSerialMidiActiveSensing(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  BLESendActiveSensing();
  ipMIDISendActiveSensing();
  AppleMidiSendActiveSensing();
  OSCSendActiveSensing();
}

void OnSerialMidiSystemReset(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;
  
  BLESendSystemReset();
  ipMIDISendSystemReset();
  AppleMidiSendSystemReset();
  OSCSendSystemReset();
}

void serial_midi_connect()
{
  // Connect the handle function called upon reception of a MIDI message from serial MIDI interface
  MIDI.setHandleNoteOn(OnSerialMidiNoteOn);
  MIDI.setHandleNoteOff(OnSerialMidiNoteOff);
  MIDI.setHandleAfterTouchPoly(OnSerialMidiAfterTouchPoly);
  MIDI.setHandleControlChange(OnSerialMidiControlChange);
  MIDI.setHandleProgramChange(OnSerialMidiProgramChange);
  MIDI.setHandleAfterTouchChannel(OnSerialMidiAfterTouchChannel);
  MIDI.setHandlePitchBend(OnSerialMidiPitchBend);
  MIDI.setHandleSystemExclusive(OnSerialMidiSystemExclusive);
  MIDI.setHandleTimeCodeQuarterFrame(OnSerialMidiTimeCodeQuarterFrame);
  MIDI.setHandleSongPosition(OnSerialMidiSongPosition);
  MIDI.setHandleSongSelect(OnSerialMidiSongSelect);
  MIDI.setHandleTuneRequest(OnSerialMidiTuneRequest);
  MIDI.setHandleClock(OnSerialMidiClock);
  MIDI.setHandleStart(OnSerialMidiStart);
  MIDI.setHandleContinue(OnSerialMidiContinue);
  MIDI.setHandleStop(OnSerialMidiStop);
  MIDI.setHandleActiveSensing(OnSerialMidiActiveSensing);
  MIDI.setHandleSystemReset(OnSerialMidiSystemReset);

  // Initiate serial MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
}
