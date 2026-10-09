// Урок 3. Вправи. Заповни TODO.
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises

#include <algorithm>
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

// 3.1 Чи відсортований масив за зростанням (кожен елемент <= наступного)?
bool is_sorted_up(const std::vector<int>& v)
{
    // TODO
    (void)v;
    return false;
}

// 3.2 Бульбашка, але за СПАДАННЯМ (від більшого до меншого).
// Візьми bubble_sort з example.cpp і зміни одну умову.
void bubble_sort_down(std::vector<int>& v)
{
    // TODO
    (void)v;
}

// 3.3 Сортування вставками (як карти в руці — див. урок).
// Для i від 1 до кінця: беремо v[i] і зсуваємо вправо всі більші за нього зліва,
// поки не знайдемо місце.
void insertion_sort(std::vector<int>& v)
{
    // TODO
    (void)v;
}

// 3.4 Медіана — середнє за порядком значення. Кількість елементів НЕПАРНА.
// {7, 1, 5} -> відсортувати -> {1, 5, 7} -> 5.
// Не змінюй вхідний вектор (він const) — зроби копію.
int median(const std::vector<int>& v)
{
    // TODO
    (void)v;
    return 0;
}

struct Drone {
    std::string name;
    int battery;
};

// 3.5 Функція-порівняння: дрони з БІЛЬШИМ зарядом — раніше.
bool more_battery(const Drone& a, const Drone& b)
{
    // TODO
    (void)a;
    (void)b;
    return false;
}

int main()
{
    check(is_sorted_up({1, 2, 2, 5}) == true, "3.1 is_sorted_up так");
    check(is_sorted_up({1, 3, 2}) == false, "3.1 is_sorted_up ні");
    check(is_sorted_up({}) == true, "3.1 порожній вважається відсортованим");

    std::vector<int> a = {3, 9, 1, 7};
    bubble_sort_down(a);
    check(a == std::vector<int>({9, 7, 3, 1}), "3.2 bubble_sort_down");

    std::vector<int> b = {5, 2, 4, 6, 1, 3};
    insertion_sort(b);
    check(b == std::vector<int>({1, 2, 3, 4, 5, 6}), "3.3 insertion_sort");
    std::vector<int> b2 = {2, 2, 1};
    insertion_sort(b2);
    check(b2 == std::vector<int>({1, 2, 2}), "3.3 insertion_sort з повторами");

    std::vector<int> m = {7, 1, 5};
    check(median(m) == 5, "3.4 median");
    check(m == std::vector<int>({7, 1, 5}), "3.4 вхідний вектор не змінився");
    check(median({100, 1, 50, 2, 99}) == 50, "3.4 median з 5 елементів");

    std::vector<Drone> fleet = {{"A", 40}, {"B", 90}, {"C", 10}};
    std::sort(fleet.begin(), fleet.end(), more_battery);
    check(fleet[0].name == "B" && fleet[1].name == "A" && fleet[2].name == "C", "3.5 more_battery");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
