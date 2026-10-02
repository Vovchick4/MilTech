// Крок 1: тільки підключення + телеметрія. Дрон НЕ рухається.
// Запуск: ./build/01_telemetry   (Ctrl+C — вихід)
#include <chrono>
#include <iostream>
#include <thread>

#if __has_include(<mavsdk/plugins/telemetry/telemetry.hpp>)
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#else
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#endif

using namespace mavsdk;
using namespace std::chrono_literals;

int main()
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    if (mavsdk.add_any_connection("tcp://127.0.0.1:5762") != ConnectionResult::Success) {
        std::cerr << "Помилка підключення\n";
        return 1;
    }

    auto system = mavsdk.first_autopilot(10.0);
    if (!system) {
        std::cerr << "Autopilot не знайдено\n";
        return 1;
    }
    std::cout << "Підключено!\n";

    Telemetry telemetry{system.value()};
    telemetry.set_rate_position(1.0);

    telemetry.subscribe_position([](Telemetry::Position p) {
        std::cout << "GPS: lat=" << p.latitude_deg << " lon=" << p.longitude_deg
                  << " rel_alt=" << p.relative_altitude_m << " m"
                  << " amsl=" << p.absolute_altitude_m << " m\n";
    });
    telemetry.subscribe_armed([](bool armed) {
        std::cout << "ARMED: " << (armed ? "YES" : "NO") << "\n";
    });
    telemetry.subscribe_flight_mode([](Telemetry::FlightMode m) {
        std::cout << "MODE: " << m << "\n";
    });
    telemetry.subscribe_battery([](Telemetry::Battery b) {
        std::cout << "BATTERY: " << b.voltage_v << " V, " << b.remaining_percent << " %\n";
    });

    while (true) {
        std::this_thread::sleep_for(1s);
    }
}
