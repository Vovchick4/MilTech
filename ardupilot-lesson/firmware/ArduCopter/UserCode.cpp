// Замінити ~/ardupilot/ArduCopter/UserCode.cpp
// Потрібно в APM_Config.h: USERHOOK_SUPERSLOWLOOP і USER_PARAMS_ENABLED (див. APM_Config_additions.h)
#include "Copter.h"

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // ---- задача 8: повідомлення в GCS ----
    static uint32_t counter = 0;
    if (++counter % 10 == 0) {
        gcs().send_text(MAV_SEVERITY_INFO, "UserCode alive: %u s", (unsigned)counter);
    }

#if USER_PARAMS_ENABLED
    if (!g2.user_parameters.enabled()) {
        return;
    }

    Location loc;
    if (!AP::ahrs().get_location(loc) || !AP::ahrs().home_is_set()) {
        return;
    }
    const float dist_m = loc.get_distance(AP::ahrs().get_home());
    const float max_m  = g2.user_parameters.max_dist_m();

    // ---- задача 15: значення для C++/MAVSDK (повідомлення NAMED_VALUE_FLOAT) ----
    gcs().send_named_float("HOME_DIST", dist_m);

#if HAL_LOGGING_ENABLED
    // ---- задача 10: свій запис у лог ----
    // @LoggerMessage: USR1
    // @Field: TimeUS: час
    // @Field: Dist: відстань до home, м
    // @Field: Max: USR_MAX_DIST, м
    AP::logger().Write("USR1", "TimeUS,Dist,Max", "Qff",
                       AP_HAL::micros64(), dist_m, max_m);
#endif

    // ---- задача 11: свій failsafe ----
    if (motors->armed() && max_m > 0.0f && dist_m > max_m) {
        const Mode::Number m = flightmode->mode_number();
        if (m != Mode::Number::RTL && m != Mode::Number::LAND) {
            gcs().send_text(MAV_SEVERITY_WARNING, "USR: %.0fm > %.0fm -> RTL", dist_m, max_m);
            set_mode(Mode::Number::RTL, ModeReason::FENCE_BREACHED);
        }
    }
#endif // USER_PARAMS_ENABLED
}
#endif
