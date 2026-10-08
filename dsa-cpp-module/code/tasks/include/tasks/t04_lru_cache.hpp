#pragma once
// T04. LRU-кеш (Least Recently Used) — наприклад, кеш тайлів карти висот (як AP_Terrain).
// get(key)  -> значення, якщо є (і ключ стає «найсвіжішим»), інакше std::nullopt.
// put(key, value) -> вставити/оновити; якщо кеш переповнений — викинути НАЙДАВНІШЕ використаний.
//
// Вимога: get і put за O(1). Класичне рішення: std::list (порядок використання)
// + std::unordered_map<K, list::iterator>.
#include <cstddef>
#include <optional>

namespace tasks {

template <typename K, typename V>
class LruCache {
public:
    explicit LruCache(std::size_t capacity) : capacity_(capacity) {}

    std::optional<V> get(const K& key)
    {
        (void)key;
        // TODO
        return std::nullopt;
    }

    void put(const K& key, V value)
    {
        (void)key;
        (void)value;
        // TODO
    }

    std::size_t size() const
    {
        // TODO
        return 0;
    }

private:
    std::size_t capacity_;
    // TODO: свої поля
};

} // namespace tasks
