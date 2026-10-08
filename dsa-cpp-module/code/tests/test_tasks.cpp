// Тести задач T01–T10. На заготовках вони ПАДАЮТЬ — це нормально.
// Мета студента: зробити все зеленим.  ./build/test_tasks        — усі
//                                       ./build/test_tasks T04    — тільки T04
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "mini_test.hpp"
#include "tasks/t01_sliding_max.hpp"
#include "tasks/t02_k_closest.hpp"
#include "tasks/t03_point_in_polygon.hpp"
#include "tasks/t04_lru_cache.hpp"
#include "tasks/t05_douglas_peucker.hpp"
#include "tasks/t06_union_find.hpp"
#include "tasks/t07_merge_intervals.hpp"
#include "tasks/t08_crc16_mavlink.hpp"
#include "tasks/t09_topo_sort.hpp"
#include "tasks/t10_route_order.hpp"

using dsa::Point2;
using namespace tasks;

// ---------------- T01 ----------------
TEST("T01 sliding max: basic")
{
    CHECK((sliding_window_max({1, 3, -1, -3, 5, 3, 6, 7}, 3) == std::vector<int>{3, 3, 5, 5, 6, 7}));
    CHECK((sliding_window_max({4, 2}, 1) == std::vector<int>{4, 2}));
    CHECK((sliding_window_max({4, 2}, 2) == std::vector<int>{4}));
    CHECK(sliding_window_max({4, 2}, 3).empty());
    CHECK(sliding_window_max({4, 2}, 0).empty());
}

TEST("T01 sliding max: random vs naive + large (must be O(n))")
{
    std::mt19937 rng(1);
    std::vector<int> a(2000);
    for (auto& x : a) x = static_cast<int>(rng() % 100);
    for (std::size_t k : {1u, 2u, 7u, 100u, 2000u}) {
        std::vector<int> expect;
        for (std::size_t i = 0; i + k <= a.size(); ++i) expect.push_back(*std::max_element(a.begin() + i, a.begin() + i + k));
        CHECK(sliding_window_max(a, k) == expect);
    }
    std::vector<int> big(2'000'000);
    for (auto& x : big) x = static_cast<int>(rng());
    CHECK_EQ(sliding_window_max(big, 20'000).size(), big.size() - 20'000 + 1); // O(n·k) тут = 4·10^10
}

// ---------------- T02 ----------------
TEST("T02 k closest")
{
    const std::vector<Point2> pts{{10, 0}, {1, 1}, {-2, 0}, {0, 5}, {3, 4}, {0, -1}};
    const auto r = k_closest(pts, {0, 0}, 3);
    CHECK((r == std::vector<Point2>{{0, -1}, {1, 1}, {-2, 0}}));
    CHECK_EQ(k_closest(pts, {0, 0}, 10).size(), pts.size());
    CHECK(k_closest(pts, {0, 0}, 0).empty());
    // рівні відстані {0,5} і {3,4} = 5: порядок появи
    const auto all = k_closest(pts, {0, 0}, 6);
    CHECK((all.size() == 6 && all[3] == Point2{0, 5} && all[4] == Point2{3, 4}));
}

// ---------------- T03 ----------------
TEST("T03 point in polygon: square and concave")
{
    const std::vector<Point2> sq{{0, 0}, {10, 0}, {10, 10}, {0, 10}};
    CHECK(point_in_polygon(sq, {5, 5}));
    CHECK(!point_in_polygon(sq, {15, 5}));
    CHECK(!point_in_polygon(sq, {-1, -1}));

    // «підкова» (неопукла):  точка у вирізі — зовні
    const std::vector<Point2> u{{0, 0}, {30, 0}, {30, 30}, {20, 30}, {20, 10}, {10, 10}, {10, 30}, {0, 30}};
    CHECK(point_in_polygon(u, {5, 20}));
    CHECK(point_in_polygon(u, {25, 20}));
    CHECK(!point_in_polygon(u, {15, 20}));
    CHECK(point_in_polygon(u, {15, 5}));
    CHECK(!point_in_polygon(u, {15, 10.0001}));
    // промінь проходить через вершину (y = 10)
    CHECK(point_in_polygon(u, {5, 10}));
}

// ---------------- T04 ----------------
TEST("T04 LRU cache")
{
    LruCache<int, std::string> c(2);
    c.put(1, "a");
    c.put(2, "b");
    CHECK(c.get(1) == std::optional<std::string>("a")); // 1 стає свіжим
    c.put(3, "c");                                      // викидає 2
    CHECK(!c.get(2).has_value());
    CHECK(c.get(3) == std::optional<std::string>("c"));
    c.put(1, "A");                                      // оновлення, не вставка
    CHECK_EQ(c.size(), 2u);
    c.put(4, "d");                                      // викидає 3 (1 свіжіший)
    CHECK(!c.get(3).has_value());
    CHECK(c.get(1) == std::optional<std::string>("A"));
    CHECK(c.get(4) == std::optional<std::string>("d"));
}

TEST("T04 LRU cache: large, O(1)")
{
    LruCache<int, int> c(10'000);
    for (int i = 0; i < 1'000'000; ++i) {
        c.put(i, i);
        if (i >= 5) CHECK(c.get(i - 5) == std::optional<int>(i - 5));
    }
    CHECK_EQ(c.size(), 10'000u);
    CHECK(!c.get(0).has_value());
}

// ---------------- T05 ----------------
static double max_deviation(const std::vector<Point2>& orig, const std::vector<Point2>& simp)
{
    // для кожної вихідної точки — відстань до найближчого відрізка спрощеної лінії
    double worst = 0;
    for (const auto& p : orig) {
        double best = std::numeric_limits<double>::infinity();
        for (std::size_t i = 0; i + 1 < simp.size(); ++i) {
            const Point2 a = simp[i], b = simp[i + 1];
            const double dx = b.x - a.x, dy = b.y - a.y, l2 = dx * dx + dy * dy;
            double t = l2 == 0 ? 0 : ((p.x - a.x) * dx + (p.y - a.y) * dy) / l2;
            t = std::clamp(t, 0.0, 1.0);
            best = std::min(best, std::hypot(p.x - (a.x + t * dx), p.y - (a.y + t * dy)));
        }
        worst = std::max(worst, best);
    }
    return worst;
}

TEST("T05 Douglas-Peucker")
{
    const std::vector<Point2> line{{0, 0}, {1, 0.1}, {2, -0.1}, {3, 0.05}, {4, 0}};
    CHECK((simplify_track(line, 0.5) == std::vector<Point2>{{0, 0}, {4, 0}}));

    const std::vector<Point2> corner{{0, 0}, {5, 0}, {10, 0}, {10, 5}, {10, 10}};
    CHECK((simplify_track(corner, 0.1) == std::vector<Point2>{{0, 0}, {10, 0}, {10, 10}}));

    // «справжній» трек: квадрат 1 км з GPS-шумом, 4000 точок
    std::mt19937 rng(5);
    std::normal_distribution<double> noise(0.0, 1.0);
    std::vector<Point2> track;
    for (int i = 0; i < 1000; ++i) track.push_back({i + noise(rng), noise(rng)});
    for (int i = 0; i < 1000; ++i) track.push_back({1000 + noise(rng), i + noise(rng)});
    for (int i = 0; i < 1000; ++i) track.push_back({1000 - i + noise(rng), 1000 + noise(rng)});
    for (int i = 0; i < 1000; ++i) track.push_back({noise(rng), 1000 - i + noise(rng)});
    const auto s = simplify_track(track, 10.0);
    CHECK(s.size() >= 5 && s.size() <= 20);
    CHECK(s.front() == track.front() && s.back() == track.back());
    CHECK(max_deviation(track, s) <= 10.0 + 1e-9);
}

// ---------------- T06 ----------------
TEST("T06 union-find")
{
    UnionFind uf(6);
    CHECK_EQ(uf.components(), 6u);
    CHECK(uf.unite(0, 1));
    CHECK(uf.unite(2, 3));
    CHECK(!uf.unite(1, 0));
    CHECK(uf.unite(1, 3));
    CHECK_EQ(uf.find(0), uf.find(2));
    CHECK(uf.find(4) != uf.find(0));
    CHECK_EQ(uf.components(), 3u);
}

TEST("T06 Kruskal MST")
{
    // 4 станції: квадрат 10 км + діагоналі
    std::vector<Edge> e{{0, 1, 10}, {1, 2, 10}, {2, 3, 10}, {3, 0, 10}, {0, 2, 14.1}, {1, 3, 14.1}};
    CHECK_NEAR(mst_kruskal(4, e), 30.0, 1e-9);
    CHECK_NEAR(mst_kruskal(3, {{0, 1, 5}, {1, 2, 3}, {0, 2, 1}}), 4.0, 1e-9);
    CHECK_NEAR(mst_kruskal(3, {{0, 1, 5}}), -1.0, 1e-9); // станція 2 недосяжна
}

// ---------------- T07 ----------------
TEST("T07 merge intervals")
{
    CHECK((merge_intervals({{8, 10}, {1, 3}, {2, 6}, {15, 18}}) == std::vector<Interval>{{1, 6}, {8, 10}, {15, 18}}));
    CHECK((merge_intervals({{1, 3}, {3, 5}}) == std::vector<Interval>{{1, 5}}));
    CHECK((merge_intervals({{1, 10}, {2, 3}, {4, 5}}) == std::vector<Interval>{{1, 10}}));
    CHECK(merge_intervals({}).empty());
}

// ---------------- T08 ----------------
TEST("T08 CRC-16 MAVLink")
{
    const char* s = "123456789";
    CHECK_EQ(crc16_mavlink(reinterpret_cast<const std::uint8_t*>(s), std::strlen(s)), 0x6F91);
    CHECK_EQ(crc16_mavlink(nullptr, 0), 0xFFFF);
    // інкрементальність: CRC(ab) == CRC(b, CRC(a))
    const std::uint8_t data[] = {0xFE, 0x09, 0x00, 0x01, 0x01, 0x00};
    const auto full = crc16_mavlink(data, 6);
    CHECK_EQ(crc16_mavlink(data + 3, 3, crc16_mavlink(data, 3)), full);
    CHECK(full != 0xFFFF);
}

// ---------------- T09 ----------------
TEST("T09 topological sort")
{
    // 0 живлення, 1 GPS, 2 компас, 3 калібрування компаса, 4 ARM
    const auto r = topo_sort(5, {{0, 1}, {0, 2}, {2, 3}, {1, 4}, {3, 4}});
    CHECK(r.has_value());
    if (r) CHECK((*r == std::vector<int>{0, 1, 2, 3, 4}));

    const auto r2 = topo_sort(4, {{3, 0}, {2, 0}});
    CHECK(r2.has_value());
    if (r2) CHECK((*r2 == std::vector<int>{1, 2, 3, 0}));

    CHECK(!topo_sort(3, {{0, 1}, {1, 2}, {2, 0}}).has_value()); // цикл
    const auto r3 = topo_sort(0, {});
    CHECK(r3.has_value() && r3->empty());
}

// ---------------- T10 ----------------
static double greedy_length(Point2 start, const std::vector<Point2>& pts)
{
    std::vector<std::size_t> order;
    std::vector<bool> used(pts.size());
    Point2 cur = start;
    for (std::size_t s = 0; s < pts.size(); ++s) {
        std::size_t b = 0;
        double bd = 1e300;
        for (std::size_t i = 0; i < pts.size(); ++i)
            if (!used[i] && dsa::distance(cur, pts[i]) < bd) bd = dsa::distance(cur, pts[i]), b = i;
        used[b] = true;
        order.push_back(b);
        cur = pts[b];
    }
    return route_length(start, pts, order);
}

static bool is_permutation_of_n(std::vector<std::size_t> v, std::size_t n)
{
    std::sort(v.begin(), v.end());
    for (std::size_t i = 0; i < v.size(); ++i)
        if (v[i] != i) return false;
    return v.size() == n;
}

TEST("T10 route order: optimal on simple shapes")
{
    // кути квадрата у «перемішаному» порядку — оптимум = периметр 4000
    const std::vector<Point2> sq{{1000, 1000}, {0, 1000}, {1000, 0}};
    const auto o = plan_visit_order({0, 0}, sq);
    CHECK(is_permutation_of_n(o, 3));
    CHECK_NEAR(route_length({0, 0}, sq, o), 4000.0, 1e-6);

    // точки на колі, перемішані: оптимум — обхід по колу
    std::vector<Point2> circle;
    for (int i = 0; i < 12; ++i) {
        const double a = (i * 5 % 12) * 2 * M_PI / 12; // 5 взаємно просте з 12 -> перемішування
        circle.push_back({1000 * std::cos(a), 1000 * std::sin(a)});
    }
    const Point2 st{1000, 0};
    const auto oc = plan_visit_order(st, circle);
    CHECK(is_permutation_of_n(oc, 12));
    CHECK_NEAR(route_length(st, circle, oc), 12 * 2 * 1000 * std::sin(M_PI / 12), 1e-6);
}

TEST("T10 route order: better than greedy on random")
{
    std::mt19937 rng(9);
    std::uniform_real_distribution<double> u(0, 10'000);
    double sum_ratio = 0;
    for (int trial = 0; trial < 20; ++trial) {
        std::vector<Point2> pts(40);
        for (auto& p : pts) p = {u(rng), u(rng)};
        const auto o = plan_visit_order({5000, 5000}, pts);
        CHECK(is_permutation_of_n(o, 40));
        const double len = route_length({5000, 5000}, pts, o);
        const double greedy = greedy_length({5000, 5000}, pts);
        CHECK(len <= greedy + 1e-6);
        sum_ratio += len / greedy;
    }
    CHECK(sum_ratio / 20 < 0.97); // 2-opt у середньому дає помітне покращення
}

TEST_MAIN()
