// Приклад 5. Планування маршруту в обхід заборонених зон: BFS vs Dijkstra vs A*.
// Результат — список waypoint'ів (тільки точки повороту) у route.csv,
// який можна підставити в маршрут з модуля ArduPilot + MAVSDK.
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "bench.hpp"
#include "dsa/grid_path.hpp"

constexpr double CELL_M = 100.0;                         // розмір клітинки
constexpr double HOME_LAT = -35.363262, HOME_LON = 149.165237; // home SITL (Canberra)

static const std::vector<std::string> MAP = {
    "..............................####......",
    "..................#####.......####......",
    "....#####.........#####.......####......",
    "....#####...............................",
    "....#####.......~~~~~~~~~~~~............",
    "....#####.......~~~~~~~~~~~~............",
    "....#####.......~~~~~~~~~~~~...#####....",
    "................~~~~~~~~~~~~...#####....",
    "................~~~~~~~~~~~~...#####....",
    "........#####...~~~~~~~~~~~~...#####....",
    "........#####...~~~~~~~~~~~~...#####....",
    "........#####...~~~~~~~~~~~~............",
    "........#####...~~~~~~~~~~~~......###...",
    "................~~~~~~~~~~~~......###...",
    "S...............~~~~~~~~~~~~...........G",
};

static void print_path(const dsa::Grid& g, const dsa::PathResult& r)
{
    std::vector<std::string> canvas = MAP;
    for (std::size_t i = 1; i + 1 < r.path.size(); ++i) canvas[r.path[i].r][r.path[i].c] = '*';
    for (const auto& line : canvas) std::printf("  %s\n", line.c_str());
    (void)g;
}

// Реальна вартість шляху за картою (для BFS вона більша, ніж «кількість кроків»).
static double real_cost(const dsa::Grid& g, const std::vector<dsa::Cell>& p)
{
    double c = 0;
    for (std::size_t i = 1; i < p.size(); ++i) {
        const bool diag = p[i].r != p[i - 1].r && p[i].c != p[i - 1].c;
        c += (diag ? dsa::kSqrt2 : 1.0) * g.cost(p[i]);
    }
    return c;
}

// Залишаємо тільки ті точки, де змінюється напрям руху.
static std::vector<dsa::Cell> turn_points(const std::vector<dsa::Cell>& p)
{
    if (p.size() < 3) return p;
    std::vector<dsa::Cell> out{p.front()};
    for (std::size_t i = 1; i + 1 < p.size(); ++i) {
        const int dr1 = p[i].r - p[i - 1].r, dc1 = p[i].c - p[i - 1].c;
        const int dr2 = p[i + 1].r - p[i].r, dc2 = p[i + 1].c - p[i].c;
        if (dr1 != dr2 || dc1 != dc2) out.push_back(p[i]);
    }
    out.push_back(p.back());
    return out;
}

int main()
{
    dsa::Cell s, t;
    const dsa::Grid g = dsa::Grid::from_ascii(MAP, &s, &t);

    struct Run {
        const char* name;
        dsa::PathResult r;
        double ms;
    };
    std::vector<Run> runs;
    dsa::PathResult tmp;
    double ms;
    ms = bench::ms([&] { tmp = dsa::bfs(g, s, t); });              runs.push_back({"BFS", tmp, ms});
    ms = bench::ms([&] { tmp = dsa::dijkstra(g, s, t); });         runs.push_back({"Dijkstra", tmp, ms});
    ms = bench::ms([&] { tmp = dsa::astar(g, s, t); });            runs.push_back({"A* 4-dir", tmp, ms});
    ms = bench::ms([&] { tmp = dsa::astar(g, s, t, true); });      runs.push_back({"A* 8-dir", tmp, ms});

    std::puts("Алгоритм  | кроків | реальна вартість | розкрито вузлів | час");
    std::puts("----------+--------+------------------+-----------------+--------");
    for (const auto& r : runs) {
        std::printf("%-9s | %6zu | %16.1f | %15zu | %.3f ms\n", r.name, r.r.path.size() - 1,
                    real_cost(g, r.r.path), r.r.expanded, r.ms);
    }
    std::puts("BFS шукає найменше КРОКІВ і летить через '~' (вартість 3); Dijkstra і A* їх обходять.");
    std::puts("A* знаходить той самий оптимум, що й Dijkstra, але розкриває менше вузлів.\n");

    for (const auto& r : runs) {
        std::printf("%s:\n", r.name);
        print_path(g, r.r);
        std::puts("");
    }

    // ---- waypoint'и для дрона ----
    const auto& best = runs[3].r;
    const auto wps = turn_points(best.path);
    std::ofstream csv("route.csv");
    csv << "name,north_m,east_m,lat,lon\n";
    std::printf("Waypoint'и (A* 8-dir), %zu точок повороту замість %zu клітинок:\n", wps.size(), best.path.size());
    for (std::size_t i = 0; i < wps.size(); ++i) {
        const double north = -(wps[i].r - s.r) * CELL_M; // рядок вниз = південь
        const double east = (wps[i].c - s.c) * CELL_M;
        const double lat = HOME_LAT + north / 6371000.0 * 180.0 / M_PI;
        const double lon = HOME_LON + east / (6371000.0 * std::cos(HOME_LAT * M_PI / 180.0)) * 180.0 / M_PI;
        std::printf("  WP%-2zu north=%7.0f m east=%7.0f m  (%.7f, %.7f)\n", i, north, east, lat, lon);
        csv << "WP" << i << ',' << north << ',' << east << ',' << std::to_string(lat) << ',' << std::to_string(lon) << '\n';
    }
    std::puts("\nЗбережено route.csv");
    return 0;
}
