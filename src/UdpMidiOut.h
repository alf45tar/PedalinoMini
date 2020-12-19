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
#include <WiFiUdp.h>
#include <AsyncUDP.h>
#include <AppleMIDI.h>
#include <ipMIDI.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

// WiFi MIDI interface to comunicate with AppleMIDI/RTP-MDI devices

//APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI); // see definition in AppleMidi_Defs.h
USING_NAMESPACE_APPLEMIDI
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, RTP_MIDI, "PedalinoMini", DEFAULT_CONTROL_PORT);

// WiFi interface to comunicate with ipMIDI devices

IPMIDI_CREATE_INSTANCE(WiFiUDP, IP_MIDI, 21928);


// WiFi OSC comunication

AsyncUDP                oscUDPout;               // A UDP instance to let us send packets over UDP
IPAddress               oscRemoteIp;             // remote IP of an external OSC device or broadcast address
const unsigned int      oscRemotePort = 9000;    // remote port of an external OSC device

AsyncUDP                oscUDPin;                // A UDP instance to let us receive packets over UDP
const unsigned int      oscLocalPort  = 8000;    // local port to listen for OSC packets
#endif  // WIFI

bool                    appleMidiConnected = false;
String                  appleMidiSessionName;
unsigned long           wifiLastOn         = 0;

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
#define AppleMidiSendRealTimeMessage(...)
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
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendNoteOn(note, velocity, channel);
}

void AppleMidiSendNoteOff(byte note, byte velocity, byte channel)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendNoteOff(note, velocity, channel);
}

void AppleMidiSendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendPolyPressure(note, pressure, channel);
}

void AppleMidiSendControlChange(byte number, byte value, byte channel)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendControlChange(number, value, channel);
}

void AppleMidiSendProgramChange(byte number, byte channel)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendProgramChange(number, channel);
}

void AppleMidiSendAfterTouch(byte pressure, byte channel)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendAfterTouch(pressure, channel);
}

void AppleMidiSendPitchBend(int bend, byte channel)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendPitchBend(bend, channel);
}

void AppleMidiSendSystemExclusive(const byte* array, unsigned int size)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendSysEx(size, array);
}

void AppleMidiSendTimeCodeQuarterFrame(byte data)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendTimeCodeQuarterFrame(data);
}

void AppleMidiSendSongPosition(unsigned int beats)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendSongPosition(beats);
}

void AppleMidiSendSongSelect(byte songnumber)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendSongSelect(songnumber);
}

void AppleMidiSendTuneRequest(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendTuneRequest();
}

void AppleMidiSendClock(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendClock();
}

void AppleMidiSendStart(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendStart();
}

void AppleMidiSendContinue(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendContinue();
}

void AppleMidiSendStop(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendStop();
}

void AppleMidiSendActiveSensing(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendActiveSensing();
}

void AppleMidiSendSystemReset(void)
{
  if (wifiEnabled && interfaces[PED_RTPMIDI].midiOut) RTP_MIDI.sendSystemReset();
}

void AppleMidiSendRealTimeMessage(byte type)
{
  switch (type) {

      case midi::TuneRequest:
        AppleMidiSendTuneRequest();
        break;

      case midi::Clock:
        AppleMidiSendClock();
        break;

      case midi::Start:
        AppleMidiSendStart();
        break;

      case midi::Continue:
        AppleMidiSendContinue();
        break;

      case midi::Stop:
        AppleMidiSendStop();
        break;

      case midi::ActiveSensing:
        AppleMidiSendActiveSensing();
        break;

      case midi::SystemReset:
        AppleMidiSendSystemReset();
        break;
    }
}


// Send messages to WiFi ipMIDI interface

void ipMIDISendNoteOn(byte note, byte velocity, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendNoteOn(note, velocity, channel);
}

void ipMIDISendNoteOff(byte note, byte velocity, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendNoteOff(note, velocity, channel);
}

void ipMIDISendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendPolyPressure(note, pressure, channel);
}

void ipMIDISendControlChange(byte number, byte value, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendControlChange(number, value, channel);
}

void ipMIDISendProgramChange(byte number, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendProgramChange(number, channel);
}

void ipMIDISendAfterTouch(byte pressure, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendAfterTouch(pressure, channel);
}

void ipMIDISendPitchBend(int bend, byte channel)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendPitchBend(bend, channel);
}

void ipMIDISendSystemExclusive(const byte* array, unsigned int size)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendSysEx(size, array);
}

void ipMIDISendTimeCodeQuarterFrame(byte data)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendTimeCodeQuarterFrame(data);
}

void ipMIDISendSongPosition(unsigned int beats)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendSongPosition(beats);
}

void ipMIDISendSongSelect(byte songnumber)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendSongSelect(songnumber);
}

void ipMIDISendTuneRequest(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendTuneRequest();
}

void ipMIDISendClock(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendClock();
}

void ipMIDISendStart(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendStart();
}

void ipMIDISendContinue(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendContinue();
}

void ipMIDISendStop(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendStop();
}

void ipMIDISendActiveSensing(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendActiveSensing();
}

void ipMIDISendSystemReset(void)
{
  if (wifiEnabled && interfaces[PED_IPMIDI].midiOut) IP_MIDI.sendSystemReset();
}

void ipMIDISendRealTimeMessage(byte type)
{
  switch (type) {

      case midi::TuneRequest:
        ipMIDISendTuneRequest();
        break;

      case midi::Clock:
        ipMIDISendClock();
        break;

      case midi::Start:
        ipMIDISendStart();
        break;

      case midi::Continue:
        ipMIDISendContinue();
        break;

      case midi::Stop:
        ipMIDISendStop();
        break;

      case midi::ActiveSensing:
        ipMIDISendActiveSensing();
        break;

      case midi::SystemReset:
        ipMIDISendSystemReset();
        break;
    }
}


// Send messages to WiFI OSC interface

void OSCSendNoteOn(byte note, byte velocity, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/note/";
  msg += note;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((float)(velocity / 127.0)).add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendNoteOff(byte note, byte velocity, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/note/";
  msg += note;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((float)0).add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/aftertouchpoly/";
  msg += note;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((float)(pressure / 127.0)).add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendControlChange(byte number, byte value, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/cc/";
  msg += number;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((float)(value / 127.0)).add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendProgramChange(byte number, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/pc/";
  msg += number;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendAfterTouch(byte pressure, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/aftertouchchannel/";
  msg += channel;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((float)(pressure / 127.0)).add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendPitchBend(int bend, byte channel)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/pitchbend/";
  msg += channel;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((float)((bend + 8192) / 16383.0)).add((int32_t)channel).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendSystemExclusive(const byte* array, unsigned size)
{
}

void OSCSendTimeCodeQuarterFrame(byte data)
{
}

void OSCSendSongPosition(unsigned int beats)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/songpostion/";
  msg += beats;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((int32_t)beats).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendSongSelect(byte songnumber)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  String msg = "/pedalino/midi/songselect/";
  msg += songnumber;
  OSCMessage oscMsg(msg.c_str());
  oscMsg.add((int32_t)songnumber).send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendTuneRequest(void)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  OSCMessage oscMsg("/pedalino/midi/tunerequest/");
  oscMsg.send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendClock(void)
{
}

void OSCSendStart(void)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  OSCMessage oscMsg("/pedalino/midi/start/");
  oscMsg.send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendContinue(void)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  OSCMessage oscMsg("/pedalino/midi/continue/");
  oscMsg.send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendStop(void)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  OSCMessage oscMsg("/pedalino/midi/stop/");
  oscMsg.send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendActiveSensing(void)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  OSCMessage oscMsg("/pedalino/midi/activesensing/");
  oscMsg.send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}

void OSCSendSystemReset(void)
{
  if (!wifiEnabled || !interfaces[PED_OSC].midiOut) return;

  AsyncUDPMessage udpMsg;
  OSCMessage oscMsg("/pedalino/midi/reset/");
  oscMsg.send(udpMsg).empty();
  oscUDPout.send(udpMsg);
}
#endif  //  NOWIFI