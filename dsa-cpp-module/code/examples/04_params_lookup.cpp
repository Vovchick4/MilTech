// Приклад 4. Пошук параметра за іменем серед ~1000 (як у ArduPilot ~1200 параметрів).
// Лінійний пошук O(n) vs відсортований масив + бінарний пошук O(log n) vs хеш-таблиця O(1).
#include <algorithm>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include "bench.hpp"
#include "dsa/hash_map.hpp"
#include "dsa/sort.hpp"

struct Param {
    std::string name;
    float value;
    bool operator<(const Param& o) const { return name < o.name; }
};

int main()
{
    const char* groups[] = {"ATC_", "WPNAV_", "BATT_", "EK3_", "SERVO", "RC", "FS_", "INS_", "COMPASS_", "SIM_"};
    std::vector<Param> params;
    for (int i = 0; i < 1000; ++i) params.push_back({std::string(groups[i % 10]) + "P" + std::to_string(i), float(i)});

    std::vector<Param> sorted = params;
    dsa::quick_sort(sorted);

    dsa::HashMap<std::string, float> table;
    for (const auto& p : params) table.insert_or_assign(p.name, p.value);

    std::mt19937 rng(3);
    std::vector<std::string> queries;
    for (int i = 0; i < 200'000; ++i) queries.push_back(params[rng() % params.size()].name);

    double sum = 0;
    std::printf("200 000 пошуків серед %zu параметрів:\n", params.size());
    std::printf("  linear search  O(n)     : %8.2f ms\n", bench::ms([&] {
                    for (const auto& q : queries)
                        for (const auto& p : params)
                            if (p.name == q) { sum += p.value; break; }
                }));
    std::printf("  binary search  O(log n) : %8.2f ms\n", bench::ms([&] {
                    for (const auto& q : queries) sum += sorted[dsa::lower_bound_index(sorted, Param{q, 0})].value;
                }));
    std::printf("  hash map       O(1)     : %8.2f ms\n", bench::ms([&] {
                    for (const auto& q : queries) sum += *table.find(q);
                }));
    bench::keep(sum);

    std::printf("\nХеш-таблиця: %zu ключів у %zu слотах, load factor %.2f, кроків пробінгу для \"%s\": %zu\n",
                table.size(), table.capacity(), double(table.size()) / table.capacity(),
                params[42].name.c_str(), table.probe_length(params[42].name));
    std::puts("ArduPilot насправді: параметри в масиві, групи — дерево; пошук за іменем рідкісний (тільки з GCS).");
    return 0;
}
