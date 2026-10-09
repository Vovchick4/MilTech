// Урок 6. Кільцевий буфер: «пам'ятати останні N вимірів».
//   g++ -std=c++17 -Wall example.cpp -o example && ./example

#include <iostream>

// Зберігає останні CAPACITY значень висоти. Старі значення перезаписуються новими.
class AltitudeHistory {
public:
    static const int CAPACITY = 5;

    // Додати новий вимір.
    void push(double value)
    {
        data[head] = value;           // пишемо туди, куди вказує head
        head = (head + 1) % CAPACITY; // head рухається вперед і «загортається» на 0 після кінця
        if (count < CAPACITY) {
            count = count + 1;        // поки не заповнили — кількість росте
        }
    }

    int size() const { return count; }

    // i = 0 — найстаріший збережений вимір, i = size()-1 — найновіший.
    double get(int i) const
    {
        int oldest = (head - count + CAPACITY) % CAPACITY;
        return data[(oldest + i) % CAPACITY];
    }

    // Надрукувати «сирий» масив і де зараз head — щоб бачити, як це працює всередині.
    void debug_print() const
    {
        std::cout << "data = [";
        for (int i = 0; i < CAPACITY; i++) {
            if (i < count || count == CAPACITY) std::cout << data[i];
            else std::cout << "_";
            if (i + 1 < CAPACITY) std::cout << ", ";
        }
        std::cout << "]  head=" << head << " count=" << count << "   по порядку: ";
        for (int i = 0; i < count; i++) {
            std::cout << get(i) << " ";
        }
        std::cout << "\n";
    }

private:
    double data[CAPACITY] = {0, 0, 0, 0, 0}; // масив фіксованого розміру: пам'ять НЕ росте
    int head = 0;                            // куди писати наступне значення
    int count = 0;                           // скільки значень реально збережено
};

int main()
{
    AltitudeHistory history;
    double measurements[] = {10, 20, 30, 40, 50, 60, 70, 80};

    for (int i = 0; i < 8; i++) {
        history.push(measurements[i]);
        std::cout << "push(" << measurements[i] << "):  ";
        history.debug_print();
    }

    std::cout << "\nМасив завжди з 5 комірок, скільки б вимірів не надійшло.\n";
    std::cout << "Найстаріший збережений: " << history.get(0)
              << ", найновіший: " << history.get(history.size() - 1) << "\n";
    return 0;
}
