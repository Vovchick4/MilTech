#pragma once
// Пошук шляху на сітці: BFS, Dijkstra, A*.
//
// Карта — сітка клітинок (наприклад 100×100 м кожна):
//   '.'  вільно, вартість 1
//   '~'  «дорога» клітинка (сильний вітер / погане покриття), вартість 3
//   '#'  заборонена зона (no-fly zone), пройти не можна
//   'S'  старт, 'G'  ціль (вартість 1)
//
// | алгоритм  | враховує вартість | оптимальний | що розкриває першим        |
// |-----------|-------------------|-------------|----------------------------|
// | BFS       | ні                | за кроками  | найближчі за кількістю кроків |
// | Dijkstra  | так               | так         | найдешевші від старту      |
// | A*        | так               | так*        | найменше g + h (до цілі)   |
// * якщо евристика h не переоцінює (admissible).
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

namespace dsa {

inline constexpr double kSqrt2 = 1.41421356237309504880;

struct Cell {
    int r = 0;
    int c = 0;
    bool operator==(const Cell& o) const { return r == o.r && c == o.c; }
    bool operator!=(const Cell& o) const { return !(*this == o); }
};

class Grid {
public:
    static constexpr std::uint8_t BLOCKED = 0;

    Grid(int rows, int cols) : rows_(rows), cols_(cols), cost_(static_cast<std::size_t>(rows) * cols, 1) {}

    static Grid from_ascii(const std::vector<std::string>& lines, Cell* start = nullptr, Cell* goal = nullptr)
    {
        if (lines.empty()) throw std::invalid_argument("empty map");
        Grid g(static_cast<int>(lines.size()), static_cast<int>(lines[0].size()));
        for (int r = 0; r < g.rows_; ++r) {
            if (static_cast<int>(lines[r].size()) != g.cols_) throw std::invalid_argument("ragged map");
            for (int c = 0; c < g.cols_; ++c) {
                const char ch = lines[r][c];
                std::uint8_t cost = 1;
                if (ch == '#') cost = BLOCKED;
                else if (ch == '~') cost = 3;
                else if (ch == 'S' && start) *start = {r, c};
                else if (ch == 'G' && goal) *goal = {r, c};
                g.set_cost({r, c}, cost);
            }
        }
        return g;
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }
    bool inside(Cell p) const { return p.r >= 0 && p.c >= 0 && p.r < rows_ && p.c < cols_; }
    bool passable(Cell p) const { return inside(p) && cost(p) != BLOCKED; }
    std::uint8_t cost(Cell p) const { return cost_[index(p)]; }
    void set_cost(Cell p, std::uint8_t v) { cost_[index(p)] = v; }
    std::size_t index(Cell p) const { return static_cast<std::size_t>(p.r) * cols_ + p.c; }
    Cell cell(std::size_t i) const { return {static_cast<int>(i / cols_), static_cast<int>(i % cols_)}; }
    std::size_t size() const { return cost_.size(); }

private:
    int rows_, cols_;
    std::vector<std::uint8_t> cost_;
};

struct PathResult {
    std::vector<Cell> path;  // від старту до цілі включно; порожній — шляху немає
    double cost = 0.0;       // сума вартостей (BFS — кількість кроків)
    std::size_t expanded = 0; // скільки вузлів розкрито — міра «роботи» алгоритму
    bool found() const { return !path.empty(); }
};

namespace detail {

inline std::vector<Cell> rebuild(const Grid& g, const std::vector<std::int64_t>& parent, Cell goal)
{
    std::vector<Cell> path;
    for (std::int64_t i = static_cast<std::int64_t>(g.index(goal)); i >= 0; i = parent[i]) {
        path.push_back(g.cell(static_cast<std::size_t>(i)));
    }
    std::reverse(path.begin(), path.end());
    return path;
}

struct Move {
    int dr, dc;
    double len; // 1 або √2 для діагоналі
};

inline const std::vector<Move>& moves(bool diagonal)
{
    static const std::vector<Move> four{{-1, 0, 1}, {1, 0, 1}, {0, -1, 1}, {0, 1, 1}};
    static const std::vector<Move> eight{{-1, 0, 1}, {1, 0, 1}, {0, -1, 1}, {0, 1, 1},
                                         {-1, -1, kSqrt2}, {-1, 1, kSqrt2}, {1, -1, kSqrt2}, {1, 1, kSqrt2}};
    return diagonal ? eight : four;
}

} // namespace detail

// BFS: черга FIFO. Знаходить шлях з найменшою КІЛЬКІСТЮ кроків, вартість клітинок ігнорує.
inline PathResult bfs(const Grid& g, Cell start, Cell goal)
{
    PathResult res;
    if (!g.passable(start) || !g.passable(goal)) return res;

    std::vector<std::int64_t> parent(g.size(), -1);
    std::vector<bool> seen(g.size(), false);
    std::queue<Cell> q;
    q.push(start);
    seen[g.index(start)] = true;

    while (!q.empty()) {
        const Cell cur = q.front();
        q.pop();
        ++res.expanded;
        if (cur == goal) {
            res.path = detail::rebuild(g, parent, goal);
            res.cost = static_cast<double>(res.path.size() - 1);
            return res;
        }
        for (const auto& m : detail::moves(false)) {
            const Cell nb{cur.r + m.dr, cur.c + m.dc};
            if (!g.passable(nb) || seen[g.index(nb)]) continue;
            seen[g.index(nb)] = true;
            parent[g.index(nb)] = static_cast<std::int64_t>(g.index(cur));
            q.push(nb);
        }
    }
    return res;
}

// Евристика для A*: оцінка вартості до цілі, яка НЕ перевищує справжню (мінімальна вартість клітинки = 1).
inline double heuristic(Cell a, Cell b, bool diagonal)
{
    const double dr = std::abs(a.r - b.r);
    const double dc = std::abs(a.c - b.c);
    if (!diagonal) return dr + dc;                              // Манхеттен
    return (dr + dc) + (kSqrt2 - 2.0) * std::min(dr, dc);      // октильна відстань
}

// Спільна реалізація: weight = 0 -> Dijkstra, weight = 1 -> A*, weight > 1 -> «жадібний» A* (швидше, не оптимально).
inline PathResult best_first(const Grid& g, Cell start, Cell goal, double h_weight, bool diagonal)
{
    PathResult res;
    if (!g.passable(start) || !g.passable(goal)) return res;

    constexpr double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(g.size(), INF);
    std::vector<std::int64_t> parent(g.size(), -1);
    std::vector<bool> closed(g.size(), false);

    struct Item {
        double f;
        std::size_t idx;
        bool operator>(const Item& o) const { return f > o.f; }
    };
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open; // min-heap за f

    dist[g.index(start)] = 0.0;
    open.push({h_weight * heuristic(start, goal, diagonal), g.index(start)});

    while (!open.empty()) {
        const Item it = open.top();
        open.pop();
        if (closed[it.idx]) continue; // застарілий запис (lazy deletion)
        closed[it.idx] = true;
        ++res.expanded;

        const Cell cur = g.cell(it.idx);
        if (cur == goal) {
            res.path = detail::rebuild(g, parent, goal);
            res.cost = dist[it.idx];
            return res;
        }
        for (const auto& m : detail::moves(diagonal)) {
            const Cell nb{cur.r + m.dr, cur.c + m.dc};
            if (!g.passable(nb)) continue;
            // діагональ не «зрізає» кут заборонених клітинок
            if (m.dr != 0 && m.dc != 0 &&
                (!g.passable({cur.r + m.dr, cur.c}) || !g.passable({cur.r, cur.c + m.dc}))) {
                continue;
            }
            const std::size_t ni = g.index(nb);
            const double nd = dist[it.idx] + m.len * g.cost(nb);
            if (nd < dist[ni]) {
                dist[ni] = nd;
                parent[ni] = static_cast<std::int64_t>(it.idx);
                open.push({nd + h_weight * heuristic(nb, goal, diagonal), ni});
            }
        }
    }
    return res;
}

inline PathResult dijkstra(const Grid& g, Cell s, Cell t, bool diagonal = false) { return best_first(g, s, t, 0.0, diagonal); }
inline PathResult astar(const Grid& g, Cell s, Cell t, bool diagonal = false) { return best_first(g, s, t, 1.0, diagonal); }

} // namespace dsa
