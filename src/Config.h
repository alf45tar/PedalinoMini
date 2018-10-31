/*  __________           .___      .__  .__                   ___ ________________    ___
    \______   \ ____   __| _/____  |  | |__| ____   ____     /  / \__    ___/     \   \  \
     |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \   /  /    |    | /  \ /  \   \  \
     |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> ) (  (     |    |/    Y    \   )  )
     |____|    \___  >____ |(____  /____/__|___|  /\____/   \  \    |____|\____|__  /  /  /
                   \/     \/     \/             \/           \__\                 \/  /__/
                                                                  (c) 2018 alf45star
                                                          https://github.com/alf45tar/Pedalino
*/

#include <EEPROM.h>

#define SIGNATURE "Pedalino(TM)"
#define EEPROM_VERSION  1                 // Increment each time you change the eeprom structure
#define EEPROM_SIZE     4096

//
//  Initialize EEPROM
//

void eeprom_init()
{
/*
  You need to call EEPROM.begin(size) before you start reading or writing,
  size being the number of bytes you want to use.
  
  ESP8266: size can be anywhere between 4 and 4096 bytes.
  ESP32: need to define AT LEAST a flash partition for EEPROM with "eeprom" name

  EEPROM.write does not write to flash immediately, instead you must call EEPROM.commit()
  whenever you wish to save changes to flash. EEPROM.end() will also commit,
  and will release the RAM copy of EEPROM contents.
*/

#ifdef ARDUINO_ARCH_ESP32
  if (!EEPROM.begin(EEPROM_SIZE)) {
    DPRINT("Failed to initialise %d bytes for EEPROM\n", EEPROM.length());
    DPRINT("Restarting...\n");
    delay(1000);
    ESP.restart();
  }
#else
  EEPROM.begin(EEPROM_SIZE);
#endif
}

//
//  Load factory deafult value for banks, pedals and interfaces
//
void load_factory_default()
{
  for (byte b = 0; b < BANKS; b++)
    for (byte p = 0; p < PEDALS; p++)
      switch (b % 4)
      {
        case 0:
          banks[b][p] = {PED_PROGRAM_CHANGE,     // MIDI message
                         (byte)(b / 4 + 1),      // MIDI channel
                         (byte)(b / 4 * 16 + p), // MIDI code
                         127,
                         0,
                         65
                      };
          break;

        case 1:
          banks[b][p] = {PED_CONTROL_CHANGE,     // MIDI message
                         (byte)(b / 4 + 1),      // MIDI channel
                         (byte)(b / 4 * 16 + p), // MIDI code
                         127,
                         0,
                         65
                      };
          break;

        case 2:
          banks[b][p] = {PED_NOTE_ON_OFF,             // MIDI message
                         (byte)(b / 4 + 1),           // MIDI channel
                         (byte)(b / 4 * 16 + p + 24), // MIDI code
                         127,
                         0,
                         65
                      };
          break;

        case 3:
          banks[b][p] = {PED_PITCH_BEND,         // MIDI message
                         (byte)(b / 4 + 1),      // MIDI channel
                         (byte)(b / 4 * 16 + p), // MIDI code
                         127,
                         0,
                         65
                      };
          break;
      }

  for (byte p = 0; p < PEDALS; p++)
    pedals[p] = {PED_MIDI,       // function
                 1,              // autosensing disabled
                 PED_MOMENTARY1, // mode
                 PED_PRESS_1,    // press mode
                 0,              // invert polarity disabled
                 0,              // map function
                 50,             // expression pedal zero
                 930,            // expression pedal max
                 0,              // last state of switch 1
                 0,              // last state of switch 2
                 millis(),       // last time switch 1 status changed
                 millis(),       // last time switch 2 status changed
                 nullptr, nullptr, nullptr, nullptr, nullptr
                };

  /*
    pedals[7].function  = PED_START;
    pedals[7].mode      = PED_MOMENTARY1;
    pedals[8].function  = PED_STOP;
    pedals[8].mode      = PED_MOMENTARY1;
    pedals[9].function  = PED_CONTINUE;
    pedals[9].mode      = PED_MOMENTARY1;
    pedals[10].function = PED_TAP;
    pedals[10].mode     = PED_MOMENTARY1;
    pedals[11].function = PED_BANK_PLUS;
    pedals[11].mode     = PED_MOMENTARY2;
    pedals[12].function = PED_MENU;
    pedals[12].mode     = PED_MOMENTARY3;
  */
  /*
    pedals[11].mode = PED_MOMENTARY2;
    pedals[12].mode = PED_MOMENTARY3;
  */
  pedals[5].function = PED_MIDI;
  pedals[5].mode = PED_ANALOG;

  for (byte i = 0; i < INTERFACES; i++)
    interfaces[i] = {
    PED_ENABLE,  // MIDI IN
    PED_ENABLE,  // MIDI OUT
    PED_DISABLE, // MIDI THRU
    PED_ENABLE,  // MIDI routing
    PED_DISABLE  // MIDI clock
  };

#ifndef NOLCD
  for (byte c = 0; c < IR_CUSTOM_CODES; c++)
    ircustomcode[c] = 0xFFFFFE;
#endif
}

//
//  Write current profile to EEPROM
//
void update_current_profile_eeprom()
{
  int offset = 0;

  DPRINT("Updating EEPROM ...\n");

  EEPROM.put(offset, SIGNATURE);
  offset += sizeof(SIGNATURE);
  EEPROM.put(offset, EEPROM_VERSION);
  offset += sizeof(byte);

  EEPROM.put(offset, currentProfile);
  offset += sizeof(byte);
  DPRINT("Current profile:   %d\n", currentProfile);

  EEPROM.commit();
}

//
//  Write current configuration to EEPROM
//
void update_eeprom()
{
  int offset = 0;

  DPRINT("Updating EEPROM ...\n");

  EEPROM.put(offset, SIGNATURE);
  offset += sizeof(SIGNATURE);
  EEPROM.put(offset, EEPROM_VERSION);
  offset += sizeof(byte);

  EEPROM.put(offset, currentProfile);
  offset += sizeof(byte);
  DPRINT("Current profile:   %d\n", currentProfile);

  // Jump to profile
  offset += currentProfile * EEPROM.length() / PROFILES;

  EEPROM.put(offset, SIGNATURE);
  offset += sizeof(SIGNATURE);
  EEPROM.put(offset, EEPROM_VERSION);
  offset += sizeof(byte);

  for (byte b = 0; b < BANKS; b++)
    for (byte p = 0; p < PEDALS; p++)
    {
      EEPROM.put(offset, banks[b][p].midiMessage);
      offset += sizeof(byte);
      EEPROM.put(offset, banks[b][p].midiChannel);
      offset += sizeof(byte);
      EEPROM.put(offset, banks[b][p].midiCode);
      offset += sizeof(byte);
      EEPROM.put(offset, banks[b][p].midiValue1);
      offset += sizeof(byte);
      EEPROM.put(offset, banks[b][p].midiValue2);
      offset += sizeof(byte);
      EEPROM.put(offset, banks[b][p].midiValue3);
      offset += sizeof(byte);
    }

  for (byte p = 0; p < PEDALS; p++)
  {
    EEPROM.put(offset, pedals[p].function);
    offset += sizeof(byte);
    EEPROM.put(offset, pedals[p].autoSensing);
    offset += sizeof(byte);
    EEPROM.put(offset, pedals[p].mode);
    offset += sizeof(byte);
    EEPROM.put(offset, pedals[p].pressMode);
    offset += sizeof(byte);
    EEPROM.put(offset, pedals[p].invertPolarity);
    offset += sizeof(byte);
    EEPROM.put(offset, pedals[p].mapFunction);
    offset += sizeof(byte);
    EEPROM.put(offset, pedals[p].expZero);
    offset += sizeof(int);
    EEPROM.put(offset, pedals[p].expMax);
    offset += sizeof(int);
  }

  for (byte i = 0; i < INTERFACES; i++)
  {
    EEPROM.put(offset, interfaces[i].midiIn);
    offset += sizeof(byte);
    EEPROM.put(offset, interfaces[i].midiOut);
    offset += sizeof(byte);
    EEPROM.put(offset, interfaces[i].midiThru);
    offset += sizeof(byte);
    EEPROM.put(offset, interfaces[i].midiRouting);
    offset += sizeof(byte);
    EEPROM.put(offset, interfaces[i].midiClock);
    offset += sizeof(byte);
  }

  EEPROM.put(offset, currentBank);
  offset += sizeof(byte);
  DPRINT("Current bank:      %d\n", currentBank);

  EEPROM.put(offset, currentPedal);
  offset += sizeof(byte);
  DPRINT("Current pedal:     %d\n", currentPedal);

  EEPROM.put(offset, currentInterface);
  offset += sizeof(byte);
  DPRINT("Current interface: %d\n", currentInterface);

  EEPROM.put(offset, currentMidiTimeCode);
  offset += sizeof(byte);
  DPRINT("Current MTC:       %d\n", currentMidiTimeCode);

#ifndef NOLCD
  EEPROM.put(offset, backlight);
  offset += sizeof(byte);
  DPRINT("Backlight:         %d\n", backlight);

  for (byte c = 0; c < IR_CUSTOM_CODES; c++)
  {
    EEPROM.put(offset, ircustomcode[c]);
    offset += sizeof(unsigned long);
  }
#endif

#ifdef ARDUINO_ARCH_ESP32
  EEPROM.writeString(offset, wifiSSID);
  offset += wifiSSID.length() + 1;
  EEPROM.writeString(offset, wifiPassword);
  offset += wifiPassword.length() + 1;
  DPRINT("SSID     : %s\n", wifiSSID.c_str());
  DPRINT("Password : %s\n", wifiPassword.c_str());
#endif

  EEPROM.commit();

  blynk_refresh();
}

//
//  Read configuration from EEPROM
//
void read_eeprom()
{
  int offset = 0;
  char signature[sizeof(SIGNATURE) + 1];
  byte saved_version;

  load_factory_default();

  EEPROM.get(offset, signature);
  offset += sizeof(SIGNATURE);
  EEPROM.get(offset, saved_version);
  offset += sizeof(byte);

  DPRINT("EEPROM signature: %s\n", signature);
  DPRINT("EEPROM version  : %d\n", saved_version);

  if ((strcmp(signature, SIGNATURE) != 0) || (saved_version != EEPROM_VERSION))
    return;

  DPRINT("Reading EEPROM ...\n");

  EEPROM.get(offset, currentProfile);
  currentProfile = constrain(currentProfile, 0, PROFILES - 1);
  offset += sizeof(byte);
  DPRINT("Current profile:   %d\n", currentProfile);

  // Jump to profile
  offset += currentProfile * EEPROM.length() / PROFILES;

  EEPROM.get(offset, signature);
  offset += sizeof(SIGNATURE);
  EEPROM.get(offset, saved_version);
  offset += sizeof(byte);

  if ((strcmp(signature, SIGNATURE) != 0) || (saved_version != EEPROM_VERSION))
    return;

  for (byte b = 0; b < BANKS; b++)
    for (byte p = 0; p < PEDALS; p++)
    {
      EEPROM.get(offset, banks[b][p].midiMessage);
      offset += sizeof(byte);
      EEPROM.get(offset, banks[b][p].midiChannel);
      offset += sizeof(byte);
      EEPROM.get(offset, banks[b][p].midiCode);
      offset += sizeof(byte);
      EEPROM.get(offset, banks[b][p].midiValue1);
      offset += sizeof(byte);
      EEPROM.get(offset, banks[b][p].midiValue2);
      offset += sizeof(byte);
      EEPROM.get(offset, banks[b][p].midiValue3);
      offset += sizeof(byte);
    }

  for (byte p = 0; p < PEDALS; p++)
  {
    EEPROM.get(offset, pedals[p].function);
    offset += sizeof(byte);
    EEPROM.get(offset, pedals[p].autoSensing);
    offset += sizeof(byte);
    EEPROM.get(offset, pedals[p].mode);
    offset += sizeof(byte);
    EEPROM.get(offset, pedals[p].pressMode);
    offset += sizeof(byte);
    EEPROM.get(offset, pedals[p].invertPolarity);
    offset += sizeof(byte);
    EEPROM.get(offset, pedals[p].mapFunction);
    offset += sizeof(byte);
    EEPROM.get(offset, pedals[p].expZero);
    offset += sizeof(int);
    EEPROM.get(offset, pedals[p].expMax);
    offset += sizeof(int);
  }

  for (byte i = 0; i < INTERFACES; i++)
  {
    EEPROM.get(offset, interfaces[i].midiIn);
    offset += sizeof(byte);
    EEPROM.get(offset, interfaces[i].midiOut);
    offset += sizeof(byte);
    EEPROM.get(offset, interfaces[i].midiThru);
    offset += sizeof(byte);
    EEPROM.get(offset, interfaces[i].midiRouting);
    offset += sizeof(byte);
    EEPROM.get(offset, interfaces[i].midiClock);
    offset += sizeof(byte);
  }

  EEPROM.get(offset, currentBank);
  currentBank = constrain(currentBank, 0, BANKS - 1);
  offset += sizeof(byte);
  DPRINT("Current bank:      %d\n", currentBank);

  EEPROM.get(offset, currentPedal);
  currentPedal = constrain(currentPedal, 0, PEDALS - 1);
  offset += sizeof(byte);
  DPRINT("Current pedal:     %d\n", currentPedal);

  EEPROM.get(offset, currentInterface);
  currentInterface = constrain(currentInterface, 0, INTERFACES - 1);
  offset += sizeof(byte);
  DPRINT("Current interface: %d\n", currentInterface);

  EEPROM.get(offset, currentMidiTimeCode);
  offset += sizeof(byte);
  DPRINT("Current MTC:       %d\n", currentMidiTimeCode);

#ifndef NOLCD
  EEPROM.get(offset, backlight);
  offset += sizeof(byte);
  DPRINT("Backlight:         %d\n", backlight);

  for (byte c = 0; c < IR_CUSTOM_CODES; c++)
  {
    EEPROM.get(offset, ircustomcode[c]);
    offset += sizeof(unsigned long);
  }
#endif

#if defined(ARDUINO_ARCH_ESP32) && !defined(NOWIFI)
  wifiSSID = EEPROM.readString(offset);
  offset += wifiSSID.length() + 1;
  wifiPassword = EEPROM.readString(offset);
  offset += wifiPassword.length() + 1;
  DPRINT("SSID     : %s\n", wifiSSID.c_str());
  DPRINT("Password : %s\n", wifiPassword.c_str());
#endif

  blynk_refresh();
}
