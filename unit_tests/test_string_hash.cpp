#include "include/doctest.h"
#include "common/StringHash.hpp"

TEST_SUITE("StringHash") {

    TEST_CASE("empty string hashes to 0") {
        CHECK(StringHash::hash("") == 0);
    }

    TEST_CASE("known Java hashCode values") {
        // Java: "Hello".hashCode() == 69609650
        CHECK(StringHash::hash("Hello") == 69609650);
        // Java: "a".hashCode() == 97
        CHECK(StringHash::hash("a") == 97);
        // Java: "ab".hashCode() == 3105
        CHECK(StringHash::hash("ab") == 3105);
    }

    TEST_CASE("deterministic") {
        int32_t h1 = StringHash::hash("test_string");
        int32_t h2 = StringHash::hash("test_string");
        CHECK(h1 == h2);
    }

    TEST_CASE("different strings produce different hashes") {
        CHECK(StringHash::hash("foo") != StringHash::hash("bar"));
    }

    TEST_CASE("case sensitive: uppercase vs lowercase differ") {
        CHECK(StringHash::hash("hello") != StringHash::hash("Hello"));
        CHECK(StringHash::hash("HELLO") != StringHash::hash("hello"));
    }

    TEST_CASE("single digit characters") {
        // Java: '0' == 48, '9' == 57
        CHECK(StringHash::hash("0") == 48);
        CHECK(StringHash::hash("9") == 57);
    }

    TEST_CASE("space character") {
        // Java: ' '.hashCode in a string => 32
        CHECK(StringHash::hash(" ") == 32);
    }

    TEST_CASE("longer known string matches Java hashCode") {
        // Java: "Minecraft".hashCode() == -1595926131
        CHECK(StringHash::hash("Minecraft") == -1595926131);
    }

    TEST_CASE("single character hash equals ASCII value") {
        CHECK(StringHash::hash("A") == 65);
        CHECK(StringHash::hash("Z") == 90);
        CHECK(StringHash::hash("z") == 122);
    }

    TEST_CASE("reversal of a two-char string differs") {
        // "ab" vs "ba": 3105 vs 3073
        CHECK(StringHash::hash("ab") != StringHash::hash("ba"));
    }

    TEST_CASE("two-char formula verification") {
        // hash("ab") = 31*97 + 98 = 3107 - 2 = 3105
        int32_t expected = 31 * static_cast<int32_t>('a') + static_cast<int32_t>('b');
        CHECK(StringHash::hash("ab") == expected);
    }
}

