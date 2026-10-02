#!/usr/bin/env bash
# Запуск ArduCopter SITL без MAVProxy.
# Виконувати: WSL Ubuntu / Linux / macOS terminal.
#
#   ./run_sitl.sh               -> звичайна швидкість
#   SPEEDUP=10 ./run_sitl.sh    -> симуляція в 10 разів швидше (для 50–100 км)
#   WIPE=1 ./run_sitl.sh        -> скинути параметри (EEPROM) до дефолтних
#   HOME_LOC="50.4501,30.5234,180,0" ./run_sitl.sh   -> стартувати над Києвом
#
# Порти після старту:
#   TCP 5760 (SERIAL0) -> Mission Planner / QGroundControl
#   TCP 5762 (SERIAL1) -> C++ / MAVSDK
#   TCP 5763 (SERIAL2) -> вільний (напр. MAVProxy, другий скрипт)
set -e
ARDUPILOT_DIR="${ARDUPILOT_DIR:-$HOME/ardupilot}"
SPEEDUP="${SPEEDUP:-1}"
cd "$ARDUPILOT_DIR"

ARGS=(--model quad --speedup "$SPEEDUP"
      --defaults Tools/autotest/default_params/copter.parm)
[ -n "$HOME_LOC" ] && ARGS+=(--home "$HOME_LOC")
[ "${WIPE:-0}" = "1" ] && ARGS+=(--wipe)

echo "SITL: build/sitl/bin/arducopter ${ARGS[*]}"
exec build/sitl/bin/arducopter "${ARGS[@]}"
