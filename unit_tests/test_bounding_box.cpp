#include "include/doctest.h"
#include "components/BoundingBox.hpp"
#include <sstream>

TEST_SUITE("BoundingBox") {

    TEST_CASE("6-arg constructor sets all fields") {
        BoundingBox bb(1, 2, 3, 10, 20, 30);
        CHECK(bb.m_minX == 1);
        CHECK(bb.m_minY == 2);
        CHECK(bb.m_minZ == 3);
        CHECK(bb.m_maxX == 10);
        CHECK(bb.m_maxY == 20);
        CHECK(bb.m_maxZ == 30);
    }

    TEST_CASE("4-arg constructor (2D)") {
        BoundingBox bb(5, 10, 15, 20);
        CHECK(bb.m_minX == 5);
        CHECK(bb.m_minZ == 10);
        CHECK(bb.m_maxX == 15);
        CHECK(bb.m_maxZ == 20);
    }

    TEST_CASE("size getters") {
        BoundingBox bb(0, 0, 0, 9, 4, 7);
        CHECK(bb.getXSize() == 10);
        CHECK(bb.getYSize() == 5);
        CHECK(bb.getZSize() == 8);
    }

    TEST_CASE("center coordinates") {
        BoundingBox bb(0, 0, 0, 10, 10, 10);
        CHECK(bb.getCenterX() == 5);
        CHECK(bb.getCenterZ() == 5);
    }

    TEST_CASE("equality") {
        BoundingBox a(1, 2, 3, 4, 5, 6);
        BoundingBox b(1, 2, 3, 4, 5, 6);
        BoundingBox c(0, 2, 3, 4, 5, 6);
        CHECK(a == b);
        CHECK_FALSE(a == c);
    }

    TEST_CASE("intersects") {
        BoundingBox a(0, 0, 0, 10, 10, 10);
        BoundingBox b(5, 5, 5, 15, 15, 15);
        BoundingBox c(20, 20, 20, 30, 30, 30);
        CHECK(a.intersects(b));
        CHECK_FALSE(a.intersects(c));
    }

    TEST_CASE("contains") {
        BoundingBox outer(0, 0, 0, 20, 20, 20);
        BoundingBox inner(5, 5, 5, 10, 10, 10);
        BoundingBox partial(15, 15, 15, 25, 25, 25);
        CHECK(outer.contains(inner));
        CHECK_FALSE(outer.contains(partial));
    }

    TEST_CASE("isVecInside 3D") {
        BoundingBox bb(0, 0, 0, 10, 10, 10);
        CHECK(bb.isVecInside(Pos3D(5, 5, 5)));
        CHECK(bb.isVecInside(Pos3D(0, 0, 0)));
        CHECK(bb.isVecInside(Pos3D(10, 10, 10)));
        CHECK_FALSE(bb.isVecInside(Pos3D(11, 5, 5)));
    }

    TEST_CASE("isVecInside 2D") {
        BoundingBox bb(0, 0, 0, 10, 10, 10);
        CHECK(bb.isVecInside(Pos2D(5, 5)));
        CHECK_FALSE(bb.isVecInside(Pos2D(11, 5)));
    }

    TEST_CASE("offset") {
        BoundingBox bb(0, 0, 0, 10, 10, 10);
        bb.offset(5, 3, 7);
        CHECK(bb.m_minX == 5);
        CHECK(bb.m_minY == 3);
        CHECK(bb.m_minZ == 7);
        CHECK(bb.m_maxX == 15);
        CHECK(bb.m_maxY == 13);
        CHECK(bb.m_maxZ == 17);
    }

    TEST_CASE("getArea") {
        BoundingBox bb(0, 0, 0, 3, 0, 4);
        CHECK(bb.getArea() == 20); // 4 * 5
    }

    TEST_CASE("getLength") {
        BoundingBox bb(0, 0, 0, 10, 0, 5);
        CHECK(bb.getLength() == 10);
    }

    TEST_CASE("makeChunkBox") {
        BoundingBox bb = BoundingBox::makeChunkBox(0, 0);
        CHECK(bb.getXSize() == 16);
        CHECK(bb.getZSize() == 16);
    }

    TEST_CASE("makeChunkBox non-zero chunk") {
        BoundingBox bb = BoundingBox::makeChunkBox(2, -1);
        CHECK(bb.m_minX == 32);
        CHECK(bb.m_maxX == 47);
        CHECK(bb.m_minZ == -16);
        CHECK(bb.m_maxZ == -1);
        CHECK(bb.m_minY == 0);
        CHECK(bb.m_maxY == 255);
    }

    TEST_CASE("encompass expands to include other box") {
        BoundingBox bb(5, 5, 5, 10, 10, 10);
        bb.encompass(BoundingBox(0, 0, 0, 3, 3, 3));
        CHECK(bb.m_minX == 0);
        CHECK(bb.m_minY == 0);
        CHECK(bb.m_minZ == 0);
        CHECK(bb.m_maxX == 10);
        CHECK(bb.m_maxY == 10);
        CHECK(bb.m_maxZ == 10);
    }

    TEST_CASE("encompass with already-contained box is no-op") {
        BoundingBox outer(0, 0, 0, 20, 20, 20);
        BoundingBox original = outer;
        outer.encompass(BoundingBox(5, 5, 5, 10, 10, 10));
        CHECK(outer == original);
    }

    TEST_CASE("encompassY only changes Y bounds") {
        BoundingBox bb(0, 10, 0, 10, 20, 10);
        bb.encompassY(BoundingBox(0, 5, 0, 10, 30, 10));
        CHECK(bb.m_minY == 5);
        CHECK(bb.m_maxY == 30);
        CHECK(bb.m_minX == 0);
        CHECK(bb.m_maxX == 10);
    }

    TEST_CASE("shrinkToFit clips to outer bounds") {
        BoundingBox bb(-5, -5, -5, 25, 25, 25);
        bb.shrinkToFit(BoundingBox(0, 0, 0, 20, 20, 20));
        CHECK(bb.m_minX == 0);
        CHECK(bb.m_minY == 0);
        CHECK(bb.m_minZ == 0);
        CHECK(bb.m_maxX == 20);
        CHECK(bb.m_maxY == 20);
        CHECK(bb.m_maxZ == 20);
    }

    TEST_CASE("shrinkToFit with already-contained box is no-op") {
        BoundingBox inner(2, 2, 2, 8, 8, 8);
        BoundingBox original = inner;
        inner.shrinkToFit(BoundingBox(0, 0, 0, 10, 10, 10));
        CHECK(inner == original);
    }

    TEST_CASE("offsetY only shifts Y") {
        BoundingBox bb(1, 10, 1, 5, 20, 5);
        bb.offsetY(5);
        CHECK(bb.m_minY == 15);
        CHECK(bb.m_maxY == 25);
        CHECK(bb.m_minX == 1);
        CHECK(bb.m_maxX == 5);
    }

    TEST_CASE("createProper handles swapped coordinates") {
        BoundingBox bb = BoundingBox::createProper(10, 20, 30, 5, 15, 25);
        CHECK(bb.m_minX == 5);
        CHECK(bb.m_minY == 15);
        CHECK(bb.m_minZ == 25);
        CHECK(bb.m_maxX == 10);
        CHECK(bb.m_maxY == 20);
        CHECK(bb.m_maxZ == 30);
    }

    TEST_CASE("createProper with already-ordered coordinates") {
        BoundingBox bb = BoundingBox::createProper(1, 2, 3, 7, 8, 9);
        CHECK(bb.m_minX == 1);
        CHECK(bb.m_minY == 2);
        CHECK(bb.m_minZ == 3);
        CHECK(bb.m_maxX == 7);
        CHECK(bb.m_maxY == 8);
        CHECK(bb.m_maxZ == 9);
    }

    TEST_CASE("toString is non-empty") {
        BoundingBox bb(1, 2, 3, 4, 5, 6);
        CHECK_FALSE(bb.toString().empty());
    }

    TEST_CASE("ostream operator produces output") {
        BoundingBox bb(0, 0, 0, 1, 1, 1);
        std::ostringstream oss;
        oss << bb;
        CHECK_FALSE(oss.str().empty());
    }

    TEST_CASE("setBoundingBox updates fields") {
        BoundingBox bb(0, 0, 0, 0, 0, 0);
        bb.setBoundingBox(BoundingBox(3, 4, 5, 6, 7, 8));
        CHECK(bb.m_minX == 3);
        CHECK(bb.m_minY == 4);
        CHECK(bb.m_minZ == 5);
        CHECK(bb.m_maxX == 6);
        CHECK(bb.m_maxY == 7);
        CHECK(bb.m_maxZ == 8);
    }

    TEST_CASE("left shift doubles coordinates") {
        BoundingBox bb(1, 2, 3, 4, 5, 6);
        BoundingBox shifted = bb << 1;
        CHECK(shifted.m_minX == 2);
        CHECK(shifted.m_minY == 4);
        CHECK(shifted.m_minZ == 6);
        CHECK(shifted.m_maxX == 8);
        CHECK(shifted.m_maxY == 10);
        CHECK(shifted.m_maxZ == 12);
    }

    TEST_CASE("right shift halves coordinates") {
        BoundingBox bb(4, 8, 16, 20, 32, 48);
        BoundingBox shifted = bb >> 2;
        CHECK(shifted.m_minX == 1);
        CHECK(shifted.m_minY == 2);
        CHECK(shifted.m_minZ == 4);
        CHECK(shifted.m_maxX == 5);
        CHECK(shifted.m_maxY == 8);
        CHECK(shifted.m_maxZ == 12);
    }

    TEST_CASE("intersects at single-point boundary") {
        BoundingBox a(0, 0, 0, 5, 5, 5);
        BoundingBox b(5, 5, 5, 10, 10, 10);
        CHECK(a.intersects(b)); // touching at corner counts as intersecting
    }

    TEST_CASE("4-arg constructor y range is 0..255") {
        BoundingBox bb(1, 2, 8, 9);
        CHECK(bb.m_minY == 0);
        CHECK(bb.m_maxY == 255);
    }

    TEST_CASE("orientBox SOUTH") {
        BoundingBox bb = BoundingBox::orientBox(0, 0, 0, 3, 2, 4, EnumFacing::SOUTH);
        CHECK(bb.m_minX == 0);
        CHECK(bb.m_minY == 0);
        CHECK(bb.m_minZ == 0);
        CHECK(bb.m_maxX == 2);  // width-1
        CHECK(bb.m_maxY == 1);  // height-1
        CHECK(bb.m_maxZ == 3);  // depth-1
    }

    TEST_CASE("orientBox NORTH") {
        BoundingBox bb = BoundingBox::orientBox(0, 0, 5, 3, 2, 4, EnumFacing::NORTH);
        CHECK(bb.m_minZ == 5 - 4 + 1); // z - depth + 1
        CHECK(bb.m_maxZ == 5);
    }

    TEST_CASE("right shift operator") {
        BoundingBox bb(16, 32, 64, 128, 256, 512);
        BoundingBox shifted = bb >> 4;
        CHECK(shifted.m_minX == 1);
        CHECK(shifted.m_minY == 2);
        CHECK(shifted.m_minZ == 4);
        CHECK(shifted.m_maxX == 8);
        CHECK(shifted.m_maxY == 16);
        CHECK(shifted.m_maxZ == 32);
    }

    TEST_CASE("toString produces non-empty string") {
        BoundingBox bb(1, 2, 3, 4, 5, 6);
        std::string s = bb.toString();
        CHECK_FALSE(s.empty());
    }
}

