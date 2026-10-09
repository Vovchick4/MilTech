// Урок 1. Як «виміряти» швидкість алгоритму — рахуємо кроки.
//
// Компіляція і запуск (WSL Ubuntu / Linux / macOS terminal):
//   g++ -std=c++17 -Wall example.cpp -o example
//   ./example

#include <iostream>
#include <vector>

// ---------------------------------------------------------------
// 1) Взяти перший елемент — 1 крок, скільки б елементів не було.
// ---------------------------------------------------------------
int first_element_steps(const std::vector<int>& values)
{
    int steps = 0;
    int first = values[0];
    steps = steps + 1;
    (void)first; // значення нам не потрібне, рахуємо тільки кроки
    return steps;
}

// ---------------------------------------------------------------
// 2) Сума всіх елементів — один прохід по масиву: n кроків.
// ---------------------------------------------------------------
int sum_steps(const std::vector<int>& values)
{
    int steps = 0;
    int sum = 0;
    for (int i = 0; i < (int)values.size(); i++) {
        sum = sum + values[i];
        steps = steps + 1;
    }
    (void)sum;
    return steps;
}

// ---------------------------------------------------------------
// 3) Перевірити всі ПАРИ дронів (чи не надто близько один до одного).
//    Цикл у циклі: приблизно n * n / 2 кроків.
// ---------------------------------------------------------------
int pairs_steps(const std::vector<int>& values)
{
    int steps = 0;
    for (int i = 0; i < (int)values.size(); i++) {
        for (int j = i + 1; j < (int)values.size(); j++) {
            steps = steps + 1; // тут була б перевірка відстані між дроном i і дроном j
        }
    }
    return steps;
}

// ---------------------------------------------------------------
// 4) Скільки разів можна поділити n навпіл, поки не залишиться 1.
//    Так працює бінарний пошук (урок 4): дуже мало кроків.
// ---------------------------------------------------------------
int halving_steps(int n)
{
    int steps = 0;
    while (n > 1) {
        n = n / 2;
        steps = steps + 1;
    }
    return steps;
}

int main()
{
    std::cout << "     n | перший | сума (n) | пари (n*n/2) | ділення навпіл\n";
    std::cout << "-------+--------+----------+--------------+---------------\n";

    int sizes[] = {10, 100, 1000, 10000};
    for (int k = 0; k < 4; k++) {
        int n = sizes[k];
        std::vector<int> data(n, 1); // n елементів, кожен = 1

        // width(6) — «надрукуй наступне значення в колонці шириною 6 символів»
        std::cout.width(6);  std::cout << n << " | ";
        std::cout.width(6);  std::cout << first_element_steps(data) << " | ";
        std::cout.width(8);  std::cout << sum_steps(data) << " | ";
        std::cout.width(12); std::cout << pairs_steps(data) << " | ";
        std::cout.width(6);  std::cout << halving_steps(n) << "\n";
    }

    std::cout << "\nДаних стало в 10 разів більше:\n";
    std::cout << "  'перший'        - кроків стільки ж        -> O(1)\n";
    std::cout << "  'сума'          - кроків у 10 разів більше  -> O(n)\n";
    std::cout << "  'пари'          - кроків у 100 разів більше -> O(n^2)\n";
    std::cout << "  'ділення навпіл'- кроків на 3-4 більше      -> O(log n)\n";
    return 0;
}
