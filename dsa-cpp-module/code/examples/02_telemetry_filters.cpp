// Приклад 2. Кільцевий буфер + фільтри на «шумній» висоті з бароміра.
// Симулюємо: дрон набирає висоту 0 -> 50 м, шум ±0.5 м, рідкісні стрибки +40 м (глітчі).
#include <cmath>
#include <cstdio>
#include <random>

#include "dsa/filters.hpp"
#include "dsa/ring_buffer.hpp"

int main()
{
    std::mt19937 rng(7);
    std::normal_distribution<double> noise(0.0, 0.5);
    std::uniform_int_distribution<int> glitch(0, 30);

    dsa::RingBuffer<double, 10> last10; // історія останніх 10 вимірів
    dsa::MovingAverage<5> avg;
    dsa::MedianFilter<5> med;
    dsa::MovingMin<20> min20;

    std::puts("  t | true  | raw    | avg5   | median5 | min20");
    std::puts("----+-------+--------+--------+---------+------");
    double err_avg = 0, err_med = 0;
    for (int t = 0; t < 60; ++t) {
        const double truth = t < 50 ? t * 1.0 : 50.0;
        double raw = truth + noise(rng);
        if (glitch(rng) == 0) raw += 40.0; // ~3% вимірів — стрибок

        last10.push(raw);
        const double a = avg.update(raw);
        const double m = med.update(raw);
        const double mn = min20.update(raw);
        err_avg += std::abs(a - truth);
        err_med += std::abs(m - truth);

        std::printf("%3d | %5.1f | %6.1f | %6.1f | %7.1f | %5.1f%s\n", t, truth, raw, a, m, mn,
                    raw - truth > 10 ? "   <- glitch" : "");
    }
    std::printf("\nСередня похибка: avg5 = %.2f м, median5 = %.2f м\n", err_avg / 60, err_med / 60);
    std::printf("Останні %zu сирих вимірів у RingBuffer: ", last10.size());
    for (std::size_t i = 0; i < last10.size(); ++i) std::printf("%.1f ", last10[i]);
    std::puts("");
    std::puts("\nСереднє «відстає» на наборі висоти і розмазує стрибки; медіана стрибки прибирає.");
    return 0;
}
