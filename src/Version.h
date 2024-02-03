/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2024 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Major version number (X.x.x) */
#define PEDALINO_VERSION_MAJOR   3
/** Minor version number (x.X.x) */
#define PEDALINO_VERSION_MINOR   2
/** Patch version number (x.x.X) */
#define PEDALINO_VERSION_PATCH   2

#define xstr(s) sstr(s)  // stringize the result of expansion of a macro argument
#define sstr(s) #s

#define VERSION xstr(PEDALINO_VERSION_MAJOR.PEDALINO_VERSION_MINOR.PEDALINO_VERSION_PATCH)

/**
 * Macro to convert PEDALINO version number into an integer
 *
 * To be used in comparisons, such as PEDALINO_VERSION >= PEDALINO_VERSION_VAL(2, 0, 0)
 */
#define PEDALINO_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

/**
 * Current PEDALINO version, as an integer
 *
 * To be used in comparisons, such as PEDALINO_VERSION >= PEDALINO_VERSION_VAL(2, 0, 0)
 */
#define PEDALINO_VERSION PEDALINO_VERSION_VAL(ESP_ARDUINO_VERSION_MAJOR, \
                                              ESP_ARDUINO_VERSION_MINOR, \
                                              ESP_ARDUINO_VERSION_PATCH)

#ifdef __cplusplus
}
#endif