#pragma once
// T01. Максимум у ковзному вікні.
// Дано висоти дрона a[0..n-1] і вікно k. Для кожного вікна [i, i+k) повернути максимум.
// Результат має n-k+1 елементів. Якщо k == 0 або k > n — порожній вектор.
//
// Вимога: O(n). Наївне рішення O(n·k) тести на великих даних не пройде за час.
// Підказка: подивись dsa::MovingMin у include/dsa/filters.hpp (монотонна черга).
#include <cstddef>
#include <vector>

namespace tasks {

inline std::vector<int> sliding_window_max(const std::vector<int>& a, std::size_t k)
{
    (void)a;
    (void)k;
    // TODO
    return {};
}

} // namespace tasks
