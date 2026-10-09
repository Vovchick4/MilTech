#!/usr/bin/env bash
# Встановлення всього потрібного на Ubuntu (WSL на Windows або звичайний Linux).
# Запуск:  bash scripts/install_ubuntu.sh
# Займає 30–60 хвилин. Можна йти пити чай.
# Якщо щось впало — НЕ панікуй: скопіюй останні 20 рядків і покажи викладачу.
set -e

echo "===== 1/4. Базові інструменти ====="
sudo apt update
sudo apt install -y git build-essential cmake ninja-build python3 python3-pip python3-venv curl

echo "===== 2/4. ArduPilot (симулятор дрона) ====="
cd ~
if [ ! -d ardupilot ]; then
  git clone --recurse-submodules https://github.com/ArduPilot/ardupilot.git
fi
cd ardupilot
Tools/environment_install/install-prereqs-ubuntu.sh -y || echo "(!) install-prereqs завершився з помилкою — пробуємо далі через venv"
python3 -m venv .venv
source .venv/bin/activate
python -m pip install empy==3.3.4 pexpect future
./waf configure --board sitl
./waf copter
deactivate
echo "ArduPilot готовий: ~/ardupilot/build/sitl/bin/arducopter"

echo "===== 3/4. MAVSDK (бібліотека для C++) ====="
cd ~
if [ ! -d MAVSDK ]; then
  git clone --recursive https://github.com/mavlink/MAVSDK.git
fi
cd MAVSDK
git submodule update --init --recursive
cmake -G Ninja -B build/default -S cpp -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF
cmake --build build/default
sudo cmake --build build/default --target install
sudo ldconfig
echo "MAVSDK готовий: /usr/local/include/mavsdk"

echo "===== 4/4. Перевірка ====="
ls ~/ardupilot/build/sitl/bin/arducopter && echo "[OK] arducopter"
ls /usr/local/lib/cmake/MAVSDK/MAVSDKConfig.cmake && echo "[OK] MAVSDK"
echo "Усе встановлено!"
