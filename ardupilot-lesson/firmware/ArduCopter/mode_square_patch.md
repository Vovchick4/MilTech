# Задача 12 — куди вставити ModeSquare (4 місця)

Шляхи — **WSL Ubuntu**, `~/ardupilot/ArduCopter/`.

## 1. `mode.h` — номер режиму

У `enum class Number`, після `TURTLE = 28,`:

```cpp
        SQUARE =       29,  // навчальний режим: облітати квадрат
```

## 2. `mode.h` — клас (вставити ПІСЛЯ класу `ModeGuidedNoGPS`)

```cpp
#if MODE_SQUARE_ENABLED
class ModeSquare : public ModeGuided {

public:
    using ModeGuided::Mode;
    Number mode_number() const override { return Number::SQUARE; }

    bool init(bool ignore_checks) override;
    void run() override;

    bool allows_arming(AP_Arming::Method method) const override { return false; }

protected:
    const char *name() const override { return "SQUARE"; }
    const char *name4() const override { return "SQRE"; }

private:
    bool go_to_corner(uint8_t corner);
    float get_side_m() const;

    static constexpr float ARRIVE_RADIUS_M = 5.0f;

    Location _origin;
    Location _target;
    uint8_t  _index;
    bool     _finished;
};
#endif
```

## 3. `Copter.h` — friend і екземпляр режиму

У список `friend class ...` (поруч з `friend class ModeZigZag;`) — інакше режим не матиме
доступу до приватних полів `copter` (`g2`, `ap`):

```cpp
    friend class ModeSquare;
```

Поруч із `ModeZigZag mode_zigzag;` (шукати `MODE_ZIGZAG_ENABLED`):

```cpp
#if MODE_SQUARE_ENABLED
    ModeSquare mode_square;
#endif
```

## 4. `mode.cpp` — дві вставки

У `Copter::mode_from_mode_num()` поруч з `case Mode::Number::ZIGZAG:`:

```cpp
#if MODE_SQUARE_ENABLED
        case Mode::Number::SQUARE:
            return &mode_square;
#endif
```

У `Copter::get_available_mode_enabled_mask()` у масив `modes[]` (якщо функція є у вашій версії):

```cpp
#if MODE_SQUARE_ENABLED
        &copter.mode_square,
#endif
```

## 5. Збірка

```bash
cd ~/ardupilot
./waf copter
```

`mode_square.cpp` підхоплюється автоматично (waf збирає всі `.cpp` у `ArduCopter/`).

> Номер 29 вільний у Copter 4.5 / master. Якщо у вашій версії зайнятий — візьміть інший
> (30 зарезервований під «offboard»).
