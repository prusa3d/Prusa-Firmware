#!/bin/bash
ARDUINO_ENV="1.8.5"
BUILD_ENV="1.0.6.2"
BOARD_VERSION="1.0.4"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
BOARD="PrusaResearch:avr:prusa_einsy_rambo"
BOARD_PACKAGE_NAME="PrusaResearch"
BOARD_FILENAME="prusa3dboards"
BOARD_FILE_URL="https://raw.githubusercontent.com/3d-gussner/Arduino_Boards/PFW-1257_Arduino_boards_v104/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"
#BOARD_FILE_URL="https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/prusa3dboards-$BOARD_VERSION.tar.bz2"

if [ ! -d "../build-env" ]; then
    mkdir ../build-env || exit 1
fi
cd ../build-env || exit 2

if [ ! -f "arduino-$ARDUINO_ENV-linux64.tar.xz" ]; then
    wget --no-check-certificate https://downloads.arduino.cc/arduino-$ARDUINO_ENV-linux64.tar.xz || exit 3
    #wget https://github.com/3d-gussner/PF-build-env-1/releases/download/$BUILD_ENV-Linux64/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
    #wget https://github.com/prusa3d/PF-build-env/releases/download/$BUILD_ENV-Linux64/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
fi

if [ ! -d "../PF-build-env-$BUILD_ENV" ]; then
    mkdir ../PF-build-env-$BUILD_ENV 
    tar -xf arduino-$ARDUINO_ENV-linux64.tar.xz -C ../PF-build-env-$BUILD_ENV|| exit 4
    #unzip -q PF-build-env-Linux64-$BUILD_ENV.zip -d ../../PF-build-env-$BUILD_ENV || exit 4
fi

if [ ! -f "$BOARD_FILENAME-$BOARD_VERSION.tar.bz2" ]; then
	wget $BOARD_FILE_URL || exit 6
fi

if [ ! -d "../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/hardware/$BOARD_PACKAGE_NAME" ]; then
    mkdir "../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/hardware/$BOARD_PACKAGE_NAME" || exit 7
    tar -xf $BOARD_FILENAME-$BOARD_VERSION.tar.bz2 || exit 8
    mv $BOARD_FILENAME-$BOARD_VERSION ../PF-build-env-$BUILD_ENV/arduino-$ARDUINO_ENV/hardware/$BOARD_PACKAGE_NAME/avr 
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
$BUILD_ENV_PATH/arduino-builder -verbose=0 -compile -fqbn=$BOARD -libraries=$ARDUINO/libraries -built-in-libraries $ARDUINO/libraries -hardware=$ARDUINO/hardware -tools=$ARDUINO/hardware/tools/avr -tools=$ARDUINO/tools-builder -build-path=$BUILD_PATH -warnings=all $SCRIPT_PATH/Firmware/Firmware.ino


cd $SCRIPT_PATH/lang
./lang-build.sh || exit 13
./lang-community.sh || exit 14
./fw-build.sh || exit 15
