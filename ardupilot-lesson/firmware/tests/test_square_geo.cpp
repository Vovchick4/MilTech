// Задача 13: модульний тест (gtest) для геометрії квадрата.
// Куди: ~/ardupilot/libraries/AP_Common/tests/test_square_geo.cpp
// Збірка і запуск (WSL Ubuntu):
//   cd ~/ardupilot
//   ./waf configure --board sitl
//   ./waf tests
//   ./build/sitl/tests/test_square_geo
#include <AP_gtest.h>
#include <AP_Common/Location.h>
#include <AP_Math/AP_Math.h>
#include <AP_AHRS/AP_AHRS.h>
#include <AP_Terrain/AP_Terrain.h>
#include <GCS_MAVLink/GCS_Dummy.h>

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

// Те саме, що робить ModeSquare::go_to_corner()
static Location corner(const Location& origin, float side_m, int n, int e)
{
    Location l = origin;
    l.offset(n * side_m, e * side_m);
    return l;
}

TEST(SquareGeo, SideLength)
{
    const Location home{-353632620, 1491652370, 58400, Location::AltFrame::ABSOLUTE}; // Canberra SITL
    const float side = 2500.0f;                                                       // 10 км периметр

    const Location p1 = corner(home, side, 1, 0);
    const Location p2 = corner(home, side, 1, 1);
    const Location p3 = corner(home, side, 0, 1);

    EXPECT_NEAR(side, home.get_distance(p1), 1.0);
    EXPECT_NEAR(side, p1.get_distance(p2), 1.0);
    EXPECT_NEAR(side, p2.get_distance(p3), 1.0);
    EXPECT_NEAR(side, p3.get_distance(home), 1.0);

    // діагональ = side * sqrt(2)
    EXPECT_NEAR(side * M_SQRT2, home.get_distance(p2), 2.0);
}

TEST(SquareGeo, NorthIsNorth)
{
    const Location home{504501000, 305234000, 18000, Location::AltFrame::ABSOLUTE}; // Київ
    const Location p1 = corner(home, 1000.0f, 1, 0);
    EXPECT_GT(p1.lat, home.lat);   // на північ — широта більша
    EXPECT_EQ(p1.lng, home.lng);   // довгота не змінилась
}

// TODO студенту: тест, що 1 км на схід у Києві — це більше градусів довготи, ніж на екваторі

AP_GTEST_MAIN()
