#!/bin/bash 
echo "***************************************************"
echo "WARNING: build.sh will be removed soon, do not use!"
echo "Switch your build to cmake or use PF-build.sh      "
echo "***************************************************"

BUILD_ENV="1.0.8"
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
LANGUAGES=$1

if [ ! -d "build-env" ]; then
    mkdir build-env || exit 1
fi
cd build-env || exit 2

if [ ! -d "../../PF-build-env/$BUILD_ENV-build" ]; then
    if [ ! -f "PF-build-env-Linux64-$BUILD_ENV.zip" ]; then
        #wget https://github.com/3d-gussner/PF-build-env-1/releases/download/$BUILD_ENV-Linux64/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
        wget https://github.com/prusa3d/PF-build-env/releases/download/$BUILD_ENV-Linux64/PF-build-env-Linux64-$BUILD_ENV.zip || exit 3
    fi
    unzip -q PF-build-env-Linux64-$BUILD_ENV.zip -d ../../PF-build-env/$BUILD_ENV-build || exit 4
fi

cd ../../PF-build-env/$BUILD_ENV-build || exit 5
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

if [[ ! -z $LANGUAGES && $LANGUAGES == "EN_FARM" ]]; then
    echo "English only language firmware will be built"
    sed -i -- "s/^#define LANG_MODE *1/#define LANG_MODE              0/g" $SCRIPT_PATH/Firmware/config.h
else
    sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              1/g" $SCRIPT_PATH/Firmware/config.h
fi

$BUILD_ENV_PATH/arduino $SCRIPT_PATH/Firmware/Firmware.ino --verify --board PrusaResearch:avr:prusa_einsy_rambo --pref build.path=$BUILD_PATH --pref compiler.warning_level=all || exit 9

export ARDUINO=$BUILD_ENV_PATH
export OUTDIR=$BUILD_PATH

if [ -z $LANGUAGES ]; then
    cd $SCRIPT_PATH/lang
    ./fw-build.sh || exit 10
fi

#Reset lang_mode
sed -i -- "s/^#define LANG_MODE *0/#define LANG_MODE              1/g" $SCRIPT_PATH/Firmware/config.h
