# USB to UART MIDI converter for CircuitPython 
#
import board
import busio
import digitalio
import usb_midi
import adafruit_midi

from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff
from adafruit_midi.control_change import ControlChange
from adafruit_midi.program_change import ProgramChange
from adafruit_midi.channel_pressure import ChannelPressure
from adafruit_midi.polyphonic_key_pressure import PolyphonicKeyPressure
from adafruit_midi.pitch_bend import PitchBend
from adafruit_midi.start import Start
from adafruit_midi.stop import Stop
from adafruit_midi.midi_continue import Continue
from adafruit_midi.timing_clock import TimingClock
from adafruit_midi.mtc_quarter_frame import MtcQuarterFrame
from adafruit_midi.system_exclusive import SystemExclusive
from adafruit_midi.midi_message import MIDIUnknownEvent

uart = busio.UART(tx=board.GP0, rx=board.GP1, baudrate=31250, timeout=0.001)
sermidi = adafruit_midi.MIDI(midi_in=uart, midi_out=uart)
usbmidi = adafruit_midi.MIDI(midi_in=usb_midi.ports[0], midi_out=usb_midi.ports[1])

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

while True:
    msg = usbmidi.receive()
    if (isinstance(msg, MIDIUnknownEvent)):
        # Ignore unknown MIDI events
        # This filters out the ActiveSensing
        pass
    elif msg is not None:
        led.value = True
        sermidi.send(msg)
    else:
       # Ignore "None"
       pass

    msg = sermidi.receive()
    if (isinstance(msg, MIDIUnknownEvent)):
        # Ignore unknown MIDI events
        pass
    elif msg is not None:
        led.value = True
        usbmidi.send(msg)
    else:
       # Ignore "None"
       pass

    led.value = False