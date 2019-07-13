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

#ifdef WIFI
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <AppleMidi.h>
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

    case 0xf0:
      switch (status) {

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
      }
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
#define rtpMIDI_listen(...)
#define ipMIDI_listen(...)
#define oscUDP_listen(...)
#else

// Forward messages received from WiFI MIDI interface to serial MIDI interface

void OnAppleMidiConnected(uint32_t ssrc, char* name)
{
  appleMidiConnected  = true;
  DPRINTLN("AppleMIDI Connected Session ID: %u Name: %s", ssrc, name);
}

void OnAppleMidiDisconnected(uint32_t ssrc)
{
  appleMidiConnected  = false;
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
}

void OnAppleMidiNoteOff(byte channel, byte note, byte velocity)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
  BLESendNoteOff(note, velocity, channel);
  ipMIDISendNoteOff(note, velocity, channel);
  OSCSendNoteOff(note, velocity, channel);
}

void OnAppleMidiReceiveAfterTouchPoly(byte channel, byte note, byte pressure)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
  BLESendAfterTouchPoly(note, pressure, channel);
  ipMIDISendAfterTouchPoly(note, pressure, channel);
  OSCSendAfterTouchPoly(note, pressure, channel);
}

void OnAppleMidiReceiveControlChange(byte channel, byte number, byte value)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
  BLESendControlChange(number, value, channel);
  ipMIDISendControlChange(number, value, channel);
  OSCSendControlChange(number, value, channel);
}

void OnAppleMidiReceiveProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
}

void OnAppleMidiReceiveAfterTouchChannel(byte channel, byte pressure)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
  BLESendAfterTouch(pressure, channel);
  ipMIDISendAfterTouch(pressure, channel);
  OSCSendAfterTouch(pressure, channel);
}

void OnAppleMidiReceivePitchBend(byte channel, int bend)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
  BLESendPitchBend(bend, channel);
  ipMIDISendPitchBend(bend, channel);
  OSCSendPitchBend(bend, channel);
}

void OnAppleMidiReceiveSysEx(const byte * data, uint16_t size)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSysEx(size, data);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSysEx(size, data);
  BLESendSystemExclusive(data, size);
  ipMIDISendSystemExclusive(data, size);
  OSCSendSystemExclusive(data, size);
  MTC.decodeMTCFullFrame(size, data);
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

void OnAppleMidiReceiveSongPosition(unsigned short beats)
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

  // Create a session and wait for a remote host to connect to us
  AppleMIDI.begin("Pedalino(TM)");

  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  // Connect the handle function called upon reception of a MIDI message from WiFi MIDI interface
  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);
  AppleMIDI.OnReceiveAfterTouchPoly(OnAppleMidiReceiveAfterTouchPoly);
  AppleMIDI.OnReceiveControlChange(OnAppleMidiReceiveControlChange);
  AppleMIDI.OnReceiveProgramChange(OnAppleMidiReceiveProgramChange);
  AppleMIDI.OnReceiveAfterTouchChannel(OnAppleMidiReceiveAfterTouchChannel);
  AppleMIDI.OnReceivePitchBend(OnAppleMidiReceivePitchBend);
  AppleMIDI.OnReceiveSysEx(OnAppleMidiReceiveSysEx);
  AppleMIDI.OnReceiveTimeCodeQuarterFrame(OnAppleMidiReceiveTimeCodeQuarterFrame);
  AppleMIDI.OnReceiveSongPosition(OnAppleMidiReceiveSongPosition);
  AppleMIDI.OnReceiveSongSelect(OnAppleMidiReceiveSongSelect);
  AppleMIDI.OnReceiveTuneRequest(OnAppleMidiReceiveTuneRequest);
  AppleMIDI.OnReceiveClock(OnAppleMidiReceiveClock);
  AppleMIDI.OnReceiveStart(OnAppleMidiReceiveStart);
  AppleMIDI.OnReceiveContinue(OnAppleMidiReceiveContinue);
  AppleMIDI.OnReceiveStop(OnAppleMidiReceiveStop);
  AppleMIDI.OnReceiveActiveSensing(OnAppleMidiReceiveActiveSensing);
  AppleMIDI.OnReceiveReset(OnAppleMidiReceiveReset);
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

// Listen to incoming OSC messages from WiFi

void oscUDP_listen() {

  if (!wifiEnabled) return;
  if (!WiFi.isConnected()) return;

  if (!interfaces[PED_OSC].midiIn) return;

  int size = oscUDP.parsePacket();

  if (size > 0) {
    while (size--) oscMsg.fill(oscUDP.read());
    if (!oscMsg.hasError()) {
      oscMsg.dispatch(" / pedalino / midi / noteOn",        OnOscNoteOn);
      oscMsg.dispatch(" / pedalino / midi / noteOff",       OnOscNoteOff);
      oscMsg.dispatch(" / pedalino / midi / controlChange", OnOscControlChange);
    } else {
      DPRINTLN("OSC error: %d", oscMsg.getError());
    }
  }
}

// Listen to incoming AppleMIDI messages from WiFi

inline void rtpMIDI_listen() {

  if (!wifiEnabled) return;
  if (!WiFi.isConnected()) return;

  AppleMIDI.run();
}

// Listen to incoming ipMIDI messages from WiFi

void ipMIDI_listen() {

  byte status, type, channel;
  byte data[2];
  byte note, velocity, pressure, number, value;
  int  bend;
  unsigned int beats;

  if (!wifiEnabled) return;
  if (!WiFi.isConnected()) return;

  if (!interfaces[PED_IPMIDI].midiIn) return;
  
  ipMIDI.parsePacket();

  while (ipMIDI.available() > 0) {

    ipMIDI.read(&status, 1);
    type    = DIN_MIDI.getTypeFromStatusByte(status);
    channel = DIN_MIDI.getChannelFromStatusByte(status);

    switch (type) {

      case midi::NoteOff:
        ipMIDI.read(data, 2);
        note     = data[0];
        velocity = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
        BLESendNoteOff(note, velocity, channel);
        AppleMidiSendNoteOff(note, velocity, channel);
        OSCSendNoteOff(note, velocity, channel);
        break;

      case midi::NoteOn:
        ipMIDI.read(data, 2);
        note     = data[0];
        velocity = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOn(note, velocity, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
        BLESendNoteOn(note, velocity, channel);
        AppleMidiSendNoteOn(note, velocity, channel);
        OSCSendNoteOn(note, velocity, channel);
        break;

      case midi::AfterTouchPoly:
        ipMIDI.read(data, 2);
        note     = data[0];
        pressure = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
        BLESendAfterTouchPoly(note, pressure, channel);
        AppleMidiSendAfterTouchPoly(note, pressure, channel);
        OSCSendAfterTouchPoly(note, pressure, channel);
        break;

      case midi::ControlChange:
        ipMIDI.read(data, 2);
        number  = data[0];
        value   = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
        BLESendControlChange(number, value, channel);
        AppleMidiSendControlChange(number, value, channel);
        OSCSendControlChange(number, value, channel);
        break;

      case midi::ProgramChange:
        ipMIDI.read(data, 1);
        number  = data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
        BLESendProgramChange(number, channel);
        AppleMidiSendProgramChange(number, channel);
        OSCSendProgramChange(number, channel);
        break;

      case midi::AfterTouchChannel:
        ipMIDI.read(data, 1);
        pressure = data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
        BLESendAfterTouch(pressure, channel);
        AppleMidiSendAfterTouch(pressure, channel);
        OSCSendAfterTouch(pressure, channel);
        break;

      case midi::PitchBend:
        ipMIDI.read(data, 2);
        bend = data[1] << 7 | data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
        BLESendPitchBend(bend, channel);
        AppleMidiSendPitchBend(bend, channel);
        OSCSendPitchBend(bend, channel);
        break;

      case 0xf0:
        switch (status) {

          case midi::SystemExclusive:
            while (ipMIDI.read(data, 1) && data[0] != 0xf7);
            break;

          case midi::TimeCodeQuarterFrame:
            ipMIDI.read(data, 1);
            value = data[0];
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTimeCodeQuarterFrame(value);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(value);
            BLESendTimeCodeQuarterFrame(value);
            AppleMidiSendTimeCodeQuarterFrame(value);
            OSCSendTimeCodeQuarterFrame(value);
            MTC.decodMTCQuarterFrame(value);
            break;

          case midi::SongPosition:
            ipMIDI.read(data, 2);
            beats = data[1] << 7 | data[0];
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongPosition(beats);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
            BLESendSongPosition(beats);
            AppleMidiSendSongPosition(beats);
            OSCSendSongPosition(beats);
            break;

          case midi::SongSelect:
            ipMIDI.read(data, 1);
            number = data[0];
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongSelect(number);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongSelect(number);
            BLESendSongSelect(number);
            AppleMidiSendSongSelect(number);
            OSCSendSongSelect(number);
            break;

          case midi::TuneRequest:
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::TuneRequest);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::TuneRequest);
            BLESendTuneRequest();
            AppleMidiSendTuneRequest();
            OSCSendTuneRequest();
            break;

          case midi::Clock:
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Clock);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Clock);
            BLESendClock();
            AppleMidiSendClock();
            OSCSendClock();
            if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) bpm = MTC.tapTempo();
            break;

          case midi::Start:
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Start);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Start);
            BLESendStart();
            AppleMidiSendStart();
            OSCSendStart();
            if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendPlay();
            break;

          case midi::Continue:
            if (interfaces[PED_USBMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Continue);
            BLESendContinue();
            AppleMidiSendContinue();
            OSCSendContinue();
            if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendContinue();
            break;

          case midi::Stop:
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::Stop);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::Stop);
            BLESendStop();
            AppleMidiSendStop();
            OSCSendStop();
            if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendStop();
            break;

          case midi::ActiveSensing:
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::ActiveSensing);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::ActiveSensing);
            BLESendActiveSensing();
            AppleMidiSendActiveSensing();
            OSCSendActiveSensing();
            break;

          case midi::SystemReset:
            if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendRealTime(midi::SystemReset);
            if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendRealTime(midi::SystemReset);
            BLESendSystemReset();
            AppleMidiSendSystemReset();
            OSCSendSystemReset();
            break;
        }
        break;

      default:
        ipMIDI.read(data, 2);
    }
    DPRINTMIDI(ipMIDI.remoteIP().toString().c_str(), status, data);
  }
}

#endif  // NOWIFI