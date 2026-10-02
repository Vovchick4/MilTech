# Модуль: ArduPilot SITL + Mission Planner / QGroundControl + C++ (MAVSDK)

Мета модуля — студент розуміє, **як влаштований стек дрона**, вміє **запустити симулятор**,
**підключити наземну станцію** і **керувати дроном з власної C++ програми**,
бачачи політ на карті.

Результат уроку: симульований квадрокоптер злітає на 100 м і проходить
квадратний маршрут 10 / 50 / 100 км, керований C++ кодом, а траєкторія видна
в Mission Planner (або QGroundControl).

## Структура пачки

| Файл | Що всередині |
|---|---|
| `01_teoriya.md` | Як усе працює: ArduPilot, SITL, MAVLink, GCS, MAVSDK, порти |
| `02_windows_wsl.md` | Встановлення на Windows 10/11 через WSL2 (основний шлях) |
| `03_macos.md` | Встановлення на macOS (нативно) |
| `04_linux_docker.md` | Linux нативно + Docker як альтернатива (з відомими проблемами) |
| `05_gcs_mission_planner_qgc.md` | Підключення Mission Planner і QGroundControl |
| `06_cpp_mavsdk.md` | C++ проєкт: CMake, підключення, телеметрія, Action, розбір коду |
| `07_praktyka.md` | Практичні завдання, чекліст, домашка, питання для самоперевірки |
| `08_troubleshooting.md` | Типові помилки і як їх лагодити |
| `code/ardupilot-cpp/` | Готовий C++ проєкт (main.cpp, приклади, CMake, VS Code, скрипти) |
| `docker/` | Dockerfile + скрипт запуску SITL у контейнері |

## Швидкий старт (якщо все вже встановлено)

Термінал 1 — **WSL Ubuntu** (або Linux / macOS terminal):

```bash
~/ardupilot-cpp/scripts/run_sitl.sh
```

Windows — **Mission Planner**: TCP → `127.0.0.1` → `5760` → Connect.

Термінал 2 — **WSL Ubuntu**:

```bash
cd ~/ardupilot-cpp
./scripts/build_run.sh
```

## План заняття (≈ 3 × 45 хв)

1. **Теорія** (30 хв) — `01_teoriya.md`: архітектура, MAVLink, порти, хто з ким говорить.
2. **Середовище** (30–45 хв) — встановлення під свою ОС, запуск SITL.
3. **GCS** (20 хв) — Mission Planner / QGC, ручний зліт з GCS, режими польоту.
4. **C++** (45 хв) — телеметрія → зліт/посадка → великий маршрут.
5. **Практика** (решта) — завдання з `07_praktyka.md`.

## Цільові версії

- MAVSDK main (v3.15+ / v4.x) — заголовки `*.hpp`, один CMake target `MAVSDK::mavsdk`.
  Код також збирається зі старою MAVSDK (`*.h`) завдяки `__has_include`.
- ArduPilot Copter 4.5 / master, SITL.
- Ubuntu 22.04 / 24.04 / 26.04 (WSL2), g++ 11–15, CMake ≥ 3.16.
