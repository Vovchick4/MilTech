// Урок 8. Вправи. Заповни TODO.
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises

#include <iostream>
#include <queue>
#include <string>
#include <vector>

int g_failed = 0;
void check(bool ok, const std::string& name)
{
    if (ok) {
        std::cout << "[OK]   " << name << "\n";
    } else {
        std::cout << "[FAIL] " << name << "\n";
        g_failed = g_failed + 1;
    }
}

struct Cell {
    int row;
    int col;
};

const int DR[4] = {-1, 1, 0, 0};
const int DC[4] = {0, 0, -1, 1};

// Вже готова допоміжна функція: чи клітинка на карті і не '#'.
bool is_free(const std::vector<std::string>& map, int r, int c)
{
    if (r < 0 || c < 0 || r >= (int)map.size() || c >= (int)map[0].size()) {
        return false;
    }
    return map[r][c] != '#';
}

// 8.1 Скільки на карті заборонених клітинок '#'?
int count_blocked(const std::vector<std::string>& map)
{
    // TODO: два вкладені цикли по рядках і стовпцях
    (void)map;
    return 0;
}

// 8.2 Скільки вільних сусідів (з 4) у клітинки (r, c)?
int free_neighbours(const std::vector<std::string>& map, int r, int c)
{
    // TODO: цикл по k від 0 до 3, використовуй DR, DC та is_free
    (void)map;
    (void)r;
    (void)c;
    return 0;
}

// 8.3 Найменша кількість кроків від start до goal, або -1, якщо не дістатися.
// Скопіюй BFS з example.cpp (parent тут не потрібен).
int shortest_steps(const std::vector<std::string>& map, Cell start, Cell goal)
{
    // TODO
    (void)map;
    (void)start;
    (void)goal;
    return -1;
}

// 8.4 Скільки клітинок взагалі досяжні зі start (включно з самим start)?
// Підказка: той самий BFS, просто рахуй кожну клітинку, яку кладеш у чергу.
int reachable_count(const std::vector<std::string>& map, Cell start)
{
    // TODO
    (void)map;
    (void)start;
    return 0;
}

int main()
{
    std::vector<std::string> map = {
        "....#",
        ".##.#",
        "....#",
        "#.#..",
    };

    check(count_blocked(map) == 7, "8.1 count_blocked = 7");
    check(count_blocked({"..", ".."}) == 0, "8.1 без перешкод");

    check(free_neighbours(map, 0, 0) == 2, "8.2 кут (0,0): 2 сусіди");
    check(free_neighbours(map, 2, 1) == 3, "8.2 (2,1): 3 сусіди");
    check(free_neighbours(map, 3, 4) == 1, "8.2 (3,4): 1 сусід");

    check(shortest_steps(map, Cell{0, 0}, Cell{3, 4}) == 7, "8.3 (0,0)->(3,4) = 7");
    check(shortest_steps(map, Cell{0, 0}, Cell{0, 0}) == 0, "8.3 старт = ціль -> 0");
    check(shortest_steps({".#.", ".#.", ".#."}, Cell{0, 0}, Cell{0, 2}) == -1, "8.3 стіна -> -1");

    check(reachable_count(map, Cell{0, 0}) == 13, "8.4 досяжно 13 клітинок");
    check(reachable_count({".#.", ".#.", ".#."}, Cell{0, 2}) == 3, "8.4 за стіною 3 клітинки");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
