#!/usr/bin/env bash
# Діагностика середовища. Виконувати: WSL Ubuntu / Linux / macOS terminal.
ok()   { printf "  [OK]  %s\n" "$1"; }
bad()  { printf "  [!!]  %s\n" "$1"; }

echo "== Компілятори =="
command -v g++   >/dev/null && ok "g++ $(g++ -dumpfullversion)" || bad "g++ не знайдено"
command -v cmake >/dev/null && ok "$(cmake --version | head -1)" || bad "cmake не знайдено"

echo "== ArduPilot =="
AP="${ARDUPILOT_DIR:-$HOME/ardupilot}"
[ -x "$AP/build/sitl/bin/arducopter" ] && ok "$AP/build/sitl/bin/arducopter" \
  || bad "arducopter не зібрано: cd $AP && ./waf configure --board sitl && ./waf copter"

echo "== MAVSDK =="
INC=/usr/local/include/mavsdk
[ -d "$INC" ] && ok "headers: $INC" || bad "немає $INC"
if [ -f "$INC/plugins/action/action.hpp" ]; then
  ok "Action: #include <mavsdk/plugins/action/action.hpp>"
elif [ -f "$INC/plugins/action/action.h" ]; then
  ok "Action: #include <mavsdk/plugins/action/action.h> (стара MAVSDK)"
else
  bad "Action header не знайдено"
fi
ls /usr/local/lib/cmake/MAVSDK/MAVSDKConfig.cmake >/dev/null 2>&1 \
  && ok "CMake config: /usr/local/lib/cmake/MAVSDK" || bad "MAVSDKConfig.cmake не знайдено"
grep -ho "MAVSDK::[a-z_]*" /usr/local/lib/cmake/MAVSDK/MAVSDKTargets.cmake 2>/dev/null | sort -u | sed 's/^/        target: /'

echo "== Порти SITL (SITL має бути запущений) =="
if command -v ss >/dev/null; then
  for p in 5760 5762 5763; do
    ss -ltn | grep -q ":$p " && ok "TCP $p слухає" || bad "TCP $p не слухає"
  done
else
  for p in 5760 5762 5763; do
    lsof -nP -iTCP:$p -sTCP:LISTEN >/dev/null 2>&1 && ok "TCP $p слухає" || bad "TCP $p не слухає"
  done
fi
