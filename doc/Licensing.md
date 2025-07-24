# 🔐 Licensing & Activation

As of **version 4.0.0**, PedalinoMini™ has transitioned to a **closed-source** model.

To activate and use the firmware, a valid license is required:

- 💶 **License cost:** €1 per device
- 🔒 License is bound to the device's **MAC address** and the firmware **version series** (`major.minor.patch`)
- 🔁 The license **includes all patches** within the same version series (e.g., 4.6.0 → 4.6.1, 4.6.2, etc.)
- 🎫 Licenses are **perpetual** for the purchased version series — **no subscription required**

## ✅ How Activation Works

Activating your PedalinoMini™ device is simple and straightforward:

1. [**Flash your device**](http://alf45tar.github.io/PedalinoMini) with the latest PedalinoMini™ firmware.
2. Select **Visit Device** from [**PedalinoMini™ Installer**](http://alf45tar.github.io/PedalinoMini) to visit the device's hosted web interface.
3. In the **top-left corner**, a **red flashing bell icon** indicates your device is **not activated**.
4. Click the red bell, or navigate to **Options → Check for Updates**, to open the **Update & Activate** page.
5. On this page, you will see:
   - Your **current firmware version**.
   - The **latest available firmware version**, which you can install using the [**PedalinoMini™ Installer**](http://alf45tar.github.io/PedalinoMini/installer).
   - Whether your device is currently **activated** or **not activated**.
   - Whether your device is **eligible for a license** for the current patch version.
6. If a **new license purchase is required**, click the **Pay Here** button, and complete payment (€1).
   After payment, you will **immediately receive your license key** — simply press **Save Key** to activate.
7. If your device is **eligible for a patch license**, **no purchase is required** — simply press **Save Key** to activate.
8. Your device is now fully activated and unlocked.

## 🧪 About Demo Mode

If not activated, the device runs in **demo mode**:

- All features are available at startup.
- After **5 minutes**, one interface is **randomly disabled**.
- Over time, more interfaces may become disabled.
- The user can manually reactivate disabled interfaces, but they will be randomly disabled again over time.
- This cycle continues, allowing testing of the device with some limitations until activation.
- You can test the device fully in demo mode, but activation removes these limitations.

> **Note:** Licenses cover a single device (by MAC address) and the firmware **version series** (`major.minor.patch`).
> A license valid for `4.6.x` covers all patch updates in that series, but upgrading to a new version series like `4.7.0` requires a new license.

## 🚫 Open Source Notice

PedalinoMini™ was originally an open-source project. As of version **4.0.0**, it is now a **closed-source product**.

- The GitHub repositories will continue to host documentation, build guides, and legacy materials.
- Source code for versions **before 4.0.0** remains available under its original license.
- All new firmware and feature development is now proprietary.

## 🔒 Why Move to Closed Source?

After years of active development and community feedback, PedalinoMini™ is transitioning to a **closed-source model** as of version **4.0.0**.

This decision is based on several important factors:

### ⚖️ 1. Lack of Meaningful Community Contributions

While PedalinoMini™ was open source, the vast majority of development — including features, fixes, and support — came from a single maintainer. Community contributions were limited, and maintaining open repositories offered little return in terms of collaboration.

### 📦 2. Public Installer Now Includes Full Features

Previously, certain features and installers were only available to **sponsors** through a private repository.
With this change, **all users** gain access to the **official installer**, with no need for custom builds — provided the device is activated via a low-cost license.

### 🛡️ 3. Protection Against Misuse & Unauthorized Commercial Use

As interest in PedalinoMini™ grows, so does the risk of misuse or unlicensed commercial integration. Moving to a closed-source model helps:

- Prevent unauthorized commercial derivatives
- Ensure quality control across all distributed devices
- Protect the project’s identity and purpose

### 💶 4. Sustainable, Fair Support Model

By requiring a **€1 per-device license**, the project can remain:

- Affordable for hobbyists, musicians, and creators
- Sustainable for continued development and updates
- Independent of large sponsors or proprietary lock-ins

This shift allows PedalinoMini™ to grow as a professional, polished tool — while staying accessible.

## ⭐ Legacy Users

If you've used PedalinoMini™ prior to version 4.0.0:

- You can continue using your existing open-source firmware.
- You’re welcome to remain part of the community — no license is required for legacy use.
- Future updates and features will require activation under the new license model.

## ❓ Frequently Asked Questions (FAQ)

**Q: Can I still build the firmware from source?**
A: No. The firmware is now closed-source. You can download precompiled binaries and activate them via a license key.

**Q: Do I need to pay €1 every time I flash?**
A: No — the license is tied to your device's MAC address and firmware version series (e.g. `4.6.x`). Reflashing that version does not require a new license.

**Q: What happens if I upgrade to a new version series (e.g., `4.7.x`)?**
A: You’ll need to purchase a new license for that version. Licenses are perpetual for the version series they cover.

**Q: Can I use the same license on multiple devices?**
A: No. Each device requires its own license, bound to its unique MAC address.

**Q: I’m a hobbyist — why charge anything?**
A: The €1 fee is symbolic — it supports sustainable development, protects against misuse, and helps keep the project affordable and independent.

**Q: What happens if I don’t activate my device?**
A: Unlicensed devices enter **demo mode**:

- All features work initially
- After 5 minutes, **one interface** (e.g., USB, BLE, MIDI) is **randomly disabled**
- Over time, more interfaces may become disabled at random
- You can still test the device, but reliable long-term use requires activation

**Q: Will I lose access to old versions?**
A: No. All open-source versions prior to `4.0.0` remain available under their original license.

**Q: I’m a GitHub Sponsor — do I get licenses included?**
A: Yes! Active sponsors will receive at least **one free license per month** of active sponsorship. These can be used for any device and firmware version.

👉 Want to support beyond the €1 license? [Become a sponsor](https://github.com/sponsors/alf45tar) and help shape PedalinoMini™’s future.
