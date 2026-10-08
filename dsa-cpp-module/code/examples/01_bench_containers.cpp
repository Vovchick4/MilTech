// Приклад 1. Big-O — не вся правда: кеш процесора.
// vector vs list vs deque: обхід, вставка в середину, map vs unordered_map.
#include <cstdio>
#include <deque>
#include <list>
#include <map>
#include <numeric>
#include <random>
#include <unordered_map>
#include <vector>

#include "bench.hpp"

int main()
{
    constexpr int N = 1'000'000;
    std::vector<int> v(N);
    std::iota(v.begin(), v.end(), 0);
    std::list<int> l(v.begin(), v.end());
    std::deque<int> d(v.begin(), v.end());

    std::puts("== Обхід 1 000 000 елементів (обидва O(n)) ==");
    long long s = 0;
    std::printf("vector: %7.2f ms\n", bench::ms([&] { for (int x : v) s += x; }));
    std::printf("deque : %7.2f ms\n", bench::ms([&] { for (int x : d) s += x; }));
    std::printf("list  : %7.2f ms   <- вузли розкидані по пам'яті, кеш-промахи\n",
                bench::ms([&] { for (int x : l) s += x; }));
    bench::keep(s);

    std::puts("\n== 10 000 вставок у СЕРЕДИНУ (vector O(n), list O(1) — але треба ще дійти до середини) ==");
    {
        std::vector<int> vv(10'000, 1);
        std::list<int> ll(10'000, 1);
        std::printf("vector insert(mid):            %7.2f ms\n",
                    bench::ms([&] { for (int i = 0; i < 10'000; ++i) vv.insert(vv.begin() + vv.size() / 2, i); }));
        std::printf("list   advance(mid)+insert:    %7.2f ms\n", bench::ms([&] {
                        for (int i = 0; i < 10'000; ++i) {
                            auto it = ll.begin();
                            std::advance(it, ll.size() / 2);
                            ll.insert(it, i);
                        }
                    }));
    }

    std::puts("\n== 1 000 000 пошуків серед 100 000 ключів ==");
    std::map<int, int> m;
    std::unordered_map<int, int> um;
    for (int i = 0; i < 100'000; ++i) {
        m[i * 7] = i;
        um[i * 7] = i;
    }
    std::mt19937 rng(1);
    std::vector<int> keys(N);
    for (auto& k : keys) k = static_cast<int>(rng() % 700'000);
    long long hits = 0;
    std::printf("std::map           (O(log n)): %7.2f ms\n",
                bench::ms([&] { for (int k : keys) hits += m.count(k); }));
    std::printf("std::unordered_map (O(1)):     %7.2f ms\n",
                bench::ms([&] { for (int k : keys) hits += um.count(k); }));
    bench::keep(hits);

    std::puts("\nВисновок: у 9 з 10 випадків починай зі std::vector.");
    return 0;
}
