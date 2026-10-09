// Урок 6. Вправи. Заповни TODO.
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises

#include <cmath>
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

bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }

class History {
public:
    static const int CAPACITY = 4;

    void push(double value)
    {
        data[head] = value;
        head = (head + 1) % CAPACITY;
        if (count < CAPACITY) {
            count = count + 1;
        }
    }

    int size() const { return count; }

    double get(int i) const
    {
        int oldest = (head - count + CAPACITY) % CAPACITY;
        return data[(oldest + i) % CAPACITY];
    }

    // 6.1 Чи буфер повністю заповнений?
    bool is_full() const
    {
        // TODO
        return false;
    }

    // 6.2 Середнє значення збережених вимірів. Якщо порожньо — 0.
    // Підказка: пройди циклом i від 0 до size() і використай get(i).
    double average() const
    {
        // TODO
        return 0.0;
    }

    // 6.3 Найбільше значення серед збережених. Буфер не порожній.
    double max_value() const
    {
        // TODO
        return 0.0;
    }

    // 6.4 Найновіший вимір. Буфер не порожній.
    double newest() const
    {
        // TODO
        return 0.0;
    }

private:
    double data[CAPACITY] = {0, 0, 0, 0};
    int head = 0;
    int count = 0;
};

// 6.5 (★) Пошук «глітчів» висоти.
// Ідемо по вимірах. Якщо в History вже є хоча б 3 виміри і новий вимір відрізняється
// від average() більше ніж на threshold — це глітч: запиши його НОМЕР і НЕ клади в історію.
// Інакше — поклади в історію.
std::vector<int> find_glitches(const std::vector<double>& heights, double threshold)
{
    std::vector<int> result;
    History h;
    // TODO
    (void)heights;
    (void)threshold;
    (void)h;
    return result;
}

int main()
{
    History h;
    check(!h.is_full(), "6.1 новий буфер не повний");
    check(near(h.average(), 0.0), "6.2 average порожнього = 0");

    h.push(10);
    h.push(20);
    h.push(30);
    check(!h.is_full(), "6.1 3 з 4 — ще не повний");
    check(near(h.average(), 20.0), "6.2 average(10,20,30) = 20");
    check(near(h.max_value(), 30.0), "6.3 max = 30");
    check(near(h.newest(), 30.0), "6.4 newest = 30");

    h.push(5);
    h.push(1); // 10 перезаписано
    check(h.is_full(), "6.1 повний");
    check(near(h.average(), (20.0 + 30 + 5 + 1) / 4), "6.2 average після перезапису");
    check(near(h.max_value(), 30.0), "6.3 max після перезапису");
    check(near(h.newest(), 1.0), "6.4 newest після перезапису");

    std::vector<double> flight = {50, 51, 50, 52, 95, 51, 50, 10, 49};
    check(find_glitches(flight, 15) == std::vector<int>({4, 7}), "6.5 глітчі на позиціях 4 і 7");
    check(find_glitches({1, 2, 3}, 0.1).empty(), "6.5 менше 4 вимірів — глітчів не шукаємо");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
