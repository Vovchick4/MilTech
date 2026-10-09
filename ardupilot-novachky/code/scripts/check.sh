#!/usr/bin/env bash
# Перевірити, чи все встановлено. Запуск: bash scripts/check.sh
ok()  { echo "  [OK]    $1"; }
bad() { echo "  [НЕМАЄ] $1  -> $2"; }

command -v g++   >/dev/null && ok "компілятор g++"  || bad "компілятор g++" "sudo apt install build-essential"
command -v cmake >/dev/null && ok "cmake"           || bad "cmake" "sudo apt install cmake"
[ -x ~/ardupilot/build/sitl/bin/arducopter ] && ok "симулятор ArduPilot (arducopter)" \
  || bad "симулятор ArduPilot" "див. урок 1, крок ArduPilot"
[ -f /usr/local/lib/cmake/MAVSDK/MAVSDKConfig.cmake ] || [ -f /opt/homebrew/lib/cmake/MAVSDK/MAVSDKConfig.cmake ] \
  && ok "бібліотека MAVSDK" || bad "бібліотека MAVSDK" "див. урок 1, крок MAVSDK"
