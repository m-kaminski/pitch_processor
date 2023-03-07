#!/bin/bash
cd "$(dirname "$0")"


cmake -S src -B build
cmake --build build -j
cd build && ctest
