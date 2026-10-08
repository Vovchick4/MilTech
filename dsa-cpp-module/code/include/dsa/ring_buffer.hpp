#pragma once
// Кільцевий буфер фіксованого розміру (circular buffer).
// - пам'ять виділяється один раз (std::array) — жодного new/malloc у польоті;
// - push() O(1); коли буфер повний — перезаписує найстаріший елемент;
// - типове застосування: історія телеметрії, черга MAVLink-пакетів, фільтри.
#include <array>
#include <cstddef>
#include <stdexcept>

namespace dsa {

template <typename T, std::size_t N>
class RingBuffer {
    static_assert(N > 0, "RingBuffer capacity must be > 0");

public:
    // Додати елемент. Повертає true, якщо найстаріший елемент було перезаписано.
    bool push(const T& value)
    {
        data_[head_] = value;
        head_ = (head_ + 1) % N;
        if (size_ < N) {
            ++size_;
            return false;
        }
        return true;
    }

    // Забрати найстаріший елемент (семантика черги FIFO).
    bool pop_oldest(T& out)
    {
        if (empty()) {
            return false;
        }
        out = data_[oldest_index()];
        --size_;
        return true;
    }

    // i = 0 — найстаріший, i = size()-1 — найновіший.
    const T& operator[](std::size_t i) const { return data_[(oldest_index() + i) % N]; }

    const T& at(std::size_t i) const
    {
        if (i >= size_) {
            throw std::out_of_range("RingBuffer::at");
        }
        return (*this)[i];
    }

    const T& oldest() const { return at(0); }
    const T& newest() const { return at(size_ - 1); }

    std::size_t size() const { return size_; }
    static constexpr std::size_t capacity() { return N; }
    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == N; }
    void clear() { head_ = size_ = 0; }

private:
    std::size_t oldest_index() const { return (head_ + N - size_) % N; }

    std::array<T, N> data_{};
    std::size_t head_ = 0; // куди писати наступний елемент
    std::size_t size_ = 0;
};

} // namespace dsa
