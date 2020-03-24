#pragma once

#include <esp_log.h>
#include <Arduino.h>
#include <stdint.h>
#include <ESPAsyncWebServer.h>

// defined in Controller.h
void midi_send(byte message, byte code, byte value, byte channel, bool on_off);
extern byte currentBank;

#define PED_EMPTY               0
#define PED_PROGRAM_CHANGE      1
#define PED_CONTROL_CHANGE      2
#define PED_NOTE_ON_OFF         3
#define PED_BANK_SELECT_INC     4
#define PED_BANK_SELECT_DEC     5
#define PED_PROGRAM_CHANGE_INC  6
#define PED_PROGRAM_CHANGE_DEC  7
#define PED_PITCH_BEND          8
#define PED_CHANNEL_PRESSURE    9
#define PED_MIDI_START          10
#define PED_MIDI_STOP           11
#define PED_MIDI_CONTINUE       12
#define PED_SEQUENCE            20

namespace VirtualButton {

// Maximum number of pedals
static constexpr uint8_t NUM_PEDALS = 6;

// Number of banks for virtual buttons
static constexpr uint8_t NUM_BANKS = 6;

// Maximum number of physical buttons per panel
static constexpr uint8_t MAX_BUTTONS = 6;

// Maximum number of configurable actions for any button type
static constexpr uint8_t MAX_ACTIONS = 4;

// Maximum number of CC / program states to keep across all MIDI channels
static constexpr uint16_t MAX_STATES = 256;

struct VirtualButtonConfig;
struct VirtualButtonAction;

uint8_t constrainedAdd(uint8_t value, int8_t diff, uint8_t min, uint8_t max);

struct VirtualButtonState {
  uint8_t channel;
  uint8_t cc; // 255 for program change
  uint8_t value;

  bool isFor(const VirtualButtonAction &action) const;

  void assign(const VirtualButtonAction &action);

  void add(int8_t diff, uint8_t min, uint8_t max) {
    value = constrainedAdd(value, diff, min, max);
  }
};

struct VirtualButtonAction {
  enum Type: uint8_t {
    NONE,
    SEND_MIDI_CC,
    SEND_MIDI_NOTE_ON,
    SEND_MIDI_NOTE_OFF,
    SEND_MIDI_CC_UP,
    SEND_MIDI_CC_DOWN,
    SEND_MIDI_PROGRAM_UP,
    SEND_MIDI_PROGRAM_DOWN,
    // TODO: MIDI BANK up/down (14-bit)
    DEVICE_BANK_UP,
    DEVICE_BANK_DOWN,
    DEVICE_BANK_SET
  };
  Type type = NONE;

  union {
    struct {
      uint8_t channel;
      uint8_t code;
      uint8_t value;
    } sendMidiMessage;

    struct {
      uint8_t channel;
      uint8_t cc; // 255 (unused) for program change
      uint8_t min;
      uint8_t max;

      uint16_t stateIdx; // index into VirtualButtonConfig.state where current value is stored
    } sendMidiStateful;

    struct {
      uint8_t min; // or value, for set bank
      uint8_t max;
    } deviceBank;
  };

  bool isSendMidiMessage() const {
    return type == SEND_MIDI_CC || type == SEND_MIDI_NOTE_ON || type == SEND_MIDI_NOTE_OFF;
  }

  bool isSendMidiStateful() const {
    return type == SEND_MIDI_CC_UP || type == SEND_MIDI_CC_DOWN ||
           type == SEND_MIDI_PROGRAM_UP || type == SEND_MIDI_PROGRAM_DOWN;
  }

  bool isSendMidiProgramChange() const {
    return type == SEND_MIDI_PROGRAM_UP  || type == SEND_MIDI_PROGRAM_DOWN;
  }

  bool isSendMidiCCChange() const {
    return type == SEND_MIDI_CC_UP  || type == SEND_MIDI_CC_DOWN;
  }

  bool isDeviceBank() const {
    return type == DEVICE_BANK_UP || type == DEVICE_BANK_DOWN || type == DEVICE_BANK_SET;
  }

  void go(VirtualButtonConfig &config);
};

struct Button {
  enum Mode: uint8_t {
    // action 0 on press, action 1 on release
    PRESS_RELEASE,
    // action 0 on down, action 1 on next down
    TOGGLE_DOWN,
    // action 0 on up, action 1 on next up
    TOGGLE_UP,
    // action 0 on press
    // action 1 on first up with no following press
    // action 2 on double-press
    // action 3 on double-press release
    PRESS_RELEASE_DOUBLE
  };
  Mode mode;

  VirtualButtonAction actions[MAX_ACTIONS];

  union {
    struct {
      bool toggled;
    } toggle;
    struct {
      bool triggered;
    } press_release_double;
  };

  void onReconfigure() {
    if (mode == TOGGLE_UP || mode == TOGGLE_DOWN) {
      toggle.toggled = false;
    } else if (mode == PRESS_RELEASE_DOUBLE) {
      press_release_double.triggered = false;
    }
  }

  void onPress(VirtualButtonConfig &config, const uint8_t count) {
    switch(mode){
      case PRESS_RELEASE: {
        actions[0].go(config);
        break;
      }
      case TOGGLE_DOWN: {
        actions[(toggle.toggled ? 1 : 0)].go(config);
        toggle.toggled = !toggle.toggled;
        break;
      }
      case TOGGLE_UP: {
        // No actions here when button is pressed.
        break;
      }
      case PRESS_RELEASE_DOUBLE: {
        if (count <= 1) {
          press_release_double.triggered = false;
        }
        break;
      }
    }
  }

  void trigger(uint8_t action, VirtualButtonConfig &config) {
    if (!press_release_double.triggered) {
      Serial.printf("Triggering %d\n", action);
      actions[action].go(config);
      press_release_double.triggered = true;
    } else {
      Serial.printf("Skipping %d\n", action);
    }
  }

  void onRelease(VirtualButtonConfig &config, const uint8_t count) {
    switch(mode){
      case PRESS_RELEASE: {
        actions[1].go(config);
        break;
      }
      case TOGGLE_DOWN: {
        // No actions here when button is released.
        break;
      }
      case TOGGLE_UP: {
        actions[(toggle.toggled ? 1 : 0)].go(config);
        toggle.toggled = !toggle.toggled;
        break;
      }
      case PRESS_RELEASE_DOUBLE: {
        if (count == 2) {
          // after a double-press with short release
          trigger(2, config);
        }
        break;
      }
    }
  }

  void onAfterPress(VirtualButtonConfig &config, const uint8_t count) {
    if (mode == PRESS_RELEASE_DOUBLE) {
      if (count == 1) {
        // after a long press
        trigger(1, config);
      } else if (count == 2) {
        // after a double-press with long release
        trigger(3, config);
      }
    }
  }

  void onAfterRelease(VirtualButtonConfig &config, const uint8_t count) {
    if (mode == PRESS_RELEASE_DOUBLE) {
      if (count == 1) {
        // after a short press (but not quickly pressed again)
        trigger(0, config);
      }
    }
  }

  uint8_t getActionCount() const {
    return (mode == PRESS_RELEASE_DOUBLE) ? 4 : 2;
  }
};

struct VirtualButtonPedal {
  Button banks[NUM_BANKS][MAX_BUTTONS];
  // How often has the currentButton been quickly pressed
  uint8_t currentCount = 0;
  // Index of the currently pressed button (or -1 if no button is being pressed)
  int8_t currentButton = -1;
  // Index of the last button that has been, or is currently, pressed for this pedal
  int8_t lastButton = -1;
  // Last-selected bank when pressing a button
  uint8_t currentBank = 0;

  // Applies a button change. If bank is to change, first
  // a call to onButton(oldBank, -1) should be made.
  // [button] should be >0 if a button is pressed, or -1 on no button.
  void onButton(VirtualButtonConfig &config, uint8_t bank, int8_t button);

  void onIdle(VirtualButtonConfig &config);
};

struct VirtualButtonConfig {
  static constexpr uint32_t SHORT_PRESS_TIME = 300;

  VirtualButtonPedal pedals[NUM_PEDALS];
  VirtualButtonState states[MAX_STATES];
  uint16_t numStates = 0;

  uint8_t lastPedal;
  uint32_t lastTime;

  int16_t findStateIdx(const VirtualButtonAction &a) const;

  VirtualButtonState &findState(VirtualButtonAction &a) {
    int16_t idx = findStateIdx(a);
    // should never be -1, this is only called when a state is available.
    return (idx == -1) ? states[0] : states[findStateIdx(a)];
  }

  const VirtualButtonState &findState(const VirtualButtonAction &a) {
    int16_t idx = findStateIdx(a);
    // should never be -1, this is only called when a state is available.
    return (idx == -1) ? states[0] : states[findStateIdx(a)];
  }

  void onReconfigure();

  void reset() {
    for (int i = 0; i < NUM_PEDALS; i++) {
      pedals[i] = {};
    }
    numStates = 0;
  }

  void onButton(uint8_t bank, uint8_t pedal, uint8_t button) {
    lastPedal = pedal;
    lastTime = millis();
    pedals[pedal].onButton(*this, bank, button);
  }

  void onIdle() {
    if (lastTime != 0 && millis() > (lastTime + SHORT_PRESS_TIME)) {
      pedals[lastPedal].onIdle(*this);
      lastTime = 0;
    }
  }
};

}
