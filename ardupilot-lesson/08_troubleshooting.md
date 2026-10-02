# 8. Типові помилки

Спершу завжди: **WSL Ubuntu / Linux / macOS terminal** → `~/ardupilot-cpp/scripts/check_env.sh`

| Симптом | Причина | Рішення |
|---|---|---|
| `Bind error: Address already in use` | У C++ `tcpin://...:5760` — програма намагається відкрити порт, зайнятий SITL | `tcp://127.0.0.1:5762` |
| `Autopilot не знайдено за 10 с` | SITL не запущений, або чекає першого клієнта на 5760, або 5762 уже зайнятий іншою програмою | Запустити SITL → підключити MP на 5760 → потім C++. Не запускати дві C++ програми одночасно |
| `mavsdk/plugins/action/action.h: No such file` | Нова MAVSDK має `.hpp` | `#include <mavsdk/plugins/action/action.hpp>` (у пачці вже через `__has_include`) |
| `use of deleted function Mavsdk::Mavsdk()` | Немає конфігурації | `Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};` |
| `'set_rate_armed' is not a member` | Такого методу немає | Просто `subscribe_armed(...)` |
| CMake: `MAVSDK::mavsdk_action not found` | Нова MAVSDK має один target | `target_link_libraries(... MAVSDK::mavsdk)` |
| CMake: `Could not find MAVSDK` | MAVSDK не встановлено або не в `/usr/local` | `scripts/install_mavsdk.sh`; на Mac `-DCMAKE_PREFIX_PATH=/opt/homebrew` |
| `error while loading shared libraries: libmavsdk.so` | Кеш лінкера не оновлено | `sudo ldconfig` |
| `ARM: CommandDenied` | PreArm перевірки (EKF ще не зійшовся, GPS) | Почекати 30–60 с; причину видно в MP → Messages (`PreArm: ...`) |
| Дрон злітає і одразу сідає/роззброюється | Між `arm()` і `takeoff()` пройшло > 10 с | Викликати `takeoff()` одразу після `arm()` |
| На довгому маршруті дрон сам іде в RTL | Failsafe батареї / GCS / геозона | У SITL: `BATT_FS_LOW_ACT=0`, `FS_GCS_ENABLE=0`, `FENCE_ENABLE=0` |
| `Таймаут до P1` | Дрон летить повільніше, ніж `SPEED_M_S`, або не в GUIDED | Перевірити режим у MP; збільшити `WPNAV_SPEED` |
| Mission Planner не бачить SITL з WSL | Не працює localhost-forwarding | У WSL `hostname -I` → ця IP у MP замість `127.0.0.1` |
| `./waf`: `you need to install empy` | PEP 668, Python externally managed | venv: див. `02_windows_wsl.md`, 2.2 |
| Docker: `failed to extract layer ... input/output error` | Проблеми диска Docker Desktop | Див. `04_linux_docker.md`, 4.4 |
| Дрон «стрибає» на карті при `SPEEDUP=10` | GCS не встигає за телеметрією | Нормально; або `SPEEDUP=5` |
