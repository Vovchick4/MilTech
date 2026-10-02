# 3. macOS (Intel та Apple Silicon)

На Mac усе працює **нативно**, без WSL і без Docker.
Mission Planner — програма під Windows (на Mac через Mono працює нестабільно),
тому на Mac використовуємо **QGroundControl**.

Схема:

```
SITL (Terminal) ── TCP 5760 ──► QGroundControl
                └─ TCP 5762 ──► C++ (MAVSDK)
```

## 3.1 Інструменти

**macOS Terminal:**

```bash
xcode-select --install
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake ninja git
```

## 3.2 ArduPilot SITL

**macOS Terminal:**

```bash
cd ~/Documents/GitHub          # або будь-яка папка
git clone --recurse-submodules https://github.com/ArduPilot/ardupilot.git
cd ardupilot
Tools/environment_install/install-prereqs-mac.sh -y
source ~/.zshrc                 # або відкрити новий термінал
./waf configure --board sitl
./waf copter
```

Якщо ArduPilot уже є (наприклад `/Users/admin/Documents/GitHub/ardupilot`) — достатньо
`./waf configure --board sitl && ./waf copter`.

Запуск **macOS Terminal:**

```bash
ARDUPILOT_DIR=~/Documents/GitHub/ardupilot ~/ardupilot-cpp/scripts/run_sitl.sh
```

## 3.3 QGroundControl

Завантажити `.dmg`: <https://docs.qgroundcontrol.com/master/en/qgc-user-guide/getting_started/download_and_install.html>.
Підключення — `05_gcs_mission_planner_qgc.md` (TCP, `127.0.0.1`, `5760`).

## 3.4 MAVSDK

**macOS Terminal** — найпростіше через Homebrew:

```bash
brew install mavsdk
```

Якщо формула недоступна — зібрати з вихідних кодів:

```bash
cd ~
git clone --recursive https://github.com/mavlink/MAVSDK.git
cd MAVSDK
cmake -G Ninja -B build/default -S cpp -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build build/default
sudo cmake --build build/default --target install
```

## 3.5 C++ проєкт

**macOS Terminal:**

```bash
cp -r ~/Downloads/ardupilot-lesson/code/ardupilot-cpp ~/ardupilot-cpp
cd ~/ardupilot-cpp
chmod +x scripts/*.sh
./scripts/check_env.sh
./scripts/build_run.sh
```

> Homebrew на Apple Silicon ставить у `/opt/homebrew`. Якщо CMake не знаходить MAVSDK:
> `cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/homebrew`.

## 3.6 Відомі особливості macOS

- Перший запуск `arducopter` — macOS може спитати дозвіл на вхідні мережеві з'єднання → **Allow**.
- `check_env.sh` на Mac перевіряє порти через `lsof` (немає `ss`) — це нормально.
- Заголовки MAVSDK з brew: `/opt/homebrew/include/mavsdk` (не `/usr/local`).
