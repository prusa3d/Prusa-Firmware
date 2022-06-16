#!/bin/bash
#
# Version 1.0.1 Build 10
#
# config.sh - multi-language support configuration script
#  Definition of absolute paths etc.
#  This file is 'included' in all scripts.
#
#############################################################################
# Change log:
# 31 May  2018, XPila,     Initial
# 17 Dec. 2021, 3d-gussner, Change default Arduino path to by PF-build.sh
#                           created one
#                           Prepare to use one config file for all languages
# 11 Jan. 2022, 3d-gussner, Added version and Change log colored output
#                           Use `git rev-list --count HEAD config.sh`
#                           to get Build Nr
#                           Check if variables are set by other scripts
#                           and use these. More flexible for different build
#                           scripts
#                           Check correctly if files or dirs exist
# 10 Feb. 2022, 3d-gussner, Add SRCDIR for compatibility with build server
#############################################################################
#
if [ -z "$SRCDIR" ]; then
    export SRCDIR=".."
fi

LNGDIR="$( cd "$(dirname "$0")" ; pwd -P )"
export LNGDIR=$LNGDIR

# Arduino main folder:
if [ -z "$ARDUINO" ]; then
    export ARDUINO=../../PF-build-env-1.0.6/1.8.5-1.0.4-linux-64 #C:/arduino-1.8.5
fi
#
# AVR gcc tools:
if [ -z "$OBJCOPY" ]; then
    export OBJCOPY=$ARDUINO/hardware/tools/avr/bin/avr-objcopy
fi
#
# Output folder:
if [ -z "$OUTDIR" ]; then
    export OUTDIR="../../Prusa-Firmware-build"
fi
#
# Output elf file:
if [ -z "$INOELF" ]; then
    export INOELF="$OUTDIR/Firmware.ino.elf"
fi
#
# Output hex file:
if [ -z "$INOHEX" ]; then
    export INOHEX="$OUTDIR/Firmware.ino.hex"
fi
#
# Generated multi-language hex file prefix:
if [ -z "$INTLHEX" ]; then
    export INTLHEX="$LNGDIR/Firmware-intl"
fi
#
# Set default languages
if [ -z "$LANGUAGES" ]; then
    export LANGUAGES="cs de es fr it pl"
fi
#
# Check for community languages
MAX_COMMINITY_LANG=10 # Total 16 - 6 default
COMMUNITY_LANGUAGES=""
#Search Firmware/config.h for active community group
COMMUNITY_LANG_GROUP=$(grep --max-count=1 "^#define COMMUNITY_LANG_GROUP" $SRCDIR/Firmware/config.h| cut -d ' ' -f3)

# Search Firmware/config.h for active community languanges
if [ "$COMMUNITY_LANG_GROUP" = "1" ]; then
    COMMUNITY_LANGUAGES=$(grep --max-count=$MAX_COMMINITY_LANG "^#define COMMUNITY_LANG_GROUP1_" $SRCDIR/Firmware/config.h| cut -d '_' -f4 |cut -d ' ' -f1 |tr '[:upper:]' '[:lower:]'| tr '\n' ' ')
elif [ "$COMMUNITY_LANG_GROUP" = "2" ]; then
    COMMUNITY_LANGUAGES=$(grep --max-count=$MAX_COMMINITY_LANG "^#define COMMUNITY_LANG_GROUP2_" $SRCDIR/Firmware/config.h| cut -d '_' -f4 |cut -d ' ' -f1 |tr '[:upper:]' '[:lower:]'| tr '\n' ' ')
elif [ "$COMMUNITY_LANG_GROUP" = "3" ]; then
    COMMUNITY_LANGUAGES=$(grep --max-count=$MAX_COMMINITY_LANG "^#define COMMUNITY_LANG_GROUP3_" $SRCDIR/Firmware/config.h| cut -d '_' -f4 |cut -d ' ' -f1 |tr '[:upper:]' '[:lower:]'| tr '\n' ' ')
fi

# End of customization
######################

if [ -z "$COMMUNITY_LANGUAGES" ]; then
    export COMMUNITY_LANGUAGES="$COMMUNITY_LANGUAGES"
fi

if [ ! -t 2 -o "$TERM" = "dumb" ]; then
    NO_COLOR=1
fi

color()
{
    color=$1
    shift
    if [ "$NO_COLOR" = 0 -o -z "$NO_COLOR" ]; then
        echo "$(tput setaf $color)$*$(tput sgr 0)"
    else
        echo "$*"
    fi
}

ok() { color 2 "OK"; }
ng() { color 1 "NG!"; }

color 2 "config.sh started" >&2

_err=0

echo -n " Arduino main folder: " >&2
if [ -d $ARDUINO ]; then ok >&2; else ng >&2; _err=1; fi

echo " AVR gcc tools:" >&2
echo -n "   objcopy " >&2
if [ -e $OBJCOPY ]; then ok >&2; else ng >&2; _err=3; fi

echo -n " Output folder: " >&2
if [ -d $OUTDIR ]; then ok >&2; else ng >&2; _err=5; fi

echo -n " Output elf file: " >&2
if [ -e $INOELF ]; then ok >&2; else ng >&2; _err=7; fi

echo -n " Output hex file: " >&2
if [ -e $INOHEX ]; then ok >&2; else ng >&2; _err=8; fi

echo -n " Intl hex file prefix: " >&2
if [ -n $INTLHEX ]; then ok >&2; else ng >&2; _err=8; fi

echo -n " Languages: " >&2
color 2 "$LANGUAGES" >&2

echo -n " Community languages: " >&2
color 2 "$COMMUNITY_LANGUAGES" >&2

if [ $_err -eq 0 ]; then
    color 2 "config.sh finished with success" >&2
    export CONFIG_OK=1
else
    color 1 "config.sh finished with errors!" >&2
    export CONFIG_OK=0
fi
