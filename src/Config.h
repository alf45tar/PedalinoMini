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
#include <nvs_flash.h>

Preferences preferences;

//
//  Load factory deafult value for banks, pedals and interfaces
//
void load_factory_default()
{
  wifiSSID = "";
  wifiPassword = "";
  theme = "bootstrap";
  blynk_set_token("");

  for (byte p = 0; p < PEDALS-2; p++)
    pedals[p] = {PED_MIDI,       // function
                 PED_ENABLE,     // autosensing
                 PED_MOMENTARY1, // mode
                 PED_PRESS_1,    // press mode
                 PED_DISABLE,    // invert polarity
                 0,              // map function
                 ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                 ADC_RESOLUTION * 90 / 100,  // expression pedal max
                 0,              // last state of switch 1
                 0,              // last state of switch 2
                 millis(),       // last time switch 1 status changed
                 millis(),       // last time switch 2 status changed
                 nullptr, nullptr, nullptr, nullptr, nullptr
                };

  pedals[PEDALS-2] = {PED_MIDI,       // function
                      PED_ENABLE,     // autosensing
                      PED_ANALOG,     // mode
                      PED_NONE,       // press mode
                      PED_ENABLE,     // invert polarity
                      0,              // map function
                      ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                      ADC_RESOLUTION * 90 / 100,  // expression pedal max
                      0,              // last state of switch 1
                      0,              // last state of switch 2
                      millis(),       // last time switch 1 status changed
                      millis(),       // last time switch 2 status changed
                      nullptr, nullptr, nullptr, nullptr, nullptr
                      };

  pedals[PEDALS-1] = {PED_MIDI,       // function
                      PED_ENABLE,     // autosensing
                      PED_ANALOG,     // mode
                      PED_NONE,       // press mode
                      PED_ENABLE,     // invert polarity
                      0,              // map function
                      ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                      ADC_RESOLUTION * 90 / 100,  // expression pedal max
                      0,              // last state of switch 1
                      0,              // last state of switch 2
                      millis(),       // last time switch 1 status changed
                      millis(),       // last time switch 2 status changed
                      nullptr, nullptr, nullptr, nullptr, nullptr
                      };

  for (byte b = 0; b < BANKS; b = b + 2) {
    banks[b][0].pedalName[0] = 'A';
    banks[b][0].pedalName[1] = 0;
    banks[b][0].midiMessage  = PED_NOTE_ON_OFF;
    banks[b][0].midiChannel  = (b + 2) / 2;
    banks[b][0].midiCode     = 60;  // C3
    banks[b][0].midiValue1   = 100;
    banks[b][0].midiValue2   = 0;
    banks[b][0].midiValue3   = 0;

    banks[b][1].pedalName[0] = 'B';
    banks[b][1].pedalName[1] = 0;
    banks[b][1].midiMessage  = PED_NOTE_ON_OFF;
    banks[b][1].midiChannel  = (b + 2) / 2;
    banks[b][1].midiCode     = 62;  // D3
    banks[b][1].midiValue1   = 100;
    banks[b][1].midiValue2   = 0;
    banks[b][1].midiValue3   = 0;

    banks[b][2].pedalName[0] = 'C';
    banks[b][2].pedalName[1] = 0;
    banks[b][2].midiMessage  = PED_NOTE_ON_OFF;
    banks[b][2].midiChannel  = (b + 2) / 2;
    banks[b][2].midiCode     = 64;  // E3
    banks[b][2].midiValue1   = 100;
    banks[b][2].midiValue2   = 0;
    banks[b][2].midiValue3   = 0;

    banks[b][3].pedalName[0] = 'D';
    banks[b][3].pedalName[1] = 0;
    banks[b][3].midiMessage  = PED_NOTE_ON_OFF;
    banks[b][3].midiChannel  = (b + 2) / 2;
    banks[b][3].midiCode     = 65;  // F3
    banks[b][3].midiValue1   = 100;
    banks[b][3].midiValue2   = 0;
    banks[b][3].midiValue3   = 0;

    banks[b][4].pedalName[0] = 'E';
    banks[b][4].pedalName[1] = 0;
    banks[b][4].midiMessage  = PED_CONTROL_CHANGE;
    banks[b][4].midiChannel  = (b + 2) / 2;
    banks[b][4].midiCode     = 12;  // Effect Controller 1
    banks[b][4].midiValue1   = 0;
    banks[b][4].midiValue2   = 0;
    banks[b][4].midiValue3   = 0;

    banks[b][5].pedalName[0] = 'F';
    banks[b][5].pedalName[1] = 0;
    banks[b][5].midiMessage  = PED_CONTROL_CHANGE;
    banks[b][5].midiChannel  = (b + 2) / 2;
    banks[b][5].midiCode     = 13;  // Effect Controller 2
    banks[b][5].midiValue1   = 0;
    banks[b][5].midiValue2   = 0;
    banks[b][5].midiValue3   = 0;

    banks[b+1][0].pedalName[0] = 'A';
    banks[b+1][0].pedalName[1] = 0;
    banks[b+1][0].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][0].midiChannel  = (b + 2) / 2;
    banks[b+1][0].midiCode     = 20;  // Undefined
    banks[b+1][0].midiValue1   = 127;
    banks[b+1][0].midiValue2   = 0;
    banks[b+1][0].midiValue3   = 0;

    banks[b+1][1].pedalName[0] = 'B';
    banks[b+1][1].pedalName[1] = 0;
    banks[b+1][1].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][1].midiChannel  = (b + 2) / 2;
    banks[b+1][1].midiCode     = 21;  // Undefined
    banks[b+1][1].midiValue1   = 127;
    banks[b+1][1].midiValue2   = 0;
    banks[b+1][1].midiValue3   = 0;

    banks[b+1][2].pedalName[0] = 'C';
    banks[b+1][2].pedalName[1] = 0;
    banks[b+1][2].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][2].midiChannel  = (b + 2) / 2;
    banks[b+1][2].midiCode     = 22;  // Undefined
    banks[b+1][2].midiValue1   = 127;
    banks[b+1][2].midiValue2   = 0;
    banks[b+1][2].midiValue3   = 0;

    banks[b+1][3].pedalName[0] = 'D';
    banks[b+1][3].pedalName[1] = 0;
    banks[b+1][3].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][3].midiChannel  = (b + 2) / 2;
    banks[b+1][3].midiCode     = 23;  // Undefined
    banks[b+1][3].midiValue1   = 127;
    banks[b+1][3].midiValue2   = 0;
    banks[b+1][3].midiValue3   = 0;

    banks[b+1][4].pedalName[0] = 'E';
    banks[b+1][4].pedalName[1] = 0;
    banks[b+1][4].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][4].midiChannel  = (b + 2) / 2;
    banks[b+1][4].midiCode     = 12;  // Effect Controller 1
    banks[b+1][4].midiValue1   = 0;
    banks[b+1][4].midiValue2   = 0;
    banks[b+1][4].midiValue3   = 0;

    banks[b+1][5].pedalName[0] = 'F';
    banks[b+1][5].pedalName[1] = 0;
    banks[b+1][5].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][5].midiChannel  = (b + 2) / 2;
    banks[b+1][5].midiCode     = 13;  // Effect Controller 2
    banks[b+1][5].midiValue1   = 0;
    banks[b+1][5].midiValue2   = 0;
    banks[b+1][5].midiValue3   = 0;
  }

  for (byte i = 0; i < INTERFACES; i++) 
    {
      interfaces[i].midiIn      = PED_ENABLE;
      interfaces[i].midiOut     = PED_ENABLE;
      interfaces[i].midiThru    = PED_DISABLE;
      interfaces[i].midiRouting = PED_ENABLE;
      interfaces[i].midiClock   = PED_DISABLE;
    };

  for (byte s = 0; s < SEQUENCES; s++) {
    for (byte t = 0; t < STEPS; t++) {
      sequences[s][t].midiMessage  = PED_NONE;
      sequences[s][t].midiChannel  = 1;
      sequences[s][t].midiCode     = 0;
      sequences[s][t].midiValue1   = 0;
      sequences[s][t].midiValue2   = 0;
      sequences[s][t].midiValue3   = 0;
    }
  }
}

void eeprom_update_device_name(String name = host)
{
  DPRINT("Updating NVS ...\n");
  preferences.begin("Global", false);
  preferences.putString("Device Name", name);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Device Name]: %s\n", name.c_str());
}

void eeprom_update_wifi_credentials(String ssid = "", String pass = "")
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("SSID", ssid);
  preferences.putString("Password", pass);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][SSID]:     %s\n", ssid.c_str());
  DPRINT("[NVS][Global][Password]: %s\n", pass.c_str());
}

void eeprom_update_current_profile(byte profile = currentProfile)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Current Profile", profile);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Current Profile]: %d\n", profile);
}

void eeprom_update_repeat_on_bank_switch_enable(bool enable = true)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Bank Switch", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Bank Switch]: %d\n", enable);
}

void eeprom_update_blynk_cloud_enable(bool enable = true)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Blynk Cloud", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Blynk Cloud]: %d\n", enable);
}

void eeprom_update_blynk_auth_token(String token = "")
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Blynk Token", token);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Blynk Token]: %s\n", token.c_str());
}

void eeprom_update_theme(String theme = "bootstrap")
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Bootstrap Theme", theme);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Bootstrap Theme]: %s\n", theme.c_str());
}

void eeprom_update_profile(byte profile = currentProfile)
{
  DPRINT("Updating NVS Profile ");

  switch (profile) {
    case 0:
      preferences.begin("A", false);
      DPRINT("A");
      break;
    case 1:
      preferences.begin("B", false);
      DPRINT("B");
      break;
    case 2:
      preferences.begin("C", false);
      DPRINT("C");
      break;
  }
  preferences.putBytes("Banks", &banks, sizeof(banks));
  preferences.putBytes("Pedals", &pedals, sizeof(pedals));
  preferences.putBytes("Interfaces", &interfaces, sizeof(interfaces));
  preferences.putBytes("Sequences", &sequences, sizeof(sequences));
  preferences.putUChar("Current Bank", currentBank);
  preferences.putUChar("Current MTC", currentMidiTimeCode);
  preferences.putString("SSID", wifiSSID);
  preferences.putString("Password", wifiPassword);
  preferences.end();

  DPRINT(" ... done\n");

  blynk_refresh();
}

void eeprom_read_global()
{
  DPRINT("Reading NVS Global ... ");
  preferences.begin("Global", true);
  host           = preferences.getString("Device Name");
  wifiSSID       = preferences.getString("SSID");
  wifiPassword   = preferences.getString("Password");
  theme          = preferences.getString("Bootstrap Theme");
  currentProfile = preferences.getUChar("Current Profile");
  repeatOnBankSwitch = preferences.getBool("Bank Switch");
  preferences.getBool("Blynk Cloud") ? blynk_enable() : blynk_disable();
  blynk_set_token(preferences.getString("Blynk Token"));
  preferences.end();
  DPRINT("done\n");
}

void eeprom_read_profile(byte profile = currentProfile)
{
  // Delete previous setup
  for (byte i = 0; i < PEDALS; i++) {
    delete pedals[i].debouncer[0];
    delete pedals[i].debouncer[1];
    delete pedals[i].footSwitch[0];
    delete pedals[i].footSwitch[1];
    delete pedals[i].analogPedal;
  }

  DPRINT("Reading NVS Profile ");
  switch (profile) {
    case 0:
      preferences.begin("A", true);
      DPRINT("A");
      break;
    case 1:
      preferences.begin("B", true);
      DPRINT("B");
      break;
    case 2:
      preferences.begin("C", true);
      DPRINT("C");
      break;
  }
  DPRINT(" ... ");
  preferences.getBytes("Banks", &banks, sizeof(banks));
  preferences.getBytes("Pedals", &pedals, sizeof(pedals));
  preferences.getBytes("Interfaces", &interfaces, sizeof(interfaces));
  preferences.getBytes("Sequences", &sequences, sizeof(sequences));
  currentBank         = preferences.getUChar("Current Bank");
  currentMidiTimeCode = preferences.getUChar("Current MTC");
  preferences.end();

  DPRINT("done\n");

  for (byte i = 0; i < PEDALS; i++) {
    pedals[i].pedalValue[0] = 0;
    pedals[i].pedalValue[1] = 0;
    pedals[i].lastUpdate[0] = millis();
    pedals[i].lastUpdate[1] = millis();
    pedals[i].debouncer[0]  = nullptr;
    pedals[i].debouncer[1]  = nullptr;
    pedals[i].footSwitch[0] = nullptr;
    pedals[i].footSwitch[1] = nullptr;
    pedals[i].analogPedal   = nullptr;
  };
  
  blynk_refresh();
}

void eeprom_initialize()
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
  eeprom_update_device_name();
  eeprom_update_wifi_credentials();
  eeprom_update_theme();
  eeprom_update_current_profile(0);
  eeprom_update_repeat_on_bank_switch_enable(false);
  eeprom_update_blynk_cloud_enable(false);
  eeprom_update_blynk_auth_token();
  load_factory_default();
  for (byte p = 0; p < PROFILES; p++) {
    currentBank = p;
    eeprom_update_profile(p);
  }
  currentBank = 1;
}

void eeprom_init_or_erase()
{
  load_factory_default();
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      // NVS partition was truncated and needs to be erased
      // Retry nvs_flash_init
      DPRINT("Unable to mount NVS partition");
      DPRINT("Formatting NVS ... ");
      ESP_ERROR_CHECK(nvs_flash_erase());
      DPRINT("done\n");
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}