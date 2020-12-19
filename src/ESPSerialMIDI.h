/*!
 *  @file       ESPSerialMIDI.h
 *  Project     Arduino MIDI Library
 *  @brief      MIDI Library for the Arduino - Platform
 *  @license    MIT - Copyright (c) 2015 Francois Best
 *  @author     lathoub, Francois Best - al45tar
 *  @date       07/09/2020
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 #pragma once

#include "midi_Namespace.h"

BEGIN_MIDI_NAMESPACE

struct DefaultESPSerialSettings
{
    /*! Override the default MIDI baudrate to transmit over USB serial, to
    a decoding program such as Hairless MIDI (set baudrate to 115200)\n
    http://projectgus.github.io/hairless-midiserial/
    */
    static const long BaudRate = 31250;

    /*! Add support for ESP32 pins assignement.
    Without pins re-mapping Serial1 cannot be used on many ESP32 dev boards.
    Default Serial1 pins (RX1=GPIO9 TX1=GPIO10) are mainly used for flash memory.
    */
    static const int8_t RxPin = -1;
    static const int8_t TxPin = -1;
};

template <class SerialPort, class _Settings = DefaultESPSerialSettings>
class ESPSerialMIDI
{
    typedef _Settings Settings;

public:
	ESPSerialMIDI(SerialPort& inSerial)
        : mSerial(inSerial)
	{
	};

public:
    static const bool thruActivated = true;

    void begin()
	{
        // Initialise the Serial port
        #if defined(AVR_CAKE)
            mSerial. template open<Settings::BaudRate>();
        #elif defined(ARDUINO_ARCH_ESP32)
            mSerial.begin(Settings::BaudRate, SERIAL_8N1, Settings::RxPin, Settings::TxPin);
        #else
            mSerial.begin(Settings::BaudRate);
        #endif
	}

	bool beginTransmission(MidiType)
	{
		return true;
	};

	void write(byte value)
	{
		mSerial.write(value);
	};

	void endTransmission()
	{
	};

	byte read()
	{
		return mSerial.read();
	};

	unsigned available()
	{
        return mSerial.available();
	};

private:
    SerialPort& mSerial;
};

END_MIDI_NAMESPACE

/*! \brief Create an instance of the library attached to a serial port.
 You can use HardwareSerial or SoftwareSerial for the serial port.
 Example: MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);
 Then call midi2.begin(), midi2.read() etc..
 */
#define MIDI_CREATE_INSTANCE_ESP(Type, SerialPort, Name)  \
    MIDI_NAMESPACE::ESPSerialMIDI<Type> serial##Name(SerialPort);\
    MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::ESPSerialMIDI<Type>> Name((MIDI_NAMESPACE::ESPSerialMIDI<Type>&)serial##Name);

#if defined(ARDUINO_SAM_DUE) || defined(USBCON) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
    // Leonardo, Due and other USB boards use Serial1 by default.
    #define MIDI_CREATE_DEFAULT_INSTANCE_ESP()                                      \
        MIDI_CREATE_INSTANCE_ESP(HardwareSerial, Serial1, MIDI);
#else
    /*! \brief Create an instance of the library with default name, serial port
    and settings, for compatibility with sketches written with pre-v4.2 MIDI Lib,
    or if you don't bother using custom names, serial port or settings.
    */
    #define MIDI_CREATE_DEFAULT_INSTANCE_ESP()                                      \
        MIDI_CREATE_INSTANCE_ESP(HardwareSerial, Serial,  MIDI);
#endif

/*! \brief Create an instance of the library attached to a serial port with
 custom settings.
 @see DefaultSettings
 @see MIDI_CREATE_INSTANCE
 */
#define MIDI_CREATE_CUSTOM_INSTANCE_ESP(Type, SerialPort, Name, Settings)           \
    MIDI_NAMESPACE::ESPSerialMIDI<Type, Settings> serial##Name(SerialPort);\
    MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::ESPSerialMIDI<Type, Settings>, Settings> Name((MIDI_NAMESPACE::ESPSerialMIDI<Type, Settings>&)serial##Name);
