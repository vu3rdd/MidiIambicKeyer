#!/usr/bin/python

import mido

device_names = mido.get_output_names()

# XXX: make sure that "Teensy MIDI" appears in the device names, if
# not, show an error and exit

tx = mido.open_output("Teensy MIDI:Teensy MIDI Port 1 24:0")
val = 30 # wpm
cmd = 0  # wpmControl
msg = mido.Message('control_change', channel=1, control=cmd & 0x7f, value = val &0x7f)

tx.send(msg)
