// Урок 2. Масиви (std::vector): прохід, сума, середнє, мінімум/максимум, пошук.
// Дані — висота дрона (метри), виміряна раз на секунду.
//
//   g++ -std=c++17 -Wall example.cpp -o example && ./example

#include <iostream>
#include <vector>

// Сума всіх висот. Передаємо vector «за посиланням» (&), щоб не копіювати його,
// і з const — функція обіцяє нічого в ньому не змінювати.
double sum_of(const std::vector<double>& heights)
{
    double sum = 0.0;
    for (int i = 0; i < (int)heights.size(); i++) {
        sum = sum + heights[i];
    }
    return sum;
}

double average_of(const std::vector<double>& heights)
{
    if (heights.empty()) {
        return 0.0; // ділити на 0 не можна!
    }
    return sum_of(heights) / heights.size();
}

// Індекс (номер) найбільшого елемента.
int index_of_max(const std::vector<double>& heights)
{
    int best = 0; // припускаємо, що найбільший — перший
    for (int i = 1; i < (int)heights.size(); i++) {
        if (heights[i] > heights[best]) {
            best = i; // знайшли більший — запам'ятали ЙОГО НОМЕР
        }
    }
    return best;
}

// Лінійний пошук: номер ПЕРШОЇ секунди, коли висота стала >= limit.
// Якщо такої немає — повертаємо -1 (домовленість «не знайдено»).
int first_time_above(const std::vector<double>& heights, double limit)
{
    for (int i = 0; i < (int)heights.size(); i++) {
        if (heights[i] >= limit) {
            return i; // знайшли — одразу виходимо, далі шукати не треба
        }
    }
    return -1;
}

int main()
{
    std::vector<double> heights = {0.0, 2.5, 6.0, 11.2, 18.4, 25.0, 30.1, 29.8, 30.2, 30.0};

    std::cout << "Висоти по секундах:\n";
    for (int i = 0; i < (int)heights.size(); i++) {
        std::cout << "  t=" << i << " c  ->  " << heights[i] << " м\n";
    }

    std::cout << "\nКількість вимірів: " << heights.size() << "\n";
    std::cout << "Сума:              " << sum_of(heights) << "\n";
    std::cout << "Середня висота:    " << average_of(heights) << " м\n";

    int imax = index_of_max(heights);
    std::cout << "Максимум:          " << heights[imax] << " м на секунді " << imax << "\n";

    std::cout << "Вперше >= 25 м:    секунда " << first_time_above(heights, 25.0) << "\n";
    std::cout << "Вперше >= 100 м:   " << first_time_above(heights, 100.0) << "  (-1 = ніколи)\n";

    // Додавання в кінець
    heights.push_back(29.9);
    std::cout << "\nПісля push_back: розмір = " << heights.size()
              << ", останній = " << heights.back() << " м\n";
    return 0;
}
