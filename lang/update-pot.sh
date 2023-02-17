#!/bin/sh
# Extract language data in the po subdir to keep the relative paths intact
cp -f po/Firmware.pot po/Firmware.pot.bak
cd po && ../lang-extract.py --no-missing -s -o Firmware.pot ../../Firmware/[a-zA-Z]*.[ch]* ../../Firmware/mmu2/[a-zA-Z]*.[ch]*
