#include "include/doctest.h"
#include "common/MathHelper.hpp"

#include <cmath>

TEST_SUITE("MathHelper") {

    TEST_CASE("floor(double) truncates toward negative infinity") {
        CHECK(MathHelper::floor(2.9) == 2);
        CHECK(MathHelper::floor(2.0) == 2);
        CHECK(MathHelper::floor(-2.1) == -3);
        CHECK(MathHelper::floor(-2.0) == -2);
        CHECK(MathHelper::floor(0.0) == 0);
    }

    TEST_CASE("floor(float) truncates toward negative infinity") {
        CHECK(MathHelper::floor(2.9f) == 2);
        CHECK(MathHelper::floor(-0.5f) == -1);
        CHECK(MathHelper::floor(0.0f) == 0);
    }

    TEST_CASE("ceil(double)") {
        CHECK(MathHelper::ceil(2.1) == 3);
        CHECK(MathHelper::ceil(2.0) == 2);
        CHECK(MathHelper::ceil(-2.9) == -2);
        CHECK(MathHelper::ceil(0.0) == 0);
    }

    TEST_CASE("ceil(float)") {
        CHECK(MathHelper::ceil(2.1f) == 3);
        CHECK(MathHelper::ceil(-0.5f) == 0);
    }

    TEST_CASE("clamp") {
        CHECK(MathHelper::clamp(5, 0, 10) == 5);
        CHECK(MathHelper::clamp(-5, 0, 10) == 0);
        CHECK(MathHelper::clamp(15, 0, 10) == 10);
        CHECK(MathHelper::clamp(0, 0, 10) == 0);
        CHECK(MathHelper::clamp(10, 0, 10) == 10);
    }

    TEST_CASE("lerp") {
        CHECK(MathHelper::lerp(0.0, 10.0, 20.0) == doctest::Approx(10.0));
        CHECK(MathHelper::lerp(1.0, 10.0, 20.0) == doctest::Approx(20.0));
        CHECK(MathHelper::lerp(0.5, 10.0, 20.0) == doctest::Approx(15.0));
        CHECK(MathHelper::lerp(0.25, 0.0, 100.0) == doctest::Approx(25.0));
    }

    TEST_CASE("clampedLerp clamps ratio") {
        CHECK(MathHelper::clampedLerp(-1.0, 10.0, 20.0) == doctest::Approx(10.0));
        CHECK(MathHelper::clampedLerp(2.0, 10.0, 20.0) == doctest::Approx(20.0));
        CHECK(MathHelper::clampedLerp(0.5, 10.0, 20.0) == doctest::Approx(15.0));
    }

    TEST_CASE("lerp2D") {
        // All corners equal => result equals that value
        CHECK(MathHelper::lerp2D(0.5, 0.5, 1.0, 1.0, 1.0, 1.0) == doctest::Approx(1.0));
        // Linear gradient
        CHECK(MathHelper::lerp2D(0.0, 0.0, 0.0, 10.0, 20.0, 30.0) == doctest::Approx(0.0));
        CHECK(MathHelper::lerp2D(1.0, 0.0, 0.0, 10.0, 20.0, 30.0) == doctest::Approx(10.0));
        CHECK(MathHelper::lerp2D(0.0, 1.0, 0.0, 10.0, 20.0, 30.0) == doctest::Approx(20.0));
        CHECK(MathHelper::lerp2D(1.0, 1.0, 0.0, 10.0, 20.0, 30.0) == doctest::Approx(30.0));
    }

    TEST_CASE("sin/cos approximate std::sin/cos") {
        // Table-based, so allow some tolerance
        constexpr float epsilon = 0.001f;
        CHECK(MathHelper::sin(0.0f) == doctest::Approx(std::sin(0.0f)).epsilon(epsilon));
        CHECK(MathHelper::cos(0.0f) == doctest::Approx(std::cos(0.0f)).epsilon(epsilon));
        CHECK(MathHelper::sin(1.0f) == doctest::Approx(std::sin(1.0f)).epsilon(epsilon));
        CHECK(MathHelper::cos(1.0f) == doctest::Approx(std::cos(1.0f)).epsilon(epsilon));
    }

    // ----------------------------------------------------------------
    //  lerp3D
    // ----------------------------------------------------------------
    TEST_CASE("lerp3D with all-same values") {
        CHECK(MathHelper::lerp3D(0.5, 0.5, 0.5,
                                  7.0, 7.0, 7.0, 7.0,
                                  7.0, 7.0, 7.0, 7.0) == doctest::Approx(7.0));
    }

    TEST_CASE("lerp3D at (0,0,0) returns topFrontLeft") {
        CHECK(MathHelper::lerp3D(0.0, 0.0, 0.0,
                                  1.0, 2.0, 3.0, 4.0,
                                  5.0, 6.0, 7.0, 8.0) == doctest::Approx(1.0));
    }

    TEST_CASE("lerp3D at (1,0,0) returns topFrontRight") {
        CHECK(MathHelper::lerp3D(1.0, 0.0, 0.0,
                                  1.0, 2.0, 3.0, 4.0,
                                  5.0, 6.0, 7.0, 8.0) == doctest::Approx(2.0));
    }

    TEST_CASE("lerp3D at (0,1,0) returns topBackLeft") {
        CHECK(MathHelper::lerp3D(0.0, 1.0, 0.0,
                                  1.0, 2.0, 3.0, 4.0,
                                  5.0, 6.0, 7.0, 8.0) == doctest::Approx(3.0));
    }

    TEST_CASE("lerp3D at (0,0,1) returns bottomFrontLeft") {
        CHECK(MathHelper::lerp3D(0.0, 0.0, 1.0,
                                  1.0, 2.0, 3.0, 4.0,
                                  5.0, 6.0, 7.0, 8.0) == doctest::Approx(5.0));
    }

    // ----------------------------------------------------------------
    //  clamp (float / double overloads tested via the int one already;
    //         add edge cases)
    // ----------------------------------------------------------------
    TEST_CASE("clamp at boundary min == max") {
        CHECK(MathHelper::clamp(5, 5, 5) == 5);
    }

    // ----------------------------------------------------------------
    //  lerp edge cases
    // ----------------------------------------------------------------
    TEST_CASE("lerp outside [0,1] extrapolates") {
        // ratio = 2 → startValue + 2*(end - start)
        CHECK(MathHelper::lerp(2.0, 0.0, 10.0) == doctest::Approx(20.0));
        CHECK(MathHelper::lerp(-1.0, 0.0, 10.0) == doctest::Approx(-10.0));
    }

    TEST_CASE("lerp with equal start and end") {
        CHECK(MathHelper::lerp(0.5, 42.0, 42.0) == doctest::Approx(42.0));
    }

    // ----------------------------------------------------------------
    //  clampedLerp boundary
    // ----------------------------------------------------------------
    TEST_CASE("clampedLerp exactly at 0 returns start") {
        CHECK(MathHelper::clampedLerp(0.0, 5.0, 15.0) == doctest::Approx(5.0));
    }

    TEST_CASE("clampedLerp exactly at 1 returns end") {
        CHECK(MathHelper::clampedLerp(1.0, 5.0, 15.0) == doctest::Approx(15.0));
    }

    // ----------------------------------------------------------------
    //  sin/cos on more values
    // ----------------------------------------------------------------
    TEST_CASE("sin/cos quarter-turn values approximate std") {
        constexpr float eps = 0.002f;
        constexpr float pi = 3.14159265f;
        CHECK(MathHelper::sin(pi / 2.0f) == doctest::Approx(std::sin(pi / 2.0f)).epsilon(eps));
        CHECK(MathHelper::cos(pi / 2.0f) == doctest::Approx(std::cos(pi / 2.0f)).epsilon(eps));
        CHECK(MathHelper::sin(pi)        == doctest::Approx(std::sin(pi)).epsilon(eps));
        CHECK(MathHelper::cos(pi)        == doctest::Approx(std::cos(pi)).epsilon(eps));
    }

    // ----------------------------------------------------------------
    //  floor edge cases
    // ----------------------------------------------------------------
    TEST_CASE("floor of very small negative") {
        CHECK(MathHelper::floor(-0.0001) == -1);
    }

    TEST_CASE("floor of very small positive") {
        CHECK(MathHelper::floor(0.0001) == 0);
    }

    // ----------------------------------------------------------------
    //  ceil edge cases
    // ----------------------------------------------------------------
    TEST_CASE("ceil of very small negative") {
        CHECK(MathHelper::ceil(-0.0001) == 0);
    }

    TEST_CASE("ceil of very small positive") {
        CHECK(MathHelper::ceil(0.0001) == 1);
    }
}

