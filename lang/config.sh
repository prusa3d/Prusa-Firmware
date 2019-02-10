#!/bin/sh
#
# config.sh - multi-language support configuration script
#  Definition of absolute paths etc.
#  This file is 'included' in all scripts.
#
# Arduino main folder:
if [ -z "$ARDUINO" ]; then
    export ARDUINO=C:/arduino-1.8.5
fi
#
# Arduino builder:
export BUILDER=$ARDUINO/arduino-builder
#
# AVR gcc tools:
export OBJCOPY=$ARDUINO/hardware/tools/avr/bin/avr-objcopy
export OBJDUMP=$ARDUINO/hardware/tools/avr/bin/avr-objdump
#
# Output folder:
export OUTDIR="../../Prusa-Firmware-build"
#
# Objects folder:
export OBJDIR="$OUTDIR/sketch"
#
# Generated elf file:
export INOELF="$OUTDIR/Firmware.ino.elf"
#
# Generated hex file:
export INOHEX="$OUTDIR/Firmware.ino.hex"


echo "config.sh started" >&2

_err=0

echo -n " Arduino main folder: " >&2
if [ -e $ARDUINO ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=1; fi

echo -n " Arduino builder: " >&2
if [ -e $BUILDER ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=2; fi

echo " AVR gcc tools:" >&2
echo -n "   objcopy " >&2
if [ -e $OBJCOPY ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=3; fi
echo -n "   objdump " >&2
if [ -e $OBJDUMP ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=4; fi

echo -n " Output folder: " >&2
if [ -e $OUTDIR ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=5; fi

echo -n " Objects folder: " >&2
if [ -e $OBJDIR ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=6; fi

echo -n " Generated elf file: " >&2
if [ -e $INOELF ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=7; fi

echo -n " Generated hex file: " >&2
if [ -e $INOHEX ]; then echo 'OK' >&2; else echo 'NG!' >&2; _err=8; fi

if [ $_err -eq 0 ]; then
 echo "config.sh finished with success" >&2
 export CONFIG_OK=1
else
 echo "config.sh finished with errors!" >&2
 export CONFIG_OK=0
fi
