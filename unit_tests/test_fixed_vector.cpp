#include "include/doctest.h"
#include "common/fixedVector.hpp"
#include <string>

TEST_SUITE("FixedVector") {

    TEST_CASE("default constructed is empty") {
        FixedVector<int, 10> v;
        CHECK(v.current_size() == 0);
        CHECK(v.capacity() == 10);
    }

    TEST_CASE("push_back increases size") {
        FixedVector<int, 5> v;
        v.push_back(42);
        CHECK(v.current_size() == 1);
        CHECK(v[0] == 42);
    }

    TEST_CASE("multiple push_backs") {
        FixedVector<int, 10> v;
        for (int i = 0; i < 5; ++i) {
            v.push_back(i * 10);
        }
        CHECK(v.current_size() == 5);
        CHECK(v[0] == 0);
        CHECK(v[2] == 20);
        CHECK(v[4] == 40);
    }

    TEST_CASE("emplace_back works") {
        FixedVector<std::pair<int,int>, 5> v;
        v.emplace_back(1, 2);
        CHECK(v.current_size() == 1);
        CHECK(v[0].first == 1);
        CHECK(v[0].second == 2);
    }

    TEST_CASE("begin/end iteration") {
        FixedVector<int, 4> v;
        v.push_back(10);
        v.push_back(20);
        v.push_back(30);

        int sum = 0;
        for (int val : v) {
            sum += val;
        }
        CHECK(sum == 60);
    }

    TEST_CASE("data pointer") {
        FixedVector<int, 4> v;
        v.push_back(7);
        v.push_back(8);

        const int* p = v.data();
        CHECK(p[0] == 7);
        CHECK(p[1] == 8);
    }

    TEST_CASE("set_size clamps to capacity") {
        FixedVector<int, 3> v;
        v.set_size(5); // exceeds capacity, should not change
        CHECK(v.current_size() == 0);

        v.set_size(2);
        CHECK(v.current_size() == 2);
    }

    TEST_CASE("capacity is constexpr") {
        static_assert(FixedVector<int, 42>::capacity() == 42);
    }

    TEST_CASE("clear resets size to zero") {
        FixedVector<int, 8> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        v.set_size(0);   // there is no clear(); use set_size(0)
        CHECK(v.current_size() == 0u);
    }

    TEST_CASE("fill to capacity") {
        FixedVector<int, 4> v;
        for (int i = 0; i < 4; ++i) v.push_back(i);
        CHECK(v.current_size() == 4u);
        CHECK(v[3] == 3);
    }

    TEST_CASE("const iteration sums values") {
        FixedVector<int, 5> v;
        v.push_back(10);
        v.push_back(20);
        v.push_back(30);

        const auto& cv = v;
        int sum = 0;
        for (const int val : cv) sum += val;
        CHECK(sum == 60);
    }

    TEST_CASE("const data pointer matches values") {
        FixedVector<int, 4> v;
        v.push_back(5);
        v.push_back(6);
        const int* p = v.data();
        CHECK(p[0] == 5);
        CHECK(p[1] == 6);
    }

    TEST_CASE("operator[] write and read-back") {
        FixedVector<int, 4> v;
        v.push_back(0);
        v[0] = 99;
        CHECK(v[0] == 99);
    }

    TEST_CASE("works with non-trivial type (std::string)") {
        FixedVector<std::string, 4> v;
        v.push_back("hello");
        v.emplace_back("world");
        CHECK(v.current_size() == 2u);
        CHECK(v[0] == "hello");
        CHECK(v[1] == "world");
    }

    TEST_CASE("empty FixedVector begin equals end") {
        FixedVector<int, 4> v;
        CHECK(v.begin() == v.end());
    }

    TEST_CASE("after push_back begin != end") {
        FixedVector<int, 4> v;
        v.push_back(1);
        CHECK(v.begin() != v.end());
    }

    TEST_CASE("set_size beyond capacity is rejected") {
        FixedVector<int, 3> v;
        v.set_size(100);      // should be a no-op
        CHECK(v.current_size() == 0u);
    }

    TEST_CASE("set_size reduces size (no truncation of existing elements)") {
        FixedVector<int, 6> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        v.set_size(1);
        CHECK(v.current_size() == 1u);
        CHECK(v[0] == 1);
    }
}

