#!/usr/bin/env bash
# Перевірити всі вправи одразу (WSL Ubuntu / Linux / macOS terminal):
#   ./check_all.sh
# Для кожного уроку компілює exercises.cpp і показує, скільки помилок лишилось.
cd "$(dirname "$0")"
mkdir -p .bin
for dir in lesson0*; do
    for src in "$dir"/exercises.cpp "$dir"/project.cpp; do
        [ -f "$src" ] || continue
        out=".bin/$(basename "$dir")"
        if ! g++ -std=c++17 -Wall "$src" -o "$out" 2> ".bin/$(basename "$dir").log"; then
            echo "$dir: ПОМИЛКА КОМПІЛЯЦІЇ (див. .bin/$(basename "$dir").log)"
            continue
        fi
        result=$("./$out" | tail -1)
        echo "$dir: $result"
    done
done
