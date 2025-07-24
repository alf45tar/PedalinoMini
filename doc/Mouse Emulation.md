# 💡 Mouse Emulation

Mouse emulation (available on **ESP32-S3**) allows PedalinoMini™ to function as a standard **USB HID mouse** or a **Bluetooth LE mouse**, giving you full cursor and button control directly from foot pedals.

## ✅ Works with *any* device that supports standard mice

- **USB Mode**: Wired mouse control on Windows, macOS, Linux, and Android (via USB OTG).
- **Bluetooth LE Mode**: Wireless mouse control on laptops, tablets, phones, smart TVs — no dongle or receiver needed.

## ✅ Fully Wireless Option (BLE Mouse Mode)

- Connects to devices over Bluetooth Low Energy (BLE).
- Appears as a standard wireless mouse.
- Perfect for live presentations, touchscreen devices, or mobile workflows.

## 🧩 Mouse Actions You Can Assign to Pedals

PedalinoMini™ lets you map a variety of mouse functions to your foot pedals:

### 🎯 Mouse Button Actions

You can choose from the following buttons:

- **Left**
- **Right**
- **Middle**
- **Back**
- **Forward**

And assign one of these behaviors:

- **Click**
  Simulates a quick press and release of the selected button
  → Example: Left-click to select, Right-click to open a context menu

- **Press**
  Press and **hold** the selected button
  → Often used to start a drag action

- **Release**
  Releases the selected button
  → Used to complete a drag or drop an item

> ℹ️ **Tip:** You can link a **mouse press** to a **pedal press**, and a **mouse release** to a **pedal release**. This makes actions like drag-and-drop feel natural and responsive.

### 🧭 Cursor Movement

Move the mouse pointer gradually in any direction with each pedal press:

- Move Up / Down
- Move Left / Right

→ Combine with multiple pedals for full directional control.

### 🔄 Scrolling & Panning

- **Scroll Up / Down**
  Vertical scrolling — as if turning the scroll wheel

- **Pan Left / Right**
  Horizontal scrolling — great for navigating wide documents, spreadsheets, or audio timelines

## ✅ Combined Keyboard + Mouse Support

- PedalinoMini can emulate **keyboard** and **mouse** simultaneously.
- Use one pedal for a key combo (`Ctrl+Z`) and another to move/click the mouse.
- No special drivers required — all via standard USB or BLE HID.

# 🖱️ USB Mouse + BLE Mouse in Parallel

Yes, PedalinoMini™ can emulate both a **USB mouse** and a **Bluetooth LE (BLE) mouse** **at the same time**, enabling simultaneous control of **two different devices**.

You can configure PedalinoMini™ to:

- Act as a **USB HID mouse** (e.g., for a PC via cable)
- Act as a **BLE HID mouse** (e.g., for a tablet or smartphone)

### ➕ Dual Output Example:

| Output  | Target Device              |
| ------- | -------------------------- |
| **USB** | Windows/macOS/Linux PC     |
| **BLE** | iPad, Android tablet, etc. |

Both outputs work **simultaneously**.
