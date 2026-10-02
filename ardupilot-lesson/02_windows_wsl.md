# 2. Windows 10/11 + WSL2 (основний шлях)

Схема: **SITL і C++ — у WSL Ubuntu**, **Mission Planner — у Windows**.
WSL2 автоматично прокидає `localhost`, тому Mission Planner бачить SITL як `127.0.0.1:5760`.

> Правило шляхів: у WSL пишемо `~/ardupilot`, у Windows — `\\wsl$\Ubuntu\home\<user>\ardupilot`.
> Не змішувати `C:\...` у командах WSL і `/home/...` у Windows.

## 2.1 WSL2 + Ubuntu

**PowerShell (від адміністратора):**

```powershell
wsl --install -d Ubuntu
wsl --set-default-version 2
```

Перезавантажити ПК, відкрити **Ubuntu** з меню Пуск, створити користувача.

Перевірка — **WSL Ubuntu:**

```bash
lsb_release -a
g++ --version
```

## 2.2 ArduPilot SITL

**WSL Ubuntu:**

```bash
sudo apt update
sudo apt install -y git build-essential cmake ninja-build python3-venv python3-pip
cd ~
git clone --recurse-submodules https://github.com/ArduPilot/ardupilot.git
cd ardupilot
Tools/environment_install/install-prereqs-ubuntu.sh -y
. ~/.profile
./waf configure --board sitl
./waf copter
```

Результат: `~/ardupilot/build/sitl/bin/arducopter`.

### Якщо `./waf` пише `you need to install empy` (нові Ubuntu, PEP 668)

Python «externally managed» — ставимо пакети у venv, а не system-wide. **WSL Ubuntu:**

```bash
cd ~/ardupilot
python3 -m venv .venv
source .venv/bin/activate
python -m pip install empy==3.3.4 pexpect future
./waf configure --board sitl
./waf copter
```

> Якщо `arducopter` уже зібраний і запускається — цей крок не потрібен.

## 2.3 Перший запуск SITL

**WSL Ubuntu:**

```bash
cd ~/ardupilot
build/sitl/bin/arducopter --model quad --defaults Tools/autotest/default_params/copter.parm
```

Або скриптом з пачки (після копіювання проєкту, див. 2.5):

```bash
~/ardupilot-cpp/scripts/run_sitl.sh
```

Очікуваний вивід:

```
bind port 5760 for SERIAL0
SERIAL0 on TCP port 5760
Waiting for connection ....
```

SITL **чекає** першого клієнта на 5760 і не стартує симуляцію, поки той не підключиться.
Тому порядок: SITL → Mission Planner → C++.

## 2.4 Mission Planner (Windows)

Завантажити: <https://firmware.ardupilot.org/Tools/MissionPlanner/> → `MissionPlanner-latest.msi`.

Підключення — див. `05_gcs_mission_planner_qgc.md`: **TCP**, host `127.0.0.1`, port `5760`.

Якщо `127.0.0.1` не працює (рідко, старі збірки WSL) — **WSL Ubuntu:**

```bash
hostname -I
```

і в Mission Planner вказати цю IP-адресу замість `127.0.0.1`.

## 2.5 MAVSDK + C++ проєкт

MAVSDK з вихідних кодів — **WSL Ubuntu** (10–30 хв, один раз):

```bash
bash ~/ardupilot-cpp/scripts/install_mavsdk.sh
```

Скопіювати проєкт з пачки у WSL. Архів лежить у Windows, наприклад у `Downloads`.
**WSL Ubuntu:**

```bash
cd ~
unzip /mnt/c/Users/<WINDOWS_USER>/Downloads/ardupilot-lesson.zip
cp -r ardupilot-lesson/code/ardupilot-cpp ~/ardupilot-cpp
chmod +x ~/ardupilot-cpp/scripts/*.sh
~/ardupilot-cpp/scripts/check_env.sh
```

> Якщо `~/ardupilot-cpp` уже існує — скопіюй тільки потрібні файли
> (`main.cpp`, `examples/`, `scripts/`, `.vscode/`), щоб не затерти свою роботу.

## 2.6 VS Code

1. Встановити VS Code у **Windows**.
2. Розширення **WSL** (ms-vscode-remote.remote-wsl).
3. **WSL Ubuntu:**
   ```bash
   cd ~/ardupilot-cpp
   code .
   ```
   VS Code відкриється «всередині» WSL (зліва внизу `WSL: Ubuntu`).
4. Встановити запропоновані розширення (C/C++, CMake Tools) — вони у `.vscode/extensions.json`.
5. `Ctrl+Shift+B` — build. `Terminal → Run Task → run route` — build + run. `F5` — debug.
