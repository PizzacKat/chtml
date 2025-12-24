#!/usr/bin/env bash

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "$@" -S . -B build
cp build/compile_commands.json .
cmake --build build
