#pragma once
// Спільні геометричні типи для задач. Площина в метрах: x = east, y = north.
#include <cmath>
#include <ostream>

namespace dsa {

struct Point2 {
    double x = 0.0;
    double y = 0.0;
    bool operator==(const Point2& o) const { return x == o.x && y == o.y; }
};

inline double distance(Point2 a, Point2 b) { return std::hypot(a.x - b.x, a.y - b.y); }

inline std::ostream& operator<<(std::ostream& os, Point2 p) { return os << '(' << p.x << ", " << p.y << ')'; }

} // namespace dsa
