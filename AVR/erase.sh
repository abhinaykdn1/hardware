#!/bin/bash

avrdude -c arduino -b 19200 -p m168 -P /dev/tty.SLAB_USBtoUART -v -e
