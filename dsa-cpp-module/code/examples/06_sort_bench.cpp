// Приклад 6. O(n²) vs O(n log n) на практиці + чому quick sort з поганим pivot небезпечний.
#include <algorithm>
#include <cstdio>
#include <random>
#include <vector>

#include "bench.hpp"
#include "dsa/sort.hpp"

int main()
{
    std::mt19937 rng(11);
    std::puts("     n | insertion |   merge   |   quick   | std::sort");
    std::puts("-------+-----------+-----------+-----------+----------");
    for (int n : {1'000, 10'000, 30'000, 100'000, 1'000'000}) {
        std::vector<int> base(n);
        for (auto& x : base) x = static_cast<int>(rng());

        auto a = base, b = base, c = base, d = base;
        char ins[16] = "     —";
        if (n <= 30'000) std::snprintf(ins, sizeof ins, "%7.2f ms", bench::ms([&] { dsa::insertion_sort(a); }));
        const double tm = bench::ms([&] { dsa::merge_sort(b); });
        const double tq = bench::ms([&] { dsa::quick_sort(c); });
        const double ts = bench::ms([&] { std::sort(d.begin(), d.end()); });
        std::printf("%6d | %9s | %6.2f ms | %6.2f ms | %6.2f ms\n", n, ins, tm, tq, ts);
    }

    std::puts("\nВже відсортований масив 1 000 000 (найгірший випадок для наївного quick sort):");
    std::vector<int> sorted(1'000'000);
    for (int i = 0; i < 1'000'000; ++i) sorted[i] = i;
    auto s1 = sorted;
    std::printf("  dsa::quick_sort (медіана з трьох): %.2f ms\n", bench::ms([&] { dsa::quick_sort(s1); }));
    std::puts("  (з pivot = перший елемент це було б ~n²/2 = 5·10^11 порівнянь і переповнення стека)");
    return 0;
}
