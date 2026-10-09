// Урок 4. Перша програма: підключитися до дрона в симуляторі.
//
// Перед запуском: SITL запущений, Mission Planner підключений до порту 5760.
// Запуск:  ./scripts/run.sh lesson04_connect

#include <iostream>

#include <mavsdk/mavsdk.hpp>   // головна частина бібліотеки MAVSDK

using namespace mavsdk;        // щоб писати Mavsdk замість mavsdk::Mavsdk

int main()
{
    // 1. Створюємо MAVSDK і кажемо, ким ми є: наземною станцією (GroundStation),
    //    тобто «пультом», а не дроном.
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    // 2. Підключаємось до симулятора. Порт 5762 — «друга розетка» SITL
    //    (перша, 5760, вже зайнята Mission Planner).
    ConnectionResult result = mavsdk.add_any_connection("tcpout://127.0.0.1:5762");
    if (result != ConnectionResult::Success) {
        std::cout << "Не вдалося підключитися: " << result << "\n";
        return 1;
    }
    std::cout << "З'єднання відкрито, шукаємо автопілот...\n";

    // 3. Чекаємо, поки автопілот «представиться» (до 10 секунд).
    auto system = mavsdk.first_autopilot(10.0);
    if (!system) {
        std::cout << "Автопілот не знайдено. Чи запущений SITL? Чи підключений Mission Planner до 5760?\n";
        return 1;
    }

    std::cout << "Підключено! Номер системи (system id): " << system.value()->get_system_id() << "\n";
    return 0;
}
