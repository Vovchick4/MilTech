#pragma once
// T09. Порядок передпольотних перевірок (топологічне сортування).
// n перевірок, пари (a, b) означають «a треба виконати ДО b»
// (наприклад: «увімкнути живлення» до «калібрування компаса»).
// Повернути порядок виконання. Якщо залежності циклічні — std::nullopt.
// Якщо можливих порядків кілька — серед готових перевірок спочатку брати МЕНШИЙ номер
// (тоді відповідь однозначна).
//
// Алгоритм Кана: рахуємо вхідні степені, у чергу (тут — min-heap) кладемо вершини зі степенем 0. O((V+E) log V).
#include <optional>
#include <utility>
#include <vector>

namespace tasks {

inline std::optional<std::vector<int>> topo_sort(int n, const std::vector<std::pair<int, int>>& deps)
{
    (void)n;
    (void)deps;
    // TODO
    return std::nullopt;
}

} // namespace tasks
