# 4. Linux (нативно) + Docker як альтернатива

## 4.1 Linux нативно (рекомендовано)

На Ubuntu/Debian кроки **ідентичні WSL** — див. `02_windows_wsl.md`, розділи 2.2, 2.3, 2.5.
GCS на Linux — **QGroundControl** (AppImage):

**Linux terminal:**

```bash
sudo usermod -a -G dialout $USER
sudo apt install -y gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl libfuse2
# завантажити AppImage зі сторінки QGC (посилання нижче), потім:
chmod +x QGroundControl*.AppImage
./QGroundControl*.AppImage
```

Сторінка завантаження: <https://docs.qgroundcontrol.com/master/en/qgc-user-guide/getting_started/download_and_install.html>.
Після `usermod` треба вийти з сесії й зайти знову.

## 4.2 Docker — що це і навіщо

**Docker** запускає програму в **контейнері** — ізольованому Linux-середовищі з усіма
залежностями. Плюс: «працює однаково на будь-якому ПК», не треба ставити Python-пакети,
empy, toolchain. Мінус: ще один шар (порти, файлові системи, архітектура процесора).

Ключові поняття:

| Термін | Пояснення |
|---|---|
| **Image** (образ) | «Шаблон»: ОС + бібліотеки + наш зібраний SITL. Створюється `docker build` |
| **Container** | Запущений екземпляр образу. `docker run` |
| **Dockerfile** | Рецепт збирання образу |
| **`-p 5760:5760`** | Проброс порту: порт контейнера → порт хоста. Без цього GCS не побачить SITL |
| **`ardupilot/ardupilot-dev-base`** | Офіційний образ ArduPilot з **інструментами для збірки** — це НЕ готовий SITL, сам ArduPilot ще треба зібрати всередині |

### Схема з Docker

```
┌──────── Docker container ────────┐
│  arducopter (SITL)               │
│   5760, 5762, 5763               │
└──────┬──────────┬────────────────┘
       │ -p 5760  │ -p 5762
       ▼          ▼
  QGC / MP     C++ (MAVSDK) на хості: tcp://127.0.0.1:5762
```

GCS і C++ працюють **на хості**, як і раніше — змінюється тільки те, де живе SITL.

## 4.3 Запуск через Docker

У пачці: `docker/Dockerfile` і `docker/run.sh`.

**Linux / macOS terminal**, з кореня пачки `ardupilot-lesson/`:

```bash
docker build -t ardupilot-sitl -f docker/Dockerfile .
./docker/run.sh
```

Далі — Mission Planner/QGC на `127.0.0.1:5760`, C++ — без змін (`tcp://127.0.0.1:5762`).

Прискорена симуляція: `SPEEDUP=10 ./docker/run.sh`.

## 4.4 ⚠️ Відомі проблеми (у нас Docker-варіант не спрацював)

Тому Docker залишаємо як **альтернативу**, основний шлях — нативний / WSL.

| Проблема | Причина | Що спробувати |
|---|---|---|
| `failed to extract layer ... input/output error` (Docker Desktop, Mac) | Пошкоджений або переповнений диск Docker VM | Docker Desktop → Troubleshoot → **Clean / Purge data**; Settings → Resources → збільшити **Disk image size**; перезапустити Docker |
| Образ не тягнеться / дуже повільно на Apple Silicon | Образ під `amd64`, емуляція через Rosetta/QEMU | `docker build --platform linux/amd64 ...` (повільно) або збирати нативно на Mac |
| GCS не підключається до 5760 | Порт не проброшений або SITL слухає тільки в контейнері | Перевірити `docker ps` → колонка PORTS має `0.0.0.0:5760->5760/tcp` |
| `empy` / `pexpect` помилки під час `./waf` | Python-залежності образу змінились | Dockerfile уже ставить `empy==3.3.4`; за потреби оновити `AP_BRANCH` |
| Docker на Windows + WSL | Docker Desktop і WSL — додатковий шар | На Windows Docker не потрібен — SITL і так у WSL |

**Висновок для студента:** Docker корисний на CI/серверах і для «чистого» відтворюваного
середовища, але для навчання з GUI-станцією нативний SITL (Linux/macOS) або WSL (Windows)
простіший і надійніший.
