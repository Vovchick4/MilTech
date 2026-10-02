#!/usr/bin/env bash
# WSL Ubuntu / Linux / macOS terminal, з папки ~/ardupilot-cpp:
#   ./scripts/build_run.sh                 -> збірка + ardupilot_cpp
#   ./scripts/build_run.sh 01_telemetry    -> збірка + приклад
set -e
cd "$(dirname "$0")/.."
TARGET="${1:-ardupilot_cpp}"
[ -f build/CMakeCache.txt ] || cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
exec "./build/${TARGET}"
