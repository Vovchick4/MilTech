// Крок 2: ARM -> TAKEOFF 20 м -> зависнути 10 с -> LAND.
// Запуск: ./build/02_takeoff_land
#include <chrono>
#include <iostream>
#include <thread>

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

int main()
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk.add_any_connection("tcp://127.0.0.1:5762");

    auto system = mavsdk.first_autopilot(10.0);
    if (!system) {
        std::cerr << "Autopilot не знайдено\n";
        return 1;
    }

    Telemetry telemetry{system.value()};
    Action action{system.value()};

    std::cout << "Чекаємо готовності (GPS + home)...\n";
    while (!(telemetry.health().is_global_position_ok && telemetry.health().is_home_position_ok)) {
        std::this_thread::sleep_for(1s);
    }

    action.set_takeoff_altitude(20.0f);

    Action::Result r = action.arm();
    std::cout << "ARM: " << r << "\n";
    if (r != Action::Result::Success) return 1;

    r = action.takeoff();
    std::cout << "TAKEOFF: " << r << "\n";
    if (r != Action::Result::Success) return 1;

    while (telemetry.position().relative_altitude_m < 19.0f) {
        std::cout << "alt=" << telemetry.position().relative_altitude_m << " m\n";
        std::this_thread::sleep_for(1s);
    }

    std::cout << "Висіти 10 с...\n";
    std::this_thread::sleep_for(10s);

    std::cout << "LAND: " << action.land() << "\n";
    while (telemetry.armed()) {
        std::this_thread::sleep_for(1s);
    }
    std::cout << "Готово, DISARMED\n";
    return 0;
}
