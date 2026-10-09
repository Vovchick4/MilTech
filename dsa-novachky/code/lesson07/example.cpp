// Урок 7. Словники: std::map і std::unordered_map («ключ -> значення»).
//   g++ -std=c++17 -Wall example.cpp -o example && ./example

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

int main()
{
    // ------------------------------------------------------------
    // 1) Параметри автопілота: ім'я -> значення (як у Mission Planner)
    // ------------------------------------------------------------
    std::map<std::string, double> params;
    params["WPNAV_SPEED"] = 1500;  // додати
    params["RTL_ALT"] = 3000;
    params["FENCE_ENABLE"] = 0;
    params["BATT_CAPACITY"] = 5200;

    params["RTL_ALT"] = 4000;      // такий ключ уже є -> значення ЗАМІНЮЄТЬСЯ

    std::cout << "RTL_ALT = " << params["RTL_ALT"] << "\n";
    std::cout << "Кількість параметрів: " << params.size() << "\n";

    // Чи є ключ? count() повертає 1 або 0.
    if (params.count("FENCE_ENABLE") == 1) {
        std::cout << "FENCE_ENABLE є\n";
    }
    if (params.count("SPEED_MAX") == 0) {
        std::cout << "SPEED_MAX немає\n";
    }

    // std::map зберігає ключі ВІДСОРТОВАНИМИ (за алфавітом)
    std::cout << "\nУсі параметри (std::map — за алфавітом):\n";
    for (const auto& item : params) {   // item.first — ключ, item.second — значення
        std::cout << "  " << item.first << " = " << item.second << "\n";
    }

    // ПАСТКА: [] для неіснуючого ключа СТВОРЮЄ його зі значенням 0!
    std::cout << "\nДо помилки розмір = " << params.size();
    double oops = params["WPNAV_SPED"]; // одруківка в імені
    std::cout << ", після params[\"WPNAV_SPED\"] розмір = " << params.size()
              << " (значення " << oops << ") — з'явився зайвий параметр!\n";
    params.erase("WPNAV_SPED"); // видалити

    // ------------------------------------------------------------
    // 2) Підрахунок: скільки секунд дрон був у кожному режимі
    // ------------------------------------------------------------
    std::vector<std::string> log = {"STABILIZE", "GUIDED", "GUIDED", "GUIDED", "AUTO",
                                    "AUTO", "AUTO", "AUTO", "RTL", "RTL", "LAND"};
    std::unordered_map<std::string, int> seconds_in_mode;
    for (int i = 0; i < (int)log.size(); i++) {
        seconds_in_mode[log[i]] = seconds_in_mode[log[i]] + 1; // тут [] створює 0 — і це нам якраз підходить
    }
    std::cout << "\nСекунд у кожному режимі (unordered_map — порядок довільний):\n";
    for (const auto& item : seconds_in_mode) {
        std::cout << "  " << item.first << ": " << item.second << "\n";
    }
    return 0;
}
