#pragma once
// Однозв'язний список на std::unique_ptr.
// Навчальна структура: вставка/видалення на початку O(1), доступ за індексом O(n),
// погана локальність кешу. У реальному коді майже завжди краще std::vector.
#include <cstddef>
#include <memory>
#include <utility>

namespace dsa {

template <typename T>
class LinkedList {
    struct Node {
        T value;
        std::unique_ptr<Node> next;
    };

public:
    LinkedList() = default;
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;
    LinkedList(LinkedList&&) noexcept = default;
    LinkedList& operator=(LinkedList&&) noexcept = default;

    // ВАЖЛИВО: деструктор за замовчуванням видаляв би вузли рекурсивно
    // (~unique_ptr -> ~Node -> ~unique_ptr ...) і на 1 000 000 елементів переповнив би стек.
    ~LinkedList() { clear(); }

    void push_front(T value)
    {
        head_ = std::make_unique<Node>(Node{std::move(value), std::move(head_)});
        ++size_;
    }

    bool pop_front(T& out)
    {
        if (!head_) {
            return false;
        }
        out = std::move(head_->value);
        head_ = std::move(head_->next);
        --size_;
        return true;
    }

    // Розворот списку на місці: O(n) часу, O(1) пам'яті.
    void reverse()
    {
        std::unique_ptr<Node> prev;
        while (head_) {
            std::unique_ptr<Node> next = std::move(head_->next);
            head_->next = std::move(prev);
            prev = std::move(head_);
            head_ = std::move(next);
        }
        head_ = std::move(prev);
    }

    void clear()
    {
        while (head_) {
            head_ = std::move(head_->next); // ітеративно, без рекурсії
        }
        size_ = 0;
    }

    template <typename F>
    void for_each(F&& f) const
    {
        for (const Node* n = head_.get(); n != nullptr; n = n->next.get()) {
            f(n->value);
        }
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

private:
    std::unique_ptr<Node> head_;
    std::size_t size_ = 0;
};

} // namespace dsa
