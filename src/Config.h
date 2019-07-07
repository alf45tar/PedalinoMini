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

#include <Preferences.h>

Preferences preferences;

void eeprom_initialize_to_zero()
{
  // Remove all preferences under the opened namespace
  preferences.begin("Global", false);
  preferences.clear();
  preferences.end();
  preferences.begin("A", false);
  preferences.clear();
  preferences.end();
  preferences.begin("B", false);
  preferences.clear();
  preferences.end();
  preferences.begin("C", false);
  preferences.clear();
  preferences.end();
}

//
//  Load factory deafult value for banks, pedals and interfaces
//
void load_factory_default()
{
  wifiSSID = "";
  wifiPassword = "";
  theme = "bootstrap";
  blynk_set_token("");

  for (byte b = 0; b < BANKS; b++)
    for (byte p = 0; p < PEDALS; p++)
      switch (b % 4)
      {
        case 0:
          banks[b][p].pedalName[0] = 0;
          banks[b][p].midiMessage  = PED_PROGRAM_CHANGE;
          banks[b][p].midiChannel  = (byte)(b / 4 + 1);
          banks[b][p].midiCode     = (byte)(b / 4 * 16 + p);
          banks[b][p].midiValue1   = 127;
          banks[b][p].midiValue2   = 0;
          banks[b][p].midiValue3   = 65;
          break;

        case 1:
          banks[b][p].pedalName[0] = 0;
          banks[b][p].midiMessage  = PED_CONTROL_CHANGE;
          banks[b][p].midiChannel  = (byte)(b / 4 + 1);
          banks[b][p].midiCode     = (byte)(b / 4 * 16 + p);
          banks[b][p].midiValue1   = 127;
          banks[b][p].midiValue2   = 0;
          banks[b][p].midiValue3   = 65;
          break;

        case 2:
          banks[b][p].pedalName[0] = 0;
          banks[b][p].midiMessage  = PED_NOTE_ON_OFF;
          banks[b][p].midiChannel  = (byte)(b / 4 + 1);
          banks[b][p].midiCode     = (byte)(b / 4 * 16 + p);
          banks[b][p].midiValue1   = 127;
          banks[b][p].midiValue2   = 0;
          banks[b][p].midiValue3   = 65;
          break;

        case 3:
          banks[b][p].pedalName[0] = 0;
          banks[b][p].midiMessage  = PED_PITCH_BEND;
          banks[b][p].midiChannel  = (byte)(b / 4 + 1);
          banks[b][p].midiCode     = (byte)(b / 4 * 16 + p);
          banks[b][p].midiValue1   = 127;
          banks[b][p].midiValue2   = 0;
          banks[b][p].midiValue3   = 65;
          break;
      }

  for (byte p = 0; p < PEDALS; p++)
    pedals[p] = {PED_MIDI,       // function
                 1,              // autosensing disabled
                 PED_MOMENTARY1, // mode
                 PED_PRESS_1,    // press mode
                 0,              // invert polarity disabled
                 0,              // map function
                 ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                 ADC_RESOLUTION * 90 / 100,  // expression pedal max
                 0,              // last state of switch 1
                 0,              // last state of switch 2
                 millis(),       // last time switch 1 status changed
                 millis(),       // last time switch 2 status changed
                 nullptr, nullptr, nullptr, nullptr, nullptr
                };

  for (byte i = 0; i < INTERFACES; i++) 
    {
      interfaces[i].midiIn      = PED_ENABLE;
      interfaces[i].midiOut     = PED_ENABLE;
      interfaces[i].midiThru    = PED_DISABLE;
      interfaces[i].midiRouting = PED_ENABLE;
      interfaces[i].midiClock   = PED_DISABLE;
    };
}

void eeprom_update_device_name(String name)
{
  DPRINT("Updating NVS ...\n");
  preferences.begin("Global", false);
  preferences.putString("Device Name", name);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Device Name]: %s\n", name.c_str());
}

void eeprom_update_wifi_credentils(String ssid, String pass)
{
  char profile = 'A';

  DPRINT("Updating NVS ... ");
  switch (currentProfile) {
    case 0:
      preferences.begin("A", false);
      profile = 'A';
      break;
    case 1:
      preferences.begin("B", false);
      profile = 'B';
      break;
    case 2:
      preferences.begin("C", false);
      profile = 'C';
      break;
  }
  preferences.putString("SSID", ssid);
  preferences.putString("Password", pass);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][%c][SSID]:     %s\n", profile, ssid.c_str());
  DPRINT("[NVS][%c][Password]: %s\n", profile, pass.c_str());
}

void eeprom_update_current_profile(byte profile)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Current Profile", currentProfile);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Current Profile]: %d\n", currentProfile);
}

void eeprom_update_blynk_cloud_enable(bool enable)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Blynk Cloud", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Blynk Cloud]: %d\n", enable);
}

void eeprom_update_blynk_auth_token(String token)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Blynk Token", token);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Blynk Token]: %s\n", token.c_str());
}

void eeprom_update_theme(String theme)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Bootstrap Theme", theme);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Bootstrap Theme]: %s\n", theme.c_str());
}

void eeprom_update()
{
  DPRINT("Updating NVS ... ");

  preferences.begin("Global", false);
  preferences.putString("Device Name", host);
  preferences.putString("Bootstrap Theme", theme);
  preferences.putBool("Blynk Cloud", blynk_enabled());
  preferences.putString("Blynk Token", blynk_get_token());
  preferences.putUChar("Current Profile", currentProfile);
  preferences.end();

  switch (currentProfile) {
    case 0:
      preferences.begin("A", false);
      break;
    case 1:
      preferences.begin("B", false);
      break;
    case 2:
      preferences.begin("C", false);
      break;
  }
  preferences.putBytes("Banks", &banks, sizeof(banks));
  preferences.putBytes("Pedals", &pedals, sizeof(pedals));
  preferences.putBytes("Interfaces", &interfaces, sizeof(interfaces));
  preferences.putUChar("Current Bank", currentBank);
  preferences.putUChar("Current Interf", currentInterface);
  preferences.putUChar("Current MTC", currentMidiTimeCode);
  preferences.putString("SSID", wifiSSID);
  preferences.putString("Password", wifiPassword);
  preferences.end();

  DPRINT("done\n");

  blynk_refresh();
}

void eeprom_read()
{
  load_factory_default();

  DPRINT("Reading NVS ... ");

  preferences.begin("Global", true);
  host  = preferences.getString("Device Name");
  theme = preferences.getString("Bootstrap Theme");
  preferences.getBool("Blynk Cloud") ? blynk_enable() : blynk_disable();
  blynk_set_token(preferences.getString("Blynk Token"));
  currentProfile = preferences.getUChar("Current Profile");
  preferences.end();

  switch (currentProfile) {
    case 0:
      preferences.begin("A", true);
      break;
    case 1:
      preferences.begin("B", true);
      break;
    case 2:
      preferences.begin("C", true);
      break;
  }
  preferences.getBytes("Banks", &banks, sizeof(banks));
  preferences.getBytes("Pedals", &pedals, sizeof(pedals));
  preferences.getBytes("Interfaces", &interfaces, sizeof(interfaces));
  currentBank         = preferences.getUChar("Current Bank");
  currentInterface    = preferences.getUChar("Current Interf");
  currentMidiTimeCode = preferences.getUChar("Current MTC");
  wifiSSID            = preferences.getString("SSID");
  wifiPassword        = preferences.getString("Password");
  preferences.end();

  DPRINT("done\n");

  for (byte p = 0; p < PEDALS; p++) {
    pedals[p].pedalValue[0] = 0;
    pedals[p].pedalValue[1] = 0;
    pedals[p].lastUpdate[0] = millis();
    pedals[p].lastUpdate[1] = millis();
    pedals[p].debouncer[0]  = nullptr;
    pedals[p].debouncer[1]  = nullptr;
    pedals[p].footSwitch[0] = nullptr;
    pedals[p].footSwitch[1] = nullptr;
    pedals[p].analogPedal   = nullptr;
  };
  
  blynk_refresh();
}