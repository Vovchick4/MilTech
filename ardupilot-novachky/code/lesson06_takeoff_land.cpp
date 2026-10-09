// Урок 6. Зліт і посадка.
// ARM -> зліт на 10 м -> висіти 5 секунд -> посадка -> чекати DISARM.
//
// Запуск:  ./scripts/run.sh lesson06_takeoff_land
// Дивись у Mission Planner: дрон злітає і сідає.

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>         // плагін «дії»: arm, takeoff, land...
#include <mavsdk/plugins/telemetry/telemetry.hpp>

using namespace mavsdk;

const float TAKEOFF_ALTITUDE_M = 10.0f;   // на яку висоту злетіти

void wait_seconds(int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// ARM + зліт з повторними спробами.
// Перші ~30 секунд після старту SITL автопілот ще «прогрівається» (EKF шукає позицію):
// ARM може вже пройти, а зліт — ще ні («requires position»). Тому пробуємо кілька разів.
// Повертає true, якщо злетіли на потрібну висоту.
bool arm_and_takeoff(Action& action, Telemetry& telemetry, float altitude_m)
{
    action.set_takeoff_altitude(altitude_m);
    bool started = false;
    for (int attempt = 1; attempt <= 20; attempt++) {
        Action::Result arm_result = action.arm();
        if (arm_result == Action::Result::Success) {
            Action::Result takeoff_result = action.takeoff();
            if (takeoff_result == Action::Result::Success) {
                started = true;
                break;
            }
            std::cout << "  зліт поки не вдається (" << takeoff_result << "), спроба " << attempt << "\n";
        } else {
            std::cout << "  ARM поки не вдається (" << arm_result << "), спроба " << attempt << "\n";
        }
        wait_seconds(3);
    }
    if (!started) {
        std::cout << "Не вдалося злетіти. Дивись причину в Mission Planner -> Messages\n";
        return false;
    }

    // Чекаємо, поки наберемо висоту. Якщо мотори раптом вимкнулись — виходимо.
    while (telemetry.position().relative_altitude_m < altitude_m - 0.5f) {
        if (!telemetry.armed()) {
            std::cout << "Мотори вимкнулись під час зльоту!\n";
            return false;
        }
        std::cout << "  набираємо висоту: " << telemetry.position().relative_altitude_m << " м\n";
        wait_seconds(1);
    }
    std::cout << "Злетіли на " << telemetry.position().relative_altitude_m << " м\n";
    return true;
}

int main()
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk.add_any_connection("tcpout://127.0.0.1:5762");
    auto system = mavsdk.first_autopilot(10.0);
    if (!system) {
        std::cout << "Автопілот не знайдено\n";
        return 1;
    }
    Telemetry telemetry{system.value()};
    // Просимо автопілот надсилати позицію 2 рази на секунду, а батарею — раз на секунду.
    // Без цього ArduPilot може взагалі не надсилати ці дані, і ми побачимо nan.
    telemetry.set_rate_position(2.0);
    telemetry.set_rate_battery(1.0);
    Action action{system.value()};

    // Чекаємо, поки дрон готовий: GPS знайшов позицію і запам'ятав «дім» (home).
    // isnan(...) — «чи це ще не число?» Поки даних немає, MAVSDK повертає nan.
    std::cout << "Чекаємо GPS...\n";
    while (!telemetry.health().is_home_position_ok || std::isnan(telemetry.position().latitude_deg)) {
        wait_seconds(1);
    }
    std::cout << "GPS готовий\n";

    // 2-4. ARM, зліт і набір висоти — у функції arm_and_takeoff (вище)
    if (!arm_and_takeoff(action, telemetry, TAKEOFF_ALTITUDE_M)) {
        return 1;
    }

    // 5. Висимо 5 секунд.
    std::cout << "Висимо 5 секунд...\n";
    wait_seconds(5);

    // 6. Посадка.
    Action::Result result = action.land();
    std::cout << "LAND: " << result << "\n";

    // 7. Чекаємо, поки дрон сяде і сам вимкне мотори (DISARM).
    while (telemetry.armed()) {
        std::cout << "  сідаємо: " << telemetry.position().relative_altitude_m << " м\n";
        wait_seconds(1);
    }
    std::cout << "Сіли, мотори вимкнено. Політ завершено!\n";
    return 0;
}
