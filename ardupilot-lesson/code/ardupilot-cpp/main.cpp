// =====================================================================
//  ArduPilot SITL + MAVSDK + C++  —  великий квадратний маршрут
// ---------------------------------------------------------------------
//  Схема підключення:
//
//      ArduPilot SITL
//          |
//          +---- TCP 5760 ----> Mission Planner / QGroundControl
//          |
//          +---- TCP 5762 ----> ця програма (MAVSDK)
//
//  Маршрут (квадрат, ROUTE_KM — ПОВНА довжина периметра):
//
//          P1 ---------> P2
//          ^              |
//          |              v
//        HOME <--------- P3
//
//  Програма:
//    1. підключається до tcp://127.0.0.1:5762
//    2. чекає GPS fix + home position
//    3. ARM -> TAKEOFF до ALTITUDE_M
//    4. летить HOME -> P1 -> P2 -> P3 -> HOME і ЧЕКАЄ фактичного досягнення
//       кожної точки (за телеметрією, а не через sleep)
//    5. LAND -> чекає DISARM
//
//  Ctrl+C під час польоту -> RTL (повернення додому).
// =====================================================================

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Нова MAVSDK (v3/v4, main-гілка) має заголовки .hpp, стара (v1/v2) — .h.
// __has_include дозволяє зібрати код з будь-якою з них.
#if __has_include(<mavsdk/plugins/action/action.hpp>)
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#else
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#endif

using namespace mavsdk;
using namespace std::chrono_literals;

// ---------------------- НАЛАШТУВАННЯ (міняти тут) ----------------------

constexpr const char* CONNECTION_URL = "tcp://127.0.0.1:5762"; // НЕ 5760!

constexpr double ROUTE_KM        = 10.0;  // 10.0 / 50.0 / 100.0 — периметр квадрата
constexpr float  ALTITUDE_M      = 100.0f; // висота над точкою зльоту
constexpr float  SPEED_M_S       = 15.0f;  // горизонтальна швидкість
constexpr double ACCEPT_RADIUS_M = 25.0;   // точка вважається досягнутою
constexpr int    PROGRESS_EVERY_S = 3;     // як часто друкувати прогрес

// -----------------------------------------------------------------------

namespace {

std::atomic<bool> g_abort{false};

void on_sigint(int) { g_abort = true; }

struct Waypoint {
    std::string name;
    double lat;
    double lon;
};

constexpr double EARTH_RADIUS_M = 6371000.0;
constexpr double DEG2RAD = M_PI / 180.0;
constexpr double RAD2DEG = 180.0 / M_PI;

// Відстань між двома GPS-точками (формула гаверсинуса), метри.
double distance_m(double lat1, double lon1, double lat2, double lon2)
{
    const double dlat = (lat2 - lat1) * DEG2RAD;
    const double dlon = (lon2 - lon1) * DEG2RAD;
    const double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
                     std::cos(lat1 * DEG2RAD) * std::cos(lat2 * DEG2RAD) *
                         std::sin(dlon / 2) * std::sin(dlon / 2);
    return 2.0 * EARTH_RADIUS_M * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}

// Зсув точки на north_m метрів на північ і east_m метрів на схід.
Waypoint offset(const std::string& name, double lat, double lon, double north_m, double east_m)
{
    const double new_lat = lat + (north_m / EARTH_RADIUS_M) * RAD2DEG;
    const double new_lon = lon + (east_m / (EARTH_RADIUS_M * std::cos(lat * DEG2RAD))) * RAD2DEG;
    return {name, new_lat, new_lon};
}

std::string fmt_time(double seconds)
{
    const int s = static_cast<int>(seconds);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", s / 3600, (s / 60) % 60, s % 60);
    return buf;
}

bool check(Action::Result r, const char* what)
{
    if (r != Action::Result::Success) {
        std::cerr << "[ERROR] " << what << " -> " << r << "\n";
        return false;
    }
    std::cout << "[OK] " << what << "\n";
    return true;
}

// Чекаємо, поки дрон долетить до точки. Повертає false при помилці/таймауті/Ctrl+C.
bool fly_to(Action& action, Telemetry& telemetry, const Waypoint& wp, float abs_alt_m)
{
    const auto start_pos = telemetry.position();
    const double leg_m = distance_m(start_pos.latitude_deg, start_pos.longitude_deg, wp.lat, wp.lon);

    // Таймаут: утричі довше за розрахунковий час + 2 хв запасу.
    const auto timeout = std::chrono::seconds(static_cast<long>(leg_m / SPEED_M_S * 3.0) + 120);

    std::cout << "\n=== Летимо до " << wp.name << " (" << std::fixed << std::setprecision(7)
              << wp.lat << ", " << wp.lon << "), відрізок " << std::setprecision(0) << leg_m
              << " м ===\n";

    if (!check(action.goto_location(wp.lat, wp.lon, abs_alt_m, NAN), "goto_location")) {
        return false;
    }

    const auto t0 = std::chrono::steady_clock::now();
    auto last_print = t0 - std::chrono::seconds(PROGRESS_EVERY_S);

    while (true) {
        if (g_abort) {
            return false;
        }
        if (!telemetry.armed()) {
            std::cerr << "[ERROR] Дрон DISARMED під час польоту!\n";
            return false;
        }

        const auto pos = telemetry.position();
        const double dist = distance_m(pos.latitude_deg, pos.longitude_deg, wp.lat, wp.lon);
        const auto now = std::chrono::steady_clock::now();

        if (now - last_print >= std::chrono::seconds(PROGRESS_EVERY_S)) {
            last_print = now;
            const double elapsed = std::chrono::duration<double>(now - t0).count();
            const double done = leg_m > 1.0 ? 100.0 * (1.0 - dist / leg_m) : 100.0;
            std::cout << std::fixed
                      << "Current: lat=" << std::setprecision(7) << pos.latitude_deg
                      << " lon=" << pos.longitude_deg
                      << " alt=" << std::setprecision(1) << pos.relative_altitude_m << "m"
                      << " | mode=" << telemetry.flight_mode()
                      << " armed=" << (telemetry.armed() ? "YES" : "NO") << "\n"
                      << "Target:  " << wp.name << " lat=" << std::setprecision(7) << wp.lat
                      << " lon=" << wp.lon << "\n"
                      << "Distance to target: " << std::setprecision(0) << dist << " m"
                      << "  (" << std::setprecision(1) << std::max(0.0, done) << "%, "
                      << fmt_time(elapsed) << ", ETA ~" << fmt_time(dist / SPEED_M_S) << ")\n\n";
        }

        if (dist <= ACCEPT_RADIUS_M) {
            std::cout << "[OK] Досягнуто " << wp.name << "\n";
            return true;
        }
        if (now - t0 > timeout) {
            std::cerr << "[ERROR] Таймаут до " << wp.name << "\n";
            return false;
        }
        std::this_thread::sleep_for(200ms);
    }
}

void land_and_wait(Action& action, Telemetry& telemetry)
{
    check(action.land(), "LAND");
    while (telemetry.armed()) {
        std::cout << "Посадка... alt=" << std::fixed << std::setprecision(1)
                  << telemetry.position().relative_altitude_m << " m\n";
        std::this_thread::sleep_for(2s);
    }
    std::cout << "[OK] Приземлились, DISARMED\n";
}

} // namespace

int main()
{
    std::signal(SIGINT, on_sigint);

    // ---------- 1. Підключення ----------
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    const ConnectionResult conn = mavsdk.add_any_connection(CONNECTION_URL);
    if (conn != ConnectionResult::Success) {
        std::cerr << "Не вдалося підключитися до " << CONNECTION_URL << ": " << conn << "\n";
        return 1;
    }
    std::cout << "Підключення до " << CONNECTION_URL << " ...\n";

    auto system_opt = mavsdk.first_autopilot(10.0);
    if (!system_opt) {
        std::cerr << "Autopilot не знайдено за 10 с. Чи запущений SITL? Чи вільний порт 5762?\n";
        return 1;
    }
    auto system = system_opt.value();
    std::cout << "[OK] Autopilot знайдено\n";

    Telemetry telemetry{system};
    Action action{system};

    telemetry.set_rate_position(2.0); // 2 Гц позиції (для armed/flight_mode set_rate не існує)

    telemetry.subscribe_armed([](bool armed) {
        std::cout << ">>> ARMED: " << (armed ? "YES" : "NO") << "\n";
    });
    telemetry.subscribe_flight_mode([](Telemetry::FlightMode mode) {
        std::cout << ">>> FLIGHT MODE: " << mode << "\n";
    });

    // ---------- 2. Чекаємо GPS + home ----------
    std::cout << "Чекаємо GPS fix та home position...\n";
    for (int i = 0; i < 120; ++i) {
        const auto h = telemetry.health();
        if (h.is_global_position_ok && h.is_home_position_ok) {
            break;
        }
        if (i % 5 == 0) {
            std::cout << "  global_pos=" << h.is_global_position_ok
                      << " home=" << h.is_home_position_ok << "\n";
        }
        std::this_thread::sleep_for(1s);
    }

    const auto home = telemetry.position();
    if (std::isnan(home.latitude_deg)) {
        std::cerr << "Немає позиції GPS. Перевір SITL.\n";
        return 1;
    }
    const float abs_alt = home.absolute_altitude_m + ALTITUDE_M; // goto_location хоче AMSL

    // ---------- 3. Будуємо маршрут ----------
    const double side_m = ROUTE_KM * 1000.0 / 4.0;
    const std::vector<Waypoint> route = {
        offset("P1", home.latitude_deg, home.longitude_deg, side_m, 0.0),
        offset("P2", home.latitude_deg, home.longitude_deg, side_m, side_m),
        offset("P3", home.latitude_deg, home.longitude_deg, 0.0, side_m),
        {"HOME", home.latitude_deg, home.longitude_deg},
    };

    std::cout << std::fixed << std::setprecision(7)
              << "\nHOME: " << home.latitude_deg << ", " << home.longitude_deg
              << "  AMSL=" << std::setprecision(1) << home.absolute_altitude_m << " m\n"
              << "Маршрут: " << ROUTE_KM << " км (сторона квадрата " << side_m / 1000.0
              << " км), висота " << ALTITUDE_M << " м, швидкість " << SPEED_M_S << " м/с\n"
              << "Орієнтовний час польоту: " << fmt_time(ROUTE_KM * 1000.0 / SPEED_M_S)
              << " (симуляційного часу)\n";
    for (const auto& wp : route) {
        std::cout << "  " << wp.name << ": " << std::setprecision(7) << wp.lat << ", " << wp.lon
                  << "\n";
    }

    // ---------- 4. ARM + TAKEOFF ----------
    check(action.set_takeoff_altitude(ALTITUDE_M), "set_takeoff_altitude");

    bool armed = false;
    for (int attempt = 1; attempt <= 10 && !armed && !g_abort; ++attempt) {
        const auto r = action.arm();
        if (r == Action::Result::Success) {
            armed = true;
        } else {
            std::cout << "ARM спроба " << attempt << ": " << r
                      << " (дивись повідомлення PreArm у Mission Planner)\n";
            std::this_thread::sleep_for(3s);
        }
    }
    if (!armed) {
        std::cerr << "Не вдалося ARM\n";
        return 1;
    }
    std::cout << "[OK] ARM\n";

    // Одразу takeoff — інакше ArduPilot сам роззброїться через кілька секунд.
    if (!check(action.takeoff(), "TAKEOFF")) {
        return 1;
    }

    while (!g_abort) {
        const float alt = telemetry.position().relative_altitude_m;
        std::cout << "Набір висоти: " << std::setprecision(1) << alt << " / " << ALTITUDE_M
                  << " м\n";
        if (alt >= ALTITUDE_M * 0.95f) {
            break;
        }
        std::this_thread::sleep_for(1s);
    }

    check(action.set_current_speed(SPEED_M_S), "set_current_speed");

    // ---------- 5. Маршрут ----------
    const auto flight_start = std::chrono::steady_clock::now();
    bool ok = true;
    for (const auto& wp : route) {
        if (!fly_to(action, telemetry, wp, abs_alt)) {
            ok = false;
            break;
        }
    }

    if (!ok) {
        std::cerr << "\nМаршрут перервано -> RTL\n";
        check(action.return_to_launch(), "RTL");
        while (telemetry.armed()) {
            std::this_thread::sleep_for(2s);
        }
        return 2;
    }

    std::cout << "\nМаршрут завершено за "
              << fmt_time(std::chrono::duration<double>(std::chrono::steady_clock::now() -
                                                        flight_start)
                              .count())
              << " (реального часу)\n";

    // ---------- 6. LAND ----------
    land_and_wait(action, telemetry);
    return 0;
}
