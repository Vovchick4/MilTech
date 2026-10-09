// Урок 5. Вправи. Заповни TODO.
//   g++ -std=c++17 -Wall exercises.cpp -o exercises && ./exercises

#include <iostream>
#include <queue>
#include <stack>
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

// 5.1 Розвернути маршрут за допомогою СТЕКУ (не через reversed з уроку 2!).
// Поклади всі елементи в std::stack, потім знімай їх у результат.
std::vector<std::string> reverse_route(const std::vector<std::string>& route)
{
    std::vector<std::string> result;
    // TODO
    (void)route;
    return result;
}

// 5.2 Скасування дій (Ctrl+Z). Оператор виконав дії по черзі, потім натиснув «скасувати» k разів.
// Повернути дії, що ЛИШИЛИСЬ, у початковому порядку.
// {"A","B","C","D"}, k = 1 -> {"A","B","C"}
// Якщо k більше за кількість дій — порожньо.
// Використай std::stack: поклади всі дії, зніми k штук, а що лишилось — дістань і розверни.
std::vector<std::string> undo(const std::vector<std::string>& actions, int k)
{
    std::vector<std::string> result;
    // TODO
    (void)actions;
    (void)k;
    return result;
}

// 5.3 Чи правильно розставлені дужки? Дужки бувають ( ) і [ ].
// "([])" -> true, "([)]" -> false, "((" -> false, ")" -> false, "" -> true
// Алгоритм: відкриваюча — в стек. Закриваюча — стек не порожній і на вершині ПАРНА відкриваюча,
// тоді pop; інакше — false. Наприкінці стек має бути порожній.
bool brackets_ok(const std::string& text)
{
    // TODO
    (void)text;
    return false;
}

// 5.4 Дрони передають телеметрію ПО ЧЕРЗІ по колу (round-robin):
// перший у черзі передає, потім стає в КІНЕЦЬ черги.
// Повернути імена тих, хто передавав, для перших k передач.
// {"A","B","C"}, k = 5 -> {"A","B","C","A","B"}
std::vector<std::string> round_robin(const std::vector<std::string>& drones, int k)
{
    std::vector<std::string> result;
    // TODO: std::queue
    (void)drones;
    (void)k;
    return result;
}

int main()
{
    check(reverse_route({"HOME", "P1", "P2"}) == std::vector<std::string>({"P2", "P1", "HOME"}), "5.1 reverse_route");
    check(reverse_route({}).empty(), "5.1 порожній маршрут");

    check(undo({"A", "B", "C", "D"}, 1) == std::vector<std::string>({"A", "B", "C"}), "5.2 undo 1");
    check(undo({"A", "B", "C", "D"}, 3) == std::vector<std::string>({"A"}), "5.2 undo 3");
    check(undo({"A", "B"}, 5).empty(), "5.2 undo більше, ніж дій");
    check(undo({"A", "B"}, 0) == std::vector<std::string>({"A", "B"}), "5.2 undo 0");

    check(brackets_ok("([])") == true, "5.3 ([])");
    check(brackets_ok("([)]") == false, "5.3 ([)]");
    check(brackets_ok("((") == false, "5.3 ((");
    check(brackets_ok(")") == false, "5.3 )");
    check(brackets_ok("") == true, "5.3 порожній рядок");
    check(brackets_ok("speed*(2+[x-1])") == true, "5.3 з іншими символами");

    check(round_robin({"A", "B", "C"}, 5) == std::vector<std::string>({"A", "B", "C", "A", "B"}), "5.4 round_robin");
    check(round_robin({"Solo"}, 3) == std::vector<std::string>({"Solo", "Solo", "Solo"}), "5.4 один дрон");

    std::cout << "\nПомилок: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
