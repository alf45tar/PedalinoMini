 /* ShiftOut.h - Arduino library that writes out values to an 8 bit shift register (74HC595).
 * You can daisy-chain several shift register in order to set up to 64 pins by only using 3 Arduino pins.
 *
 * Created by Henrik Heine, July 24, 2016
 *
LICENSE
The MIT License (MIT)

Copyright (c) 2016 Henrik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef ShiftOut_h
#define ShiftOut_h

#include "Arduino.h"

template<byte chipCount, typename ShiftType>
class _ShiftOut {
private:
	byte dataPin;
	byte clockPin;
	byte latchPin;

	const uint16_t dataWidth;

	ShiftType state;
public:
	_ShiftOut() : dataWidth(chipCount * 8), state(0) {}
	
	// setup all pins
	void begin(int data, int clock, int latch) {
		pinMode(dataPin = data, OUTPUT);
		pinMode(clockPin = clock, OUTPUT);
		pinMode(latchPin = latch, OUTPUT);
		write();
	}
	
	inline uint16_t getDataWidth() { return dataWidth; }
	
	inline boolean get(int id) { return bitRead(state, id); }
	inline void set(int id, int value) {
		if(value == 0) setLow(id);
		else setHigh(id);
	}
	inline void setHigh(int id) { state = bitSet(state, id); }
	inline void setLow(int id) { state = bitClear(state, id); }
	inline void invert(int id) { set(id, !get(id)); }
	
	inline void setAllHigh() { state = ~0; }
	inline void setAllLow() { state = 0; }
	inline void invert() { state = ~state; }
	
	ShiftType write() {
		for(byte i = 0; i < chipCount; i++) {
			shiftOut(dataPin, clockPin, MSBFIRST, (byte)(state >> (8 * i)));
		}
		digitalWrite(latchPin, HIGH); 
		digitalWrite(latchPin, LOW);
		return state;
	}
	ShiftType write(ShiftType data) {
		state = data;
		return write();
	}

	void kittCar() {
  		setAllLow();
  		write();
  		for (int i = 0; i < dataWidth; i++) {
    		setAllLow();
    		setHigh(i);
    		write();
    		delay(50);
  		}
  		for (int i = dataWidth-1; i >= 0; i--) {
    		setAllLow();
    		setHigh(i);
    		write();
    		delay(50);
  		}
  		setAllLow();
  		write();
	}
};

// fallback with 64 bit state (up to 8 shift registers)
template<byte chipCount>
class ShiftOut : public _ShiftOut<chipCount, uint64_t> {};
// single shift register (8 bit state)
template<>
class ShiftOut<1> : public _ShiftOut<1, uint8_t> {};
// two shift registers (16 bit state)
template<>
class ShiftOut<2> : public _ShiftOut<2, uint16_t> {};
// three shift registers (32 bit state)
template<>
class ShiftOut<3> : public _ShiftOut<3, uint32_t> {};
// four shift registers (32 bit state)
template<>
class ShiftOut<4> : public _ShiftOut<4, uint32_t> {};

#endif