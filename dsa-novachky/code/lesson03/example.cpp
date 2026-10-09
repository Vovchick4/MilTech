// Урок 3. Сортування: бульбашка, вибір, std::sort, сортування структур.
//   g++ -std=c++17 -Wall example.cpp -o example && ./example

#include <algorithm> // std::sort, std::swap
#include <iostream>
#include <string>
#include <vector>

void print(const std::vector<int>& v)
{
    for (int i = 0; i < (int)v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
}

// ------------------------------------------------------------------
// Сортування бульбашкою: порівнюємо СУСІДІВ і міняємо, якщо стоять не так.
// Після кожного проходу найбільший елемент «спливає» в кінець.
// ------------------------------------------------------------------
void bubble_sort(std::vector<int>& v) // без const: функція ЗМІНЮЄ вектор
{
    int n = (int)v.size();
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        for (int i = 0; i < n - 1 - pass; i++) {   // останні pass елементів уже на місці
            if (v[i] > v[i + 1]) {
                std::swap(v[i], v[i + 1]);          // поміняти місцями
                swapped = true;
            }
        }
        std::cout << "  після проходу " << pass + 1 << ": ";
        print(v);
        if (!swapped) {
            std::cout << "  обмінів не було -> вже відсортовано, зупиняємось\n";
            break;
        }
    }
}

// ------------------------------------------------------------------
// Сортування вибором: знаходимо найменший серед «невідсортованих»
// і ставимо його на чергове місце зліва.
// ------------------------------------------------------------------
void selection_sort(std::vector<int>& v)
{
    int n = (int)v.size();
    for (int place = 0; place < n - 1; place++) {
        int min_index = place;
        for (int i = place + 1; i < n; i++) {
            if (v[i] < v[min_index]) {
                min_index = i;
            }
        }
        std::swap(v[place], v[min_index]);
        std::cout << "  місце " << place << " <- " << v[place] << ":   ";
        print(v);
    }
}

// ------------------------------------------------------------------
// Сортування структур: дрони за зарядом батареї.
// ------------------------------------------------------------------
struct Drone {
    std::string name;
    int battery; // %
};

// Функція-порівняння для std::sort: «чи має a стояти РАНІШЕ за b?»
bool less_battery(const Drone& a, const Drone& b)
{
    return a.battery < b.battery;
}

int main()
{
    std::vector<int> a = {64, 25, 12, 22, 11};
    std::cout << "Бульбашка, початок: ";
    print(a);
    bubble_sort(a);

    std::vector<int> b = {64, 25, 12, 22, 11};
    std::cout << "\nВибір, початок: ";
    print(b);
    selection_sort(b);

    std::vector<int> c = {5, 3, 9, 1, 7, 3};
    std::sort(c.begin(), c.end()); // готове сортування зі стандартної бібліотеки
    std::cout << "\nstd::sort: ";
    print(c);

    std::vector<Drone> fleet = {{"Alpha", 76}, {"Bravo", 23}, {"Charlie", 91}, {"Delta", 45}};
    std::sort(fleet.begin(), fleet.end(), less_battery);
    std::cout << "\nДрони від найменшого заряду (кого першим на зарядку):\n";
    for (int i = 0; i < (int)fleet.size(); i++) {
        std::cout << "  " << fleet[i].name << ": " << fleet[i].battery << "%\n";
    }
    return 0;
}
