#!/bin/sh
set -e
export PATH=$PWD/.dependencies/cmake-*/bin:$PWD/.dependencies/ninja-*:$PATH
rm -rf build && mkdir build && cd build
cmake .. -G Ninja
ninja tests
ctest
