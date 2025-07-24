# Interfaces

![WEBUI INTERFACES](./images/webui-interfaces.png "Interfaces")

The supported interfaces are:

| Interface    | Description                                                                                                                                                                                                                                                                                       |
| :----------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| USB MIDI     | Built-in plug&play USB MIDI interface for ESP32-S3 board. Requires additional hardware (see schematic) on ESP32.                                                                                                                                                                                  |
| Legacy MIDI  | Legacy serial MIDI interface with 5 pins DIN connectors. Requires additional hardware (see schematic).                                                                                                                                                                                            |
| RTP-MIDI     | RTP-MIDI also know as AppleMIDI or Network MIDI. It is based on UDP protocol and it requires WiFi.                                                                                                                                                                                                |
| ipMIDI       | ipMIDI is a multicast UDP protocol and it requires WiFi. ipMIDI works only if both end points are connected to the same 2.4 GHz WiFi network. Same router is not enough when one end point is connected to a 5 GHz WiFi network or to a different 2.4 GHz WiFI network hosted by the same router. |
| BLE MIDI     | Requires Bluetooth Low Energy (BLE).                                                                                                                                                                                                                                                              |
| OSC          | OSC messages are based on UDP protocol and it requires WiFi.                                                                                                                                                                                                                                      |
| USB Keyboard | ESP32-S3 board can emulate a USB HID keyboard                                                                                                                                                                                                                                                     |
| BLE Keyboard | ESP32-S3 board can emulate a Bluetooth LE keyboard                                                                                                                                                                                                                                                |

For each MIDI interface there are 4 filtering options:

| ______ | Description                                                                                                                              |
| :----- | :--------------------------------------------------------------------------------------------------------------------------------------- |
| IN     | Enable/disable receiving MIDI messages from the interface                                                                                |
| OUT    | Enable/disable sending MIDI messages to the interface                                                                                    |
| THRU   | Enable/disable so called MIDI Thru (every message received from the IN port is automatically sent to the OUT port of the same interface) |
| CLOCK  | Enable/disable receiving and sending CLOCK MIDI message                                                                                  |

Routing between different interfaces is enabled between every IN enabled interfaces to any OUT enabled interfaces. Do not confuse routing (different interfaces) with MIDI Thru (between IN and OUT of same interface).
