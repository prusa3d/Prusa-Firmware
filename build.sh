#!/bin/bash 
BUILD_ENV="1.0.6.1"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

if [ ! -d "build-env" ]; then
    mkdir build-env || exit 1
fi
cd build-env || exit 2

if [ ! -f "PF-build-env-Linux64-$BUILD_ENV.zip" ]; then
    #wget https://github.com/3d-gussner/PF-build-env-1/releases/download/$BUILD_ENV-Linux64/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
	wget https://github.com/prusa3d/PF-build-env/releases/download/$BUILD_ENV-Linux64/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
fi

if [ ! -d "../../PF-build-env-$BUILD_ENV" ]; then
    unzip -q PF-build-env-Linux64-$BUILD_ENV.zip -d ../../PF-build-env-$BUILD_ENV || exit 4
fi

cd ../../PF-build-env-$BUILD_ENV || exit 5
BUILD_ENV_PATH="$( pwd -P )"

cd ..

if [ ! -d "Prusa-Firmware-build" ]; then
    mkdir Prusa-Firmware-build  || exit 6
fi

cd Prusa-Firmware-build || exit 7
BUILD_PATH="$( pwd -P )"

if [ ! -f "$SCRIPT_PATH/Firmware/Configuration_prusa.h" ]; then
    cp $SCRIPT_PATH/Firmware/variants/1_75mm_MK3-EINSy10a-E3Dv6full.h $SCRIPT_PATH/Firmware/Configuration_prusa.h || exit 8
fi

$BUILD_ENV_PATH/arduino $SCRIPT_PATH/Firmware/Firmware.ino --verify --board PrusaResearchRambo:avr:rambo --pref build.path=$BUILD_PATH --pref compiler.warning_level=all || exit 9

export ARDUINO=$BUILD_ENV_PATH

cd $SCRIPT_PATH/lang
./lang-build.sh || exit 10
./fw-build.sh || exit 11
