# 9. Задачі по прошивці ArduPilot

Задачі йдуть драбиною: **читати код → змінювати параметри → писати свій код у прошивку →
зв'язати прошивку з C++ програмою з уроку**. Усе виконується в SITL, реальний дрон не потрібен.

Заготовки коду — у папці `firmware/` цієї пачки:

| Файл | Задачі |
|---|---|
| `firmware/ArduCopter/APM_Config_additions.h` | 8–12: які `#define` увімкнути |
| `firmware/ArduCopter/UserCode.cpp` | 8, 10, 11, 15: повідомлення, лог, failsafe, HOME_DIST |
| `firmware/ArduCopter/UserParameters.h/.cpp` | 9: параметри `USR_ENABLE`, `USR_MAX_DIST`, `USR_SQUARE_M` |
| `firmware/ArduCopter/mode_square.cpp` + `mode_square_patch.md` | 12: новий режим SQUARE |
| `firmware/lua/distance_guard.lua` | 6: Lua-скрипт |
| `firmware/tests/test_square_geo.cpp` | 13: модульний тест (gtest) |
| `firmware/autotest/square_test.py` | 14: автотест |
| `code/ardupilot-cpp/examples/03_firmware_link.cpp` | 15–16: C++ ↔ прошивка |

> Перед змінами в прошивці зроби гілку, щоб завжди можна було повернутись — **WSL Ubuntu:**
> ```bash
> cd ~/ardupilot
> git checkout -b student-tasks
> ```
> Повернути все як було: `git checkout master` (або `git stash`).

Цикл роботи з прошивкою:

```bash
cd ~/ardupilot
./waf copter                         # перезбірка — лише змінені файли, 10–60 с
~/ardupilot-cpp/scripts/run_sitl.sh  # запуск нової прошивки
```

---

## Рівень 0 — читання коду

### Задача 1. Шлях команди від C++ до моторів

Наша C++ програма викликає `action.goto_location()`. Знайди, що відбувається далі.

**Підказки — WSL Ubuntu:**
```bash
cd ~/ardupilot
grep -rn "MAV_CMD_DO_REPOSITION" ArduCopter/ libraries/GCS_MAVLink/ | head
grep -n "set_destination" ArduCopter/mode_guided.cpp | head
```

**Результат:** схема `MAVSDK → COMMAND_INT → GCS_MAVLINK_Copter::handle_command_int_do_reposition()
→ ModeGuided::set_destination() → AC_WPNav → AC_PosControl → AC_AttitudeControl → AP_Motors`
з назвами файлів.

### Задача 2. Головний цикл

Відкрий `ArduCopter/Copter.cpp`, знайди таблицю `scheduler_tasks[]`.

**Питання:** з якою частотою виконуються `update_flight_mode`, `update_batt_compass`,
`userhook_SuperSlowLoop`? Що буде, якщо задача не вкладеться у свій час? (параметр `SCHED_LOOP_RATE`,
повідомлення `PERF` у лозі).

### Задача 3. Опис плати

Відкрий `libraries/AP_HAL_ChibiOS/hwdef/MatekH743/hwdef.dat` (або плату, яка є в групі).

**Результат:** таблиця — який UART для GPS / телеметрії, скільки виходів для моторів, які
IMU/барометр, як плата отримує напругу батареї.

---

## Рівень 1 — параметри і Lua (без перезбирання)

### Задача 4. Вітер і відмови датчиків

У Mission Planner → Config → Full Parameter List:

| Параметр | Значення | Що спостерігати |
|---|---|---|
| `SIM_WIND_SPD` | 10 | Як змінюється час маршруту 10 км з уроку |
| `SIM_WIND_DIR` | 90 | Нахил дрона, відхилення від прямої |
| `SIM_GPS*` (вимкнення GPS — назва залежить від версії: `SIM_GPS_DISABLE` або `SIM_GPS1_ENABLE`) | вимкнути в польоті | Повідомлення EKF, failsafe, що робить дрон |

**Результат:** короткий звіт + скріншоти, яка реакція на кожну відмову.

### Задача 5. PID і AUTOTUNE

1. Запам'ятай `ATC_RAT_RLL_P`, `ATC_RAT_PIT_P`.
2. Збільш `ATC_RAT_RLL_P` у 3 рази → злітай у LOITER → подивись коливання (графік `ATT.Roll` / `ATT.DesRoll` у лозі).
3. Поверни значення. Злітай, режим **AUTOTUNE** (номер 15), дочекайся завершення.

**Результат:** графіки «до/після», пояснення що робить P, I, D.

### Задача 6. Lua-скрипт «віртуальна прив'язка»

Файл: `firmware/lua/distance_guard.lua`.

**WSL Ubuntu:**
```bash
mkdir -p ~/ardupilot/scripts
cp firmware/lua/distance_guard.lua ~/ardupilot/scripts/
```

У MP: `SCR_ENABLE = 1` → перезапустити SITL. У Messages має з'явитись `LUA distance_guard loaded`.
Запусти маршрут 10 км з `MAX_DIST_M = 3000` — дрон має піти в RTL, не закінчивши квадрат.

**Розширення:** додай свої параметри скрипту через `param:add_table()` / `param:add_param()`
замість константи `MAX_DIST_M`.

### Задача 7. Геозона

`FENCE_ENABLE=1`, `FENCE_TYPE=1` (коло), `FENCE_RADIUS=2000`, `FENCE_ACTION=1` (RTL).
Запусти маршрут 50 км. **Питання:** чому `goto_location` в точку за межею зони відхиляється
одразу (підказка: `ModeGuided::set_destination` → `check_location_within_fence`)?

---

## Рівень 2 — перші зміни в прошивці

Спочатку: вміст `firmware/ArduCopter/APM_Config_additions.h` додати в кінець
`~/ardupilot/ArduCopter/APM_Config.h`.

### Задача 8. UserCode: «я живий»

Скопіюй `firmware/ArduCopter/UserCode.cpp` у `~/ardupilot/ArduCopter/`. Перезбери, запусти.

**Результат:** у MP → Messages кожні 10 с `UserCode alive: N s`.

**Питання:** чому не можна робити `sleep()` або довгі цикли всередині `userhook_*`?

### Задача 9. Свої параметри

Скопіюй `UserParameters.h` і `UserParameters.cpp`.

> ⚠️ **Справжній баг ArduPilot (master, перевірено при збиранні).** Якщо просто увімкнути
> `USER_PARAMS_ENABLED`, прошивка не збереться. Потрібні два виправлення:
>
> 1. `ArduCopter/Copter.h` — помилка `'UserParameters' does not name a type`.
>    Перенеси `#include "UserParameters.h"` (разом з `#if USER_PARAMS_ENABLED ... #endif`)
>    **вище** рядка `#include "Parameters.h"`.
> 2. `ArduCopter/Parameters.cpp` — помилка `will be initialized after [-Werror=reorder]`.
>    У конструкторі `ParametersG2::ParametersG2()` видали три рядки:
>    ```cpp
>    #if USER_PARAMS_ENABLED
>        ,user_parameters()
>    #endif
>    ```
>
> Це гарна вправа: прочитати помилку компілятора, знайти причину, виправити. Розширення —
> оформити виправлення як pull request в ArduPilot.

**Результат:** у Full Parameter List є `USR_ENABLE`, `USR_MAX_DIST`, `USR_SQUARE_M`;
значення зберігаються після перезапуску SITL.

**Розширення:** додай `USR_ACTION` (0 = тільки попередження, 1 = RTL, 2 = LAND) і використай у задачі 11.

### Задача 10. Свій запис у лог

Код уже в `UserCode.cpp` (`AP::logger().Write("USR1", ...)`).

**Перевірка:** політай, потім MP → DataFlash Logs → Download → Review a Log → знайди `USR1`,
побудуй графік `Dist`. Лог SITL лежить у `~/ardupilot/logs/`.

### Задача 11. Свій failsafe

`USR_MAX_DIST = 1500` → запусти маршрут 10 км з уроку → дрон іде в RTL з повідомленням
`USR: ...m > 1500m -> RTL`.

**Обговорення:** задача 6 (Lua) і задача 11 (C++) роблять одне й те саме. Порівняйте:
швидкість розробки, ризик зламати прошивку, продуктивність, можливість змінити без перепрошивки,
сертифікація/рев'ю коду.

---

## Рівень 3 — повноцінні фічі

### Задача 12. Новий режим польоту SQUARE

Файли: `firmware/ArduCopter/mode_square.cpp`, інструкція `mode_square_patch.md` (4 місця правок).

**Ідея:** `ModeSquare` успадковується від `ModeGuided` — так само, як у самому ArduPilot зроблено
`ModeGuidedNoGPS`. Навігація готова, ми лише по черзі даємо кути квадрата через `set_destination()`
і перевіряємо відстань до поточного кута (`Location::get_distance()`).

**Перевірка:** злітай у GUIDED на 30 м, увімкни режим 29 (з C++ — задача 16, або
`Tools/autotest/...` / MAVProxy `mode 29`). У Messages: `SQUARE: corner 1 … 4`, `SQUARE: done`.

> Щоб пройти квадрат ще раз — перемкни в GUIDED і знову в SQUARE (повторне ввімкнення того самого
> режиму ArduPilot ігнорує).
>
> Mission Planner може показувати назву режиму як номер — він не знає про наш новий режим. Це нормально.

**Розширення:** параметр `USR_SQUARE_N` — кількість кіл; поворот носом за напрямком руху
(`set_destination(dest, true, yaw_rad)`); фігура «вісімка».

### Задача 13. Модульний тест (gtest)

`firmware/tests/test_square_geo.cpp` → `~/ardupilot/libraries/AP_Common/tests/`.

**WSL Ubuntu:**
```bash
cd ~/ardupilot
./waf configure --board sitl
./waf tests
./build/sitl/tests/test_square_geo
```

**Завдання:** допиши TODO-тест (1 км на схід у Києві vs на екваторі) і тест на «нульовий» квадрат.

### Задача 14. Автотест

`firmware/autotest/square_test.py` — метод для `Tools/autotest/arducopter.py`.

```bash
cd ~/ardupilot
Tools/autotest/autotest.py build.Copter test.Copter.SquareMode
```

Автотест сам запускає SITL, злітає, вмикає режим і перевіряє відстані. Так ArduPilot перевіряє
кожен pull request.

---

## Рівень 4 — прошивка + C++ програма

### Задача 15. Дані з прошивки в C++

`UserCode.cpp` шле `NAMED_VALUE_FLOAT "HOME_DIST"`. `examples/03_firmware_link.cpp` ловить його
через плагін **MavlinkDirect** (у старій MAVSDK — `MavlinkPassthrough::subscribe_message`).

**Завдання:** додай у прошивку ще одне значення (наприклад, кут нахилу `AP::ahrs().get_roll()`),
виведи його в C++ і побудуй графік з CSV-файлу, який запише твоя програма.

### Задача 16. Керування своїм режимом з C++

`03_firmware_link.cpp`: ставить `USR_SQUARE_M` через плагін **Param**, злітає, вмикає режим 29
командою `MAV_CMD_DO_SET_MODE` через MavlinkDirect.

**WSL Ubuntu:**
```bash
cd ~/ardupilot-cpp
./scripts/build_run.sh 03_firmware_link
```

**Завдання:** замість `sleep` чекай повідомлення `SQUARE: done` (підписка на `STATUSTEXT`
через MavlinkDirect), потім LAND.

### Задача 17. Збірка під реальну плату

```bash
cd ~/ardupilot
./waf list_boards | tr ' ' '\n' | grep -i -E "matek|cube|pixhawk" | head
./waf configure --board MatekH743
./waf copter
ls build/MatekH743/bin/
```

**Питання:** що таке `.apj` і `.hex`? Чим HAL ChibiOS відрізняється від HAL SITL? Чи потрапить
наш режим SQUARE і UserCode у прошивку для плати (так — вони в `ArduCopter/`, не в SITL)?

> Прошивати реальну плату — тільки з викладачем і **без пропелерів**.

---

## Що перевірено перед видачею

Заготовки зібрано й запущено на ArduPilot master (жовтень 2026), SITL quad:

| Що | Результат |
|---|---|
| `./waf copter` з UserCode + UserParameters + ModeSquare (з виправленнями із задачі 9) | ✅ збирається без попереджень |
| `UserCode alive`, параметри `USR_*`, `HOME_DIST` | ✅ працює |
| Режим SQUARE, сторона 150 м | ✅ `corner 1…4`, `SQUARE: done` |
| Failsafe `USR_MAX_DIST` | ✅ `USR: 300m > 200m -> RTL` |
| Lua `distance_guard.lua` | ✅ `LUA: 106m > 100m -> RTL` |
| gtest `test_square_geo` | ✅ 2/2 PASSED |
| `03_firmware_link.cpp` | компілюється з MAVSDK v4 (запуск — на стороні студента) |
| Автотест `square_test.py` | каркас, не запускався |

> `NAMED_VALUE_FLOAT` і `STATUSTEXT` ArduPilot шле лише в «активні» канали — ті, звідки
> приходили HEARTBEAT від GCS. MAVSDK шле їх автоматично, тож у C++ нічого робити не треба.

## Підсумковий проєкт: «Патрулювання периметра»

| Частина | Що зробити |
|---|---|
| Прошивка | Режим PATROL: N кіл по квадрату/колу, параметри розміру, швидкості і кількості кіл, запис у лог, свій failsafe |
| C++ / MAVSDK | Налаштовує параметри, злітає, вмикає режим, логує телеметрію в CSV, після посадки друкує звіт (час, дистанція, макс. віддалення) |
| Перевірка | gtest для геометрії + автотест режиму + демонстрація в Mission Planner з вітром `SIM_WIND_SPD=8` |
| Захист | Студент пояснює шлях команди від C++ до моторів |

## Критерії оцінювання

| Критерій | Бали |
|---|---|
| Прошивка збирається без нових попереджень (`./waf copter`) | 15 |
| Поведінка в SITL відповідає завданню | 25 |
| Параметри видно в GCS, значення зберігаються | 10 |
| Свій запис у лозі, графік у MP | 10 |
| gtest / автотест проходять | 15 |
| C++ програма: перевірка всіх `Result`, таймаути, без «голих» `sleep` | 15 |
| Пояснення архітектури (захист) | 10 |
