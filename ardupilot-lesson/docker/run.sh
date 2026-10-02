#!/usr/bin/env bash
# Linux / macOS terminal. Запуск SITL-контейнера з пробросом портів.
#   ./docker/run.sh            -> звичайна швидкість
#   SPEEDUP=10 ./docker/run.sh -> прискорена симуляція
docker run --rm -it --name ardupilot-sitl \
  -e SPEEDUP="${SPEEDUP:-1}" \
  -p 5760:5760 -p 5762:5762 -p 5763:5763 \
  ardupilot-sitl
