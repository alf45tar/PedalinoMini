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


void leds_update(byte type, byte channel, byte data1, byte data2)
{
  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].function == PED_MIDI && banks[currentBank][i].midiChannel == channel) {
      switch (banks[currentBank][i].midiMessage) {
        case PED_PROGRAM_CHANGE:
          // Led on on PC pedal, other PC pedals off (see below)
          if (type == midi::ProgramChange && banks[currentBank][i].midiCode == data1) {
            leds.setHigh(i);
            leds.write();
          }
          break;
        case PED_CONTROL_CHANGE:
          if (type == midi::ControlChange && banks[currentBank][i].midiCode == data1) {
            if (data2 == 0) {
              // Led off if CC value == 0
              leds.setLow(i);
              leds.write();
            } else {
              // Led on if CC value is > 0
              leds.setHigh(i);
              leds.write();
            }
          }
          break;
        case PED_NOTE_ON_OFF:
          // Invert the status only on NoteOn
          if (type == midi::NoteOn && banks[currentBank][i].midiCode == data1) {
            leds.invert(i);
            leds.write();
          }
          break;
      }
    }
  }
  // Reset leds of other Program Change pedals
  if (type == midi::ProgramChange) {
    for (byte i = 0; i < PEDALS; i++) {
      if (pedals[i].function                == PED_MIDI &&
          banks[currentBank][i].midiChannel == channel &&
          banks[currentBank][i].midiMessage == PED_PROGRAM_CHANGE &&
          banks[currentBank][i].midiCode    != data1) {
        leds.setLow(i);
        leds.write();
      }
    }
  }
}


//
//
//
void mtc_midi_real_time_message_send(byte b)
{
  if (interfaces[PED_RTPMIDI].midiClock) AppleMidiSendRealTimeMessage(b);
  if (interfaces[PED_IPMIDI].midiClock)  ipMIDISendRealTimeMessage(b);
}

void mtc_midi_time_code_quarter_frame_send(byte b)
{
  if (interfaces[PED_RTPMIDI].midiClock) AppleMidiSendTimeCodeQuarterFrame(b);
  if (interfaces[PED_IPMIDI].midiClock)  ipMIDISendTimeCodeQuarterFrame(b);
}

//
// MIDI Time Code/MIDI Clock setup
//
void mtc_setup() {

  MTC.setup(mtc_midi_real_time_message_send, mtc_midi_time_code_quarter_frame_send);

  switch (currentMidiTimeCode) {

    case PED_MTC_NONE:
      DPRINTLN("MTC None");
      MTC.setMode(MidiTimeCode::SynchroNone);
      break;

    case PED_MTC_SLAVE:
      DPRINTLN("MTC Slave");
      MTC.setMode(MidiTimeCode::SynchroMTCSlave);
      break;

    case PED_MTC_MASTER_24:
    case PED_MTC_MASTER_25:
    case PED_MTC_MASTER_30DF:
    case PED_MTC_MASTER_30:
      DPRINTLN("MTC Master");
      MTC.setMode(MidiTimeCode::SynchroMTCMaster);
      MTC.sendPosition(0, 0, 0, 0);
      break;

    case PED_MIDI_CLOCK_SLAVE:
      DPRINTLN("MIDI Clock Slave");
      MTC.setMode(MidiTimeCode::SynchroClockSlave);
      bpm = 0;
      break;

    case PED_MIDI_CLOCK_MASTER:
      DPRINTLN("MIDI Clock Master");
      MTC.setMode(MidiTimeCode::SynchroClockMaster);
      MTC.setBpm(bpm);
      break;
  }
}

void mtc_start()
{
  if (MTC.isPlaying())
    MTC.sendStop();
  else {
    MTC.sendPosition(0, 0, 0, 0);
    MTC.sendPlay();
  }
}

void mtc_stop()
{
  if (MTC.isPlaying())
    MTC.sendStop();
  else
    MTC.sendPosition(0, 0, 0, 0);
}

void mtc_continue()
{
  if (MTC.isPlaying())
    MTC.sendStop();
  else if (MTC.getFrames()  == 0 &&
           MTC.getSeconds() == 0 &&
           MTC.getMinutes() == 0 &&
           MTC.getHours()   == 0)
    MTC.sendPlay();
  else
    MTC.sendContinue();
}

void mtc_tap()
{
  bpm = MTC.tapTempo();
  if      (bpm <  40) MTC.setBpm(40);
  else if (bpm > 300) MTC.setBpm(300);
  else                MTC.setBpm(bpm);
}

void mtc_tap_continue()
{
  switch (MTC.getMode()) {
    case MidiTimeCode::SynchroClockMaster:
      mtc_tap();
      break;
    case MidiTimeCode::SynchroMTCMaster:
      mtc_continue();
      break;
    case MidiTimeCode::SynchroNone:
    case MidiTimeCode::SynchroClockSlave:
    case MidiTimeCode::SynchroMTCSlave:
      break;
  }
}

//
//  Autosensing setup
//
void autosensing_setup()
{
  int tip;    // tip connected to an input digital pin with internal pull-up resistor
  int ring;   // ring connected to an input analog pin
  /*        */// sleeve connected to GND

  DPRINT("Pedal autosensing...\n");
  analogReadResolution(ADC_RESOLUTION_BITS);
  analogSetAttenuation(ADC_11db);

  for (byte p = 0; p < PEDALS; p++) {
    if (pedals[p].autoSensing && pedals[p].mode != PED_NONE && pedals[p].mode != PED_LADDER) {

      pinMode(PIN_D(p), INPUT_PULLUP);
      pinMode(PIN_A(p), INPUT_PULLUP);
      tip  = analogRead(PIN_D(p));
      ring = analogRead(PIN_A(p));

      DPRINT("Pedal %2d   Tip Pin %2d Value %4d    Ring Pin %2d Value %4d", p + 1, PIN_D(p), tip, PIN_A(p), ring);

      switch (tip) {
        case 0:
          // tip connected to GND
          switch (ring) {
            case 0:
              // tip and ring connected to GND
              // switch between tip and ring normally closed
              pedals[p].mode = PED_MOMENTARY1;
              pedals[p].pressMode = PED_PRESS_1;
              pedals[p].invertPolarity = true;
              DPRINT(" MOMENTARY POLARITY-");
              break;
            case ADC_RESOLUTION-1:
              break;
            default:
              break;
          }
          break;
        case ADC_RESOLUTION-1:
          // tip not connected (pull up resistor)
          switch (ring) {
            case 0:
              break;
            case ADC_RESOLUTION-1:
              // tip and ring not connected (pul up resitor)
              // switch between tip and ring normally open
              pedals[p].mode = PED_MOMENTARY1;
              pedals[p].pressMode = PED_PRESS_1;
              DPRINT(" MOMENTARY");
              break;
            default:
              break;
          }
          break;
        default:
          // tip connected connected to a pot
          pedals[p].mode = PED_ANALOG;
          //pedals[p].invertPolarity = true;
          // inititalize continuos calibration
          pedals[p].expZero = ADC_RESOLUTION - 1;
          pedals[p].expMax = 0;
          DPRINT(" ANALOG POLARITY+");
      }
    }
    else {
      DPRINT("Pedal %2d   autosensing disabled", p + 1);
    }
    DPRINT("\n");
  }
}

void ladder_config()
{
  for (byte p = 0; p < PEDALS; p++) {
    if (pedals[p].mode == PED_LADDER) {
      ResponsiveAnalogRead analog(PIN_A(p), true);

      analogReadResolution(ADC_RESOLUTION_BITS);
      analogSetAttenuation(ADC_11db);
      analog.setAnalogResolution(ADC_RESOLUTION);
      analog.enableEdgeSnap();
      pinMode(PIN_D(p), OUTPUT);
      digitalWrite(PIN_D(p), HIGH);

      for (byte i = 0; i < LADDER_STEPS; i++) {
        display_progress_bar_title2("Press and hold", "Switch " + String(i+1));
        for (byte j = 0; j < i; j++)
          display_progress_bar_2_label(j + 1, 128 * ab[j].threshold / ADC_RESOLUTION);
        unsigned long start = millis();
        while (millis() - start < 3000) {
          display_progress_bar_update(3000 - (millis() - start), 3000);
          analog.update();
          if (analog.hasChanged()) display_progress_bar_2_update(analog.getValue(), ADC_RESOLUTION);
        }
        if (analog.getValue() != ADC_RESOLUTION-1) {
          kt[i].adcThreshold = analog.getValue();
          kt[i].value = i;
          ab[i].threshold = analog.getValue();
          ab[i].id = i + 1;
        }
      }
      display_progress_bar_2_label(LADDER_STEPS, 128 * kt[LADDER_STEPS-1].adcThreshold / ADC_RESOLUTION);
      delay(1000);

      for (byte i = 0; i < LADDER_STEPS; i++) {
        switch (i) {
          case 0:
            kt[0].adcTolerance = constrain(abs((kt[1].adcThreshold - kt[0].adcThreshold) / 2 - 1), 0 , 255);
            ab[0].tolerance = constrain(abs((ab[1].threshold - ab[0].threshold) / 2 - 1), 0 , 255);
            break;
          case LADDER_STEPS - 1:
            kt[i].adcTolerance = constrain(abs((kt[i].adcThreshold - kt[i-1].adcThreshold) / 2 - 1), 0, 255);
            ab[i].tolerance = constrain(abs((ab[i].threshold - ab[i-1].threshold) / 2 - 1), 0, 255);
            break;
          default:
            kt[i].adcTolerance = constrain(min(abs((kt[i].adcThreshold   - kt[i-1].adcThreshold) / 2 - 1),
                                               abs((kt[i+1].adcThreshold - kt[i].adcThreshold) / 2 - 1)), 0, 255);
            ab[i].tolerance = constrain(min(abs((ab[i].threshold   - ab[i-1].threshold) / 2 - 1),
                                            abs((ab[i+1].threshold - ab[i].threshold) / 2 - 1)), 0, 255);
            break;
        }
      }
      eeprom_update_ladder();
      //eeprom_update_profile();
      break;
    }
  }
}

byte map_digital(byte p, byte value)
{
  p = constrain(p, 0, PEDALS - 1);
  return value;
}

unsigned int map_analog(byte p, unsigned int value)
{
  p = constrain(p, 0, PEDALS - 1);
  value = constrain(value, pedals[p].expZero, pedals[p].expMax);                  // make sure that the analog value is between the minimum and maximum value
  value = map(value, pedals[p].expZero, pedals[p].expMax, 0, ADC_RESOLUTION - 1); // map the value from [minimumValue, maximumValue] to [0, ADC_RESOLUTION-1]
  switch (pedals[p].mapFunction) {
    case PED_LINEAR:
      break;
    case PED_LOG:
      value = round((ADC_RESOLUTION-1)*log(value+1)/log(ADC_RESOLUTION));
      break;
    case PED_ANTILOG:
      value = round((exp(3*value/(double)(ADC_RESOLUTION-1))-1)/(exp(3)-1)*(ADC_RESOLUTION-1));
      break;
  }
  return value;
}

void footswitch_update(byte p, byte f)
{
  pedals[p].footSwitch[f]->setPressTime(pressTime);
  pedals[p].footSwitch[f]->setDoublePressTime(doublePressTime);
  pedals[p].footSwitch[f]->setLongPressTime(longPressTime);
  pedals[p].footSwitch[f]->setRepeatTime(repeatPressTime);
  switch (pedals[p].pressMode) {
    case PED_PRESS_1:
      pedals[p].footSwitch[f]->enableDoublePress(false);
      pedals[p].footSwitch[f]->enableLongPress(false);
      break;
    case PED_PRESS_2:
    case PED_PRESS_1_2:
      pedals[p].footSwitch[f]->enableDoublePress(true);
      pedals[p].footSwitch[f]->enableLongPress(false);
      break;
    case PED_PRESS_L:
      case PED_PRESS_1_L:
      pedals[p].footSwitch[f]->enableDoublePress(false);
      pedals[p].footSwitch[f]->enableLongPress(true);
      break;
    case PED_PRESS_1_2_L:
    case PED_PRESS_2_L:
      pedals[p].footSwitch[f]->enableDoublePress(true);
      pedals[p].footSwitch[f]->enableLongPress(true);
      break;
  }
  pedals[p].footSwitch[f]->enableRepeat(pedals[p].function != PED_MIDI);
}

void bank_update (byte b, byte p, int d = 1, bool enable = true)
{
  if (enable) {
    byte m = banks[b][p].midiMessage;
    switch (d) {
      case -1:
        // Invert direction
        switch (m) {
          case PED_BANK_SELECT_INC:
            m = PED_BANK_SELECT_DEC;
            break;
          case PED_BANK_SELECT_DEC:
            m = PED_BANK_SELECT_INC;
            break;
          case PED_PROGRAM_CHANGE_INC:
            m = PED_PROGRAM_CHANGE_DEC;
            break;
          case PED_PROGRAM_CHANGE_DEC:
            m = PED_PROGRAM_CHANGE_INC;
            break;
        }
      case 1:
        switch (m) {

          case PED_BANK_SELECT_INC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue3) banks[b][p].midiValue2 = banks[b][p].midiValue1;
            else banks[b][p].midiValue2++;
            banks[b][p].midiValue2 = constrain(banks[b][p].midiValue2, 0, MIDI_RESOLUTION - 1);
            banks[b][p].midiCode = banks[b][p].midiValue2;
            break;

          case PED_BANK_SELECT_DEC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue1) banks[b][p].midiValue2 = banks[b][p].midiValue3;
            else banks[b][p].midiValue2--;
            banks[b][p].midiValue2 = constrain(banks[b][p].midiValue2, 0, MIDI_RESOLUTION - 1);
            banks[b][p].midiCode = banks[b][p].midiValue2;
            break;

          case PED_PROGRAM_CHANGE_INC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue3) banks[b][p].midiValue2 = banks[b][p].midiValue1;
            else banks[b][p].midiValue2++;
            banks[b][p].midiValue2 = constrain(banks[b][p].midiValue2, 0, MIDI_RESOLUTION - 1);
            banks[b][p].midiCode = banks[b][p].midiValue2;
            for (byte i = 0; i < BANKS; i++) {
              for (byte j = 0; j < PEDALS; j++) {
                if (banks[i][j].midiChannel == banks[b][p].midiChannel &&
                   (banks[i][j].midiMessage == PED_PROGRAM_CHANGE_INC || banks[i][j].midiMessage == PED_PROGRAM_CHANGE_DEC)) {
                  banks[i][j].midiValue2 = banks[b][p].midiValue2;
                  banks[i][j].midiCode = banks[b][p].midiValue2;
                }
              }
            }
            break;

          case PED_PROGRAM_CHANGE_DEC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue1) banks[b][p].midiValue2 = banks[b][p].midiValue3;
            else banks[b][p].midiValue2--;
            banks[b][p].midiValue2 = constrain(banks[b][p].midiValue2, 0, MIDI_RESOLUTION - 1);
            banks[b][p].midiCode = banks[b][p].midiValue2;
            for (byte i = 0; i < BANKS; i++) {
              for (byte j = 0; j < PEDALS; j++) {
                if (banks[i][j].midiChannel == banks[b][p].midiChannel &&
                   (banks[i][j].midiMessage == PED_PROGRAM_CHANGE_INC || banks[i][j].midiMessage == PED_PROGRAM_CHANGE_DEC)) {
                  banks[i][j].midiValue2 = banks[b][p].midiValue2;
                  banks[i][j].midiCode = banks[b][p].midiValue2;
                }
              }
            }

            break;
        }
        break;

      case 0:
        // Bottom value
        switch (m) {
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            banks[b][p].midiCode = banks[b][p].midiValue1;
          case PED_BANK_SELECT_INC:
          case PED_BANK_SELECT_DEC:
            //banks[b][p].midiValue2 = banks[b][p].midiValue1;
            banks[b][p].midiCode = banks[b][p].midiValue1;
            break;
        }
        break;

      case 2:
        // Top value
        switch (m) {
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            banks[b][p].midiCode = banks[b][p].midiValue3;
          case PED_BANK_SELECT_INC:
          case PED_BANK_SELECT_DEC:
            //banks[b][p].midiValue2 = banks[b][p].midiValue3;
            banks[b][p].midiCode = banks[b][p].midiValue3;
            break;
        }
        break;
    }
  }
}

void midi_send(byte message, byte code, byte value, byte channel, bool on_off, byte range_min, byte range_max, byte bank, byte pedal)
{
  switch (message) {

    case PED_NOTE_ON_OFF:

      if (on_off && value > 0) {
        DPRINT("NOTE ON.....Note %3d.....Velocity %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOn(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOn(code, value, channel);
        AppleMidiSendNoteOn(code, value, channel);
        ipMIDISendNoteOn(code, value, channel);
        BLESendNoteOn(code, value, channel);
        OSCSendNoteOn(code, value, channel);
        screen_info(midi::NoteOn, code, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = value;
        lastMIDIMessage[currentBank] = {PED_NOTE_ON_OFF, code, value, channel};
      }
      else {
        DPRINT("NOTE OFF....Note %3d......Velocity %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOff(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOff(code, value, channel);
        AppleMidiSendNoteOff(code, value, channel);
        ipMIDISendNoteOff(code, value, channel);
        BLESendNoteOff(code, value, channel);
        OSCSendNoteOff(code, value, channel);
        screen_info(midi::NoteOff, code, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = value;
        lastMIDIMessage[currentBank] = {PED_NOTE_ON_OFF, code, value, channel};
      }
      break;

    case PED_CONTROL_CHANGE:

      if (on_off) {
        DPRINT("CONTROL CHANGE.....Code %3d......Value %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(code, value, channel);
        AppleMidiSendControlChange(code, value, channel);
        ipMIDISendControlChange(code, value, channel);
        BLESendControlChange(code, value, channel);
        OSCSendControlChange(code, value, channel);
        screen_info(midi::ControlChange, code, value, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = value;
        lastMIDIMessage[currentBank] = {PED_CONTROL_CHANGE, code, value, channel};
      }
      break;

    case PED_PROGRAM_CHANGE:
    case PED_PROGRAM_CHANGE_INC:
    case PED_PROGRAM_CHANGE_DEC:

      if (on_off) {
        DPRINT("PROGRAM CHANGE.....Program %3d.....Channel %2d\n", code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendProgramChange(code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendProgramChange(code, channel);
        AppleMidiSendProgramChange(code, channel);
        ipMIDISendProgramChange(code, channel);
        BLESendProgramChange(code, channel);
        OSCSendProgramChange(code, channel);
        screen_info(midi::ProgramChange, code, 0, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = value;
        lastMIDIMessage[currentBank] = {PED_PROGRAM_CHANGE, code, 0, channel};
      }
      break;

    case PED_BANK_SELECT_INC:
    case PED_BANK_SELECT_DEC:

      if (on_off) {
        // MSB
        DPRINT("CONTROL CHANGE.....Code %3d.....Value %3d.....Channel %2d\n", midi::BankSelect, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect, value, channel);
        AppleMidiSendControlChange(midi::BankSelect, value, channel);
        ipMIDISendControlChange(midi::BankSelect, value, channel);
        BLESendControlChange(midi::BankSelect, value, channel);
        OSCSendControlChange(midi::BankSelect, value, channel);
        // LSB
        DPRINT("CONTROL CHANGE.....Code %3d.....Value %3d.....Channel %2d\n", midi::BankSelect+32, code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect+32, code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect+32, code, channel);
        AppleMidiSendControlChange(midi::BankSelect+32, code, channel);
        ipMIDISendControlChange(midi::BankSelect+32, code, channel);
        BLESendControlChange(midi::BankSelect+32, code, channel);
        OSCSendControlChange(midi::BankSelect+32, code, channel);
        screen_info(midi::ControlChange, midi::BankSelect+32, code, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = code;
      }
      break;

    case PED_PITCH_BEND:

      if (on_off) {
        int bend = map(value, 0, MIDI_RESOLUTION-1, MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MAX);
        DPRINT("PITCH BEND.....Value %5d.....Channel %2d\n", bend, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendPitchBend(bend, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendPitchBend(bend, channel);
        AppleMidiSendPitchBend(bend, channel);
        ipMIDISendPitchBend(bend, channel);
        BLESendPitchBend(bend, channel);
        OSCSendPitchBend(bend, channel);
        const unsigned ubend = unsigned(bend - int(MIDI_PITCHBEND_MIN));
        screen_info(midi::PitchBend, ubend & 0x7f, (ubend >> 7) & 0x7f, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = value;
      }
      break;

    case PED_CHANNEL_PRESSURE:

      if (on_off) {
        DPRINT("CHANNEL PRESSURE.....Channel %2d\n", channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendAfterTouch(value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendAfterTouch(value, channel);
        AppleMidiSendAfterTouch(value, channel);
        ipMIDISendAfterTouch(value, channel);
        BLESendAfterTouch(value, channel);
        OSCSendAfterTouch(value, channel);
        screen_info(midi::AfterTouchChannel, value, 0, channel, range_min, range_max);
        currentMIDIValue[bank][pedal] = value;
      }
      break;

    case PED_MIDI_START:

      if (on_off) {
        DPRINT("START.....\n");
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendRealTime(midi::Start);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendRealTime(midi::Start);
        AppleMidiSendStart();
        ipMIDISendStart();
        BLESendStart();
        OSCSendStart();
        screen_info(midi::Start, 0, 0, 0, range_min, range_max);
        currentMIDIValue[bank][pedal] = 0;
      }
      break;

    case PED_MIDI_STOP:

      if (on_off) {
        DPRINT("STOP.....\n");
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendRealTime(midi::Stop);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendRealTime(midi::Stop);
        AppleMidiSendStop();
        ipMIDISendStop();
        BLESendStop();
        OSCSendStop();
        screen_info(midi::Stop, 0, 0, 0, range_min, range_max);
        currentMIDIValue[bank][pedal] = 0;
      }
      break;

    case PED_MIDI_CONTINUE:

      if (on_off) {
        DPRINT("CONTINUE.....\n");
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendRealTime(midi::Continue);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendRealTime(midi::Continue);
        AppleMidiSendContinue();
        ipMIDISendContinue();
        BLESendContinue();
        OSCSendContinue();
        screen_info(midi::Continue, 0, 0, 0, range_min, range_max);
        currentMIDIValue[bank][pedal] = 0;
      }
      break;

    case PED_SEQUENCE:
      channel = constrain(channel, 1, SEQUENCES);
      DPRINT("=======================================================\n");
      DPRINT("SEQUENCE.....Number %2d\n", channel);
      DPRINT("-------------------------------------------------------\n");
      for (byte s = 0; s < STEPS; s++)
        if (sequences[channel-1][s].midiMessage == PED_CONTROL_CHANGE)
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, value, sequences[channel-1][s].midiChannel, on_off, range_min, range_max, bank, pedal);
        else if (on_off)
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, sequences[channel-1][s].midiValue1, sequences[channel-1][s].midiChannel, on_off, range_min, range_max, bank, pedal);
        else
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, sequences[channel-1][s].midiValue2, sequences[channel-1][s].midiChannel, on_off, range_min, range_max, bank, pedal);
      DPRINT("=======================================================\n");
      currentMIDIValue[bank][pedal] = channel;
      lastMIDIMessage[currentBank] = {PED_SEQUENCE, code, value, channel};
      break;
  }
}

void refresh_switch_1_midi(byte i, bool send)
{
  bool         state1, state2;
  unsigned int input;
  unsigned int value;
  byte         b;

  state1 = false;
  state2 = false;
  if (pedals[i].debouncer[0] != nullptr) state1 = pedals[i].debouncer[0]->update();
  if (pedals[i].debouncer[1] != nullptr) state2 = pedals[i].debouncer[1]->update();

  if (state1 && state2) {                                                     // pin state changed
    input = pedals[i].debouncer[0]->read();                                   // reads the updated pin state
    if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;        // invert the value
    value = map_digital(i, input);                                            // apply the digital map function to the value

    DPRINT("Pedal %2d   input %d output %d\n", i + 1, input, value);

    b = (currentBank + 2) % BANKS;
    bank_update(b, i);
    switch (value) {
      case LOW:   // LOW = pressed
        if (send) midi_send(banks[b][i].midiMessage,
                            banks[b][i].midiCode,
                            banks[b][i].midiMessage == PED_CONTROL_CHANGE ?
                                  (leds.get(i) ? banks[b][i].midiValue2 : banks[b][i].midiValue1) : banks[b][i].midiValue1,
                            banks[b][i].midiChannel,
                            true,
                            banks[b][i].midiValue1,
                            banks[b][i].midiValue3,
                            b, i);
        break;
      case HIGH:  // HIGH = release
        if (send) midi_send(banks[b][i].midiMessage,
                            banks[b][i].midiCode,
                            banks[b][i].midiValue2,
                            banks[b][i].midiChannel,
                            pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2,
                            banks[b][i].midiValue1,
                            banks[b][i].midiValue3,
                            b, i);
        break;
    }
    pedals[i].pedalValue[0] = value;
    pedals[i].lastUpdate[0] = millis();
    pedals[i].pedalValue[1] = pedals[i].pedalValue[0];
    pedals[i].lastUpdate[1] = pedals[i].lastUpdate[0];
    lastUsedSwitch = i;
    lastUsed = i;
    strncpy(lastPedalName, banks[b][i].pedalName, MAXPEDALNAME+1);
  }
  else {
    if (state1) {                                                             // pin state changed
      input = pedals[i].debouncer[0]->read();                                 // reads the updated pin state
      if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;      // invert the value
      value = map_digital(i, input);                                          // apply the digital map function to the value

      DPRINT("Pedal %2d   input %d output %d\n", i + 1, input, value);

      if (tapDanceMode && tapDanceBank && value == LOW && send) {
        currentBank = constrain(i, 0, BANKS - 1);
        screen_info(midi::InvalidType, currentBank+1, 0, 0);
        pedals[i].pedalValue[0] = value;
        pedals[i].lastUpdate[0] = millis();
        lastUsedSwitch = i;
        lastUsed = i;
        strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXPEDALNAME+1);
        DPRINT("Bank %d\n", currentBank + 1);
        return;
      }
      if (tapDanceMode && tapDanceBank && value == HIGH && send) {
        tapDanceBank = false;
        return;
      }

      b = currentBank;
      switch (value) {
        case LOW:   // LOW = pressed
          if (send) {
            bank_update(b, i);
            midi_send(banks[b][i].midiMessage,
                      banks[b][i].midiCode,
                      banks[b][i].midiMessage == PED_CONTROL_CHANGE ?
                            (leds.get(i) ? banks[b][i].midiValue2 : banks[b][i].midiValue1) : banks[b][i].midiValue1,
                      banks[b][i].midiChannel,
                      true,
                      banks[b][i].midiValue1,
                      banks[b][i].midiValue3,
                      b, i);
            leds.invert(i);
            leds.write();
#ifdef BLYNK
            if (blynk_cloud_connected()) Blynk.virtualWrite(71 + i, leds.get(i) ? 1 : 0);
#endif
          }
          break;
        case HIGH:  // HIGH = release
          if (send) {
            tapDanceBank = true;
            bool latch = pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2;
            bank_update(banks[b][i].midiMessage, b, i, latch);
            midi_send(banks[b][i].midiMessage,
                      banks[b][i].midiCode,
                      banks[b][i].midiValue2,
                      banks[b][i].midiChannel,
                      latch,
                      banks[b][i].midiValue1,
                      banks[b][i].midiValue3,
                      b, i);
            if (latch) {
              leds.invert(i);
              leds.write();
#ifdef BLYNK
              if (blynk_cloud_connected()) Blynk.virtualWrite(71 + i, leds.get(i) ? 1 : 0);
#endif
            }

          }
          break;
      }
      pedals[i].pedalValue[0] = value;
      pedals[i].lastUpdate[0] = millis();
      lastUsedSwitch = i;
      lastUsed = i;
      strncpy(lastPedalName, banks[b][i].pedalName, MAXPEDALNAME+1);
    }
    if (state2) {                                                             // pin state changed
      input = pedals[i].debouncer[1]->read();                                 // reads the updated pin state
      if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;      // invert the value
      value = map_digital(i, input);                                          // apply the digital map function to the value

      DPRINT("Pedal %2d   input %d output %d\n", i + 1, input, value);

      b = (currentBank + 1) % BANKS;
      bank_update(b, i);
      switch (value) {
        case LOW:   // LOW = pressed
          if (send) midi_send(banks[b][i].midiMessage,
                              banks[b][i].midiCode,
                              banks[b][i].midiMessage == PED_CONTROL_CHANGE ?
                                    (leds.get(i) ? banks[b][i].midiValue2 : banks[b][i].midiValue1) : banks[b][i].midiValue1,
                              banks[b][i].midiChannel,
                              true,
                              banks[b][i].midiValue1,
                              banks[b][i].midiValue3,
                              b, i);
          break;
        case HIGH:  // HIGH = release
          if (send) midi_send(banks[b][i].midiMessage,
                              banks[b][i].midiCode,
                              banks[b][i].midiValue2,
                              banks[b][i].midiChannel,
                              pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2,
                              banks[b][i].midiValue1,
                              banks[b][i].midiValue3,
                              b, i);
          break;
      }
      pedals[i].pedalValue[1] = value;
      pedals[i].lastUpdate[1] = millis();
      lastUsedSwitch = i;
      lastUsed = i;
      strncpy(lastPedalName, banks[b][i].pedalName, MAXPEDALNAME+1);
    }
  }
}

void refresh_switch_12L_midi(byte i, bool send)
{
  MD_UISwitch::keyResult_t k1;    // Close status between T and S
  MD_UISwitch::keyResult_t k2;    // Close status between R and S
  MD_UISwitch::keyResult_t k3;
  byte                     k;     /*       k1      k2
                                     0 =  Open    Open
                                     1 = Closed   Open
                                     2 =  Open   Closed
                                     3 = Closed  Closed */

  byte                     b;

  if (pedals[i].mode     == PED_NONE)   return;
  if (pedals[i].mode     == PED_ANALOG) return;
  if (pedals[i].mode     == PED_LATCH1) return;
  if (pedals[i].mode     == PED_LATCH2) return;
  if (pedals[i].function != PED_MIDI)   return;

  switch (pedals[i].mode) {
    case PED_LADDER:
      pedals[i].pedalValue[0] = 0;
      pedals[i].pedalValue[1] = 0;
      break;
    default:
      pedals[i].pedalValue[0] = digitalRead(PIN_D(i));
      pedals[i].pedalValue[1] = digitalRead(PIN_A(i));
  }

  k = 0;
  k1 = MD_UISwitch::KEY_NULL;
  k2 = MD_UISwitch::KEY_NULL;
  if (pedals[i].footSwitch[0] != nullptr)
    k1 = pedals[i].footSwitch[0]->read();
  if (pedals[i].footSwitch[1] != nullptr && (pedals[i].mode == PED_MOMENTARY2 ||
                                             pedals[i].mode == PED_MOMENTARY3 ||
                                             pedals[i].mode == PED_LATCH2))
    k2 = pedals[i].footSwitch[1]->read();
  if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) && k2 == MD_UISwitch::KEY_NULL) k = 1;
  if ((k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS) && k1 == MD_UISwitch::KEY_NULL) k = 2;
  if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) &&
      (k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS)) k = 3;

  b = currentBank;

  if (pedals[i].mode == PED_LADDER) {
    if (k1 == MD_UISwitch::KEY_PRESS) {
      b = (b + pedals[i].footSwitch[0]->getKey()) % BANKS;
    }
    else return;
  }

  switch (k) {
    case 0:
      k3 = MD_UISwitch::KEY_NULL;
      break;
    case 1:
      k3 = k1;
      break;
    case 2:
    case 3:
      k3 = k2;
      break;
   }

    switch (k3) {

      case MD_UISwitch::KEY_PRESS:

        DPRINT("Pedal %2d   SINGLE PRESS \n", i + 1);
        if ((pedals[i].pressMode & PED_PRESS_1) != PED_PRESS_1) break;
        switch (banks[b][i].midiMessage) {
          case PED_BANK_SELECT_INC:
          case PED_BANK_SELECT_DEC:
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            bank_update(b, i, 1);   // Increase
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;

          case PED_PROGRAM_CHANGE:
            b = (b + k - 1) % BANKS;
            if (send) midi_send(banks[b][i].midiMessage,  banks[b][i].midiValue1, banks[b][i].midiValue1, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;

          default:
            b = (b + k - 1) % BANKS;
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue1, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b , i);
            break;
        }
        lastUsedSwitch = i;
        lastUsed = i;
        strncpy(lastPedalName, banks[b][i].pedalName, MAXPEDALNAME+1);
        break;

      case MD_UISwitch::KEY_DPRESS:

        DPRINT("Pedal %2d   DOUBLE PRESS \n", i + 1);
        if ((pedals[i].pressMode & PED_PRESS_2) != PED_PRESS_2) break;
        switch (banks[b][i].midiMessage) {
          case PED_BANK_SELECT_INC:
          case PED_BANK_SELECT_DEC:
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            bank_update(b, i, -1);    // Decrease
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;

          case PED_PROGRAM_CHANGE:
            b = (b + k - 1) % BANKS;
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiValue2, banks[b][i].midiValue2, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;

          default:
            b = (b + k - 1) % BANKS;
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;
        }
        lastUsedSwitch = i;
        strncpy(lastPedalName, banks[b][i].pedalName, MAXPEDALNAME+1);
        break;

      case MD_UISwitch::KEY_LONGPRESS:

        DPRINT("Pedal %2d   LONG   PRESS \n", i + 1);
        if ((pedals[i].pressMode & PED_PRESS_L) != PED_PRESS_L) break;
        switch (banks[b][i].midiMessage) {
          case PED_BANK_SELECT_INC:
          case PED_PROGRAM_CHANGE_INC:
            bank_update(b, i, 0);   // Set to bottom value
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b ,i);
            break;

          case PED_BANK_SELECT_DEC:
          case PED_PROGRAM_CHANGE_DEC:
            bank_update(b, i, 2);   // Set to top value
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;

          case PED_PROGRAM_CHANGE:
            b = (b + k - 1) % BANKS;
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiValue3, banks[b][i].midiValue3, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;

          default:
            b = (b + k - 1) % BANKS;
            if (send) midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue3, banks[b][i].midiChannel, true, banks[b][i].midiValue1, banks[b][i].midiValue3, b, i);
            break;
        }
        lastUsedSwitch = i;
        lastUsed = i;
        strncpy(lastPedalName, banks[b][i].pedalName, MAXPEDALNAME+1);
        break;

      case MD_UISwitch::KEY_RPTPRESS:
      case MD_UISwitch::KEY_NULL:
      case MD_UISwitch::KEY_DOWN:
      case MD_UISwitch::KEY_UP:
        break;
    }
}


void refresh_switch_12L(byte i)
{
  MD_UISwitch::keyResult_t k1;    // Close status between T and S
  MD_UISwitch::keyResult_t k2;    // Close status between R and S
  byte                     k;     /*       k1      k2
                                     0 =  Open    Open
                                     1 = Closed   Open
                                     2 =  Open   Closed
                                     3 = Closed  Closed */

  byte f, step;

  if (pedals[i].mode     == PED_NONE)   return;
  if (pedals[i].mode     == PED_ANALOG) return;
  if (pedals[i].function == PED_MIDI)   return;

  k = 0;
  k1 = MD_UISwitch::KEY_NULL;
  k2 = MD_UISwitch::KEY_NULL;
  if (pedals[i].footSwitch[0] != nullptr)
    k1 = pedals[i].footSwitch[0]->read();
  if (pedals[i].footSwitch[1] != nullptr && (pedals[i].mode == PED_MOMENTARY2 ||
                                             pedals[i].mode == PED_MOMENTARY3 ||
                                             pedals[i].mode == PED_LATCH2))
    k2 = pedals[i].footSwitch[1]->read();
  if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) && k2 == MD_UISwitch::KEY_NULL) k = 1;
  if ((k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS) && k1 == MD_UISwitch::KEY_NULL) k = 2;
  if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) &&
      (k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS)) k = 3;

  f = pedals[i].function;

  if ((pedals[i].mode == PED_LADDER) && k1 == MD_UISwitch::KEY_PRESS) {
      switch (pedals[i].footSwitch[0]->getKey()) {
            case 0:
              break;

            case 1:
              break;

            case 2:
              break;

            case 3:
              break;

            case 4:
              break;

            case 5:
              break;
        }
    }


  // Double press or long press invert pedal function
  if (k > 0 && (k1 == MD_UISwitch::KEY_DPRESS    || k2 == MD_UISwitch::KEY_DPRESS ||
                k1 == MD_UISwitch::KEY_LONGPRESS || k2 == MD_UISwitch::KEY_LONGPRESS))
    switch (pedals[i].function) {
      case PED_BANK_PLUS:
        f = PED_BANK_MINUS;
        break;
      case PED_BANK_PLUS_2:
        f = PED_BANK_MINUS_2;
        break;
      case PED_BANK_PLUS_3:
        f = PED_BANK_MINUS_3;
        break;
      case PED_BANK_MINUS:
        f = PED_BANK_PLUS;
        break;
      case PED_BANK_MINUS_2:
        f = PED_BANK_PLUS_2;
        break;
      case PED_BANK_MINUS_3:
        f = PED_BANK_PLUS_3;
        break;
    }

  //if (k > 0 && (k1 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_PRESS)) {
    if (k > 0) {
    switch (f) {
      case PED_BANK_PLUS:
      case PED_BANK_PLUS_2:
      case PED_BANK_PLUS_3:
        switch (f) {
          case PED_BANK_PLUS:
            step = 1;
            break;
          case PED_BANK_PLUS_2:
            step = 2;
            break;
          case PED_BANK_PLUS_3:
            step = 3;
            break;
          default:
            step = 0;
            break;
        }
        switch (k) {
          case 1:
            if (currentBank == pedals[i].expMax - 1)
              currentBank = pedals[i].expZero - 1;
            else
              currentBank = currentBank + step;
            break;
          case 2:
            if (currentBank == (pedals[i].expZero - 1))
              currentBank = pedals[i].expMax - 1;
            else
              currentBank = currentBank - step;
            break;
          case 3:
            currentBank = pedals[i].expZero - 1;
            break;
        }
        currentBank = constrain(currentBank, pedals[i].expZero - 1, pedals[i].expMax - 1);
        currentBank = constrain(currentBank, 0, BANKS - 1);
        if (repeatOnBankSwitch)
          midi_send(lastMIDIMessage[currentBank].midiMessage,
                    lastMIDIMessage[currentBank].midiCode,
                    lastMIDIMessage[currentBank].midiValue,
                    lastMIDIMessage[currentBank].midiChannel,
                    true,
                    0, MIDI_RESOLUTION - 1,
                    currentBank, i);
        break;

      case PED_BANK_MINUS:
      case PED_BANK_MINUS_2:
      case PED_BANK_MINUS_3:
        switch (f) {
          case PED_BANK_MINUS:
            step = 1;
            break;
          case PED_BANK_MINUS_2:
            step = 2;
            break;
          case PED_BANK_MINUS_3:
            step = 3;
            break;
          default:
            step = 0;
            break;
        }
        switch (k) {
          case 1:
            if (currentBank == (pedals[i].expZero - 1))
              currentBank = pedals[i].expMax - 1;
            else
              currentBank = currentBank - step;
            break;
          case 2:
            if (currentBank == pedals[i].expMax - 1)
              currentBank = pedals[i].expZero - 1;
            else
              currentBank = currentBank + step;
            break;
          case 3:
            currentBank = pedals[i].expMax - 1;
            break;
        }
        currentBank = constrain(currentBank, pedals[i].expZero - 1, pedals[i].expMax - 1);
        currentBank = constrain(currentBank, 0, BANKS - 1);
        if (repeatOnBankSwitch)
          midi_send(lastMIDIMessage[currentBank].midiMessage,
                    lastMIDIMessage[currentBank].midiCode,
                    lastMIDIMessage[currentBank].midiValue,
                    lastMIDIMessage[currentBank].midiChannel,
                    true,
                    0, MIDI_RESOLUTION - 1,
                    currentBank, i);
        break;

      case PED_START:
        switch (k) {
          case 1:
            mtc_start();
            break;
          case 2:
            mtc_stop();
            break;
          case 3:
            mtc_tap_continue();
            break;
        }
        break;

      case PED_STOP:
        switch (k) {
          case 1:
            mtc_stop();
            break;
          case 2:
            mtc_start();
            break;
          case 3:
            mtc_tap_continue();
            break;
        }
        break;

      case PED_CONTINUE:
        switch (k) {
          case 1:
            mtc_continue();
            break;
          case 2:
            mtc_stop();
            break;
          case 3:
            mtc_tap();
            break;
        }
        break;

      case PED_TAP:
        switch (k) {
          case 1:
            mtc_tap();
            break;
          case 2:
            mtc_start();
            break;
          case 3:
            mtc_stop();
            break;
        }
        break;
    }
  }
}


void refresh_analog(byte i, bool send)
{
  const int SAFE_ZONE = ADC_RESOLUTION / 20;                // 5% of margin at both end of the scale

  int input;
  int value;

  if (pedals[i].analogPedal == nullptr) return;             // sanity check

  input = analogRead(PIN_A(i));                             // read the raw analog input value
  if (pedals[i].autoSensing) {                              // continuos calibration
    if (pedals[i].expZero > (input + SAFE_ZONE)) {
      DPRINT("Pedal %2d calibration min %d\n", i + 1, input);
    }
    if (pedals[i].expMax < (input - SAFE_ZONE)) {
      DPRINT("Pedal %2d calibration max %d\n", i + 1, input);
    }
    pedals[i].expZero = _min(pedals[i].expZero, input + SAFE_ZONE);
    pedals[i].expMax  = _max(pedals[i].expMax,  input - SAFE_ZONE);
    //DPRINT("%d -> [%d, %d]\n", input, pedals[i].expZero, pedals[i].expMax);
  }
  value = map_analog(i, input);                             // expand to [0, ADC_RESOLUTION-1] and apply the map function
  pedals[i].analogPedal->update(value);                     // update the responsive analog average
  if (pedals[i].analogPedal->hasChanged()) {                // if the value changed since last time
    value = pedals[i].analogPedal->getValue();              // get the responsive analog average value
    value = map(value,                                      // map from [0, ADC_RESOLUTION-1] to [min, max] MIDI value
              0,
              ADC_RESOLUTION - 1,
              pedals[i].invertPolarity ? banks[currentBank][i].midiValue3 : banks[currentBank][i].midiValue1,
              pedals[i].invertPolarity ? banks[currentBank][i].midiValue1 : banks[currentBank][i].midiValue3);
    double velocity = (1000.0 * ((int)value - pedals[i].pedalValue[0])) / (micros() - pedals[i].lastUpdate[0]);
    switch (pedals[i].function) {
      case PED_MIDI:
        DPRINT("Pedal %2d   input %d output %d velocity %.2f\n", i + 1, input, value, velocity);
        if (send) {
          switch (banks[currentBank][i].midiMessage) {
            case PED_PROGRAM_CHANGE:
            case PED_PROGRAM_CHANGE_INC:
            case PED_PROGRAM_CHANGE_DEC:
            case PED_BANK_SELECT_INC:
            case PED_BANK_SELECT_DEC:
              midi_send(banks[currentBank][i].midiMessage, value, 0, banks[currentBank][i].midiChannel, true, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
              midi_send(banks[currentBank][i].midiMessage, value, 0, banks[currentBank][i].midiChannel, false, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
              break;
            default:
              midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel, true, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
              midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel, false, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
              break;
          }
        }
        if (send) midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel, true, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
        if (send) midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel, false, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
        pedals[i].pedalValue[0] = value;
        pedals[i].lastUpdate[0] = micros();
        lastUsedPedal = i;
        lastUsed = i;
        strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXPEDALNAME+1);
        break;
      case PED_BPM_PLUS:
      case PED_BPM_MINUS:
        bpm = map(value, 0, MIDI_RESOLUTION, 40, 300);
        MTC.setBpm(bpm);
        break;
    }
  }
 }

void controller_setup();

//
//  Delete controllers
//
void controller_delete()
{
  // Delete previous setup
  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].debouncer[0]  != nullptr) delete pedals[i].debouncer[0];
    if (pedals[i].debouncer[1]  != nullptr) delete pedals[i].debouncer[1];
    if (pedals[i].footSwitch[0] != nullptr) delete pedals[i].footSwitch[0];
    if (pedals[i].footSwitch[1] != nullptr) delete pedals[i].footSwitch[1];
    if (pedals[i].analogPedal   != nullptr) delete pedals[i].analogPedal;
    for (byte s = 0; s < LADDER_STEPS; s++)
      if (pedals[i].button[s]   != nullptr) delete pedals[i].button[s];
    if (pedals[i].buttonConfig  != nullptr) delete pedals[i].buttonConfig;
  }
}

//
//  Refresh pedals
//
void controller_run(bool send = true)
{
  if (saveProfile && send) {
    DPRINT("Saving profile ...\n");
    eeprom_update_current_profile(currentProfile);
    saveProfile = false;
    return;
  }

  if (reloadProfile && send) {
    DPRINT("Loading profile ...\n");
    eeprom_read_profile(currentProfile);
    autosensing_setup();
    controller_setup();
    mtc_setup();
#ifdef WIFI
    OscControllerUpdate();
#endif
    reloadProfile = false;
    return;
  }

if (loadConfig && send) {
    DPRINT("Loading configuration ...\n");
    autosensing_setup();
    controller_setup();
    mtc_setup();
#ifdef WIFI
    OscControllerUpdate();
#endif
    loadConfig = false;
    return;
  }
  for (byte i = 0; i < PEDALS; i++) {
    switch (pedals[i].mode) {

      case PED_MOMENTARY1:
      case PED_LATCH1:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        break;

      case PED_MOMENTARY2:
      case PED_LATCH2:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        if (pedals[i].button[1] != nullptr) pedals[i].button[1]->check();
        break;

      case PED_MOMENTARY3:
        if (pedals[i].button[0] != nullptr) pedals[i].button[0]->check();
        if (pedals[i].button[1] != nullptr) pedals[i].button[1]->check();
        if (pedals[i].button[2] != nullptr) pedals[i].button[2]->check();
        break;

      case PED_ANALOG:
        //refresh_analog(i, send);
        break;

      case PED_LADDER:
        for (byte s = 0; s < LADDER_STEPS; s++)
          if (pedals[i].button[s] != nullptr) pedals[i].button[s]->check();
        break;

      case PED_JOG_WHEEL:
        uint8_t direction = pedals[i].jogwheel->read();
        switch (direction) {
          case DIR_NONE:
            break;
          case DIR_CW:
          case DIR_CCW:
            action *act = actions[currentBank];
            while (act != nullptr) {
              if (act->pedal == i) {
                switch (act->midiMessage) {

                  case PED_EMPTY:
                    break;

                  case PED_PROGRAM_CHANGE:
                  case PED_CONTROL_CHANGE:
                  case PED_NOTE_ON_OFF:
                  case PED_NOTE_OFF:
                  case PED_PITCH_BEND:
                  case PED_CHANNEL_PRESSURE:
                  case PED_BANK_SELECT_INC:
                  case PED_BANK_SELECT_DEC:
                  case PED_PROGRAM_CHANGE_INC:
                  case PED_PROGRAM_CHANGE_DEC:
                  case PED_SEQUENCE:
                    currentMIDIValue[currentBank][i] = constrain(currentMIDIValue[currentBank][i] +
                                                        ((direction == DIR_CW) ? 1 : -1) *
                                                        _max(1, (pedals[i].jogwheel->speed() + 1) * (act->midiValue2 - act->midiValue1) / (MIDI_RESOLUTION - 1)),
                                                        pedals[i].invertPolarity ? act->midiValue2 : act->midiValue1,
                                                        pedals[i].invertPolarity ? act->midiValue1 : act->midiValue2);
                    DPRINT("Pedal %2d   input %d output %d velocity %d\n", i + 1, ((direction == DIR_CW) ? 1 : -1), currentMIDIValue[currentBank][i], pedals[i].jogwheel->speed());
                    if (send) {
                      switch (act->midiMessage) {
                        case PED_PROGRAM_CHANGE:
                        case PED_PROGRAM_CHANGE_INC:
                        case PED_PROGRAM_CHANGE_DEC:
                        case PED_BANK_SELECT_INC:
                        case PED_BANK_SELECT_DEC:
                          midi_send(act->midiMessage, currentMIDIValue[currentBank][i], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, i);
                          midi_send(act->midiMessage, currentMIDIValue[currentBank][i], 0, act->midiChannel, false, act->midiValue1, act->midiValue2, currentBank, i);
                          break;
                        default:
                          midi_send(act->midiMessage, act->midiCode, currentMIDIValue[currentBank][i], act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, i);
                          midi_send(act->midiMessage, act->midiCode, currentMIDIValue[currentBank][i], act->midiChannel, false, act->midiValue1, act->midiValue2, currentBank, i);
                          break;
                      }
                    }
                    pedals[i].lastUpdate[0] = micros();
                    lastUsedPedal = i;
                    lastUsed = i;
                    strncpy(lastPedalName, act->name, MAXACTIONNAME+1);
                    break;

                  case PED_BANK_PLUS:
                  case PED_BANK_MINUS:
                    {
                      int b = currentBank + ((direction == DIR_CW) ? 1 : -1) * (pedals[i].invertPolarity ? -1 : 1);
                      b = constrain(b, pedals[i].expZero - 1, pedals[i].expMax - 1);
                      currentBank = constrain(b, 0, BANKS - 1);
                      break;
                    }
                  case PED_BPM_PLUS:
                  case PED_BPM_MINUS:
                    bpm = constrain(bpm + ((direction == DIR_CW) ? 1 : -1) * (pedals[i].jogwheel->speed() + 1),
                                    pedals[i].invertPolarity ? 300 : 40,
                                    pedals[i].invertPolarity ? 40 : 300);
                    MTC.setBpm(bpm);
                    break;
                }
                DPRINT("Action: %s\n", act->name);
              }
              act = act->next;
            }
            break;
        }
        break;
    }
  }

  return;

  for (byte i = 0; i < PEDALS; i++) {
    switch (pedals[i].mode) {

      case PED_MOMENTARY1:
      case PED_MOMENTARY2:
      case PED_MOMENTARY3:
      case PED_LATCH1:
      case PED_LATCH2:

        switch (pedals[i].pressMode) {

          case PED_PRESS_1:
            if (pedals[i].function == PED_MIDI) refresh_switch_1_midi(i, send);
            else refresh_switch_12L(i);
            break;

          case PED_PRESS_1_2:
          case PED_PRESS_1_L:
          case PED_PRESS_1_2_L:
          case PED_PRESS_2:
          case PED_PRESS_2_L:
          case PED_PRESS_L:
            if (pedals[i].function == PED_MIDI) refresh_switch_12L_midi(i, send);
            else refresh_switch_12L(i);
            break;
        }
        break;

      case PED_ANALOG:
        refresh_analog(i, send);
        break;

      case PED_LADDER:
        if (pedals[i].function == PED_MIDI) refresh_switch_12L_midi(i, send);
        else refresh_switch_12L(i);
        break;

      case PED_JOG_WHEEL:
        uint8_t direction = pedals[i].jogwheel->read();
        switch (direction) {
          case DIR_NONE:
            break;
          case DIR_CW:
          case DIR_CCW:
            switch (pedals[i].function) {
              case PED_MIDI:
                currentMIDIValue[currentBank][i] = constrain(currentMIDIValue[currentBank][i] +
                                                    ((direction == DIR_CW) ? 1 : -1) *
                                                    _max(1, (pedals[i].jogwheel->speed() + 1) * (banks[currentBank][i].midiValue3 - banks[currentBank][i].midiValue1) / (MIDI_RESOLUTION - 1)),
                                                    pedals[i].invertPolarity ? banks[currentBank][i].midiValue3 : banks[currentBank][i].midiValue1,
                                                    pedals[i].invertPolarity ? banks[currentBank][i].midiValue1 : banks[currentBank][i].midiValue3);
                DPRINT("Pedal %2d   input %d output %d velocity %d\n", i + 1, ((direction == DIR_CW) ? 1 : -1), currentMIDIValue[currentBank][i], pedals[i].jogwheel->speed());
                if (send) {
                  switch (banks[currentBank][i].midiMessage) {
                    case PED_PROGRAM_CHANGE:
                    case PED_PROGRAM_CHANGE_INC:
                    case PED_PROGRAM_CHANGE_DEC:
                    case PED_BANK_SELECT_INC:
                    case PED_BANK_SELECT_DEC:
                      midi_send(banks[currentBank][i].midiMessage, currentMIDIValue[currentBank][i], 0, banks[currentBank][i].midiChannel, true, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
                      midi_send(banks[currentBank][i].midiMessage, currentMIDIValue[currentBank][i], 0, banks[currentBank][i].midiChannel, false, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
                      break;
                    default:
                      midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, currentMIDIValue[currentBank][i], banks[currentBank][i].midiChannel, true, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
                      midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, currentMIDIValue[currentBank][i], banks[currentBank][i].midiChannel, false, banks[currentBank][i].midiValue1, banks[currentBank][i].midiValue3, currentBank, i);
                      break;
                  }
                }
                pedals[i].lastUpdate[0] = micros();
                lastUsedPedal = i;
                lastUsed = i;
                strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXPEDALNAME+1);
                break;
              case PED_BANK_PLUS:
              case PED_BANK_MINUS:
                {
                  int b = currentBank + ((direction == DIR_CW) ? 1 : -1) * (pedals[i].invertPolarity ? -1 : 1);
                  b = constrain(b, pedals[i].expZero - 1, pedals[i].expMax - 1);
                  currentBank = constrain(b, 0, BANKS - 1);
                  break;
                }
              case PED_BPM_PLUS:
              case PED_BPM_MINUS:
                bpm = constrain(bpm + ((direction == DIR_CW) ? 1 : -1) * (pedals[i].jogwheel->speed() + 1),
                                pedals[i].invertPolarity ? 300 : 40,
                                pedals[i].invertPolarity ? 40 : 300);
                MTC.setBpm(bpm);
                break;
            }
            break;
        }
        break;
    }
  }
}

//
// Trigger Actions on Buttons Events
//
void controller_event_handler_button(AceButton* button, uint8_t eventType, uint8_t buttonState)
{
  byte p = constrain(button->getId() / 10 - 1, 0, PEDALS - 1);
  byte i = constrain(button->getId() % 10 - 1, 0, LADDER_STEPS - 1);

  DPRINT("Pedal: %d     Button: %d    EventType: %d     ButtonState: %d\n", p, i, eventType, buttonState);

  if ((pedals[p].function != PED_NONE) && (eventType != AceButton::kEventPressed) && (eventType != AceButton::kEventRepeatPressed)) return;

  switch (pedals[p].function) {

    case PED_NONE: {
      action *act = actions[currentBank];
      while (act != nullptr) {
        if ((act->pedal == p) && (act->button == i) && (act->event == eventType)) {
          DPRINT("Action: %s\n", act->name);
          switch (act->midiMessage) {
            case PED_EMPTY:
              break;

            case PED_PROGRAM_CHANGE:
            case PED_CONTROL_CHANGE:
            case PED_NOTE_ON_OFF:
            case PED_PITCH_BEND:
            case PED_CHANNEL_PRESSURE:
            case PED_MIDI_START:
            case PED_MIDI_STOP:
            case PED_MIDI_CONTINUE:
            case PED_SEQUENCE:
              midi_send(act->midiMessage, act->midiCode, act->midiValue1, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, currentPedal);
              break;

            case PED_BANK_SELECT_INC:
              lastBankSelect[act->midiChannel] = constrain(lastBankSelect[act->midiChannel] + 1, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, (lastBankSelect[act->midiChannel] & 0b0011111110000000) >> 7, lastBankSelect[act->midiChannel] & 0b0000000001111111, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, currentPedal);
              break;

            case PED_BANK_SELECT_DEC:
              lastBankSelect[act->midiChannel] = constrain(lastBankSelect[act->midiChannel] - 1, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, (lastBankSelect[act->midiChannel] & 0b0011111110000000) >> 7, lastBankSelect[act->midiChannel] & 0b0000000001111111, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, currentPedal);
              break;

            case PED_PROGRAM_CHANGE_INC:
              lastProgramChange[act->midiChannel] = constrain(lastProgramChange[act->midiChannel] + 1, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, lastProgramChange[act->midiChannel], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, currentPedal);
              break;

            case PED_PROGRAM_CHANGE_DEC:
              lastProgramChange[act->midiChannel] = constrain(lastProgramChange[act->midiChannel] - 1, act->midiValue1, act->midiValue2);
              midi_send(act->midiMessage, lastProgramChange[act->midiChannel], 0, act->midiChannel, true, act->midiValue1, act->midiValue2, currentBank, currentPedal);
              break;

            case PED_ACTION_BANK_PLUS:
              currentBank = constrain(currentBank + 1, constrain(act->midiValue1 - 1, 0, BANKS - 1), constrain(act->midiValue2 - 1, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, act->pedal);
              break;

            case PED_ACTION_BANK_MINUS:
              currentBank = constrain(currentBank - 1, constrain(act->midiValue1 - 1, 0, BANKS - 1), constrain(act->midiValue2 - 1, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, act->pedal);
              break;

            case PED_ACTION_START:
              mtc_start();
              break;

            case PED_ACTION_STOP:
              mtc_stop();
              break;

            case PED_ACTION_CONTINUE:
              mtc_continue();
              break;

            case PED_ACTION_TAP:
              mtc_tap();
              break;

            case PED_ACTION_BPM_PLUS:
              bpm = constrain(bpm + 1, 40, 300);
              MTC.setBpm(bpm);
              break;

            case PED_ACTION_BPM_MINUS:
              bpm = constrain(bpm - 1, 40, 300);
              MTC.setBpm(bpm);
              break;
          }
        }
        act = act->next;
      }
      break;
    }

    case PED_BANK_PLUS:
              currentBank = constrain(currentBank + 1, constrain(pedals[p].expZero - 1, 0, BANKS - 1), constrain(pedals[p].expMax - 1, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, p);
              break;

    case PED_BANK_MINUS:
              currentBank = constrain(currentBank - 1, constrain(pedals[p].expZero - 1, 0, BANKS - 1), constrain(pedals[p].expMax - 1, 0, BANKS - 1));
              currentBank = constrain(currentBank, 0, BANKS - 1);
              if (repeatOnBankSwitch)
                midi_send(lastMIDIMessage[currentBank].midiMessage,
                          lastMIDIMessage[currentBank].midiCode,
                          lastMIDIMessage[currentBank].midiValue,
                          lastMIDIMessage[currentBank].midiChannel,
                          true,
                          0, MIDI_RESOLUTION - 1,
                          currentBank, p);
              break;

    case PED_START:
              mtc_start();
              break;

    case PED_STOP:
              mtc_stop();
              break;

    case PED_CONTINUE:
              mtc_continue();
              break;

    case PED_TAP:
              mtc_tap();
              break;

    case PED_BPM_PLUS:
              bpm = constrain(bpm + 1, 40, 300);
              MTC.setBpm(bpm);
              break;

    case PED_BPM_MINUS:
              bpm = constrain(bpm - 1, 40, 300);
              MTC.setBpm(bpm);
              break;
  }
}

//
//  Create new MIDI controllers setup
//
void controller_setup()
{
  lastUsedSwitch = 0xFF;
  lastUsedPedal  = 0xFF;
  lastUsed       = 0xFF;
  memset(lastPedalName, 0, MAXPEDALNAME+1);
  controller_delete();

  analogReadResolution(ADC_RESOLUTION_BITS);

  DPRINT("Bank %2d\n", currentBank + 1);

  // Build new MIDI controllers setup
  for (byte i = 0; i < PEDALS; i++) {
    DPRINT("Pedal %2d     ", i + 1);
    switch (pedals[i].function) {
      case PED_MIDI:         DPRINT("MIDI      "); break;
      case PED_BANK_PLUS:    DPRINT("BANK+1    "); break;
      case PED_BANK_MINUS:   DPRINT("BANK-1    "); break;
      case PED_START:        DPRINT("START     "); break;
      case PED_STOP:         DPRINT("STOP      "); break;
      case PED_CONTINUE:     DPRINT("CONTINUE  "); break;
      case PED_TAP:          DPRINT("TAP       "); break;
      case PED_BPM_PLUS:     DPRINT("BPM+      "); break;
      case PED_BPM_MINUS:    DPRINT("BPM-      "); break;
      case PED_BANK_PLUS_2:  DPRINT("BANK+2    "); break;
      case PED_BANK_MINUS_2: DPRINT("BANK-2    "); break;
      case PED_BANK_PLUS_3:  DPRINT("BANK+3    "); break;
      case PED_BANK_MINUS_3: DPRINT("BANK-3    "); break;
      default:               DPRINT("          "); break;
    }
    DPRINT("   ");
    switch (pedals[i].mode) {
      case PED_MOMENTARY1:  DPRINT("MOMENTARY1"); break;
      case PED_MOMENTARY2:  DPRINT("MOMENTARY2"); break;
      case PED_MOMENTARY3:  DPRINT("MOMENTARY3"); break;
      case PED_LATCH1:      DPRINT("LATCH1    "); break;
      case PED_LATCH2:      DPRINT("LATCH2    "); break;
      case PED_ANALOG:      DPRINT("ANALOG    "); break;
      case PED_JOG_WHEEL:   DPRINT("JOG_WHEEL "); break;
      case PED_LADDER:      DPRINT("LADDER    "); break;
      default:              DPRINT("          "); break;
    }
    DPRINT("   ");
    switch (pedals[i].pressMode) {
      case PED_PRESS_1:     DPRINT("PRESS_1    "); break;
      case PED_PRESS_2:     DPRINT("PRESS_2    "); break;
      case PED_PRESS_L:     DPRINT("PRESS_L    "); break;
      case PED_PRESS_1_2:   DPRINT("PRESS_1_2  "); break;
      case PED_PRESS_1_L:   DPRINT("PRESS_1_L  "); break;
      case PED_PRESS_1_2_L: DPRINT("PRESS_1_2_L"); break;
      case PED_PRESS_2_L:   DPRINT("PRESS_2_L  "); break;
      default:              DPRINT("           "); break;
    }
    DPRINT("   ");
    switch (pedals[i].invertPolarity) {
      case false:           DPRINT("POLARITY+"); break;
      case true:            DPRINT("POLARITY-"); break;
    }
    DPRINT("   ");
    switch (banks[currentBank][i].midiMessage) {
      case PED_PROGRAM_CHANGE:
        DPRINT("PROGRAM_CHANGE     %3d", banks[currentBank][i].midiCode);
        break;
      case PED_CONTROL_CHANGE:
        DPRINT("CONTROL_CHANGE     %3d", banks[currentBank][i].midiCode);
        break;
      case PED_NOTE_ON_OFF:
        DPRINT("NOTE_ON_OFF        %3d", banks[currentBank][i].midiCode);
        break;
      case PED_BANK_SELECT_INC:
        DPRINT("BANK_SELECT_INC    %3d", banks[currentBank][i].midiCode);
        break;
      case PED_BANK_SELECT_DEC:
        DPRINT("BANK_SELECT_DEC    %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PROGRAM_CHANGE_INC:
        DPRINT("PROGRAM_CHANGE_INC %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PROGRAM_CHANGE_DEC:
        DPRINT("PROGRAM_CHANGE_DEC %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PITCH_BEND:
        DPRINT("PITCH_BEND            ");
        break;
      case PED_CHANNEL_PRESSURE:
        DPRINT("CHANNEL PRESSURE      ");
        break;
      case PED_MIDI_START:
        DPRINT("MIDI_START            ");
        break;
      case PED_MIDI_STOP:
        DPRINT("MIDI_STOP             ");
        break;
      case PED_MIDI_CONTINUE:
        DPRINT("MIDI_CONTINUE         ");
        break;
      case PED_SEQUENCE:
        DPRINT("SEQUENCE           %3d", banks[currentBank][i].midiCode);
        break;
    }
    DPRINT("   Channel %2d", banks[currentBank][i].midiChannel);

    // Pedals setup for Actions
    switch (pedals[i].mode) {

      case PED_MOMENTARY1:
      case PED_LATCH1:

        pedals[i].buttonConfig = new ButtonConfig;
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, PIN_D(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 1);
        pinMode(PIN_D(i), INPUT_PULLUP);
        pedals[i].button[0]->setEventHandler(controller_event_handler_button);
        DPRINT("   Pin D%d", PIN_D(i));
        break;

      case PED_MOMENTARY2:
      case PED_LATCH2:

        pedals[i].buttonConfig = new ButtonConfig;
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, PIN_D(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 1);
        pinMode(PIN_D(i), INPUT_PULLUP);
        pedals[i].button[0]->setEventHandler(controller_event_handler_button);
        DPRINT("   Pin D%d", PIN_D(i));
        pedals[i].button[1] = new AceButton(pedals[i].buttonConfig, PIN_A(i), pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + 2);
        pinMode(PIN_A(i), INPUT_PULLUP);
        pedals[i].button[1]->setEventHandler(controller_event_handler_button);
        DPRINT("   Pin A%d", PIN_A(i));
        break;

      case PED_MOMENTARY3:

        pedals[i].buttonConfig = new Encoded4To2ButtonConfig(PIN_D(i), PIN_A(i), pedals[i].invertPolarity ? LOW : HIGH);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        pedals[i].button[0] = new AceButton(pedals[i].buttonConfig, (i + 1) * 10 + 1);
        pedals[i].button[1] = new AceButton(pedals[i].buttonConfig, (i + 1) * 10 + 2);
        pedals[i].button[2] = new AceButton(pedals[i].buttonConfig, (i + 1) * 10 + 3);
        pinMode(PIN_D(i), INPUT_PULLUP);
        DPRINT("   Pin D%d", PIN_D(i));
        pinMode(PIN_A(i), INPUT_PULLUP);
        DPRINT("   Pin A%d", PIN_A(i));
        pedals[i].buttonConfig->setEventHandler(controller_event_handler_button);
        break;

      case PED_ANALOG:

        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        pedals[i].analogPedal = new ResponsiveAnalogRead(PIN_A(i), true);
        pedals[i].analogPedal->setAnalogResolution(ADC_RESOLUTION);
        pedals[i].analogPedal->enableEdgeSnap();
        pedals[i].analogPedal->setActivityThreshold(10.0);
        //pedals[i].analogPedal->setSnapMultiplier(0.1);
        //analogSetPinAttenuation(PIN_A(i), ADC_11db);
        if (lastUsedPedal == 0xFF) {
          lastUsedPedal = i;
          lastUsed = i;
          strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXPEDALNAME+1);
        }
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;

      case PED_LADDER:

        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        for (byte s = 0; s < LADDER_STEPS; s++)
          ab[s].id = (i + 1) * 10 + s + 1;
        pedals[i].buttonConfig = new LadderButtonConfig(PIN_A(i), ab, LADDER_STEPS, pedals[i].invertPolarity ? LOW : HIGH);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
        pedals[i].buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);
        pedals[i].buttonConfig->setDebounceDelay(DEBOUNCE_INTERVAL);
        pedals[i].buttonConfig->setClickDelay(pressTime);
        pedals[i].buttonConfig->setDoubleClickDelay(doublePressTime);
        pedals[i].buttonConfig->setLongPressDelay(longPressTime);
        pedals[i].buttonConfig->setRepeatPressDelay(repeatPressTime);
        pedals[i].buttonConfig->setRepeatPressInterval(repeatPressTime);
        for (byte s = 0; s < LADDER_STEPS; s++)
          pedals[i].button[s] = new AceButton(pedals[i].buttonConfig, (i + 1) * 10 + s + 1,  pedals[i].invertPolarity ? LOW : HIGH, (i + 1) * 10 + s + 1);
        pinMode(PIN_A(i), INPUT_PULLUP);
        DPRINT("   Pin A%d", PIN_A(i));
        pedals[i].buttonConfig->setEventHandler(controller_event_handler_button);
        break;

      case PED_JOG_WHEEL:
        break;
    }
    pedals[i].pedalValue[0] = pedals[i].invertPolarity ? LOW : HIGH;
    pedals[i].lastUpdate[0] = millis();
    pedals[i].pedalValue[1] = pedals[i].pedalValue[0];
    pedals[i].lastUpdate[1] = pedals[i].lastUpdate[0];
    DPRINT("\n");

    //continue;

    // Pedals setup for Banks
    switch (pedals[i].mode) {

      case PED_MOMENTARY1:
      case PED_MOMENTARY2:
      case PED_MOMENTARY3:
      case PED_LATCH1:
      case PED_LATCH2:

        unsigned int input;
        unsigned int value;
        for (byte p = 0; p < 2; p++) {
          if (pedals[i].mode == PED_MOMENTARY1 && p == 1) continue;
          if (pedals[i].mode == PED_LATCH1     && p == 1) continue;

          pedals[i].debouncer[p] = new Bounce();
          switch (p) {
            case 0:
              // Setup the button with an internal pull-up
              pinMode(PIN_D(i), INPUT_PULLUP);

              // After setting up the button, setup the Bounce instance
              pedals[i].debouncer[0]->attach(PIN_D(i));
              DPRINT("   Pin D%d", PIN_D(i));
              break;
            case 1:
              // Setup the button with an internal pull-up
              pinMode(PIN_A(i), INPUT_PULLUP);

              // After setting up the button, setup the Bounce instance
              pedals[i].debouncer[1]->attach(PIN_A(i));
              DPRINT(" A%d", PIN_A(i));
              break;
          }
          pedals[i].debouncer[p]->interval(DEBOUNCE_INTERVAL);
          pedals[i].debouncer[p]->update();
          input = pedals[i].debouncer[p]->read();                                 // reads the updated pin state
          if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;      // invert the value
          value = map_digital(i, input);                                          // apply the digital map function to the value
          pedals[i].pedalValue[p] = value;
          pedals[i].lastUpdate[p] = millis();

          if (pedals[i].mode == PED_MOMENTARY1 || pedals[i].mode == PED_MOMENTARY2 || pedals[i].mode == PED_MOMENTARY3)
          {
            switch (p) {
              case 0:
                pedals[i].footSwitch[0] = new MD_UISwitch_Digital(PIN_D(i), pedals[i].invertPolarity ? HIGH : LOW);
                break;
              case 1:
                pedals[i].footSwitch[1] = new MD_UISwitch_Digital(PIN_A(i), pedals[i].invertPolarity ? HIGH : LOW);
                break;
            }
            pedals[i].footSwitch[p]->begin();
            footswitch_update(i, p);
          }
        }
        break;

      case PED_ANALOG:
        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        //if (pedals[i].function == PED_MIDI)
        {
          pedals[i].analogPedal = new ResponsiveAnalogRead(PIN_A(i), true);
          pedals[i].analogPedal->setAnalogResolution(ADC_RESOLUTION);
          pedals[i].analogPedal->enableEdgeSnap();
          pedals[i].analogPedal->setActivityThreshold(10.0);
          //pedals[i].analogPedal->setSnapMultiplier(0.1);
          //analogSetPinAttenuation(PIN_A(i), ADC_11db);
          if (lastUsedPedal == 0xFF) {
            lastUsedPedal = i;
            lastUsed = i;
            strncpy(lastPedalName, banks[currentBank][i].pedalName, MAXPEDALNAME+1);
          }
        }
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;

      case PED_LADDER:
        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        pedals[i].footSwitch[0] = new MD_UISwitch_Analog(PIN_A(i), kt, LADDER_STEPS);
        pedals[i].footSwitch[0]->begin();
        footswitch_update(i, 0);
        pedals[i].footSwitch[0]->read();
        DPRINT("   Pin A%d", PIN_A(i));
        break;

      case PED_JOG_WHEEL:
        pedals[i].jogwheel = new MD_REncoder(PIN_D(i), PIN_A(i));
        pedals[i].jogwheel->begin();
        pedals[i].jogwheel->setPeriod((11 - encoderSensitivity) * 100);   // From 1...10 to 1000..100
        DPRINT("   Pin A%d (CLK) D%d (DT)", PIN_A(i), PIN_D(i));
        break;
    }
    DPRINT("\n");
  }

  for (int i = 0; i < 100; i++) {
    controller_run(false);            // to avoid spurious readings
  }
}