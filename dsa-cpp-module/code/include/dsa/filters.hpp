#pragma once
// Фільтри «ковзного вікна» для телеметрії (висота, напруга, швидкість).
#include <algorithm>
#include <array>
#include <cstddef>
#include <deque>
#include "ring_buffer.hpp"

namespace dsa {

// Ковзне середнє за останні N значень. O(1) на крок завдяки «біжучій сумі».
template <std::size_t N>
class MovingAverage {
public:
    double update(double x)
    {
        if (buf_.full()) sum_ -= buf_.oldest();
        buf_.push(x);
        sum_ += x;
        return value();
    }
    double value() const { return buf_.empty() ? 0.0 : sum_ / static_cast<double>(buf_.size()); }

private:
    RingBuffer<double, N> buf_;
    double sum_ = 0.0;
};

// Медіанний фільтр: прибирає поодинокі «викиди» (стрибок GPS/бароміра), які середнє розмазує.
// O(N) на крок через std::nth_element — для N = 5..15 це дешевше за складні структури.
template <std::size_t N>
class MedianFilter {
public:
    double update(double x)
    {
        buf_.push(x);
        std::array<double, N> w{};
        for (std::size_t i = 0; i < buf_.size(); ++i) w[i] = buf_[i];
        auto mid = w.begin() + buf_.size() / 2;
        std::nth_element(w.begin(), mid, w.begin() + buf_.size());
        return *mid;
    }

private:
    RingBuffer<double, N> buf_;
};

// Ковзний мінімум за вікно N: монотонна черга (deque), амортизовано O(1) на крок.
// (Ковзний максимум — задача T01 для студентів.)
template <std::size_t N>
class MovingMin {
public:
    double update(double x)
    {
        ++t_;
        while (!q_.empty() && q_.back().second >= x) q_.pop_back();
        q_.emplace_back(t_, x);
        if (q_.front().first + N <= t_) q_.pop_front();
        return q_.front().second;
    }

private:
    std::deque<std::pair<std::size_t, double>> q_;
    std::size_t t_ = 0;
};

} // namespace dsa
