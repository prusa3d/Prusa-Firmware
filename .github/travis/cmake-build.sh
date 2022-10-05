#!/bin/sh
set -xe

dep_path() { ./utils/bootstrap.py --print-dependency-directory "$1"; }

CMAKE_PATH=$(dep_path cmake)/bin
NINJA_PATH=$(dep_path ninja)

python --version
python3 --version

rm -rf build

$CMAKE_PATH/cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$PWD/cmake/LocalAvrGcc.cmake -DCMAKE_MAKE_PROGRAM:STRING=$NINJA_PATH/ninja -S$PWD -B$PWD/build -G Ninja
cd build
$NINJA_PATH/ninja ALL_ENGLISH
$NINJA_PATH/ninja ALL_MULTILANG
