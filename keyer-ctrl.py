#!/usr/bin/python

import mido


def midi_callback(msg):
    print(msg)

device_names = mido.get_output_names()

# XXX: make sure that "Teensy MIDI" appears in the device names, if
# not, show an error and exit

tx = mido.open_output("Teensy MIDI:Teensy MIDI Port 1 20:0", midi_callback)
rx = mido.open_input("Teensy MIDI:Teensy MIDI Port 1 20:0", midi_callback)

rx_channel=2 #python has 0 indexed channel numbers (teensy has 1 indexed)

val = 10 # wpm
cmd = 0  # wpmControl
msg = mido.Message('control_change', channel=1, control=cmd & 0x7f, value = val &0x7f)

tx.send(msg)

# get current WPM
# value doesn't matter
msg = mido.Message('control_change', channel=1, control=0x04 & 0x7f, value = 0)
tx.send(msg)

for msg in rx:
    print(msg)

# respMsg = rx.receive()
# print(respMsg)

# read the response from the response channel
