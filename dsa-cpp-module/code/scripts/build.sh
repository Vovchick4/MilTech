#!/usr/bin/env bash
# WSL Ubuntu / Linux / macOS terminal, з папки code/:
#   ./scripts/build.sh            -> Release-збірка
#   ./scripts/build.sh asan       -> Debug + AddressSanitizer/UBSan (у build-asan/)
set -e
cd "$(dirname "$0")/.."
if [ "$1" = "asan" ]; then
  cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DDSA_SANITIZE=ON
  cmake --build build-asan -j
else
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build -j
fi
