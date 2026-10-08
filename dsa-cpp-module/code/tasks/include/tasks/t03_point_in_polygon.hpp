#pragma once
// T03. Геозона: чи всередині багатокутника точка?
// poly — вершини багатокутника (без повтору першої в кінці), може бути неопуклим.
// Повернути true, якщо p строго всередині; для точок на межі — будь-яка відповідь.
//
// Алгоритм: ray casting — пускаємо промінь з p вправо і рахуємо перетини з ребрами.
// Непарна кількість -> всередині. O(n).
#include <vector>
#include "dsa/geometry.hpp"

namespace tasks {

inline bool point_in_polygon(const std::vector<dsa::Point2>& poly, dsa::Point2 p)
{
    (void)poly;
    (void)p;
    // TODO
    return false;
}

} // namespace tasks
