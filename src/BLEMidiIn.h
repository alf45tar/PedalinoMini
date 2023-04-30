/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2022 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#undef CLASSIC_BT_ENABLED

#ifdef BLE
#include <BLEMIDI_Transport.h>
#ifdef BLECLIENT
#include <hardware/BLEMIDI_Client_ESP32.h>
/**
 * Client BLEMIDI works im a similar way Server (Common) BLEMIDI, but with some exception.
 *
 * The most importart exception is read() method. This function works as usual, but
 * now it manages machine-states BLE connection too. The
 * read() function must be called several times continuously in order to scan BLE device
 * and connect with the server. In this example, read() is called in a "multitask function of
 * FreeRTOS", but it can be called in loop() function as usual.
 *
 * Some BLEMIDI_CREATE_INSTANCE() are added in MidiBLE-Client to be able to choose a specific server to connect
 * or to connect to the first server which has the MIDI characteristic. You can choose the server by typing in the name field
 * the name of the server or the BLE address of the server. If you want to connect
 * to the first MIDI server BLE found by the device, you just have to set the name field empty ("").
 *
 * FOR ADVANCED USERS: Other advanced BLE configurations can be changed in hardware/BLEMIDI_Client_ESP32.h
 * #defines in the head of the file (IMPORTANT: Only the first user defines must be modified). These configurations
 * are related to security (password, pairing and securityCallback()), communication params, the device name
 * and other stuffs. Modify defines at your own risk.
 */

/**
 * This function is called by xTaskCreatePinnedToCore() to perform a multitask execution.
 * In this task, read() is called every millisecond (approx.).
 * read() function performs connection, reconnection and scan-BLE functions.
 * Call read() method repeatedly to perform a successfull connection with the server
 * in case connection is lost.
*/
void BleMidiReadCallBack(void *parameter)
{
  DPRINT("BLE MIDI READ Task is started on core: %d\n", xPortGetCoreID());
  for (;;)
  {
    if (bleEnabled) {
      if (interfaces[PED_BLEMIDI].midiIn && BLE_MIDI.read())
        DPRINTMIDI("BLE MIDI", BLE_MIDI.getType(), BLE_MIDI.getChannel(), BLE_MIDI.getData1(), BLE_MIDI.getData2());
    }
    vTaskDelay(1 / portTICK_PERIOD_MS); //Feed the watchdog of FreeRTOS.
    //Serial.println(uxTaskGetStackHighWaterMark(NULL)); //Only for debug. You can see the watermark of the free resources assigned by the xTaskCreatePinnedToCore() function.
  }
  vTaskDelay(1);
}
#else
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#endif


void OnBleMidiConnected() {
  bleMidiConnected = true;
#ifdef BLECLIENT
  DPRINTLN("Pedalino BLE MIDI client %s connected to server %s", host.c_str(), bleServer.c_str());
#else
  DPRINTLN("BLE MIDI client connected to Pedalino %s", host.c_str());
#endif
}

void OnBleMidiDisconnected() {
  bleMidiConnected = false;
  DPRINTLN("BLE MIDI client disconnected");
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
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::NoteOn, note, velocity, channel);
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
  leds_update(midi::NoteOff, channel, note, velocity);
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::NoteOff, note, velocity, channel);
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
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::AfterTouchPoly, note, pressure, channel);
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
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::ControlChange, number, value, channel);
  switch_profile_or_bank(channel, number, value);
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
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::ProgramChange, number, 0, channel);
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
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::AfterTouchChannel, pressure, 0, channel);
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
  if (IS_SHOW_ENABLED(interfaces[PED_BLEMIDI].midiIn)) screen_info(midi::PitchBend, bend, 0, channel);
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
  MTC.decodeMTCFullFrame(size, array);
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
  MTC.decodMTCQuarterFrame(data);
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
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) bpm = MTC.tapTempo();
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
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendPlay();
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
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendContinue();
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
  if (MTC.getMode() == MidiTimeCode::SynchroClockSlave) MTC.sendStop();
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

  BLEBLE_MIDI.setHandleConnected(OnBleMidiConnected);
  BLEBLE_MIDI.setHandleDisconnected(OnBleMidiDisconnected);

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

#ifdef BLECLIENT
  xTaskCreatePinnedToCore(BleMidiReadCallBack, "MIDI-READ", 3000, NULL, 1, NULL, 1);
#endif
}

#endif  // BLE