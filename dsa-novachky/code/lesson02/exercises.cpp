// Урок 2. Вправи. Заповни TODO.
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises

#include <iostream>
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

// 2.1 Найменше значення (заряд батареї у відсотках). Масив не порожній.
int min_value(const std::vector<int>& values)
{
    // TODO
    (void)values;
    return 0;
}

// 2.2 Скільки вимірів висоти перевищують limit (строго більше).
int count_above(const std::vector<double>& heights, double limit)
{
    // TODO
    (void)heights;
    (void)limit;
    return 0;
}

// 2.3 Номер ОСТАННЬОГО елемента, рівного x. Якщо немає — -1.
// Підказка: можна йти з кінця масиву до початку.
int last_index_of(const std::vector<int>& values, int x)
{
    // TODO
    (void)values;
    (void)x;
    return -1;
}

// 2.4 Новий вектор, у якому елементи йдуть у зворотному порядку.
// {1, 2, 3} -> {3, 2, 1}
std::vector<int> reversed(const std::vector<int>& values)
{
    std::vector<int> result;
    // TODO
    (void)values;
    return result;
}

// 2.5 Наскільки дрон піднявся або опустився за кожну секунду.
// {0, 5, 12, 10} -> {5, 7, -2}   (різниця сусідніх елементів; результат на 1 коротший)
std::vector<double> climb_per_second(const std::vector<double>& heights)
{
    std::vector<double> result;
    // TODO
    (void)heights;
    return result;
}

// 2.6 (★) Чи є в масиві два однакові числа? Підказка: порівняй кожен з кожним.
// Яка складність твого рішення? (урок 1)
bool has_duplicates(const std::vector<int>& values)
{
    // TODO
    (void)values;
    return false;
}

int main()
{
    check(min_value({57, 80, 12, 99}) == 12, "2.1 min_value");
    check(min_value({-3}) == -3, "2.1 min_value з одного елемента");

    check(count_above({10, 20, 30, 40}, 25) == 2, "2.2 count_above");
    check(count_above({10, 20}, 20) == 0, "2.2 count_above: рівне не рахується");

    check(last_index_of({5, 1, 5, 2}, 5) == 2, "2.3 last_index_of знайдено");
    check(last_index_of({5, 1, 5, 2}, 7) == -1, "2.3 last_index_of не знайдено");

    check(reversed({1, 2, 3}) == std::vector<int>({3, 2, 1}), "2.4 reversed");
    check(reversed({}).empty(), "2.4 reversed порожнього");

    check(climb_per_second({0, 5, 12, 10}) == std::vector<double>({5, 7, -2}), "2.5 climb_per_second");
    check(climb_per_second({42}).empty(), "2.5 один вимір -> порожньо");

    check(has_duplicates({3, 1, 4, 1, 5}) == true, "2.6 has_duplicates так");
    check(has_duplicates({3, 1, 4, 5}) == false, "2.6 has_duplicates ні");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
