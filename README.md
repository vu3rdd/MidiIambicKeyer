# MidiIambicKeyer
CW Keyer interface for Teensy boards.

A simple Sketch for using the Teensy as a simple interface for connecting a straight key or an external keyer. Plus added functionality as a simple Iambic keyer.

See here for more information: https://dl3lsm.blogspot.com/2020/06/using-teensy-as-cw-keying-interface.html

# VU2JXN modifications.

A stereo socket can be plugged into a small breadboard on which teensy
also sits. The socket I plugged in makes the GPIO pin 6 and 10 as the
dit/dah inputs.

A 220ohm resistor and a small value capacitor was plugged into the
GPIO pin 4 in series. The negative pin of the capacitor goes into
another stereo plug. I plugged the stereo pin such that the sheath pin
sits on the breadboard pins corresponding to the GND pin (physical pin
0) of teensy.

The small modification in the code produces an 800Hz tone (square wave
but not too bad) as sidetone.

# modifying CW parameters from the host

The TeensyKeyer also accepts inputs from the host via
channel 2. Things like CW speed, Iambic mode etc can be changes.

To play with it from Python, install the library `mido`. On debian:

```
sudo apt install python3-mido python3-rtmidi
```



```
$ python
Python 3.10.8 (main, Oct 24 2022, 10:07:16) [GCC 12.2.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import mido
>>> mido.get_output_names()
['Midi Through:Midi Through Port-0 14:0', 'Teensy MIDI:Teensy MIDI Port 1 24:0', 'Midi Through:Midi Through Port-0 14:0']
>>> tx = mido.open_output("Teensy MIDI:Teensy MIDI Port 1 24:0")
>>> val = 30 # wpmControl
>>> cmd = 0 # wpmControl
>>> msg = mido.Message('control_change', channel=1, control=cmd & 0x7f, value = val &0x7f)
>>> tx.send(msg)
```

Now, the WPM of the keyer is changed. Similarly other parameters can also be changed.

This is done via MIDI "control change" (0xb0) command. The channel
number defined in Teensy is 2, but it is one less when used via host.
