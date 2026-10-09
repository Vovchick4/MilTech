# Урок 1. Встановлення

> Це найнудніший урок. Далі — цікавіше, обіцяємо.
> Якщо викладач уже все встановив на твоєму комп'ютері — переходь одразу до кроку «Перевірка».

## Що треба встановити

| Що | Навіщо | Де працює |
|---|---|---|
| WSL + Ubuntu (тільки Windows) | Linux усередині Windows — ArduPilot збирається під Linux | Windows |
| ArduPilot | симулятор дрона | Ubuntu / macOS |
| MAVSDK | бібліотека для C++ | Ubuntu / macOS |
| Mission Planner **або** QGroundControl | наземна станція (карта, кнопки) | Windows / macOS / Linux |
| VS Code (за бажанням) | редактор коду | будь-де |

## Де виконувати команди

У цьому модулі перед кожною командою написано, **де** її виконувати:

| Напис | Що відкрити |
|---|---|
| **PowerShell (адмін)** | Пуск → набрати «PowerShell» → правою кнопкою → «Запуск від імені адміністратора» |
| **Ubuntu** | Пуск → «Ubuntu» (на Windows) або звичайний Термінал (на Linux) |
| **Термінал macOS** | Програми → Утиліти → Термінал |

---

## Windows

### Крок 1. WSL і Ubuntu

**PowerShell (адмін):**

```powershell
wsl --install -d Ubuntu
```

Перезавантаж комп'ютер. Після перезавантаження відкриється вікно Ubuntu — придумай ім'я
користувача і пароль (пароль під час набору **не видно** — це нормально).

**Що маєш побачити:** рядок на кшталт `ivan@DESKTOP-ABC:~$` — Ubuntu готова.

### Крок 2. Завантажити матеріали модуля в Ubuntu

Архів з модулем лежить у Windows, наприклад у «Завантаженнях». **Ubuntu:**

```bash
cd ~
cp /mnt/c/Users/ТВОЄ_ІМ'Я_В_WINDOWS/Downloads/ardupilot-novachky.zip .
sudo apt install -y unzip
unzip ardupilot-novachky.zip
cd ardupilot-novachky/code
```

> `/mnt/c/` — це диск `C:` Windows, як його бачить Ubuntu.

### Крок 3. Встановити ArduPilot і MAVSDK одним скриптом

**Ubuntu**, у папці `~/ardupilot-novachky/code`:

```bash
bash scripts/install_ubuntu.sh
```

Скрипт попросить пароль (той, що ти придумав для Ubuntu) і працюватиме **30–60 хвилин**.
Він по черзі:

1. ставить базові інструменти (компілятор, git, cmake);
2. завантажує і збирає ArduPilot (симулятор);
3. завантажує і збирає MAVSDK (бібліотеку);
4. перевіряє, що все на місці.

**Що маєш побачити в кінці:**

```
[OK] arducopter
[OK] MAVSDK
Усе встановлено!
```

### Крок 4. Mission Planner

Завантаж і встанови у **Windows**: <https://firmware.ardupilot.org/Tools/MissionPlanner/>
→ файл `MissionPlanner-latest.msi`.

---

## macOS

На Mac усе працює без WSL. Mission Planner на Mac працює погано — бери **QGroundControl**.

**Термінал macOS:**

```bash
xcode-select --install
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake ninja git mavsdk

cd ~
git clone --recurse-submodules https://github.com/ArduPilot/ardupilot.git
cd ardupilot
Tools/environment_install/install-prereqs-mac.sh -y
source ~/.zshrc
./waf configure --board sitl
./waf copter
```

QGroundControl: <https://docs.qgroundcontrol.com/master/en/qgc-user-guide/getting_started/download_and_install.html>

---

## Linux (Ubuntu)

Так само, як Windows, починаючи з **кроку 2** (WSL не потрібен).
Наземна станція — QGroundControl (посилання вище).

---

## Перевірка

**Ubuntu / Термінал macOS**, у папці `code`:

```bash
bash scripts/check.sh
```

**Що маєш побачити:**

```
  [OK]    компілятор g++
  [OK]    cmake
  [OK]    симулятор ArduPilot (arducopter)
  [OK]    бібліотека MAVSDK
```

Якщо десь `[НЕМАЄ]` — поруч написано, що робити. Не допомогло — [problemy.md](problemy.md) або викладач.

## Коротко

- Windows: WSL → Ubuntu → `install_ubuntu.sh` → Mission Planner.
- macOS: brew + ArduPilot → QGroundControl.
- Перевірка: `bash scripts/check.sh` — усе має бути `[OK]`.
