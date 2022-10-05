#!/bin/sh
set -e
export PATH=$PWD/.dependencies/cmake-*/bin:$PWD/.dependencies/ninja-*:$PATH
rm -rf build && mkdir build && cd build
cmake .. -CCMAKE_TOOCHAIN_FILE="../cmake/LocalAvrGcc.cmake" -G Ninja
ninja ALL_FIRMWARE
