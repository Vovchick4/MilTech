#pragma once
// T05. Спрощення треку польоту (алгоритм Дугласа—Пекера).
// Лог GPS має тисячі точок; для відображення/передачі по радіоканалу лишаємо мінімум,
// так щоб жодна викинута точка не відхилялась від спрощеної лінії більше ніж на eps метрів.
//
// Алгоритм: для відрізка [first, last] знайти точку з максимальною відстанню до прямої.
// Якщо > eps — рекурсивно спростити дві половини, інакше лишити тільки first і last.
// Перша й остання точки завжди в результаті. Для pts.size() < 3 — повернути як є.
// Середня складність O(n log n), гірша O(n²).
#include <vector>
#include "dsa/geometry.hpp"

namespace tasks {

inline std::vector<dsa::Point2> simplify_track(const std::vector<dsa::Point2>& pts, double eps)
{
    (void)eps;
    // TODO
    return pts;
}

} // namespace tasks
