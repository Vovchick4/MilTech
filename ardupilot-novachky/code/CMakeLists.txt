# Опис проєкту для CMake. Тобі НЕ треба його змінювати.
# Він каже: «є такі-то програми, кожну збери з такого-то .cpp файлу і підключи бібліотеку MAVSDK».
cmake_minimum_required(VERSION 3.16)
project(ardupilot_novachky CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MAVSDK REQUIRED)

# Кожен рядок нижче = одна програма з уроку.
foreach(program
        lesson04_connect
        lesson05_telemetry
        lesson06_takeoff_land
        lesson07_goto_point
        lesson08_route
        lesson10_final_task)
    add_executable(${program} ${program}.cpp)
    target_link_libraries(${program} MAVSDK::mavsdk)
endforeach()
