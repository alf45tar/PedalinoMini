# 💡 Keyboard emulation

Keyboard emulation (available only on ESP32-S3) — either as a **USB HID keyboard** or **Bluetooth LE keyboard** — allows PedalinoMini™ to act exactly like a standard keyboard.
Instead of only sending MIDI messages, it can send **key presses** directly to your computer, tablet, or phone.

## ✅ Works with *any* software, not just MIDI-enabled apps

- Many apps (presentation tools, video editors, streaming software) support keyboard shortcuts but do **not** accept MIDI directly.
- With keyboard emulation, pedals can be mapped to:
  - Next/previous slide
  - Start/stop recording
  - Trigger sound effects
  - Toggle camera or mic
  - Any custom hotkey combination

## ✅ Simpler integration

- No need for virtual MIDI drivers or extra software bridges.
- Just plug in and control as if you're pressing keys on a real keyboard.

## ✅ Fully wireless option (BLE)

- Bluetooth LE keyboard mode enables wireless connection to tablets, laptops, or smartphones.
- Ideal for live presentations or on-stage movement.

## ✅ Supports modifier keys

- You can send advanced combinations such as:
  - **Ctrl + Shift + P** (e.g., play/pause in streaming or video apps)
  - **Cmd + Option + Esc** (macOS commands)
  - Media keys (volume, play/pause, next track, etc.)

## ✅ Cross-platform

- Works on Windows, macOS, iOS, Android, Linux — anything that supports standard keyboards.

## Supported Key Names

> **Note:** The key names correspond to the **physical positions** on a standard US keyboard layout, **not necessarily the printed labels** on your keyboard.
> For example, a key labeled differently on non-US layouts will still be identified by its US position.

![us-layout](https://upload.wikimedia.org/wikipedia/commons/3/3a/Qwerty.svg)

Key strings are case insensitive except for `A` to `Z` and `a` to `z`.

### 🧩 Modifier Keys

| Key String   | Aliases                                       |
| ------------ | --------------------------------------------- |
| `Ctrl`       | `LeftCtrl`                                    |
| `RightCtrl`  | —                                             |
| `Shift`      | `LeftShift`                                   |
| `RightShift` | —                                             |
| `Alt`        | `LeftAlt`, `Option`                           |
| `RightAlt`   | `RightOption`                                 |
| `GUI`        | `Win`, `Cmd`, `LeftGUI`, `LeftWin`, `LeftCmd` |
| `RightGUI`   | `RightWin`, `RightCmd`                        |

"Win" = Windows key (also known as "GUI" or "Cmd")

### 🔤 Alphanumeric Keys

| Key String |
| ---------- |
| `A` to `Z` |
| `a` to `z` |
| `0` to `9` |

### 🎯 Function Keys

| Key String    |
| ------------- |
| `F1` to `F24` |

### 🔢 Number Row Symbols (via Shift or ASCII)

> You can use either the **symbol itself** (e.g., `!`) or the equivalent **Shift+Number** form (e.g., `Shift+1`).
> Both are valid in key combinations.

| Symbol | Valid Inputs     | Description       |
| ------ | ---------------- | ----------------- |
| `!`    | `!` or `Shift+1` | Exclamation mark  |
| `"`    | `"` or `Shift+2` | Double quotes     |
| `£`    | `£` or `Shift+3` | Pound sign        |
| `$`    | `$` or `Shift+4` | Dollar sign       |
| `%`    | `%` or `Shift+5` | Percent sign      |
| `^`    | `^` or `Shift+6` | Caret             |
| `&`    | `&` or `Shift+7` | Ampersand         |
| `*`    | `*` or `Shift+8` | Asterisk          |
| `(`    | `(` or `Shift+9` | Open parenthesis  |
| `)`    | `)` or `Shift+0` | Close parenthesis |

### 🔣 Punctuation & Symbol Keys

| Symbol  | Valid Inputs        | Description   |
| ------- | ------------------- | ------------- |
| `` ` `` | `` ` ``             | Backtick      |
| `~`     | `~` or ``Shift+` `` | Tilde         |
| `-`     | -                   | Dash          |
| `_`     | `_` or `Shift+-`    | Underscore    |
| `=`     | =                   | Equals        |
| `+`     | `+` or `Shift+=`    | Plus          |
| `[`     | [                   | Open bracket  |
| `{`     | `{` or `Shift+[`    | Left brace    |
| `]`     | ]                   | Close bracket |
| `}`     | `}` or `Shift+]`    | Right brace   |
| `\`     | \                   | Backslash     |
| `\|`    | `\|` or `Shift+\`   | Pipe          |
| `;`     | ;                   | Semicolon     |
| `:`     | `:` + `Shift+;`     | Colon         |
| `'`     | '                   | Apostrophe    |
| `"`     | `"` or `Shift+'`    | Double quote  |
| `,`     | ,                   | Comma         |
| `<`     | `<` or `Shift+,`    | Less-than     |
| `.`     | .                   | Period        |
| `>`     | `>` or `Shift+.`    | Greater-than  |
| `/`     | /                   | Slash         |
| `?`     | `?` or `Shift+/`    | Question mark |

### ⌨️ Special Keys

| Key String    | Description               |
| ------------- | ------------------------- |
| `Enter`       | Return key                |
| `Esc`         | Escape key                |
| `Tab`         | Tab key                   |
| `Space`       | Spacebar                  |
| `Backspace`   | Delete previous character |
| `CapsLock`    | Caps lock toggle          |
| `Delete`      | Forward delete            |
| `Insert`      | Insert key                |
| `Home`        | Move cursor to line start |
| `End`         | Move cursor to line end   |
| `PageUp`      | Scroll up                 |
| `PageDown`    | Scroll down               |
| `PrintScreen` | Screenshot key            |
| `ScrollLock`  | Toggle scroll lock        |
| `Pause`       | Pause/Break key           |

### 🧭 Arrow Keys

| Key String |
| ---------- |
| `Up`       |
| `Down`     |
| `Left`     |
| `Right`    |

### 🔢 Numpad Keys

| Key String             |
| ---------------------- |
| `NumLock`              |
| `Numpad0` to `Numpad9` |
| `NumpadPeriod`         |
| `NumpadEnter`          |
| `NumpadDivide`         |
| `NumpadMultiply`       |
| `NumpadSubtract`       |
| `NumpadAdd`            |

### 🎵 Media Keys

| Key String     | Description              |
| -------------- | ------------------------ |
| `PlayPause`    | Toggle play/pause        |
| `Next`         | Next track               |
| `Previous`     | Previous track           |
| `Stop`         | Stop playback            |
| `Mute`         | Mute audio               |
| `VolumeUp`     | Increase volume          |
| `VolumeDown`   | Decrease volume          |
| `WWW`          | Internet browser         |
| `WwwSearch`    | Internet search          |
| `WwwStop`      | Internet stop            |
| `WwwBack`      | Internet browser back    |
| `WwwForward`   | Internet browser forward |
| `WwwBookmarks` | Internet bookmarks       |
| `Email`        | Email app                |
| `Calculator`   | Calculator app           |
| `MyComputer`   | Local browser app        |

Media keys can't be combined with other keys.
