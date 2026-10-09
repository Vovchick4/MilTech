// Урок 7. Політ до точки.
// Зліт на 20 м -> летимо в точку на 100 м ПІВНІЧНІШЕ -> повертаємось додому -> посадка.
//
// Запуск:  ./scripts/run.sh lesson07_goto_point

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>

using namespace mavsdk;

const float ALTITUDE_M = 20.0f;              // висота польоту над точкою зльоту
const double METERS_PER_DEGREE = 111320.0;   // скільки метрів в 1 градусі широти
const double ARRIVED_RADIUS_M = 5.0;         // ближче за 5 м — вважаємо, що долетіли

void wait_seconds(int seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

// Перетворити «на скільки метрів на північ» у «на скільки градусів широти».
double north_meters_to_degrees(double meters)
{
    return meters / METERS_PER_DEGREE;
}

// Те саме для сходу. Градус довготи коротшає ближче до полюсів, тому множимо на cos(широти).
double east_meters_to_degrees(double meters, double latitude_deg)
{
    double latitude_rad = latitude_deg * M_PI / 180.0;
    return meters / (METERS_PER_DEGREE * std::cos(latitude_rad));
}

// Приблизна відстань у метрах між двома точками (для коротких відстаней — дуже точно).
double distance_m(double lat1, double lon1, double lat2, double lon2)
{
    double north = (lat2 - lat1) * METERS_PER_DEGREE;
    double east = (lon2 - lon1) * METERS_PER_DEGREE * std::cos(lat1 * M_PI / 180.0);
    return std::sqrt(north * north + east * east);   // теорема Піфагора
}

// Полетіти в точку і чекати, поки долетимо.
void fly_to(Action& action, Telemetry& telemetry, double lat, double lon, float altitude_amsl)
{
    Action::Result result = action.goto_location(lat, lon, altitude_amsl, NAN);
    std::cout << "GOTO: " << result << "\n";

    while (true) {
        Telemetry::Position now = telemetry.position();
        double left = distance_m(now.latitude_deg, now.longitude_deg, lat, lon);
        std::cout << "  до точки: " << (int)left << " м,  висота " << now.relative_altitude_m << " м\n";
        if (left < ARRIVED_RADIUS_M) {
            break;
        }
        wait_seconds(1);
    }
    std::cout << "Долетіли!\n";
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

    // Запам'ятовуємо «дім» — точку, з якої злітаємо.
    Telemetry::Position home = telemetry.position();
    std::cout << "Дім: lat=" << home.latitude_deg << " lon=" << home.longitude_deg << "\n";

    // goto_location хоче висоту НАД РІВНЕМ МОРЯ (AMSL), а не над землею.
    // Висота дому над морем + скільки хочемо над землею:
    float altitude_amsl = home.absolute_altitude_m + ALTITUDE_M;

    // Точка на 100 м північніше дому.
    double target_lat = home.latitude_deg + north_meters_to_degrees(100.0);
    double target_lon = home.longitude_deg;

    // ARM + зліт (функція з уроку 6)
    if (!arm_and_takeoff(action, telemetry, ALTITUDE_M)) {
        return 1;
    }

    std::cout << "\n--- Летимо до точки (100 м на північ) ---\n";
    fly_to(action, telemetry, target_lat, target_lon, altitude_amsl);

    std::cout << "\n--- Повертаємось додому ---\n";
    fly_to(action, telemetry, home.latitude_deg, home.longitude_deg, altitude_amsl);

    action.land();
    while (telemetry.armed()) {
        wait_seconds(1);
    }
    std::cout << "Сіли. Готово!\n";
    return 0;
}
