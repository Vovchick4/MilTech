// Урок 9. Підсумковий міні-проєкт: «Бортовий журнал польоту».
// Кожна функція — завдання з одного з уроків. Заповни TODO, щоб усі перевірки стали [OK],
// а внизу надрукувався звіт про політ.
//
//   g++ -std=c++17 -Wall project.cpp -o project && ./project

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <queue>
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

struct LogEntry {
    int time;        // секунди від старту (записи йдуть за зростанням часу!)
    double altitude; // м
    int battery;     // %
    std::string mode;
};

// Журнал: запис кожні 5 секунд.
std::vector<LogEntry> make_log()
{
    return {
        {0, 0.0, 100, "STABILIZE"}, {5, 0.0, 100, "GUIDED"},  {10, 8.0, 98, "GUIDED"},
        {15, 19.5, 96, "GUIDED"},   {20, 30.2, 94, "GUIDED"}, {25, 30.0, 92, "AUTO"},
        {30, 31.0, 90, "AUTO"},     {35, 30.4, 88, "AUTO"},   {40, 92.0, 86, "AUTO"},   // 92 м — глітч бароміра
        {45, 30.1, 84, "AUTO"},     {50, 29.9, 82, "AUTO"},   {55, 30.3, 80, "AUTO"},
        {60, 30.0, 77, "RTL"},      {65, 30.0, 74, "RTL"},    {70, 30.0, 71, "RTL"},
        {75, 22.0, 69, "LAND"},     {80, 12.0, 67, "LAND"},   {85, 4.0, 66, "LAND"},
        {90, 0.0, 65, "LAND"},      {95, 0.0, 65, "LAND"},
    };
}

// П1 (урок 2). Номер запису з найбільшою висотою.
int index_of_max_altitude(const std::vector<LogEntry>& log)
{
    // TODO
    (void)log;
    return 0;
}

// П2 (урок 2). Скільки записів, де батарея < limit.
int count_battery_below(const std::vector<LogEntry>& log, int limit)
{
    // TODO
    (void)log;
    (void)limit;
    return 0;
}

// П3 (урок 3). Висоти, відсортовані за зростанням (для медіани).
// Медіана для парної кількості — середнє двох середніх елементів.
double median_altitude(const std::vector<LogEntry>& log)
{
    // TODO: збери висоти в std::vector<double>, std::sort, візьми середину
    (void)log;
    return 0.0;
}

// П4 (урок 4). Що відбувалось у момент t? Повернути індекс ПЕРШОГО запису з time >= t
// (або log.size(), якщо такого немає). Обов'язково БІНАРНИМ пошуком.
int entry_at_time(const std::vector<LogEntry>& log, int t)
{
    // TODO
    (void)log;
    (void)t;
    return 0;
}

// П5 (урок 5). Послідовність режимів БЕЗ повторів підряд, у порядку зміни:
// STABILIZE, GUIDED, AUTO, RTL, LAND. Потім — у ЗВОРОТНОМУ порядку через std::stack
// (як «розмотати» історію назад).
std::vector<std::string> mode_changes_reversed(const std::vector<LogEntry>& log)
{
    std::vector<std::string> result;
    // TODO
    (void)log;
    return result;
}

// П6 (урок 6 + 2). Знайти глітчі висоти: запис — глітч, якщо його висота відрізняється
// від середнього ПОПЕРЕДНІХ 3 НЕ-глітчевих записів більше ніж на 25 м. Перші 3 записи не перевіряємо.
// Повернути час (time) глітчів.
std::vector<int> glitch_times(const std::vector<LogEntry>& log)
{
    std::vector<int> result;
    // TODO: можна взяти клас History з уроку 6 (CAPACITY = 3) або вектор останніх 3 значень
    (void)log;
    return result;
}

// П7 (урок 7). Скільки секунд у кожному режимі. Кожен запис = 5 секунд.
std::map<std::string, int> seconds_per_mode(const std::vector<LogEntry>& log)
{
    std::map<std::string, int> result;
    // TODO
    (void)log;
    return result;
}

// П8 (урок 8). Найкоротший шлях повернення (кроків) по карті з 'H' (дім) до 'D' (дрон).
// Знайди 'H' і 'D' на карті самостійно. Якщо шляху немає — -1.
int return_steps(const std::vector<std::string>& map)
{
    // TODO
    (void)map;
    return -1;
}

int main()
{
    std::vector<LogEntry> log = make_log();

    check(index_of_max_altitude(log) == 8, "П1 максимальна висота — запис 8 (той самий глітч)");
    check(count_battery_below(log, 70) == 5, "П2 батарея < 70% — 5 записів");
    check(std::fabs(median_altitude(log) - 29.95) < 1e-9, "П3 медіана висоти 29.95 м (глітч їй не заважає)");
    check(entry_at_time(log, 33) == 7, "П4 t = 33 -> запис 7 (t = 35)");
    check(entry_at_time(log, 40) == 8, "П4 t = 40 -> запис 8");
    check(entry_at_time(log, 1000) == (int)log.size(), "П4 t = 1000 -> log.size()");
    check(mode_changes_reversed(log) == std::vector<std::string>({"LAND", "RTL", "AUTO", "GUIDED", "STABILIZE"}),
          "П5 режими у зворотному порядку");
    check(glitch_times(log) == std::vector<int>({40}), "П6 глітч на 40-й секунді");
    std::map<std::string, int> sec = seconds_per_mode(log);
    check(sec.size() == 5 && sec["AUTO"] == 35 && sec["LAND"] == 25 && sec["STABILIZE"] == 5,
          "П7 секунди в режимах");

    std::vector<std::string> map = {
        "H..#......",
        ".#.#.####.",
        ".#...#..#.",
        ".####.#.#D",
        "......#...",
    };
    check(return_steps(map) == 16, "П8 шлях додому 16 кроків");
    check(return_steps({"H#D"}) == -1, "П8 шляху немає");

    std::cout << "\nПомилок: " << g_failed << "\n";
    if (g_failed == 0) {
        std::cout << "\n========== ЗВІТ ПРО ПОЛІТ ==========\n";
        std::cout << "Тривалість: " << log.back().time << " с\n";
        std::cout << "Медіана висоти: " << median_altitude(log) << " м\n";
        std::cout << "Заряд наприкінці: " << log.back().battery << "%\n";
        std::cout << "Глітчів бароміра: " << glitch_times(log).size() << "\n";
        for (const auto& item : sec) {
            std::cout << "  " << item.first << ": " << item.second << " с\n";
        }
        std::cout << "Шлях додому по карті: " << return_steps(map) << " клітинок\n";
    }
    return g_failed == 0 ? 0 : 1;
}
