#!/bin/sh
# Extract language data in the po subdir to keep the relative paths intact
cd po && ../lang-extract.py --no-missing -s -o Firmware.pot ../../Firmware/[a-zA-Z]*.[ch]*
