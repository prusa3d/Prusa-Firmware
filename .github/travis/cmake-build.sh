#!/bin/sh
set -xe

dep_path() { ./utils/bootstrap.py --print-dependency-directory "$1"; }
PATH=$PWD/$(dep_path cmake)/bin:$PATH
PATH=$PWD/$(dep_path ninja):$PATH

rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="../cmake/LocalAvrGcc.cmake" -G Ninja
ninja ALL_FIRMWARE
