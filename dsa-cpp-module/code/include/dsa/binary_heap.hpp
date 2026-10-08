#pragma once
// Двійкова купа (binary heap) на масиві.
// top() O(1), push()/pop() O(log n). Основа std::priority_queue і планувальників задач.
// Compare = std::less<T>  -> min-heap (зверху найменший), std::greater<T> -> max-heap.
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dsa {

template <typename T, typename Compare = std::less<T>>
class BinaryHeap {
public:
    explicit BinaryHeap(Compare cmp = Compare{}) : cmp_(std::move(cmp)) {}

    void push(T value)
    {
        data_.push_back(std::move(value));
        sift_up(data_.size() - 1);
    }

    const T& top() const
    {
        if (data_.empty()) {
            throw std::out_of_range("BinaryHeap::top on empty heap");
        }
        return data_.front();
    }

    T pop()
    {
        if (data_.empty()) {
            throw std::out_of_range("BinaryHeap::pop on empty heap");
        }
        T result = std::move(data_.front());
        data_.front() = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            sift_down(0);
        }
        return result;
    }

    std::size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

private:
    // Індекси: батько (i-1)/2, діти 2i+1 і 2i+2.
    void sift_up(std::size_t i)
    {
        while (i > 0) {
            const std::size_t parent = (i - 1) / 2;
            if (!cmp_(data_[i], data_[parent])) {
                break;
            }
            std::swap(data_[i], data_[parent]);
            i = parent;
        }
    }

    void sift_down(std::size_t i)
    {
        const std::size_t n = data_.size();
        while (true) {
            const std::size_t l = 2 * i + 1;
            const std::size_t r = l + 1;
            std::size_t best = i;
            if (l < n && cmp_(data_[l], data_[best])) best = l;
            if (r < n && cmp_(data_[r], data_[best])) best = r;
            if (best == i) {
                break;
            }
            std::swap(data_[i], data_[best]);
            i = best;
        }
    }

    std::vector<T> data_;
    Compare cmp_;
};

} // namespace dsa
