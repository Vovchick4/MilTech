// Урок 5. Стек (stack) і черга (queue).
//   g++ -std=c++17 -Wall example.cpp -o example && ./example

#include <iostream>
#include <queue>
#include <stack>
#include <string>

int main()
{
    // ------------------------------------------------------------
    // ЧЕРГА (FIFO — first in, first out: хто перший прийшов, той перший вийшов)
    // Команди для дрона виконуються в тому порядку, в якому надійшли.
    // ------------------------------------------------------------
    std::queue<std::string> commands;
    commands.push("ARM");
    commands.push("TAKEOFF 20m");
    commands.push("GOTO P1");
    commands.push("GOTO P2");
    commands.push("LAND");

    std::cout << "Черга команд, розмір " << commands.size() << "\n";
    while (!commands.empty()) {
        std::string cmd = commands.front(); // подивитись, хто ПЕРШИЙ
        commands.pop();                     // забрати його з черги
        std::cout << "  виконую: " << cmd << "   (лишилось " << commands.size() << ")\n";
    }

    // ------------------------------------------------------------
    // СТЕК (LIFO — last in, first out: останній поклав — першим забрав, як стопка тарілок)
    // Запам'ятовуємо точки, через які летіли, щоб повернутися ТИМ САМИМ шляхом.
    // ------------------------------------------------------------
    std::stack<std::string> path;
    std::cout << "\nЛетимо вперед:\n";
    std::string points[] = {"HOME", "Міст", "Ліс", "Поле", "Ціль"};
    for (int i = 0; i < 5; i++) {
        path.push(points[i]);
        std::cout << "  пролетіли " << points[i] << "  (у стеку " << path.size() << ")\n";
    }

    std::cout << "\nПовертаємось (у зворотному порядку):\n";
    while (!path.empty()) {
        std::cout << "  " << path.top() << "\n"; // подивитись ВЕРХНІЙ
        path.pop();                              // зняти верхній
    }

    // ------------------------------------------------------------
    // Типова помилка: front()/top() на ПОРОЖНІЙ черзі/стеку — невизначена поведінка (краш).
    // Завжди перевіряй empty() перед front()/top()/pop().
    // ------------------------------------------------------------
    std::cout << "\nСтек порожній? " << (path.empty() ? "так" : "ні") << "\n";
    return 0;
}
