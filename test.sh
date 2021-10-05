#!/bin/bash
cd .. || exit 5

if [ ! -d "Prusa-Firmware-test" ]; then
    mkdir Prusa-Firmware-test  || exit 10
fi

cd Prusa-Firmware-test || exit 20

cmake -G "Eclipse CDT4 - Ninja" ../Prusa-Firmware || exit 30
cmake --build . || exit 35
./tests || exit 40