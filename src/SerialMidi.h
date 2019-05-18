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

// Forward messages received from USB MIDI interface to WiFI interface

void OnUSBMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
  BLESendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
}

void OnUSBMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
}

void OnUSBMidiAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
}

void OnUSBMidiControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
}

void OnUSBMidiProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
}

void OnUSBMidiAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
}

void OnUSBMidiPitchBend(byte channel, int bend)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
}

void OnUSBMidiSystemExclusive(byte* array, unsigned size)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, array);
  BLESendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
}

void OnUSBMidiTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(data);
  BLESendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
}

void OnUSBMidiSongPosition(unsigned int beats)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
  BLESendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnUSBMidiSongSelect(byte songnumber)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(songnumber);
  BLESendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnUSBMidiTuneRequest(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTuneRequest();
  BLESendTuneRequest();
  ipMIDISendTuneRequest();
  AppleMidiSendTuneRequest();
  OSCSendTuneRequest();
}

void OnUSBMidiClock(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
  BLESendClock();
  ipMIDISendClock();
  AppleMidiSendClock();
  OSCSendClock();
}

void OnUSBMidiStart(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
  BLESendStart();
  ipMIDISendStart();
  AppleMidiSendStart();
  OSCSendStart();
}

void OnUSBMidiContinue(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
  BLESendContinue();
  ipMIDISendContinue();
  AppleMidiSendContinue();
  OSCSendContinue();
}

void OnUSBMidiStop(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
  BLESendStop();
  ipMIDISendStop();
  AppleMidiSendStop();
  OSCSendStop();
}

void OnUSBMidiActiveSensing(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;

  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
  BLESendActiveSensing();
  ipMIDISendActiveSensing();
  AppleMidiSendActiveSensing();
  OSCSendActiveSensing();
}

void OnUSBMidiSystemReset(void)
{
  if (!interfaces[PED_USBMIDI].midiIn) return;
  
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
  BLESendSystemReset();
  ipMIDISendSystemReset();
  AppleMidiSendSystemReset();
  OSCSendSystemReset();
}

// Forward messages received from serial MIDI interface to WiFI interface

void OnSerialMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOn(note, velocity, channel);
  BLESendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
}

void OnSerialMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
}

void OnSerialMidiAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
}

void OnSerialMidiControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
}

void OnSerialMidiProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  ipMIDISendProgramChange(number, channel);
  AppleMidiSendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
}

void OnSerialMidiAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
}

void OnSerialMidiPitchBend(byte channel, int bend)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
}

void OnSerialMidiSystemExclusive(byte* array, unsigned size)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSysEx(size, array);
  BLESendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
}

void OnSerialMidiTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTimeCodeQuarterFrame(data);
  BLESendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
}

void OnSerialMidiSongPosition(unsigned int beats)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongPosition(beats);
  BLESendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnSerialMidiSongSelect(byte songnumber)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongSelect(songnumber);
  BLESendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnSerialMidiTuneRequest(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTuneRequest();
  BLESendTuneRequest();
  ipMIDISendTuneRequest();
  AppleMidiSendTuneRequest();
  OSCSendTuneRequest();
}

void OnSerialMidiClock(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Clock);
  BLESendClock();
  ipMIDISendClock();
  AppleMidiSendClock();
  OSCSendClock();
}

void OnSerialMidiStart(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Start);
  BLESendStart();
  ipMIDISendStart();
  AppleMidiSendStart();
  OSCSendStart();
}

void OnSerialMidiContinue(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Continue);
  BLESendContinue();
  ipMIDISendContinue();
  AppleMidiSendContinue();
  OSCSendContinue();
}

void OnSerialMidiStop(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Stop);
  BLESendStop();
  ipMIDISendStop();
  AppleMidiSendStop();
  OSCSendStop();
}

void OnSerialMidiActiveSensing(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::ActiveSensing);
  BLESendActiveSensing();
  ipMIDISendActiveSensing();
  AppleMidiSendActiveSensing();
  OSCSendActiveSensing();
}

void OnSerialMidiSystemReset(void)
{
  if (!interfaces[PED_DINMIDI].midiIn) return;
  
  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::SystemReset);
  BLESendSystemReset();
  ipMIDISendSystemReset();
  AppleMidiSendSystemReset();
  OSCSendSystemReset();
}

void serial_midi_connect()
{
  // Connect the handle function called upon reception of a MIDI message from USB MIDI interface
  USB_MIDI.setHandleNoteOn(OnUSBMidiNoteOn);
  USB_MIDI.setHandleNoteOff(OnUSBMidiNoteOff);
  USB_MIDI.setHandleAfterTouchPoly(OnUSBMidiAfterTouchPoly);
  USB_MIDI.setHandleControlChange(OnUSBMidiControlChange);
  USB_MIDI.setHandleProgramChange(OnUSBMidiProgramChange);
  USB_MIDI.setHandleAfterTouchChannel(OnUSBMidiAfterTouchChannel);
  USB_MIDI.setHandlePitchBend(OnUSBMidiPitchBend);
  USB_MIDI.setHandleSystemExclusive(OnUSBMidiSystemExclusive);
  USB_MIDI.setHandleTimeCodeQuarterFrame(OnUSBMidiTimeCodeQuarterFrame);
  USB_MIDI.setHandleSongPosition(OnUSBMidiSongPosition);
  USB_MIDI.setHandleSongSelect(OnUSBMidiSongSelect);
  USB_MIDI.setHandleTuneRequest(OnUSBMidiTuneRequest);
  USB_MIDI.setHandleClock(OnUSBMidiClock);
  USB_MIDI.setHandleStart(OnUSBMidiStart);
  USB_MIDI.setHandleContinue(OnUSBMidiContinue);
  USB_MIDI.setHandleStop(OnUSBMidiStop);
  USB_MIDI.setHandleActiveSensing(OnUSBMidiActiveSensing);
  USB_MIDI.setHandleSystemReset(OnUSBMidiSystemReset);

  // Initiate USB MIDI communications, listen to all channels
  USB_MIDI.begin(MIDI_CHANNEL_OMNI);
  interfaces[PED_USBMIDI].midiThru ? USB_MIDI.turnThruOn() : USB_MIDI.turnThruOff();

  // Connect the handle function called upon reception of a MIDI message from serial MIDI interface
  DIN_MIDI.setHandleNoteOn(OnSerialMidiNoteOn);
  DIN_MIDI.setHandleNoteOff(OnSerialMidiNoteOff);
  DIN_MIDI.setHandleAfterTouchPoly(OnSerialMidiAfterTouchPoly);
  DIN_MIDI.setHandleControlChange(OnSerialMidiControlChange);
  DIN_MIDI.setHandleProgramChange(OnSerialMidiProgramChange);
  DIN_MIDI.setHandleAfterTouchChannel(OnSerialMidiAfterTouchChannel);
  DIN_MIDI.setHandlePitchBend(OnSerialMidiPitchBend);
  DIN_MIDI.setHandleSystemExclusive(OnSerialMidiSystemExclusive);
  DIN_MIDI.setHandleTimeCodeQuarterFrame(OnSerialMidiTimeCodeQuarterFrame);
  DIN_MIDI.setHandleSongPosition(OnSerialMidiSongPosition);
  DIN_MIDI.setHandleSongSelect(OnSerialMidiSongSelect);
  DIN_MIDI.setHandleTuneRequest(OnSerialMidiTuneRequest);
  DIN_MIDI.setHandleClock(OnSerialMidiClock);
  DIN_MIDI.setHandleStart(OnSerialMidiStart);
  DIN_MIDI.setHandleContinue(OnSerialMidiContinue);
  DIN_MIDI.setHandleStop(OnSerialMidiStop);
  DIN_MIDI.setHandleActiveSensing(OnSerialMidiActiveSensing);
  DIN_MIDI.setHandleSystemReset(OnSerialMidiSystemReset);

  // Initiate serial MIDI communications, listen to all channels
  DIN_MIDI.begin(MIDI_CHANNEL_OMNI);
  interfaces[PED_DINMIDI].midiThru ? DIN_MIDI.turnThruOn() : DIN_MIDI.turnThruOff();
}