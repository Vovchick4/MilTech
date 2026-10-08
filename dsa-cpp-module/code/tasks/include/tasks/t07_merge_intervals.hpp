#pragma once
// T07. Об'єднання часових вікон.
// Є список інтервалів [start, end] (секунди), коли повітряний простір закрито.
// Повернути об'єднані непересічні інтервали, відсортовані за start.
// Інтервали, що торкаються ([1,3] і [3,5]), теж об'єднуються.
// Вимога: O(n log n).
#include <vector>

namespace tasks {

struct Interval {
    int start;
    int end;
    bool operator==(const Interval& o) const { return start == o.start && end == o.end; }
};

inline std::vector<Interval> merge_intervals(std::vector<Interval> v)
{
    (void)v;
    // TODO
    return {};
}

} // namespace tasks
