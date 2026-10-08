#pragma once
// Хеш-таблиця з відкритою адресацією (linear probing).
// Середній випадок: find/insert/erase O(1). Найгірший — O(n) (усі ключі в один кошик).
// Застосування: таблиця параметрів за іменем (як AP_Param в ArduPilot), кеші, індекси.
#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace dsa {

template <typename K, typename V, typename Hash = std::hash<K>>
class HashMap {
    enum class State : unsigned char { Empty, Used, Deleted };
    struct Slot {
        State state = State::Empty;
        K key{};
        V value{};
    };

public:
    explicit HashMap(std::size_t initial_capacity = 16) : slots_(round_up_pow2(initial_capacity)) {}

    // Вставити або оновити. Повертає true, якщо ключ новий.
    bool insert_or_assign(const K& key, V value)
    {
        if ((used_ + deleted_ + 1) * 10 > slots_.size() * 7) { // load factor > 0.7
            rehash(slots_.size() * 2);
        }
        std::size_t first_deleted = npos;
        std::size_t i = index_for(key);
        while (true) {
            Slot& s = slots_[i];
            if (s.state == State::Empty) {
                Slot& target = first_deleted != npos ? slots_[first_deleted] : s;
                if (first_deleted != npos) --deleted_;
                target.state = State::Used;
                target.key = key;
                target.value = std::move(value);
                ++used_;
                return true;
            }
            if (s.state == State::Deleted) {
                if (first_deleted == npos) first_deleted = i;
            } else if (s.key == key) {
                s.value = std::move(value);
                return false;
            }
            i = (i + 1) & mask();
        }
    }

    V* find(const K& key)
    {
        const std::size_t i = locate(key);
        return i == npos ? nullptr : &slots_[i].value;
    }
    const V* find(const K& key) const
    {
        const std::size_t i = locate(key);
        return i == npos ? nullptr : &slots_[i].value;
    }

    bool erase(const K& key)
    {
        const std::size_t i = locate(key);
        if (i == npos) {
            return false;
        }
        // Не можна просто зробити Empty — розірвемо ланцюжок пошуку для інших ключів.
        slots_[i].state = State::Deleted;
        --used_;
        ++deleted_;
        return true;
    }

    std::size_t size() const { return used_; }
    std::size_t capacity() const { return slots_.size(); }

    // Скільки кроків пробінгу знадобилось, щоб знайти ключ (для аналізу колізій).
    std::size_t probe_length(const K& key) const
    {
        std::size_t steps = 1;
        for (std::size_t i = index_for(key); slots_[i].state != State::Empty; i = (i + 1) & mask(), ++steps) {
            if (slots_[i].state == State::Used && slots_[i].key == key) return steps;
        }
        return 0;
    }

private:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    static std::size_t round_up_pow2(std::size_t n)
    {
        std::size_t p = 8;
        while (p < n) p <<= 1;
        return p;
    }
    std::size_t mask() const { return slots_.size() - 1; }
    std::size_t index_for(const K& key) const { return Hash{}(key) & mask(); }

    std::size_t locate(const K& key) const
    {
        for (std::size_t i = index_for(key); slots_[i].state != State::Empty; i = (i + 1) & mask()) {
            if (slots_[i].state == State::Used && slots_[i].key == key) return i;
        }
        return npos;
    }

    void rehash(std::size_t new_capacity)
    {
        std::vector<Slot> old = std::move(slots_);
        slots_ = std::vector<Slot>(round_up_pow2(new_capacity));
        used_ = deleted_ = 0;
        for (Slot& s : old) {
            if (s.state == State::Used) insert_or_assign(s.key, std::move(s.value));
        }
    }

    std::vector<Slot> slots_;
    std::size_t used_ = 0;
    std::size_t deleted_ = 0;
};

} // namespace dsa
