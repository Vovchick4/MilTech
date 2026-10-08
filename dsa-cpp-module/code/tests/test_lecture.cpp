// Тести для коду з лекцій (include/dsa). Мають проходити одразу.
#include <algorithm>
#include <random>
#include <string>
#include <vector>

#include "dsa/binary_heap.hpp"
#include "dsa/bst.hpp"
#include "dsa/filters.hpp"
#include "dsa/grid_path.hpp"
#include "dsa/hash_map.hpp"
#include "dsa/linked_list.hpp"
#include "dsa/ring_buffer.hpp"
#include "dsa/sort.hpp"
#include "mini_test.hpp"

using namespace dsa;

TEST("RingBuffer: push/overwrite/order")
{
    RingBuffer<int, 3> rb;
    CHECK(rb.empty());
    CHECK(!rb.push(1));
    rb.push(2);
    rb.push(3);
    CHECK(rb.full());
    CHECK(rb.push(4)); // перезаписали 1
    CHECK_EQ(rb.oldest(), 2);
    CHECK_EQ(rb.newest(), 4);
    int x = 0;
    CHECK(rb.pop_oldest(x));
    CHECK_EQ(x, 2);
    CHECK_EQ(rb.size(), 2u);
    rb.push(5);
    CHECK_EQ(rb[0], 3);
    CHECK_EQ(rb[2], 5);
}

TEST("LinkedList: reverse + big list destructor")
{
    LinkedList<int> l;
    for (int i = 0; i < 5; ++i) l.push_front(i); // 4 3 2 1 0
    l.reverse();                                  // 0 1 2 3 4
    std::vector<int> v;
    l.for_each([&](int x) { v.push_back(x); });
    CHECK((v == std::vector<int>{0, 1, 2, 3, 4}));

    LinkedList<int> big;
    for (int i = 0; i < 1'000'000; ++i) big.push_front(i); // деструктор не має впасти
    CHECK_EQ(big.size(), 1'000'000u);
}

TEST("BinaryHeap: min and max heap")
{
    BinaryHeap<int> h;
    for (int x : {5, 1, 9, 3, 7, 1}) h.push(x);
    std::vector<int> out;
    while (!h.empty()) out.push_back(h.pop());
    CHECK((out == std::vector<int>{1, 1, 3, 5, 7, 9}));

    BinaryHeap<int, std::greater<int>> mx;
    for (int x : {5, 1, 9}) mx.push(x);
    CHECK_EQ(mx.top(), 9);
}

TEST("HashMap: insert/find/erase/rehash")
{
    HashMap<std::string, float> m(4);
    CHECK(m.insert_or_assign("WPNAV_SPEED", 1500));
    CHECK(!m.insert_or_assign("WPNAV_SPEED", 2000));
    CHECK_EQ(*m.find("WPNAV_SPEED"), 2000.0f);
    for (int i = 0; i < 1000; ++i) m.insert_or_assign("P" + std::to_string(i), static_cast<float>(i));
    CHECK_EQ(m.size(), 1001u);
    CHECK_EQ(*m.find("P777"), 777.0f);
    CHECK(m.erase("P777"));
    CHECK(m.find("P777") == nullptr);
    CHECK_EQ(*m.find("P778"), 778.0f); // ланцюжок пошуку не розірвано
    CHECK(m.insert_or_assign("P777", 1));
    CHECK_EQ(m.size(), 1001u);
}

TEST("BST: in-order sorted, degenerate height")
{
    BST<int, int> t;
    for (int x : {50, 30, 70, 20, 40, 60, 80}) t.insert(x, x * 10);
    std::vector<int> keys;
    t.in_order([&](int k, int) { keys.push_back(k); });
    CHECK(std::is_sorted(keys.begin(), keys.end()));
    CHECK_EQ(t.height(), 3u);
    CHECK_EQ(*t.find(60), 600);
    CHECK(t.find(65) == nullptr);

    BST<int, int> stick;
    for (int i = 0; i < 1000; ++i) stick.insert(i, i); // відсортовані ключі -> «палиця»
    CHECK_EQ(stick.height(), 1000u);
}

TEST("Sorts: random, sorted, reversed, duplicates")
{
    std::mt19937 rng(42);
    std::vector<std::vector<int>> inputs;
    std::vector<int> r(5000);
    for (auto& x : r) x = static_cast<int>(rng() % 1000);
    inputs.push_back(r);
    std::vector<int> s(5000);
    for (int i = 0; i < 5000; ++i) s[i] = i;
    inputs.push_back(s);
    inputs.emplace_back(s.rbegin(), s.rend());
    inputs.emplace_back(5000, 7);
    inputs.push_back({});
    inputs.push_back({1});

    for (const auto& in : inputs) {
        auto expect = in;
        std::sort(expect.begin(), expect.end());
        auto a = in, b = in, c = in;
        quick_sort(a);
        merge_sort(b);
        if (c.size() <= 5000) insertion_sort(c);
        CHECK(a == expect);
        CHECK(b == expect);
        CHECK(c == expect);
    }
}

TEST("Sorts: merge sort is stable")
{
    std::vector<std::pair<int, int>> v{{2, 0}, {1, 1}, {2, 2}, {1, 3}, {2, 4}};
    merge_sort(v, [](auto& a, auto& b) { return a.first < b.first; });
    CHECK((v == std::vector<std::pair<int, int>>{{1, 1}, {1, 3}, {2, 0}, {2, 2}, {2, 4}}));
}

TEST("Binary search: lower_bound_index")
{
    std::vector<int> a{1, 3, 3, 5, 9};
    CHECK_EQ(lower_bound_index(a, 3), 1u);
    CHECK_EQ(lower_bound_index(a, 4), 3u);
    CHECK_EQ(lower_bound_index(a, 0), 0u);
    CHECK_EQ(lower_bound_index(a, 10), 5u);
}

TEST("Filters: average, median spike rejection, moving min")
{
    MovingAverage<4> ma;
    for (double x : {1.0, 2.0, 3.0, 4.0, 5.0}) ma.update(x);
    CHECK_NEAR(ma.value(), 3.5, 1e-9);

    MedianFilter<5> med;
    double out = 0;
    for (double x : {100.0, 100.0, 100.0, 900.0, 100.0}) out = med.update(x); // 900 — стрибок
    CHECK_NEAR(out, 100.0, 1e-9);

    MovingMin<3> mn;
    std::vector<double> got;
    for (double x : {5.0, 3.0, 4.0, 6.0, 7.0, 1.0}) got.push_back(mn.update(x));
    CHECK((got == std::vector<double>{5, 3, 3, 3, 4, 1}));
}

TEST("Grid: BFS / Dijkstra / A* agree on open map")
{
    Cell s, t;
    auto g = Grid::from_ascii({
        "S.........",
        "..........",
        "....###...",
        "....#.....",
        ".........G",
    }, &s, &t);
    auto b = bfs(g, s, t), d = dijkstra(g, s, t), a = astar(g, s, t);
    CHECK(b.found() && d.found() && a.found());
    CHECK_NEAR(d.cost, 13.0, 1e-9);
    CHECK_NEAR(a.cost, d.cost, 1e-9);
    CHECK(a.expanded <= d.expanded); // A* розкриває не більше вузлів
}

TEST("Grid: Dijkstra avoids expensive cells, BFS does not")
{
    Cell s, t;
    auto g = Grid::from_ascii({
        "S~~~G",
        ".###.",
        ".....",
    }, &s, &t);
    auto b = bfs(g, s, t);
    auto d = dijkstra(g, s, t);
    auto a = astar(g, s, t);
    CHECK_EQ(b.path.size(), 5u);       // напряму через '~'
    CHECK_NEAR(d.cost, 8.0, 1e-9);     // обхід знизу: 8 клітинок по 1 < 3*3+1
    CHECK_NEAR(a.cost, d.cost, 1e-9);
}

TEST("Grid: no path, diagonal does not cut corners")
{
    Cell s, t;
    auto g = Grid::from_ascii({
        "S#.",
        "##.",
        "..G",
    }, &s, &t);
    CHECK(!astar(g, s, t).found());
    CHECK(!astar(g, s, t, true).found()); // діагональ (0,0)->(1,1) заборонена: (1,1) — '#'

    auto open = Grid::from_ascii({"S..", "...", "..G"}, &s, &t);
    CHECK_NEAR(astar(open, s, t, true).cost, 2 * kSqrt2, 1e-9);
}

TEST_MAIN()
