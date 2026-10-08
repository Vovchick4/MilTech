#pragma once
// Бінарне дерево пошуку (BST), без балансування.
// find/insert: O(h), де h — висота. Випадкові ключі: h ~ log n. Відсортовані ключі: h = n (!).
// std::map — це збалансоване дерево (червоно-чорне), там h <= 2·log n завжди.
#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace dsa {

template <typename K, typename V>
class BST {
    struct Node {
        K key;
        V value;
        std::unique_ptr<Node> left, right;
    };

public:
    BST() = default;
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    // Ітеративне видалення: рекурсивний деструктор unique_ptr на дереві-«палиці»
    // глибиною 10^5 переповнив би стек.
    ~BST()
    {
        std::vector<std::unique_ptr<Node>> stack;
        if (root_) stack.push_back(std::move(root_));
        while (!stack.empty()) {
            std::unique_ptr<Node> n = std::move(stack.back());
            stack.pop_back();
            if (n->left) stack.push_back(std::move(n->left));
            if (n->right) stack.push_back(std::move(n->right));
        }
    }

    bool insert(const K& key, V value)
    {
        std::unique_ptr<Node>* cur = &root_;
        while (*cur) {
            if (key < (*cur)->key) {
                cur = &(*cur)->left;
            } else if ((*cur)->key < key) {
                cur = &(*cur)->right;
            } else {
                (*cur)->value = std::move(value);
                return false;
            }
        }
        *cur = std::make_unique<Node>(Node{key, std::move(value), nullptr, nullptr});
        ++size_;
        return true;
    }

    const V* find(const K& key) const
    {
        const Node* n = root_.get();
        while (n) {
            if (key < n->key) n = n->left.get();
            else if (n->key < key) n = n->right.get();
            else return &n->value;
        }
        return nullptr;
    }

    // Обхід у порядку зростання ключів (in-order).
    template <typename F>
    void in_order(F&& f) const { in_order(root_.get(), f); }

    std::size_t height() const { return height(root_.get()); }
    std::size_t size() const { return size_; }

private:
    template <typename F>
    static void in_order(const Node* n, F& f)
    {
        if (!n) return;
        in_order(n->left.get(), f);
        f(n->key, n->value);
        in_order(n->right.get(), f);
    }

    // Рекурсія глибиною h. Для дерева-«палиці» на 10^6 вузлів — переповнення стека,
    // тому в тестах і бенчмарках виродження показуємо на невеликих n.
    static std::size_t height(const Node* n)
    {
        return n ? 1 + std::max(height(n->left.get()), height(n->right.get())) : 0;
    }

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
};

} // namespace dsa
