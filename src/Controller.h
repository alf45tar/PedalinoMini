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

void screen_info(int, int, int, int);

//
//
//
void mtc_midi_send(byte b)
{
  if (interfaces[PED_RTPMIDI].midiClock) AppleMidiSendTimeCodeQuarterFrame(b);
  if (interfaces[PED_IPMIDI].midiClock)  ipMIDISendTimeCodeQuarterFrame(b);
}

//
// MIDI Time Code/MIDI Clock setup
//
void mtc_setup() {

  MTC.setup(mtc_midi_send);

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
  if (bpm > 0) MTC.setBpm(bpm);
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

  for (byte p = 0; p < PEDALS; p++) {
    if (pedals[p].autoSensing && pedals[p].mode != PED_NONE) {

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
          pedals[p].invertPolarity = true;
          // inititalize continuos calibration
          pedals[p].expZero = ADC_RESOLUTION - 1;
          pedals[p].expMax = 0;
          DPRINT(" ANALOG POLARITY-");
      }
    }
    else {
      DPRINT("Pedal %2d   autosensing disabled", p + 1);
    }
  DPRINT("\n");  
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
  value = map(value, pedals[p].expZero, pedals[p].expMax, 0, ADC_RESOLUTION - 1); // map the value from [minimumValue, maximumValue] to [0, 1023]
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
            break;

          case PED_BANK_SELECT_DEC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue1) banks[b][p].midiValue2 = banks[b][p].midiValue3;
            else banks[b][p].midiValue2--;
            break;

          case PED_PROGRAM_CHANGE_INC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue3) banks[b][p].midiValue2 = banks[b][p].midiValue1;
            else banks[b][p].midiValue2++;
            banks[b][p].midiCode = banks[b][p].midiValue2;
            break;

          case PED_PROGRAM_CHANGE_DEC:
            if (banks[b][p].midiValue2 == banks[b][p].midiValue1) banks[b][p].midiValue2 = banks[b][p].midiValue3;
            else banks[b][p].midiValue2--;
            banks[b][p].midiCode = banks[b][p].midiValue2;
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
            banks[b][p].midiValue2 = banks[b][p].midiValue1;
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
            banks[b][p].midiValue2 = banks[b][p].midiValue3;
            break;
        }
        break;
    }
  }
}

void midi_send(byte message, byte code, byte value, byte channel, bool on_off = true )
{
  switch (message) {

    case PED_NONE:
      break;

    case PED_NOTE_ON_OFF:

      if (on_off && value > 0) {
        DPRINT("NOTE ON.....Note %3d.....Velocity %3d.....Channel %2d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOn(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOn(code, value, channel);
        AppleMidiSendNoteOn(code, value, channel);
        ipMIDISendNoteOn(code, value, channel);
        BLESendNoteOn(code, value, channel);
        OSCSendNoteOn(code, value, channel);
        screen_info(midi::NoteOn, code, value, channel);
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
        screen_info(midi::NoteOff, code, value, channel);
        lastMIDIMessage[currentBank] = {PED_NOTE_ON_OFF, code, value, channel};
      }
      break;

    case PED_CONTROL_CHANGE:

      DPRINT("CONTROL CHANGE.....Code %3d......Value %3d.....Channel %2d\n", code, value, channel);
      if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(code, value, channel);
      if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(code, value, channel);
      AppleMidiSendControlChange(code, value, channel);
      ipMIDISendControlChange(code, value, channel);
      BLESendControlChange(code, value, channel);
      OSCSendControlChange(code, value, channel);
      screen_info(midi::ControlChange, code, value, channel);
      lastMIDIMessage[currentBank] = {PED_CONTROL_CHANGE, code, value, channel};
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
        screen_info(midi::ProgramChange, code, 0, channel);
        lastMIDIMessage[currentBank] = {PED_PROGRAM_CHANGE, code, 0, channel};
      }
      break;
    
    case PED_BANK_SELECT_INC:
    case PED_BANK_SELECT_DEC:

      if (on_off) {
        // MSB
        DPRINT("CONTROL CHANGE.....Code %3d.....Value %3d.....Channel %2d\n", midi::BankSelect, code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect, code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect, code, channel);
        AppleMidiSendControlChange(midi::BankSelect, code, channel);
        ipMIDISendControlChange(midi::BankSelect, code, channel);
        BLESendControlChange(midi::BankSelect, code, channel);
        OSCSendControlChange(midi::BankSelect, code, channel);
        screen_info(midi::ControlChange, midi::BankSelect, code, channel);
        // LSB
        DPRINT("CONTROL CHANGE.....Code %3d.....Value %3d.....Channel %2d\n", midi::BankSelect+32, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect+32, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect+32, value, channel);
        AppleMidiSendControlChange(midi::BankSelect+32, value, channel);
        ipMIDISendControlChange(midi::BankSelect+32, value, channel);
        BLESendControlChange(midi::BankSelect+32, value, channel);
        OSCSendControlChange(midi::BankSelect+32, value, channel);
        screen_info(midi::ControlChange, midi::BankSelect+32, value, channel);
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
        screen_info(midi::PitchBend, bend, 0, channel);
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
        screen_info(midi::AfterTouchChannel, 0, value, channel);
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
        screen_info(midi::Start, 0, 0, 0);
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
        screen_info(midi::Stop, 0, 0, 0);
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
        screen_info(midi::Continue, 0, 0, 0);
      }
      break;
    
    case PED_SEQUENCE:
      channel = constrain(channel, 1, SEQUENCES);
      DPRINT("=======================================================\n");
      DPRINT("SEQUENCE.....Number %2d\n", channel);
      DPRINT("-------------------------------------------------------\n");
      for (byte s = 0; s < STEPS; s++)
        if (sequences[channel-1][s].midiMessage == PED_CONTROL_CHANGE)
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, value, sequences[channel-1][s].midiChannel, on_off);
        else if (on_off) 
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, sequences[channel-1][s].midiValue1, sequences[channel-1][s].midiChannel, on_off);
        else
          midi_send(sequences[channel-1][s].midiMessage, sequences[channel-1][s].midiCode, sequences[channel-1][s].midiValue2, sequences[channel-1][s].midiChannel, on_off);
      DPRINT("=======================================================\n");
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
                            banks[b][i].midiValue1,
                            banks[b][i].midiChannel);
        break;
      case HIGH:  // HIGH = release
        if (send) midi_send(banks[b][i].midiMessage,
                            banks[b][i].midiCode,
                            banks[b][i].midiValue2,
                            banks[b][i].midiChannel,
                            pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2);
        break;
    }
    pedals[i].pedalValue[0] = value;
    pedals[i].lastUpdate[0] = millis();
    pedals[i].pedalValue[1] = pedals[i].pedalValue[0];
    pedals[i].lastUpdate[1] = pedals[i].lastUpdate[0];
    lastUsedSwitch = i;
    lastUsed = i;
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
                      banks[b][i].midiValue1,
                      banks[b][i].midiChannel);
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
                      latch);
          }
          break;
      }
      pedals[i].pedalValue[0] = value;
      pedals[i].lastUpdate[0] = millis();
      lastUsedSwitch = i;
      lastUsed = i;
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
                              banks[b][i].midiValue1,
                              banks[b][i].midiChannel);
        case HIGH:  // HIGH = release
          if (send) midi_send(banks[b][i].midiMessage,
                              banks[b][i].midiCode,
                              banks[b][i].midiValue2,
                              banks[b][i].midiChannel,
                              pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2);
      }
      pedals[i].pedalValue[1] = value;
      pedals[i].lastUpdate[1] = millis();
      lastUsedSwitch = i;
      lastUsed = i;
    }
  }
}

void refresh_switch_12L_midi(byte i, bool send)
{
  MD_UISwitch::keyResult_t  k, k1, k2;
  byte                      b;

  if (pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2) return;

  pedals[i].pedalValue[0] = digitalRead(PIN_D(i));
  //pedals[i].lastUpdate[0] = millis();
  pedals[i].pedalValue[1] = digitalRead(PIN_A(i));
  //pedals[i].lastUpdate[1] = millis();

  k1 = MD_UISwitch::KEY_NULL;
  k2 = MD_UISwitch::KEY_NULL;
  if (pedals[i].footSwitch[0] != nullptr) k1 = pedals[i].footSwitch[0]->read();
  if (pedals[i].footSwitch[1] != nullptr) k2 = pedals[i].footSwitch[1]->read();

  int j = 2;
  while ( j >= 0) {
    switch (j) {
      case 0: k = k1; break;
      case 1: k = k2; break;
      case 2: k = (k1 == k2) ? k1 : MD_UISwitch::KEY_NULL; break;
    }
    b = currentBank;
    switch (k) {

      case MD_UISwitch::KEY_PRESS:

        DPRINT("Pedal %2d   SINGLE PRESS \n", i + 1);
        switch (banks[b][i].midiMessage) {
          case PED_BANK_SELECT_INC:
          case PED_BANK_SELECT_DEC:
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            bank_update(b, i, 1);   // Increase
            if (send) {
              midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel);
            }
            break;

          default:
            b = (currentBank + j) % BANKS;
            if (send) {
              midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue1, banks[b][i].midiChannel);
            }
            break;
        }                    
        lastUsedSwitch = i;
        lastUsed = i;
        break;

      case MD_UISwitch::KEY_DPRESS:

        DPRINT("Pedal %2d   DOUBLE PRESS \n", i + 1);
        switch (banks[b][i].midiMessage) {
          case PED_BANK_SELECT_INC:
          case PED_BANK_SELECT_DEC:
          case PED_PROGRAM_CHANGE_INC:
          case PED_PROGRAM_CHANGE_DEC:
            bank_update(b, i, -1);    // Decrease
            break;
                     
          default:
            b = (currentBank + j) % BANKS;
            break;
        }
        if (send) {
          midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel);
        }                   
        lastUsedSwitch = i;
        break;

      case MD_UISwitch::KEY_LONGPRESS:

        DPRINT("Pedal %2d   LONG   PRESS \n", i + 1);
        switch (banks[b][i].midiMessage) {
          case PED_BANK_SELECT_INC:
          case PED_PROGRAM_CHANGE_INC:
            bank_update(b, i, 0);   // Set to bottom value
            if (send) {
              midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel);
            }
            break;

          case PED_BANK_SELECT_DEC:
          case PED_PROGRAM_CHANGE_DEC:
            bank_update(b, i, 2);   // Set to top value
            if (send) {
              midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue2, banks[b][i].midiChannel);
            }
            break;
                        
          default:
            b = (currentBank + j) % BANKS;
            if (send) {
              midi_send(banks[b][i].midiMessage, banks[b][i].midiCode, banks[b][i].midiValue3, banks[b][i].midiChannel);
            }
            break;
        }
        lastUsedSwitch = i;
        lastUsed = i;
        break;

      case MD_UISwitch::KEY_RPTPRESS:
      case MD_UISwitch::KEY_NULL:
      case MD_UISwitch::KEY_DOWN:
      case MD_UISwitch::KEY_UP:
        break;
    }
    if (k1 == k2 && k1 != MD_UISwitch::KEY_NULL) j = -1;
    else j--;
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

  if (pedals[i].function == PED_MIDI) return;

  k = 0;
  k1 = MD_UISwitch::KEY_NULL;
  k2 = MD_UISwitch::KEY_NULL;
  if (pedals[i].footSwitch[0] != nullptr) k1 = pedals[i].footSwitch[0]->read();
  if (pedals[i].footSwitch[1] != nullptr) k2 = pedals[i].footSwitch[1]->read();
  if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) && k2 == MD_UISwitch::KEY_NULL) k = 1;
  if ((k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS) && k1 == MD_UISwitch::KEY_NULL) k = 2;
  if ((k1 == MD_UISwitch::KEY_PRESS || k1 == MD_UISwitch::KEY_DPRESS || k1 == MD_UISwitch::KEY_LONGPRESS) &&
      (k2 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_DPRESS || k2 == MD_UISwitch::KEY_LONGPRESS)) k = 3;
  if (k > 0 && (k1 == MD_UISwitch::KEY_PRESS || k2 == MD_UISwitch::KEY_PRESS)) {
    // Single press
    if (pedals[i].mode == PED_LADDER) {
      if (k1 != MD_UISwitch::KEY_NULL) {
        switch (pedals[i].footSwitch[0]->getKey()) {
/*
            case 'S':
              return MD_Menu::NAV_SEL;
              break;

            case 'L':
              if (M.isInMenu())
                return MD_Menu::NAV_ESC;
              else if (MTC.isPlaying()) 
                MTC.sendStop();
              else
                MTC.sendPosition(0, 0, 0, 0);
              return MD_Menu::NAV_NULL;
              break;

            case 'U':
              if (M.isInMenu())
                return MD_Menu::NAV_INC;
              else if (currentBank < BANKS - 1) currentBank++;
              return MD_Menu::NAV_NULL;
              break;

            case 'D':
              if (M.isInMenu())
                return MD_Menu::NAV_DEC;
              else if (currentBank > 0) currentBank--;
              return MD_Menu::NAV_NULL;
              break;

            case 'R':
              if (M.isInMenu())
                return MD_Menu::NAV_NULL;
              else if (MTC.isPlaying())
                MTC.sendStop();
              else if (MTC.getFrames() == 0 && MTC.getSeconds() == 0 && MTC.getMinutes() == 0 && MTC.getHours() == 0)
                MTC.sendPlay();
              else
                MTC.sendContinue();
              return MD_Menu::NAV_NULL;
              break;
*/
        }
      }
    }
    else {
      switch (pedals[i].function) {
      case PED_BANK_PLUS:
        switch (k) {
          case 1:
            currentBank = (currentBank + 1) % BANKS;
            break;
          case 2:
            if (currentBank > 0) currentBank--;
            else currentBank = BANKS - 1;
            break;
          case 3:
            currentBank = 0;
            break;
        }
        if (repeatOnBankSwitch)
          midi_send(lastMIDIMessage[currentBank].midiMessage,
                    lastMIDIMessage[currentBank].midiCode,
                    lastMIDIMessage[currentBank].midiValue, 
                    lastMIDIMessage[currentBank].midiChannel);
        break;

      case PED_BANK_MINUS:
        switch (k) {
          case 1:
            if (currentBank > 0) currentBank--;
            else currentBank = BANKS - 1;
            break;
          case 2:
            currentBank = (currentBank + 1) % BANKS;
            break;
          case 3:
            currentBank = BANKS - 1;
            break;
        }
        if (repeatOnBankSwitch)
          midi_send(lastMIDIMessage[currentBank].midiMessage,
                    lastMIDIMessage[currentBank].midiCode,
                    lastMIDIMessage[currentBank].midiValue, 
                    lastMIDIMessage[currentBank].midiChannel);
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

  // Double press, long press and repeat
  if (pedals[i].footSwitch[0] != nullptr)
    switch (k1) {
      case MD_UISwitch::KEY_NULL:
        pedals[i].footSwitch[0]->setDoublePressTime(doublePressTime);
        pedals[i].footSwitch[0]->setLongPressTime(longPressTime);
        pedals[i].footSwitch[0]->setRepeatTime(repeatPressTime);
        pedals[i].footSwitch[0]->enableDoublePress(true);
        pedals[i].footSwitch[0]->enableLongPress(true);
        break;
      case MD_UISwitch::KEY_RPTPRESS:
        pedals[i].footSwitch[0]->setDoublePressTime(0);
        pedals[i].footSwitch[0]->setLongPressTime(0);
        pedals[i].footSwitch[0]->setRepeatTime(10);
        pedals[i].footSwitch[0]->enableDoublePress(false);
        pedals[i].footSwitch[0]->enableLongPress(false);
        break;
      case MD_UISwitch::KEY_DPRESS:
        //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_SEL;
        break;
      case MD_UISwitch::KEY_LONGPRESS:
        //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_ESC;
        break;
      case MD_UISwitch::KEY_PRESS:
      case MD_UISwitch::KEY_DOWN:
      case MD_UISwitch::KEY_UP:
        break;
    }

  if (pedals[i].footSwitch[1] != nullptr)
    switch (k2) {
      case MD_UISwitch::KEY_NULL:
        pedals[i].footSwitch[1]->setDoublePressTime(doublePressTime);
        pedals[i].footSwitch[1]->setLongPressTime(longPressTime);
        pedals[i].footSwitch[1]->setRepeatTime(repeatPressTime);
        pedals[i].footSwitch[1]->enableDoublePress(true);
        pedals[i].footSwitch[1]->enableLongPress(true);
        break;
      case MD_UISwitch::KEY_RPTPRESS:
        pedals[i].footSwitch[1]->setDoublePressTime(0);
        pedals[i].footSwitch[1]->setLongPressTime(0);
        pedals[i].footSwitch[1]->setRepeatTime(10);
        pedals[i].footSwitch[1]->enableDoublePress(false);
        pedals[i].footSwitch[1]->enableLongPress(false);
        break;
      case MD_UISwitch::KEY_DPRESS:
        //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_SEL;
        break;
      case MD_UISwitch::KEY_LONGPRESS:
        //if (pedals[i].function == PED_MENU) return MD_Menu::NAV_ESC;
        break;
      case MD_UISwitch::KEY_PRESS:
      case MD_UISwitch::KEY_DOWN:
      case MD_UISwitch::KEY_UP:
        break;
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
  value = map_analog(i, input);                             // expand to [0, 1023] and apply the map function
  pedals[i].analogPedal->update(value);                     // update the responsive analog average
  if (pedals[i].analogPedal->hasChanged()) {                // if the value changed since last time
    value = pedals[i].analogPedal->getValue();              // get the responsive analog average value
    value = map(value,                                      // map from [0, 1023] to [min, max] MIDI value
              0,
              ADC_RESOLUTION - 1,
              pedals[i].invertPolarity ? banks[currentBank][i].midiValue3 : banks[currentBank][i].midiValue1,
              pedals[i].invertPolarity ? banks[currentBank][i].midiValue1 : banks[currentBank][i].midiValue3);
    double velocity = (1000.0 * ((int)value - pedals[i].pedalValue[0])) / (micros() - pedals[i].lastUpdate[0]);
    switch (pedals[i].function) {
      case PED_MIDI:
        DPRINT("Pedal %2d   input %d output %d velocity %.2f\n", i + 1, input, value, velocity);
        if (send) midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel);
        if (send) midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel, false);
        pedals[i].pedalValue[0] = value;
        pedals[i].lastUpdate[0] = micros();
        lastUsedPedal = i;
        lastUsed = i;
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
    delete pedals[i].debouncer[0];
    delete pedals[i].debouncer[1];
    delete pedals[i].footSwitch[0];
    delete pedals[i].footSwitch[1];
    delete pedals[i].analogPedal;
  }
}

//
//  Refresh pedals
//
void controller_run(bool send = true)
{
  if (saveProfile && send) {
    DPRINT("Saving profile ...\n");
    eeprom_update_current_profile();
    delay(500);
    saveProfile = false;
    return;
  }

  if (reloadProfile && send) {
    DPRINT("Loading profile ...\n");
    eeprom_read_profile();
    autosensing_setup();
    controller_setup();
    mtc_setup();
    delay(200);
    reloadProfile = false;
    return;
  }

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
    }
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

  DPRINT("Bank %2d\n", currentBank + 1);

  // Build new MIDI controllers setup
  for (byte i = 0; i < PEDALS; i++) {
    DPRINT("Pedal %2d     ", i + 1);
    switch (pedals[i].function) {
      case PED_MIDI:        DPRINT("MIDI      "); break;
      case PED_BANK_PLUS:   DPRINT("BANK_PLUS "); break;
      case PED_BANK_MINUS:  DPRINT("BANK_MINUS"); break;
      case PED_START:       DPRINT("START     "); break;
      case PED_STOP:        DPRINT("STOP      "); break;
      case PED_CONTINUE:    DPRINT("CONTINUE  "); break;
      case PED_TAP:         DPRINT("TAP       "); break;
      case PED_MENU:        DPRINT("MENU      "); break;
      case PED_CONFIRM:     DPRINT("CONFIRM   "); break;
      case PED_ESCAPE:      DPRINT("ESCAPE    "); break;
      case PED_NEXT:        DPRINT("NEXT      "); break;
      case PED_PREVIOUS:    DPRINT("PREVIOUS  "); break;
      case PED_BPM_PLUS:    DPRINT("BPM+      "); break;
      case PED_BPM_MINUS:   DPRINT("BPM-      "); break;
      default:              DPRINT("          "); break;
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
        DPRINT("PROGRAM_CHANGE %3d", banks[currentBank][i].midiCode);
        break;
      case PED_CONTROL_CHANGE:
        DPRINT("CONTROL_CHANGE %3d", banks[currentBank][i].midiCode);
        break;
      case PED_NOTE_ON_OFF:
        DPRINT("NOTE_ON_OFF    %3d", banks[currentBank][i].midiCode);
        break;
      case PED_PITCH_BEND:
        DPRINT("PITCH_BEND        ");
        break;
      case PED_CHANNEL_PRESSURE:
        DPRINT("CHANNEL PRESSURE  ");
        break;
      case PED_MIDI_START:
        DPRINT("MIDI START        ");
        break;
      case PED_MIDI_STOP:
        DPRINT("MIDI STOP         ");
        break;
      case PED_MIDI_CONTINUE:
        DPRINT("MIDI CONTINUE     ");
        break;
      
    }
    DPRINT("   Channel %2d", banks[currentBank][i].midiChannel);

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
            //pedals[i].footSwitch[p]->setDebounceTime(DEBOUNCE_INTERVAL);
            pedals[i].footSwitch[p]->setPressTime(pressTime);
            if (pedals[i].function == PED_MIDI) {
              switch (pedals[i].pressMode) {
                case PED_PRESS_1:
                  pedals[i].footSwitch[p]->enableDoublePress(false);
                  pedals[i].footSwitch[p]->enableLongPress(false);
                  break;
                case PED_PRESS_2:
                case PED_PRESS_1_2:
                  pedals[i].footSwitch[p]->enableDoublePress(true);
                  pedals[i].footSwitch[p]->enableLongPress(false);
                  break;
                case PED_PRESS_L:
                case PED_PRESS_1_L:
                  pedals[i].footSwitch[p]->enableDoublePress(false);
                  pedals[i].footSwitch[p]->enableLongPress(true);
                  break;
                case PED_PRESS_1_2_L:
                case PED_PRESS_2_L:
                  pedals[i].footSwitch[p]->enableDoublePress(true);
                  pedals[i].footSwitch[p]->enableLongPress(true);
                  break;
              }
              pedals[i].footSwitch[p]->setDoublePressTime(doublePressTime);
              pedals[i].footSwitch[p]->setLongPressTime(longPressTime);
              pedals[i].footSwitch[p]->enableRepeat(false);
            }
            else
            {
              pedals[i].footSwitch[p]->setDoublePressTime(doublePressTime);
              pedals[i].footSwitch[p]->setLongPressTime(longPressTime);
              pedals[i].footSwitch[p]->setRepeatTime(repeatPressTime);
              pedals[i].footSwitch[p]->enableRepeatResult(true);
            }
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
          pedals[i].analogPedal->setActivityThreshold(6.0);
          analogReadResolution(ADC_RESOLUTION_BITS);
          analogSetPinAttenuation(PIN_A(i), ADC_11db);
          if (lastUsedPedal == 0xFF) {
            lastUsedPedal = i;
            lastUsed = i;
          }
        }
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;

      case PED_LADDER:
        pedals[i].footSwitch[0] = new MD_UISwitch_Analog(PIN_A(i), kt, ARRAY_SIZE(kt));
        DPRINT("   Pin A%d", PIN_A(i));
        pedals[i].footSwitch[0]->begin();
        //pedals[i].footSwitch[0]->setDebounceTime(DEBOUNCE_INTERVAL);
        pedals[i].footSwitch[0]->setPressTime(pressTime);
        if (pedals[i].function == PED_MIDI) {
          switch (pedals[i].pressMode) {
            case PED_PRESS_1:
              pedals[i].footSwitch[0]->enableDoublePress(false);
              pedals[i].footSwitch[0]->enableLongPress(false);
              break;
            case PED_PRESS_2:
            case PED_PRESS_1_2:
              pedals[i].footSwitch[0]->enableDoublePress(true);
              pedals[i].footSwitch[0]->enableLongPress(false);
              break;
            case PED_PRESS_L:
            case PED_PRESS_1_L:
              pedals[i].footSwitch[0]->enableDoublePress(false);
              pedals[i].footSwitch[0]->enableLongPress(true);
              break;
            case PED_PRESS_1_2_L:
            case PED_PRESS_2_L:
              pedals[i].footSwitch[0]->enableDoublePress(true);
              pedals[i].footSwitch[0]->enableLongPress(true);
              break;
          }
          pedals[i].footSwitch[0]->setDoublePressTime(doublePressTime);
          pedals[i].footSwitch[0]->setLongPressTime(longPressTime);
          pedals[i].footSwitch[0]->enableRepeat(false);
        }
        else
        {
          /*
            pedals[i].footSwitch[0]->setDoublePressTime(doublePressTime);
            pedals[i].footSwitch[0]->setLongPressTime(longPressTime);
            pedals[i].footSwitch[0]->setRepeatTime(repeatPressTime);
            pedals[i].footSwitch[0]->enableRepeatResult(true);
          */
          pedals[i].footSwitch[0]->enableDoublePress(false);
          pedals[i].footSwitch[0]->enableLongPress(false);
        }
        break;

      case PED_JOG_WHEEL:
        break;
    }
   DPRINT("\n");
  }
  for (byte i = 0; i < 100; i++)
    controller_run(false);            // to avoid spurious readings
}