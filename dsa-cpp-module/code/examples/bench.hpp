#pragma once
// Простий таймер для бенчмарків.
#include <chrono>

namespace bench {

template <typename F>
double ms(F&& f)
{
    const auto t0 = std::chrono::steady_clock::now();
    f();
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
}

// Не дати компілятору викинути «непотрібний» результат.
template <typename T>
void keep(T const& value)
{
    asm volatile("" : : "r,m"(value) : "memory");
}

} // namespace bench
