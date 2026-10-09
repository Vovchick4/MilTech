// Урок 1. Вправи.
// Заповни місця з TODO. Запуск:
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises
// Мета: усі рядки [OK].

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

// Вправа 1.1
// Порахуй, скільки разів виконається тіло циклу:
//   for (int i = 0; i < n; i = i + 2) { ... }
// Напиши функцію, яка ЦИКЛОМ рахує ці кроки (не формулою).
int count_every_second(int n)
{
    int steps = 0;
    // TODO: напиши такий самий цикл і збільшуй steps у тілі
    (void)n;
    return steps;
}

// Вправа 1.2
// Дрон має n точок маршруту. Для кожної точки ми порівнюємо її з УСІМА точками
// (включно з нею самою). Скільки буде порівнянь? Порахуй вкладеними циклами.
int count_all_with_all(int n)
{
    int steps = 0;
    // TODO: два вкладені цикли від 0 до n
    (void)n;
    return steps;
}

// Вправа 1.3
// Скільки разів треба помножити 1 на 2, щоб отримати число >= n?
// (1 -> 2 -> 4 -> 8 -> ...). Наприклад, для n = 8 відповідь 3, для n = 9 відповідь 4.
int doubling_steps(int n)
{
    int steps = 0;
    // TODO: value = 1; поки value < n: value = value * 2, steps + 1
    (void)n;
    return steps;
}

// Вправа 1.4 (без коду — заміни -1 на свою відповідь)
// Програма обробляє 1000 точок за 1 секунду, і її алгоритм — O(n^2).
// Скільки секунд вона оброблятиме 10 000 точок?
int answer_1_4 = -1;

int main()
{
    check(count_every_second(10) == 5, "1.1 count_every_second(10) == 5");
    check(count_every_second(7) == 4, "1.1 count_every_second(7) == 4");
    check(count_all_with_all(5) == 25, "1.2 count_all_with_all(5) == 25");
    check(count_all_with_all(100) == 10000, "1.2 count_all_with_all(100) == 10000");
    check(doubling_steps(8) == 3, "1.3 doubling_steps(8) == 3");
    check(doubling_steps(9) == 4, "1.3 doubling_steps(9) == 4");
    check(doubling_steps(1000000) == 20, "1.3 doubling_steps(1000000) == 20");
    check(answer_1_4 == 100, "1.4 відповідь про O(n^2)");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
