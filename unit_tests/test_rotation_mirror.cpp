#include "include/doctest.h"
#include "common/RotationAndMirror.hpp"

// ============================================================
//  Rotation
// ============================================================
TEST_SUITE("Rotation") {

    TEST_CASE("equality") {
        CHECK(Rotation::NONE == Rotation::NONE);
        CHECK(Rotation::CLOCKWISE_90 == Rotation::CLOCKWISE_90);
        CHECK_FALSE(Rotation::NONE == Rotation::CLOCKWISE_90);
        CHECK_FALSE(Rotation::CLOCKWISE_90 == Rotation::CLOCKWISE_180);
    }

    TEST_CASE("add NONE is identity") {
        CHECK(Rotation::NONE.add(Rotation::NONE) == Rotation::NONE);
        CHECK(Rotation::CLOCKWISE_90.add(Rotation::NONE) == Rotation::CLOCKWISE_90);
        CHECK(Rotation::CLOCKWISE_180.add(Rotation::NONE) == Rotation::CLOCKWISE_180);
        CHECK(Rotation::COUNTERCLOCKWISE_90.add(Rotation::NONE) == Rotation::COUNTERCLOCKWISE_90);
    }

    TEST_CASE("add CLOCKWISE_90 advances by 90 degrees") {
        CHECK(Rotation::NONE.add(Rotation::CLOCKWISE_90) == Rotation::CLOCKWISE_90);
        CHECK(Rotation::CLOCKWISE_90.add(Rotation::CLOCKWISE_90) == Rotation::CLOCKWISE_180);
        CHECK(Rotation::CLOCKWISE_180.add(Rotation::CLOCKWISE_90) == Rotation::COUNTERCLOCKWISE_90);
        CHECK(Rotation::COUNTERCLOCKWISE_90.add(Rotation::CLOCKWISE_90) == Rotation::NONE);
    }

    TEST_CASE("add CLOCKWISE_180") {
        CHECK(Rotation::NONE.add(Rotation::CLOCKWISE_180) == Rotation::CLOCKWISE_180);
        CHECK(Rotation::CLOCKWISE_90.add(Rotation::CLOCKWISE_180) == Rotation::COUNTERCLOCKWISE_90);
        CHECK(Rotation::CLOCKWISE_180.add(Rotation::CLOCKWISE_180) == Rotation::NONE);
        CHECK(Rotation::COUNTERCLOCKWISE_90.add(Rotation::CLOCKWISE_180) == Rotation::CLOCKWISE_90);
    }

    TEST_CASE("add COUNTERCLOCKWISE_90") {
        CHECK(Rotation::NONE.add(Rotation::COUNTERCLOCKWISE_90) == Rotation::COUNTERCLOCKWISE_90);
        CHECK(Rotation::CLOCKWISE_90.add(Rotation::COUNTERCLOCKWISE_90) == Rotation::NONE);
        CHECK(Rotation::CLOCKWISE_180.add(Rotation::COUNTERCLOCKWISE_90) == Rotation::CLOCKWISE_90);
        CHECK(Rotation::COUNTERCLOCKWISE_90.add(Rotation::COUNTERCLOCKWISE_90) == Rotation::CLOCKWISE_180);
    }

    TEST_CASE("rotateFacing CW90: NORTH -> EAST -> SOUTH -> WEST -> NORTH") {
        CHECK(Rotation::CLOCKWISE_90.rotateFacing(EnumFacing::NORTH) == EnumFacing::EAST);
        CHECK(Rotation::CLOCKWISE_90.rotateFacing(EnumFacing::EAST)  == EnumFacing::SOUTH);
        CHECK(Rotation::CLOCKWISE_90.rotateFacing(EnumFacing::SOUTH) == EnumFacing::WEST);
        CHECK(Rotation::CLOCKWISE_90.rotateFacing(EnumFacing::WEST)  == EnumFacing::NORTH);
    }

    TEST_CASE("rotateFacing CCW90: NORTH -> WEST -> SOUTH -> EAST -> NORTH") {
        CHECK(Rotation::COUNTERCLOCKWISE_90.rotateFacing(EnumFacing::NORTH) == EnumFacing::WEST);
        CHECK(Rotation::COUNTERCLOCKWISE_90.rotateFacing(EnumFacing::WEST)  == EnumFacing::SOUTH);
        CHECK(Rotation::COUNTERCLOCKWISE_90.rotateFacing(EnumFacing::SOUTH) == EnumFacing::EAST);
        CHECK(Rotation::COUNTERCLOCKWISE_90.rotateFacing(EnumFacing::EAST)  == EnumFacing::NORTH);
    }

    TEST_CASE("rotateFacing CW180 is opposite") {
        CHECK(Rotation::CLOCKWISE_180.rotateFacing(EnumFacing::NORTH) == EnumFacing::SOUTH);
        CHECK(Rotation::CLOCKWISE_180.rotateFacing(EnumFacing::SOUTH) == EnumFacing::NORTH);
        CHECK(Rotation::CLOCKWISE_180.rotateFacing(EnumFacing::EAST)  == EnumFacing::WEST);
        CHECK(Rotation::CLOCKWISE_180.rotateFacing(EnumFacing::WEST)  == EnumFacing::EAST);
    }

    TEST_CASE("rotateFacing NONE is identity") {
        CHECK(Rotation::NONE.rotateFacing(EnumFacing::NORTH) == EnumFacing::NORTH);
        CHECK(Rotation::NONE.rotateFacing(EnumFacing::EAST)  == EnumFacing::EAST);
        CHECK(Rotation::NONE.rotateFacing(EnumFacing::SOUTH) == EnumFacing::SOUTH);
        CHECK(Rotation::NONE.rotateFacing(EnumFacing::WEST)  == EnumFacing::WEST);
    }

    TEST_CASE("rotateFacing UP/DOWN are preserved by all rotations") {
        for (const Rotation* r : Rotation::values()) {
            CHECK(r->rotateFacing(EnumFacing::UP)   == EnumFacing::UP);
            CHECK(r->rotateFacing(EnumFacing::DOWN) == EnumFacing::DOWN);
        }
    }

    TEST_CASE("rotate index with count 4") {
        // NONE: no change
        CHECK(Rotation::NONE.rotate(0, 4) == 0);
        CHECK(Rotation::NONE.rotate(3, 4) == 3);
        // CW90: +1 step
        CHECK(Rotation::CLOCKWISE_90.rotate(0, 4) == 1);
        CHECK(Rotation::CLOCKWISE_90.rotate(3, 4) == 0); // wraps
        // CW180: +2 steps
        CHECK(Rotation::CLOCKWISE_180.rotate(0, 4) == 2);
        CHECK(Rotation::CLOCKWISE_180.rotate(2, 4) == 0);
        // CCW90: +3 steps
        CHECK(Rotation::COUNTERCLOCKWISE_90.rotate(0, 4) == 3);
        CHECK(Rotation::COUNTERCLOCKWISE_90.rotate(1, 4) == 0);
    }

    TEST_CASE("values() returns 4 distinct rotations") {
        auto vals = Rotation::values();
        CHECK(vals.size() == 4);
        CHECK(*vals[0] == Rotation::NONE);
        CHECK(*vals[1] == Rotation::CLOCKWISE_90);
        CHECK(*vals[2] == Rotation::CLOCKWISE_180);
        CHECK(*vals[3] == Rotation::COUNTERCLOCKWISE_90);
    }

    TEST_CASE("apply with NONE mirror is same as rotateFacing") {
        CHECK(Rotation::CLOCKWISE_90.apply(Mirror::NONE, EnumFacing::NORTH) ==
              Rotation::CLOCKWISE_90.rotateFacing(EnumFacing::NORTH));
        CHECK(Rotation::CLOCKWISE_180.apply(Mirror::NONE, EnumFacing::SOUTH) ==
              Rotation::CLOCKWISE_180.rotateFacing(EnumFacing::SOUTH));
    }

    TEST_CASE("apply NONE rotation with FRONT_BACK mirror: swaps EAST/WEST") {
        CHECK(Rotation::NONE.apply(Mirror::FRONT_BACK, EnumFacing::EAST)  == EnumFacing::WEST);
        CHECK(Rotation::NONE.apply(Mirror::FRONT_BACK, EnumFacing::WEST)  == EnumFacing::EAST);
        CHECK(Rotation::NONE.apply(Mirror::FRONT_BACK, EnumFacing::NORTH) == EnumFacing::NORTH);
        CHECK(Rotation::NONE.apply(Mirror::FRONT_BACK, EnumFacing::SOUTH) == EnumFacing::SOUTH);
    }

    TEST_CASE("apply NONE rotation with LEFT_RIGHT mirror: swaps NORTH/SOUTH") {
        CHECK(Rotation::NONE.apply(Mirror::LEFT_RIGHT, EnumFacing::NORTH) == EnumFacing::SOUTH);
        CHECK(Rotation::NONE.apply(Mirror::LEFT_RIGHT, EnumFacing::SOUTH) == EnumFacing::NORTH);
        CHECK(Rotation::NONE.apply(Mirror::LEFT_RIGHT, EnumFacing::EAST)  == EnumFacing::EAST);
        CHECK(Rotation::NONE.apply(Mirror::LEFT_RIGHT, EnumFacing::WEST)  == EnumFacing::WEST);
    }
}

// ============================================================
//  Mirror
// ============================================================
TEST_SUITE("Mirror") {

    TEST_CASE("equality") {
        CHECK(Mirror::NONE == Mirror::NONE);
        CHECK(Mirror::LEFT_RIGHT == Mirror::LEFT_RIGHT);
        CHECK(Mirror::FRONT_BACK == Mirror::FRONT_BACK);
        CHECK_FALSE(Mirror::NONE == Mirror::LEFT_RIGHT);
        CHECK_FALSE(Mirror::LEFT_RIGHT == Mirror::FRONT_BACK);
    }

    TEST_CASE("mirrorFACING NONE is identity") {
        CHECK(Mirror::NONE.mirrorFACING(EnumFacing::NORTH) == EnumFacing::NORTH);
        CHECK(Mirror::NONE.mirrorFACING(EnumFacing::SOUTH) == EnumFacing::SOUTH);
        CHECK(Mirror::NONE.mirrorFACING(EnumFacing::EAST)  == EnumFacing::EAST);
        CHECK(Mirror::NONE.mirrorFACING(EnumFacing::WEST)  == EnumFacing::WEST);
    }

    TEST_CASE("mirrorFACING FRONT_BACK swaps EAST and WEST") {
        CHECK(Mirror::FRONT_BACK.mirrorFACING(EnumFacing::EAST)  == EnumFacing::WEST);
        CHECK(Mirror::FRONT_BACK.mirrorFACING(EnumFacing::WEST)  == EnumFacing::EAST);
        CHECK(Mirror::FRONT_BACK.mirrorFACING(EnumFacing::NORTH) == EnumFacing::NORTH);
        CHECK(Mirror::FRONT_BACK.mirrorFACING(EnumFacing::SOUTH) == EnumFacing::SOUTH);
    }

    TEST_CASE("mirrorFACING LEFT_RIGHT swaps NORTH and SOUTH") {
        CHECK(Mirror::LEFT_RIGHT.mirrorFACING(EnumFacing::NORTH) == EnumFacing::SOUTH);
        CHECK(Mirror::LEFT_RIGHT.mirrorFACING(EnumFacing::SOUTH) == EnumFacing::NORTH);
        CHECK(Mirror::LEFT_RIGHT.mirrorFACING(EnumFacing::EAST)  == EnumFacing::EAST);
        CHECK(Mirror::LEFT_RIGHT.mirrorFACING(EnumFacing::WEST)  == EnumFacing::WEST);
    }

    TEST_CASE("mirrorFACING is an involution (applying twice gives identity)") {
        for (const EnumFacing f : {EnumFacing::NORTH, EnumFacing::SOUTH, EnumFacing::EAST, EnumFacing::WEST}) {
            CHECK(Mirror::FRONT_BACK.mirrorFACING(Mirror::FRONT_BACK.mirrorFACING(f)) == f);
            CHECK(Mirror::LEFT_RIGHT.mirrorFACING(Mirror::LEFT_RIGHT.mirrorFACING(f)) == f);
        }
    }

    TEST_CASE("toRotation returns NONE for axes that don't trigger 180") {
        // FRONT_BACK on X axis (EAST/WEST) -> CLOCKWISE_180
        CHECK(Mirror::FRONT_BACK.toRotation(EnumFacing::EAST) == Rotation::CLOCKWISE_180);
        CHECK(Mirror::FRONT_BACK.toRotation(EnumFacing::WEST) == Rotation::CLOCKWISE_180);
        // FRONT_BACK on Z axis -> NONE
        CHECK(Mirror::FRONT_BACK.toRotation(EnumFacing::NORTH) == Rotation::NONE);
        CHECK(Mirror::FRONT_BACK.toRotation(EnumFacing::SOUTH) == Rotation::NONE);
    }

    TEST_CASE("toRotation LEFT_RIGHT on Z axis returns CLOCKWISE_180") {
        CHECK(Mirror::LEFT_RIGHT.toRotation(EnumFacing::NORTH) == Rotation::CLOCKWISE_180);
        CHECK(Mirror::LEFT_RIGHT.toRotation(EnumFacing::SOUTH) == Rotation::CLOCKWISE_180);
        // LEFT_RIGHT on X axis -> NONE
        CHECK(Mirror::LEFT_RIGHT.toRotation(EnumFacing::EAST) == Rotation::NONE);
        CHECK(Mirror::LEFT_RIGHT.toRotation(EnumFacing::WEST) == Rotation::NONE);
    }

    TEST_CASE("NONE mirror toRotation always returns NONE") {
        CHECK(Mirror::NONE.toRotation(EnumFacing::NORTH) == Rotation::NONE);
        CHECK(Mirror::NONE.toRotation(EnumFacing::EAST)  == Rotation::NONE);
        CHECK(Mirror::NONE.toRotation(EnumFacing::SOUTH) == Rotation::NONE);
        CHECK(Mirror::NONE.toRotation(EnumFacing::WEST)  == Rotation::NONE);
    }

    TEST_CASE("mirrorRotation NONE is identity") {
        CHECK(Mirror::NONE.mirrorRotation(0, 4) == 0);
        CHECK(Mirror::NONE.mirrorRotation(2, 4) == 2);
        CHECK(Mirror::NONE.mirrorRotation(3, 4) == 3);
    }

    TEST_CASE("mirrorRotation FRONT_BACK: 0 maps to 0, 1 maps to 3") {
        CHECK(Mirror::FRONT_BACK.mirrorRotation(0, 4) == 0);
        CHECK(Mirror::FRONT_BACK.mirrorRotation(1, 4) == 3);
        CHECK(Mirror::FRONT_BACK.mirrorRotation(2, 4) == 2);
        CHECK(Mirror::FRONT_BACK.mirrorRotation(3, 4) == 1);
    }

    TEST_CASE("values() returns 3 mirrors") {
        auto vals = Mirror::values();
        CHECK(vals.size() == 3);
        CHECK(vals[0] == Mirror::NONE);
        CHECK(vals[1] == Mirror::LEFT_RIGHT);
        CHECK(vals[2] == Mirror::FRONT_BACK);
    }
}

