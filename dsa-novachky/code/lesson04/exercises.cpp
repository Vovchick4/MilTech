// Урок 4. Вправи. Заповни TODO.
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

// 4.1 Бінарний пошук: індекс x у відсортованому v або -1.
// Спробуй написати САМ, не підглядаючи в example.cpp.
int find_sorted(const std::vector<int>& v, int x)
{
    // TODO
    (void)v;
    (void)x;
    return -1;
}

// 4.2 Скільки максимум спроб треба в грі «Вгадай число» від 1 до n?
// Це скільки разів n можна ділити навпіл, поки не стане 0:
//   100 -> 50 -> 25 -> 12 -> 6 -> 3 -> 1 -> 0   = 7
int max_guesses(int n)
{
    // TODO
    (void)n;
    return 0;
}

// 4.3 (★) Перший waypoint, час якого >= t.
// times відсортований. Повернути ІНДЕКС. Якщо всі менші за t — повернути times.size().
// {0, 12, 25, 31}, t = 20 -> 2 (бо 25 >= 20)
// {0, 12, 25, 31}, t = 25 -> 2
// {0, 12, 25, 31}, t = 99 -> 4
// Підказка: якщо v[mid] >= t — відповідь mid або лівіше (high = mid),
//           інакше — точно правіше (low = mid + 1). Цикл while (low < high), high = size.
int first_at_least(const std::vector<int>& times, int t)
{
    // TODO
    (void)times;
    (void)t;
    return 0;
}

// 4.4 (★★) Ціла частина квадратного кореня БЕЗ sqrt(): найбільше k, для якого k*k <= n.
// int_sqrt(17) = 4, int_sqrt(16) = 4, int_sqrt(0) = 0.
// Ідея: бінарний пошук відповіді k у діапазоні [0, n]. Обережно: k*k може не влізти в int —
// використай long long.
int int_sqrt(int n)
{
    // TODO
    (void)n;
    return 0;
}

int main()
{
    std::vector<int> v = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    check(find_sorted(v, 23) == 5, "4.1 знайти 23");
    check(find_sorted(v, 2) == 0, "4.1 знайти перший");
    check(find_sorted(v, 91) == 9, "4.1 знайти останній");
    check(find_sorted(v, 7) == -1, "4.1 немає 7");
    check(find_sorted({}, 7) == -1, "4.1 порожній масив");

    check(max_guesses(100) == 7, "4.2 max_guesses(100) == 7");
    check(max_guesses(1000000) == 20, "4.2 max_guesses(1000000) == 20");

    std::vector<int> t = {0, 12, 25, 31};
    check(first_at_least(t, 20) == 2, "4.3 t = 20");
    check(first_at_least(t, 25) == 2, "4.3 t = 25");
    check(first_at_least(t, -5) == 0, "4.3 t = -5");
    check(first_at_least(t, 99) == 4, "4.3 t = 99");

    check(int_sqrt(17) == 4, "4.4 int_sqrt(17)");
    check(int_sqrt(16) == 4, "4.4 int_sqrt(16)");
    check(int_sqrt(0) == 0, "4.4 int_sqrt(0)");
    check(int_sqrt(2000000000) == 44721, "4.4 int_sqrt(2 000 000 000)");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
