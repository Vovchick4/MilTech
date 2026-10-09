# Словник

| Термін | English | Що означає |
|---|---|---|
| автопілот | autopilot | програма, що керує дроном |
| польотний контролер | flight controller | маленька плата-«мозок» на дроні, на ній працює автопілот |
| ArduPilot | ArduPilot | безкоштовний автопілот з відкритим кодом |
| прошивка | firmware | програма, записана в польотний контролер |
| SITL | Software In The Loop | симулятор: справжній ArduPilot + віртуальний дрон |
| наземна станція | GCS, Ground Control Station | програма-пульт: Mission Planner, QGroundControl |
| MAVLink | MAVLink | «мова» повідомлень між дроном і наземною станцією |
| MAVSDK | MAVSDK | бібліотека для C++, що «перекладає» прості команди в MAVLink |
| плагін | plugin | частина MAVSDK: Telemetry (дані), Action (команди) |
| порт | port | номер «розетки» для мережевого підключення: 5760, 5762 |
| телеметрія | telemetry | дані від дрона: висота, координати, заряд |
| ARM / DISARM | arm / disarm | увімкнути / вимкнути мотори |
| режим польоту | flight mode | хто і як керує дроном: STABILIZE, GUIDED, LAND, RTL |
| GUIDED | guided | дрон летить туди, куди кажуть ззовні (у MAVSDK — `Offboard`) |
| RTL | Return To Launch | повернутися додому і сісти |
| дім | home | точка, з якої дрон злетів |
| точка маршруту | waypoint | одна точка, через яку летить дрон |
| широта | latitude | координата північ–південь |
| довгота | longitude | координата схід–захід |
| AMSL | Above Mean Sea Level | висота над рівнем моря |
| висота над домом | relative altitude | висота над точкою зльоту |
| параметр | parameter | налаштування автопілота (WPNAV_SPEED, RTL_ALT...) |
| EKF | Extended Kalman Filter | частина автопілота, що з датчиків обчислює, де дрон |
| PreArm | pre-arm check | перевірки перед ARM; причина відмови пишеться в Messages |
| Lua-скрипт | Lua script | маленька програма, що працює всередині автопілота |
| `nan` | not a number | «не число»: даних ще немає |
