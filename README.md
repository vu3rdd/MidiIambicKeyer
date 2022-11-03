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
