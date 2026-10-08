#pragma once
// T02. k найближчих майданчиків для посадки.
// Дано координати майданчиків pts і поточну позицію from. Повернути k найближчих,
// відсортованих за зростанням відстані (при рівності — у порядку появи в pts).
// Якщо k >= pts.size() — усі точки, відсортовані.
//
// Вимога: O(n log k) — купа розміру k (std::priority_queue або dsa::BinaryHeap),
// а не сортування всього масиву O(n log n).
#include <cstddef>
#include <vector>
#include "dsa/geometry.hpp"

namespace tasks {

inline std::vector<dsa::Point2> k_closest(const std::vector<dsa::Point2>& pts, dsa::Point2 from, std::size_t k)
{
    (void)pts;
    (void)from;
    (void)k;
    // TODO
    return {};
}

} // namespace tasks
