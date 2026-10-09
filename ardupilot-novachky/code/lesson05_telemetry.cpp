// Урок 5. Телеметрія: читаємо, що відбувається з дроном.
// Програма 15 секунд щосекунди друкує висоту, координати, заряд, режим і чи ARMED.
//
// Запуск:  ./scripts/run.sh lesson05_telemetry
// Поки програма працює — спробуй у Mission Planner змінити режим або ARM. Що зміниться у виводі?

#include <chrono>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>   // плагін «телеметрія»

using namespace mavsdk;

int main()
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk.add_any_connection("tcpout://127.0.0.1:5762");
    auto system = mavsdk.first_autopilot(10.0);
    if (!system) {
        std::cout << "Автопілот не знайдено\n";
        return 1;
    }

    // Об'єкт, через який читаємо дані з дрона
    Telemetry telemetry{system.value()};
    // Просимо автопілот надсилати позицію 2 рази на секунду, а батарею — раз на секунду.
    // Без цього ArduPilot може взагалі не надсилати ці дані, і ми побачимо nan.
    telemetry.set_rate_position(2.0);
    telemetry.set_rate_battery(1.0);

    for (int second = 1; second <= 15; second++) {
        Telemetry::Position position = telemetry.position();
        Telemetry::Battery battery = telemetry.battery();

        std::cout << "t=" << second << "с"
                  << "  висота=" << position.relative_altitude_m << " м"
                  << "  lat=" << position.latitude_deg
                  << "  lon=" << position.longitude_deg
                  << "  заряд=" << battery.remaining_percent << "%"
                  << "  режим=" << telemetry.flight_mode()
                  << "  ARMED=" << (telemetry.armed() ? "так" : "ні")
                  << "\n";

        std::this_thread::sleep_for(std::chrono::seconds(1)); // почекати 1 секунду
    }
    return 0;
}
