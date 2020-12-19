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

#ifdef WIFI
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncUDP.h>
#include <AppleMIDI.h>
#include <ipMIDI.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#endif  // WIFI

#define DPRINTMIDI(...)   printMIDI(__VA_ARGS__)

void printMIDI(const char *interface, midi::StatusByte status, const byte *data)
{
  midi::MidiType  type;
  midi::Channel   channel;
  byte            note, velocity, pressure, number, value;
  int             bend;
  unsigned int    beats;

  type    = DIN_MIDI.getTypeFromStatusByte(status);
  channel = DIN_MIDI.getChannelFromStatusByte(status);

  switch (type) {
    case midi::NoteOff:
    case midi::NoteOn:
    case midi::ControlChange:
    case midi::ProgramChange:
    case midi::PitchBend:
    case midi::AfterTouchChannel:
      lastUsed = 0;
      lastUsedPedal = 0;
      lastPedalName[0] = 0;
      screen_info(type, data[0], data[1], channel);
      break;

    default:
      break;
  }

  switch (type) {

    case midi::NoteOff:
      note     = data[0];
      velocity = data[1];
      DPRINTLN("Received from %s  NoteOff 0x%02X   Velocity 0x%02X   Channel %02d", interface, note, velocity, channel);
      break;

    case midi::NoteOn:
      note     = data[0];
      velocity = data[1];
      DPRINTLN("Received from %s  NoteOn  0x%02X   Velocity 0x%02X   Channel %02d", interface, note, velocity, channel);
      break;

    case midi::AfterTouchPoly:
      note     = data[0];
      pressure = data[1];
      DPRINTLN("Received from %s  AfterTouchPoly   Note 0x%02X   Pressure 0x%02X   Channel %02d", interface, note, pressure, channel);
      break;

    case midi::ControlChange:
      number  = data[0];
      value   = data[1];
      DPRINTLN("Received from %s  ControlChange 0x%02X   Value 0x%02X   Channel %02d", interface, number, value, channel);
      break;

    case midi::ProgramChange:
      number  = data[0];
      DPRINTLN("Received from %s  ProgramChange 0x%02X   Channel %02d", interface, number, channel);
      break;

    case midi::AfterTouchChannel:
      pressure = data[0];
      DPRINTLN("Received from %s  AfterTouchChannel   Pressure 0x%02X   Channel %02d", interface, pressure, channel);
      break;

    case midi::PitchBend:
      bend = data[1] << 7 | data[0];
      DPRINTLN("Received from %s  PitchBend   Bend 0x%02X   Channel %02d", interface, bend, channel);
      break;

    case midi::SystemExclusive:
      DPRINTLN("Received from %s  SystemExclusive 0x%02X", interface, data[0]);
      break;

    case midi::TimeCodeQuarterFrame:
      value = data[0];
      DPRINTLN("Received from %s  TimeCodeQuarterFrame 0x%02X", interface, value);
      break;

    case midi::SongPosition:
      beats = data[1] << 7 | data[0];
      DPRINTLN("Received from %s  SongPosition Beats 0x%04X", interface, beats);
      break;

    case midi::SongSelect:
      number = data[0];
      DPRINTLN("Received from %s  SongSelect 0x%02X", interface, number);
      break;

    case midi::TuneRequest:
      DPRINTLN("Received from %s  TuneRequest", interface);
      break;

    case midi::Clock:
      //DPRINTLN("Received from %s  Clock", interface);
      break;

    case midi::Start:
      DPRINTLN("Received from %s  Start", interface);
      break;

    case midi::Continue:
      DPRINTLN("Received from %s  Continue", interface);
      break;

    case midi::Stop:
      DPRINTLN("Received from %s  Stop", interface);
      break;

    case midi::ActiveSensing:
      DPRINTLN("Received from %s  ActiveSensing", interface);
      break;

    case midi::SystemReset:
      DPRINTLN("Received from %s  SystemReset", interface);
      break;

    default:
      break;
  }
}

void printMIDI (const char *interface, const midi::MidiType type, const midi::Channel channel, const byte data1, const byte data2)
{
  midi::StatusByte  status;
  byte              data[2];

  status = type | ((channel - 1) & 0x0f);
  data[0] = data1;
  data[1] = data2;
  printMIDI(interface, status, data);
}


#ifdef NOWIFI
#define apple_midi_start(...)
#define ip_midi_start(...)
#define oscUDP_listen(...)
#else

// Forward messages received from Apple MIDI interface to other interfaces

void OnAppleMidiConnected(const ssrc_t & ssrc, const char* name)
{
  appleMidiConnected  = true;
  appleMidiSessionName = String(name);
  DPRINTLN("AppleMIDI Connected Session ID: %u Name: %s", ssrc, name);
}

void OnAppleMidiDisconnected(const ssrc_t & ssrc)
{
  appleMidiConnected  = false;
  appleMidiSessionName = "";
  DPRINTLN("AppleMIDI Disconnected Session ID %u", ssrc);
}

void OnAppleMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOn(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
  BLESendNoteOn(note, velocity, channel);
  ipMIDISendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
  leds_update(midi::NoteOn, channel, note, velocity);
  screen_info(midi::NoteOn, note, velocity, channel);
}

void OnAppleMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
  screen_info(midi::NoteOff, note, velocity, channel);
}

void OnAppleMidiReceiveAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
  screen_info(midi::AfterTouchPoly, note, pressure, channel);
}

void OnAppleMidiReceiveControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
  leds_update(midi::ControlChange, channel, number, value);
  screen_info(midi::ControlChange, number, value, channel);
}

void OnAppleMidiReceiveProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
  leds_update(midi::ProgramChange, channel, number, 0);
  screen_info(midi::ProgramChange, number, 0, channel);
}

void OnAppleMidiReceiveAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
  screen_info(midi::AfterTouchChannel, pressure, 0, channel);
}

void OnAppleMidiReceivePitchBend(byte channel, int bend)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
  screen_info(midi::PitchBend, bend, 0, channel);
}

void OnAppleMidiReceiveSystemExclusive(byte* array, unsigned int size)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSysEx(size, array);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, array);
  BLESendSystemExclusive(array, size);
  ipMIDISendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
  MTC.decodeMTCFullFrame(size, array);
}

void OnAppleMidiReceiveTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTimeCodeQuarterFrame(data);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(data);
  BLESendTimeCodeQuarterFrame(data);
  ipMIDISendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
  MTC.decodMTCQuarterFrame(data);
}

void OnAppleMidiReceiveSongPosition(unsigned int beats)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongPosition(beats);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
  BLESendSongPosition(beats);
  ipMIDISendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnAppleMidiReceiveSongSelect(byte songnumber)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongSelect(songnumber);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(songnumber);
  BLESendSongSelect(songnumber);
  ipMIDISendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnAppleMidiReceiveTuneRequest(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTuneRequest();
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTuneRequest();
  BLESendTuneRequest();
  ipMIDISendTuneRequest();
  OSCSendTuneRequest();
}

void OnAppleMidiReceiveClock(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Clock);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
  BLESendClock();
  ipMIDISendClock();
  OSCSendClock();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) bpm = MTC.tapTempo();
}

void OnAppleMidiReceiveStart(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Start);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
  BLESendStart();
  ipMIDISendStart();
  OSCSendStart();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendPlay();
}

void OnAppleMidiReceiveContinue(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Continue);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
  BLESendContinue();
  ipMIDISendContinue();
  OSCSendContinue();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendContinue();
}

void OnAppleMidiReceiveStop(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Stop);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
  BLESendStop();
  ipMIDISendStop();
  OSCSendStop();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendStop();
}

void OnAppleMidiReceiveActiveSensing(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::ActiveSensing);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
  BLESendActiveSensing();
  ipMIDISendActiveSensing();
  OSCSendActiveSensing();
}

void OnAppleMidiReceiveReset(void)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::SystemReset);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
  BLESendSystemReset();
  ipMIDISendSystemReset();
  OSCSendSystemReset();
}

void apple_midi_start()
{
  if (!wifiEnabled) return;

  AppleRTP_MIDI.setHandleConnected(OnAppleMidiConnected);
  AppleRTP_MIDI.setHandleDisconnected(OnAppleMidiDisconnected);

  // Connect the handle function called upon reception of a MIDI message from AppleMIDI interface
  RTP_MIDI.setHandleNoteOn(OnAppleMidiNoteOn);
  RTP_MIDI.setHandleNoteOff(OnAppleMidiNoteOff);
  RTP_MIDI.setHandleAfterTouchPoly(OnAppleMidiReceiveAfterTouchPoly);
  RTP_MIDI.setHandleControlChange(OnAppleMidiReceiveControlChange);
  RTP_MIDI.setHandleProgramChange(OnAppleMidiReceiveProgramChange);
  RTP_MIDI.setHandleAfterTouchChannel(OnAppleMidiReceiveAfterTouchChannel);
  RTP_MIDI.setHandlePitchBend(OnAppleMidiReceivePitchBend);
  RTP_MIDI.setHandleSystemExclusive(OnAppleMidiReceiveSystemExclusive);
  RTP_MIDI.setHandleTimeCodeQuarterFrame(OnAppleMidiReceiveTimeCodeQuarterFrame);
  RTP_MIDI.setHandleSongPosition(OnAppleMidiReceiveSongPosition);
  RTP_MIDI.setHandleSongSelect(OnAppleMidiReceiveSongSelect);
  RTP_MIDI.setHandleTuneRequest(OnAppleMidiReceiveTuneRequest);
  RTP_MIDI.setHandleClock(OnAppleMidiReceiveClock);
  RTP_MIDI.setHandleStart(OnAppleMidiReceiveStart);
  RTP_MIDI.setHandleContinue(OnAppleMidiReceiveContinue);
  RTP_MIDI.setHandleStop(OnAppleMidiReceiveStop);
  RTP_MIDI.setHandleActiveSensing(OnAppleMidiReceiveActiveSensing);
  RTP_MIDI.setHandleSystemReset(OnAppleMidiReceiveReset);

  // Listen to all channels
  RTP_MIDI.begin(MIDI_CHANNEL_OMNI);
}


// Forward messages received from ipMIDI interface to other interfaces

void OnIpMidiNoteOn(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOn(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
  BLESendNoteOn(note, velocity, channel);
  AppleMidiSendNoteOn(note, velocity, channel);
  OSCSendNoteOn(note, velocity, channel);
  leds_update(midi::NoteOn, channel, note, velocity);
  screen_info(midi::NoteOn, note, velocity, channel);
}

void OnIpMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  AppleMidiSendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
  screen_info(midi::NoteOff, note, velocity, channel);
}

void OnIpMidiReceiveAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  AppleMidiSendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
  screen_info(midi::AfterTouchPoly, note, pressure, channel);
}

void OnIpMidiReceiveControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  AppleMidiSendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
  leds_update(midi::ControlChange, channel, number, value);
  screen_info(midi::ControlChange, number, value, channel);
}

void OnIpMidiReceiveProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
  leds_update(midi::ProgramChange, channel, number, 0);
  screen_info(midi::ProgramChange, number, 0, channel);
}

void OnIpMidiReceiveAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  AppleMidiSendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
  screen_info(midi::AfterTouchChannel, pressure, 0, channel);
}

void OnIpMidiReceivePitchBend(byte channel, int bend)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  AppleMidiSendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
  screen_info(midi::PitchBend, bend, 0, channel);
}

void OnIpMidiReceiveSystemExclusive(byte* array, unsigned int size)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSysEx(size, array);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, array);
  BLESendSystemExclusive(array, size);
  AppleMidiSendSystemExclusive(array, size);
  OSCSendSystemExclusive(array, size);
  MTC.decodeMTCFullFrame(size, array);
}

void OnIpMidiReceiveTimeCodeQuarterFrame(byte data)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTimeCodeQuarterFrame(data);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(data);
  BLESendTimeCodeQuarterFrame(data);
  AppleMidiSendTimeCodeQuarterFrame(data);
  OSCSendTimeCodeQuarterFrame(data);
  MTC.decodMTCQuarterFrame(data);
}

void OnIpMidiReceiveSongPosition(unsigned int beats)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongPosition(beats);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
  BLESendSongPosition(beats);
  AppleMidiSendSongPosition(beats);
  OSCSendSongPosition(beats);
}

void OnIpMidiReceiveSongSelect(byte songnumber)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongSelect(songnumber);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(songnumber);
  BLESendSongSelect(songnumber);
  AppleMidiSendSongSelect(songnumber);
  OSCSendSongSelect(songnumber);
}

void OnIpMidiReceiveTuneRequest(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTuneRequest();
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTuneRequest();
  BLESendTuneRequest();
  AppleMidiSendTuneRequest();
  OSCSendTuneRequest();
}

void OnIpMidiReceiveClock(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Clock);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
  BLESendClock();
  AppleMidiSendClock();
  OSCSendClock();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) bpm = MTC.tapTempo();
}

void OnIpMidiReceiveStart(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Start);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
  BLESendStart();
  AppleMidiSendStart();
  OSCSendStart();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendPlay();
}

void OnIpMidiReceiveContinue(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Continue);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
  BLESendContinue();
  AppleMidiSendContinue();
  OSCSendContinue();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendContinue();
}

void OnIpMidiReceiveStop(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Stop);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
  BLESendStop();
  AppleMidiSendStop();
  OSCSendStop();
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendStop();
}

void OnIpMidiReceiveActiveSensing(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::ActiveSensing);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
  BLESendActiveSensing();
  AppleMidiSendActiveSensing();
  OSCSendActiveSensing();
}

void OnIpMidiReceiveReset(void)
{
  if (!interfaces[PED_IPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::SystemReset);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
  BLESendSystemReset();
  AppleMidiSendSystemReset();
  OSCSendSystemReset();
}

void ip_midi_start()
{
  if (!wifiEnabled) return;

  // Connect the handle function called upon reception of a MIDI message from ipMIDI interface
  IP_MIDI.setHandleNoteOn(OnIpMidiNoteOn);
  IP_MIDI.setHandleNoteOff(OnIpMidiNoteOff);
  IP_MIDI.setHandleAfterTouchPoly(OnIpMidiReceiveAfterTouchPoly);
  IP_MIDI.setHandleControlChange(OnIpMidiReceiveControlChange);
  IP_MIDI.setHandleProgramChange(OnIpMidiReceiveProgramChange);
  IP_MIDI.setHandleAfterTouchChannel(OnIpMidiReceiveAfterTouchChannel);
  IP_MIDI.setHandlePitchBend(OnIpMidiReceivePitchBend);
  IP_MIDI.setHandleSystemExclusive(OnIpMidiReceiveSystemExclusive);
  IP_MIDI.setHandleTimeCodeQuarterFrame(OnIpMidiReceiveTimeCodeQuarterFrame);
  IP_MIDI.setHandleSongPosition(OnIpMidiReceiveSongPosition);
  IP_MIDI.setHandleSongSelect(OnIpMidiReceiveSongSelect);
  IP_MIDI.setHandleTuneRequest(OnIpMidiReceiveTuneRequest);
  IP_MIDI.setHandleClock(OnIpMidiReceiveClock);
  IP_MIDI.setHandleStart(OnIpMidiReceiveStart);
  IP_MIDI.setHandleContinue(OnIpMidiReceiveContinue);
  IP_MIDI.setHandleStop(OnIpMidiReceiveStop);
  IP_MIDI.setHandleActiveSensing(OnIpMidiReceiveActiveSensing);
  IP_MIDI.setHandleSystemReset(OnIpMidiReceiveReset);

  // Listen to all channels
  IP_MIDI.begin(MIDI_CHANNEL_OMNI);
}


// Forward messages received from WiFI OSC interface to serial MIDI interface

void OnOscNoteOn(OSCMessage &msg)
{
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(msg.getInt(1), msg.getInt(2), msg.getInt(0));
}

void OnOscNoteOff(OSCMessage &msg)
{
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(msg.getInt(1), msg.getInt(2), msg.getInt(0));
}

void OnOscControlChange(OSCMessage &msg)
{
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(msg.getInt(1), msg.getInt(2), msg.getInt(0));
}

#define OSC_CONTROLLER_PORT   8080
IPAddress oscControllerIP;

void OscSendBank()
{
  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg;
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  AsyncUDPMessage udpMsg4;
  OSCMessage      oscMsg("profile");
  OSCMessage      oscMsg1("bank");
  OSCMessage      oscMsg2("bank-1");
  OSCMessage      oscMsg3("bank-2");
  OSCMessage      oscMsg4("pedal_name");

  oscMsg.add(currentProfile).send(udpMsg).empty();
  udpOut.sendTo(udpMsg, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg1.add(currentBank + 1).send(udpMsg1).empty();
  udpOut.sendTo(udpMsg1, oscControllerIP, OSC_CONTROLLER_PORT);

  if (currentBank < 5) {
    oscMsg2.add(currentBank + 1).send(udpMsg2).empty();
    udpOut.sendTo(udpMsg2, oscControllerIP, OSC_CONTROLLER_PORT);
  }
  else {
    oscMsg2.add(0).send(udpMsg2).empty();
    udpOut.sendTo(udpMsg2, oscControllerIP, OSC_CONTROLLER_PORT);
  }
  if (currentBank > 4) {
    oscMsg3.add(currentBank - 4).send(udpMsg3).empty();
    udpOut.sendTo(udpMsg3, oscControllerIP, OSC_CONTROLLER_PORT);
  }
  else {
    oscMsg3.add(0).send(udpMsg3).empty();
    udpOut.sendTo(udpMsg3, oscControllerIP, OSC_CONTROLLER_PORT);
  }

  oscMsg4.add(banks[currentBank][currentPedal].pedalName).send(udpMsg4).empty();
  udpOut.sendTo(udpMsg4, oscControllerIP, OSC_CONTROLLER_PORT);

}

void OscSendPedal()
{
  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg;
  AsyncUDPMessage udpMsg0;
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  AsyncUDPMessage udpMsg4;
  AsyncUDPMessage udpMsg5;
  AsyncUDPMessage udpMsg6;
  AsyncUDPMessage udpMsg7;
  AsyncUDPMessage udpMsg8;
  AsyncUDPMessage udpMsg9;
  AsyncUDPMessage udpMsg10;
  OSCMessage      oscMsg("profile");
  OSCMessage      oscMsg0("pedal");
  OSCMessage      oscMsg1("pedal_mode");
  OSCMessage      oscMsg2("pedal_function");
  OSCMessage      oscMsg3("auto_sensing");
  OSCMessage      oscMsg4("single_press");
  OSCMessage      oscMsg5("double_press");
  OSCMessage      oscMsg6("long_press");
  OSCMessage      oscMsg7("invert_polarity");
  OSCMessage      oscMsg8("analog_map");
  OSCMessage      oscMsg9("analog_min");
  OSCMessage      oscMsg10("analog_max");

  oscMsg.add(currentProfile).send(udpMsg).empty();
  udpOut.sendTo(udpMsg, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg0.add(currentPedal).send(udpMsg0).empty();
  udpOut.sendTo(udpMsg0, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg1.add(pedals[currentPedal].mode - 2).send(udpMsg1).empty();
  udpOut.sendTo(udpMsg1, oscControllerIP, OSC_CONTROLLER_PORT);
  //oscMsg2.add(pedals[currentPedal].function).send(udpMsg2).empty();
  //udpOut.sendTo(udpMsg2, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg3.add(pedals[currentPedal].autoSensing).send(udpMsg3).empty();
  udpOut.sendTo(udpMsg3, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg4.add((pedals[currentPedal].pressMode & PED_PRESS_1) == PED_PRESS_1 ? 1 : 0).send(udpMsg4).empty();
  udpOut.sendTo(udpMsg4, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg5.add((pedals[currentPedal].pressMode & PED_PRESS_2) == PED_PRESS_2 ? 1 : 0).send(udpMsg5).empty();
  udpOut.sendTo(udpMsg5, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg6.add((pedals[currentPedal].pressMode & PED_PRESS_L) == PED_PRESS_L ? 1 : 0).send(udpMsg6).empty();
  udpOut.sendTo(udpMsg6, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg7.add(pedals[currentPedal].invertPolarity).send(udpMsg7).empty();
  udpOut.sendTo(udpMsg7, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg8.add(pedals[currentPedal].mapFunction).send(udpMsg8).empty();
  udpOut.sendTo(udpMsg8, oscControllerIP, OSC_CONTROLLER_PORT);
  switch (pedals[currentPedal].function) {
    case PED_BANK_PLUS:
    case PED_BANK_MINUS:
    case PED_BANK_PLUS_2:
    case PED_BANK_MINUS_2:
    case PED_BANK_PLUS_3:
    case PED_BANK_MINUS_3:
      oscMsg9.add((float)pedals[currentPedal].expZero / (BANKS - 1)).send(udpMsg9).empty();
      oscMsg10.add((float)pedals[currentPedal].expMax / (BANKS - 1)).send(udpMsg10).empty();
      break;
    case PED_BPM_PLUS:
    case PED_BPM_MINUS:
      oscMsg9.add((float)(pedals[currentPedal].expZero - 40) / 260).send(udpMsg9).empty();
      oscMsg10.add((float)(pedals[currentPedal].expMax - 40)/ 260).send(udpMsg10).empty();
      break;
    default:
      oscMsg9.add((float)pedals[currentPedal].expZero / (ADC_RESOLUTION - 1)).send(udpMsg9).empty();
      oscMsg10.add((float)pedals[currentPedal].expMax / (ADC_RESOLUTION - 1)).send(udpMsg10).empty();
      break;
  }
  udpOut.sendTo(udpMsg9, oscControllerIP, OSC_CONTROLLER_PORT);
  udpOut.sendTo(udpMsg10, oscControllerIP, OSC_CONTROLLER_PORT);
}

void OscSendLive()
{
  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg;
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  AsyncUDPMessage udpMsg4;
  AsyncUDPMessage udpMsg5;
  OSCMessage      oscMsg("profile");
  OSCMessage      oscMsg1("clock");
  OSCMessage      oscMsg2("masterslave");
  OSCMessage      oscMsg3("bpm");
  OSCMessage      oscMsg4("play");
  OSCMessage      oscMsg5("beats_per_minute");

  oscMsg.add(currentProfile).send(udpMsg).empty();
  udpOut.sendTo(udpMsg, oscControllerIP, OSC_CONTROLLER_PORT);

  switch (MTC.getMode()) {
    case MidiTimeCode::SynchroNone:
      oscMsg1.add(0).send(udpMsg1).empty();
      oscMsg2.add(0).send(udpMsg2).empty();
      break;
    case MidiTimeCode::SynchroClockMaster:
      oscMsg1.add(1).send(udpMsg1).empty();
      oscMsg2.add(0).send(udpMsg2).empty();
      break;
    case MidiTimeCode::SynchroClockSlave:
      oscMsg1.add(1).send(udpMsg1).empty();
      oscMsg2.add(1).send(udpMsg2).empty();
      break;
    case MidiTimeCode::SynchroMTCMaster:
      oscMsg1.add(2).send(udpMsg1).empty();
      oscMsg2.add(0).send(udpMsg2).empty();
      break;
    case MidiTimeCode::SynchroMTCSlave:
      oscMsg1.add(2).send(udpMsg1).empty();
      oscMsg2.add(1).send(udpMsg2).empty();
      break;
  }
  udpOut.sendTo(udpMsg1, oscControllerIP, OSC_CONTROLLER_PORT);
  udpOut.sendTo(udpMsg2, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg3.add((float)((bpm - 40) / 260.0)).send(udpMsg3).empty();
  udpOut.sendTo(udpMsg3, oscControllerIP, OSC_CONTROLLER_PORT);
  oscMsg4.add(0).send(udpMsg4).empty();
  udpOut.sendTo(udpMsg4, oscControllerIP, OSC_CONTROLLER_PORT);
  char beats_per_minute[4] = {0, 0, 0, 0};
  sprintf(beats_per_minute, "%3d", bpm);
  oscMsg5.add(bpm).send(udpMsg5).empty();
  udpOut.sendTo(udpMsg5, oscControllerIP, OSC_CONTROLLER_PORT);
}


void OscSendInterface()
{
  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg;
  AsyncUDPMessage udpMsg0;
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  AsyncUDPMessage udpMsg4;
  OSCMessage      oscMsg("profile");
  OSCMessage      oscMsg0("interface");
  OSCMessage      oscMsg1("midi_in");
  OSCMessage      oscMsg2("midi_out");
  OSCMessage      oscMsg3("midi_thru");
  OSCMessage      oscMsg4("midi_clock");

  oscMsg.add(currentProfile).send(udpMsg).empty();
  udpOut.sendTo(udpMsg, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg0.add(currentInterface).send(udpMsg0).empty();
  udpOut.sendTo(udpMsg0, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg1.add(interfaces[currentInterface].midiIn).send(udpMsg1).empty();
  udpOut.sendTo(udpMsg1, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg1.add(interfaces[currentInterface].midiIn).send(udpMsg1).empty();
  udpOut.sendTo(udpMsg1, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg2.add(interfaces[currentInterface].midiOut).send(udpMsg2).empty();
  udpOut.sendTo(udpMsg2, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg3.add(interfaces[currentInterface].midiThru).send(udpMsg3).empty();
  udpOut.sendTo(udpMsg3, oscControllerIP, OSC_CONTROLLER_PORT);

  oscMsg4.add(interfaces[currentInterface].midiClock).send(udpMsg4).empty();
  udpOut.sendTo(udpMsg4, oscControllerIP, OSC_CONTROLLER_PORT);
}

void OscControllerUpdate()
{
  if (wifiEnabled) {
    OscSendBank();
    OscSendPedal();
    OscSendInterface();
    OscSendLive();
  }
}

void OnOscPedalName(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][currentPedal].pedalName, MAXACTIONNAME) > 0) {
    DPRINT("OSC message /pedal_name %s received from %s\n", banks[currentBank][currentPedal].pedalName, oscControllerIP.toString().c_str());
  } else {
    DPRINT("OSC error: %d\n", msg.getError());
  }
}



void OnOscLed1(OSCMessage &msg)
{
  DPRINT("OSC message /led_1 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  leds.set(0, msg.getInt(0));
  leds.write();
}

void OnOscLed2(OSCMessage &msg)
{
  DPRINT("OSC message /led_2 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  leds.set(1, msg.getInt(0));
  leds.write();
}

void OnOscLed3(OSCMessage &msg)
{
  DPRINT("OSC message /led_3 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  leds.set(2, msg.getInt(0));
  leds.write();
}

void OnOscSave(OSCMessage &msg)
{
  DPRINT("OSC message /save received from %s\n", oscControllerIP.toString().c_str());
  eeprom_update_current_profile(currentProfile);
  eeprom_update_profile();
}

void OnOscProfile(OSCMessage &msg)
{
  DPRINT("OSC message /profile %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  currentProfile = constrain(msg.getInt(0), 0, PROFILES - 1);
  reloadProfile = true;
}

void OnOscPedal(OSCMessage &msg)
{
  DPRINT("OSC message /pedal %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  currentPedal = constrain(msg.getInt(0), 0, PEDALS - 1);
  OscSendBank();
  OscSendPedal();
}

void OnOscPedalMode(OSCMessage &msg)
{
  DPRINT("OSC message /pedal_mode %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  pedals[currentPedal].mode = msg.getInt(0) + 2;

}

void OnOscPedalFunction(OSCMessage &msg)
{
  DPRINT("OSC message /pedal_function %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  pedals[currentPedal].function = msg.getInt(0);

}

void OnOscPedalAutoSensing(OSCMessage &msg)
{
  DPRINT("OSC message /auto_sensing %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  pedals[currentPedal].autoSensing = constrain(msg.getInt(0), 0, 1);

}

void OnOscPedalSinglePress(OSCMessage &msg)
{
  DPRINT("OSC message /single_press %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  switch (msg.getInt(0)) {
    case 0:
      pedals[currentPedal].pressMode &= ~PED_PRESS_1;
      break;
    case 1:
      pedals[currentPedal].pressMode |= PED_PRESS_1;
      break;
  }
}

void OnOscPedalDoublePress(OSCMessage &msg)
{
  DPRINT("OSC message /double_press %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  switch (msg.getInt(0)) {
    case 0:
      pedals[currentPedal].pressMode &= ~PED_PRESS_2;
      break;
    case 1:
      pedals[currentPedal].pressMode |= PED_PRESS_2;
      break;
  }
}

void OnOscPedalLongPress(OSCMessage &msg)
{
  DPRINT("OSC message /long_press %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  switch (msg.getInt(0)) {
    case 0:
      pedals[currentPedal].pressMode &= ~PED_PRESS_L;
      break;
    case 1:
      pedals[currentPedal].pressMode |= PED_PRESS_L;
      break;
  }
}

void OnOscPedalInvertPolarity(OSCMessage &msg)
{
  DPRINT("OSC message /invert_polarity %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  pedals[currentPedal].invertPolarity = constrain(msg.getInt(0), 0, 1);
}

void OnOscPedalAnalogMap(OSCMessage &msg)
{
  DPRINT("OSC message /analog_map %f received from %s\n", msg.getFloat(0), oscControllerIP.toString().c_str());
  pedals[currentPedal].mapFunction = constrain(msg.getInt(0), 0, 2);
}

void OnOscPedalAnalogMin(OSCMessage &msg)
{
  DPRINT("OSC message /analog_min %f received from %s\n", msg.getFloat(0), oscControllerIP.toString().c_str());
  switch (pedals[currentPedal].function) {
    case PED_BANK_PLUS:
    case PED_BANK_MINUS:
    case PED_BANK_PLUS_2:
    case PED_BANK_MINUS_2:
    case PED_BANK_PLUS_3:
    case PED_BANK_MINUS_3:
      pedals[currentPedal].expZero = constrain(msg.getFloat(0) * (PEDALS - 1), 0, PEDALS);
      break;
    case PED_BPM_PLUS:
    case PED_BPM_MINUS:
      pedals[currentPedal].expZero = constrain(40 + msg.getFloat(0) * 260, 40, 300);
      break;
    default:
      pedals[currentPedal].expZero = constrain(msg.getFloat(0) * (ADC_RESOLUTION - 1), 0, ADC_RESOLUTION - 1);
      break;
  }
  if (pedals[currentPedal].expZero > pedals[currentPedal].expMax)
    pedals[currentPedal].expMax = pedals[currentPedal].expZero;
}

void OnOscPedalAnalogMax(OSCMessage &msg)
{
  DPRINT("OSC message /analog_max %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  switch (pedals[currentPedal].function) {
    case PED_BANK_PLUS:
    case PED_BANK_MINUS:
    case PED_BANK_PLUS_2:
    case PED_BANK_MINUS_2:
    case PED_BANK_PLUS_3:
    case PED_BANK_MINUS_3:
      pedals[currentPedal].expMax = constrain(msg.getFloat(0) * (PEDALS - 1), 0, PEDALS);
      break;
    case PED_BPM_PLUS:
    case PED_BPM_MINUS:
      pedals[currentPedal].expMax = constrain(40 + msg.getFloat(0) * 260, 40, 300);
      break;
    default:
      pedals[currentPedal].expMax = constrain(msg.getFloat(0) * (ADC_RESOLUTION - 1), 0, ADC_RESOLUTION - 1);
      break;
  }
  if (pedals[currentPedal].expMax < pedals[currentPedal].expMax)
    pedals[currentPedal].expZero = pedals[currentPedal].expMax;
}

void OnOscBank(OSCMessage &msg)
{
  DPRINT("OSC message /bank %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  currentBank = constrain(msg.getInt(0) - 1, 0, BANKS - 1);
  OscSendBank();
  leds_refresh();
}

void OnOscBank_1(OSCMessage &msg)
{
  DPRINT("OSC message /bank-1 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  if (msg.getInt(0) > 0) {
    currentBank = constrain(msg.getInt(0) - 1, 0, BANKS - 1);
    OscSendBank();
    leds_refresh();
  }
}

void OnOscBank_2(OSCMessage &msg)
{
  DPRINT("OSC message /bank-2 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  if (msg.getInt(0) > 0) {
    currentBank = constrain(5 + msg.getInt(0) - 1, 0, BANKS - 1);
    OscSendBank();
    leds_refresh();
  }
}

void OnOscClock(OSCMessage &msg)
{
  DPRINT("OSC message /clock %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());

  switch (msg.getInt(0)) {

    case 0:   // None
      MTC.setMode(MidiTimeCode::SynchroNone);
      currentMidiTimeCode = PED_MTC_NONE;
      break;

    case 1:   // MIDI Clock
      switch (MTC.getMode()) {
        case MidiTimeCode::SynchroNone:
        case MidiTimeCode::SynchroClockMaster:
        case MidiTimeCode::SynchroMTCMaster:
          MTC.setMode(MidiTimeCode::SynchroClockMaster);
          currentMidiTimeCode = PED_MIDI_CLOCK_MASTER;
          bpm = (bpm == 0) ? 120 : bpm;
          MTC.setBpm(bpm);
          break;
        case MidiTimeCode::SynchroClockSlave:
        case MidiTimeCode::SynchroMTCSlave:
          MTC.setMode(MidiTimeCode::SynchroClockSlave);
          currentMidiTimeCode = PED_MIDI_CLOCK_SLAVE;
          bpm = 0;
          break;
      }
      break;

    case 2:   // MIDI Time Code
      switch (MTC.getMode()) {
        case MidiTimeCode::SynchroNone:
        case MidiTimeCode::SynchroClockMaster:
        case MidiTimeCode::SynchroMTCMaster:
          MTC.setMode(MidiTimeCode::SynchroMTCMaster);
          currentMidiTimeCode = PED_MTC_MASTER_24;
          MTC.sendPosition(0, 0, 0, 0);
          break;
        case MidiTimeCode::SynchroClockSlave:
        case MidiTimeCode::SynchroMTCSlave:
          MTC.setMode(MidiTimeCode::SynchroMTCSlave);
          currentMidiTimeCode = PED_MTC_SLAVE;
          break;
      }
      break;
  }
}

void OnOscMasterSlave(OSCMessage &msg)
{
  DPRINT("OSC message /masterslave %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());

  switch (msg.getInt(0)) {

    case 0:   // MASTER
      switch (MTC.getMode()) {
        case MidiTimeCode::SynchroClockMaster:
        case MidiTimeCode::SynchroMTCMaster:
          break;
        case MidiTimeCode::SynchroClockSlave:
          MTC.setMode(MidiTimeCode::SynchroClockMaster);
          currentMidiTimeCode = PED_MIDI_CLOCK_MASTER;
          bpm = (bpm == 0) ? 120 : bpm;
          MTC.setBpm(bpm);
          break;
        case MidiTimeCode::SynchroMTCSlave:
          MTC.setMode(MidiTimeCode::SynchroMTCMaster);
          currentMidiTimeCode = PED_MTC_MASTER_24;
          MTC.sendPosition(0, 0, 0, 0);
          break;
        default:
          break;
      }
      break;

    case 1:   // SLAVE
      switch (MTC.getMode()) {
        case MidiTimeCode::SynchroClockMaster:
          MTC.setMode(MidiTimeCode::SynchroClockSlave);
          currentMidiTimeCode = PED_MIDI_CLOCK_SLAVE;
          bpm = 0;
          break;
        case MidiTimeCode::SynchroMTCMaster:
          MTC.setMode(MidiTimeCode::SynchroMTCSlave);
          currentMidiTimeCode = PED_MTC_SLAVE;
          break;
        default:
          break;
      }
      break;
  }
}

void OnOscBPM(OSCMessage &msg)
{
  DPRINT("OSC message /bpm %f received from %s\n", msg.getFloat(0), oscControllerIP.toString().c_str());
  bpm = 40 + 260 * msg.getFloat(0);
  MTC.setBpm(bpm);
  OscSendLive();
}

void OnOscPlay(OSCMessage &msg)
{
  DPRINT("OSC message /play %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());

  switch (msg.getInt(0)) {
    case 0:
      mtc_stop();
      break;

    case 1:
      mtc_start();
      break;

    case 2:
      mtc_continue();
      break;

    case 3:
      mtc_stop();
      break;
  }
}

void OnOscInterface(OSCMessage &msg)
{
  DPRINT("OSC message /interface %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  currentInterface = constrain(msg.getInt(0), 0, INTERFACES);
  OscSendInterface();
}

void OnOscMidiIn(OSCMessage &msg)
{
  DPRINT("OSC message /midi_in %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  interfaces[currentInterface].midiIn = msg.getInt(0);
}

void OnOscMidiOut(OSCMessage &msg)
{
  DPRINT("OSC message /midi_out %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  interfaces[currentInterface].midiOut = msg.getInt(0);
}

void OnOscMidiThru(OSCMessage &msg)
{
  DPRINT("OSC message /midi_thru %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  interfaces[currentInterface].midiThru = msg.getInt(0);
}

void OnOscMidiClock(OSCMessage &msg)
{
  DPRINT("OSC message /midi_clock %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  interfaces[currentInterface].midiClock = msg.getInt(0);
}


// Listen to incoming OSC messages from WiFi

void oscOnPacket(AsyncUDPPacket packet) {

  if (!wifiEnabled) return;
  if (!WiFi.isConnected()) return;

  if (!interfaces[PED_OSC].midiIn) return;

  OSCMessage oscMsg;

  while (packet.available() > 0) {
    oscMsg.fill(packet.read());
  }
  oscControllerIP = packet.remoteIP();
  //oscControllerIP = IPAddress(192,168,2,120);

  if (!oscMsg.hasError()) {
    oscMsg.dispatch("/profile",                     OnOscProfile);
    oscMsg.dispatch("/save",                        OnOscSave);
    oscMsg.dispatch("/led_1",                       OnOscLed1);
    oscMsg.dispatch("/led_2",                       OnOscLed2);
    oscMsg.dispatch("/led_3",                       OnOscLed3);
    oscMsg.dispatch("/bank",                        OnOscBank);
    oscMsg.dispatch("/bank-1",                      OnOscBank_1);
    oscMsg.dispatch("/bank-2",                      OnOscBank_2);
    oscMsg.dispatch("/pedal_name",                  OnOscPedalName);

    oscMsg.dispatch("/pedal",                       OnOscPedal);
    oscMsg.dispatch("/pedal_mode",                  OnOscPedalMode);
    oscMsg.dispatch("/pedal_function",              OnOscPedalFunction);
    oscMsg.dispatch("/auto_sensing",                OnOscPedalAutoSensing);
    oscMsg.dispatch("/single_press",                OnOscPedalSinglePress);
    oscMsg.dispatch("/double_press",                OnOscPedalDoublePress);
    oscMsg.dispatch("/long_press",                  OnOscPedalLongPress);
    oscMsg.dispatch("/invert_polarity",             OnOscPedalInvertPolarity);
    oscMsg.dispatch("/analog_map",                  OnOscPedalAnalogMap);
    oscMsg.dispatch("/analog_min",                  OnOscPedalAnalogMin);
    oscMsg.dispatch("/analog_max",                  OnOscPedalAnalogMax);

    oscMsg.dispatch("/clock",                       OnOscClock);
    oscMsg.dispatch("/masterslave",                 OnOscMasterSlave);
    oscMsg.dispatch("/bpm",                         OnOscBPM);
    oscMsg.dispatch("/play",                        OnOscPlay);
    oscMsg.dispatch("/interface",                   OnOscInterface);
    oscMsg.dispatch("/midi_in",                     OnOscMidiIn);
    oscMsg.dispatch("/midi_out",                    OnOscMidiOut);
    oscMsg.dispatch("/midi_thru",                   OnOscMidiThru);
    oscMsg.dispatch("/midi_clock",                  OnOscMidiClock);

    oscMsg.dispatch("/pedalino/midi/noteOn",        OnOscNoteOn);
    oscMsg.dispatch("/pedalino/midi/noteOff",       OnOscNoteOff);
    oscMsg.dispatch("/pedalino/midi/controlChange", OnOscControlChange);
  } else {
    DPRINTLN("OSC error: %d", oscMsg.getError());
  }
}

#endif  // NOWIFI