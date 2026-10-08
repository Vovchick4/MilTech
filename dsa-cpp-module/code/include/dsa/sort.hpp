#pragma once
// Класичні сортування. Усі працюють з std::vector<T> і компаратором.
//
// | алгоритм   | кращий     | середній   | гірший     | пам'ять  | стабільний |
// |------------|------------|------------|------------|----------|------------|
// | insertion  | O(n)       | O(n²)      | O(n²)      | O(1)     | так        |
// | merge      | O(n log n) | O(n log n) | O(n log n) | O(n)     | так        |
// | quick      | O(n log n) | O(n log n) | O(n²)      | O(log n) | ні         |
// | std::sort  | introsort: quick + heap + insertion, гірший O(n log n)          |
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace dsa {

template <typename T, typename Cmp = std::less<T>>
void insertion_sort(std::vector<T>& a, std::size_t lo, std::size_t hi, Cmp cmp = Cmp{})
{
    for (std::size_t i = lo + 1; i < hi; ++i) {
        T x = std::move(a[i]);
        std::size_t j = i;
        while (j > lo && cmp(x, a[j - 1])) {
            a[j] = std::move(a[j - 1]);
            --j;
        }
        a[j] = std::move(x);
    }
}

template <typename T, typename Cmp = std::less<T>>
void insertion_sort(std::vector<T>& a, Cmp cmp = Cmp{})
{
    insertion_sort(a, 0, a.size(), cmp);
}

namespace detail {
template <typename T, typename Cmp>
void merge_sort_rec(std::vector<T>& a, std::vector<T>& tmp, std::size_t lo, std::size_t hi, Cmp& cmp)
{
    if (hi - lo <= 16) { // маленькі шматки — insertion sort швидший
        insertion_sort(a, lo, hi, cmp);
        return;
    }
    const std::size_t mid = lo + (hi - lo) / 2;
    merge_sort_rec(a, tmp, lo, mid, cmp);
    merge_sort_rec(a, tmp, mid, hi, cmp);

    std::size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        // "<=" через !cmp(b, a) — зберігає стабільність
        tmp[k++] = !cmp(a[j], a[i]) ? std::move(a[i++]) : std::move(a[j++]);
    }
    while (i < mid) tmp[k++] = std::move(a[i++]);
    while (j < hi) tmp[k++] = std::move(a[j++]);
    for (k = lo; k < hi; ++k) a[k] = std::move(tmp[k]);
}

template <typename T, typename Cmp>
void quick_sort_rec(std::vector<T>& a, std::size_t lo, std::size_t hi, Cmp& cmp)
{
    while (hi - lo > 16) {
        // медіана з трьох — захист від O(n²) на вже відсортованих даних
        const std::size_t mid = lo + (hi - lo) / 2;
        if (cmp(a[mid], a[lo])) std::swap(a[mid], a[lo]);
        if (cmp(a[hi - 1], a[lo])) std::swap(a[hi - 1], a[lo]);
        if (cmp(a[hi - 1], a[mid])) std::swap(a[hi - 1], a[mid]);
        const T pivot = a[mid];

        // розбиття Хоара
        std::size_t i = lo, j = hi - 1;
        while (true) {
            while (cmp(a[i], pivot)) ++i;
            while (cmp(pivot, a[j])) --j;
            if (i >= j) break;
            std::swap(a[i], a[j]);
            ++i;
            --j;
        }
        // рекурсія в меншу половину, цикл — у більшу: глибина стека O(log n)
        if (j + 1 - lo < hi - (j + 1)) {
            quick_sort_rec(a, lo, j + 1, cmp);
            lo = j + 1;
        } else {
            quick_sort_rec(a, j + 1, hi, cmp);
            hi = j + 1;
        }
    }
    insertion_sort(a, lo, hi, cmp);
}
} // namespace detail

template <typename T, typename Cmp = std::less<T>>
void merge_sort(std::vector<T>& a, Cmp cmp = Cmp{})
{
    std::vector<T> tmp(a.size());
    detail::merge_sort_rec(a, tmp, 0, a.size(), cmp);
}

template <typename T, typename Cmp = std::less<T>>
void quick_sort(std::vector<T>& a, Cmp cmp = Cmp{})
{
    if (a.size() > 1) detail::quick_sort_rec(a, 0, a.size(), cmp);
}

// Бінарний пошук: індекс першого елемента >= x (аналог std::lower_bound). O(log n).
template <typename T, typename Cmp = std::less<T>>
std::size_t lower_bound_index(const std::vector<T>& a, const T& x, Cmp cmp = Cmp{})
{
    std::size_t lo = 0, hi = a.size(); // шукаємо в [lo, hi)
    while (lo < hi) {
        const std::size_t mid = lo + (hi - lo) / 2; // не (lo+hi)/2 — переповнення
        if (cmp(a[mid], x)) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

} // namespace dsa
