# 5. Наземна станція: Mission Planner і QGroundControl

Обидві GCS вміють те саме для нашого уроку: карта, телеметрія, ARM, режими, параметри, місії.

| | Mission Planner | QGroundControl |
|---|---|---|
| ОС | Windows | Windows / macOS / Linux / Android |
| Автопілоти | ArduPilot (рідна) | PX4 та ArduPilot |
| Плюси | Усі параметри ArduPilot, логи, MAVLink Inspector, симуляція | Простий інтерфейс, кросплатформенний |

## 5.1 Підключення Mission Planner до SITL

1. SITL запущений і пише `Waiting for connection`.
2. Праворуч угорі: у випадаючому списку порту обрати **TCP**.
3. Натиснути **Connect** → Host: `127.0.0.1` → Port: `5760`.
4. У терміналі SITL з'явиться `Connection on serial port 5760`, у MP — завантаження параметрів.

Що має бути на екрані **DATA**:

- `DISARMED`, режим `Stabilize`;
- статус GPS `3D Fix`, EKF зелений;
- дрон над Канберрою (-35.3632, 149.1652) — дефолтний home SITL;
- через ~30–60 с — `Ready to Arm` (поки EKF не зійдеться, ARM відхилятиметься).

## 5.2 Підключення QGroundControl

QGC за замовчуванням слухає UDP 14550 — для TCP треба додати з'єднання:

1. Q (логотип) → **Application Settings** → **Comm Links** → **Add**.
2. Name: `SITL TCP`, Type: **TCP**, Server Address: `127.0.0.1`, Port: `5760`.
3. **OK** → обрати `SITL TCP` → **Connect**.

> Не тримайте Mission Planner і QGC на одному порті 5760 одночасно.
> Хочете обидві — QGC на `5763`.

## 5.3 Вправа: ручний політ з GCS (до C++!)

Мета — побачити, які кроки потім автоматизуватиме C++.

**Mission Planner:**

1. DATA → вкладка **Actions** → режим `Guided` → **Set Mode**.
2. **Arm/Disarm** → дрон ARMED.
3. ПКМ на карті біля дрона → **Takeoff** → `20` м.
4. ПКМ у точку на карті → **Fly To Here** → дрон летить (це і є `goto_location`).
5. Actions → `LAND` → **Set Mode**.

**QGC:** кнопка Takeoff зліва → повзунок; клік на карті → **Go to location**; Land.

## 5.4 Що дивитися, коли летить C++ програма

- Карта: фіолетова лінія траєкторії (MP малює слід дрона).
- `Mode` змінюється: `Stabilize → Guided → Land`.
- **Messages** (вкладка внизу в MP): тут ArduPilot пише `PreArm: ...` — причину, чому ARM відхилено.
- `Ctrl+F` → **MAVLink Inspector** — видно `COMMAND_LONG` від нашої програми (sysid 245 = MAVSDK GCS).

## 5.5 Корисні параметри SITL (Config → Full Parameter List)

| Параметр | Значення | Навіщо |
|---|---|---|
| `WPNAV_SPEED` | 1500 (см/с) | Дефолтна швидкість GUIDED/AUTO (C++ і так задає `set_current_speed`) |
| `RTL_ALT` | 3000 (см) | Висота повернення |
| `FENCE_ENABLE` | 0 | Якщо геозона заважає дальнім маршрутам |
| `BATT_FS_LOW_ACT` | 0 | Вимкнути failsafe батареї для 100 км маршруту (лише в SITL!) |
| `FS_GCS_ENABLE` | 0 | Не робити RTL при втраті зв'язку з GCS під час тестів |

> Після `WIPE=1 ./run_sitl.sh` усі параметри повертаються до дефолтних.
