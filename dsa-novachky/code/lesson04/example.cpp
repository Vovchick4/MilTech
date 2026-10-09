// Урок 4. Бінарний пошук — пошук у ВІДСОРТОВАНОМУ масиві діленням навпіл.
//   g++ -std=c++17 -Wall example.cpp -o example && ./example

#include <iostream>
#include <vector>

// ------------------------------------------------------------------
// Гра «Вгадай число» від 1 до 100. Комп'ютер вгадує, діленням навпіл.
// ------------------------------------------------------------------
void guess_game(int secret)
{
    int low = 1, high = 100;
    int attempt = 0;
    std::cout << "Загадане число: " << secret << "\n";
    while (low <= high) {
        int guess = (low + high) / 2;
        attempt = attempt + 1;
        std::cout << "  спроба " << attempt << ": діапазон [" << low << ".." << high << "], пробую " << guess;
        if (guess == secret) {
            std::cout << " -> ВГАДАВ!\n";
            return;
        } else if (guess < secret) {
            std::cout << " -> більше\n";
            low = guess + 1;  // ліва половина вже не цікава
        } else {
            std::cout << " -> менше\n";
            high = guess - 1; // права половина вже не цікава
        }
    }
}

// ------------------------------------------------------------------
// Бінарний пошук у відсортованому векторі. Повертає індекс або -1.
// show = true — друкувати кожен крок.
// ------------------------------------------------------------------
int binary_search(const std::vector<int>& v, int x, bool show)
{
    int low = 0;
    int high = (int)v.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2; // те саме, що (low+high)/2, але без переповнення
        if (show) {
            std::cout << "  low=" << low << " high=" << high << " mid=" << mid << " v[mid]=" << v[mid] << "\n";
        }
        if (v[mid] == x) {
            return mid;
        } else if (v[mid] < x) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

int linear_search_steps(const std::vector<int>& v, int x)
{
    int steps = 0;
    for (int i = 0; i < (int)v.size(); i++) {
        steps = steps + 1;
        if (v[i] == x) {
            break;
        }
    }
    return steps;
}

int binary_search_steps(const std::vector<int>& v, int x)
{
    int steps = 0;
    int low = 0, high = (int)v.size() - 1;
    while (low <= high) {
        steps = steps + 1;
        int mid = low + (high - low) / 2;
        if (v[mid] == x) break;
        if (v[mid] < x) low = mid + 1;
        else high = mid - 1;
    }
    return steps;
}

int main()
{
    guess_game(73);

    // Час (секунди від старту), коли дрон пролітав waypoint'и. Вже відсортовано!
    std::vector<int> times = {0, 12, 25, 31, 47, 58, 66, 80, 95, 120};
    std::cout << "\nШукаємо 66 у {0 12 25 31 47 58 66 80 95 120}:\n";
    int idx = binary_search(times, 66, true);
    std::cout << "  знайдено на позиції " << idx << "\n";

    std::cout << "\nШукаємо 50 (його немає):\n";
    int not_found = binary_search(times, 50, true);
    std::cout << "  результат " << not_found << " (low став більшим за high — шукати ніде)\n";

    std::vector<int> big;
    for (int i = 0; i < 1000000; i++) {
        big.push_back(i * 2); // 0, 2, 4, ... — відсортовано
    }
    int target = 1999998; // останній елемент — найгірший випадок для лінійного пошуку
    std::cout << "\n1 000 000 елементів, шукаємо останній:\n";
    std::cout << "  лінійний пошук: " << linear_search_steps(big, target) << " кроків\n";
    std::cout << "  бінарний пошук: " << binary_search_steps(big, target) << " кроків\n";
    return 0;
}
