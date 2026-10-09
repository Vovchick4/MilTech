#!/usr/bin/env bash
# Зібрати і запустити одну програму з уроку.
#   ./scripts/run.sh lesson05_telemetry
# (виконувати з папки code/)
set -e
cd "$(dirname "$0")/.."
if [ -z "$1" ]; then
  echo "Вкажи програму, наприклад: ./scripts/run.sh lesson04_connect"
  exit 1
fi
cmake -S . -B build > /dev/null
cmake --build build --target "$1"
echo "================ запуск $1 ================"
./build/"$1"
