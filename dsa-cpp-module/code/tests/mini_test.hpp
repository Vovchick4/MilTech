#pragma once
// Мінімальний тестовий фреймворк без залежностей (замість GoogleTest/Catch2),
// щоб модуль збирався будь-де однією командою.
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace mini {

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& registry()
{
    static std::vector<TestCase> r;
    return r;
}
inline int& failures()
{
    static int f = 0;
    return f;
}

struct Registrar {
    Registrar(const char* name, std::function<void()> fn) { registry().push_back({name, std::move(fn)}); }
};

inline int run_all(const std::string& filter = "")
{
    int failed_tests = 0, run = 0;
    for (auto& t : registry()) {
        if (!filter.empty() && t.name.find(filter) == std::string::npos) continue;
        ++run;
        const int before = failures();
        try {
            t.fn();
        } catch (const std::exception& e) {
            std::cout << "    exception: " << e.what() << "\n";
            ++failures();
        }
        const bool ok = failures() == before;
        if (!ok) ++failed_tests;
        std::cout << (ok ? "[  OK  ] " : "[ FAIL ] ") << t.name << "\n";
    }
    std::cout << "\n" << (run - failed_tests) << "/" << run << " tests passed\n";
    return failed_tests == 0 ? 0 : 1;
}

} // namespace mini

#define MT_CAT2(a, b) a##b
#define MT_CAT(a, b) MT_CAT2(a, b)
#define TEST(name)                                                              \
    static void MT_CAT(test_fn_, __LINE__)();                                   \
    static mini::Registrar MT_CAT(test_reg_, __LINE__)(name, MT_CAT(test_fn_, __LINE__)); \
    static void MT_CAT(test_fn_, __LINE__)()

#define CHECK(cond)                                                                       \
    do {                                                                                  \
        if (!(cond)) {                                                                    \
            std::cout << "    " << __FILE__ << ":" << __LINE__ << ": CHECK(" #cond ")\n"; \
            ++mini::failures();                                                           \
        }                                                                                 \
    } while (0)

#define CHECK_EQ(a, b)                                                                              \
    do {                                                                                            \
        const auto& _va = (a);                                                                      \
        const auto& _vb = (b);                                                                      \
        if (!(_va == _vb)) {                                                                        \
            std::cout << "    " << __FILE__ << ":" << __LINE__ << ": " #a " == " #b " -> " << _va \
                      << " != " << _vb << "\n";                                                     \
            ++mini::failures();                                                                     \
        }                                                                                           \
    } while (0)

#define CHECK_NEAR(a, b, eps)                                                                        \
    do {                                                                                             \
        const double _va = (a), _vb = (b);                                                           \
        if (!(std::fabs(_va - _vb) <= (eps))) {                                                      \
            std::cout << "    " << __FILE__ << ":" << __LINE__ << ": " #a " ~ " #b " -> " << _va   \
                      << " vs " << _vb << "\n";                                                      \
            ++mini::failures();                                                                      \
        }                                                                                            \
    } while (0)

#define TEST_MAIN()                                                   \
    int main(int argc, char** argv)                                   \
    {                                                                 \
        return mini::run_all(argc > 1 ? std::string(argv[1]) : "");  \
    }
