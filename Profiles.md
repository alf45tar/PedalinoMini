# 🎭 What Are Profiles in PedalinoMini™

Profiles in **PedalinoMini™** are saved sets of configurations that define how your footswitches behave. Think of them like user presets — you can quickly switch between different control schemes depending on what you're doing:

- 🎸 **Profile 1**: Control your guitar effects and DAW
- 🎥 **Profile 2**: Trigger OBS scenes and sound effects
- 🎤 **Profile 3**: Advance slides and control a teleprompter

Each profile can have its own **banks**, **actions**, **sequences**, **pedal behaviors**, and **MIDI/HID mappings**.

## 🧠 How Profiles Work

### 🔢 Three Customizable Profiles

You get **3 fully independent user profiles**.
Each profile contains:

- Up to **20 banks** of switch configurations
- A **Global Bank** shared across all banks
  (e.g., for common actions like *tap tempo* or *mute mic*)

### 🔁 Switching Profiles

Profiles can be switched:

- Using a **footswitch** assigned to the “Profile” action
- Automatically via triggers like **MIDI control changes**
- Manually via the Web UI: `http://pedalino.local`

### 🔒 Profile Persistence

The current profile is **only saved** when you explicitly save the configuration via the Web UI.
If you switch profiles but do **not** save, the last saved profile remains active after a reboot.
This means profile persistence depends on saving your changes through the Web interface to ensure the device resumes with your desired profile.

## ✨ Use Case Examples

### 🎵 Live Music Gig

- **Profile 1**: Effects toggle + DAW loop control
- **Profile 2**: Sheet music navigation + tap tempo
- **Profile 3**: Backing track playback + mixer levels

### 📺 Streaming Setup

- **Profile 1**: OBS scene switching
- **Profile 2**: Mic/video mute, overlays
- **Profile 3**: Control music/soundboard in background

### 🗣 Conference Presentation

- **Profile 1**: Slide navigation
- **Profile 2**: Trigger video clips
- **Profile 3**: Scroll PDF script

## 🎛️ Switching Profiles or Banks via MIDI

**PedalinoMini™** supports remote switching of profiles and banks using **MIDI Control Change (CC)** messages on **channel 16**.

### 🔁 Switch Profiles

To switch profiles (e.g., live during a performance):

| Parameter          | Value                       |
| ------------------ | --------------------------- |
| **MIDI Channel**   | 16                          |
| **Control Number** | CC 0 (BankSelect)           |
| **Value Range**    | 1 to 3                      |

**🔧 Example:**
Sending `CC 0`, value `2` on channel `16` switches to **Profile 2**.

> ⚠️ Values must start at 1 —
> Profile 1 = value 1, Profile 2 = value 2, etc.

### 📁 Switch Banks

To change pedal banks **within the current profile**:

| Parameter          | Value                      |
| ------------------ | -------------------------- |
| **MIDI Channel**   | 16                         |
| **Control Number** | CC 32 (BankSelect + 32)    |
| **Value Range**    | 0 to 20                    |

**🔧 Example:**
Sending `CC 32`, value `3` on channel `16` switches to **Bank 3** in the current profile.

### 💡 Tip

You can program a **DAW**, **MIDI controller**, or **footswitch** to send these CC messages for **seamless control** of profiles and banks during a performance.
