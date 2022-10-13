#!/bin/sh
set -xe
rm -rf build
mkdir build
cd build
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../cmake/AvrGcc.cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -G Ninja

# ignore all failures in order to show as much output as possible
ninja -k0 check_lang || true
