# 6. C++ і MAVSDK: від телеметрії до 100 км маршруту

## 6.1 Структура проєкту

```
~/ardupilot-cpp/
├── CMakeLists.txt
├── main.cpp                    # великий квадратний маршрут
├── examples/
│   ├── 01_telemetry.cpp        # тільки читаємо дані
│   └── 02_takeoff_land.cpp     # зліт 20 м → посадка
├── scripts/
│   ├── run_sitl.sh             # запуск SITL з правильними параметрами
│   ├── build_run.sh            # збірка + запуск однією командою
│   ├── check_env.sh            # діагностика середовища
│   └── install_mavsdk.sh       # збірка MAVSDK з вихідників
└── .vscode/                    # Build (Ctrl+Shift+B), Run Task, Debug (F5)
```

## 6.2 CMake і MAVSDK

```cmake
find_package(MAVSDK REQUIRED)
target_link_libraries(ardupilot_cpp PRIVATE MAVSDK::mavsdk)
```

Нова MAVSDK експортує **один** target `MAVSDK::mavsdk` — усі плагіни (telemetry, action,
mission…) всередині. Старі `MAVSDK::mavsdk_action`, `MAVSDK::mavsdk_telemetry` більше не існують.

### Заголовки: `.h` чи `.hpp`

У новій MAVSDK (v3.15+/v4, main) **усі заголовки `.hpp`**. Помилка
`mavsdk/plugins/action/action.h: No such file or directory` означає саме це: файл
називається `action.hpp`. Перевірка — **WSL Ubuntu:**

```bash
ls /usr/local/include/mavsdk/plugins/action/
```

Код у пачці підтримує обидві версії:

```cpp
#if __has_include(<mavsdk/plugins/action/action.hpp>)
#include <mavsdk/plugins/action/action.hpp>
#else
#include <mavsdk/plugins/action/action.h>
#endif
```

## 6.3 Збірка й запуск

**WSL Ubuntu / Linux / macOS terminal:**

```bash
cd ~/ardupilot-cpp
cmake -S . -B build          # один раз (або після зміни CMakeLists.txt)
cmake --build build -j       # після кожної зміни .cpp
./build/ardupilot_cpp
```

Або все одразу: `./scripts/build_run.sh` (`./scripts/build_run.sh 01_telemetry` — приклад).

`cmake --build` сам перевіряє, що змінилося — якщо нічого, він нічого не збирає. Тому
`build_run.sh` можна запускати завжди.

## 6.4 Крок 1 — підключення і телеметрія (`examples/01_telemetry.cpp`)

```cpp
Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
mavsdk.add_any_connection("tcp://127.0.0.1:5762");
auto system = mavsdk.first_autopilot(10.0);   // std::optional<std::shared_ptr<System>>
Telemetry telemetry{system.value()};
```

- `Mavsdk mavsdk;` **не скомпілюється** — конструктор за замовчуванням видалено,
  треба сказати, ким ми є (`GroundStation`, sysid 245).
- `first_autopilot(10.0)` — чекає HEARTBEAT від автопілота до 10 с.
- Два стилі отримання даних:
  - **підписка** (callback у фоновому потоці): `telemetry.subscribe_position([](auto p){...})`;
  - **опитування** (останнє значення): `telemetry.position()`, `telemetry.armed()`.
- Частоту можна задати тільки для деяких потоків: `set_rate_position(2.0)` є,
  а `set_rate_armed()` / `set_rate_flight_mode()` **не існує**.

## 6.5 Крок 2 — ARM, TAKEOFF, LAND (`examples/02_takeoff_land.cpp`)

```cpp
Action action{system.value()};
action.set_takeoff_altitude(20.0f);
action.arm();
action.takeoff();      // одразу після arm — ArduPilot роззброюється, якщо не злетіти за ~10 с
// ... чекаємо висоту за телеметрією ...
action.land();
```

Кожен виклик повертає `Action::Result`. Його треба **завжди** перевіряти й друкувати
(`std::cout << result`) — там буде `CommandDenied`, `Timeout` тощо.

## 6.6 Крок 3 — великий маршрут (`main.cpp`)

### Налаштування у верхній частині файлу

```cpp
constexpr double ROUTE_KM        = 10.0;  // 10.0 / 50.0 / 100.0 — периметр квадрата
constexpr float  ALTITUDE_M      = 100.0f;
constexpr float  SPEED_M_S       = 15.0f;
constexpr double ACCEPT_RADIUS_M = 25.0;
```

Міняєш `ROUTE_KM` → `cmake --build build` → запуск. Більше нічого переписувати не треба.

### Геометрія

`ROUTE_KM` — повна довжина. Сторона квадрата = `ROUTE_KM / 4`.

```
   P1 ───────► P2           10 км  → сторона 2.5 км
   ▲            │           50 км  → сторона 12.5 км
   │            ▼          100 км  → сторона 25 км
 HOME ◄─────── P3
```

### Алгоритм `fly_to()`

1. `goto_location(lat, lon, abs_alt, NAN)` — висота **AMSL** (над рівнем моря), тому
   `abs_alt = home.absolute_altitude_m + ALTITUDE_M`. `NAN` для yaw = «не змінювати курс».
2. Цикл кожні 200 мс:
   - Ctrl+C? → вихід, у `main` буде RTL;
   - дрон DISARMED? → помилка;
   - відстань до цілі (гаверсинус) ≤ 25 м → точку досягнуто;
   - таймаут = 3 × (відстань / швидкість) + 2 хв.
3. Кожні 3 с друкує прогрес:

```
Current: lat=-35.3407421 lon=149.1652370 alt=100.2m | mode=Offboard armed=YES
Target:  P1 lat=-35.3407822 lon=149.1652370
Distance to target: 412 m  (83.5%, 00:02:15, ETA ~00:00:27)
```

### Скільки летіти

| ROUTE_KM | При 15 м/с | SITL `SPEEDUP=10` |
|---|---|---|
| 10 | ~11 хв | ~1 хв |
| 50 | ~56 хв | ~6 хв |
| 100 | ~1 год 51 хв | ~11 хв |

Прискорення — **WSL Ubuntu:** `SPEEDUP=10 ~/ardupilot-cpp/scripts/run_sitl.sh`.
Час у програмі (ETA, таймаути) рахується в реальних секундах, тому при прискоренні
дрон просто прилітає раніше — нічого міняти в коді не треба.

## 6.7 Порядок запуску (запам'ятати)

| # | Де | Що |
|---|---|---|
| 1 | WSL Ubuntu, термінал 1 | `~/ardupilot-cpp/scripts/run_sitl.sh` |
| 2 | Windows | Mission Planner → TCP `127.0.0.1:5760` → Connect, дочекатися `Ready to Arm` |
| 3 | WSL Ubuntu / VS Code terminal, термінал 2 | `cd ~/ardupilot-cpp && ./scripts/build_run.sh` |
| 4 | Mission Planner | Дивитися траєкторію на карті |

Зупинка: `Ctrl+C` у терміналі C++ → дрон іде в RTL. `Ctrl+C` у терміналі SITL → симулятор вимкнено.
