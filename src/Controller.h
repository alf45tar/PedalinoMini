/*  __________           .___      .__  .__                   ___ ________________    ___
 *  \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \   
 *   |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \  
 *   |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
 *   |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
 *                 \/     \/     \/             \/           \__\                 \/  /__/
 *                                                                (c) 2018 alf45star
 *                                                        https://github.com/alf45tar/Pedalino
 */

void screen_info(int, int, int, int);

//
//  Autosensing setup
//
void autosensing_setup()
{
  int tip;    // tip connected to an input digital pin with internal pull-up resistor
  int ring;   // ring connected to an input analog pin
  /*        */// sleeve connected to GND
  int ring_min;
  int ring_max;
  Bounce debouncer;

  return;

  DPRINT("Pedal autosensing...\n");

  for (byte p = 0; p < PEDALS; p++) {
    pinMode(PIN_D(p), INPUT_PULLUP);
    if (pedals[p].autoSensing) {
      debouncer.attach(PIN_D(p));
      debouncer.interval(DEBOUNCE_INTERVAL);
      debouncer.update();
      tip = debouncer.read();

      DPRINT("Pedal %2d   Tip Pin %2d ", p + 1, PIN_D(p));
      switch (tip) {
        case LOW:
          DPRINT("LOW ");
          break;
        case HIGH:
          DPRINT("HIGH");
          break;
      }
      DPRINT("    Ring Pin A %2d ", p);

      ring_min = ADC_RESOLUTION;
      ring_max = 0;
      for (int i = 0; i < 10; i++) {
        ring = analogRead(PIN_A(p));
        ring_min = _min(ring, ring_min);
        ring_max = _max(ring, ring_max);

        DPRINT("%d ", ring);
      }
      if ((ring_max - ring_min) > 1) {
        if (tip == LOW) {
          // tip connected to GND
          // switch between tip and ring normally closed
          pedals[p].mode = PED_MOMENTARY1;
          pedals[p].invertPolarity = true;
          DPRINT(" MOMENTARY POLARITY-\n");
        }
        else {
          // not connected
          pedals[p].mode = PED_MOMENTARY1;
          pedals[p].invertPolarity = false;
          DPRINT(" FLOATING PIN - NOT CONNECTED\n");
        }
      }
      else if (ring <= 1) {
        // ring connected to sleeve (GND)
        // switch between tip and ring
        pedals[p].mode = PED_MOMENTARY1;
        if (tip == LOW) pedals[p].invertPolarity = true; // switch normally closed
        DPRINT(" MOMENTARY");
        if (pedals[p].invertPolarity) DPRINT(" POLARITY-");
        DPRINTLN("");
      }
      else if (ring > 0) {
        // analog
        pedals[p].mode = PED_ANALOG;
        pedals[p].invertPolarity = true;
        // inititalize continuos calibration
        pedals[p].expZero = ADC_RESOLUTION - 1;
        pedals[p].expMax = 0;
        DPRINT(" ANALOG POLARITY-\n");
      }
    }
    else {
      DPRINT("Pedal %2d   autosensing disabled", p + 1);
    }
  }
  DPRINT("\n");
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
      value = round(log(value + 1) * 147.61);             // y=log(x+1)/log(1023)*1023
      break;
    case PED_ANTILOG:
      value = round((exp(value / 511.5) - 1) * 160.12);   // y=[e^(2*x/1023)-1]/[e^2-1]*1023
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

    case PED_NOTE_ON_OFF:

      if (on_off && value > 0) {
        DPRINT("     NOTE ON     Note %d     Velocity %d     Channel %d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOn(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOn(code, value, channel);
        AppleMidiSendNoteOn(code, value, channel);
        ipMIDISendNoteOn(code, value, channel);
        BLESendNoteOn(code, value, channel);
        OSCSendNoteOn(code, value, channel);
        screen_info(midi::NoteOn, code, value, channel);
      }
      else {
        DPRINT("     NOTE OFF    Note %d     Velocity %d     Channel %d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendNoteOff(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendNoteOff(code, value, channel);
        AppleMidiSendNoteOff(code, value, channel);
        ipMIDISendNoteOff(code, value, channel);
        BLESendNoteOff(code, value, channel);
        OSCSendNoteOff(code, value, channel);
        screen_info(midi::NoteOff, code, value, channel);
      }
      break;

    case PED_CONTROL_CHANGE:

      if (on_off) {
        DPRINT("     CONTROL CHANGE     Code %d     Value %d     Channel %d\n", code, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(code, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(code, value, channel);
        AppleMidiSendControlChange(code, value, channel);
        ipMIDISendControlChange(code, value, channel);
        BLESendControlChange(code, value, channel);
        OSCSendControlChange(code, value, channel);
        screen_info(midi::ControlChange, code, value, channel);
      }
      break;

    case PED_PROGRAM_CHANGE:
    case PED_PROGRAM_CHANGE_INC:
    case PED_PROGRAM_CHANGE_DEC:

      if (on_off) {
        DPRINT("     PROGRAM CHANGE     Program %d     Channel %d", code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendProgramChange(code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendProgramChange(code, channel);
        AppleMidiSendProgramChange(code, channel);
        ipMIDISendProgramChange(code, channel);
        BLESendProgramChange(code, channel);
        OSCSendProgramChange(code, channel);
        screen_info(midi::ProgramChange, code, 0, channel);
      }
      break;

    case PED_PITCH_BEND:

      if (on_off) {
        int bend = map(value, 0, MIDI_RESOLUTION-1, MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MAX);
        DPRINT("     PITCH BEND     Value %d     Channel %d", bend, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendPitchBend(bend, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendPitchBend(bend, channel);
        AppleMidiSendPitchBend(bend, channel);
        ipMIDISendPitchBend(bend, channel);
        BLESendPitchBend(bend, channel);
        OSCSendPitchBend(bend, channel);
        screen_info(midi::PitchBend, bend, 0, channel);
      }
      break;
    
    case PED_BANK_SELECT_INC:
    case PED_BANK_SELECT_DEC:

      if (on_off) {
        // MSB
        DPRINT("     CONTROL CHANGE     Code %d     Value %d     Channel %d\n", midi::BankSelect, code, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect, code, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect, code, channel);
        AppleMidiSendControlChange(midi::BankSelect, code, channel);
        ipMIDISendControlChange(midi::BankSelect, code, channel);
        BLESendControlChange(midi::BankSelect, code, channel);
        OSCSendControlChange(midi::BankSelect, code, channel);
        screen_info(midi::ControlChange, midi::BankSelect, code, channel);
        // LSB
        DPRINT("     CONTROL CHANGE     Code %d     Value %d     Channel %d\n", midi::BankSelect+32, value, channel);
        if (interfaces[PED_USBMIDI].midiOut)  USB_MIDI.sendControlChange(midi::BankSelect+32, value, channel);
        if (interfaces[PED_DINMIDI].midiOut)  DIN_MIDI.sendControlChange(midi::BankSelect+32, value, channel);
        AppleMidiSendControlChange(midi::BankSelect+32, value, channel);
        ipMIDISendControlChange(midi::BankSelect+32, value, channel);
        BLESendControlChange(midi::BankSelect+32, value, channel);
        OSCSendControlChange(midi::BankSelect+32, value, channel);
        screen_info(midi::ControlChange, midi::BankSelect+32, value, channel);
      }
      break;
  }
}

//
//  MIDI messages refresh
//
void midi_refresh(bool send = true)
{
  MD_UISwitch::keyResult_t  k, k1, k2;
  bool                      state1, state2;
  unsigned int              input;
  unsigned int              value;
  byte                      b;

  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].function == PED_MIDI) {
      switch (pedals[i].mode) {

        case PED_MOMENTARY1:
        case PED_MOMENTARY2:
        case PED_MOMENTARY3:
        case PED_LATCH1:
        case PED_LATCH2:

          switch (pedals[i].pressMode) {

            case PED_PRESS_1:
              state1 = false;
              state2 = false;
              if (pedals[i].debouncer[0] != nullptr) state1 = pedals[i].debouncer[0]->update();
              if (pedals[i].debouncer[1] != nullptr) state2 = pedals[i].debouncer[1]->update();
              if (state1 && state2) {                                                     // pin state changed
                input = pedals[i].debouncer[0]->read();                                   // reads the updated pin state
                if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;        // invert the value
                value = map_digital(i, input);                                            // apply the digital map function to the value

                DPRINT("\nPedal %2d   input %d output %d", i + 1, input, value);

                b = (currentBank + 2) % BANKS;
                bank_update(b, i);
                if (value == LOW)                                                         // LOW = pressed, HIGH = released
                  if (send) midi_send(banks[b][i].midiMessage,
                                      banks[b][i].midiCode,
                                      banks[b][i].midiValue1,
                                      banks[b][i].midiChannel);
                else if (send) midi_send(banks[b][i].midiMessage,
                                         banks[b][i].midiCode,
                                         banks[b][i].midiValue2,
                                         banks[b][i].midiChannel,
                                         pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2);
                pedals[i].pedalValue[0] = value;
                pedals[i].lastUpdate[0] = millis();
                pedals[i].pedalValue[1] = pedals[i].pedalValue[0];
                pedals[i].lastUpdate[1] = pedals[i].lastUpdate[0];
                lastUsedSwitch = i;
              }
              else {
                if (state1) {                                                             // pin state changed
                  input = pedals[i].debouncer[0]->read();                                 // reads the updated pin state
                  if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;      // invert the value
                  value = map_digital(i, input);                                          // apply the digital map function to the value

                  DPRINT("\nPedal %2d   input %d output %d", i + 1, input, value);

                  b = currentBank;
                  if (value == LOW) {                                                     // LOW = pressed, HIGH = released
                    if (send) {
                      bank_update(b, i);
                      midi_send(banks[b][i].midiMessage,
                                banks[b][i].midiCode,
                                banks[b][i].midiValue1,
                                banks[b][i].midiChannel);
                    }
                  }
                  else
                    if (send) {
                      bool latch = pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2;
                      bank_update(banks[b][i].midiMessage, b, i, latch);
                      midi_send(banks[b][i].midiMessage,
                                banks[b][i].midiCode,
                                banks[b][i].midiValue2,
                                banks[b][i].midiChannel,
                                latch);
                    }
                  pedals[i].pedalValue[0] = value;
                  pedals[i].lastUpdate[0] = millis();
                  lastUsedSwitch = i;
                }
                if (state2) {                                                             // pin state changed
                  input = pedals[i].debouncer[1]->read();                                 // reads the updated pin state
                  if (pedals[i].invertPolarity) input = (input == LOW) ? HIGH : LOW;      // invert the value
                  value = map_digital(i, input);                                          // apply the digital map function to the value

                  DPRINT("\nPedal %2d   input %d output %d", i + 1, input, value);

                  b = (currentBank + 1) % BANKS;
                  bank_update(b, i);
                  if (value == LOW) {                                                      // LOW = pressed, HIGH = released
                    if (send) midi_send(banks[b][i].midiMessage,
                                        banks[b][i].midiCode,
                                        banks[b][i].midiValue1,
                                        banks[b][i].midiChannel);
                  }
                  else if (send) midi_send(banks[b][i].midiMessage,
                                           banks[b][i].midiCode,
                                           banks[b][i].midiValue2,
                                           banks[b][i].midiChannel,
                                           pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2);
                  pedals[i].pedalValue[1] = value;
                  pedals[i].lastUpdate[1] = millis();
                  lastUsedSwitch = i;
                }
              }
              break;

            case PED_PRESS_1_2:
            case PED_PRESS_1_L:
            case PED_PRESS_1_2_L:
            case PED_PRESS_2:
            case PED_PRESS_2_L:
            case PED_PRESS_L:

              if (pedals[i].mode == PED_LATCH1 || pedals[i].mode == PED_LATCH2) break;

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

                    DPRINT("\nPedal %2d   SINGLE PRESS ", i + 1);
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
                    break;

                  case MD_UISwitch::KEY_DPRESS:

                    DPRINT("\nPedal %2d   DOUBLE PRESS ", i + 1);
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

                    DPRINT("\nPedal %2d   LONG   PRESS ", i + 1);
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
                    break;

                  case MD_UISwitch::KEY_RPTPRESS:
                  case MD_UISwitch::KEY_NULL:
                    break;
                }
                if (k1 == k2 && k1 != MD_UISwitch::KEY_NULL) j = -1;
                else j--;
              }
              break;
          }
          break;

        case PED_ANALOG:

          if (pedals[i].analogPedal == nullptr) continue;           // sanity check

          input = analogRead(PIN_A(i));                             // read the raw analog input value
          if (pedals[i].autoSensing) {                              // continuos calibration

            if (pedals[i].expZero > round(1.1 * input)) {
              DPRINT("Pedal %2d calibration min %d\n", i + 1, round(1.1 * input));
            }
            if (pedals[i].expMax < round(0.9 * input)) {
              DPRINT("Pedal %2d calibration max %d\n", i + 1, round(0.9 * input));
            }

            pedals[i].expZero = _min(pedals[i].expZero, round(1.1 * input));
            pedals[i].expMax  = _max(pedals[i].expMax,  round(0.9 * input));
          }
          value = map_analog(i, input);                             // apply the digital map function to the value
          if (pedals[i].invertPolarity) value = ADC_RESOLUTION - 1 - value;   // invert the scale
          value = value >> 5;                                       // map from 12-bit value [0, 4095] to the 7-bit MIDI value [0, 127]
          pedals[i].analogPedal->update(value);                     // update the responsive analog average
          if (pedals[i].analogPedal->hasChanged())                  // if the value changed since last time
          {
            value = pedals[i].analogPedal->getValue();              // get the responsive analog average value
            double velocity = (1.0 * (value - pedals[i].pedalValue[0])) / (millis() - pedals[i].lastUpdate[0]);
            DPRINT("\nPedal %2d   input %d output %d velocity %.2f", i + 1, input, value, velocity);
            if (send) midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel);
            if (send) midi_send(banks[currentBank][i].midiMessage, banks[currentBank][i].midiCode, value, banks[currentBank][i].midiChannel, false);
            pedals[i].pedalValue[0] = value;
            pedals[i].lastUpdate[0] = millis();
            lastUsedPedal = i;
          }
          break;
      }
    }
  }
}

//
//  Create new MIDI controllers setup
//
void controller_setup()
{
  // Delete previous setup
  for (byte i = 0; i < PEDALS; i++) {
    //delete pedals[i].debouncer[0];
    //delete pedals[i].debouncer[1];
    //delete pedals[i].footSwitch[0];
    //delete pedals[i].footSwitch[1];
    delete pedals[i].analogPedal;
  }

  lastUsedSwitch = 0xFF;
  lastUsedPedal  = 0xFF;

  DPRINT("MIDI Interface ");
  switch (currentInterface) {
    case PED_USBMIDI:
      DPRINT("USB\n");
      break;
    case PED_DINMIDI:
      DPRINT("DIN MIDI\n");
      break;
    case PED_RTPMIDI:
      DPRINT("RTP-MIDI\n");
      break;
    case PED_IPMIDI:
      DPRINT("ipMIDI\n");
      break;
    case PED_BLEMIDI:
      DPRINT("Bluetooth\n");
      break;
  }
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
            pedals[i].footSwitch[p]->setDebounceTime(DEBOUNCE_INTERVAL);
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
              pedals[i].footSwitch[p]->setDoublePressTime(300);
              pedals[i].footSwitch[p]->setLongPressTime(500);
              pedals[i].footSwitch[p]->enableRepeat(false);
            }
            else
            {
              pedals[i].footSwitch[p]->setDoublePressTime(300);
              pedals[i].footSwitch[p]->setLongPressTime(500);
              pedals[i].footSwitch[p]->setRepeatTime(500);
              pedals[i].footSwitch[p]->enableRepeatResult(true);
            }
          }
        }
        break;

      case PED_ANALOG:
        pinMode(PIN_D(i), OUTPUT);
        digitalWrite(PIN_D(i), HIGH);
        if (pedals[i].function == PED_MIDI) {
          pedals[i].analogPedal = new ResponsiveAnalogRead(PIN_A(i), true);
          pedals[i].analogPedal->setActivityThreshold(6.0);
          pedals[i].analogPedal->setAnalogResolution(MIDI_RESOLUTION);        // 7-bit MIDI resolution
          pedals[i].analogPedal->enableEdgeSnap();                            // ensures that values at the edges of the spectrum can be easily reached when sleep is enabled
          if (lastUsedPedal == 0xFF) lastUsedPedal = i;
        }
        DPRINT("   Pin A%d D%d", PIN_A(i), PIN_D(i));
        break;

      case PED_LADDER:
        pedals[i].footSwitch[0] = new MD_UISwitch_Analog(PIN_A(i), kt, ARRAY_SIZE(kt));
        DPRINT("   Pin A%d", PIN_A(i));
        pedals[i].footSwitch[0]->begin();
        pedals[i].footSwitch[0]->setDebounceTime(DEBOUNCE_INTERVAL);
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
          pedals[i].footSwitch[0]->setDoublePressTime(300);
          pedals[i].footSwitch[0]->setLongPressTime(500);
          pedals[i].footSwitch[0]->enableRepeat(false);
        }
        else
        {
          /*
            pedals[i].footSwitch[0]->setDoublePressTime(300);
            pedals[i].footSwitch[0]->setLongPressTime(500);
            pedals[i].footSwitch[0]->setRepeatTime(500);
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
    midi_refresh(false);            // to avoid spurious readings
}

//
// Calibration for analog controllers
//
#ifdef NOLCD
#define calibrate(...)
#else
void calibrate()
{
  unsigned long start = millis();

  // Clear display
  lcd.clear();
  lcd.setCursor(0, 0);

  // Display countdown bar
  for (int i = 1; i <= LCD_COLS; i++)
    lcd.print(char(B10100101));

  // Move expression pedal from min to max during CALIBRATION_DURATION
  pedals[currentPedal].expZero = ADC_RESOLUTION - 1;
  pedals[currentPedal].expMax = 0;

  while (millis() - start < CALIBRATION_DURATION) {

    // Read the current value and update min and max
    int ax = analogRead(PIN_A(currentPedal));
    pedals[currentPedal].expZero = min( pedals[currentPedal].expZero, ax + 20);
    pedals[currentPedal].expMax  = max( pedals[currentPedal].expMax, ax - 20);

    // Update countdown bar (1st row)
    lcd.setCursor(LCD_COLS - map(millis() - start, 0, CALIBRATION_DURATION, 0, LCD_COLS), 0);
    lcd.print(" ");

    // Update value bar (2nd row)
    lcd.setCursor(0, 1);
    lcd.print(pedals[currentPedal].expZero);
    for (int i = 1; i < LCD_COLS - floor(log10(pedals[currentPedal].expZero + 1)) - floor(log10(pedals[currentPedal].expMax + 1)) - 1; i++)
      lcd.print(" ");
    lcd.print(pedals[currentPedal].expMax);
  }
}
#endif  // NOLCD

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
