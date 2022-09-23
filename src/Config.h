/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2022 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#include <ArduinoJson.h>
#include <SPIFFS.h>

#ifdef NVS
#include <Preferences.h>
#include <nvs_flash.h>
Preferences preferences;
#endif

extern String httpUsername;
extern String httpPassword;

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
//  Delete a file from SPIFFS
//

void spiffs_remove_file (const String& filename) {

  if (SPIFFS.exists(filename) && !SPIFFS.remove(filename)) {
    DPRINTLN("SPIFFS: can't remove file %s\n", filename.c_str());
    return;
  }
}


//
//  Remove /globals.json configuration files from SPIFFS
//

void spiffs_remove_globals() {

  spiffs_remove_file("/globals.json");

}

//
//  Remove profile configuration files from SPIFFS
//

void spiffs_remove_profile(byte profile) {

  profile = constrain(profile, 0, PROFILES - 1);

  spiffs_remove_file("/" + String(profile) + "/pedals.json");
  spiffs_remove_file("/" + String(profile) + "/interfaces.json");
  spiffs_remove_file("/" + String(profile) + "/actions.json");
  spiffs_remove_file("/" + String(profile) + "/sequences.json");

}


//
//  Save configuration to SPIFFS file
//

void spiffs_save_config(const String& filename, bool saveActions = true, bool savePedals = true, bool saveInterfaces = true, bool saveSequences = true, bool saveOptions  = true) {

  uint32_t allocated = ESP.getMaxAllocHeap();
  DynamicJsonDocument jdoc(ESP.getMaxAllocHeap());
  DPRINT("Memory allocated for JSON document: %d bytes\n", allocated);

  if (saveOptions) {
    JsonArray jglobals = jdoc.createNestedArray("Globals");
    JsonObject jo = jglobals.createNestedObject();
    jo["Hostname"]            = host;
    jo["BootMode"]            = bootMode;
    jo["Profile"]             = currentProfile;
    jo["SSID"]                = wifiSSID;
    jo["WiFiPassword"]        = wifiPassword;
    jo["SSIDsoftAP"]          = ssidSoftAP;
    jo["PasswordSoftAP"]      = passwordSoftAP;
    jo["HTTPUsername"]        = httpUsername;
    jo["HTTPPassword"]        = httpPassword;
    jo["Theme"]               = theme;
    jo["PressTime"]           = pressTime;
    jo["DoublePressTime"]     = doublePressTime;
    jo["LongPressTime"]       = longPressTime;
    jo["RepeatPressTime"]     = repeatPressTime;
    jo["EncoderSesitivity"]   = encoderSensitivity;
    jo["FlipScreen"]          = flipScreen;
    jo["TapDanceMode"]        = tapDanceMode;
    jo["RepeatOnBankSwitch"]  = repeatOnBankSwitch;
    jo["TapDanceBank"]        = tapDanceBank;
    jo["Leds"]                = LEDS;
    switch (rgbOrder) {
      case RGB:
        jo["RGBOrder"]        = "RGB";
        break;
      case RBG:
        jo["RGBOrder"]        = "RBG";
       break;
      case GRB:
        jo["RGBOrder"]        = "GRB";
        break;
      case GBR:
        jo["RGBOrder"]        = "GBR";
        break;
      case BRG:
        jo["RGBOrder"]        = "BRG";
        break;
      case BGR:
        jo["RGBOrder"]        = "BGR";
        break;
    }
    jo["LedsOnBrightness"]    = ledsOnBrightness;
    jo["LedsOffBrightness"]   = ledsOffBrightness;
    jo["OSCLocalPort"]        = oscLocalPort;
    jo["OSCRemoteHost"]       = oscRemoteHost;
    jo["OSCRemotePort"]       = oscRemotePort;
    JsonArray jladder = jdoc.createNestedArray("Ladder");
    for (byte s = 0; s < LADDER_STEPS + 1; s++) {
      JsonObject jo = jladder.createNestedObject();
      jo["Step"]            = s + 1;
      jo["Level"]           = ladderLevels[s];
    }
  }

  if (savePedals) {
    JsonArray jpedals = jdoc.createNestedArray("Pedals");
    for (byte p = 0; p < PEDALS; p++) {
      JsonObject jo = jpedals.createNestedObject();
      jo["Pedal"]             = p + 1;
      jo["Mode"]              = pedalModeName[pedals[p].mode];
      jo["InvertPolarity"]    = (pedals[p].invertPolarity == PED_ENABLE);
      jo["PressMode"]         = pedalPressModeName[pedals[p].pressMode];
      jo["AnalogResponse"]    = pedalAnalogResponse[pedals[p].analogResponse];
      jo["Min"]               = pedals[p].expZero;
      jo["Max"]               = pedals[p].expMax;
      jo["Easing"]            = pedals[p].snapMultiplier;
      jo["ActivityThreshold"] = lround(pedals[p].activityThreshold);
      jo["AutoSensing"]       = (pedals[p].autoSensing == PED_ENABLE);
      for (byte b = 0; b < LADDER_STEPS; b++)
        jo[String("LedButton") + String(b)] = (pedals[p].ledbuttons[b] == LEDS ? 0 : pedals[p].ledbuttons[b] + 1);
    }
  }

  if (saveActions) {
    JsonArray jbnames = jdoc.createNestedArray("BankNames");
    for (byte b = 0; b < BANKS; b++) {
      JsonObject jo = jbnames.createNestedObject();
      jo["Bank"]              = b;
      jo["Name"]              = banknames[b];
    }


    JsonArray jactions = jdoc.createNestedArray("Actions");
    for (byte b = 0; b < BANKS; b++) {
      action *act = actions[b];
      while (act != nullptr) {
        char color[8];
        JsonObject jo = jactions.createNestedObject();
        jo["Bank"]            = b;
        jo["Pedal"]           = act->pedal + 1;
        jo["Button"]          = act->button + 1;
        jo["Led"]             = (act->led == LEDS ? 0 : (act->led == 255 ? 255 : act->led + 1));
        snprintf(color, 8, "#%06x", act->color0);
        jo["Color0"]          = color;
        snprintf(color, 8, "#%06x", act->color1);
        jo["Color1"]          = color;
        jo["NameOff"]         = act->tag0;
        jo["NameOn"]          = act->tag1;
        jo["Slot"]            = act->slot;
        jo["Event"]           = eventName[act->event];
        switch (act->midiMessage) {
          case PED_EMPTY:
            jo["Message"] = "None";
            break;
          case PED_PROGRAM_CHANGE:
            jo["Message"] = "Program Change";
            break;
          case PED_CONTROL_CHANGE:
            jo["Message"] = "Control Change";
            break;
          case PED_NOTE_ON:
            jo["Message"] = "Note On";
            break;
          case PED_NOTE_OFF:
            jo["Message"] = "Note Off";
            break;
          case PED_BANK_SELECT_INC:
            jo["Message"] = "Bank Select+";
            break;
          case PED_BANK_SELECT_DEC:
            jo["Message"] = "Bank Select-";
            break;
          case PED_PROGRAM_CHANGE_INC:
            jo["Message"] = "Program Change+";
            break;
          case PED_PROGRAM_CHANGE_DEC:
            jo["Message"] = "Program Change-";
            break;
          case PED_PITCH_BEND:
            jo["Message"] = "Pitch Bend";
            break;
          case PED_CHANNEL_PRESSURE:
            jo["Message"] = "Channel Pressure";
            break;
          case PED_MIDI_START:
            jo["Message"] = "Midi Start";
            break;
          case PED_MIDI_STOP:
            jo["Message"] = "Midi Stop";
            break;
          case PED_MIDI_CONTINUE:
            jo["Message"] = "Midi Continue";
            break;
          case PED_SEQUENCE:
            jo["Message"] = "Sequence";
            break;
          case PED_ACTION_BANK_PLUS:
            jo["Message"] = "Bank+";
            break;
          case PED_ACTION_BANK_MINUS:
            jo["Message"] = "Bank-";
            break;
          case PED_ACTION_START:
            jo["Message"] = "Start";
            break;
          case PED_ACTION_STOP:
            jo["Message"] = "Stop";
            break;
          case PED_ACTION_CONTINUE:
            jo["Message"] = "Continue";
            break;
          case PED_ACTION_TAP:
            jo["Message"] = "Tap";
            break;
          case PED_ACTION_BPM_PLUS:
            jo["Message"] = "BPM+";
            break;
          case PED_ACTION_BPM_MINUS:
            jo["Message"] = "BPM-";
            break;
          case PED_OSC_MESSAGE:
            jo["Message"] = "OSC Message";
            break;
          case PED_ACTION_PROFILE_PLUS:
            jo["Message"] = "Profile+";
            break;
          case PED_ACTION_PROFILE_MINUS:
            jo["Message"] = "Profile-";
            break;
          case PED_ACTION_LED_COLOR:
            jo["Message"] = "Set Led Color";
            break;
          case PED_ACTION_DEVICE_INFO:
            jo["Message"] = "Device Info";
            break;
          case PED_ACTION_POWER_ON_OFF:
            jo["Message"] = "Power On/Off";
            break;
          default:
            jo["Message"] = "None";
            break;
        }
        jo["Channel"]         = act->midiChannel;
        jo["Code"]            = act->midiCode;
        jo["Value1"]          = act->midiValue1;
        jo["Value2"]          = act->midiValue2;
        jo["OSCAddress"]      = act->oscAddress;
        act = act->next;
      }
    }
  }

  if (saveInterfaces) {
    JsonArray jinterfaces = jdoc.createNestedArray("Interfaces");
    for (byte i = 0; i < INTERFACES; i++) {
      JsonObject jo = jinterfaces.createNestedObject();
      jo["Interface"]       = i + 1;
      jo["Name"]            = interfaces[i].name;
      jo["In"]              = (IS_INTERFACE_ENABLED(interfaces[i].midiIn));
      jo["Out"]             = (IS_INTERFACE_ENABLED(interfaces[i].midiOut));
      jo["Thru"]            = (interfaces[i].midiThru  == PED_ENABLE);
      jo["Clock"]           = (interfaces[i].midiClock == PED_ENABLE);
      jo["ShowIncoming"]    = (IS_SHOW_ENABLED(interfaces[i].midiIn));
      jo["ShowOutcoming"]   = (IS_SHOW_ENABLED(interfaces[i].midiOut));
    }
  }

  if (saveSequences) {
    JsonArray jsequences = jdoc.createNestedArray("Sequences");
    for (byte s = 0; s < SEQUENCES; s++) {
      for (byte t = 0; t < STEPS; t++) {
        JsonObject jo = jsequences.createNestedObject();
        jo["Sequence"]        = s + 1;
        jo["Step"]            = t + 1;
        switch (sequences[s][t].midiMessage) {
          case PED_EMPTY:
            jo["Message"] = "None";
            break;
          case PED_PROGRAM_CHANGE:
            jo["Message"] = "Program Change";
            break;
          case PED_CONTROL_CHANGE:
            jo["Message"] = "Control Change";
            break;
          case PED_NOTE_ON:
            jo["Message"] = "Note On";
            break;
          case PED_NOTE_OFF:
            jo["Message"] = "Note Off";
            break;
          case PED_BANK_SELECT_INC:
            jo["Message"] = "Bank Select+";
            break;
          case PED_BANK_SELECT_DEC:
            jo["Message"] = "Bank Select-";
            break;
          case PED_PROGRAM_CHANGE_INC:
            jo["Message"] = "Program Change+";
            break;
          case PED_PROGRAM_CHANGE_DEC:
            jo["Message"] = "Program Change-";
            break;
          case PED_PITCH_BEND:
            jo["Message"] = "Pitch Bend";
            break;
          case PED_CHANNEL_PRESSURE:
            jo["Message"] = "Channel Pressure";
            break;
          case PED_MIDI_START:
            jo["Message"] = "Midi Start";
            break;
          case PED_MIDI_STOP:
            jo["Message"] = "Midi Stop";
            break;
          case PED_MIDI_CONTINUE:
            jo["Message"] = "Midi Continue";
            break;
          case PED_SEQUENCE:
            jo["Message"] = "Sequence";
            break;
          case PED_ACTION_BANK_PLUS:
            jo["Message"] = "Bank+";
            break;
          case PED_ACTION_BANK_MINUS:
            jo["Message"] = "Bank-";
            break;
          case PED_ACTION_START:
            jo["Message"] = "Start";
            break;
          case PED_ACTION_STOP:
            jo["Message"] = "Stop";
            break;
          case PED_ACTION_CONTINUE:
            jo["Message"] = "Continue";
            break;
          case PED_ACTION_TAP:
            jo["Message"] = "Tap";
            break;
          case PED_ACTION_BPM_PLUS:
            jo["Message"] = "BPM+";
            break;
          case PED_ACTION_BPM_MINUS:
            jo["Message"] = "BPM-";
            break;
          case PED_ACTION_PROFILE_PLUS:
            jo["Message"] = "Profile+";
            break;
          case PED_ACTION_PROFILE_MINUS:
            jo["Message"] = "Profile-";
            break;
          case PED_ACTION_LED_COLOR:
            jo["Message"] = "Set Led Color";
            break;
          case PED_ACTION_DEVICE_INFO:
            jo["Message"] = "Device Info";
            break;
          case PED_ACTION_POWER_ON_OFF:
            jo["Message"] = "Power On/Off";
            break;
          default:
            jo["Message"] = "None";
            break;
        }
        switch (sequences[s][t].midiMessage) {
          case PED_ACTION_LED_COLOR:
            jo["Led"]             = sequences[s][t].led + 1;
            char color[8];
            snprintf(color, 8, "#%06x", sequences[s][t].color);
            jo["Color"]           = color;
            break;
          default:
            jo["Channel"]         = sequences[s][t].midiChannel;
            jo["Code"]            = sequences[s][t].midiCode;
            jo["Value"]           = sequences[s][t].midiValue;
            break;
        }
      }
    }
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
//  Save /globals.json configuration files to SPIFFS
//

void spiffs_save_globals() {

  spiffs_save_config("/globals.json", false, false, false, false, true);

}

//
//  Save profile configuration files to SPIFFS
//

void spiffs_save_profile(byte profile) {

  profile = constrain(profile, 0, PROFILES - 1);

  spiffs_save_config("/" + String(profile) + "/pedals.json",     false, true,  false, false, false);
  spiffs_save_config("/" + String(profile) + "/interfaces.json", false, false, true,  false, false);
  spiffs_save_config("/" + String(profile) + "/actions.json",    true,  false, false, false, false);
  spiffs_save_config("/" + String(profile) + "/sequences.json",  false, false, false, true,  false);

}


//
//  Load configuration from SPIFFS file
//

void spiffs_load_config(const String& filename, bool loadActions = true, bool loadPedals = true, bool loadInterfaces = true, bool loadSequences = true, bool loadOptions  = true) {

  uint32_t allocated = ESP.getMaxAllocHeap();
  DynamicJsonDocument jdoc(ESP.getMaxAllocHeap());
  DPRINT("Memory allocated for JSON document: %d bytes\n", allocated);

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
#ifdef DEBUG_ESP_PORT
  serializeJson(jdoc, SERIALDEBUG);
#endif
  DPRINT("\n");

  // Get a reference to the root object
  JsonObject jro = jdoc.as<JsonObject>();
  // Loop through all the key-value pairs in obj
  for (JsonPair jp : jro) {
    if (loadOptions && String(jp.key().c_str()) == String("Globals")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          host                = String((const char *)(jo["Hostname"]       | host.c_str()));
          bootMode            = jo["BootMode"]                             | bootMode;
          currentProfile      = jo["Profile"]                              | currentProfile;
          wifiSSID            = String((const char *)(jo["SSID"]           | wifiSSID.c_str()));
          wifiPassword        = String((const char *)(jo["WiFiPassword"]   | wifiPassword.c_str()));
          ssidSoftAP          = String((const char *)(jo["SSIDsoftAP"]     | ssidSoftAP.c_str()));
          passwordSoftAP      = String((const char *)(jo["PasswordSoftAP"] | passwordSoftAP.c_str()));
          httpUsername        = String((const char *)(jo["HTTPUsername"]   | httpUsername.c_str()));
          httpPassword        = String((const char *)(jo["HTTPPassword"]   | httpPassword.c_str()));
          theme               = String((const char *)(jo["Theme"]          | theme.c_str()));
          pressTime           = jo["PressTime"]                            | pressTime;
          doublePressTime     = jo["DoublePressTime"]                      | doublePressTime;
          longPressTime       = jo["LongPressTime"]                        | longPressTime;
          repeatPressTime     = jo["RepeatPressTime"]                      | repeatPressTime;
          encoderSensitivity  = jo["EncoderSesitivity"]                    | encoderSensitivity;
          flipScreen          = jo["FlipScreen"]                           | flipScreen;
          tapDanceMode        = jo["TapDanceMode"]                         | tapDanceMode;
          repeatOnBankSwitch  = jo["RepeatOnBankSwitch"]                   | repeatOnBankSwitch;
          tapDanceBank        = jo["TapDanceBank"]                         | tapDanceBank;
          String order = jo["RGBOrder"];
          if      (order.equals("RGB")) rgbOrder = RGB;
          else if (order.equals("RBG")) rgbOrder = RBG;
          else if (order.equals("GRB")) rgbOrder = GRB;
          else if (order.equals("GBR")) rgbOrder = GBR;
          else if (order.equals("BRG")) rgbOrder = BRG;
          else if (order.equals("BGR")) rgbOrder = BGR;
          else                          rgbOrder = RGB;
          ledsOnBrightness    = jo["LedsOnBrightness"]                     | ledsOnBrightness;
          ledsOffBrightness   = jo["LedsOffBrightness"]                    | ledsOffBrightness;
          oscLocalPort        = jo["OSCLocalPort"]                         | oscLocalPort;
          oscRemoteHost       = jo["OSCRemoteHost"]                        | oscRemoteHost;
          oscRemotePort       = jo["OSCRemotePort"]                        | oscRemotePort;
        }
      }
    }
    else if (loadOptions && String(jp.key().c_str()) == String("Ladder")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int s = jo["Step"];
          s--;
          s = constrain(s, 0, LADDER_STEPS - 1);
          ladderLevels[s]             = jo["Level"];
        }
      }
    }
    else if (loadPedals && String(jp.key().c_str()) == String("Pedals")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int p = jo["Pedal"];
          p--;
          p = constrain(p, 0, PEDALS - 1);

          pedals[p].mode = PED_NONE;
          for (byte m = 1; m <= PED_ANALOG_LATCH; m++)
            if (pedalModeName[m] == jo["Mode"]) {
              pedals[p].mode = m;
              break;
            }

          pedals[p].invertPolarity  = (jo["InvertPolarity"] ? PED_ENABLE : PED_DISABLE);

          pedals[p].pressMode = 0;
          for (byte m = 0; m <= PED_PRESS_1_2_L; m++)
            if (pedalPressModeName[m] == jo["PressMode"]) {
              pedals[p].pressMode = m;
              break;
            }

          pedals[p].analogResponse = PED_LINEAR;
          for (byte m = 0; m <= PED_ANTILOG; m++)
            if (pedalAnalogResponse[m] == jo["AnalogResponse"]) {
              pedals[p].analogResponse = m;
              break;
            }

          pedals[p].expZero           = jo["Min"];
          pedals[p].expMax            = jo["Max"];
          pedals[p].snapMultiplier    = jo["Easing"];
          pedals[p].activityThreshold = jo["ActivityThreshold"];
          pedals[p].autoSensing       = (jo["AutoSensing"] ? PED_ENABLE : PED_DISABLE);
          for (byte b = 0; b < LADDER_STEPS; b++) {
            pedals[p].ledbuttons[b]   = jo[String("LedButton") + String(b)];
            pedals[p].ledbuttons[b]   = pedals[p].ledbuttons[b] == 0 ? LEDS : pedals[p].ledbuttons[b] - 1;
          }
        }
      }
    }
    else if (loadActions && String(jp.key().c_str()) == String("BankNames")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int b = jo["Bank"];
          b = constrain(b, 0, BANKS - 1);
          strlcpy(banknames[b], jo["Name"] | "", sizeof(banknames[b]));
        }
      }
    }
    else if (loadActions && String(jp.key().c_str()) == String("Actions")) {
      if (jp.value().is<JsonArray>()) {
        delete_actions();
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          unsigned int red, green, blue;
          int b = jo["Bank"];
          b = constrain(b, 0, BANKS - 1);
          action *act = actions[b];
          if (act == nullptr) {
            actions[b] = (action*)malloc(sizeof(action));
            assert(actions[b] != nullptr);
            actions[b]->pedal          = jo["Pedal"];
            actions[b]->pedal--;
            actions[b]->pedal          = constrain(actions[b]->pedal, 0, PEDALS - 1);
            actions[b]->button         = jo["Button"];
            actions[b]->button--;
            actions[b]->button         = constrain(actions[b]->button, 0, LADDER_STEPS - 1);
            actions[b]->led            = jo["Led"];
            actions[b]->led            = (actions[b]->led == 0 ? LEDS : (actions[b]->led == 255 ? 255 : actions[b]->led - 1));
            sscanf(jo["Color0"] | "#000000", "#%02x%02x%02x", &red, &green, &blue);
            actions[b]->color0         = ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);
            sscanf(jo["Color1"] | "#000000", "#%02x%02x%02x", &red, &green, &blue);
            actions[b]->color1         = ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);
            strlcpy(actions[b]->tag0,    jo["NameOff"] | "", sizeof(actions[b]->tag0));
            strlcpy(actions[b]->tag1,    jo["NameOn"]  | "", sizeof(actions[b]->tag1));
            actions[b]->slot           = jo["Slot"];
            actions[b]->event = PED_EVENT_NONE;
            for (byte m = 0; m <= PED_EVENT_NONE; m++)
              if (eventName[m] == jo["Event"]) {
                actions[b]->event = m;
                break;
              }
            String msg = jo["Message"];
            if      (msg.equals("None"))              actions[b]->midiMessage = PED_EMPTY;
            else if (msg.equals("Program Change"))    actions[b]->midiMessage = PED_PROGRAM_CHANGE;
            else if (msg.equals("Control Change"))    actions[b]->midiMessage = PED_CONTROL_CHANGE;
            else if (msg.equals("Note On"))           actions[b]->midiMessage = PED_NOTE_ON;
            else if (msg.equals("Note Off"))          actions[b]->midiMessage = PED_NOTE_OFF;
            else if (msg.equals("Bank Select+"))      actions[b]->midiMessage = PED_BANK_SELECT_INC;
            else if (msg.equals("Bank Select-"))      actions[b]->midiMessage = PED_BANK_SELECT_DEC;
            else if (msg.equals("Program Change+"))   actions[b]->midiMessage = PED_PROGRAM_CHANGE_INC;
            else if (msg.equals("Program Change-"))   actions[b]->midiMessage = PED_PROGRAM_CHANGE_DEC;
            else if (msg.equals("Pitch Bend"))        actions[b]->midiMessage = PED_PITCH_BEND;
            else if (msg.equals("Channel Pressure"))  actions[b]->midiMessage = PED_CHANNEL_PRESSURE;
            else if (msg.equals("Midi Start"))        actions[b]->midiMessage = PED_MIDI_START;
            else if (msg.equals("Midi Stop"))         actions[b]->midiMessage = PED_MIDI_STOP;
            else if (msg.equals("Midi Continue"))     actions[b]->midiMessage = PED_MIDI_CONTINUE;
            else if (msg.equals("Sequence"))          actions[b]->midiMessage = PED_SEQUENCE;
            else if (msg.equals("Bank+"))             actions[b]->midiMessage = PED_ACTION_BANK_PLUS;
            else if (msg.equals("Bank-"))             actions[b]->midiMessage = PED_ACTION_BANK_MINUS;
            else if (msg.equals("Start"))             actions[b]->midiMessage = PED_ACTION_START;
            else if (msg.equals("Stop"))              actions[b]->midiMessage = PED_ACTION_STOP;
            else if (msg.equals("Continue"))          actions[b]->midiMessage = PED_ACTION_CONTINUE;
            else if (msg.equals("Tap"))               actions[b]->midiMessage = PED_ACTION_TAP;
            else if (msg.equals("BPM+"))              actions[b]->midiMessage = PED_ACTION_BPM_PLUS;
            else if (msg.equals("BPM-"))              actions[b]->midiMessage = PED_ACTION_BPM_MINUS;
            else if (msg.equals("OSC Message"))       actions[b]->midiMessage = PED_OSC_MESSAGE;
            else if (msg.equals("Profile+"))          actions[b]->midiMessage = PED_ACTION_PROFILE_PLUS;
            else if (msg.equals("Profile-"))          actions[b]->midiMessage = PED_ACTION_PROFILE_MINUS;
            else if (msg.equals("Set Led Color"))     actions[b]->midiMessage = PED_ACTION_LED_COLOR;
            else if (msg.equals("Device Info"))       actions[b]->midiMessage = PED_ACTION_DEVICE_INFO;
            else if (msg.equals("Power On/Off"))      actions[b]->midiMessage = PED_ACTION_POWER_ON_OFF;
            else                                      actions[b]->midiMessage = PED_EMPTY;
            actions[b]->midiChannel    = jo["Channel"];
            actions[b]->midiCode       = jo["Code"];
            actions[b]->midiValue1     = jo["Value1"];
            actions[b]->midiValue2     = jo["Value2"];
            strlcpy(actions[b]->oscAddress, jo["OSCAddress"] | "", sizeof(actions[b]->oscAddress));
            actions[b]->next           = nullptr;
          }
          else while (act != nullptr) {
                if (act->next == nullptr) {
                  act->next = (action*)malloc(sizeof(action));
                  assert(act->next != nullptr);
                  //if (!act->next) return;
                  act = act->next;
                  act->pedal          = jo["Pedal"];
                  act->pedal--;
                  act->pedal          = constrain(act->pedal, 0, PEDALS - 1);
                  act->button         = jo["Button"];
                  act->button--;
                  act->button         = constrain(act->button, 0, LADDER_STEPS - 1);
                  act->led            = jo["Led"];
                  act->led            = (act->led == 0 ? LEDS : (act->led == 255 ? 255 : act->led - 1));
                  sscanf(jo["Color0"] | "#000000", "#%02x%02x%02x", &red, &green, &blue);
                  act->color0         = ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);
                  sscanf(jo["Color1"] | "#000000", "#%02x%02x%02x", &red, &green, &blue);
                  act->color1         = ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);
                  strlcpy(act->tag0,    jo["NameOff"] | "", sizeof(act->tag0));
                  strlcpy(act->tag1,    jo["NameOn"]  | "", sizeof(act->tag1));
                  act->slot           = jo["Slot"];
                  act->event = PED_EVENT_NONE;
                  for (byte m = 0; m <= PED_EVENT_NONE; m++)
                    if (eventName[m] == jo["Event"]) {
                      act->event = m;
                      break;
                    }
                  String msg = jo["Message"];
                  if      (msg.equals("None"))              act->midiMessage = PED_EMPTY;
                  else if (msg.equals("Program Change"))    act->midiMessage = PED_PROGRAM_CHANGE;
                  else if (msg.equals("Control Change"))    act->midiMessage = PED_CONTROL_CHANGE;
                  else if (msg.equals("Note On"))           act->midiMessage = PED_NOTE_ON;
                  else if (msg.equals("Note Off"))          act->midiMessage = PED_NOTE_OFF;
                  else if (msg.equals("Bank Select+"))      act->midiMessage = PED_BANK_SELECT_INC;
                  else if (msg.equals("Bank Select-"))      act->midiMessage = PED_BANK_SELECT_DEC;
                  else if (msg.equals("Program Change+"))   act->midiMessage = PED_PROGRAM_CHANGE_INC;
                  else if (msg.equals("Program Change-"))   act->midiMessage = PED_PROGRAM_CHANGE_DEC;
                  else if (msg.equals("Pitch Bend"))        act->midiMessage = PED_PITCH_BEND;
                  else if (msg.equals("Channel Pressure"))  act->midiMessage = PED_CHANNEL_PRESSURE;
                  else if (msg.equals("Midi Start"))        act->midiMessage = PED_MIDI_START;
                  else if (msg.equals("Midi Stop"))         act->midiMessage = PED_MIDI_STOP;
                  else if (msg.equals("Midi Continue"))     act->midiMessage = PED_MIDI_CONTINUE;
                  else if (msg.equals("Sequence"))          act->midiMessage = PED_SEQUENCE;
                  else if (msg.equals("Bank+"))             act->midiMessage = PED_ACTION_BANK_PLUS;
                  else if (msg.equals("Bank-"))             act->midiMessage = PED_ACTION_BANK_MINUS;
                  else if (msg.equals("Start"))             act->midiMessage = PED_ACTION_START;
                  else if (msg.equals("Stop"))              act->midiMessage = PED_ACTION_STOP;
                  else if (msg.equals("Continue"))          act->midiMessage = PED_ACTION_CONTINUE;
                  else if (msg.equals("Tap"))               act->midiMessage = PED_ACTION_TAP;
                  else if (msg.equals("BPM+"))              act->midiMessage = PED_ACTION_BPM_PLUS;
                  else if (msg.equals("BPM-"))              act->midiMessage = PED_ACTION_BPM_MINUS;
                  else if (msg.equals("OSC Message"))       act->midiMessage = PED_OSC_MESSAGE;
                  else if (msg.equals("Profile+"))          act->midiMessage = PED_ACTION_PROFILE_PLUS;
                  else if (msg.equals("Profile-"))          act->midiMessage = PED_ACTION_PROFILE_MINUS;
                  else if (msg.equals("Set Led Color"))     act->midiMessage = PED_ACTION_LED_COLOR;
                  else if (msg.equals("Device Info"))       act->midiMessage = PED_ACTION_DEVICE_INFO;
                  else if (msg.equals("Power On/Off"))      act->midiMessage = PED_ACTION_POWER_ON_OFF;
                  else                                      act->midiMessage = PED_EMPTY;
                  act->midiChannel    = jo["Channel"];
                  act->midiCode       = jo["Code"];
                  act->midiValue1     = jo["Value1"];
                  act->midiValue2     = jo["Value2"];
                  strlcpy(act->oscAddress, jo["OSCAddress"] | "", sizeof(act->oscAddress));
                  act->next           = nullptr;
                }
                act = act->next;
              }
        }
      }
    }
    else if (loadInterfaces && String(jp.key().c_str()) == String("Interfaces")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          int i = jo["Interface"];
          i--;
          i = constrain(i, 0, INTERFACES - 1);
          strlcpy(interfaces[i].name, jo["Name"] | "", sizeof(interfaces[i].name));
          interfaces[i].midiIn        = (jo["In"]    ? PED_ENABLE : PED_DISABLE);
          interfaces[i].midiOut       = (jo["Out"]   ? PED_ENABLE : PED_DISABLE);
          interfaces[i].midiThru      = (jo["Thru"]  ? PED_ENABLE : PED_DISABLE);
          interfaces[i].midiClock     = (jo["Clock"] ? PED_ENABLE : PED_DISABLE);
          interfaces[i].midiIn        += (jo["ShowIncoming"]  ? PED_SHOW : 0);
          interfaces[i].midiOut       += (jo["ShowOutcoming"] ? PED_SHOW : 0);
        }
      }
    }
    else if (loadSequences && String(jp.key().c_str()) == String("Sequences")) {
      if (jp.value().is<JsonArray>()) {
        JsonArray ja = jp.value();
        for (JsonObject jo : ja) {
          unsigned int red, green, blue;
          int s = jo["Sequence"];
          s--;
          s = constrain(s, 0, SEQUENCES - 1);
          int t = jo["Step"];
          t--;
          t = constrain(t, 0, STEPS - 1);
          String msg = jo["Message"];
            if      (msg.equals("None"))              sequences[s][t].midiMessage = PED_EMPTY;
            else if (msg.equals("Program Change"))    sequences[s][t].midiMessage = PED_PROGRAM_CHANGE;
            else if (msg.equals("Control Change"))    sequences[s][t].midiMessage = PED_CONTROL_CHANGE;
            else if (msg.equals("Note On"))           sequences[s][t].midiMessage = PED_NOTE_ON;
            else if (msg.equals("Note Off"))          sequences[s][t].midiMessage = PED_NOTE_OFF;
            else if (msg.equals("Bank Select+"))      sequences[s][t].midiMessage = PED_BANK_SELECT_INC;
            else if (msg.equals("Bank Select-"))      sequences[s][t].midiMessage = PED_BANK_SELECT_DEC;
            else if (msg.equals("Program Change+"))   sequences[s][t].midiMessage = PED_PROGRAM_CHANGE_INC;
            else if (msg.equals("Program Change-"))   sequences[s][t].midiMessage = PED_PROGRAM_CHANGE_DEC;
            else if (msg.equals("Pitch Bend"))        sequences[s][t].midiMessage = PED_PITCH_BEND;
            else if (msg.equals("Channel Pressure"))  sequences[s][t].midiMessage = PED_CHANNEL_PRESSURE;
            //else if (msg.equals("Midi Start"))        sequences[s][t].midiMessage = PED_MIDI_START;
            //else if (msg.equals("Midi Stop"))         sequences[s][t].midiMessage = PED_MIDI_STOP;
            //else if (msg.equals("Midi Continue"))     sequences[s][t].midiMessage = PED_MIDI_CONTINUE;
            else if (msg.equals("Sequence"))          sequences[s][t].midiMessage = PED_SEQUENCE;
            //else if (msg.equals("Bank+"))             sequences[s][t].midiMessage = PED_ACTION_BANK_PLUS;
            //else if (msg.equals("Bank-"))             sequences[s][t].midiMessage = PED_ACTION_BANK_MINUS;
            else if (msg.equals("Start"))             sequences[s][t].midiMessage = PED_ACTION_START;
            else if (msg.equals("Stop"))              sequences[s][t].midiMessage = PED_ACTION_STOP;
            else if (msg.equals("Continue"))          sequences[s][t].midiMessage = PED_ACTION_CONTINUE;
            //else if (msg.equals("Tap"))               sequences[s][t].midiMessage = PED_ACTION_TAP;
            //else if (msg.equals("BPM+"))              sequences[s][t].midiMessage = PED_ACTION_BPM_PLUS;
            //else if (msg.equals("BPM-"))              sequences[s][t].midiMessage = PED_ACTION_BPM_MINUS;
            //else if (msg.equals("Profile+"))          sequences[s][t].midiMessage = PED_ACTION_PROFILE_PLUS;
            //else if (msg.equals("Profile-"))          sequences[s][t].midiMessage = PED_ACTION_PROFILE_MINUS;
            else if (msg.equals("Set Led Color"))     sequences[s][t].midiMessage = PED_ACTION_LED_COLOR;
            //else if (msg.equals("Device Info"))       sequences[s][t].midiMessage = PED_ACTION_DEVICE_INFO;
            //else if (msg.equals("Power On/Off"))      sequences[s][t].midiMessage = PED_ACTION_POWER_ON_OFF;
            else                                      sequences[s][t].midiMessage = PED_EMPTY;
          switch (sequences[s][t].midiMessage) {
            case PED_ACTION_LED_COLOR:
              sequences[s][t].led         = jo["Led"];
              sequences[s][t].led         = (sequences[s][t].led == 0 ? LEDS : (sequences[s][t].led == 255 ? 255 : sequences[s][t].led - 1));
              sequences[s][t].led         = constrain(sequences[s][t].led, 0, LEDS);
              sscanf(jo["Color"] | "#000000", "#%02x%02x%02x", &red, &green, &blue);
              sequences[s][t].color       = ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);
              break;
            default:
              sequences[s][t].midiChannel = jo["Channel"];
              sequences[s][t].midiChannel = constrain(sequences[s][t].midiChannel, 1, 16);
              sequences[s][t].midiCode    = jo["Code"];
              sequences[s][t].midiCode    = constrain(sequences[s][t].midiCode,    0, MIDI_RESOLUTION - 1);
              sequences[s][t].midiValue   = jo["Value"];
              sequences[s][t].midiValue    = constrain(sequences[s][t].midiValue,  0, MIDI_RESOLUTION - 1);
              break;
          }
        }
      }
    }
  }
}


//
//  Load /globals.json configuration files from SPIFFS
//

void spiffs_load_globals() {

  spiffs_load_config("/globals.json", false, false, false, false, true);

}

//
//  Load profile configuration files from SPIFFS
//

void spiffs_load_profile(byte profile) {

  profile = constrain(profile, 0, PROFILES - 1);

  spiffs_load_config("/" + String(profile) + "/pedals.json",     false, true,  false, false, false);
  spiffs_load_config("/" + String(profile) + "/interfaces.json", false, false, true,  false, false);
  spiffs_load_config("/" + String(profile) + "/actions.json",    true,  false, false, false, false);
  spiffs_load_config("/" + String(profile) + "/sequences.json",  false, false, false, true,  false);

}


//
//  Load factory deafult value for banks, pedals and interfaces
//
void load_factory_default()
{
  host               = getChipId();
  bootMode           = PED_BOOT_NORMAL;
  wifiSSID           = "";
  wifiPassword       = "";
  ssidSoftAP         = String("Pedalino-") + getChipId();
  passwordSoftAP     = getChipId();
  httpUsername       = "admin";
  httpPassword       = getChipId();
  theme              = "bootstrap";
  currentBank        = 1;
  currentProfile     = 0;
  pressTime          = PED_PRESS_TIME;
  doublePressTime    = PED_DOUBLE_PRESS_TIME;
  longPressTime      = PED_LONG_PRESS_TIME;
  repeatPressTime    = PED_REPEAT_PRESS_TIME;
  encoderSensitivity = 5;
  rgbOrder           = RGB;
  ledsOnBrightness   = 5;
  ledsOffBrightness  = 1;
  tapDanceMode       = false;
  repeatOnBankSwitch = false;
  tapDanceBank       = true;

  for (byte b = 0; b < BANKS; b++) {
    memset(banknames[b], 0, MAXBANKNAME+1);
    actions[b] = nullptr;
  }
  strncpy(banknames[0], "Global", MAXBANKNAME+1);

#ifdef TTGO_T_EIGHT
  for (byte p = 0; p < PEDALS; p++) {
    pedals[p] = {PED_DISABLE,    // autosensing
                 (p < 3 || p > 5) ? PED_MOMENTARY1 : PED_JOG_WHEEL, // mode
                 PED_PRESS_1,    // press mode
                 PED_DISABLE,    // invert polarity
                 0,              // map function
                 ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                 ADC_RESOLUTION * 90 / 100,  // expression pedal max
                 0.01,           // snap multiplier
                 8.0,            // activity threshold
                 LEDS, LEDS, LEDS, LEDS, LEDS, LEDS,
                 0,              // last state of switch 1
                 0,              // last state of switch 2
                 millis(),       // last time switch 1 status changed
                 millis(),       // last time switch 2 status changed
                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                 nullptr,
                 nullptr,
                 nullptr
                };
  }
#else
  for (byte p = 0; p < PEDALS; p++)
    pedals[p] = {PED_DISABLE,    // autosensing
                 PED_MOMENTARY1, // mode
                 PED_PRESS_1,    // press mode
                 PED_DISABLE,    // invert polarity
                 0,              // map function
                 ADC_RESOLUTION * 10 / 100,  // expression pedal zero
                 ADC_RESOLUTION * 90 / 100,  // expression pedal max
                 0.01,           // snap multiplier
                 8.0,            // activity threshold
                 LEDS, LEDS, LEDS, LEDS, LEDS, LEDS,
                 0,              // last state of switch 1
                 0,              // last state of switch 2
                 millis(),       // last time switch 1 status changed
                 millis(),       // last time switch 2 status changed
                 nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                 nullptr,
                 nullptr,
                 nullptr
                };

  pedals[PEDALS-1].pressMode = PED_PRESS_1_2_L;
#ifdef TTGO_T_DISPLAY
  action *act;
  act = actions[0] = (action*)malloc(sizeof(action));
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 2;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_PRESS;
  act->midiMessage  = PED_ACTION_BANK_PLUS;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 1;
  act->midiValue2   = BANKS - 1;
  act->slot         = SLOTS;
  act->oscAddress[0] = 0;
  act->next         = (action*)malloc(sizeof(action));
  act = act->next;
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 1;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_CLICK;
  act->midiMessage  = PED_ACTION_PROFILE_PLUS;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 1;
  act->midiValue2   = PROFILES;
  act->oscAddress[0] = 0;
  act->slot         = SLOTS;
  act->next         = (action*)malloc(sizeof(action));
  act = act->next;
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 1;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_DOUBLE_CLICK;
  act->midiMessage  = PED_ACTION_POWER_ON_OFF;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 0;
  act->midiValue2   = 127;
  act->oscAddress[0] = 0;
  act->slot         = SLOTS;
  act->next         = (action*)malloc(sizeof(action));
  act = act->next;
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 1;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_LONG_PRESS;
  act->midiMessage  = PED_ACTION_DEVICE_INFO;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 0;
  act->midiValue2   = 127;
  act->oscAddress[0] = 0;
  act->slot         = SLOTS;
  act->next         = nullptr;
  create_banks();
#else
  action *act;
  act = actions[0] = (action*)malloc(sizeof(action));
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 1;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_CLICK;
  act->midiMessage  = PED_ACTION_PROFILE_PLUS;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 1;
  act->midiValue2   = PROFILES;
  act->oscAddress[0] = 0;
  act->slot         = SLOTS;
  act->next         = (action*)malloc(sizeof(action));
  act = act->next;
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 1;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_DOUBLE_CLICK;
  act->midiMessage  = PED_ACTION_PROFILE_MINUS;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 0;
  act->midiValue2   = 127;
  act->oscAddress[0] = 0;
  act->slot         = SLOTS;
  act->next         = (action*)malloc(sizeof(action));
  act = act->next;
  act->tag0[0]      = 0;
  act->tag1[0]      = 0;
  act->pedal        = PEDALS - 1;
  act->button       = 0;
  act->led          = LEDS;
  act->color0       = CRGB::Black;
  act->color1       = CRGB::Black;
  act->event        = PED_EVENT_LONG_PRESS;
  act->midiMessage  = PED_ACTION_DEVICE_INFO;
  act->midiChannel  = 1;
  act->midiCode     = 0;
  act->midiValue1   = 0;
  act->midiValue2   = 127;
  act->oscAddress[0] = 0;
  act->slot         = SLOTS;
  act->next         = nullptr;
  create_banks();
#endif
#endif

  for (byte i = 0; i < INTERFACES; i++)
    {
      interfaces[i].midiIn      = PED_ENABLE;
      interfaces[i].midiOut     = PED_ENABLE;
      interfaces[i].midiThru    = PED_DISABLE;
      interfaces[i].midiClock   = PED_DISABLE;
    };
  interfaces[PED_USBMIDI].midiIn = PED_DISABLE;
  interfaces[PED_DINMIDI].midiIn = PED_DISABLE;

  for (byte s = 0; s < SEQUENCES; s++) {
    for (byte t = 0; t < STEPS; t++) {
      sequences[s][t].midiMessage  = PED_EMPTY;
      sequences[s][t].midiChannel  = 1;
      sequences[s][t].midiCode     = 0;
      sequences[s][t].midiValue    = 0;
    }
  }

  // TC-Helicon Switch 6
  ladderLevels[0] = 352;
  ladderLevels[1] = 533;
  ladderLevels[2] = 640;
  ladderLevels[3] = 723;
  ladderLevels[4] = 806;
  ladderLevels[5] = 908;
  ladderLevels[6] = ADC_RESOLUTION - 1;
}

void eeprom_update_device_name(const String& name = getChipId())
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Device Name", name);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Device Name]: %s\n", name.c_str());
#else
  host = name;
  spiffs_save_globals();
#endif
}

void eeprom_update_boot_mode(byte mode = bootMode)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Boot Mode", mode);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Boot Mode]: %d\n", mode);
#else
  spiffs_save_globals();
#endif
}

void eeprom_update_sta_wifi_credentials(const String& ssid = wifiSSID, const String& pass = wifiPassword)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("STA SSID", ssid);
  preferences.putString("STA Password", pass);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][STA SSID]:     %s\n", ssid.c_str());
  DPRINT("[NVS][Global][STA Password]: %s\n", pass.c_str());
#else
  wifiSSID     = ssid;
  wifiPassword = pass;
  spiffs_save_globals();
#endif
}

void eeprom_update_ap_wifi_credentials(const String& ssid = String("Pedalino-") + getChipId(), const String& pass = getChipId())
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("AP SSID", ssid);
  preferences.putString("AP Password", pass);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][AP SSID]:     %s\n", ssid.c_str());
  DPRINT("[NVS][Global][AP Password]: %s\n", pass.c_str());
#else
  ssidSoftAP     = ssid;
  passwordSoftAP = pass;
  spiffs_save_globals();
#endif
}

void eeprom_update_login_credentials(const String& username = "admin", const String& password = getChipId())
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("HTTP Username", username);
  preferences.putString("HTTP Password", password);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][HTTP Username]: %s\n", username.c_str());
  DPRINT("[NVS][Global][HTTP Password]: %s\n", password.c_str());
#else
  httpUsername = username;
  httpPassword = password;
  spiffs_save_globals();
#endif
}

void eeprom_update_current_profile(byte profile = 0)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Current Profile", profile);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Current Profile]: %d\n", profile);
#else
  currentProfile = profile;
  spiffs_save_globals();
#endif
}

void eeprom_update_flip_screen(bool enable = false)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Flip Screen", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Flip Screen]: %d\n", enable);
#else
  flipScreen = enable;
  spiffs_save_globals();
#endif
}

void eeprom_update_tap_dance(bool enable = false)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Tap Dance Mode", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Tap Dance Mode]: %d\n", enable);
#else
  tapDanceMode = enable;
  spiffs_save_globals();
#endif
}

void eeprom_update_repeat_on_bank_switch(bool enable = false)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBool("Bank Switch", enable);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Bank Switch]: %d\n", enable);
#else
  repeatOnBankSwitch = enable;
  spiffs_save_globals();
#endif
}

void eeprom_update_press_time(long p1 = PED_PRESS_TIME,
                              long p2 = PED_DOUBLE_PRESS_TIME,
                              long p3 = PED_LONG_PRESS_TIME,
                              long p4 = PED_REPEAT_PRESS_TIME)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putLong("Single Time", p1);
  preferences.putLong("Double Time", p2);
  preferences.putLong("Long   Time", p3);
  preferences.putLong("Repeat Time", p4);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Single Time]: %ld\n", p1);
  DPRINT("[NVS][Global[Double Time]: %ld\n", p2);
  DPRINT("[NVS][Global[Long   Time]: %ld\n", p3);
  DPRINT("[NVS][Global[Repeat Time]: %ld\n", p4);
#else
  pressTime       = p1;
  doublePressTime = p2;
  longPressTime   = p3;
  repeatPressTime = p4;
  spiffs_save_globals();
#endif
}

void eeprom_update_theme(const String& t = "bootstrap")
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putString("Bootstrap Theme", t);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Bootstrap Theme]: %s\n", t.c_str());
#else
  theme = t;
  spiffs_save_globals();
#endif
}

void eeprom_update_ladder()
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putBytes("Ladder", ladderLevels, sizeof(ladderLevels));
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global][Ladder]:\n");
  for (byte i = 0; i < LADDER_STEPS + 1; i++) {
    DPRINT("Level %d: %d\n", i + 1, ladderLevels[i]);
  }
#else
  spiffs_save_globals();
#endif
}

void eeprom_update_encoder_sensitivity(byte sensitivity = 5)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("Encoder Sensit", sensitivity);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[Encoder Sensit]: %d\n", sensitivity);
#else
  encoderSensitivity = sensitivity;
  spiffs_save_globals();
#endif
}

void eeprom_update_rgb_order(EOrder order = RGB)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("RGB Order", order);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[RGB Order]:  %d\n", order);
#else
  rgbOrder = order;
  spiffs_save_globals();
#endif
}

void eeprom_update_leds_brightness(byte on = 5, byte off = 1)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUChar("LedsOnBright", on);
  preferences.putUChar("LedsOffBright", off);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[LedsOnBright]:  %d\n", on);
  DPRINT("[NVS][Global[LedsOffBright]: %d\n", off);
#else
  ledsOnBrightness  = on,
  ledsOffBrightness = off;
  spiffs_save_globals();
#endif
}

void eeprom_update_osc_parameters(unsigned int localport = 8000, String remotehost = "255.255.255.255", unsigned int remoteport = 9000)
{
#ifdef NVS
  DPRINT("Updating NVS ... ");
  preferences.begin("Global", false);
  preferences.putUInt("OSCLocalPort", localport);
  preferences.putString("OSCRemoteHost", remotehost);
  preferences.putUInt("OSCRemotePort", remoteport);
  preferences.end();
  DPRINT("done\n");
  DPRINT("[NVS][Global[OSCLocalPort]:  %d\n", localport);
  DPRINT("[NVS][Global[OSCRemoteHost]: %s\n", remotehost.c_str());
  DPRINT("[NVS][Global[OSCRemotePort]: %d\n", remoteport);
#else
  oscLocalPort = localport;
  oscRemoteHost = remotehost;
  oscRemotePort = remoteport;
  spiffs_save_globals();
#endif
}

void eeprom_update_profile(byte profile = currentProfile)
{
#ifdef NVS
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

  pedal pedals_copy[PEDALS];
  memcpy(pedals_copy, pedals, sizeof(pedals));
  for (byte i = 0; i < PEDALS; i++) {
    pedals_copy[i].pedalValue[0] = 0;
    pedals_copy[i].pedalValue[1] = 0;
    pedals_copy[i].lastUpdate[0] = 0;
    pedals_copy[i].lastUpdate[1] = 0;
    pedals_copy[i].analogPedal   = nullptr;
    pedals_copy[i].jogwheel      = nullptr;
    pedals_copy[i].buttonConfig  = nullptr;
    for (byte s = 0; s < LADDER_STEPS; s++)
      pedals_copy[i].button[s]   = nullptr;
    if (pedals_copy[i].autoSensing) {
      pedals_copy[i].expZero     = ADC_RESOLUTION - 1;
      pedals_copy[i].expMax      = 0;
    }
  };
  preferences.putBytes("Pedals",      &pedals_copy, sizeof(pedals));
  preferences.putBytes("BankNames",   &banknames,   sizeof(banknames));
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
      preferences.putBytes(label, act, sizeof(action));
      act = act->next;
      i++;
    }
    memset(label, 0, 10);
    sprintf(label, "Size%d", b);
    preferences.putUChar(label, i);
  }
  preferences.end();
  DPRINT(" ... done\n");
#else
  spiffs_save_profile(profile);
#endif
}

void eeprom_update_current_bank(byte profile = currentProfile, byte bank = currentBank)
{
#ifdef NVS
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
  DPRINT(" ...\n");
  preferences.putUChar("Current Bank", bank);
  preferences.end();
  DPRINT("[NVS][Current Bank]:  %d\n", bank);
  DPRINT("done\n");
#else
  currentProfile = profile;
  currentBank    = bank;
  spiffs_save_globals();
#endif
}

void eeprom_read_global()
{
#ifdef NVS
  DPRINT("Reading NVS Global ... ");
  if (preferences.begin("Global", true)) {
    host               = preferences.getString("Device Name");
    bootMode           = preferences.getUChar("Boot Mode");
    wifiSSID           = preferences.getString("STA SSID");
    wifiPassword       = preferences.getString("STA Password");
    ssidSoftAP         = preferences.getString("AP SSID");
    passwordSoftAP     = preferences.getString("AP Password");
    httpUsername       = preferences.getString("HTTP Username");
    httpPassword       = preferences.getString("HTTP Password");
    theme              = preferences.getString("Bootstrap Theme");
    currentProfile     = preferences.getUChar("Current Profile");
    flipScreen         = preferences.getBool("Flip Screen");
    tapDanceMode       = preferences.getBool("Tap Dance Mode");
    repeatOnBankSwitch = preferences.getBool("Bank Switch");
    pressTime          = preferences.getLong("Single Time");
    doublePressTime    = preferences.getLong("Double Time");
    longPressTime      = preferences.getLong("Long   Time");
    repeatPressTime    = preferences.getLong("Repeat Time");
    encoderSensitivity = preferences.getUChar("Encoder Sensit");
    rgbOrder           = (EOrder)preferences.getUChar("RGB Order");
    ledsOnBrightness   = preferences.getUChar("LedsOnBright");
    ledsOffBrightness  = preferences.getUChar("LedsOffBright");
    oscLocalPort       = preferences.getUInt("OSCLocalPort");
    oscRemoteHost      = preferences.getString("OSCRemoteHost");
    oscRemotePort      = preferences.getUInt("OSCRemotePort");
    preferences.getBytes("Ladder", ladderLevels, sizeof(ladderLevels));
    preferences.end();
    DPRINT("done\n");
    DPRINT("[NVS][Global][Device Name]:      %s\n", host.c_str());
    DPRINT("[NVS][Global][Boot Mode]:        %d\n", bootMode);
    DPRINT("[NVS][Global][STA SSID]:         %s\n", wifiSSID.c_str());
    DPRINT("[NVS][Global][STA Password]:     %s\n", wifiPassword.c_str());
    DPRINT("[NVS][Global][AP SSID]:          %s\n", ssidSoftAP.c_str());
    DPRINT("[NVS][Global][AP Password]:      %s\n", passwordSoftAP.c_str());
    DPRINT("[NVS][Global][HTTP Username]:    %s\n", httpUsername.c_str());
    DPRINT("[NVS][Global][HTTP Password]:    %s\n", httpPassword.c_str());
    DPRINT("[NVS][Global][Bootstrap Theme]:  %s\n", theme.c_str());
    DPRINT("[NVS][Global][Current Profile]:  %d\n", currentProfile);
    DPRINT("[NVS][Global][Flip Screen]:      %d\n", flipScreen);
    DPRINT("[NVS][Global][Tap Dance Mode]:   %d\n", tapDanceMode);
    DPRINT("[NVS][Global][Bank Switch]:      %d\n", repeatOnBankSwitch);
    DPRINT("[NVS][Global][Single Time]:      %ld\n", pressTime);
    DPRINT("[NVS][Global][Double Time]:      %ld\n", doublePressTime);
    DPRINT("[NVS][Global][Long   Time]:      %ld\n", longPressTime);
    DPRINT("[NVS][Global][Repeat Time]:      %ld\n", repeatPressTime);
    DPRINT("[NVS][Global][Encoder Sensit]:   %d\n", encoderSensitivity);
    DPRINT("[NVS][Global][RGB Order]:        %d\n", rgbOrder);
    DPRINT("[NVS][Global][LedsOnBright]:     %d\n", ledsOnBrightness);
    DPRINT("[NVS][Global][LedsOffBright]:    %d\n", ledsOffBrightness);
    DPRINT("[NVS][Global][OSCLocalPort]:     %d\n", oscLocalPort);
    DPRINT("[NVS][Global][OSCRemoteHost]:    %s\n", oscRemoteHost.c_str());
    DPRINT("[NVS][Global][OSCRemotePort]:    %d\n", oscRemotePort);
    for (byte i = 0; i < LADDER_STEPS; i++) {
      DPRINT("[NVS][Global][Ladder]:           Ladder %d Level %d\n", i + 1, ladderLevels[i]);
    }
  }
  else {
    DPRINT("NVS open error ... using default values\n");
  }
#else
  spiffs_load_globals();
#endif
}

void eeprom_read_profile(byte profile = currentProfile)
{
#ifdef NVS
  controller_delete();
  delete_actions();
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

  preferences.getBytes("Pedals",      &pedals,      sizeof(pedals));
  for (byte i = 0; i < PEDALS; i++) {
    pedals[i].pedalValue[0] = 0;
    pedals[i].pedalValue[1] = 0;
    pedals[i].lastUpdate[0] = 0;
    pedals[i].lastUpdate[1] = 0;
    pedals[i].analogPedal   = nullptr;
    pedals[i].jogwheel      = nullptr;
    pedals[i].buttonConfig  = nullptr;
    for (byte s = 0; s < LADDER_STEPS; s++)
      pedals[i].button[s]   = nullptr;
    if (pedals[i].autoSensing) {
      pedals[i].expZero     = ADC_RESOLUTION - 1;
      pedals[i].expMax      = 0;
    }
  }
  preferences.getBytes("BankNames",   &banknames,   sizeof(banknames));
  preferences.getBytes("Interfaces",  &interfaces,  sizeof(interfaces));
  preferences.getBytes("Sequences",   &sequences,   sizeof(sequences));
  currentBank         = preferences.getUChar("Current Bank");
  currentMidiTimeCode = preferences.getUChar("Current MTC");

  for (byte b = 0; b < BANKS; b++) {
    char label[10];
    memset(label, 0, 10);
    sprintf(label, "Size%d", b);
    byte action_size = preferences.getUChar(label);
    action *act = nullptr;
    for (byte i = 0; i < action_size; i++) {
      memset(label, 0, 10);
      sprintf(label, "%d.%d", b, i);
      action *a = (action*)malloc(sizeof(action));
      assert(a != nullptr);
      int n = preferences.getBytes(label, a, sizeof(action));
      assert(n == sizeof(action));
      a->next = nullptr;
      if (act == nullptr)
        act = actions[b] = a;
      else {
        act->next = a;
        act = act->next;
      }
    }
  }
  create_banks();
  preferences.end();
  DPRINT("done\n");
#else
  controller_delete();
  delete_actions();
  spiffs_load_profile(profile);
  create_banks();
#endif
}

void eeprom_update_globals()
{
#ifdef NVS
  eeprom_update_device_name(host);
  eeprom_update_boot_mode(bootMode);
  eeprom_update_sta_wifi_credentials(wifiSSID, wifiPassword);
  eeprom_update_ap_wifi_credentials(ssidSoftAP, passwordSoftAP);
  eeprom_update_login_credentials(httpUsername, httpPassword);
  eeprom_update_theme(theme);
  eeprom_update_current_profile(currentProfile);
  eeprom_update_flip_screen(flipScreen);
  eeprom_update_tap_dance(tapDanceMode);
  eeprom_update_repeat_on_bank_switch(repeatOnBankSwitch);
  eeprom_update_press_time(pressTime, doublePressTime, longPressTime, repeatPressTime);
  eeprom_update_ladder();
  eeprom_update_rgb_order(rgbOrder);
  eeprom_update_encoder_sensitivity(encoderSensitivity);
  eeprom_update_leds_brightness(ledsOnBrightness, ledsOffBrightness);
  eeprom_update_osc_parameters(oscLocalPort, oscRemoteHost, oscRemotePort);
#else
  spiffs_save_globals();
#endif
}

void eeprom_update_all()
{
  eeprom_update_globals();
  for (byte p = 0; p < PROFILES; p++)
    eeprom_update_profile(p);
}

void eeprom_initialize()
{
#ifdef NVS
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
  eeprom_update_login_credentials();
  eeprom_update_theme();
  eeprom_update_current_profile();
  eeprom_update_flip_screen();
  eeprom_update_tap_dance();
  eeprom_update_repeat_on_bank_switch();
  eeprom_update_press_time();
  eeprom_update_ladder();
  eeprom_update_encoder_sensitivity();
  eeprom_update_rgb_order();
  eeprom_update_leds_brightness();
  eeprom_update_osc_parameters();
  for (byte p = 0; p < PROFILES; p++)
    eeprom_update_profile(p);
#else
  load_factory_default();
  spiffs_remove_globals();
  for (byte p = 0; p < PROFILES; p++)
    spiffs_remove_profile(p);
  spiffs_save_globals();
  for (byte p = 0; p < PROFILES; p++)
    spiffs_save_profile(p);
#endif
}

void eeprom_init_or_erase()
{
  load_factory_default();
#ifdef NVS
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
#endif
}