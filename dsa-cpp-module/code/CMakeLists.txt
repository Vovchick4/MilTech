cmake_minimum_required(VERSION 3.16)
project(dsa_cpp_module LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)   # бенчмарки мають сенс тільки з оптимізацією
endif()

option(DSA_SANITIZE "AddressSanitizer + UBSan (шукати вихід за межі масиву, use-after-free)" OFF)
option(DSA_USE_SOLUTIONS "Збирати задачі з розв'язками викладача замість заготовок" OFF)

add_compile_options(-Wall -Wextra -Wpedantic)
if(DSA_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# --- код з лекцій ---
add_library(dsa INTERFACE)
target_include_directories(dsa INTERFACE include)

# --- задачі студентів (або розв'язки) ---
add_library(tasks INTERFACE)
if(DSA_USE_SOLUTIONS AND EXISTS ${CMAKE_SOURCE_DIR}/solutions/include)
    message(STATUS "Tasks: SOLUTIONS")
    target_include_directories(tasks INTERFACE solutions/include)
else()
    message(STATUS "Tasks: student skeletons (tasks/include)")
    target_include_directories(tasks INTERFACE tasks/include)
endif()
target_link_libraries(tasks INTERFACE dsa)

# --- приклади ---
foreach(ex 01_bench_containers 02_telemetry_filters 03_scheduler_heap
           04_params_lookup 05_route_planner 06_sort_bench)
    add_executable(${ex} examples/${ex}.cpp)
    target_link_libraries(${ex} PRIVATE dsa)
endforeach()

# --- тести ---
enable_testing()
add_executable(test_lecture tests/test_lecture.cpp)
target_link_libraries(test_lecture PRIVATE dsa)
target_include_directories(test_lecture PRIVATE tests)
add_test(NAME lecture COMMAND test_lecture)

add_executable(test_tasks tests/test_tasks.cpp)
target_link_libraries(test_tasks PRIVATE tasks)
target_include_directories(test_tasks PRIVATE tests)
add_test(NAME tasks COMMAND test_tasks)
set_tests_properties(tasks PROPERTIES TIMEOUT 60)
