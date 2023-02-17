#!/bin/sh
set -xe
rm -rf build
mkdir build
cd build
cmake .. -G Ninja
ninja tests
ctest
