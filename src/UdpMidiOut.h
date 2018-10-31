/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

#ifdef WIFI
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <AppleMidi.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#endif

// WiFi MIDI interface to comunicate with AppleMIDI/RTP-MDI devices

#ifdef WIFI
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI); // see definition in AppleMidi_Defs.h
#endif

bool          appleMidiConnected = false;
unsigned long wifiLastOn         = 0;

// ipMIDI

#ifdef WIFI
WiFiUDP                 ipMIDI;
IPAddress               ipMIDImulticast(225, 0, 0, 37);
unsigned int            ipMIDIdestPort = 21928;
#endif

// WiFi OSC comunication

#ifdef WIFI
WiFiUDP                 oscUDP;                  // A UDP instance to let us send and receive packets over UDP
IPAddress               oscRemoteIp;             // remote IP of an external OSC device or broadcast address
const unsigned int      oscRemotePort = 9000;    // remote port of an external OSC device
const unsigned int      oscLocalPort = 8000;     // local port to listen for OSC packets (actually not used for sending)
OSCMessage              oscMsg;
#endif

#ifdef NOWIFI
#define AppleMidiSendNoteOn(...)
#define AppleMidiSendNoteOff(...)
#define AppleMidiSendAfterTouchPoly(...)
#define AppleMidiSendControlChange(...)
#define AppleMidiSendProgramChange(...)
#define AppleMidiSendAfterTouch(...)
#define AppleMidiSendPitchBend(...)
#define AppleMidiSendSystemExclusive(...)
#define AppleMidiSendTimeCodeQuarterFrame(...)
#define AppleMidiSendSongPosition(...)
#define AppleMidiSendSongSelect(...)
#define AppleMidiSendTuneRequest(...)
#define AppleMidiSendClock(...)
#define AppleMidiSendStart(...)
#define AppleMidiSendContinue(...)
#define AppleMidiSendStop(...)
#define AppleMidiSendActiveSensing(...)
#define AppleMidiSendSystemReset(...)
#define ipMIDISendChannelMessage1(...)
#define ipMIDISendChannelMessage2(...)
#define ipMIDISendSystemCommonMessage1(...)
#define ipMIDISendSystemCommonMessage2(...)
#define ipMIDISendRealTimeMessage(...)
#define ipMIDISendNoteOn(...)
#define ipMIDISendNoteOff(...)
#define ipMIDISendAfterTouchPoly(...)
#define ipMIDISendControlChange(...)
#define ipMIDISendProgramChange(...)
#define ipMIDISendAfterTouch(...)
#define ipMIDISendPitchBend(...)
#define ipMIDISendSystemExclusive(...)
#define ipMIDISendTimeCodeQuarterFrame(...)
#define ipMIDISendSongPosition(...)
#define ipMIDISendSongSelect(...)
#define ipMIDISendTuneRequest(...)
#define ipMIDISendClock(...)
#define ipMIDISendStart(...)
#define ipMIDISendContinue(...)
#define ipMIDISendStop(...)
#define ipMIDISendActiveSensing(...)
#define ipMIDISendSystemReset(...)
#define OSCSendNoteOn(...)
#define OSCSendNoteOff(...)
#define OSCSendAfterTouchPoly(...)
#define OSCSendControlChange(...)
#define OSCSendProgramChange(...)
#define OSCSendAfterTouch(...)
#define OSCSendPitchBend(...)
#define OSCSendSystemExclusive(...)
#define OSCSendTimeCodeQuarterFrame(...)
#define OSCSendSongPosition(...)
#define OSCSendSongSelect(...)
#define OSCSendTuneRequest(...)
#define OSCSendClock(...)
#define OSCSendStart(...)
#define OSCSendContinue(...)
#define OSCSendStop(...)
#define OSCSendActiveSensing(...)
#define OSCSendSystemReset(...)
#else

void AppleMidiSendNoteOn(byte note, byte velocity, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.noteOn(note, velocity, channel);
}

void AppleMidiSendNoteOff(byte note, byte velocity, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.noteOff(note, velocity, channel);
}

void AppleMidiSendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.polyPressure(note, pressure, channel);
}

void AppleMidiSendControlChange(byte number, byte value, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.controlChange(number, value, channel);
}

void AppleMidiSendProgramChange(byte number, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.programChange(number, channel);
}

void AppleMidiSendAfterTouch(byte pressure, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.afterTouch(pressure, channel);
}

void AppleMidiSendPitchBend(int bend, byte channel)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.pitchBend(bend, channel);
}

void AppleMidiSendSystemExclusive(byte* array, unsigned size)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.sysEx(array, size);
}

void AppleMidiSendTimeCodeQuarterFrame(byte data)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.timeCodeQuarterFrame(data);
}

void AppleMidiSendSongPosition(unsigned int beats)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.songPosition(beats);
}

void AppleMidiSendSongSelect(byte songnumber)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.songSelect(songnumber);
}

void AppleMidiSendTuneRequest(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.tuneRequest();
}

void AppleMidiSendClock(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.clock();
}

void AppleMidiSendStart(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.start();
}

void AppleMidiSendContinue(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI._continue();
}

void AppleMidiSendStop(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.stop();
}

void AppleMidiSendActiveSensing(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.activeSensing();
}

void AppleMidiSendSystemReset(void)
{
  if (interfaces[PED_RTPMIDI].midiOut) AppleMIDI.reset();
}


// Send messages to WiFi ipMIDI interface

void ipMIDISendChannelMessage1(byte type, byte channel, byte data1)
{
  byte midiPacket[2];

  if (!interfaces[PED_IPMIDI].midiOut) return;

  midiPacket[0] = (type & 0xf0) | ((channel - 1) & 0x0f);
  midiPacket[1] = data1;
#ifdef ARDUINO_ARCH_ESP8266
  ipMIDI.beginPacketMulticast(ipMIDImulticast, ipMIDIdestPort, WiFi.localIP());
#endif
#ifdef ARDUINO_ARCH_ESP32
  ipMIDI.beginMulticastPacket();
#endif
  ipMIDI.write(midiPacket, 2);
  ipMIDI.endPacket();
}

void ipMIDISendChannelMessage2(byte type, byte channel, byte data1, byte data2)
{
  byte midiPacket[3];

  if (!interfaces[PED_IPMIDI].midiOut) return;

  midiPacket[0] = (type & 0xf0) | ((channel - 1) & 0x0f);
  midiPacket[1] = data1;
  midiPacket[2] = data2;
#ifdef ARDUINO_ARCH_ESP8266
  ipMIDI.beginPacketMulticast(ipMIDImulticast, ipMIDIdestPort, WiFi.localIP());
#endif
#ifdef ARDUINO_ARCH_ESP32
  ipMIDI.beginMulticastPacket();
#endif
  ipMIDI.write(midiPacket, 3);
  ipMIDI.endPacket();
}

void ipMIDISendSystemCommonMessage1(byte type, byte data1)
{
  byte midiPacket[2];

  if (!interfaces[PED_IPMIDI].midiOut) return;

  midiPacket[0] = type;
  midiPacket[1] = data1;
#ifdef ARDUINO_ARCH_ESP8266
  ipMIDI.beginPacketMulticast(ipMIDImulticast, ipMIDIdestPort, WiFi.localIP());
#endif
#ifdef ARDUINO_ARCH_ESP32
  ipMIDI.beginMulticastPacket();
#endif
  ipMIDI.write(midiPacket, 2);
  ipMIDI.endPacket();
}

void ipMIDISendSystemCommonMessage2(byte type, byte data1, byte data2)
{
  byte  midiPacket[3];

  if (!interfaces[PED_IPMIDI].midiOut) return;

  midiPacket[0] = type;
  midiPacket[1] = data1;
  midiPacket[2] = data2;
#ifdef ARDUINO_ARCH_ESP8266
  ipMIDI.beginPacketMulticast(ipMIDImulticast, ipMIDIdestPort, WiFi.localIP());
#endif
#ifdef ARDUINO_ARCH_ESP32
  ipMIDI.beginMulticastPacket();
#endif
  ipMIDI.write(midiPacket, 3);
  ipMIDI.endPacket();
}

void ipMIDISendRealTimeMessage(byte type)
{
  byte midiPacket[1];

  if (!interfaces[PED_IPMIDI].midiOut) return;

  midiPacket[0] = type;
#ifdef ARDUINO_ARCH_ESP8266
  ipMIDI.beginPacketMulticast(ipMIDImulticast, ipMIDIdestPort, WiFi.localIP());
#endif
#ifdef ARDUINO_ARCH_ESP32
  ipMIDI.beginMulticastPacket();
#endif
  ipMIDI.write(midiPacket, 1);
  ipMIDI.endPacket();
}

void ipMIDISendNoteOn(byte note, byte velocity, byte channel)
{
  ipMIDISendChannelMessage2(midi::NoteOn, channel, note, velocity);
}

void ipMIDISendNoteOff(byte note, byte velocity, byte channel)
{
  ipMIDISendChannelMessage2(midi::NoteOff, channel, note, velocity);
}

void ipMIDISendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  ipMIDISendChannelMessage2(midi::AfterTouchPoly, channel, note, pressure);
}

void ipMIDISendControlChange(byte number, byte value, byte channel)
{
  ipMIDISendChannelMessage2(midi::ControlChange, channel, number, value);
}

void ipMIDISendProgramChange(byte number, byte channel)
{
  ipMIDISendChannelMessage1(midi::ProgramChange, channel, number);
}

void ipMIDISendAfterTouch(byte pressure, byte channel)
{
  ipMIDISendChannelMessage1(midi::AfterTouchChannel, channel, pressure);
}

void ipMIDISendPitchBend(int bend, byte channel)
{
  ipMIDISendChannelMessage1(midi::PitchBend, channel, bend);
}

void ipMIDISendSystemExclusive(const byte* array, unsigned size)
{
  //
  //  to be implemented
  //
}

void ipMIDISendTimeCodeQuarterFrame(byte data)
{
  ipMIDISendSystemCommonMessage1(midi::TimeCodeQuarterFrame, data);
}

void ipMIDISendSongPosition(unsigned int beats)
{
  ipMIDISendSystemCommonMessage2(midi::SongPosition, beats >> 4, beats & 0x0f);
}

void ipMIDISendSongSelect(byte songnumber)
{
  ipMIDISendSystemCommonMessage1(midi::SongSelect, songnumber);
}

void ipMIDISendTuneRequest(void)
{
  ipMIDISendRealTimeMessage(midi::TuneRequest);
}

void ipMIDISendClock(void)
{
  ipMIDISendRealTimeMessage(midi::Clock);
}

void ipMIDISendStart(void)
{
  ipMIDISendRealTimeMessage(midi::Start);
}

void ipMIDISendContinue(void)
{
  ipMIDISendRealTimeMessage(midi::Continue);
}

void ipMIDISendStop(void)
{
  ipMIDISendRealTimeMessage(midi::Stop);
}

void ipMIDISendActiveSensing(void)
{
  ipMIDISendRealTimeMessage(midi::ActiveSensing);
}

void ipMIDISendSystemReset(void)
{
  ipMIDISendRealTimeMessage(midi::SystemReset);
}


// Send messages to WiFI OSC interface

void OSCSendNoteOn(byte note, byte velocity, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/note/";
  msg += note;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((float)(velocity / 127.0)).add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendNoteOff(byte note, byte velocity, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/note/";
  msg += note;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((float)0).add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/aftertouchpoly/";
  msg += note;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((float)(pressure / 127.0)).add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendControlChange(byte number, byte value, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/cc/";
  msg += number;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((float)(value / 127.0)).add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendProgramChange(byte number, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/pc/";
  msg += number;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendAfterTouch(byte pressure, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/aftertouchchannel/";
  msg += channel;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((float)(pressure / 127.0)).add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendPitchBend(int bend, byte channel)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/pitchbend/";
  msg += channel;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((float)((bend + 8192) / 16383.0)).add((int32_t)channel).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendSystemExclusive(const byte* array, unsigned size)
{
}

void OSCSendTimeCodeQuarterFrame(byte data)
{
}

void OSCSendSongPosition(unsigned int beats)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/songpostion/";
  msg += beats;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((int32_t)beats).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendSongSelect(byte songnumber)
{
  if (!interfaces[PED_OSC].midiOut) return;

  String msg = "/pedalino/midi/songselect/";
  msg += songnumber;
  OSCMessage oscMsg(msg.c_str());
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.add((int32_t)songnumber).send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendTuneRequest(void)
{
  if (!interfaces[PED_OSC].midiOut) return;

  OSCMessage oscMsg("/pedalino/midi/tunerequest/");
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendClock(void)
{
}

void OSCSendStart(void)
{
  if (!interfaces[PED_OSC].midiOut) return;

  OSCMessage oscMsg("/pedalino/midi/start/");
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendContinue(void)
{
  if (!interfaces[PED_OSC].midiOut) return;

  OSCMessage oscMsg("/pedalino/midi/continue/");
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendStop(void)
{
  if (!interfaces[PED_OSC].midiOut) return;

  OSCMessage oscMsg("/pedalino/midi/stop/");
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendActiveSensing(void)
{
  if (!interfaces[PED_OSC].midiOut) return;

  OSCMessage oscMsg("/pedalino/midi/activesensing/");
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.send(oscUDP).empty();
  oscUDP.endPacket();
}

void OSCSendSystemReset(void)
{
  if (!interfaces[PED_OSC].midiOut) return;

  OSCMessage oscMsg("/pedalino/midi/reset/");
  oscUDP.beginPacket(oscRemoteIp, oscRemotePort);
  oscMsg.send(oscUDP).empty();
  oscUDP.endPacket();
}
#endif  //  NOWIFI
