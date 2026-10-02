#!/usr/bin/env bash
# Збірка та встановлення MAVSDK з вихідних кодів у /usr/local.
# Виконувати: WSL Ubuntu / Linux. (macOS: brew install mavsdk — простіше)
# Займає 10–30 хв. Якщо MAVSDK вже встановлено — НЕ запускай.
set -e
sudo apt update
sudo apt install -y build-essential cmake git ninja-build python3 python3-pip curl
cd ~
[ -d MAVSDK ] || git clone https://github.com/mavlink/MAVSDK.git --recursive
cd MAVSDK
git submodule update --init --recursive
cmake -G Ninja -B build/default -S cpp -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build build/default -j"$(nproc)"
sudo cmake --build build/default --target install
sudo ldconfig
echo "MAVSDK встановлено. Перевірка: ls /usr/local/include/mavsdk/plugins/action/"
