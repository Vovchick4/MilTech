#pragma once
// T06. Система неперетинних множин (Union-Find / DSU) + мінімальне кістякове дерево (Kruskal).
// Задача: є N наземних ретрансляторів і можливі радіолінки між ними з «вартістю» (відстань).
// Потрібно з'єднати всі станції мережею мінімальної сумарної вартості.
//
// UnionFind: find з path compression, unite з union by size -> майже O(1) амортизовано.
// mst_kruskal: відсортувати ребра за вагою, додавати ребро, якщо воно з'єднує різні компоненти.
// Повернути сумарну вагу MST, або -1.0, якщо граф незв'язний.
#include <cstddef>
#include <vector>

namespace tasks {

class UnionFind {
public:
    explicit UnionFind(std::size_t n) { (void)n; /* TODO */ }

    std::size_t find(std::size_t x)
    {
        // TODO
        return x;
    }

    // true, якщо x і y були в різних множинах і їх об'єднано
    bool unite(std::size_t x, std::size_t y)
    {
        (void)x;
        (void)y;
        // TODO
        return false;
    }

    std::size_t components() const
    {
        // TODO
        return 0;
    }
};

struct Edge {
    std::size_t a, b;
    double w;
};

inline double mst_kruskal(std::size_t n, std::vector<Edge> edges)
{
    (void)n;
    (void)edges;
    // TODO
    return -1.0;
}

} // namespace tasks
