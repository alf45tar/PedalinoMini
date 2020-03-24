#include <Arduino.h>
#include "VirtualButton.hpp"

using namespace VirtualButton;

uint8_t ::VirtualButton::constrainedAdd(uint8_t value, int8_t diff, uint8_t min, uint8_t max) {
  int16_t result = int16_t(value) + diff;
  if (result > max) {
    result -= (max - min + 1);
  }
  if (result < min) {
      result += (max - min + 1);
  }
  return result;
}

bool VirtualButtonState::isFor(const VirtualButtonAction &a) const {
  return channel == a.sendMidiStateful.channel &&
    cc == (a.isSendMidiProgramChange()) ? 255 : a.sendMidiStateful.cc;
}

void VirtualButtonState::assign(const VirtualButtonAction &a) {
  channel = a.sendMidiStateful.channel;
  cc = (a.isSendMidiProgramChange()) ? 255 : a.sendMidiStateful.cc;
  value = 0;
}

int16_t VirtualButtonConfig::findStateIdx(const VirtualButtonAction &a) const {
  if (a.isSendMidiStateful()){
    for (int i = 0; i < numStates; i++) {
      if (states[i].isFor(a)) {
        return i;
      }
    }
  }
  return -1;
}

void VirtualButtonConfig::onReconfigure() {
  numStates = 0;
  for (uint8_t pedal = 0; pedal < NUM_PEDALS; pedal++) {
    pedals[pedal].currentButton = -1;

    for (uint8_t bank = 0; bank < NUM_BANKS; bank++) {
      for (uint8_t button = 0; button < MAX_BUTTONS; button++) {
        Button &b = pedals[pedal].banks[bank][button];
        const uint8_t maxAction = b.getActionCount();
        for (uint8_t action = 0; action < maxAction; action++) {
          VirtualButtonAction &a = b.actions[action];
          if (a.isSendMidiStateful()) {
            const int16_t i = findStateIdx(a);
            Serial.printf("  %d/%d/%d/%d -> %d\n", pedal, bank, button, action, i);
            Serial.printf("  (%d,%d,%d)\n", a.sendMidiStateful.channel, a.sendMidiStateful.min, a.sendMidiStateful.max);
            if (i == -1) {
              states[numStates].assign(a);
              a.sendMidiStateful.stateIdx = numStates;
              numStates++;
            } else {
              a.sendMidiStateful.stateIdx = i;
            }
          }
        }
      }
    }
  }
}

void VirtualButtonPedal::onButton(VirtualButtonConfig &config, uint8_t bank, int8_t button) {
  currentBank = bank;
  if (button != -1 && button != lastButton) {
    currentCount = 0;
  }
  if (button != currentButton) {
    Serial.printf("%d -> %d: ", currentButton, button);
    if (currentButton >= 0) {
      Serial.printf("  release %d with count %d\n", currentButton, currentCount);
      banks[bank][currentButton].onRelease(config, currentCount);
    }
    currentButton = button;
    if (currentButton >= 0) {
      currentCount++;
      Serial.printf("  press %d with count %d\n", currentButton, currentCount);
      banks[bank][currentButton].onPress(config, currentCount);
      lastButton = currentButton;
    }
  }
}

void VirtualButtonPedal::onIdle(VirtualButtonConfig &config) {
  Serial.printf("Idle for button %d (%d), count %d\n", currentButton, lastButton, currentCount);
  if (currentCount > 0) {
    if (currentButton >= 0) {
      banks[currentBank][currentButton].onAfterPress(config, currentCount);
    } else {
      banks[currentBank][lastButton].onAfterRelease(config, currentCount);
      currentCount = 0;
    }
  }
}

void VirtualButtonAction::go(VirtualButtonConfig &config) {
  Serial.printf("go() for %d\n", type);
  switch(type) {
    case NONE:
      break;
    case SEND_MIDI_CC:
      midi_send(PED_CONTROL_CHANGE, sendMidiMessage.code, sendMidiMessage.value, sendMidiMessage.channel, true);
      break;
    case SEND_MIDI_NOTE_ON:
      midi_send(PED_NOTE_ON_OFF, sendMidiMessage.code, sendMidiMessage.value, sendMidiMessage.channel, true);
      break;
    case SEND_MIDI_NOTE_OFF:
      midi_send(PED_NOTE_ON_OFF, sendMidiMessage.code, sendMidiMessage.value, sendMidiMessage.channel, false);
      break;
    case SEND_MIDI_PROGRAM_UP:
    case SEND_MIDI_PROGRAM_DOWN:
    case SEND_MIDI_CC_UP:
    case SEND_MIDI_CC_DOWN: {
      VirtualButtonState &state = config.findState(*this);
      state.add((type == SEND_MIDI_CC_UP || type == SEND_MIDI_PROGRAM_UP) ? 1 : -1,
                sendMidiStateful.min, sendMidiStateful.max);

      if (type == SEND_MIDI_PROGRAM_UP || type == SEND_MIDI_PROGRAM_DOWN) {
        midi_send(PED_PROGRAM_CHANGE, state.value, 0, sendMidiStateful.channel, true);
      } else {
        midi_send(PED_CONTROL_CHANGE, sendMidiStateful.cc, state.value, sendMidiStateful.channel, true);
      }
      break;
    }
    case DEVICE_BANK_UP:
      Serial.printf("Bank up from %d, min %d, max %d\n", currentBank, deviceBank.min, deviceBank.max);
      currentBank = constrainedAdd(currentBank, 1, deviceBank.min, deviceBank.max);
      break;
    case DEVICE_BANK_DOWN:
      Serial.printf("Bank down from %d, min %d, max %d\n", currentBank, deviceBank.min, deviceBank.max);
      currentBank = constrainedAdd(currentBank, -1, deviceBank.min, deviceBank.max);
      break;
    case DEVICE_BANK_SET:
      currentBank = deviceBank.min;
      break;
  }
}
