#include "include/doctest.h"
#include "components/PieceWeight.hpp"
#include "components/GenerationStep.hpp"

// ============================================================
//  PieceWeight
// ============================================================
TEST_SUITE("PieceWeight") {

    TEST_CASE("fields are stored correctly") {
        PieceWeight pw{10, 5, 2};
        CHECK(pw.weight == 10);
        CHECK(pw.maxPlaceCount == 5);
        CHECK(pw.minDepth == 2);
    }

    TEST_CASE("getWeight returns weight") {
        PieceWeight pw{7, 3, 1};
        CHECK(pw.getWeight() == 7);
    }

    TEST_CASE("getMaxCount returns maxPlaceCount") {
        PieceWeight pw{1, 99, 0};
        CHECK(pw.getMaxCount() == 99);
    }

    TEST_CASE("getMinDepth returns minDepth") {
        PieceWeight pw{1, 1, 42};
        CHECK(pw.getMinDepth() == 42);
    }

    TEST_CASE("zero weight is valid") {
        PieceWeight pw{0, 0, 0};
        CHECK(pw.getWeight() == 0);
        CHECK(pw.getMaxCount() == 0);
        CHECK(pw.getMinDepth() == 0);
    }

    TEST_CASE("large values are stored correctly") {
        PieceWeight pw{1000000, 999, 255};
        CHECK(pw.getWeight() == 1000000);
        CHECK(pw.getMaxCount() == 999);
        CHECK(pw.getMinDepth() == 255);
    }

    TEST_CASE("aggregate initialization works") {
        PieceWeight pw = {3, 2, 1};
        CHECK(pw.weight == 3);
        CHECK(pw.maxPlaceCount == 2);
        CHECK(pw.minDepth == 1);
    }
}

// ============================================================
//  GenerationStep
// ============================================================
TEST_SUITE("GenerationStep") {

    TEST_CASE("GS_NONE is 0") {
        CHECK(GS_NONE == 0);
    }

    TEST_CASE("Village steps are ordered") {
        CHECK(GS_Village_NONE < GS_Village_Valid);
        CHECK(GS_Village_Valid < GS_Village_Blacksmith);
        CHECK(GS_Village_Blacksmith < GS_Village_Layout);
        CHECK(GS_Village_Layout < GS_Village_Full);
    }

    TEST_CASE("Stronghold steps are ordered") {
        CHECK(GS_Stronghold_NONE < GS_Stronghold_Portal);
        CHECK(GS_Stronghold_Portal < GS_Stronghold_Layout);
        CHECK(GS_Stronghold_Layout < GS_Stronghold_Full);
    }

    TEST_CASE("NONE aliases are zero") {
        CHECK(static_cast<int>(GS_Village_NONE) == 0);
        CHECK(static_cast<int>(GS_Stronghold_NONE) == 0);
    }

    TEST_CASE("Village_Full value is 4") {
        CHECK(GS_Village_Full == 4);
    }

    TEST_CASE("Stronghold_Full value is 3") {
        CHECK(GS_Stronghold_Full == 3);
    }

    TEST_CASE("can be used in comparison") {
        GenerationStep step = GS_Village_Blacksmith;
        CHECK(step > GS_Village_Valid);
        CHECK(step < GS_Village_Layout);
    }
}

