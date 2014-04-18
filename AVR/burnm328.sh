#!/bin/bash

#avrdude -c stk500v2 -p m168 -P /dev/tty.usbserial-A8004PfX -v -e -U efuse:w:0xF8:m -U hfuse:w:0xDF:m -U lfuse:w:0xFF:m
avrdude -c stk500v2 -p m328p -P /dev/tty.usbserial-A8004Dfg -v -e -F -U flash:w:ATmegaBOOT_168_atmega328.hex
#avrdude -c stk500v2 -p m328p -P /dev/tty.usbserial-A8004Dfg -v -e -F -U flash:w:ATmegaBOOT_168_diecimila.hex
