#include "include/doctest.h"
#include "common/MemPool.hpp"

// ============================================================
//  MemPool<int>
// ============================================================
TEST_SUITE("MemPool") {

    TEST_CASE("acquireFromPool on empty pool returns nullptr") {
        MemPool<int> pool(sizeof(int));
        CHECK(pool.acquireFromPool() == nullptr);
    }

    TEST_CASE("release then acquire round-trips") {
        MemPool<int> pool(sizeof(int));
        int* p = new int(42);
        pool.releaseToPool(p);

        int* got = pool.acquireFromPool();
        REQUIRE(got != nullptr);
        CHECK(*got == 42);
        delete got;
    }

    TEST_CASE("pool size tracks bytes") {
        MemPool<int> pool(sizeof(int));
        CHECK(pool.getPoolSizeBytes() == 0u);

        int* p = new int(1);
        pool.releaseToPool(p);
        CHECK(pool.getPoolSizeBytes() == sizeof(int));

        int* got = pool.acquireFromPool();
        CHECK(pool.getPoolSizeBytes() == 0u);
        delete got;
    }

    TEST_CASE("clearPool empties the pool") {
        MemPool<int> pool(sizeof(int));
        pool.releaseToPool(new int(10));
        pool.releaseToPool(new int(20));
        CHECK(pool.getPoolSizeBytes() == 2 * sizeof(int));

        pool.clearPool();
        CHECK(pool.getPoolSizeBytes() == 0u);
        CHECK(pool.acquireFromPool() == nullptr);
    }

    TEST_CASE("setPoolLimitBytes evicts excess items") {
        MemPool<int> pool(sizeof(int));
        pool.releaseToPool(new int(1));
        pool.releaseToPool(new int(2));
        pool.releaseToPool(new int(3));

        // Shrink limit below current usage; excess items are deleted
        pool.setPoolLimitBytes(sizeof(int));
        CHECK(pool.getPoolSizeBytes() <= sizeof(int));
    }

    TEST_CASE("releaseToPool nullptr is a no-op") {
        MemPool<int> pool(sizeof(int));
        pool.releaseToPool(nullptr); // Must not crash
        CHECK(pool.getPoolSizeBytes() == 0u);
    }

    TEST_CASE("getPoolLimitBytes reflects setPoolLimitBytes") {
        MemPool<int> pool(sizeof(int));
        pool.setPoolLimitBytes(1024u);
        CHECK(pool.getPoolLimitBytes() == 1024u);
    }

    TEST_CASE("over-capacity release deletes the object") {
        // Set limit to 0 so nothing can be pooled
        MemPool<int> pool(sizeof(int));
        pool.setPoolLimitBytes(0u);
        // This must not crash and must not store the item
        pool.releaseToPool(new int(99));
        CHECK(pool.getPoolSizeBytes() == 0u);
        CHECK(pool.acquireFromPool() == nullptr);
    }

    TEST_CASE("LIFO order: last released is first acquired") {
        MemPool<int> pool(sizeof(int));
        int* a = new int(111);
        int* b = new int(222);
        pool.releaseToPool(a);
        pool.releaseToPool(b);

        int* first = pool.acquireFromPool();
        REQUIRE(first != nullptr);
        CHECK(*first == 222); // b was added last
        delete first;

        int* second = pool.acquireFromPool();
        REQUIRE(second != nullptr);
        CHECK(*second == 111);
        delete second;
    }
}

