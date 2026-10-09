# Урок 4. Перша програма на C++

## Що зробимо

- Зберемо і запустимо програму, яка підключається до дрона.
- Розберемо кожен рядок.
- Навчимося читати «шум» у виводі MAVSDK.

## Підготовка: три вікна

```
вікно 1 (Ubuntu):  ./scripts/run_sitl.sh          <- симулятор
вікно 2:           Mission Planner на 5760         <- дивимось на дрон
вікно 3 (Ubuntu):  ./scripts/run.sh lesson04_connect  <- наша програма
```

> Щоб відкрити ще одне вікно Ubuntu на Windows — просто запусти «Ubuntu» з меню Пуск ще раз.
> Не забудь `cd ~/ardupilot-novachky/code`.

## Запуск

**Ubuntu (вікно 3):**

```bash
./scripts/run.sh lesson04_connect
```

`run.sh` робить дві речі: **збирає** програму (компілює `lesson04_connect.cpp` разом із бібліотекою
MAVSDK) і **запускає** її. Перша збірка займає ~30 секунд, наступні — швидше.

**Що маєш побачити:**

```
[Info ] MAVSDK version: v4.0.3 (mavsdk_impl.cpp:58)
З'єднання відкрито, шукаємо автопілот...
[Debug] Component Autopilot (component ID: 1) added. (system_impl.cpp:518)
[Debug] Discovered component Autopilot (system ID: 1, component ID: 1) (system_impl.cpp:640)
Підключено! Номер системи (system id): 1
```

Рядки з `[Info ]`, `[Debug]`, `[Warn ]` — це повідомлення самої бібліотеки MAVSDK (кольорові).
Наші рядки — українською. Щоб сховати повідомлення бібліотеки:

```bash
./build/lesson04_connect 2>/dev/null
```

## Розбір коду

Відкрий `code/lesson04_connect.cpp`.

```cpp
#include <mavsdk/mavsdk.hpp>   // головна частина бібліотеки MAVSDK
using namespace mavsdk;        // щоб писати Mavsdk замість mavsdk::Mavsdk
```

Підключаємо бібліотеку. `using namespace` — щоб не писати `mavsdk::` перед кожним словом.

```cpp
Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
```

Створюємо об'єкт `mavsdk` і кажемо: «ми — наземна станція», тобто пульт. (Без цієї
конфігурації програма не скомпілюється — MAVSDK обов'язково хоче знати, хто ми.)

```cpp
ConnectionResult result = mavsdk.add_any_connection("tcpout://127.0.0.1:5762");
```

Підключаємось до SITL:

| Частина | Значення |
|---|---|
| `tcpout://` | ми підключаємось **до** когось (SITL — сервер, ми — клієнт) |
| `127.0.0.1` | цей самий комп'ютер |
| `5762` | друга «розетка» SITL (перша, 5760, у Mission Planner) |

```cpp
auto system = mavsdk.first_autopilot(10.0);
if (!system) { ... }
```

Чекаємо до 10 секунд, поки автопілот «представиться». `auto` — компілятор сам визначить тип
(він довгий). `!system` — «не знайшли».

```cpp
system.value()->get_system_id()
```

`system.value()` — сам знайдений дрон. `get_system_id()` — його номер (у SITL це 1).
Номер потрібен, коли дронів кілька.

## Що може піти не так

| Повідомлення | Причина | Що робити |
|---|---|---|
| `Connect error: Connection refused` | SITL не запущений | запусти `./scripts/run_sitl.sh` |
| `Автопілот не знайдено` | SITL ще чекає Mission Planner на 5760 (`Waiting for connection`) | підключи Mission Planner |
| `Could not find MAVSDK` під час збірки | MAVSDK не встановлено | урок 1, `bash scripts/check.sh` |
| `Connection using tcp:// is deprecated` | у старому коді `tcp://` | пиши `tcpout://` |

## Вправи

1. Зупини SITL (`Ctrl+C`) і запусти програму. Що вона пише? Скільки чекає?
2. Зміни порт на `5763` (третя «розетка» SITL). Чи працює? А `5764`?
3. Зміни `first_autopilot(10.0)` на `first_autopilot(2.0)`. Що зміниться, якщо SITL вимкнено?
4. Додай наприкінці рядок, який друкує `"Програма завершилась без помилок"`.

## Коротко

- `./scripts/run.sh lesson04_connect` — зібрати і запустити.
- Три кроки підключення: створити `Mavsdk` → `add_any_connection("tcpout://127.0.0.1:5762")` → `first_autopilot`.
- `[Info]/[Debug]` — це MAVSDK. Сховати: `2>/dev/null`.
