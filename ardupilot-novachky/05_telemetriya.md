# Урок 5. Телеметрія — що відбувається з дроном

## Що вивчимо

- Як читати висоту, координати, заряд, режим, ARMED.
- Чому спочатку приходить `nan` і як попросити дрон надсилати дані.
- Як зробити паузу в програмі.

## Запуск

SITL і Mission Planner вже працюють (урок 2). **Ubuntu:**

```bash
./scripts/run.sh lesson05_telemetry 2>/dev/null
```

**Що маєш побачити** (15 рядків, раз на секунду):

```
t=1с  висота=nan м  lat=nan  lon=nan  заряд=nan%  режим=Stabilized  ARMED=ні
t=2с  висота=-0.006 м  lat=-35.3633  lon=149.165  заряд=100%  режим=Stabilized  ARMED=ні
t=3с  висота=-0.006 м  lat=-35.3633  lon=149.165  заряд=100%  режим=Stabilized  ARMED=ні
...
```

**Поки програма працює**, зроби в Mission Planner ARM, потім DISARM, змінюй режим.
Подивись, як змінюється вивід.

## Розбір коду

### Плагін «телеметрія»

```cpp
#include <mavsdk/plugins/telemetry/telemetry.hpp>
...
Telemetry telemetry{system.value()};
```

MAVSDK складається з **плагінів** — частин для різних задач. `Telemetry` — для читання даних,
`Action` (урок 6) — для команд.

### Попросити дрон надсилати дані

```cpp
telemetry.set_rate_position(2.0);   // позиція 2 рази на секунду
telemetry.set_rate_battery(1.0);    // батарея раз на секунду
```

ArduPilot не надсилає все підряд — лише те, що попросили. Якщо прибрати ці рядки,
у SITL ти побачиш `nan` **весь час**. (Перевір — це вправа 1.)

### Що таке `nan`

`nan` = «not a number», «не число». MAVSDK повертає `nan`, поки **ще не отримав** даних від дрона.
Тому перший рядок — з `nan`, а з другої секунди дані вже є.

### Читання даних

```cpp
Telemetry::Position position = telemetry.position();
Telemetry::Battery battery = telemetry.battery();
```

`position` і `battery` — це `struct` з кількома полями:

| Поле | Що це | Одиниці |
|---|---|---|
| `position.latitude_deg` | широта | градуси |
| `position.longitude_deg` | довгота | градуси |
| `position.relative_altitude_m` | висота **над точкою зльоту** | метри |
| `position.absolute_altitude_m` | висота **над рівнем моря** | метри |
| `battery.remaining_percent` | заряд | % |
| `battery.voltage_v` | напруга | вольти |
| `telemetry.armed()` | чи увімкнені мотори | true / false |
| `telemetry.flight_mode()` | режим польоту | Stabilized, Offboard (=GUIDED), Land... |

### Пауза

```cpp
std::this_thread::sleep_for(std::chrono::seconds(1));
```

Програма «спить» 1 секунду. Потрібні `#include <thread>` і `#include <chrono>`.

### Тернарний оператор

```cpp
(telemetry.armed() ? "так" : "ні")
```

Коротке `if`: якщо `armed()` — `"так"`, інакше — `"ні"`.

## Чому висота -0.006 м?

Висота над точкою зльоту на землі — майже 0. Датчик (барометр) трохи «шумить» — як і в справжньому дроні.
Абсолютна висота в Канберрі — 584 м над рівнем моря.

## Вправи

1. Закоментуй рядки `set_rate_position` і `set_rate_battery` (`//` на початку). Що бачиш?
   Розкоментуй назад.
2. Додай у вивід напругу батареї (`battery.voltage_v`) і абсолютну висоту.
3. Нехай програма працює не 15, а 60 секунд, але друкує раз на 5 секунд.
4. Якщо дрон ARMED — друкуй додатково `"!!! МОТОРИ УВІМКНЕНО"`.
5. (★) Друкуй максимальну висоту за весь час роботи програми. Злети з Mission Planner на 30 м і
   перевір. (Згадай «найвищу людину в кімнаті» з модуля алгоритмів.)
6. (★) Пропускай рядки, де `lat` ще `nan`. Підказка: `#include <cmath>` і `std::isnan(x)`.

## Коротко

- `Telemetry telemetry{system.value()};` — плагін для читання даних.
- `set_rate_...` — попросити дрон надсилати дані, інакше буде `nan`.
- `telemetry.position()`, `.battery()`, `.armed()`, `.flight_mode()` — останні отримані значення.
