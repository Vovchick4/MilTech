# Якщо щось не працює

Знайди свій симптом. Якщо не допомогло — покажи викладачу **останні 20 рядків** з терміналу.

## Встановлення

| Симптом | Що робити |
|---|---|
| `wsl: command not found` / WSL не встановлюється | Windows має бути оновлений (Windows 10 2004+ або Windows 11). Запусти PowerShell **від адміністратора** |
| Пароль в Ubuntu «не набирається» | Він набирається, просто не показується. Набери і натисни Enter |
| `install_ubuntu.sh` пише `you need to install empy` | Скрипт сам ставить empy у venv. Якщо все одно падає — покажи викладачу |
| `check.sh`: `[НЕМАЄ] симулятор ArduPilot` | `cd ~/ardupilot && ./waf configure --board sitl && ./waf copter` |
| `check.sh`: `[НЕМАЄ] бібліотека MAVSDK` | крок 3 у `install_ubuntu.sh` не завершився — запусти скрипт ще раз |

## Симулятор і наземна станція

| Симптом | Що робити |
|---|---|
| `Не знайдено папку ArduPilot` під час `run_sitl.sh` | ArduPilot не в `~/ardupilot`. Вкажи шлях: `ARDUPILOT_DIR=/шлях ./scripts/run_sitl.sh` |
| Mission Planner не підключається до `127.0.0.1:5760` | 1) SITL запущений і пише `Waiting for connection`? 2) Обрано **TCP**, а не UDP/COM? 3) Спробуй замість `127.0.0.1` адресу з команди `hostname -I` в Ubuntu |
| Дрон не хоче ARM | Вкладка **Messages**, рядок `PreArm: ...` — там причина. Найчастіше: зачекай 30–60 с після старту |
| Усе «зламалось», дивні налаштування | `WIPE=1 ./scripts/run_sitl.sh` — скинути все до заводських |
| Батарея 20%, дрон одразу летить додому | Батарея в симуляторі розряджається. Перезапусти SITL |

## Збірка програми

| Симптом | Що робити |
|---|---|
| `Could not find a package configuration file provided by "MAVSDK"` | MAVSDK не встановлено — урок 1 |
| `mavsdk/plugins/action/action.h: No such file` | у новій MAVSDK файли закінчуються на `.hpp`: `action.hpp` |
| `use of deleted function Mavsdk::Mavsdk()` | пиши `Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};` |
| `error: ... was not declared in this scope` | забув `#include` або одруківка в імені |
| `error while loading shared libraries: libmavsdk.so` | `sudo ldconfig` |

## Запуск програми

| Симптом | Що робити |
|---|---|
| `Connect error: Connection refused` | SITL не запущений |
| `Автопілот не знайдено` | SITL чекає Mission Planner на 5760 — підключи його першим |
| `Connection using tcp:// is deprecated` | пиши `tcpout://127.0.0.1:5762` |
| Скрізь `nan` | немає `set_rate_position` / `set_rate_battery`, або минула лише перша секунда |
| `зліт поки не вдається (Failed)` кілька разів | автопілот ще «прогрівається» — програма сама повторить спробу |
| Програма «зависла» на `Чекаємо GPS...` | SITL не отримує даних GPS — перезапусти SITL |
| Програма «зависла» в польоті | `Ctrl+C`. Потім у Mission Planner режим **RTL**. Подивись, у якому циклі `while` вона чекала і чому умова ніколи не стала правдою |
| Дрон летить не туди | перевір метри → градуси: на північ — широта, на схід — довгота (і `cos`) |
| Дрон знижується до землі на `goto_location` | висота має бути **над морем**: `home.absolute_altitude_m + ALTITUDE_M` |

## Lua

| Симптом | Що робити |
|---|---|
| Скрипт нічого не пише | `SCR_ENABLE = 1`? Файл у `~/ardupilot/scripts/`? SITL перезапущено? |
| У Messages `Lua: ... error` | помилка в скрипті: номер рядка вказано в повідомленні |
| «Кракозябри» замість тексту | пиши в `gcs:send_text` англійською |
