// Приклад 3. Планувальник задач на купі (як AP_Scheduler в ArduPilot, спрощено).
// Кожна задача має частоту. Купа завжди дає задачу з найближчим часом запуску: O(log n).
#include <cstdio>
#include <string>
#include <vector>

#include "dsa/binary_heap.hpp"

struct Job {
    long next_run_us;
    int id;
    // Компаратор для min-heap: раніше -> вище. При рівності — менший id (стабільний порядок).
    bool operator<(const Job& o) const { return next_run_us != o.next_run_us ? next_run_us < o.next_run_us : id < o.id; }
};

struct TaskInfo {
    std::string name;
    int rate_hz;
    int runs = 0;
};

int main()
{
    std::vector<TaskInfo> tasks = {
        {"read_imu", 400}, {"attitude_ctrl", 400}, {"update_gps", 50},
        {"send_telemetry", 10}, {"check_battery", 10}, {"log_write", 25}, {"check_failsafe", 1},
    };

    dsa::BinaryHeap<Job> heap;
    for (int i = 0; i < static_cast<int>(tasks.size()); ++i) heap.push({0, i});

    const long SIM_US = 1'000'000; // 1 секунда
    int printed = 0;
    while (heap.top().next_run_us < SIM_US) {
        Job j = heap.pop();
        TaskInfo& t = tasks[j.id];
        ++t.runs;
        if (printed < 12) {
            std::printf("t=%6ld us  run %-15s\n", j.next_run_us, t.name.c_str());
            ++printed;
        }
        j.next_run_us += 1'000'000 / t.rate_hz;
        heap.push(j);
    }

    std::puts("...\n\nЗапусків за 1 с:");
    for (const auto& t : tasks) std::printf("  %-15s %4d Hz -> %4d\n", t.name.c_str(), t.rate_hz, t.runs);
    return 0;
}
