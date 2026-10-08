#!/usr/bin/env bash
# Зібрати і запустити тести.
#   ./scripts/test.sh          -> усі тести
#   ./scripts/test.sh T04      -> тільки задача T04
set -e
cd "$(dirname "$0")/.."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release > /dev/null
cmake --build build -j
echo "===== Лекційний код ====="
./build/test_lecture
echo
echo "===== Задачі ====="
./build/test_tasks "$1"
