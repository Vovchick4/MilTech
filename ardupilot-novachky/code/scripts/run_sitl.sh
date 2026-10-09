#!/usr/bin/env bash
# Запустити симулятор дрона (SITL).
#   ./scripts/run_sitl.sh              -> звичайна швидкість
#   SPEEDUP=10 ./scripts/run_sitl.sh   -> у 10 разів швидше (для довгих маршрутів)
#   WIPE=1 ./scripts/run_sitl.sh       -> скинути всі налаштування дрона до заводських
cd "${ARDUPILOT_DIR:-$HOME/ardupilot}" || { echo "Не знайдено папку ArduPilot (~/ardupilot)"; exit 1; }
EXTRA=""
[ "${WIPE:-0}" = "1" ] && EXTRA="--wipe"
exec build/sitl/bin/arducopter --model quad --speedup "${SPEEDUP:-1}" \
     --defaults Tools/autotest/default_params/copter.parm $EXTRA
