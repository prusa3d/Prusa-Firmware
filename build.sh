#!/bin/bash

# Version: 1.6.0.2-Build_10
# Change log:
# 26 Nov. 2018, mkbel     , Automate secondary language support build.
# 18 Jan. 2019, mkbel     , Do not list build environment files being extracted on standard output.
# 13 Mar. 2019, mkbel     , Update build environment to fix maximum flash size check.
# 14 May  2019, mkbel     , Enable compiler warnings.
#  8 July 2019, 3d-gussner, update build.sh for the new board definition
#  9 July 2019, mkbel     , Use build environment which includes PrusaResearchRambo:avr:rambo board definition.
#  9 July 2019, mkbel     , Fix build time flash size check.
#  4 Feb. 2020, 3d-gussner, Change to 1.0.6.1
#  2 Mar. 2021, 3d-gussner, Run lang-community.sh in build.sh
# 12 July 2021, 3d-gussner, Download Arduino IDE from Ardunio.cc
#  2 Aug. 2021, 3d-gussner, Download from Arduino IDE 1.8.5 and Prusa Arduino_boards from prusa3d github release
#                           Change Ardunio IDE to portable
#                           Changed temporary download folder to `PF-build-dl`

ARDUINO_ENV="1.8.5"
BUILD_ENV="1.0.6.2"
BOARD_VERSION="1.0.4"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
BOARD="PrusaResearch:avr:prusa_einsy_rambo"
BOARD_PACKAGE_NAME="PrusaResearch"
BOARD_FILENAME="prusa3dboards"
#BOARD_FILE_URL="https://raw.githubusercontent.com/3d-gussner/Arduino_Boards/PFW-1257_Arduino_boards_v104/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"
BOARD_FILE_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"

#Check if Dowload folder exists
if [ ! -d "../PF-build-dl" ]; then
    mkdir ../PF-build-dl || exit 1
fi
cd ../PF-build-dl || exit 2

if [ ! -f "arduino-$ARDUINO_ENV-linux64.tar.xz" ]; then
    #wget https://downloads.arduino.cc/arduino-$ARDUINO_ENV-linux64.tar.xz || exit 3
    wget https://github.com/3d-gussner/PF-build-env-1/releases/download/Test_$ARDUINO_ENV/arduino-$ARDUINO_ENV-linux64.tar.xz || exit 3
    #wget https://github.com/prusa3d/PF-build-env/releases/download/arduino-$ARDUINO_ENV/arduino-$ARDUINO_ENV-linux64.tar.xz || exit 3
fi

if [ ! -d "../PF-build-env-$BUILD_ENV" ]; then
    mkdir ../PF-build-env-$BUILD_ENV 
    tar -xf arduino-$ARDUINO_ENV-linux64.tar.xz -C ../PF-build-env-$BUILD_ENV|| exit 4
    #unzip -q PF-build-env-Linux64-$BUILD_ENV.zip -d ../../PF-build-env-$BUILD_ENV || exit 4
fi

# Make Arduino IDE portable
if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/
fi

if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/output/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/output
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/sketchbook/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/sketchbook
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/sketchbook/libraries/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/sketchbook/libraries
fi
if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/staging/ ]; then
	mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/staging
fi

# Download and extract PrusaResearchRambo board

if [ ! -f "$BOARD_FILENAME-$BOARD_VERSION.tar.bz2" ]; then
	wget $BOARD_FILE_URL || exit 6
fi
if [ ! -d "../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION" ]; then
	echo "$(tput setaf 6)Unzipping $BOARD_PACKAGE_NAME Arduino IDE portable...$(tput setaf 2)"
	tar -xf $BOARD_FILENAME-$BOARD_VERSION.tar.bz2 -C ../PF-build-env-$BUILD_ENV/ || exit 10
	if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME ]; then
		mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME
	fi
	if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME ]; then
		mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME
	fi
	if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME/hardware ]; then
		mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME/hardware
	fi
	if [ ! -d ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr ]; then
		mkdir ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr
	fi
	
	mv ../PF-build-env-$BUILD_ENV/$BOARD_FILENAME-$BOARD_VERSION ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/portable/packages/$BOARD_PACKAGE_NAME/hardware/avr/$BOARD_VERSION
	echo "$(tput sgr 0)"
fi

cd ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV || exit 8
BUILD_ENV_PATH="$( pwd -P )"

cd ..

if [ ! -d "Prusa-Firmware-build" ]; then
    mkdir Prusa-Firmware-build  || exit 9
fi

cd Prusa-Firmware-build || exit 10
BUILD_PATH="$( pwd -P )"

if [ ! -f "$SCRIPT_PATH/Firmware/Configuration_prusa.h" ]; then
    cp $SCRIPT_PATH/Firmware/variants/1_75mm_MK3-EINSy10a-E3Dv6full.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 11
fi

export ARDUINO=$BUILD_ENV_PATH
export OUTDIR=$BUILD_PATH

#$BUILD_ENV_PATH/arduino $SCRIPT_PATH/Firmware/Firmware.ino --verify --board $BOARD --pref build.path=$BUILD_PATH --pref compiler.warning_level=all || exit 12
$BUILD_ENV_PATH/arduino-builder -verbose=0 -compile -fqbn=$BOARD -libraries=$ARDUINO/libraries -libraries $ARDUINO/portable/sketchbook/libraries -built-in-libraries $ARDUINO/libraries -hardware=$ARDUINO/hardware -hardware $ARDUINO/portable/packages -tools=$ARDUINO/hardware/tools/avr -tools $ARDUINO/portable/packages -tools=$ARDUINO/tools-builder -build-path=$BUILD_PATH -warnings=all $SCRIPT_PATH/Firmware/Firmware.ino || exit 12


cd $SCRIPT_PATH/lang
./lang-build.sh || exit 13
./lang-community.sh || exit 14
./fw-build.sh || exit 15
