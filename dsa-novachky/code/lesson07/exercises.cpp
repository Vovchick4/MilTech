// Урок 7. Вправи. Заповни TODO.
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises

#include <iostream>
#include <map>
#include <string>
#include <vector>

int g_failed = 0;
void check(bool ok, const std::string& name)
{
    if (ok) {
        std::cout << "[OK]   " << name << "\n";
    } else {
        std::cout << "[FAIL] " << name << "\n";
        g_failed = g_failed + 1;
    }
}

// 7.1 Порахувати, скільки разів зустрічається кожен режим у лозі.
std::map<std::string, int> count_modes(const std::vector<std::string>& log)
{
    std::map<std::string, int> result;
    // TODO
    (void)log;
    return result;
}

// 7.2 Повернути значення параметра, а якщо його немає — default_value.
// НЕ використовуй params[name] — він би створив параметр (а params тут const, тож і не скомпілюється).
// Використай params.count(name) і params.at(name).
double get_param(const std::map<std::string, double>& params, const std::string& name, double default_value)
{
    // TODO
    (void)params;
    (void)name;
    return default_value;
}

// 7.3 Режим, у якому дрон був найдовше (найчастіше в лозі). Лог не порожній.
// Якщо таких кілька — той, що раніше за алфавітом (std::map вже обходить за алфавітом).
std::string most_common_mode(const std::vector<std::string>& log)
{
    // TODO: count_modes + пошук максимуму
    (void)log;
    return "";
}

// 7.4 Порівняти параметри «до» і «після» польоту. Повернути імена параметрів (за алфавітом),
// які ЗМІНИЛИ значення, ЗНИКЛИ або З'ЯВИЛИСЬ.
std::vector<std::string> changed_params(const std::map<std::string, double>& before,
                                        const std::map<std::string, double>& after)
{
    std::vector<std::string> result;
    // TODO: пройди по before (змінені + зниклі), потім по after (нові).
    // Наприкінці відсортуй result (std::sort з <algorithm>) — або подумай, як обійтись без цього.
    (void)before;
    (void)after;
    return result;
}

int main()
{
    std::vector<std::string> log = {"GUIDED", "AUTO", "AUTO", "RTL", "AUTO", "GUIDED"};
    std::map<std::string, int> modes = count_modes(log);
    check(modes.size() == 3, "7.1 три різні режими");
    check(modes.count("AUTO") == 1 && modes.at("AUTO") == 3, "7.1 AUTO = 3");
    check(modes.count("RTL") == 1 && modes.at("RTL") == 1, "7.1 RTL = 1");

    std::map<std::string, double> p = {{"RTL_ALT", 3000}, {"WPNAV_SPEED", 1500}};
    check(get_param(p, "RTL_ALT", -1) == 3000, "7.2 є параметр");
    check(get_param(p, "XXX", -1) == -1, "7.2 немає параметра -> default");

    check(most_common_mode(log) == "AUTO", "7.3 AUTO найчастіший");
    check(most_common_mode({"RTL", "LAND", "LAND", "RTL"}) == "LAND", "7.3 нічия -> за алфавітом");

    std::map<std::string, double> before = {{"A", 1}, {"B", 2}, {"C", 3}};
    std::map<std::string, double> after = {{"A", 1}, {"B", 5}, {"D", 4}};
    check(changed_params(before, after) == std::vector<std::string>({"B", "C", "D"}), "7.4 B змінився, C зник, D новий");
    check(changed_params(before, before).empty(), "7.4 без змін");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
