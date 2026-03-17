# Switch Event Logic & Timing Specification

This document defines the behavior and timing requirements for switch input events. The logic uses a set of configurable parameters to distinguish between simple clicks, sustained holds, and rapid double-taps.

## 1. Configuration Parameters

The following constants (in milliseconds) define the event detection windows:

These parameters are available in the **Options** section for configuration.

| Constant | Parameter | Default Value | Description |
| :--- | :--- | :--- | :--- |
| $T_{debounce}$ | **Debounce Interval** | 5 ms | Minimum stable time to filter mechanical noise. |
| $T_{sim}$ | **Simultaneous Gap** | 50 ms | Max time between two button presses to count as a chord. |
| $T_{long}$ | **Long Press Time** | 500 ms | Threshold to promote a standard press to a "Long Press." |
| $T_{double}$ | **Double Press Time** | 400 ms | Max window from the *first* press to detect a second. |
| $T_{repeat}$ | **Repeat Press Time** | 1000 ms| Interval for sequential events during a sustained hold. |

Event generation also depends on **Pedals** settings: enable **Singles Press** for single press/release/click events, **Double Press** for double click events, and **Long Press** for long press/repeat/long release events.

## 2. Event Definitions

### 2.1 PRESS
* **Description:** Generated when the switch transitions from RELEASED to PRESSED.
* **Trigger Condition:** Physical edge detected and remains stable for $\ge T_{debounce}$.
* **Trigger Time:** Immediately after debounce validation.

### 2.2 RELEASE
* **Description:** Generated when the switch transitions from PRESSED to RELEASED.
* **Trigger Condition:** Release edge detected and remains stable for $\ge T_{debounce}$.
* **Trigger Time:** Immediately after debounce validation.

### 2.3 PRESS & RELEASE
* **Description:** A full physical cycle (down then up), independent of duration.
* **Trigger Condition:** Fires on every valid **PRESS** and on every valid **RELEASE**.
* **Note:** It is never suppressed, regardless of whether a higher-level event (CLICK, DOUBLE CLICK, LONG PRESS, etc.) is also generated. Typical use case: latch switches, where an action must trigger on every state change (every movement).

### 2.4 CLICK
* **Description:** A standard momentary interaction.
* **Trigger Condition:**
    * Valid **PRESS** and **RELEASE** occur.
    * Total hold duration $< T_{long}$.
    * No second press occurs within the $T_{double}$ window.
* **Trigger Time:** Once the $T_{double}$ window expires after the release.

### 2.5 DOUBLE CLICK
* **Description:** Two rapid presses treated as one command.
* **Trigger Condition:**
    * First **PRESS** and **RELEASE** cycle completes.
    * Second **PRESS** begins within $T_{double}$ of the first press's start.
* **Trigger Time:** Immediately upon the second **RELEASE**.

### 2.6 LONG PRESS
* **Description:** A sustained hold for secondary functions.
* **Trigger Condition:** Switch remains pressed continuously for $\ge T_{long}$.
* **Trigger Time:** The exact moment the $T_{long}$ threshold is reached.

### 2.7 REPEAT PRESSED
* **Description:** Repeated virtual events while the button is held.
* **Trigger Condition:** **LONG PRESS** has occurred and button remains held.
* **Trigger Time:** Every $T_{repeat}$ milliseconds after the initial press detection.

### 2.8 LONG RELEASE
* **Description:** The release event following a long-duration interaction.
* **Trigger Condition:** Button is released after a **LONG PRESS** state was reached.
* **Trigger Time:** On release after the long press threshold.

## 3. Scenario Examples

### Scenario A: Standard Click
1. **0ms:** Press detected.
2. **5ms:** **PRESS** validated (debounce $T_{debounce}$ = 5ms elapsed). **PRESS** fires.
3. **120ms:** Release detected.
4. **125ms:** **RELEASE** validated (debounce $T_{debounce}$ = 5ms elapsed). **RELEASE** fires.
5. **405ms:** **CLICK** generated (within $T_{double}$ = 400ms window from first press).

### Scenario B: Double Click
1. **0ms:** First press detected.
2. **5ms:** **PRESS** validated. **PRESS** fires.
3. **100ms:** First release detected.
4. **105ms:** **RELEASE** validated. **RELEASE** fires.
5. **250ms:** Second press detected.
6. **255ms:** **PRESS** validated. **PRESS** fires. **DOUBLE CLICK** generated (within $T_{double}$ = 400ms window from first press). **CLICK** is suppressed.
7. **450ms:** Second release detected.
8. **455ms:** **RELEASE** validated. **RELEASE** fires.

### Scenario C: Long Press & Repeat
1. **0ms:** Press detected.
2. **5ms:** **PRESS** validated. **PRESS** fires.
3. **505ms:** **LONG PRESS** generated ($T_{long}$ = 500ms after **PRESS** validation).
4. **1005ms:** **REPEAT PRESSED** #1 generated ($T_{repeat}$ = 1000ms after initial press).
5. **2005ms:** **REPEAT PRESSED** #2 generated.
6. **2500ms:** Release detected.
7. **2505ms:** **RELEASE** validated. **RELEASE** suppressed. **LONG RELEASE** generated.

## 4. Practical Suggestions

* **PRESS** and **RELEASE** are the most responsive events because they fire immediately after debounce and do not require extra waiting windows.
* **CLICK** can also feel responsive when **DOUBLE PRESS** and **LONG PRESS** are disabled, because no additional classification windows are needed.
* When **DOUBLE PRESS** or **LONG PRESS** is enabled, prefer using **CLICK** (instead of **PRESS** or **RELEASE**) for single-action commands to avoid unwanted triggers.
* If **DOUBLE PRESS** is enabled, **CLICK** is postponed until the DOUBLE PRESS window ($T_{double}$) expires.