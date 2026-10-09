// Урок 10. Підсумкове завдання: «Патруль трикутником».
//
// Що має зробити програма:
//   1. Підключитися і дочекатися GPS.                          (готово)
//   2. ARM і зліт на 25 м.                                      (TODO 1)
//   3. Облетіти трикутник: A (150 м північ), B (150 м схід),    (TODO 2)
//      C (150 м північ і 150 м схід).
//   4. У кожній точці надрукувати звіт: назва, висота, заряд.   (TODO 3)
//   5. Якщо заряд < MIN_BATTERY — НЕ летіти далі, а одразу RTL.  (TODO 4)
//   6. Наприкінці — RTL (повернення додому) і чекати DISARM.    (TODO 5)
//
// Підказки — в уроках 6, 7, 8. Можна (і треба!) копіювати звідти код.
// Запуск:  ./scripts/run.sh lesson10_final_task

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>

using namespace mavsdk;

const float ALTITUDE_M = 25.0f;
const float MIN_BATTERY = 30.0f;   // % — нижче цього летимо додому
const double METERS_PER_DEGREE = 111320.0;
const double ARRIVED_RADIUS_M = 5.0;

struct Waypoint {
    std::string name;
    double north_m;
    double east_m;
};

void wait_seconds(int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

double distance_m(double lat1, double lon1, double lat2, double lon2)
{
    double north = (lat2 - lat1) * METERS_PER_DEGREE;
    double east = (lon2 - lon1) * METERS_PER_DEGREE * std::cos(lat1 * M_PI / 180.0);
    return std::sqrt(north * north + east * east);
}

// Готова функція: летіти в точку і чекати.
void fly_to(Action& action, Telemetry& telemetry, double lat, double lon, float altitude_amsl)
{
    action.goto_location(lat, lon, altitude_amsl, NAN);
    while (true) {
        Telemetry::Position now = telemetry.position();
        if (distance_m(now.latitude_deg, now.longitude_deg, lat, lon) < ARRIVED_RADIUS_M) {
            return;
        }
        wait_seconds(1);
    }
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
    Telemetry::Position home = telemetry.position();
    float altitude_amsl = home.absolute_altitude_m + ALTITUDE_M;
    std::cout << "Дім запам'ятали\n";

    // TODO 1: ARM і зліт на ALTITUDE_M — виклич функцію arm_and_takeoff (вона вже є вище).
    //         Якщо вона повернула false — завершити програму (return 1).


    // TODO 2: створити std::vector<Waypoint> з трьох точок A, B, C (див. опис зверху).
    std::vector<Waypoint> route;


    for (int i = 0; i < (int)route.size(); i++) {
        // TODO 4: якщо заряд (telemetry.battery().remaining_percent) < MIN_BATTERY —
        //         надрукувати попередження і вийти з циклу (break).


        double lat = home.latitude_deg + route[i].north_m / METERS_PER_DEGREE;
        double lon = home.longitude_deg +
                     route[i].east_m / (METERS_PER_DEGREE * std::cos(home.latitude_deg * M_PI / 180.0));
        fly_to(action, telemetry, lat, lon, altitude_amsl);

        // TODO 3: надрукувати звіт у точці: назва, висота (relative_altitude_m), заряд.

    }

    // TODO 5: action.return_to_launch() і чекати, поки telemetry.armed() стане false.


    std::cout << "Патруль завершено\n";
    return 0;
}
