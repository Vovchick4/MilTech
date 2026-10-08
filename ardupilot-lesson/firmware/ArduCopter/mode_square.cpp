// Новий файл: ~/ardupilot/ArduCopter/mode_square.cpp   (задача 12)
//
// Режим SQUARE (номер 29): дрон облітає квадрат зі стороною USR_SQUARE_M
// на поточній висоті, повертається в точку входу й висить (як GUIDED).
//
// Ідея: успадковуємось від ModeGuided (так само зроблено ModeGuidedNoGPS),
// тобто вся навігація/контролери — готові. Ми лише по черзі даємо точки.
#include "Copter.h"

#if MODE_SQUARE_ENABLED

bool ModeSquare::init(bool ignore_checks)
{
    // Входимо тільки в повітрі: зліт робимо в GUIDED, потім перемикаємось.
    if (!motors->armed() || copter.ap.land_complete) {
        gcs().send_text(MAV_SEVERITY_WARNING, "SQUARE: take off first");
        return false;
    }
    if (!ModeGuided::init(ignore_checks)) {
        return false;
    }

    Location here;
    if (!AP::ahrs().get_location(here)) {
        return false;
    }
    _origin = here;
    _index = 0;
    _finished = false;

    gcs().send_text(MAV_SEVERITY_INFO, "SQUARE: side %.0f m", get_side_m());
    return go_to_corner(1);
}

void ModeSquare::run()
{
    // Перевіряємо, чи долетіли до поточного кута, і даємо наступний.
    // Рахуємо відстань самі: ModeGuided може вести дрон і без wp_nav
    // (залежить від GUID_OPTIONS), тому wp_nav->reached_wp_destination() ненадійний.
    Location here;
    if (!_finished && AP::ahrs().get_location(here) &&
        here.get_distance(_target) < ARRIVE_RADIUS_M) {
        if (_index >= 4) {
            _finished = true;
            gcs().send_text(MAV_SEVERITY_INFO, "SQUARE: done");
        } else {
            go_to_corner(_index + 1);
        }
    }

    // Уся «важка» робота — у батьківському класі.
    ModeGuided::run();
}

float ModeSquare::get_side_m() const
{
#if USER_PARAMS_ENABLED
    return copter.g2.user_parameters.square_m();
#else
    return 200.0f;
#endif
}

// Кути: 1 = північ, 2 = північ+схід, 3 = схід, 4 = назад у точку входу.
bool ModeSquare::go_to_corner(uint8_t corner)
{
    const float s = get_side_m();
    static const int8_t ne[5][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}};

    Location dest = _origin;                     // висота = висота входу в режим
    dest.offset(ne[corner][0] * s, ne[corner][1] * s);

    if (!set_destination(dest)) {
        gcs().send_text(MAV_SEVERITY_WARNING, "SQUARE: bad corner %u", corner);
        return false;
    }
    _index = corner;
    _target = dest;
    gcs().send_text(MAV_SEVERITY_INFO, "SQUARE: corner %u", corner);
    return true;
}

#endif // MODE_SQUARE_ENABLED
