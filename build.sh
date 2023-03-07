#!/bin/bash
cd "$(dirname "$0")"


cmake -S src -B build
cmake --build build -j

if [ $? -ne 0 ]
then
    echo build failed
else
    cd build && ctest
if [ $? -ne 0 ]
then
    ctest --rerun-failed --output-on-failure
    fi
fi
