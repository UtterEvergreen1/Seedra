#include "include/doctest.h"
#include "common/Pos2DTemplate.hpp"
#include "common/Pos3DTemplate.hpp"
#include <sstream>
#include <string>

TEST_SUITE("Pos2D") {

    TEST_CASE("default constructor is zero") {
        Pos2D p;
        CHECK(p.x == 0);
        CHECK(p.z == 0);
    }

    TEST_CASE("parameterized constructor") {
        Pos2D p(3, 7);
        CHECK(p.x == 3);
        CHECK(p.z == 7);
    }

    TEST_CASE("equality") {
        CHECK(Pos2D(1, 2) == Pos2D(1, 2));
        CHECK_FALSE(Pos2D(1, 2) == Pos2D(3, 4));
    }

    TEST_CASE("inequality") {
        CHECK(Pos2D(1, 2) != Pos2D(3, 4));
        CHECK_FALSE(Pos2D(1, 2) != Pos2D(1, 2));
    }

    TEST_CASE("addition") {
        Pos2D a(1, 2);
        Pos2D b(3, 4);
        Pos2D c = a + b;
        CHECK(c.x == 4);
        CHECK(c.z == 6);
    }

    TEST_CASE("addition with int") {
        Pos2D p(5, 10);
        Pos2D r = p + 3;
        CHECK(r.x == 8);
        CHECK(r.z == 13);
    }

    TEST_CASE("multiplication") {
        Pos2D a(2, 3);
        Pos2D b(4, 5);
        Pos2D c = a * b;
        CHECK(c.x == 8);
        CHECK(c.z == 15);
    }

    TEST_CASE("multiplication with int") {
        Pos2D p(3, 7);
        Pos2D r = p * 2;
        CHECK(r.x == 6);
        CHECK(r.z == 14);
    }
}

TEST_SUITE("Pos3D") {

    TEST_CASE("default constructor is zero") {
        Pos3D p;
        CHECK(p.x == 0);
        CHECK(p.z == 0);
    }

    TEST_CASE("parameterized constructor") {
        Pos3D p(1, 2, 3);
        CHECK(p.x == 1);
        CHECK(p.z == 3);
    }

    TEST_CASE("equality") {
        CHECK(Pos3D(1, 2, 3) == Pos3D(1, 2, 3));
        CHECK_FALSE(Pos3D(1, 2, 3) == Pos3D(4, 5, 6));
    }

    TEST_CASE("subtraction") {
        Pos2D a(10, 8);
        Pos2D b(3, 5);
        Pos2D c = a - b;
        CHECK(c.x == 7);
        CHECK(c.z == 3);
    }

    TEST_CASE("subtraction with int") {
        Pos2D p(10, 7);
        Pos2D r = p - 4;
        CHECK(r.x == 6);
        CHECK(r.z == 3);
    }

    TEST_CASE("division by int") {
        Pos2D p(12, 8);
        Pos2D r = p / 4;
        CHECK(r.x == 3);
        CHECK(r.z == 2);
    }

    TEST_CASE("right shift") {
        Pos2D p(16, 32);
        Pos2D r = p >> 4;
        CHECK(r.x == 1);
        CHECK(r.z == 2);
    }

    TEST_CASE("left shift") {
        Pos2D p(1, 3);
        Pos2D r = p << 2;
        CHECK(r.x == 4);
        CHECK(r.z == 12);
    }

    TEST_CASE("toChunkPos") {
        Pos2D p(32, 48);
        Pos2D cp = p.toChunkPos();
        CHECK(cp.x == 2);
        CHECK(cp.z == 3);
    }

    TEST_CASE("toBlockPos") {
        Pos2D p(2, 3);
        Pos2D bp = p.toBlockPos();
        CHECK(bp.x == 32);
        CHECK(bp.z == 48);
    }

    TEST_CASE("asType<double>") {
        Pos2D p(5, 7);
        auto d = p.asType<double>();
        CHECK(d.x == doctest::Approx(5.0));
        CHECK(d.z == doctest::Approx(7.0));
    }

    TEST_CASE("distanceSq from origin") {
        Pos2D p(3, 4);
        CHECK(p.distanceSq() == doctest::Approx(25.0));
    }

    TEST_CASE("insideBounds true") {
        Pos2D p(5, 5);
        CHECK(p.insideBounds(0, 0, 10, 10));
    }

    TEST_CASE("insideBounds false") {
        Pos2D p(11, 5);
        CHECK_FALSE(p.insideBounds(0, 0, 10, 10));
    }

    TEST_CASE("insideChunk true") {
        Pos2D p(16, 16);   // block (16,16) is in chunk (1,1)
        CHECK(p.insideChunk(1, 1));
    }

    TEST_CASE("insideChunk false") {
        Pos2D p(32, 0);    // block (32,0) is in chunk (2,0)
        CHECK_FALSE(p.insideChunk(1, 0));
    }

    TEST_CASE("asLong round-trips") {
        Pos2D p(7, 13);
        uint64_t val = p.asLong();
        int32_t rx = static_cast<int32_t>(val >> 32);
        int32_t rz = static_cast<int32_t>(val & 0xFFFF'FFFF);
        CHECK(rx == 7);
        CHECK(rz == 13);
    }

    TEST_CASE("comparison operators") {
        Pos2D p(5, 0);
        CHECK_FALSE(p > 4);
        CHECK_FALSE(p > 5);
        CHECK(p < 6);
        CHECK_FALSE(p < 5);
        CHECK_FALSE(p >= 5);
        CHECK(p <= 5);

        Pos2D p1(5, 6);
        CHECK(p1 > 4);
        CHECK_FALSE(p1 > 5);
        CHECK_FALSE(p1 < 6);
        CHECK(p1 < 7);
        CHECK(p1 >= 5);
        CHECK_FALSE(p1 <= 5);
    }

    TEST_CASE("abs helper") {
        Pos2D p(-3, -7);
        Pos2D a = abs(p);
        CHECK(a.x == 3);
        CHECK(a.z == 7);
    }

    TEST_CASE("setPos updates both coordinates") {
        Pos2D p(1, 2);
        p.setPos(9, 11);
        CHECK(p.x == 9);
        CHECK(p.z == 11);
    }

    TEST_CASE("toString is non-empty") {
        Pos2D p(4, 8);
        CHECK_FALSE(p.toString().empty());
    }

    TEST_CASE("hash is consistent") {
        Pos2D p(100, 200);
        CHECK(p.hash() == p.hash());
    }

    TEST_CASE("y coordinate is set") {
        Pos3D p(1, 64, 3);
        CHECK(p.y == 64);
    }

    TEST_CASE("addition 3D + 3D") {
        Pos3D a(1, 2, 3);
        Pos3D b(4, 5, 6);
        Pos3D c = a + b;
        CHECK(c.x == 5);
        CHECK(c.y == 7);
        CHECK(c.z == 9);
    }

    TEST_CASE("addition 3D + scalar") {
        Pos3D a(1, 2, 3);
        Pos3D c = a + 10;
        CHECK(c.x == 11);
        CHECK(c.y == 12);
        CHECK(c.z == 13);
    }

    TEST_CASE("subtraction 3D - 3D") {
        Pos3D a(10, 20, 30);
        Pos3D b(1, 2, 3);
        Pos3D c = a - b;
        CHECK(c.x == 9);
        CHECK(c.y == 18);
        CHECK(c.z == 27);
    }

    TEST_CASE("add() method") {
        Pos3D p(1, 2, 3);
        Pos3D r = p.add(10, 20, 30);
        CHECK(r.x == 11);
        CHECK(r.y == 22);
        CHECK(r.z == 33);
    }

    TEST_CASE("subtract() method") {
        Pos3D p(5, 10, 15);
        Pos3D q(1, 2, 3);
        Pos3D r = p.subtract(q);
        CHECK(r.x == 4);
        CHECK(r.y == 8);
        CHECK(r.z == 12);
    }

    TEST_CASE("up / down") {
        Pos3D p(0, 10, 0);
        CHECK(p.up().y == 11);
        CHECK(p.up(5).y == 15);
        CHECK(p.down().y == 9);
        CHECK(p.down(3).y == 7);
    }

    TEST_CASE("east / west") {
        Pos3D p(0, 0, 0);
        CHECK(p.east().x == 1);
        CHECK(p.east(4).x == 4);
        CHECK(p.west().x == -1);
        CHECK(p.west(2).x == -2);
    }

    TEST_CASE("north / south") {
        Pos3D p(0, 0, 0);
        CHECK(p.north().z == -1);
        CHECK(p.north(3).z == -3);
        CHECK(p.south().z == 1);
        CHECK(p.south(2).z == 2);
    }

    TEST_CASE("isNull") {
        CHECK(Pos3D(0, 0, 0).isNull());
        CHECK_FALSE(Pos3D(0, 1, 0).isNull());
    }

    TEST_CASE("getX / getY / getZ") {
        Pos3D p(3, 7, 11);
        CHECK(p.getX() == 3);
        CHECK(p.getY() == 7);
        CHECK(p.getZ() == 11);
    }

    TEST_CASE("setPos") {
        Pos3D p(0, 0, 0);
        p.setPos(5, 10, 15);
        CHECK(p.x == 5);
        CHECK(p.y == 10);
        CHECK(p.z == 15);
    }

    TEST_CASE("asPos2D drops y") {
        Pos3D p(7, 64, 13);
        Pos2D p2 = p.asPos2D();
        CHECK(p2.x == 7);
        CHECK(p2.z == 13);
    }

    TEST_CASE("asType<double>") {
        Pos3D p(2, 3, 5);
        auto d = p.asType<double>();
        CHECK(d.x == doctest::Approx(2.0));
        CHECK(d.y == doctest::Approx(3.0));
        CHECK(d.z == doctest::Approx(5.0));
    }

    TEST_CASE("right shift") {
        Pos3D p(32, 64, 128);
        Pos3D r = p >> 4;
        CHECK(r.x == 2);
        CHECK(r.y == 4);
        CHECK(r.z == 8);
    }

    TEST_CASE("left shift") {
        Pos3D p(1, 2, 3);
        Pos3D r = p << 3;
        CHECK(r.x == 8);
        CHECK(r.y == 16);
        CHECK(r.z == 24);
    }

    TEST_CASE("distanceSq from origin") {
        Pos3D p(3, 4, 0);
        CHECK(p.distanceSq() == doctest::Approx(25.0));
    }

    TEST_CASE("distanceSqXZ ignores y") {
        Pos3D p(3, 100, 4);
        CHECK(p.distanceSqXZ() == doctest::Approx(25.0));
    }

    TEST_CASE("distanceSq to point") {
        Pos3D p(0, 0, 0);
        CHECK(p.distanceSq(1.0, 0.0, 0.0) == doctest::Approx(1.0));
        CHECK(p.distanceSq(3.0, 4.0, 0.0) == doctest::Approx(25.0));
    }

    TEST_CASE("abs helper") {
        Pos3D p(-1, -2, -3);
        Pos3D a = abs(p);
        CHECK(a.x == 1);
        CHECK(a.y == 2);
        CHECK(a.z == 3);
    }

    TEST_CASE("construct from Pos2D sets y=0") {
        Pos2D p2(5, 7);
        Pos3D p3(p2);
        CHECK(p3.x == 5);
        CHECK(p3.y == 0);
        CHECK(p3.z == 7);
    }

    TEST_CASE("ostream operator is non-empty") {
        Pos3D p(1, 2, 3);
        std::ostringstream oss;
        oss << p;
        CHECK_FALSE(oss.str().empty());
    }
}
