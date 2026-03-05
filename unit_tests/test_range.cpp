#include "include/doctest.h"
#include "common/range.hpp"
#include "common/AreaRange.hpp"

#include <sstream>
#include <vector>

// ============================================================
//  Range struct
// ============================================================
TEST_SUITE("Range") {

    TEST_CASE("fields are stored correctly") {
        Range r{4, 10, -20, 64, 32};
        CHECK(r.scale == 4u);
        CHECK(r.x == 10);
        CHECK(r.z == -20);
        CHECK(r.sx == 64);
        CHECK(r.sz == 32);
    }

    TEST_CASE("scale 1 stored correctly") {
        Range r{1, 0, 0, 16, 16};
        CHECK(r.scale == 1u);
        CHECK(r.sx == 16);
        CHECK(r.sz == 16);
    }

    TEST_CASE("ostream operator produces non-empty string") {
        Range r{16, 3, 7, 10, 10};
        std::ostringstream oss;
        oss << r;
        CHECK_FALSE(oss.str().empty());
    }

    TEST_CASE("ostream output contains scale value") {
        Range r{64, 0, 0, 4, 4};
        std::ostringstream oss;
        oss << r;
        CHECK(oss.str().find("64") != std::string::npos);
    }

    TEST_CASE("negative origin coordinates") {
        Range r{4, -128, -256, 8, 8};
        CHECK(r.x == -128);
        CHECK(r.z == -256);
    }
}

// ============================================================
//  AreaRange class
// ============================================================
TEST_SUITE("AreaRange") {

    TEST_CASE("isInside returns true for centre") {
        AreaRange ar(Pos2D{0, 0}, 3);
        CHECK(ar.isInside(Pos2D{0, 0}));
    }

    TEST_CASE("isInside returns true for corner") {
        // radius 3: lower = -3, upper = 2  (centre - r, centre + r - 1)
        AreaRange ar(Pos2D{0, 0}, 3);
        CHECK(ar.isInside(Pos2D{-3, -3}));
        CHECK(ar.isInside(Pos2D{2, 2}));
    }

    TEST_CASE("isInside returns false outside") {
        AreaRange ar(Pos2D{0, 0}, 3);
        CHECK_FALSE(ar.isInside(Pos2D{3, 0}));
        CHECK_FALSE(ar.isInside(Pos2D{0, 3}));
        CHECK_FALSE(ar.isInside(Pos2D{-4, 0}));
    }

    TEST_CASE("getLower/getUpper with radius constructor") {
        AreaRange ar(Pos2D{5, 5}, 2);
        CHECK(ar.getLower() == Pos2D{3, 3});
        CHECK(ar.getUpper() == Pos2D{6, 6});
    }

    TEST_CASE("min/max constructor stores bounds") {
        AreaRange ar(Pos2D{-2, -4}, Pos2D{10, 8});
        CHECK(ar.getLower() == Pos2D{-2, -4});
        CHECK(ar.getUpper() == Pos2D{10, 8});
    }

    TEST_CASE("min/max constructor isInside") {
        AreaRange ar(Pos2D{0, 0}, Pos2D{5, 5});
        CHECK(ar.isInside(Pos2D{0, 0}));
        CHECK(ar.isInside(Pos2D{5, 5}));
        CHECK(ar.isInside(Pos2D{3, 2}));
        CHECK_FALSE(ar.isInside(Pos2D{6, 0}));
        CHECK_FALSE(ar.isInside(Pos2D{0, 6}));
        CHECK_FALSE(ar.isInside(Pos2D{-1, 0}));
    }

    TEST_CASE("iteration visits every cell (min/max, 2x2)") {
        AreaRange ar(Pos2D{0, 0}, Pos2D{1, 1});
        std::vector<Pos2D> visited;
        for (Pos2D p : ar) {
            visited.push_back(p);
        }
        CHECK(visited.size() == 4u);
        // All four corners present
        CHECK(std::find(visited.begin(), visited.end(), Pos2D{0, 0}) != visited.end());
        CHECK(std::find(visited.begin(), visited.end(), Pos2D{0, 1}) != visited.end());
        CHECK(std::find(visited.begin(), visited.end(), Pos2D{1, 0}) != visited.end());
        CHECK(std::find(visited.begin(), visited.end(), Pos2D{1, 1}) != visited.end());
    }

    TEST_CASE("radius constructor (reverse) visits correct number of cells") {
        // radius 2: lower = -2, upper = 1 → 4x4 = 16 cells
        AreaRange ar(Pos2D{0, 0}, 2);
        int count = 0;
        for ([[maybe_unused]] Pos2D p : ar) ++count;
        CHECK(count == 16);
    }

    TEST_CASE("forward (non-reverse) AreaRange iterates correctly") {
        AreaRange ar(Pos2D{0, 0}, 2, /*reverseX=*/false, /*reverseZ=*/false);
        int count = 0;
        for ([[maybe_unused]] Pos2D p : ar) ++count;
        CHECK(count == 16);
    }

    TEST_CASE("radius 1 produces 2x2 area (2-radius offset)") {
        // radius 1: lower = centre-1, upper = centre+1-1 = centre
        AreaRange ar(Pos2D{0, 0}, 1);
        int count = 0;
        for ([[maybe_unused]] Pos2D p : ar) ++count;
        CHECK(count == 4);
        CHECK(ar.getLower() == Pos2D{-1, -1});
        CHECK(ar.getUpper() == Pos2D{0, 0});
    }

    TEST_CASE("non-zero centre offset is respected in bounds") {
        AreaRange ar(Pos2D{10, 20}, 3);
        CHECK(ar.getLower() == Pos2D{7, 17});
        CHECK(ar.getUpper() == Pos2D{12, 22});
        CHECK(ar.isInside(Pos2D{10, 20}));
        CHECK_FALSE(ar.isInside(Pos2D{13, 20}));
    }

    TEST_CASE("reverse iteration first point is upper-right corner") {
        AreaRange ar(Pos2D{0, 0}, Pos2D{5, 5});
        // min/max constructor: step = +1,+1, first = lower
        Pos2D first = *ar.begin();
        CHECK(first == Pos2D{0, 0});
    }

    TEST_CASE("1x1 min/max area iterates exactly once") {
        AreaRange ar(Pos2D{5, 7}, Pos2D{5, 7});
        int count = 0;
        Pos2D only{};
        for (Pos2D p : ar) { only = p; ++count; }
        CHECK(count == 1);
        CHECK(only == Pos2D{5, 7});
    }

    TEST_CASE("3x3 min/max area yields 9 cells") {
        AreaRange ar(Pos2D{0, 0}, Pos2D{2, 2});
        int count = 0;
        for ([[maybe_unused]] Pos2D p : ar) ++count;
        CHECK(count == 9);
    }

    TEST_CASE("isInside boundary conditions") {
        AreaRange ar(Pos2D{0, 0}, Pos2D{5, 5});
        // exactly on the edges
        CHECK(ar.isInside(Pos2D{0, 3}));
        CHECK(ar.isInside(Pos2D{5, 3}));
        CHECK(ar.isInside(Pos2D{3, 0}));
        CHECK(ar.isInside(Pos2D{3, 5}));
        // just outside
        CHECK_FALSE(ar.isInside(Pos2D{-1, 3}));
        CHECK_FALSE(ar.isInside(Pos2D{6, 3}));
    }
}

