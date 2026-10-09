// Урок 8. Граф на карті і пошук у ширину (BFS): найкоротший шлях в обхід перешкод.
//   g++ -std=c++17 -Wall example.cpp -o example && ./example
//
// Карта: '.' — вільно, '#' — заборонена зона, 'S' — старт, 'G' — ціль.
// Дрон може рухатись на 1 клітинку вгору/вниз/вліво/вправо.

#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Cell {
    int row;
    int col;
};

// Зсуви до 4 сусідів: вгору, вниз, вліво, вправо.
const int DR[4] = {-1, 1, 0, 0};
const int DC[4] = {0, 0, -1, 1};

bool is_free(const std::vector<std::string>& map, int r, int c)
{
    if (r < 0 || c < 0 || r >= (int)map.size() || c >= (int)map[0].size()) {
        return false; // за межами карти
    }
    return map[r][c] != '#';
}

int main()
{
    std::vector<std::string> map = {
        "S...#.....",
        ".##.#.###.",
        ".#..#...#.",
        ".#.##.#.#.",
        "......#..G",
    };
    int rows = (int)map.size();
    int cols = (int)map[0].size();
    Cell start = {0, 0};
    Cell goal = {4, 9};

    // dist[r][c] — за скільки кроків можна дістатися клітинки; -1 = ще не були там
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
    // parent[r][c] — з якої клітинки ми прийшли (щоб потім відновити шлях)
    std::vector<std::vector<Cell>> parent(rows, std::vector<Cell>(cols, Cell{-1, -1}));

    std::queue<Cell> q;
    q.push(start);
    dist[start.row][start.col] = 0;

    // ---------------- BFS ----------------
    while (!q.empty()) {
        Cell cur = q.front();
        q.pop();
        for (int k = 0; k < 4; k++) {
            int nr = cur.row + DR[k];
            int nc = cur.col + DC[k];
            if (is_free(map, nr, nc) && dist[nr][nc] == -1) { // вільна і ще не відвідана
                dist[nr][nc] = dist[cur.row][cur.col] + 1;
                parent[nr][nc] = cur;
                q.push(Cell{nr, nc});
            }
        }
    }

    // «Хвиля»: скільки кроків до кожної клітинки (остання цифра числа, щоб влізло)
    std::cout << "Хвиля BFS (кроків від старту, остання цифра):\n";
    for (int r = 0; r < rows; r++) {
        std::cout << "  ";
        for (int c = 0; c < cols; c++) {
            if (map[r][c] == '#') std::cout << '#';
            else if (dist[r][c] == -1) std::cout << '?';
            else std::cout << dist[r][c] % 10;
        }
        std::cout << "\n";
    }

    if (dist[goal.row][goal.col] == -1) {
        std::cout << "До цілі не долетіти!\n";
        return 0;
    }
    std::cout << "\nНайкоротший шлях: " << dist[goal.row][goal.col] << " кроків\n";

    // Відновлюємо шлях ВІД ЦІЛІ ДО СТАРТУ по parent і малюємо '*'
    std::vector<std::string> drawn = map;
    Cell cur = parent[goal.row][goal.col];
    while (!(cur.row == start.row && cur.col == start.col)) {
        drawn[cur.row][cur.col] = '*';
        cur = parent[cur.row][cur.col];
    }
    for (int r = 0; r < rows; r++) {
        std::cout << "  " << drawn[r] << "\n";
    }
    return 0;
}
