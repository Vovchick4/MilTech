cmake_minimum_required(VERSION 3.16)

project(ardupilot_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # для IntelliSense у VS Code

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# MAVSDK встановлено в /usr/local (make install).
# Нова MAVSDK експортує ОДИН target MAVSDK::mavsdk — усі плагіни
# (telemetry, action, mission ...) вже всередині нього.
find_package(MAVSDK REQUIRED)

# Головна програма — великий маршрут
add_executable(ardupilot_cpp main.cpp)
target_link_libraries(ardupilot_cpp PRIVATE MAVSDK::mavsdk)

# Навчальні приклади (крок за кроком)
add_executable(01_telemetry examples/01_telemetry.cpp)
target_link_libraries(01_telemetry PRIVATE MAVSDK::mavsdk)

add_executable(02_takeoff_land examples/02_takeoff_land.cpp)
target_link_libraries(02_takeoff_land PRIVATE MAVSDK::mavsdk)
