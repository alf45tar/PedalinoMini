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
  leds_update(midi::NoteOn, channel, note, velocity);
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
  leds_update(midi::ControlChange, channel, number, value);
}

void OnAppleMidiReceiveProgramChange(byte channel, byte number)
{
  if (!interfaces[PED_RTPMIDI].midiIn) return;

  if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
  if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
  BLESendProgramChange(number, channel);
  OSCSendProgramChange(number, channel);
  leds_update(midi::ProgramChange, channel, number, 0);
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

#define OSC_CONTROLLER_PORT   8080
IPAddress oscControllerIP;

void OscSendBank()
{
  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  OSCMessage      oscMsg1("bank");
  OSCMessage      oscMsg2("bank-1");
  OSCMessage      oscMsg3("bank-2");


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
}

void OscSendPedal()
{
  AsyncUDP        udpOut;
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
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  AsyncUDPMessage udpMsg4;
  OSCMessage      oscMsg1("clock");
  OSCMessage      oscMsg2("masterslave");
  OSCMessage      oscMsg3("bpm");
  OSCMessage      oscMsg4("play");

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
}


void OscSendInterface()
{
  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg0;
  AsyncUDPMessage udpMsg1;
  AsyncUDPMessage udpMsg2;
  AsyncUDPMessage udpMsg3;
  AsyncUDPMessage udpMsg4;
  OSCMessage      oscMsg0("interface");
  OSCMessage      oscMsg1("midi_in");
  OSCMessage      oscMsg2("midi_out");
  OSCMessage      oscMsg3("midi_thru");
  OSCMessage      oscMsg4("midi_clock");

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
  OscSendBank();
  OscSendPedal();
  OscSendInterface();
  OscSendLive();
}

void OnOscSendConfiguration(OSCMessage &msg)
{
  DPRINT("OSC message /send_configuration received from %s\n", oscControllerIP.toString().c_str());

  AsyncUDP        udpOut;
  AsyncUDPMessage udpMsg;

  for (byte i = 0; i < PEDALS; i++) {
    char address[8] = "pedal_1";
    address[6] = 61 + i;
    address[7] = 0;
    OSCMessage  oscMsg(address);
    oscMsg.add(banks[currentBank][i].pedalName).send(udpMsg).empty();
    udpOut.sendTo(udpMsg, oscControllerIP, OSC_CONTROLLER_PORT);
  }
}

void OnOscPedal1(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][0].pedalName, MAXPEDALNAME) > 0) {
    DPRINT("OSC message /pedal_1 %s received from %s\n", banks[currentBank][0].pedalName, oscControllerIP.toString().c_str());
  } else {
    DPRINT("OSC error: %d\n", msg.getError());
  }
}

void OnOscPedal2(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][1].pedalName, MAXPEDALNAME) > 0) {
    DPRINT("OSC message /pedal_2 %s received from %s\n", banks[currentBank][1].pedalName, oscControllerIP.toString().c_str());
  } else {
    DPRINT("OSC error: %d\n", msg.getError());
  }
}

void OnOscPedal3(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][2].pedalName, MAXPEDALNAME) > 0) {
    DPRINT("OSC message /pedal_3 %s received from %s\n", banks[currentBank][2].pedalName, oscControllerIP.toString().c_str());
  } else {
    DPRINT("OSC error: %d\n", msg.getError());
  }
}

void OnOscPedal4(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][3].pedalName, MAXPEDALNAME) > 0) {
    DPRINT("OSC message /pedal_4 %s received from %s\n", banks[currentBank][3].pedalName, oscControllerIP.toString().c_str());
  } else {
    DPRINT("OSC error: %d\n", msg.getError());
  }
}

void OnOscPedal5(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][4].pedalName, MAXPEDALNAME) > 0) {
    DPRINT("OSC message /pedal_5 %s received from %s\n", banks[currentBank][4].pedalName, oscControllerIP.toString().c_str());
  } else {
    DPRINT("OSC error: %d\n", msg.getError());
  }
}

void OnOscPedal6(OSCMessage &msg)
{
  if (msg.getString(0, banks[currentBank][5].pedalName, MAXPEDALNAME) > 0) {
    DPRINT("OSC message /pedal_6 %s received from %s\n", banks[currentBank][5].pedalName, oscControllerIP.toString().c_str());
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
}

void OnOscBank_1(OSCMessage &msg)
{
  DPRINT("OSC message /bank-1 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  if (msg.getInt(0) > 0) {
    currentBank = constrain(msg.getInt(0) - 1, 0, BANKS - 1);
    OscSendBank();
  }
}

void OnOscBank_2(OSCMessage &msg)
{
  DPRINT("OSC message /bank-2 %d received from %s\n", msg.getInt(0), oscControllerIP.toString().c_str());
  if (msg.getInt(0) > 0) {
    currentBank = constrain(5 + msg.getInt(0) - 1, 0, BANKS - 1);
    OscSendBank();
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
      }
      break;
  }
}

void OnOscBPM(OSCMessage &msg)
{
  DPRINT("OSC message /bpm %f received from %s\n", msg.getFloat(0), oscControllerIP.toString().c_str());
  bpm = 40 + 260 * msg.getFloat(0);
  MTC.setBpm(bpm);
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
    oscMsg.dispatch("/send_configuration",          OnOscSendConfiguration);
    oscMsg.dispatch("/save",                        OnOscSave);
    oscMsg.dispatch("/pedal_1",                     OnOscPedal1);
    oscMsg.dispatch("/pedal_2",                     OnOscPedal2);
    oscMsg.dispatch("/pedal_3",                     OnOscPedal3);
    oscMsg.dispatch("/pedal_4",                     OnOscPedal4);
    oscMsg.dispatch("/pedal_5",                     OnOscPedal5);
    oscMsg.dispatch("/pedal_6",                     OnOscPedal6);
    oscMsg.dispatch("/led_1",                       OnOscLed1);
    oscMsg.dispatch("/led_2",                       OnOscLed2);
    oscMsg.dispatch("/led_3",                       OnOscLed3);
    oscMsg.dispatch("/profile",                     OnOscProfile);
    oscMsg.dispatch("/bank",                        OnOscBank);
    oscMsg.dispatch("/bank-1",                      OnOscBank_1);
    oscMsg.dispatch("/bank-2",                      OnOscBank_2);

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


// Listen to incoming ipMIDI messages from WiFi

void ipMidiOnPacket(AsyncUDPPacket packet) {

  byte status, type, channel;
  byte data[2] = {0, 0};
  byte note, velocity, pressure, number, value;
  int  bend;
  unsigned int beats;

  if (!wifiEnabled) return;
  if (!WiFi.isConnected()) return;

  if (!interfaces[PED_IPMIDI].midiIn) return;

  while (packet.available() > 0) {

    packet.read(&status, 1);
    type    = DIN_MIDI.getTypeFromStatusByte(status);
    channel = DIN_MIDI.getChannelFromStatusByte(status);

    switch (type) {

      case midi::NoteOff:
        packet.read(data, 2);
        note     = data[0];
        velocity = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOff(note, velocity, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOff(note, velocity, channel);
        BLESendNoteOff(note, velocity, channel);
        AppleMidiSendNoteOff(note, velocity, channel);
        OSCSendNoteOff(note, velocity, channel);
        break;

      case midi::NoteOn:
        packet.read(data, 2);
        note     = data[0];
        velocity = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendNoteOn(note, velocity, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendNoteOn(note, velocity, channel);
        BLESendNoteOn(note, velocity, channel);
        AppleMidiSendNoteOn(note, velocity, channel);
        OSCSendNoteOn(note, velocity, channel);
        break;

      case midi::AfterTouchPoly:
        packet.read(data, 2);
        note     = data[0];
        pressure = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(note, pressure, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(note, pressure, channel);
        BLESendAfterTouchPoly(note, pressure, channel);
        AppleMidiSendAfterTouchPoly(note, pressure, channel);
        OSCSendAfterTouchPoly(note, pressure, channel);
        break;

      case midi::ControlChange:
        packet.read(data, 2);
        number  = data[0];
        value   = data[1];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendControlChange(number, value, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendControlChange(number, value, channel);
        BLESendControlChange(number, value, channel);
        AppleMidiSendControlChange(number, value, channel);
        OSCSendControlChange(number, value, channel);
        break;

      case midi::ProgramChange:
        packet.read(data, 1);
        number  = data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendProgramChange(number, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendProgramChange(number, channel);
        BLESendProgramChange(number, channel);
        AppleMidiSendProgramChange(number, channel);
        OSCSendProgramChange(number, channel);
        break;

      case midi::AfterTouchChannel:
        packet.read(data, 1);
        pressure = data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendAfterTouch(pressure, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendAfterTouch(pressure, channel);
        BLESendAfterTouch(pressure, channel);
        AppleMidiSendAfterTouch(pressure, channel);
        OSCSendAfterTouch(pressure, channel);
        break;

      case midi::PitchBend:
        packet.read(data, 2);
        bend = data[1] << 7 | data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendPitchBend(bend, channel);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendPitchBend(bend, channel);
        BLESendPitchBend(bend, channel);
        AppleMidiSendPitchBend(bend, channel);
        OSCSendPitchBend(bend, channel);
        break;

      case midi::SystemExclusive:
        while (packet.read(data, 1) && data[0] != 0xf7);
        break;

      case midi::TimeCodeQuarterFrame:
        packet.read(data, 1);
        value = data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendTimeCodeQuarterFrame(value);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendTimeCodeQuarterFrame(value);
        BLESendTimeCodeQuarterFrame(value);
        AppleMidiSendTimeCodeQuarterFrame(value);
        OSCSendTimeCodeQuarterFrame(value);
        MTC.decodMTCQuarterFrame(value);
      break;

      case midi::SongPosition:
        packet.read(data, 2);
        beats = data[1] << 7 | data[0];
        if (interfaces[PED_USBMIDI].midiOut) USB_MIDI.sendSongPosition(beats);
        if (interfaces[PED_DINMIDI].midiOut) DIN_MIDI.sendSongPosition(beats);
        BLESendSongPosition(beats);
        AppleMidiSendSongPosition(beats);
        OSCSendSongPosition(beats);
        break;

      case midi::SongSelect:
        packet.read(data, 1);
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

      default:
        DPRINT("ipMIDI status byte %d unknown/d", status);
        packet.read(data, 1);
        break;
    }
    leds_update(type, channel, data[0], data[1]);
    DPRINTMIDI(packet.remoteIP().toString().c_str(), status, data);
  }

}


// Listen to incoming AppleMIDI messages from WiFi

inline void rtpMIDI_listen() {

  if (!wifiEnabled) return;
  if (!WiFi.isConnected()) return;

  AppleMIDI.read();
}

#endif  // NOWIFI