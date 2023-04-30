/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2023 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#include <MIDI.h>


void USB_MIDI_SendRealTimeMessage(byte type)
{
  if (!interfaces[PED_USBMIDI].midiOut) return;

  switch (type) {

      case midi::TuneRequest:
        USB_MIDI.sendTuneRequest();
        break;

      case midi::Clock:
        USB_MIDI.sendClock();
        break;

      case midi::Start:
        USB_MIDI.sendStart();
        break;

      case midi::Continue:
        USB_MIDI.sendContinue();
        break;

      case midi::Stop:
        USB_MIDI.sendStop();
        break;

      case midi::ActiveSensing:
        USB_MIDI.sendActiveSensing();
        break;

      case midi::SystemReset:
        USB_MIDI.sendSystemReset();
        break;
    }
}

void DIN_MIDI_SendRealTimeMessage(byte type)
{
  if (!interfaces[PED_DINMIDI].midiOut) return;

  switch (type) {

      case midi::TuneRequest:
        DIN_MIDI.sendTuneRequest();
        break;

      case midi::Clock:
        DIN_MIDI.sendClock();
        break;

      case midi::Start:
        DIN_MIDI.sendStart();
        break;

      case midi::Continue:
        DIN_MIDI.sendContinue();
        break;

      case midi::Stop:
        DIN_MIDI.sendStop();
        break;

      case midi::ActiveSensing:
        DIN_MIDI.sendActiveSensing();
        break;

      case midi::SystemReset:
        DIN_MIDI.sendSystemReset();
        break;
    }
}