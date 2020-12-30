/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2021 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#ifdef TTGO_T_DISPLAY
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>

TFT_eSPI    display  = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
TFT_eSprite top      = TFT_eSprite(&display);

#endif

#include <WiFi.h>
#include "Images.h"

bool          uiUpdate = true;



// Font generated or edited with the glyphEditor
const uint8_t wifiSignal[] PROGMEM = {
0x14, // Width: 20
0x0A, // Height: 10
0x30, // First char: 48
0x08, // Number of chars: 8
// Jump Table:
0x00, 0x00, 0x11, 0x14, // 48
0x00, 0x11, 0x11, 0x14, // 49
0x00, 0x22, 0x12, 0x14, // 50
0x00, 0x34, 0x16, 0x14, // 51
0x00, 0x4A, 0x1A, 0x14, // 52
0x00, 0x64, 0x1E, 0x14, // 53
0x00, 0x82, 0x22, 0x14, // 54
0x00, 0xA4, 0x26, 0x14, // 55
// Font Data:
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x00, 0x05, 0x00, 0x03, // 48
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x03, // 49
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x83, 0x03, // 50
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x83, 0x03, 0x00, 0x00, 0xC0, 0x03, // 51
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x83, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xE0, 0x03, // 52
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x83, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x03, // 53
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x83, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0xF8, 0x03, // 54
0x03, 0x00, 0x05, 0x00, 0x09, 0x00, 0x11, 0x00, 0xFF, 0x03, 0x11, 0x00, 0x09, 0x03, 0x05, 0x00, 0x83, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x00, 0xFC, 0x03, // 55
};

// Font generated or edited with the glyphEditor
const uint8_t bluetoothSign[] PROGMEM = {
0x08, // Width: 8
0x0A, // Height: 10
0x30, // First char: 48
0x02, // Number of chars: 2
// Jump Table:
0xFF, 0xFF, 0x00, 0x08, // 48
0x00, 0x00, 0x0D, 0x08, // 49
// Font Data:
0x82, 0x00, 0x44, 0x00, 0x28, 0x00, 0xFF, 0x01, 0x11, 0x01, 0xAA, 0x00, 0x44, // 49
};

// Font generated or edited with the glyphEditor
const uint8_t profileSign10[] PROGMEM = {
0x0A, // Width: 10
0x0A, // Height: 10
0x30, // First char: 48
0x03, // Number of chars: 3
// Jump Table:
0x00, 0x00, 0x14, 0x0A, // 48
0x00, 0x14, 0x14, 0x0A, // 49
0x00, 0x28, 0x14, 0x0A, // 50
// Font Data:
0xFE, 0x01, 0x01, 0x02, 0x01, 0x02, 0xF9, 0x02, 0x25, 0x02, 0x25, 0x02, 0xF9, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFE, 0x01, // 48
0xFE, 0x01, 0x01, 0x02, 0x01, 0x02, 0xFD, 0x02, 0x95, 0x02, 0x95, 0x02, 0x79, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFE, 0x01, // 49
0xFE, 0x01, 0x01, 0x02, 0x01, 0x02, 0x79, 0x02, 0x85, 0x02, 0x85, 0x02, 0x49, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFE, 0x01, // 50
};

// Font generated or edited with the glyphEditor
const uint8_t profileSign[] PROGMEM = {
0x0A, // Width: 10
0x0B, // Height: 11
0x30, // First char: 48
0x03, // Number of chars: 3
// Jump Table:
0x00, 0x00, 0x14, 0x0A, // 48
0x00, 0x14, 0x14, 0x0A, // 49
0x00, 0x28, 0x14, 0x0A, // 50
// Font Data:
0xFE, 0x03, 0x01, 0x04, 0x01, 0x04, 0xF9, 0x05, 0x25, 0x04, 0x25, 0x04, 0xF9, 0x05, 0x01, 0x04, 0x01, 0x04, 0xFE, 0x03, // 48
0xFE, 0x03, 0x01, 0x04, 0x01, 0x04, 0xFD, 0x05, 0x25, 0x05, 0x25, 0x05, 0xF9, 0x04, 0x01, 0x04, 0x01, 0x04, 0xFE, 0x03, // 49
0xFE, 0x03, 0x01, 0x04, 0x01, 0x04, 0xF9, 0x04, 0x05, 0x05, 0x05, 0x05, 0x89, 0x04, 0x01, 0x04, 0x01, 0x04, 0xFE, 0x03, // 50
};

// Font generated or edited with the glyphEditor
const uint8_t _midiIcons[] PROGMEM = {
0x14, // Width: 20
0x0A, // Height: 10
0x30, // First char: 48
0x04, // Number of chars: 4
// Jump Table:
0xFF, 0xFF, 0x00, 0x14, // 48
0x00, 0x00, 0x22, 0x14, // 49
0x00, 0x22, 0x28, 0x14, // 50
0x00, 0x4A, 0x21, 0x14, // 51
// Font Data:
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x01, 0x02, 0x01, 0x7E, 0x01, 0x02, 0x01, 0x7C, 0x01, 0x00, 0x01, 0x7E, 0x01, 0x00, 0x01, 0x7A, 0x01, 0x42, 0x01, 0x42, 0x01, 0x3C, 0x01, 0x00, 0x01, 0x7E, 0x01, // 49
0x7A, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x48, 0x00, 0x30, 0x00, 0x00, 0x00, 0x7E, 0x01, 0x02, 0x01, 0x7E, 0x01, 0x02, 0x01, 0x7C, 0x01, 0x00, 0x01, 0x7E, 0x01, 0x00, 0x01, 0x7A, 0x01, 0x42, 0x01, 0x42, 0x01, 0x3C, 0x01, 0x00, 0x01, 0x7E, 0x01, // 50
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x82, 0x00, 0x82, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x4C, 0x00, 0x92, 0x00, 0x92, 0x00, 0x64, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x82, 0x00, 0x82, 0x00, 0x44, // 51
};

// Font generated or edited with the glyphEditor
const uint8_t midiIcons[] PROGMEM = {
0x14, // Width: 20
0x0A, // Height: 10
0x30, // First char: 48
0x04, // Number of chars: 4
// Jump Table:
0xFF, 0xFF, 0x00, 0x14, // 48
0x00, 0x00, 0x26, 0x14, // 49
0x00, 0x26, 0x28, 0x14, // 50
0x00, 0x4E, 0x26, 0x14, // 51
// Font Data:
0x00, 0x00, 0xFF, 0x03, 0xFF, 0x03, 0x81, 0x02, 0xFD, 0x02, 0x81, 0x02, 0xFD, 0x02, 0x81, 0x02, 0xFF, 0x02, 0x81, 0x02, 0xFF, 0x02, 0x85, 0x02, 0xBD, 0x02, 0xBD, 0x02, 0x83, 0x02, 0xFF, 0x02, 0x81, 0x02, 0xFF, 0x03, 0xFF, 0x03, // 49
0x85, 0x03, 0xFF, 0x03, 0x07, 0x02, 0xB7, 0x03, 0xCF, 0x03, 0xFF, 0x03, 0x81, 0x02, 0xFD, 0x02, 0x81, 0x02, 0xFD, 0x02, 0x83, 0x02, 0xFF, 0x02, 0x81, 0x02, 0xFF, 0x02, 0x85, 0x02, 0xBD, 0x02, 0xBD, 0x02, 0xC3, 0x02, 0xFF, 0x02, 0x81, 0x02, // 50
0x00, 0x00, 0xFF, 0x03, 0xFF, 0x03, 0x83, 0x03, 0x7D, 0x03, 0x7D, 0x03, 0x83, 0x03, 0xFF, 0x03, 0xB3, 0x03, 0x6D, 0x03, 0x6D, 0x03, 0x9B, 0x03, 0xFF, 0x03, 0x83, 0x03, 0x7D, 0x03, 0x7D, 0x03, 0xBB, 0x03, 0xFF, 0x03, 0xFF, 0x03, // 51
};
// Font generated or edited with the glyphEditor
const uint8_t batteryIndicator[] PROGMEM = {
0x14, // Width: 20
0x0A, // Height: 10
0x30, // First char: 48
0x04, // Number of chars: 4
// Jump Table:
0x00, 0x00, 0x28, 0x14, // 48
0x00, 0x28, 0x28, 0x14, // 49
0x00, 0x50, 0x28, 0x14, // 50
0x00, 0x78, 0x28, 0x14, // 51
// Font Data:
0x78, 0x00, 0x48, 0x00, 0xFF, 0x03, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFF, 0x03, // 48
0x78, 0x00, 0x48, 0x00, 0xFF, 0x03, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFF, 0x03, // 49
0x78, 0x00, 0x48, 0x00, 0xFF, 0x03, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFF, 0x03, // 50
0x78, 0x00, 0x48, 0x00, 0xFF, 0x03, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFF, 0x03, // 51
};

// Font generated or edited with the glyphEditor
const uint8_t beats4[] PROGMEM = {
0x20, // Width: 32
0x20, // Height: 32
0x30, // First char: 48
0x04, // Number of chars: 4
// Jump Table:
0x00, 0x00, 0x7E, 0x20, // 48
0x00, 0x7E, 0x7F, 0x20, // 49
0x00, 0xFD, 0x7F, 0x20, // 50
0x01, 0x7C, 0x7F, 0x20, // 51
// Font Data:
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFC, 0x7F, 0x00, 0x00, 0xFC, 0x7F, 0x00, 0x00, 0xF8, 0x7F, 0x00, 0x00, 0xF0, 0x7F, 0x00, 0x00, 0xE0, 0x7F, 0x00, 0x00, 0xC0, 0x7F, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x7C, // 48
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x7F, 0xFE, 0x3F, 0xFC, 0x7F, 0xFE, 0x3F, 0xF8, 0x7F, 0xFE, 0x1F, 0xF0, 0x7F, 0xFE, 0x0F, 0xE0, 0x7F, 0xFE, 0x07, 0xC0, 0x7F, 0xFE, 0x03, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x7C, 0x3E, // 49
0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0x03, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x00, 0xFE, 0x0F, 0x00, 0x00, 0xFE, 0x1F, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x7F, 0xFE, 0x3F, 0xFC, 0x7F, 0xFE, 0x3F, 0xF8, 0x7F, 0xFE, 0x1F, 0xF0, 0x7F, 0xFE, 0x0F, 0xE0, 0x7F, 0xFE, 0x07, 0xC0, 0x7F, 0xFE, 0x03, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x7C, 0x3E, // 50
0x00, 0x7C, 0x3E, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0xC0, 0x7F, 0xFE, 0x03, 0xE0, 0x7F, 0xFE, 0x07, 0xF0, 0x7F, 0xFE, 0x0F, 0xF8, 0x7F, 0xFE, 0x1F, 0xFC, 0x7F, 0xFE, 0x3F, 0xFC, 0x7F, 0xFE, 0x3F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x7F, 0xFE, 0x3F, 0xFC, 0x7F, 0xFE, 0x3F, 0xF8, 0x7F, 0xFE, 0x1F, 0xF0, 0x7F, 0xFE, 0x0F, 0xE0, 0x7F, 0xFE, 0x07, 0xC0, 0x7F, 0xFE, 0x03, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x7C, 0x3E, // 51
};

// Font generated or edited with the glyphEditor
const uint8_t beats4off[] PROGMEM = {
0x20, // Width: 32
0x20, // Height: 32
0x30, // First char: 48
0x04, // Number of chars: 4
// Jump Table:
0x00, 0x00, 0x7F, 0x20, // 48
0x00, 0x7F, 0x7F, 0x20, // 49
0x00, 0xFE, 0x7F, 0x20, // 50
0x01, 0x7D, 0x7F, 0x20, // 51
// Font Data:
0x00, 0x7C, 0x3E, 0x00, 0x00, 0x43, 0xC2, 0x00, 0xC0, 0x40, 0x02, 0x03, 0x20, 0x40, 0x02, 0x04, 0x10, 0x40, 0x02, 0x08, 0x08, 0x40, 0x02, 0x10, 0x04, 0x40, 0x02, 0x20, 0x04, 0x40, 0x02, 0x20, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0xFF, 0x7F, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0x01, 0x40, 0x02, 0x80, 0xFD, 0x5F, 0x02, 0x80, 0xFD, 0x5F, 0x02, 0x80, 0xF9, 0x5F, 0x02, 0x80, 0xFA, 0x5F, 0x02, 0x40, 0xF2, 0x5F, 0x02, 0x40, 0xF4, 0x5F, 0x02, 0x20, 0xE4, 0x5F, 0x02, 0x20, 0xC8, 0x5F, 0x02, 0x10, 0x90, 0x5F, 0x02, 0x08, 0x20, 0x5E, 0x02, 0x04, 0xC0, 0x58, 0x02, 0x03, 0x00, 0x43, 0xC2, 0x00, 0x00, 0x7C, 0x3E, // 48
0x00, 0x7C, 0x3E, 0x00, 0x00, 0x43, 0xC2, 0x00, 0xC0, 0x40, 0x02, 0x03, 0x20, 0x40, 0x02, 0x04, 0x10, 0x40, 0x02, 0x08, 0x08, 0x40, 0x02, 0x10, 0x04, 0x40, 0x02, 0x20, 0x04, 0x40, 0x02, 0x20, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0xFF, 0x7F, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0xFA, 0xBF, 0x01, 0x40, 0xFA, 0xBF, 0x01, 0x40, 0xFA, 0x9F, 0x02, 0x40, 0xFA, 0x5F, 0x02, 0x40, 0xFA, 0x4F, 0x04, 0x40, 0xFA, 0x2F, 0x04, 0x40, 0xFA, 0x27, 0x08, 0x40, 0xFA, 0x13, 0x10, 0x40, 0xFA, 0x09, 0x20, 0x40, 0x7A, 0x04, 0xC0, 0x40, 0x1A, 0x03, 0x00, 0x43, 0xC2, 0x00, 0x00, 0x7C, 0x3E, // 49
0x00, 0x7C, 0x3E, 0x00, 0x00, 0x43, 0xC2, 0x00, 0xC0, 0x40, 0x1A, 0x03, 0x20, 0x40, 0x7A, 0x04, 0x10, 0x40, 0xFA, 0x09, 0x08, 0x40, 0xFA, 0x13, 0x04, 0x40, 0xFA, 0x27, 0x04, 0x40, 0xFA, 0x2F, 0x02, 0x40, 0xFA, 0x4F, 0x02, 0x40, 0xFA, 0x5F, 0x01, 0x40, 0xFA, 0x9F, 0x01, 0x40, 0xFA, 0xBF, 0x01, 0x40, 0xFA, 0xBF, 0x01, 0x40, 0x02, 0x80, 0xFF, 0x7F, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x04, 0x40, 0x02, 0x20, 0x04, 0x40, 0x02, 0x20, 0x08, 0x40, 0x02, 0x10, 0x10, 0x40, 0x02, 0x08, 0x20, 0x40, 0x02, 0x04, 0xC0, 0x40, 0x02, 0x03, 0x00, 0x43, 0xC2, 0x00, 0x00, 0x7C, 0x3E, // 50
0x00, 0x7C, 0x3E, 0x00, 0x00, 0x43, 0xC2, 0x00, 0xC0, 0x58, 0x02, 0x03, 0x20, 0x5E, 0x02, 0x04, 0x90, 0x5F, 0x02, 0x08, 0xC8, 0x5F, 0x02, 0x10, 0xE4, 0x5F, 0x02, 0x20, 0xF4, 0x5F, 0x02, 0x20, 0xF2, 0x5F, 0x02, 0x40, 0xFA, 0x5F, 0x02, 0x40, 0xF9, 0x5F, 0x02, 0x80, 0xFD, 0x5F, 0x02, 0x80, 0xFD, 0x5F, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0xFF, 0x7F, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0xFE, 0xFF, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x01, 0x40, 0x02, 0x80, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x04, 0x40, 0x02, 0x20, 0x04, 0x40, 0x02, 0x20, 0x08, 0x40, 0x02, 0x10, 0x10, 0x40, 0x02, 0x08, 0x20, 0x40, 0x02, 0x04, 0xC0, 0x40, 0x02, 0x03, 0x00, 0x43, 0xC2, 0x00, 0x00, 0x7C, 0x3E, // 51
};

// Font generated or edited with the glyphEditor
const uint8_t block[] PROGMEM = {
0x12, // Width: 18
0x12, // Height: 18
0x30, // First char: 48
0x03, // Number of chars: 3
// Jump Table:
0x00, 0x00, 0x36, 0x12, // 48
0x00, 0x36, 0x36, 0x12, // 49
0x00, 0x6C, 0x36, 0x12, // 50
// Font Data:
0xFE, 0xFF, 0x01, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0xFE, 0xFF, 0x01, // 48
0xFE, 0xFF, 0x01, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x03, 0x02, 0x81, 0x07, 0x02, 0x81, 0x07, 0x02, 0x01, 0x03, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0xFE, 0xFF, 0x01, // 49
0xFE, 0xFF, 0x01, 0x01, 0x00, 0x02, 0xF9, 0x7F, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xFD, 0xFF, 0x02, 0xF9, 0x7F, 0x02, 0x01, 0x00, 0x02, 0xFE, 0xFF, 0x01, // 50
};

// Font generated or edited with the glyphEditor
const uint8_t block10x10[] PROGMEM = {
0x0A, // Width: 10
0x0A, // Height: 10
0x30, // First char: 48
0x03, // Number of chars: 3
// Jump Table:
0x00, 0x00, 0x14, 0x0A, // 48
0x00, 0x14, 0x14, 0x0A, // 49
0x00, 0x28, 0x14, 0x0A, // 50
// Font Data:
0xFE, 0x01, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFE, 0x01, // 48
0xFE, 0x01, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x31, 0x02, 0x31, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0xFE, 0x01, // 49
0xFE, 0x01, 0x01, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0x01, 0x02, 0xFE, 0x01, // 50
};


void display_clear()
{
  display.fillScreen(TFT_BLACK);
}

void display_progress_bar_title(String title)
{
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setFreeFont(&FreeSans18pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title, display.width() / 2, display.height() / 2);
}

void display_progress_bar_title2(String title1, String title2)
{
  display.fillScreen(TFT_BLACK);
  display.setTextSize(1);
  display.setFreeFont(&FreeSans18pt7b);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title2, display.width() / 2, display.height() / 2);
  uint16_t h = display.fontHeight(1);
  display.setFreeFont(&FreeSans9pt7b);
  display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display.setTextDatum(BC_DATUM);
  display.drawString(title1, display.width() / 2, display.height() / 2 - h);

}

void display_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress)
{
  height += height % 2;

  uint16_t radius = height / 2;
  uint16_t doubleRadius = 2 * radius;
  uint16_t innerRadius = radius - 2;

  TFT_eSprite bar = TFT_eSprite(&display);

  bar.createSprite(width, height);

  bar.drawCircleHelper(radius,             radius, radius, 0b00001001, TFT_LIGHTGREY);
  bar.drawCircleHelper(width - 1 - radius, radius, radius, 0b00000110, TFT_LIGHTGREY);
  bar.drawFastHLine(radius - 1, 0,          width - doubleRadius + 1, TFT_LIGHTGREY);
  bar.drawFastHLine(radius - 1, height - 1, width - doubleRadius + 1, TFT_LIGHTGREY);

  uint16_t maxProgressWidth = (width - doubleRadius - 1) * progress / 100;

  bar.fillCircle(radius, radius, innerRadius, TFT_ORANGE);
  bar.fillRect(radius + 1, 2, maxProgressWidth, height - 4, TFT_ORANGE);
  bar.fillCircle(radius + maxProgressWidth, radius, innerRadius, TFT_ORANGE);

  bar.pushSprite(x, y);
}

void display_progress_bar_update(unsigned int progress, unsigned int total)
{
  display_progress_bar(0, display.height() / 2 + 8, display.width() - 1, display.height() / 8, 100*progress/total);
}

void display_progress_bar_2_update(unsigned int progress, unsigned int total)
{
  display_progress_bar(0, display.height() - display.height() / 8, display.width() - 1, display.height() / 8, 100*progress/total);
}

void display_progress_bar_2_label(unsigned int label, unsigned int x)
{
  /*
  const String l(label);

  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_10);
  if (x <= display.getStringWidth(l) / 2) {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 42, l);
  }
  else if (x >= (128 - display.getStringWidth(l) / 2)) {
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(display.width() + 1, 42, l);
  }
  else {
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(x, 42, l);
  }
  display.drawLine(x, 53, x, 63);
  display.display();
  */
}

void topOverlay()
{
  static uint64_t timeStamp       = 0;
  static float    battery_voltage = 4.2F;

  if (millis() - timeStamp > 1005 ) {
    timeStamp = millis();

#ifdef WIFI
    if (wifiEnabled) {
      static int signal = WiFi.RSSI();

      top.setSwapBytes(true);
      signal = (4*signal + WiFi.RSSI()) / 5;
      if      (signal < -90) top.pushImage(1, 1, 32, 22, w0);
      else if (signal < -80) top.pushImage(1, 1, 32, 22, w25);
      else if (signal < -70) top.pushImage(1, 1, 32, 22, w50);
      else if (signal < -60) top.pushImage(1, 1, 32, 22, w75);
      else                   top.pushImage(1, 1, 32, 22, w100);
    }
#endif

#ifdef BLE
    top.setSwapBytes(true);
    if (bleMidiConnected)
      if (wifiEnabled) top.pushImage(40, 1, 16, 22, bt);
      else top.pushImage(1, 1, 16, 22, bt);
    else if (wifiEnabled) top.fillRect(40, 1, 16, 22, TFT_WHITE);
      else top.fillRect(1, 1, 16, 22, TFT_WHITE);
#endif

#ifdef BATTERY
    uint16_t v = analogRead(BATTERY_PIN);
    digitalWrite(BATTERY_ADC_EN, LOW);

    float voltage = ((float)v / 1024.0) * 2.0 * 3.3 * (vref / 1000.0);
    if (abs(voltage - battery_voltage) > 0.2F)
      battery_voltage = voltage;
    else
      battery_voltage = (9.0F*battery_voltage + voltage) / 10.0F;
/*
        String vol = "Voltage :" + String(battery_voltage) + "V";
        display.setFreeFont(&FreeSans9pt7b);
        display.setTextColor(TFT_BLACK);
        display.setTextDatum(ML_DATUM);
        display.drawString(vol,  0, 100);
*/
    top.setSwapBytes(true);
    if      (battery_voltage > 4.2F) top.pushImage(display.width() - 50, 1, 50, 22, bcharge);
    else if (battery_voltage > 4.0F) top.pushImage(display.width() - 50, 1, 50, 22, b100);
    else if (battery_voltage > 3.8F) top.pushImage(display.width() - 50, 1, 50, 22, b75);
    else if (battery_voltage > 3.6F) top.pushImage(display.width() - 50, 1, 50, 22, b50);
    else if (battery_voltage > 3.4F) top.pushImage(display.width() - 50, 1, 50, 22, b25);
    else if (battery_voltage > 3.2F) top.pushImage(display.width() - 50, 1, 50, 22, b10);
    else top.pushImage(display.width() - 50, 1, 50, 22, b10);
#endif

  } else if (millis() - timeStamp > 1000) {
#ifdef BATTERY
    /*
      BATTERY_ADC_EN is the ADC detection enable port
      If the USB port is used for power supply, it is turned on by default.
      If it is powered by battery, it needs to be set to high level
    */
    pinMode(BATTERY_ADC_EN, OUTPUT);
    digitalWrite(BATTERY_ADC_EN, HIGH);
  } else if (millis() - timeStamp > 500 && battery_voltage <= 3.2F) {
      top.pushImage(display.width() - 50, 1, 50, 22, b0);
#endif
  }
  top.pushSprite(0, 0);

/*

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(profileSign);
    display->drawString(64 + (scrollingMode ? 10*currentProfile : 0), 0, String(currentProfile));

  if ((millis() >= endMillis2) ||
      (millis() < endMillis2 && MTC.getMode() == MidiTimeCode::SynchroNone)) {

#ifdef BATTERY
  static uint16_t voltage = bat.voltage();
  static uint8_t  level   = bat.level(voltage);
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(batteryIndicator);
    voltage = (199*voltage + bat.voltage()) / 200;
    level   = (199*level + bat.level(voltage)) / 200;

    if      (level == 100) display->drawString(128, 0, String((millis() >> 10) % 4));
    else if (level >   70) display->drawString(128, 0, String(3));
    else if (level >   40) display->drawString(128, 0, String(2));
    else if (level >   10) display->drawString(128, 0, String(1));
    else if ((millis() >> 10) % 2) display->drawString(128, 0, String(0));
    else display->drawString(128, 0, String(4));

    if      (voltage > 3700) display->drawString(128, 0, String((millis() >> 10) % 4));
    else if (voltage > 3600) display->drawString(128, 0, String(3));
    else if (voltage > 3300) display->drawString(128, 0, String(2));
    else if (voltage > 3100) display->drawString(128, 0, String(1));
    else if ((millis() >> 10) % 2) display->drawString(128, 0, String(0));
    else display->drawString(128, 0, String(4));
#endif
  }

  if (millis() < endMillis2) {
    if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
        MTC.getMode() == MidiTimeCode::SynchroClockSlave) {

      display->setFont(ArialMT_Plain_10);
      display->setTextAlignment(TEXT_ALIGN_CENTER);
      display->drawString(64, 0, String(bpm) + "BPM");
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->setFont(block10x10);
      switch (timeSignature) {
        case PED_TIMESIGNATURE_2_4:
          display->drawString( 98, 0, String(0));
          display->drawString(108, 0, String(0));
          break;
        case PED_TIMESIGNATURE_4_4:
          display->drawString( 98, 0, String(0));
          display->drawString(108, 0, String(0));
          display->drawString(118, 0, String(0));
          display->drawString(128, 0, String(0));
          break;
        case PED_TIMESIGNATURE_3_4:
        case PED_TIMESIGNATURE_3_8:
        case PED_TIMESIGNATURE_6_8:
        case PED_TIMESIGNATURE_9_8:
        case PED_TIMESIGNATURE_12_8:
          display->drawString( 98, 0, String(0));
          display->drawString(108, 0, String(0));
          display->drawString(118, 0, String(0));
          break;
      }
      switch (MTC.getBeat()) {
        case 0:
          if (MTC.isPlaying())
            display->drawString(98, 0, String(2));
          else
            display->drawString(98, 0, String(1));
          break;
        case 1:
          if (MTC.isPlaying())
            display->drawString(108, 0, String(2));
          else
            display->drawString(108, 0, String(1));
          break;
        case 2:
          if (MTC.isPlaying())
            display->drawString(118, 0, String(2));
          else
            display->drawString(118, 0, String(1));
          break;
        case 3:
          if (MTC.isPlaying())
            display->drawString(128, 0, String(2));
          else
            display->drawString(128, 0, String(1));
          break;
      }

      //MTC.isPlaying() ? display->setColor(WHITE) : display->setColor(BLACK);
      switch (timeSignature) {
        case PED_TIMESIGNATURE_2_4:
          display->fillRect(64 * MTC.getBeat(), 0, 64, 10);
          break;
        case PED_TIMESIGNATURE_4_4:
          display->drawRect(77 + 13 * 0, 0, 12, 10);
          display->drawRect(77 + 13 * 1, 0, 12, 10);
          display->drawRect(77 + 13 * 2, 0, 12, 10);
          display->drawRect(77 + 13 * 3, 0, 12, 10);
          if (MTC.isPlaying())
            display->fillRect(79 + 13 * MTC.getBeat(), 2,  8,  6);
          else
            display->drawRect(81 + 13 * MTC.getBeat(), 3,  4,  4);
          break;
        case PED_TIMESIGNATURE_3_4:
        case PED_TIMESIGNATURE_3_8:
        case PED_TIMESIGNATURE_6_8:
        case PED_TIMESIGNATURE_9_8:
        case PED_TIMESIGNATURE_12_8:
          display->fillRect(43 * MTC.getBeat(), 0, 42, 10);
          break;
      }
      //display->setColor(WHITE);
    }
    else if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
             MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
      char buf[12];
      sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
      display->setFont(ArialMT_Plain_10);
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->drawString(128, 0, buf);
    }
  }
  */
}

void bottomOverlay()
{
  /*
  if (lastUsed == lastUsedPedal && lastUsed != 0xFF && millis() < endMillis2 && lastPedalName[0] != ':') {
    //byte p = map(pedals[lastUsedPedal].pedalValue[0], 0, MIDI_RESOLUTION - 1, 0, 100);
    int p;
    switch (m1) {

      case midi::ControlChange:
        //p = map(m3, 0, MIDI_RESOLUTION - 1, 0, 100);
        m3 = constrain(m3, rmin, rmax);
        p = map(m3, rmin, rmax, 0, 100);
        display->drawProgressBar(0, 54, 127, 8, p);
        if (lastPedalName[0] != 0) display_progress_bar_2_label(m3, map(p, 0, 100, 3, 124));
        break;

      case midi::PitchBend:
        p = map(((m3 << 7) | m2) + MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MIN, MIDI_PITCHBEND_MAX, -100, 100);
        if ( p >= 0 ) {
          display->drawProgressBar(60, 54, 67, 8, p);
          uint16_t radius = 8 / 2;
          uint16_t xRadius = 0 + radius;
          uint16_t yRadius = 54 + radius;
          uint16_t doubleRadius = 2 * radius;
          display->drawCircleQuads(xRadius, yRadius, radius, 0b00000110);
          display->drawHorizontalLine(xRadius, 54, 68 - doubleRadius);
          display->drawHorizontalLine(xRadius, 54 + 8, 68 - doubleRadius);
          display->drawCircleQuads(0 + 68 - radius, yRadius, radius, 0b00001001);
        }
        else {
          display->drawProgressBar(60, 54, 67, 8, 0);
          uint16_t radius = 8 / 2;
          uint16_t xRadius = 0 + radius;
          uint16_t yRadius = 54 + radius;
          uint16_t doubleRadius = 2 * radius;
          uint16_t innerRadius = radius - 2;
          display->drawCircleQuads(xRadius, yRadius, radius, 0b00000110);
          display->drawHorizontalLine(xRadius, 54, 68 - doubleRadius);
          display->drawHorizontalLine(xRadius, 54 + 8, 68 - doubleRadius);
          display->drawCircleQuads(0 + 68 - radius, yRadius, radius, 0b00001001);
          uint16_t maxProgressWidth = (68 - doubleRadius) * p / 100;
          display->fillCircle(68 + maxProgressWidth - xRadius, yRadius, innerRadius);
          display->fillRect(68 + maxProgressWidth - xRadius + 1, 54 + 2, -maxProgressWidth, 8 - 3);
          display->fillCircle(68 - xRadius, yRadius, innerRadius);
        }
        break;

      case midi::AfterTouchChannel:
        ///p = map(m3, 0, MIDI_RESOLUTION - 1, 0, 100);
        m3 = constrain(m2, rmin, rmax);
        p = map(m3, rmin, rmax, 0, 100);
        display->drawProgressBar(0, 54, 127, 8, p);
        break;
    }
  }
  else if (scrollingMode || MTC.getMode() != MidiTimeCode::SynchroNone) {
    display->drawLine(0, 51, 127, 51);

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_10);
    if (tapDanceMode && tapDanceBank) {
      display->setColor(BLACK);
      display->fillRect(0, 54, 40, 63);
      display->setColor(WHITE);
    }
    else {
      display->fillRect(0, 54, 40, 63);
      display->setColor(BLACK);
    }

    if (currentBank < 9)
      display->drawString(0, 53, String("Bank 0" + String(currentBank+1)));
    else
      display->drawString(0, 53, String("Bank " + String(currentBank+1)));
    display->setColor(WHITE);

#ifdef WIFI
    if (wifiEnabled) {
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->setFont(midiIcons);
      if(appleMidiConnected) display->drawString(84, 54, String(1));
      else display->drawString(84, 54, String(0));

      if (interfaces[PED_IPMIDI].midiIn || interfaces[PED_IPMIDI].midiOut) display->drawString(106, 54, String(2));
      else display->drawString(106, 54, String(0));

      if (interfaces[PED_OSC].midiIn || interfaces[PED_OSC].midiOut) display->drawString(128, 54, String(3));
      else display->drawString(128, 54, String(0));
    }
#endif
  }
  */
}

void drawRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  display.drawLine(x0+1, y0,   x1-1, y0,   TFT_WHITE);
  display.drawLine(x1,   y0+1, x1,   y1-1, TFT_WHITE);
  display.drawLine(x1-1, y1,   x0+1, y1,   TFT_WHITE);
  display.drawLine(x0,   y1-1, x0,   y0+1, TFT_WHITE);
}

void drawFrame1(int16_t x, int16_t y)
{
  /*
  if (millis() < endMillis2 && lastPedalName[0] != ':') {
    ui.disableAutoTransition();
    ui.switchToFrame(0);
    if (strlen(lastPedalName) != 0 && lastPedalName[strlen(lastPedalName) - 1] == '.') lastPedalName[strlen(lastPedalName) - 1] = 0;
    if (lastPedalName[0] == 0) {
      display->setTextAlignment(TEXT_ALIGN_CENTER);
      switch (m1) {
        case midi::InvalidType:
          drawRect(display, 64-22, 15, 64+24, 15+23);
          display->setFont(ArialMT_Plain_10);
          display->drawString( 64 + x, 39 + y, String("Bank"));
          display->setFont(ArialMT_Plain_24);
          display->drawString( 64 + x, 14 + y, String(m2));
          break;
        case midi::NoteOn:
        case midi::NoteOff:
          drawRect(display, 64-22, 15, 64+24, 15+23);
          display->setFont(ArialMT_Plain_10);
          display->drawString( 64 + x, 39 + y, String("Note"));
          display->setFont(ArialMT_Plain_24);
          display->drawString( 64 + x, 14 + y, String(m2));
          display->setFont(ArialMT_Plain_10);
          display->drawString(110 + x, 39 + y, String("Velocity"));
          display->setFont(ArialMT_Plain_16);
          display->drawString(110 + x, 22 + y, String(m3));
          break;
        case midi::ControlChange:
          drawRect(display, 64-22, 15, 64+24, 15+23);
          display->setFont(ArialMT_Plain_10);
          display->drawString( 64 + x, 39 + y, String("CC"));
          display->setFont(ArialMT_Plain_24);
          display->drawString( 64 + x, 14 + y, String(m2));
          display->setFont(ArialMT_Plain_10);
          display->drawString(110 + x, 39 + y, String("Value"));
          display->setFont(ArialMT_Plain_16);
          display->drawString(110 + x, 22 + y, String(m3));
          break;
        case midi::ProgramChange:
          drawRect(display, 84-22, 15, 84+24, 15+23);
          display->setFont(ArialMT_Plain_10);
          display->drawString(84 + x, 39 + y, String("PC"));
          display->setFont(ArialMT_Plain_24);
          display->drawString(84 + x, 14 + y, String(m2));
          break;
        case midi::PitchBend:
          drawRect(display, 84-38, 15, 84+36, 15+23);
          display->setFont(ArialMT_Plain_10);
          display->drawString(84 + x, 39 + y, String("Pitch"));
          display->setFont(ArialMT_Plain_24);
          display->drawString(84 + x, 14 + y, String(((m3 << 7) | m2) + MIDI_PITCHBEND_MIN));
          break;
        case midi::AfterTouchChannel:
          drawRect(display, 84-22, 15, 84+24, 15+23);
          display->setFont(ArialMT_Plain_10);
          display->drawString(84 + x, 39 + y, String("Pressure"));
          display->setFont(ArialMT_Plain_24);
          display->drawString(84 + x, 14 + y, String(m2));
          break;
      }
      if (m1 != midi::InvalidType) {
        display->setFont(ArialMT_Plain_10);
        display->drawString(18 + x, 39 + y, String("Channel"));
        display->setFont(ArialMT_Plain_16);
        display->drawString(18 + x, 22 + y, String(m4));
      }
    }
    else {
      String name = lastPedalName;
      switch (m1) {
        case midi::InvalidType:
          drawRect(display, 64-22, 15, 64+24, 15+23);
          display->setTextAlignment(TEXT_ALIGN_CENTER);
          display->setFont(ArialMT_Plain_10);
          display->drawString( 64 + x, 39 + y, String("Bank"));
          display->setFont(ArialMT_Plain_24);
          display->drawString( 64 + x, 14 + y, String(m2));
          break;
        case midi::NoteOn:
        case midi::NoteOff:
        case midi::ControlChange:
          name.replace(String("###"), String(m3));
        case midi::ProgramChange:
        case midi::AfterTouchChannel:
          name.replace(String("###"), String(m2));
        default:
          display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
          display->setFont(ArialMT_Plain_24);
          display->drawString(64, 32, name);
          break;
      }
    }
  }
  else if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
           MTC.getMode() == MidiTimeCode::SynchroClockSlave) {
    display->setFont(ArialMT_Plain_24);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(22 + x, 12 + y, String(bpm));
    display->setFont(ArialMT_Plain_10);
    display->drawString(22 + x, 36 + y, "BPM");
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        display->drawString(128 + x, 36 + y, "2/4");
        break;
      case PED_TIMESIGNATURE_4_4:
        display->drawString(128 + x, 36 + y, "4/4");
        break;
      case PED_TIMESIGNATURE_3_4:
        display->drawString(128 + x, 36 + y, "3/4");
        break;
      case PED_TIMESIGNATURE_3_8:
        display->drawString(128 + x, 36 + y, "3/8");
        break;
      case PED_TIMESIGNATURE_6_8:
        display->drawString(128 + x, 36 + y, "6/8");
        break;
      case PED_TIMESIGNATURE_9_8:
        display->drawString(128 + x, 36 + y, "9/8");
        break;
      case PED_TIMESIGNATURE_12_8:
        display->drawString(128 + x, 36 + y, "12/8");
        break;
    }
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    if (MTC.getMode() == MidiTimeCode::SynchroClockMaster)
      display->drawString(68 + x, 36 + y, "Master");
    else if (MTC.getMode() == MidiTimeCode::SynchroClockSlave)
      display->drawString(68 + x, 36 + y, "Slave");

    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(block);
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        display->drawString( 68 + x, 16 + x, String(0));
        display->drawString( 88 + x, 16 + x, String(0));
        break;
      case PED_TIMESIGNATURE_4_4:
        display->drawString( 68 + x, 16 + x, String(0));
        display->drawString( 88 + x, 16 + x, String(0));
        display->drawString(108 + x, 16 + x, String(0));
        display->drawString(128 + x, 16 + x, String(0));
        break;
      case PED_TIMESIGNATURE_3_4:
      case PED_TIMESIGNATURE_3_8:
      case PED_TIMESIGNATURE_6_8:
      case PED_TIMESIGNATURE_9_8:
      case PED_TIMESIGNATURE_12_8:
        display->drawString( 68 + x, 16 + x, String(0));
        display->drawString( 88 + x, 16 + x, String(0));
        display->drawString(108 + x, 16 + x, String(0));
        break;
    }
    switch (MTC.getBeat()) {
      case 0:
        if (MTC.isPlaying())
          display->drawString( 68 + x, 16 + x, String(2));
        else
          display->drawString( 68 + x, 16 + x, String(1));
        break;
      case 1:
        if (MTC.isPlaying())
          display->drawString( 88 + x, 16 + x, String(2));
        else
          display->drawString( 88 + x, 16 + x, String(1));
        break;
      case 2:
        if (MTC.isPlaying())
          display->drawString(108 + x, 16 + x, String(2));
        else
          display->drawString(108 + x, 16 + x, String(1));
        break;
      case 3:
        if (MTC.isPlaying())
          display->drawString(128 + x, 16 + x, String(2));
        else
          display->drawString(128 + x, 16 + x, String(1));
        break;
    }
    ui.disableAutoTransition();
  }
  else if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
           MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
    char buf[12];
    sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
    display->setFont(ArialMT_Plain_24);
    display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display->drawString(64 + x, 32 + y, buf);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster)
      display->drawString(128 + x, 0 + y, "Master");
    else if (MTC.getMode() == MidiTimeCode::SynchroMTCSlave)
      display->drawString(128 + x, 0 + y, "Slave");
    ui.disableAutoTransition();
  }
  else {
    if (scrollingMode) {
      display->setFont(ArialMT_Plain_16);
      display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display->drawString(64 + x, 32 + y, MODEL);
      display->setFont(ArialMT_Plain_10);
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->drawString(110 + x, 16 + y, String("TM"));
      ui.enableAutoTransition();
    }
    else {
      if (banknames[currentBank][0] == 0) {
        display->setFont(DSEG7_Classic_Bold_50);
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(  0 + x, 9 + y, (currentProfile == 0 ? String('A') : (currentProfile == 1 ? String('B') : String('C'))));
        display->drawString( 38 + x, 9 + y, String("."));
        display->setTextAlignment(TEXT_ALIGN_RIGHT);
        display->drawString(128 + x, 9 + y, (currentBank >= 9  ? String("") : String('0')) + String(currentBank + 1));
      }
      else {
        String name;
        int offsetText       = 0;
        int offsetBackground = 0;
        static unsigned long ms = millis();

        // Display pedals name
        display->setFont(ArialMT_Plain_10);
        for (byte p = 0; p < PEDALS/2; p++) {
          switch (p) {
            case 0:
              display->setTextAlignment(TEXT_ALIGN_LEFT);
              offsetText = 1;
              offsetBackground = 0;
              break;
            case PEDALS / 2 - 1:
              display->setTextAlignment(TEXT_ALIGN_RIGHT);
              offsetText = -1;
              offsetBackground = 2;
              break;
            default:
              display->setTextAlignment(TEXT_ALIGN_CENTER);
              offsetText = 0;
              offsetBackground = 1;
              break;
          }
          // Top line
          name = String((banks[currentBank][p].pedalName[0] == ':') ? &banks[currentBank][p].pedalName[1] : banks[currentBank][p].pedalName);
          name.replace(String("###"), String(currentMIDIValue[currentBank][p][0]));
          if (pedals[p].function == PED_MIDI && currentMIDIValue[currentBank][p][0] == banks[currentBank][p].midiValue2) {
            display->fillRect((128 / (PEDALS / 2 - 1)) * p - offsetBackground * display->getStringWidth(name) / 2 + offsetText + x,
                              12 + y,
                              display->getStringWidth(name) + 1,
                              10);
            display->setColor(BLACK);
          }
          else
            display->setColor(WHITE);
          display->drawString((128 / (PEDALS / 2 - 1)) * p + offsetText + x, 10 + y, name);
          // Bottom line
          name = String((banks[currentBank][p + PEDALS / 2].pedalName[0] == ':') ? &banks[currentBank][p + PEDALS / 2].pedalName[1] : banks[currentBank][p + PEDALS / 2].pedalName);
          name.replace(String("###"), String(currentMIDIValue[currentBank][p + PEDALS / 2][0]));
          if (pedals[p + PEDALS / 2].function == PED_MIDI && currentMIDIValue[currentBank][p + PEDALS / 2][0] == banks[currentBank][p + PEDALS / 2].midiValue2) {
            display->fillRect((128 / (PEDALS / 2 - 1)) * p - offsetBackground * display->getStringWidth(name) / 2 + offsetText + x,
                              53 + y,
                              display->getStringWidth(name) + 1,
                              10);
            display->setColor(BLACK);
          }
          else
            display->setColor(WHITE);
          display->drawString((128 / (PEDALS / 2 - 1)) * p + offsetText + x, 51 + y, name);
          display->setColor(WHITE);
        }
        // Center area
        if (((millis() - ms < 4000) && (banknames[currentBank][0] != '.')) || (banknames[currentBank][0] == ':')) {
          // Display bank name
          display->drawRect(0, 23, 128, 29);
          name = (banknames[currentBank][0] == ':') ? &banknames[currentBank][1] : banknames[currentBank];
          name.replace(String("##"), String(currentBank));
          display->setFont(ArialMT_Plain_24);
          display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
          display->drawString( 64 + x, 37 + y, name);
        }
        else if (((millis() - ms < 8000) || (banknames[currentBank][0] == '.')) && (banknames[currentBank][0] != ':')) {
          // Display pedal values
          name = (banknames[currentBank][0] == '.') ? &banknames[currentBank][1] : banknames[currentBank];
          name.replace(String("##"), String(currentBank));
          display->setFont(ArialMT_Plain_10);
          display->setTextAlignment(TEXT_ALIGN_RIGHT);
          display->drawString(128 + x, y, name);
          for (byte p = 0; p < PEDALS/2; p++) {
            if ((pedals[p].function == PED_MIDI) && (banks[currentBank][p].midiMessage != PED_EMPTY)) {
              display->drawProgressBar((128 / (PEDALS / 2)) * p + 2 + x, 25 + y, 39, 11, constrain(map(currentMIDIValue[currentBank][p][0],
                                                                                                       banks[currentBank][p].midiValue1,
                                                                                                       banks[currentBank][p].midiValue2,
                                                                                                       0, 100),
                                                                                                   0, 100));
            }
            if ((pedals[p + PEDALS / 2].function == PED_MIDI) && (banks[currentBank][p + PEDALS / 2].midiMessage != PED_EMPTY)) {
              display->drawProgressBar((128 / (PEDALS / 2)) * p + 2 + x, 39 + y, 39, 11, constrain(map(currentMIDIValue[currentBank][p + PEDALS / 2][0],
                                                                                                       banks[currentBank][p + PEDALS / 2].midiValue1,
                                                                                                       banks[currentBank][p + PEDALS / 2].midiValue2,
                                                                                                       0, 100),
                                                                                                   0, 100));
            }
          }
        }
        else {
          ms = millis();
        }
      }
      ui.disableAutoTransition();
    }
  }

#ifdef WEBSOCKET
  events.send(MTC.isPlaying() ? "1" : "0", "play");

  if (MTC.getMode() == MidiTimeCode::SynchroClockMaster ||
      MTC.getMode() == MidiTimeCode::SynchroClockSlave)  {
    char buf[4];
    events.send("", "mtc");
    sprintf(buf, "%3d", bpm);
    events.send(buf, "bpm");
    sprintf(buf, "%d", MTC.getBeat() + 1);
    events.send(buf, "beat");
    switch (timeSignature) {
      case PED_TIMESIGNATURE_2_4:
        events.send("2/4", "timesignature");
        break;
      case PED_TIMESIGNATURE_4_4:
        events.send("4/4", "timesignature");
        break;
      case PED_TIMESIGNATURE_3_4:
        events.send("3/4", "timesignature");
        break;
      case PED_TIMESIGNATURE_3_8:
        events.send("3/8", "timesignature");
        break;
      case PED_TIMESIGNATURE_6_8:
        events.send("6/8", "timesignature");
        break;
      case PED_TIMESIGNATURE_9_8:
        events.send("9/8", "timesignature");
        break;
      case PED_TIMESIGNATURE_12_8:
        events.send("12/8", "timesignature");
        break;
    }
  }

  if (MTC.getMode() == MidiTimeCode::SynchroMTCMaster ||
      MTC.getMode() == MidiTimeCode::SynchroMTCSlave) {
    char buf[12];
    sprintf(buf, "%02d:%02d:%02d:%02d", MTC.getHours(), MTC.getMinutes(), MTC.getSeconds(), MTC.getFrames());
    events.send(buf, "mtc");
    events.send("", "bpm");
    events.send("", "beat");
    events.send("", "timesignature");
  }
#endif
  */
}

void drawFrame2(int16_t x, int16_t y)
{
  /*
  if (!scrollingMode || MTC.isPlaying() || MTC.getMode() != PED_MTC_NONE || millis() < endMillis2)
    ui.switchToFrame(0);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 16 + y, "Device:");
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 16 + y, host);
#ifdef WIFI
  switch (WiFi.getMode()) {
    case WIFI_AP:
    case WIFI_AP_STA:
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->drawString(0 + x, 26 + y, "AP:");
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->drawString(128 + x, 26 + y, ssidSoftAP);
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->drawString(0 + x, 36 + y, "AP IP:");
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->drawString(128 + x, 36 + y, WiFi.softAPIP().toString());
      break;
    case WIFI_STA:
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->drawString(0 + x, 26 + y, "SSID:");
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->drawString(128 + x, 26 + y, wifiSSID);
      display->setTextAlignment(TEXT_ALIGN_LEFT);
      display->drawString(0 + x, 36 + y, "IP:");
      display->setTextAlignment(TEXT_ALIGN_RIGHT);
      display->drawString(128 + x, 36 + y, WiFi.localIP().toString());
      break;
    case WIFI_MODE_MAX:
    case WIFI_MODE_NULL:
      break;
  }
#endif
*/
}

void drawFrame3(int16_t x, int16_t y)
{
  /*
  if (!scrollingMode || MTC.isPlaying() || MTC.getMode() != PED_MTC_NONE || millis() < endMillis2)
    ui.switchToFrame(0);

  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 16 + y, "Free heap:");
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 16 + y, ESP.getFreeHeap()/1024 + String(" Kb"));

#ifdef BATTERY
  static uint16_t voltage = bat.voltage();
  voltage = (99*voltage + bat.voltage()) / 100;
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 26 + y, "Battery voltage:");
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 26 + y, voltage + String(" mV"));
#endif

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 36 + y, "Running time:");
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  long sec = (millis() / 1000) % 60;
  long min = (millis() / 1000 / 60) % 60;
  long h   = (millis() / 1000 / 3600);
  display->drawString(128 + x, 36 + y, h + String("h ") + min + String("m ") + sec + String("s"));
  */
}


void display_init()
{
    display.init();
    display.setRotation(1);
    display.fillScreen(TFT_BLACK);
    top.createSprite(display.width(), 24);
    top.fillRect(0, 0, display.width(), 24, TFT_WHITE);

    /*
    if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    */

#ifdef WIFI
  if (wifiEnabled) {
    display.fillScreen(TFT_WHITE);
    display.setSwapBytes(true);
    display.pushImage((display.width() - WIFI_LOGO_WIDTH) / 2, (display.height() - WIFI_LOGO_HEIGHT) / 2, WIFI_LOGO_WIDTH, WIFI_LOGO_HEIGHT, WiFiLogo);
    delay(1000);
  }
#endif

#ifdef BLE
  if (bleEnabled) {
    display.fillScreen(TFT_WHITE);
    display.setSwapBytes(true);
    display.pushImage((display.width() - BLUETOOTH_LOGO_WIDTH) / 2, (display.height() - BLUETOOTH_LOGO_HEIGHT) / 2, BLUETOOTH_LOGO_WIDTH, BLUETOOTH_LOGO_HEIGHT, BluetoothLogo);
    delay(1000);
  }
#endif

    display.fillScreen(TFT_BLACK);

    //display.setSwapBytes(true);
    //display.pushImage((display.width() - PEDALINO_LOGO_WIDTH) / 2, (display.height() - PEDALINO_LOGO_HEIGHT) / 2, PEDALINO_LOGO_WIDTH, PEDALINO_LOGO_HEIGHT, PedalinoLogo);

    display.fillScreen(TFT_BLACK);
}

void display_ui_update_disable()
{
  uiUpdate = false;
}

void display_ui_update_enable()
{
  uiUpdate = true;
}

void display_update()
{
  if (uiUpdate) {
    topOverlay();
  }
}

void display_off()
{
  digitalWrite(TFT_BL, !TFT_BACKLIGHT_ON);

  display.writecommand(TFT_DISPOFF);
  display.writecommand(TFT_SLPIN);
}

