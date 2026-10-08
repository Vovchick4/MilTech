#pragma once
// T10. Порядок облітання точок (задача комівояжера, евристики).
// Дрон стартує в start, має відвідати всі точки pts і повернутись у start.
// Повернути порядок обходу — перестановку індексів 0..n-1.
//
// Точний розв'язок — O(n!) або O(n²·2ⁿ), для 30+ точок нереально. Тому:
//   1) жадібно: щоразу летіти до найближчої невідвіданої точки (nearest neighbour), O(n²);
//   2) покращити 2-opt: якщо розворот відрізка маршруту скорочує його — розвернути; повторювати,
//      поки є покращення.
// Тести перевіряють, що маршрут — перестановка, не довший за жадібний, і що на простих
// конфігураціях знаходиться оптимум.
#include <cstddef>
#include <numeric>
#include <vector>
#include "dsa/geometry.hpp"

namespace tasks {

// Довжина замкненого маршруту start -> pts[order[0]] -> ... -> pts[order[n-1]] -> start
inline double route_length(dsa::Point2 start, const std::vector<dsa::Point2>& pts, const std::vector<std::size_t>& order)
{
    double len = 0.0;
    dsa::Point2 cur = start;
    for (std::size_t i : order) {
        len += dsa::distance(cur, pts[i]);
        cur = pts[i];
    }
    return len + dsa::distance(cur, start);
}

inline std::vector<std::size_t> plan_visit_order(dsa::Point2 start, const std::vector<dsa::Point2>& pts)
{
    (void)start;
    // TODO: зараз — просто в порядку списку
    std::vector<std::size_t> order(pts.size());
    std::iota(order.begin(), order.end(), 0);
    return order;
}

} // namespace tasks
