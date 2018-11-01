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
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#endif

#ifdef ARDUINO_ARCH_ESP8266
#define BLE_LED_OFF()
#define BLE_LED_ON()
#endif

#ifdef ARDUINO_ARCH_ESP32
#define BLE_LED         15
#define BLE_LED_OFF()   digitalWrite(BLE_LED, LOW)
#define BLE_LED_ON()    digitalWrite(BLE_LED, HIGH)
#endif

// Bluetooth LE MIDI interface

#if defined(ARDUINO_ARCH_ESP32) && defined(BLE)

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

BLEServer             *pServer;
BLEService            *pService;
BLEAdvertising        *pAdvertising;
BLECharacteristic     *pCharacteristic;
BLESecurity           *pSecurity;
#endif

bool                  bleMidiConnected = false;
unsigned long         bleLastOn        = 0;

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
#define BLESendStop(...) {}
#define BLESendActiveSensing(...)
#define BLESendSystemReset(...)
#else
void BLEMidiReceive(uint8_t *, uint8_t);

class MyBLEServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      bleMidiConnected = true;
      DPRINT("BLE client connected");
    };

    void onDisconnect(BLEServer* pServer) {
      bleMidiConnected = false;
      DPRINT("BLE client disconnected");
    }
};

class MyBLECharateristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0)
        if (interfaces[PED_BLEMIDI].midiIn) {
          BLEMidiReceive((uint8_t *)(rxValue.c_str()), rxValue.length());
          DPRINT("BLE Received %2d bytes", rxValue.length());
        }
    }
};

void ble_midi_start_service ()
{
  BLEDevice::init("Pedal");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyBLEServerCallbacks());

  pService = pServer->createService(MIDI_SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(MIDI_CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_WRITE_NR);

  pCharacteristic->setCallbacks(new MyBLECharateristicCallbacks());

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();

  pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
}

void BLEMidiTimestamp (uint8_t *header, uint8_t *timestamp)
{
  /*
    The first byte of all BLE packets must be a header byte. This is followed by timestamp bytes and MIDI messages.

    Header Byte
      bit 7     Set to 1.
      bit 6     Set to 0. (Reserved for future use)
      bits 5-0  timestampHigh:Most significant 6 bits of timestamp information.
    The header byte contains the topmost 6 bits of timing information for MIDI events in the BLE
    packet. The remaining 7 bits of timing information for individual MIDI messages encoded in a
    packet is expressed by timestamp bytes.
    Timestamp Byte
    bit 7       Set to 1.
    bits 6-0    timestampLow: Least Significant 7 bits of timestamp information.
    The 13-bit timestamp for the first MIDI message in a packet is calculated using 6 bits from the
    header byte and 7 bits from the timestamp byte.
    Timestamps are 13-bit values in milliseconds, and therefore the maximum value is 8,191 ms.
    Timestamps must be issued by the sender in a monotonically increasing fashion.
    timestampHigh is initially set using the lower 6 bits from the header byte while the timestampLow is
    formed of the lower 7 bits from the timestamp byte. Should the timestamp value of a subsequent
    MIDI message in the same packet overflow/wrap (i.e., the timestampLow is smaller than a
    preceding timestampLow), the receiver is responsible for tracking this by incrementing the
    timestampHigh by one (the incremented value is not transmitted, only understood as a result of the
    overflow condition).
    In practice, the time difference between MIDI messages in the same BLE packet should not span
    more than twice the connection interval. As a result, a maximum of one overflow/wrap may occur
    per BLE packet.
    Timestamps are in the sender’s clock domain and are not allowed to be scheduled in the future.
    Correlation between the receiver’s clock and the received timestamps must be performed to
    ensure accurate rendering of MIDI messages, and is not addressed in this document.
  */
  /*
    Calculating a Timestamp
    To calculate the timestamp, the built-in millis() is used.
    The BLE standard only specifies 13 bits worth of millisecond data though,
    so it’s bitwise anded with 0x1FFF for an ever repeating cycle of 13 bits.
    This is done right after a MIDI message is detected. It’s split into a 6 upper bits, 7 lower bits,
    and the MSB of both bytes are set to indicate that this is a header byte.
    Both bytes are placed into the first two position of an array in preparation for a MIDI message.
  */
  unsigned long currentTimeStamp = millis() & 0x01FFF;

  *header = ((currentTimeStamp >> 7) & 0x3F) | 0x80;        // 6 bits plus MSB
  *timestamp = (currentTimeStamp & 0x7F) | 0x80;            // 7 bits plus MSB
}


// Decodes the BLE characteristics and calls MIDI.send if the packet contains sendable MIDI data
// https://learn.sparkfun.com/tutorials/midi-ble-tutorial

void BLEMidiReceive(uint8_t *buffer, uint8_t bufferSize)
{
  /*
    The general form of a MIDI message follows:

    n-byte MIDI Message
      Byte 0            MIDI message Status byte, Bit 7 is Set to 1.
      Bytes 1 to n-1    MIDI message Data bytes, if n > 1. Bit 7 is Set to 0
    There are two types of MIDI messages that can appear in a single packet: full MIDI messages and
    Running Status MIDI messages. Each is encoded differently.
    A full MIDI message is simply the MIDI message with the Status byte included.
    A Running Status MIDI message is a MIDI message with the Status byte omitted. Running Status
    MIDI messages may only be placed in the data stream if the following criteria are met:
    1.  The original MIDI message is 2 bytes or greater and is not a System Common or System
        Real-Time message.
    2.  The omitted Status byte matches the most recently preceding full MIDI message’s Status
        byte within the same BLE packet.
    In addition, the following rules apply with respect to Running Status:
    1.  A Running Status MIDI message is allowed within the packet after at least one full MIDI
        message.
    2.  Every MIDI Status byte must be preceded by a timestamp byte. Running Status MIDI
        messages may be preceded by a timestamp byte. If a Running Status MIDI message is not
        preceded by a timestamp byte, the timestamp byte of the most recently preceding message
        in the same packet is used.
    3.  System Common and System Real-Time messages do not cancel Running Status if
        interspersed between Running Status MIDI messages. However, a timestamp byte must
        precede the Running Status MIDI message that follows.
    4.  The end of a BLE packet does cancel Running Status.
    In the MIDI 1.0 protocol, System Real-Time messages can be sent at any time and may be
    inserted anywhere in a MIDI data stream, including between Status and Data bytes of any other
    MIDI messages. In the MIDI BLE protocol, the System Real-Time messages must be deinterleaved
    from other messages – except for System Exclusive messages.
  */
  midi::Channel   channel;
  midi::MidiType  command;

  //Pointers used to search through payload.
  uint8_t lPtr = 0;
  uint8_t rPtr = 0;
  //Decode first packet -- SHALL be "Full MIDI message"
  lPtr = 2; //Start at first MIDI status -- SHALL be "MIDI status"
  //While statement contains incrementing pointers and breaks when buffer size exceeded.
  while (1) {
    //lastStatus used to capture runningStatus
    uint8_t lastStatus = buffer[lPtr];
    if ( (buffer[lPtr] < 0x80) ) {
      //Status message not present, bail
      return;
    }
    command = MIDI.getTypeFromStatusByte(lastStatus);
    channel = MIDI.getChannelFromStatusByte(lastStatus);
    //Point to next non-data byte
    rPtr = lPtr;
    while ( (buffer[rPtr + 1] < 0x80) && (rPtr < (bufferSize - 1)) ) {
      rPtr++;
    }
    //look at l and r pointers and decode by size.
    if ( rPtr - lPtr < 1 ) {
      //Time code or system
      MIDI.send(command, 0, 0, channel);
    } else if ( rPtr - lPtr < 2 ) {
      MIDI.send(command, buffer[lPtr + 1], 0, channel);
    } else if ( rPtr - lPtr < 3 ) {
      MIDI.send(command, buffer[lPtr + 1], buffer[lPtr + 2], channel);
    } else {
      //Too much data
      //If not System Common or System Real-Time, send it as running status
      switch ( buffer[lPtr] & 0xF0 )
      {
        case 0x80:
        case 0x90:
        case 0xA0:
        case 0xB0:
        case 0xE0:
          for (int i = lPtr; i < rPtr; i = i + 2) {
            MIDI.send(command, buffer[i + 1], buffer[i + 2], channel);
          }
          break;
        case 0xC0:
        case 0xD0:
          for (int i = lPtr; i < rPtr; i = i + 1) {
            MIDI.send(command, buffer[i + 1], 0, channel);
          }
          break;
        default:
          break;
      }
    }
    //Point to next status
    lPtr = rPtr + 2;
    if (lPtr >= bufferSize) {
      //end of packet
      return;
    }
  }
}

void BLESendChannelMessage1(byte type, byte channel, byte data1)
{
  uint8_t midiPacket[4];

  if (!interfaces[PED_BLEMIDI].midiOut) return;

  BLEMidiTimestamp(&midiPacket[0], &midiPacket[1]);
  midiPacket[2] = (type & 0xf0) | ((channel - 1) & 0x0f);
  midiPacket[3] = data1;
  pCharacteristic->setValue(midiPacket, 4);
  pCharacteristic->notify();
}

void BLESendChannelMessage2(byte type, byte channel, byte data1, byte data2)
{
  uint8_t midiPacket[5];

  if (!interfaces[PED_BLEMIDI].midiOut) return;

  BLEMidiTimestamp(&midiPacket[0], &midiPacket[1]);
  midiPacket[2] = (type & 0xf0) | ((channel - 1) & 0x0f);
  midiPacket[3] = data1;
  midiPacket[4] = data2;
  pCharacteristic->setValue(midiPacket, 5);
  pCharacteristic->notify();
}

void BLESendSystemCommonMessage1(byte type, byte data1)
{
  uint8_t midiPacket[4];

  if (!interfaces[PED_BLEMIDI].midiOut) return;

  BLEMidiTimestamp(&midiPacket[0], &midiPacket[1]);
  midiPacket[2] = type;
  midiPacket[3] = data1;
  pCharacteristic->setValue(midiPacket, 4);
  pCharacteristic->notify();
}

void BLESendSystemCommonMessage2(byte type, byte data1, byte data2)
{
  uint8_t midiPacket[5];

  if (!interfaces[PED_BLEMIDI].midiOut) return;

  BLEMidiTimestamp(&midiPacket[0], &midiPacket[1]);
  midiPacket[2] = type;
  midiPacket[3] = data1;
  midiPacket[4] = data2;
  pCharacteristic->setValue(midiPacket, 5);
  pCharacteristic->notify();
}

void BLESendRealTimeMessage(byte type)
{
  uint8_t midiPacket[3];

  if (!interfaces[PED_BLEMIDI].midiOut) return;

  BLEMidiTimestamp(&midiPacket[0], &midiPacket[1]);
  midiPacket[2] = type;
  pCharacteristic->setValue(midiPacket, 3);
  pCharacteristic->notify();
}

void BLESendNoteOn(byte note, byte velocity, byte channel)
{
  BLESendChannelMessage2(midi::NoteOn, channel, note, velocity);
}

void BLESendNoteOff(byte note, byte velocity, byte channel)
{
  BLESendChannelMessage2(midi::NoteOff, channel, note, velocity);
}

void BLESendAfterTouchPoly(byte note, byte pressure, byte channel)
{
  BLESendChannelMessage2(midi::AfterTouchPoly, channel, note, pressure);
}

void BLESendControlChange(byte number, byte value, byte channel)
{
  BLESendChannelMessage2(midi::ControlChange, channel, number, value);
}

void BLESendProgramChange(byte number, byte channel)
{
  BLESendChannelMessage1(midi::ProgramChange, channel, number);
}

void BLESendAfterTouch(byte pressure, byte channel)
{
  BLESendChannelMessage1(midi::AfterTouchChannel, channel, pressure);
}

void BLESendPitchBend(int bend, byte channel)
{
  BLESendChannelMessage1(midi::PitchBend, channel, bend);
}

void BLESendSystemExclusive(const byte* array, unsigned size)
{
  /*
    Multiple Packet Encoding (SysEx Only)
    Only a SysEx (System Exclusive) message may span multiple BLE packets and is encoded as
    follows:
    1.  The SysEx start byte, which is a MIDI Status byte, is preceded by a timestamp byte.
    2.  Following the SysEx start byte, any number of Data bytes (up to the number of the
        remaining bytes in the packet) may be written.
    3.  Any remaining data may be sent in one or more SysEx continuation packets. A SysEx
        continuation packet begins with a header byte but does not contain a timestamp byte. It
        then contains one or more bytes of the SysEx data, up to the maximum packet length. This
        lack of a timestamp byte serves as a signal to the decoder of a SysEx continuation.
    4.  System Real-Time messages may appear at any point inside a SysEx message and must
        be preceded by a timestamp byte.
    5.  SysEx continuations for unterminated SysEx messages must follow either the packet’s
        header byte or a real-time byte.
    6.  Continue sending SysEx continuation packets until the entire message is transmitted.
    7.  In the last packet containing SysEx data, precede the EOX message (SysEx end byte),
        which is a MIDI Status byte, with a timestamp byte.
    Once a SysEx transfer has begun, only System Real-Time messages are allowed to precede its
    completion as follows:
    1.  A System Real-Time message interrupting a yet unterminated SysEx message must be
        preceded by its own timestamp byte.
    2.  SysEx continuations for unterminated SysEx messages must follow either the packet’s
        header byte or a real-time byte.
  */

  //
  //  to be implemented
  //
}

void BLESendTimeCodeQuarterFrame(byte data)
{
  BLESendSystemCommonMessage1(midi::TimeCodeQuarterFrame, data);
}

void BLESendSongPosition(unsigned int beats)
{
  BLESendSystemCommonMessage2(midi::SongPosition, beats >> 4, beats & 0x0f);
}

void BLESendSongSelect(byte songnumber)
{
  BLESendSystemCommonMessage1(midi::SongSelect, songnumber);
}

void BLESendTuneRequest(void)
{
  BLESendRealTimeMessage(midi::TuneRequest);
}

void BLESendClock(void)
{
  BLESendRealTimeMessage(midi::Clock);
}

void BLESendStart(void)
{
  BLESendRealTimeMessage(midi::Start);
}

void BLESendContinue(void)
{
  BLESendRealTimeMessage(midi::Continue);
}

void BLESendStop(void)
{
  BLESendRealTimeMessage(midi::Stop);
}

void BLESendActiveSensing(void)
{
  BLESendRealTimeMessage(midi::ActiveSensing);
}

void BLESendSystemReset(void)
{
  BLESendRealTimeMessage(midi::SystemReset);
}

#endif  // NOBLE