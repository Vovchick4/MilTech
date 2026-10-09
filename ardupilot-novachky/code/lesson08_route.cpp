// Урок 8. Маршрут з кількох точок.
// Точки задаються списком (вектором): скільки метрів на північ і на схід від дому.
// Зліт -> облітаємо всі точки по черзі -> посадка вдома.
//
// Запуск:  ./scripts/run.sh lesson08_route

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

const float ALTITUDE_M = 30.0f;
const double SIDE_M = 200.0;          // сторона квадрата. Спробуй 2500 (= маршрут 10 км)!
const float SPEED_M_S = 10.0f;        // швидкість польоту
const double METERS_PER_DEGREE = 111320.0;
const double ARRIVED_RADIUS_M = 5.0;

// Одна точка маршруту
struct Waypoint {
    std::string name;
    double north_m;   // на скільки метрів північніше дому
    double east_m;    // на скільки метрів східніше дому
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

void fly_to(Action& action, Telemetry& telemetry, const std::string& name, double lat, double lon, float altitude_amsl)
{
    Telemetry::Position start = telemetry.position();
    double total = distance_m(start.latitude_deg, start.longitude_deg, lat, lon);
    std::cout << "\n--- Летимо до " << name << " (" << (int)total << " м) ---\n";
    action.goto_location(lat, lon, altitude_amsl, NAN);

    int seconds = 0;
    while (true) {
        Telemetry::Position now = telemetry.position();
        double left = distance_m(now.latitude_deg, now.longitude_deg, lat, lon);
        if (left < ARRIVED_RADIUS_M) {
            break;
        }
        if (seconds % 5 == 0) {   // друкуємо раз на 5 секунд, щоб не засмічувати екран
            int percent = (int)(100.0 * (total - left) / total);
            std::cout << "  лишилось " << (int)left << " м  (" << percent << "%)\n";
        }
        wait_seconds(1);
        seconds = seconds + 1;
    }
    std::cout << "  " << name << " досягнуто за " << seconds << " с\n";
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
    // Маршрут: квадрат. Дім -> північ -> північ-схід -> схід -> дім.
    std::vector<Waypoint> route = {
        {"P1 (північ)", SIDE_M, 0},
        {"P2 (північ-схід)", SIDE_M, SIDE_M},
        {"P3 (схід)", 0, SIDE_M},
        {"ДІМ", 0, 0},
    };

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

    std::cout << "Маршрут з " << route.size() << " точок, сторона квадрата " << SIDE_M << " м\n";

    // ARM + зліт (функція з уроку 6)
    if (!arm_and_takeoff(action, telemetry, ALTITUDE_M)) {
        return 1;
    }
    action.set_current_speed(SPEED_M_S);

    // Головне: пройти циклом по всіх точках маршруту
    for (int i = 0; i < (int)route.size(); i++) {
        double lat = home.latitude_deg + route[i].north_m / METERS_PER_DEGREE;
        double lon = home.longitude_deg +
                     route[i].east_m / (METERS_PER_DEGREE * std::cos(home.latitude_deg * M_PI / 180.0));
        fly_to(action, telemetry, route[i].name, lat, lon, altitude_amsl);
    }

    std::cout << "\nМаршрут пройдено, сідаємо\n";
    action.land();
    while (telemetry.armed()) {
        wait_seconds(1);
    }
    std::cout << "Готово!\n";
    return 0;
}
