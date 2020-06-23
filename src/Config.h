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

#include <Preferences.h>
#include <nvs_flash.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

Preferences preferences;


#ifdef BOARD_HAS_PSRAM
struct SpiRamAllocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }
  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }
};

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;
#endif

//
//  Save configuration to SPIFFS file
//

void spiffs_save_config(String filename) {

  DynamicJsonDocument jdoc(ESP.getMaxAllocHeap());
  DPRINT("Memory allocated for JSON document: %d bytes\n", ESP.getMaxAllocHeap());

  JsonArray jglobals = jdoc.createNestedArray("Globals");
  JsonObject jo = jglobals.createNestedObject();
  jo["Hostname"]            = host;
  jo["BootMode"]            = bootMode;
  jo["SSID"]                = wifiSSID;
  jo["Password"]            = wifiPassword;
  jo["SSIDsoftAP"]          = ssidSoftAP;
  jo["PasswordSoftAP"]      = passwordSoftAP;
  jo["Theme"]               = theme;
  jo["Profile"]             = currentProfile;
  jo["PressTime"]           = pressTime;
  jo["DoublePressTime"]     = doublePressTime;
  jo["LongPressTime"]       = longPressTime;
  jo["RepeatPressTime"]     = repeatPressTime;
  jo["EncoderSesitivity"]   = encoderSensitivity;
  jo["TapDanceMode"]        = tapDanceMode;
  jo["RepeatOnBankSwitch"]  = repeatOnBankSwitch;
  jo["TapDanceBank"]        = tapDanceBank;

  JsonArray jpedals = jdoc.createNestedArray("Pedals");
  for (byte p = 0; p < PEDALS; p++) {
      JsonObject jo = jpedals.createNestedObject();
      jo["Pedal"]           = p + 1;
      jo["Function"]        = pedals[p].function;
      jo["AutoSensing"]     = pedals[p].autoSensing;
      jo["Mode"]            = pedals[p].mode;
      jo["PressMode"]       = pedals[p].pressMode;
      jo["InvertPolarity"]  = pedals[p].invertPolarity;
      jo["MapFunction"]     = pedals[p].mapFunction;
      jo["Min"]             = pedals[p].expZero;
      jo["Max"]             = pedals[p].expMax;
  }

  JsonArray jbnames = jdoc.createNestedArray("BankNames");
  for (byte b = 0; b < BANKS; b++) {
    JsonObject jo = jbnames.createNestedObject();
    jo["Bank"]              = b + 1;
    jo["Name"]              = banknames[b];
  }

  JsonArray jactions = jdoc.createNestedArray("Actions");
  for (byte b = 0; b < BANKS; b++) {
    action *act = actions[b];
    while (act != nullptr) {
      JsonObject jo = jactions.createNestedObject();
      jo["Bank"]            = b + 1;
      jo["Pedal"]           = act->pedal + 1;
      jo["Button"]          = act->button + 1;
      jo["Name"]            = act->name;
      jo["Event"]           = act->event;
      jo["Message"]         = act->midiMessage;
      jo["Channel"]         = act->midiChannel;
      jo["Code"]            = act->midiCode;
      jo["Value1"]          = act->midiValue1;
      jo["Value2"]          = act->midiValue2;
      act = act->next;
    }
  }

  JsonArray jbanks = jdoc.createNestedArray("Banks");
  for (byte b = 0; b < BANKS; b++) {
    for (byte p = 0; p < PEDALS; p++) {
      JsonObject jo = jbanks.createNestedObject();
      jo["Bank"]            = b + 1;
      jo["Pedal"]           = p + 1;
      jo["Name"]            = banks[b][p].pedalName;
      jo["Message"]         = banks[b][p].midiMessage;
      jo["Channel"]         = banks[b][p].midiChannel;
      jo["Code"]            = banks[b][p].midiCode;
      jo["Value1"]          = banks[b][p].midiValue1;
      jo["Value2"]          = banks[b][p].midiValue2;
      jo["Value3"]          = banks[b][p].midiValue3;
    }
  }

  JsonArray jinterfaces = jdoc.createNestedArray("Interfaces");
  for (byte i = 0; i < INTERFACES; i++) {
      JsonObject jo = jinterfaces.createNestedObject();
      jo["Interface"]       = i + 1;
      jo["Name"]            = interfaces[i].name;
      jo["In"]              = interfaces[i].midiIn;
      jo["Out"]             = interfaces[i].midiOut;
      jo["Thru"]            = interfaces[i].midiThru;
      jo["Routing"]         = interfaces[i].midiRouting;
      jo["Clock"]           = interfaces[i].midiClock;
  }

  JsonArray jsequences = jdoc.createNestedArray("Sequences");
  for (byte s = 0; s < SEQUENCES; s++) {
    for (byte t = 0; t < STEPS; t++) {
      JsonObject jo = jsequences.createNestedObject();
      jo["Sequence"]        = s + 1;
      jo["Step"]            = t + 1;
      jo["Message"]         = sequences[s][t].midiMessage;
      jo["Channel"]         = sequences[s][t].midiChannel;
      jo["Code"]            = sequences[s][t].midiCode;
      jo["Value1"]          = sequences[s][t].midiValue1;
      jo["Value2"]          = sequences[s][t].midiValue2;
      jo["Value3"]          = sequences[s][t].midiValue3;
    }
  }

  JsonArray jladder = jdoc.createNestedArray("Ladder");
  for (byte s = 0; s < LADDER_STEPS; s++) {
      JsonObject jo = jladder.createNestedObject();
      jo["Step"]            = s + 1;
      jo["Id"]              = ab[s].id;
      jo["Threshold"]       = ab[s].threshold;
      jo["Tolerance"]       = ab[s].tolerance;
  }

  jdoc.shrinkToFit();
  DPRINT("Memory used by JSON document: %d bytes\n", jdoc.memoryUsage());

  if (SPIFFS.exists(filename) && !SPIFFS.remove(filename)) {
    DPRINTLN("SPIFFS: can't remove file %s\n", filename.c_str());
    return;
  }

  if ((SPIFFS.totalBytes() - SPIFFS.usedBytes()) < (measureJson(jdoc) + 1)) {
    DPRINTLN("SPIFFS: not enough space to write file %s\n", filename.c_str());
    return;
  }

  DPRINT("Writing %s to SPIFFS ... ", filename.c_str());
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    DPRINT("can't open file\n");
    return;
  }

  // Serialize JSON to file
  if (serializeJson(jdoc, file) == 0) {
    DPRINT("serializeJson() failed to write %d bytes\n", measureJson(jdoc));
    file.close();
    return;
  }

  file.close();
  DPRINT("done (%d bytes written)\n", measureJson(jdoc));
}

//
//  Load configuration from SPIFFS file
//

void spiffs_load_config(String filename) {

  for (byte b = 0; b < BANKS; b++) {
    memset(banknames[b], 0, MAXBANKNAME+1);
    action *act = actions[b];
    actions[b] = nullptr;
    while (act != nullptr) {
      action *a = act;
      act = act->next;
      free(a);
    }
  }

  DynamicJsonDocument jdoc(ESP.getMaxAllocHeap());
  DPRINT("Memory allocated for JSON document: %d bytes\n", ESP.getMaxAllocHeap());

  DPRINT("Reading %s from SPIFFS ... ", filename.c_str());
  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    DPRINT("can't open file\n");
    return;
  }

  DeserializationError err = deserializeJson(jdoc, file);
  if (err) {
    DPRINT("deserializeJson() failed with code %s\n", err.c_str());
    return;
  }

  file.close();
  DPRINT("done\n");

  jdoc.shrinkToFit();
  DPRINT("Memory used by JSON document: %d bytes\n", jdoc.memoryUsage());

  // Get a reference to the root object
  JsonObject jro = jdoc.as<JsonObject>();
  // Loop through all the key-value pairs in obj
  for (JsonPair jp : jro) {
    if (String(jp.key().c_str()) == String("Globals")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          host                = String((const char *)jo["Hostname"]);
          bootMode            = jo["BootMode"];
          wifiSSID            = String((const char *)jo["SSID"]);
          wifiPassword        = String((const char *)jo["Password"]);
          ssidSoftAP          = String((const char *)jo["SSIDsoftAP"]);
          passwordSoftAP      = String((const char *)jo["PasswordSoftAP"]);
          theme               = String((const char *)jo["Theme"]);
          currentProfile      = jo["Profile"];
          pressTime           = jo["PressTime"];
          doublePressTime     = jo["DoublePressTime"];
          longPressTime       = jo["LongPressTime"];
          repeatPressTime     = jo["RepeatPressTime"];
          encoderSensitivity  = jo["EncoderSesitivity"];
          tapDanceMode        = jo["TapDanceMode"];
          repeatOnBankSwitch  = jo["RepeatOnBankSwitch"];
          tapDanceBank        = jo["TapDanceBank"];
        }
      }
    }
    else if (String(jp.key().c_str()) == String("Pedals")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int p = jo["Pedal"];
          p--;
          p = constrain(p, 0, PEDALS - 1);
          pedals[p].function        = jo["Function"];
          pedals[p].autoSensing     = jo["AutoSensing"];
          pedals[p].mode            = jo["Mode"];
          pedals[p].pressMode       = jo["PressMode"];
          pedals[p].invertPolarity  = jo["InvertPolarity"];
          pedals[p].mapFunction     = jo["MapFunction"];
          pedals[p].expZero         = jo["Min"];
          pedals[p].expMax          = jo["Max"];
        }
      }
    }
    else if (String(jp.key().c_str()) == String("BankNames")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int b = jo["Bank"];
          b--;
          b = constrain(b, 0, BANKS - 1);
          strlcpy(banknames[b], jo["Name"] | "", sizeof(banknames[b]));
        }
      }
    }
    else if (String(jp.key().c_str()) == String("Actions")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int b = jo["Bank"];
          b--;
          b = constrain(b, 0, BANKS - 1);
          action *act = actions[b];
          if (act == nullptr) {
            actions[b] = (action*)malloc(sizeof(action));
            if (!actions[b]) return;
            actions[b]->pedal          = jo["Pedal"];
            actions[b]->pedal--;
            actions[b]->pedal          = constrain(actions[b]->pedal, 0, PEDALS - 1);
            actions[b]->button         = jo["Button"];
            actions[b]->button--;
            actions[b]->button         = constrain(actions[b]->button, 0, LADDER_STEPS - 1);
            strlcpy(actions[b]->name, jo["Name"] | "", sizeof(actions[b]->name));
            actions[b]->event          = jo["Event"];
            actions[b]->midiMessage    = jo["Message"];
            actions[b]->midiChannel    = jo["Channel"];
            actions[b]->midiCode       = jo["Code"];
            actions[b]->midiValue1     = jo["Value1"];
            actions[b]->midiValue2     = jo["Value2"];
            actions[b]->next           = nullptr;
          }
          else while (act != nullptr) {
                if (act->next == nullptr) {
                  act->next = (action*)malloc(sizeof(action));
                  if (!act->next) return;
                  act = act->next;
                  act->pedal          = jo["Pedal"];
                  act->pedal--;
                  act->pedal          = constrain(act->pedal, 0, PEDALS - 1);
                  act->button         = jo["Button"];
                  act->button--;
                  act->button         = constrain(act->button, 0, LADDER_STEPS - 1);
                  strlcpy(act->name, jo["Name"] | "", sizeof(act->name));
                  act->event          = jo["Event"];
                  act->midiMessage    = jo["Message"];
                  act->midiChannel    = jo["Channel"];
                  act->midiCode       = jo["Code"];
                  act->midiValue1     = jo["Value1"];
                  act->midiValue2     = jo["Value2"];
                  act->next           = nullptr;
                }
                act = act->next;
              }
        }
      }
    }
    else if (String(jp.key().c_str()) == String("Banks")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int b = jo["Bank"];
          b--;
          b = constrain(b, 0, BANKS - 1);
          int p = jo["Pedal"];
          p--;
          p = constrain(p, 0, PEDALS - 1);
          strlcpy(banks[b][p].pedalName, jo["Name"] | "", sizeof(banks[b][p].pedalName));
          banks[b][p].midiMessage   = jo["Message"];
          banks[b][p].midiChannel   = jo["Channel"];
          banks[b][p].midiCode      = jo["Code"];
          banks[b][p].midiValue1    = jo["Value1"];
          banks[b][p].midiValue2    = jo["Value2"];
          banks[b][p].midiValue3    = jo["Value3"];
        }
      }
    }
    else if (String(jp.key().c_str()) == String("Interfaces")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int i = jo["Interface"];
          i--;
          i = constrain(i, 0, INTERFACES - 1);
          strlcpy(interfaces[i].name, jo["Name"] | "", sizeof(interfaces[i].name));
          interfaces[i].midiIn        = jo["In"];
          interfaces[i].midiOut       = jo["Out"];
          interfaces[i].midiThru      = jo["Thru"];
          interfaces[i].midiRouting   = jo["Routing"];
          interfaces[i].midiClock     = jo["Clock"];
        }
      }
    }
    else if (String(jp.key().c_str()) == String("Sequences")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int s = jo["Sequence"];
          s--;
          s = constrain(s, 0, SEQUENCES - 1);
          int t = jo["Step"];
          t--;
          t = constrain(t, 0, STEPS - 1);
          sequences[s][t].midiMessage = jo["Message"];
          sequences[s][t].midiChannel = jo["Channel"];
          sequences[s][t].midiCode    = jo["Code"];
          sequences[s][t].midiValue1  = jo["Value1"];
          sequences[s][t].midiValue2  = jo["Value2"];
          sequences[s][t].midiValue3  = jo["Value3"];
        }
      }
    }
    else if (String(jp.key().c_str()) == String("Ladder")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int s = jo["Step"];
          s--;
          s = constrain(s, 0, LADDER_STEPS - 1);
          ab[s].id            = jo["Id"];
          ab[s].threshold     = jo["Threshold"];
          ab[s].tolerance     = jo["Tolerance"];
        }
      }
    }
  }
}


//
//  Load factory deafult value for banks, pedals and interfaces
//
void load_factory_default()
{
  host               = getChipId();
  bootMode           = PED_BOOT_NORMAL;
#ifdef BLE
  bootMode           = PED_BOOT_BLE;
#endif
#ifdef WIFI
  bootMode           = PED_BOOT_WIFI;
#endif
  wifiSSID           = "";
  wifiPassword       = "";
  ssidSoftAP         = String("Pedalino-") + getChipId();
  passwordSoftAP     = getChipId();
  theme              = "bootstrap";
  currentProfile     = 0;
  pressTime          = PED_PRESS_TIME;
  doublePressTime    = PED_DOUBLE_PRESS_TIME;
  longPressTime      = PED_LONG_PRESS_TIME;
  repeatPressTime    = PED_REPEAT_PRESS_TIME;
  encoderSensitivity = 5;
  tapDanceMode       = false;
  repeatOnBankSwitch = false;
  tapDanceBank       = true;
  blynk_set_token("");

#ifdef TTGO_T_EIGHT
  for (byte p = 0; p < PEDALS; p++) {
    pedals[p] = {PED_MIDI,       // function
                 PED_DISABLE,    // autosensing
                 (p < PEDALS/2) ? PED_MOMENTARY1 : PED_JOG_WHEEL, // mode
                 PED_PRESS_1,    // press mode
                 PED_DISABLE,    // invert polarity
                 0,              // map function
                 ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                 ADC_RESOLUTION * 90 / 100,  // expression pedal max
                 0,              // last state of switch 1
                 0,              // last state of switch 2
                 millis(),       // last time switch 1 status changed
                 millis(),       // last time switch 2 status changed
                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
                };
  }
  for (byte b = 0; b < BANKS; b++) {
    for (byte p = 0; p < PEDALS; p++) {
      banks[b][p].pedalName[0] = 0;
      banks[b][p].midiMessage  = PED_CONTROL_CHANGE;
      banks[b][p].midiChannel  = (b + 1) % 16;
      banks[b][p].midiCode     = (12 + 10*b + p) % 128;
      banks[b][p].midiValue1   = 0;
      banks[b][p].midiValue2   = 63;
      banks[b][p].midiValue3   = 127;
    }
  }
#else
  for (byte p = 0; p < PEDALS-2; p++)
    pedals[p] = {PED_MIDI,       // function
                 PED_DISABLE,    // autosensing
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
                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
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
    banks[b][4].midiValue2   = 63;
    banks[b][4].midiValue3   = 127;

    banks[b][5].pedalName[0] = 'F';
    banks[b][5].pedalName[1] = 0;
    banks[b][5].midiMessage  = PED_CONTROL_CHANGE;
    banks[b][5].midiChannel  = (b + 2) / 2;
    banks[b][5].midiCode     = 13;  // Effect Controller 2
    banks[b][5].midiValue1   = 0;
    banks[b][5].midiValue2   = 63;
    banks[b][5].midiValue3   = 127;

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
    banks[b+1][4].midiValue2   = 63;
    banks[b+1][4].midiValue3   = 127;

    banks[b+1][5].pedalName[0] = 'F';
    banks[b+1][5].pedalName[1] = 0;
    banks[b+1][5].midiMessage  = PED_CONTROL_CHANGE;
    banks[b+1][5].midiChannel  = (b + 2) / 2;
    banks[b+1][5].midiCode     = 13;  // Effect Controller 2
    banks[b+1][5].midiValue1   = 0;
    banks[b+1][5].midiValue2   = 63;
    banks[b+1][5].midiValue3   = 127;
  }
#endif  // TTGO_T_EIGHT

  for (byte b = 0; b < BANKS; b++) {
    memset(banknames[b], 0, MAXBANKNAME+1);
    actions[b] = nullptr;
  }

  for (byte i = 0; i < INTERFACES; i++)
    {
      interfaces[i].midiIn      = PED_ENABLE;
      interfaces[i].midiOut     = PED_ENABLE;
      interfaces[i].midiThru    = PED_DISABLE;
      interfaces[i].midiRouting = PED_DISABLE;
      interfaces[i].midiClock   = PED_DISABLE;
    };
  interfaces[PED_USBMIDI].midiIn = PED_DISABLE;
  interfaces[PED_DINMIDI].midiIn = PED_DISABLE;

  for (byte s = 0; s < SEQUENCES; s++) {
    for (byte t = 0; t < STEPS; t++) {
      sequences[s][t].midiMessage  = PED_EMPTY;
      sequences[s][t].midiChannel  = 1;
      sequences[s][t].midiCode     = 0;
      sequences[s][t].midiValue1   = 0;
      sequences[s][t].midiValue2   = 0;
      sequences[s][t].midiValue3   = 0;
    }
  }

  // TC-Helicon Switch 6
  ab[0].id = 1;
  ab[0].threshold = 497;
  ab[0].tolerance = 82;
  ab[1].id = 2;
  ab[1].threshold = 660;
  ab[1].tolerance = 47;
  ab[2].id = 3;
  ab[2].threshold = 752;
  ab[2].tolerance = 33;
  ab[3].id = 4;
  ab[3].threshold = 816;
  ab[3].tolerance = 31;
  ab[4].id = 5;
  ab[4].threshold = 876;
  ab[4].tolerance = 31;
  ab[5].id = 6;
  ab[5].threshold = 945;
  ab[5].tolerance = 35;
}

void eeprom_update_device_name(String name = getChipId())
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Device Name", name);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Device Name]: %s\n", name.c_str());
}

void eeprom_update_boot_mode(byte mode = bootMode)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Boot Mode", mode);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Boot Mode]: %d\n", mode);
}

void eeprom_update_sta_wifi_credentials(String ssid = wifiSSID, String pass = wifiPassword)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("STA SSID", ssid);
  preferences.putString("STA Password", pass);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][STA SSID]:     %s\n", ssid.c_str());
  DPRINT("[NVS][Global][STA Password]: %s\n", pass.c_str());
}

void eeprom_update_ap_wifi_credentials(String ssid = String("Pedalino-") + getChipId(), String pass = getChipId())
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("AP SSID", ssid);
  preferences.putString("AP Password", pass);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][AP SSID]:     %s\n", ssid.c_str());
  DPRINT("[NVS][Global][AP Password]: %s\n", pass.c_str());
}

void eeprom_update_current_profile(byte profile = 0)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Current Profile", profile);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Current Profile]: %d\n", profile);
}

void eeprom_update_tap_dance(bool enable = false)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Tap Dance Mode", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Tap Dance Mode]: %d\n", enable);
}

void eeprom_update_repeat_on_bank_switch(bool enable = false)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Bank Switch", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Bank Switch]: %d\n", enable);
}

void eeprom_update_press_time(long p1 = PED_PRESS_TIME,
                              long p2 = PED_DOUBLE_PRESS_TIME,
                              long p3 = PED_LONG_PRESS_TIME,
                              long p4 = PED_REPEAT_PRESS_TIME)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putLong("Single Time", p1);
  preferences.putLong("Double Time", p2);
  preferences.putLong("Long   Time", p3);
  preferences.putLong("Repeat Time", p4);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Single Time]: %d\n", p1);
  DPRINT("[NVS][Global[Double Time]: %d\n", p2);
  DPRINT("[NVS][Global[Long   Time]: %d\n", p3);
  DPRINT("[NVS][Global[Repeat Time]: %d\n", p4);
}

void eeprom_update_blynk_cloud_enable(bool enable = false)
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

void eeprom_update_ladder()
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBytes("Ladder", &ab, sizeof(ab));
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Ladder]:\n");
  for (byte i = 0; i < LADDER_STEPS; i++) {
    DPRINT("Ladder %d Threshold %d Tolerance %d\n", ab[i].id, ab[i].threshold, ab[i].tolerance);
  }
}

void eeprom_update_encoder_sensitivity(byte sensitivity = 5)
{
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Encoder Sensit", sensitivity);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Encoder Sensit]: %d\n", sensitivity);
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
  preferences.putBytes("BankNames",   &banknames,   sizeof(banknames));
  preferences.putBytes("Banks",       &banks,       sizeof(banks));
  preferences.putBytes("Pedals",      &pedals,      sizeof(pedals));
  preferences.putBytes("Interfaces",  &interfaces,  sizeof(interfaces));
  preferences.putBytes("Sequences",   &sequences,   sizeof(sequences));
  preferences.putUChar("Current Bank", currentBank);
  preferences.putUChar("Current MTC", currentMidiTimeCode);
  for (byte b = 0; b < BANKS; b++) {
    char    label[10];
    byte    i = 0;
    action *act = actions[b];
    while (act != nullptr) {
      memset(label, 0, 10);
      sprintf(label, "%d.%d", b, i);
      DPRINT("%s\n", label);
      preferences.putBytes(label, act, sizeof(action));
      act = act->next;
      i++;
    }
    memset(label, 0, 10);
    sprintf(label, "Size%d", b);
    preferences.putUChar(label, i);
    DPRINT("%s %d\n", label, i);
  }
  preferences.end();

  DPRINT(" ... done\n");

  blynk_refresh();
}

void eeprom_read_global()
{
  DPRINT("Reading NVS Global ... ");
  if (preferences.begin("Global", true)) {
    host               = preferences.getString("Device Name");
    bootMode           = preferences.getUChar("Boot Mode");
    wifiSSID           = preferences.getString("STA SSID");
    wifiPassword       = preferences.getString("STA Password");
    ssidSoftAP         = preferences.getString("AP SSID");
    passwordSoftAP     = preferences.getString("AP Password");
    theme              = preferences.getString("Bootstrap Theme");
    currentProfile     = preferences.getUChar("Current Profile");
    tapDanceMode       = preferences.getBool("Tap Dance Mode");
    repeatOnBankSwitch = preferences.getBool("Bank Switch");
    pressTime          = preferences.getLong("Single Time");
    doublePressTime    = preferences.getLong("Double Time");
    longPressTime      = preferences.getLong("Long   Time");
    repeatPressTime    = preferences.getLong("Repeat Time");
    encoderSensitivity = preferences.getUChar("Encoder Sensit");
    preferences.getBytes("Ladder", &ab, sizeof(ab));
    preferences.getBool("Blynk Cloud") ? blynk_enable() : blynk_disable();
    blynk_set_token(preferences.getString("Blynk Token"));
    preferences.end();
    DPRINT("done\n");
    DPRINT("[NVS][Global][Device Name]:      %s\n", host.c_str());
    DPRINT("[NVS][Global][Boot Mode]:        %d\n", bootMode);
    DPRINT("[NVS][Global][STA SSID]:         %s\n", wifiSSID.c_str());
    DPRINT("[NVS][Global][STA Password]:     %s\n", wifiPassword.c_str());
    DPRINT("[NVS][Global][AP SSID]:          %s\n", ssidSoftAP.c_str());
    DPRINT("[NVS][Global][AP Password]:      %s\n", passwordSoftAP.c_str());
    DPRINT("[NVS][Global][Bootstrap Theme]:  %s\n", theme.c_str());
    DPRINT("[NVS][Global][Current Profile]:  %d\n", currentProfile);
    DPRINT("[NVS][Global][Tap Dance Mode]:   %d\n", tapDanceMode);
    DPRINT("[NVS][Global][Bank Switch]:      %d\n", repeatOnBankSwitch);
    DPRINT("[NVS][Global][Single Time]:      %d\n", pressTime);
    DPRINT("[NVS][Global][Double Time]:      %d\n", doublePressTime);
    DPRINT("[NVS][Global][Long   Time]:      %d\n", longPressTime);
    DPRINT("[NVS][Global][Repeat Time]:      %d\n", repeatPressTime);
    DPRINT("[NVS][Global][Encoder Sensit]:   %d\n", encoderSensitivity);
    DPRINT("[NVS][Global][Blynk Cloud]:      %d\n", blynk_enabled());
    DPRINT("[NVS][Global][Blynk Token]:      %s\n", blynk_get_token().c_str());
    for (byte i = 0; i < LADDER_STEPS; i++) {
      DPRINT("[NVS][Global][Ladder]:           Ladder %d Threshold %d Tolerance %d\n", ab[i].id, ab[i].threshold, ab[i].tolerance);
    }
  }
  else {
    DPRINT("NVS open error ... using default values\n");
  }
}

void eeprom_read_profile(byte profile = currentProfile)
{
  // Delete previous setup
  for (byte i = 0; i < PEDALS; i++) {
    if (pedals[i].analogPedal   != nullptr) delete pedals[i].analogPedal;
    for (byte s = 0; s < LADDER_STEPS; s++)
      if (pedals[i].button[s]   != nullptr) delete pedals[i].button[s];
    if (pedals[i].buttonConfig  != nullptr) delete pedals[i].buttonConfig;
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
  preferences.getBytes("BankNames",   &banknames,   sizeof(banknames));
  preferences.getBytes("Banks",       &banks,       sizeof(banks));
  preferences.getBytes("Pedals",      &pedals,      sizeof(pedals));
  preferences.getBytes("Interfaces",  &interfaces,  sizeof(interfaces));
  preferences.getBytes("Sequences",   &sequences,   sizeof(sequences));
  currentBank         = preferences.getUChar("Current Bank");
  currentMidiTimeCode = preferences.getUChar("Current MTC");

  for (byte i = 0; i < PEDALS; i++) {
    pedals[i].pedalValue[0] = 0;
    pedals[i].pedalValue[1] = 0;
    pedals[i].lastUpdate[0] = millis();
    pedals[i].lastUpdate[1] = millis();
    pedals[i].analogPedal   = nullptr;
    for (byte s = 0; s < LADDER_STEPS; s++)
      pedals[i].button[s]   = nullptr;
    pedals[i].buttonConfig  = nullptr;
    if (pedals[i].autoSensing) {
      pedals[i].expZero       = ADC_RESOLUTION - 1;
      pedals[i].expMax        = 0;
    }
  };

  for (byte b = 0; b < BANKS; b++) {
    char label[10];
    memset(label, 0, 10);
    sprintf(label, "Size%d", b);
    byte action_size = preferences.getUChar(label);
    action *act = actions[b] = nullptr;
    for (byte i = 0; i < action_size; i++) {
      memset(label, 0, 10);
      sprintf(label, "%d.%d", b, i);
      action *a = (action*)malloc(sizeof(action));
      preferences.getBytes(label, a, sizeof(action));
      a->next = nullptr;
      if (act == nullptr)
        act = actions[b] = a;
      else {
        act->next = a;
        act = act->next;
      }
    }
  }
  preferences.end();
  DPRINT("done\n");
  blynk_refresh();
}

void eeprom_update_all()
{
  eeprom_update_device_name(host);
  eeprom_update_boot_mode(bootMode);
  eeprom_update_sta_wifi_credentials(wifiSSID, wifiPassword);
  eeprom_update_ap_wifi_credentials(ssidSoftAP, passwordSoftAP);
  eeprom_update_theme(theme);
  eeprom_update_current_profile(currentProfile);
  eeprom_update_tap_dance(tapDanceMode);
  eeprom_update_repeat_on_bank_switch(repeatOnBankSwitch);
  eeprom_update_press_time(pressTime, doublePressTime, longPressTime, repeatPressTime);
  eeprom_update_ladder();
  eeprom_update_encoder_sensitivity(encoderSensitivity);
  eeprom_update_blynk_cloud_enable(blynk_enabled());
  eeprom_update_blynk_auth_token(blynk_get_token());
  for (byte p = 0; p < PROFILES; p++)
    eeprom_update_profile(p);
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
  load_factory_default();
  eeprom_update_device_name();
  eeprom_update_boot_mode();
  eeprom_update_sta_wifi_credentials();
  eeprom_update_ap_wifi_credentials();
  eeprom_update_theme();
  eeprom_update_current_profile();
  eeprom_update_tap_dance();
  eeprom_update_repeat_on_bank_switch();
  eeprom_update_press_time();
  eeprom_update_ladder();
  eeprom_update_encoder_sensitivity();
  eeprom_update_blynk_cloud_enable();
  eeprom_update_blynk_auth_token();
  for (byte p = 0; p < PROFILES; p++)
    eeprom_update_profile(p);
}

void eeprom_init_or_erase()
{
  load_factory_default();
  esp_err_t err = nvs_flash_init_partition("nvs");
  switch (err) {
    case ESP_OK:
      DPRINT("'nvs' partition was successfully initialized\n");
      if (preferences.begin("Global", true)) {
        preferences.end();
        break;
      }
      DPRINT("[NVS][Global] not found\n");
    case ESP_ERR_NVS_NO_FREE_PAGES:
    case ESP_ERR_NVS_NEW_VERSION_FOUND:
      // NVS partition was truncated and needs to be erased
      ESP_ERROR_CHECK(nvs_flash_erase());
      DPRINT("'nvs' partition formatted\n");
      ESP_ERROR_CHECK(nvs_flash_init());
      eeprom_initialize();
      break;
    case ESP_ERR_NOT_FOUND:
      DPRINT("'nvs' partition not found\n");
      break;
  }
}