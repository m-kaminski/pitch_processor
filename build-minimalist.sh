#!/bin/bash
cd "$(dirname "$0")"

FILE_NAME=pitch_processor_minimalist

echo building in $FILE_NAME

c++ -std=c++2a -o $FILE_NAME \
src/main.cpp \
src/app_config.cpp \
src/execution_policy/execution_policy_single_threaded.cpp \
src/io_engine/io_engine_ios.cpp \
src/pitch/event_accumulator.cpp \
src/pitch/summary_formatter.cpp \
  -DMINIMALIST -O2

