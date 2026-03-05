#include "include/doctest.h"

// ── loot primitives ────────────────────────────────────────────────────────
#include "loot/classes/Roll.hpp"
#include "loot/classes/LootItem.hpp"
#include "loot/classes/LootTable.hpp"
#include "loot/classes/Container.hpp"
#include "loot/classes/Buffer.hpp"
#include "loot/classes/LootGenMode.hpp"

// ── full loot tables that contain no enchant functions ─────────────────────
#include "loot/tables/spawn_bonus_chest.hpp"   // no enchant funcs
#include "loot/tables/buried_treasure.hpp"     // no enchant funcs

// ── items for asserting item IDs ───────────────────────────────────────────
#include "lce/items/itemsInit.hpp"

#include <sstream>

using namespace loot;
using namespace lce::items;

// ============================================================
//  Roll<Min, Max>
// ============================================================
TEST_SUITE("Roll") {

    TEST_CASE("getMin and getMax return template parameters") {
        using R = Roll<2, 8>;
        CHECK(R::getMin() == 2);
        CHECK(R::getMax() == 8);
    }

    TEST_CASE("Roll(1,1) min == max == 1") {
        using R = Roll<1, 1>;
        CHECK(R::getMin() == 1);
        CHECK(R::getMax() == 1);
    }

    TEST_CASE("rollValue stays within [Min, Max]") {
        RNG rng(42ULL);
        for (int i = 0; i < 10000; ++i) {
            const u8 v = Roll<3, 7>::rollValue<false>(rng);
            CHECK(v >= 3);
            CHECK(v <= 7);
        }
    }

    TEST_CASE("rollValue(1,1) always returns 1") {
        RNG rng(0ULL);
        for (int i = 0; i < 100; ++i) {
            CHECK(Roll<1, 1>::rollValue<false>(rng) == 1);
        }
    }

    TEST_CASE("ostream produces non-empty output") {
        Roll<2, 5> r;
        std::ostringstream oss;
        oss << r;
        CHECK_FALSE(oss.str().empty());
    }

    TEST_CASE("ostream output contains min and max values") {
        Roll<3, 9> r;
        std::ostringstream oss;
        oss << r;
        const std::string s = oss.str();
        CHECK(s.find('3') != std::string::npos);
        CHECK(s.find('9') != std::string::npos);
    }
}

// ============================================================
//  LootItem static properties
// ============================================================
TEST_SUITE("LootItem") {

    TEST_CASE("k_Weight reflects template parameter") {
        using Item_t = LootItem<25, 1, 4, IRON_INGOT.getState()>;
        CHECK(Item_t::k_Weight == 25);
    }

    TEST_CASE("k_MinRoll / k_MaxRoll reflect template parameters") {
        using Item_t = LootItem<10, 2, 6, GOLD_INGOT.getState()>;
        CHECK(Item_t::k_MinRoll == 2);
        CHECK(Item_t::k_MaxRoll == 6);
    }

    TEST_CASE("isOnlyOneItem true when MinRoll == MaxRoll == 1") {
        using Item_t = LootItem<5, 1, 1, SADDLE.getState()>;
        CHECK(Item_t::isOnlyOneItem());
    }

    TEST_CASE("isOnlyOneItem false when roll range > 1") {
        using Item_t = LootItem<5, 1, 4, BONE.getState()>;
        CHECK_FALSE(Item_t::isOnlyOneItem());
    }

    TEST_CASE("isAlwaysNItems true when MinRoll == MaxRoll") {
        using Item_t = LootItem<10, 3, 3, COAL.getState()>;
        CHECK(Item_t::isAlwaysNItems());
    }

    TEST_CASE("isAlwaysNItems false when range differs") {
        using Item_t = LootItem<10, 1, 4, COAL.getState()>;
        CHECK_FALSE(Item_t::isAlwaysNItems());
    }

    TEST_CASE("weight() equals k_Weight") {
        using Item_t = LootItem<42, 1, 1, WHEAT.getState()>;
        CHECK(Item_t::weight() == 42u);
    }

    TEST_CASE("generateItemState item count within [Min, Max]") {
        using Item_t = LootItem<10, 2, 8, GUNPOWDER.getState()>;
        constexpr Item_t item{};
        Container<27> container;
        RNG rng(99ULL);
        for (int i = 0; i < 500; ++i) {
            lce::ItemState s = item.generateItemState<27, false>(container, rng);
            CHECK(s.getCount() >= 2);
            CHECK(s.getCount() <= 8);
        }
    }

    TEST_CASE("generateItemState preserves item ID") {
        using Item_t = LootItem<10, 1, 1, DIAMOND.getState()>;
        constexpr Item_t item{};
        Container<27> container;
        RNG rng(7ULL);
        lce::ItemState s = item.generateItemState<27, false>(container, rng);
        CHECK(s.getID() == DIAMOND.getID());
    }
}

// ============================================================
//  TableWrapper / LootTable
// ============================================================
TEST_SUITE("LootTable") {

    // A simple one-item table: Roll(1,1), one item with weight 10.
    using SingleTable = TableWrapper<1, 1, LootItem<10, 1, 1, BREAD.getState()>>;

    TEST_CASE("single-item table: k_Size == 1") {
        CHECK(SingleTable::table_t::k_Size == 1);
    }

    TEST_CASE("single-item table: hasOneItem()") {
        CHECK(SingleTable::value.hasOneItem());
    }

    TEST_CASE("single-item table: totalWeight() == item weight") {
        CHECK(SingleTable::value.totalWeight() == 10u);
    }

    TEST_CASE("single-item table: maxItemCount() == MaxRoll") {
        CHECK(SingleTable::value.maxItemCount() == 1);
    }

    TEST_CASE("single-item table: k_TableType is RETURNS_1") {
        CHECK(SingleTable::table_t::k_TableType == LootTableType::RETURNS_1);
    }

    TEST_CASE("single-item table: rollLootItem always returns BREAD") {
        Container<27> container;
        RNG rng(1ULL);
        for (int i = 0; i < 100; ++i) {
            lce::ItemState s = SingleTable::value.rollLootItem<27, false>(container, rng);
            CHECK(s.getID() == BREAD.getID());
        }
    }

    // A multi-item table: Roll(1,3), two items.
    using MultiTable = TableWrapper<1, 3,
        LootItem<20, 1, 4, IRON_INGOT.getState()>,
        LootItem<10, 1, 2, GOLD_INGOT.getState()>
    >;

    TEST_CASE("multi-item table: k_Size == 2") {
        CHECK(MultiTable::table_t::k_Size == 2);
    }

    TEST_CASE("multi-item table: totalWeight() == sum of weights") {
        CHECK(MultiTable::value.totalWeight() == 30u); // 20 + 10
    }

    TEST_CASE("multi-item table: maxItemCount() == MaxRoll") {
        CHECK(MultiTable::value.maxItemCount() == 3);
    }

    TEST_CASE("multi-item table: k_TableType is RETURNS_R") {
        CHECK(MultiTable::table_t::k_TableType == LootTableType::RETURNS_R);
    }

    TEST_CASE("multi-item table: rollLootItem returns one of the two items") {
        Container<27> container;
        RNG rng(55ULL);
        bool sawIron = false, sawGold = false;
        for (int i = 0; i < 1000; ++i) {
            lce::ItemState s = MultiTable::value.rollLootItem<27, false>(container, rng);
            const u16 id = s.getID();
            CHECK((id == IRON_INGOT.getID() || id == GOLD_INGOT.getID()));
            if (id == IRON_INGOT.getID()) sawIron = true;
            if (id == GOLD_INGOT.getID()) sawGold = true;
        }
        CHECK(sawIron);
        CHECK(sawGold);
    }

    TEST_CASE("fixed-count table: k_TableType is RETURNS_N") {
        using FixedTable = TableWrapper<3, 3, LootItem<10, 1, 4, BONE.getState()>>;
        CHECK(FixedTable::table_t::k_TableType == LootTableType::RETURNS_N);
        CHECK(FixedTable::value.maxItemCount() == 3);
    }

    TEST_CASE("k_MinRoll and k_MaxRoll accessible on table") {
        CHECK(MultiTable::table_t::k_MinRoll == 1);
        CHECK(MultiTable::table_t::k_MaxRoll == 3);
    }

    TEST_CASE("getRollCount stays within [MinRoll, MaxRoll]") {
        RNG rng(123ULL);
        for (int i = 0; i < 1000; ++i) {
            const i32 count = MultiTable::value.getRollCount<false>(rng);
            CHECK(count >= 1);
            CHECK(count <= 3);
        }
    }

    TEST_CASE("LootTableType::setTableType RETURNS_1") {
        CHECK(setTableType<1, 1>() == LootTableType::RETURNS_1);
    }

    TEST_CASE("LootTableType::setTableType RETURNS_N") {
        CHECK(setTableType<3, 3>() == LootTableType::RETURNS_N);
    }

    TEST_CASE("LootTableType::setTableType RETURNS_R") {
        CHECK(setTableType<1, 5>() == LootTableType::RETURNS_R);
    }
}

// ============================================================
//  Container
// ============================================================
TEST_SUITE("Container") {

    TEST_CASE("default constructed container is empty") {
        Container<27> c;
        CHECK(c.slotCount() == 0u);
    }

    TEST_CASE("addItem increases slotCount") {
        Container<27> c;
        lce::ItemState item(BREAD.getID(), 0);
        item.setItemCount(1);
        c.addItem(item);
        CHECK(c.slotCount() == 1u);
    }

    TEST_CASE("addItem stores correct item") {
        Container<27> c;
        lce::ItemState item(DIAMOND.getID(), 0);
        item.setItemCount(3);
        c.addItem(item);
        CHECK(c.getSlotAt(0).getID() == DIAMOND.getID());
        CHECK(c.getSlotAt(0).getCount() == 3);
    }

    TEST_CASE("addItem sets container index sequentially") {
        Container<27> c;
        lce::ItemState a(IRON_INGOT.getID(), 0); a.setItemCount(1);
        lce::ItemState b(GOLD_INGOT.getID(), 0); b.setItemCount(1);
        c.addItem(a);
        c.addItem(b);
        CHECK(c.getSlotAt(0).getContainerIndex() == 0u);
        CHECK(c.getSlotAt(1).getContainerIndex() == 1u);
    }

    TEST_CASE("clear empties the container") {
        Container<27> c;
        lce::ItemState item(COAL.getID(), 0); item.setItemCount(2);
        c.addItem(item);
        c.addItem(item);
        CHECK(c.slotCount() == 2u);
        c.clear();
        CHECK(c.slotCount() == 0u);
    }

    TEST_CASE("setSlot stores item at given index") {
        Container<27> c;
        // manually set size first by adding a placeholder
        lce::ItemState dummy(0, 0); dummy.setItemCount(0);
        for (int i = 0; i < 5; ++i) c.addItem(dummy);

        lce::ItemState item(EMERALD.getID(), 0); item.setItemCount(7);
        c.setSlot<false>(2, item);
        CHECK(c.getSlotAt(2).getID() == EMERALD.getID());
        CHECK(c.getSlotAt(2).getCount() == 7);
    }

    TEST_CASE("enchantCount is 0 on default constructed container") {
        Container<27> c;
        CHECK(c.enchantCount() == 0u);
    }

    TEST_CASE("multiple items can be added up to capacity") {
        Container<27> c;
        lce::ItemState item(STICK.getID(), 0); item.setItemCount(1);
        for (int i = 0; i < 27; ++i) c.addItem(item);
        CHECK(c.slotCount() == 27u);
    }
}

// ============================================================
//  Buffer
// ============================================================
TEST_SUITE("Buffer") {

    TEST_CASE("size() returns 128") {
        CHECK(Buffer::size() == 128u);
    }

    TEST_CASE("itemBuffer() returns non-null pointer") {
        Buffer buf;
        CHECK(buf.itemBuffer() != nullptr);
    }

    TEST_CASE("stackBuffer() returns non-null pointer") {
        Buffer buf;
        CHECK(buf.stackBuffer() != nullptr);
    }

    TEST_CASE("itemBuffer and stackBuffer are distinct pointers") {
        Buffer buf;
        CHECK(buf.itemBuffer() != buf.stackBuffer());
    }

    TEST_CASE("two Buffer instances have independent storage") {
        Buffer a, b;
        CHECK(a.itemBuffer() != b.itemBuffer());
        CHECK(a.stackBuffer() != b.stackBuffer());
    }
}

// ============================================================
//  spawn_bonus_chest (no enchant functions – safe to call directly)
// ============================================================
TEST_SUITE("LootTable_spawn_bonus_chest") {

    TEST_CASE("deterministic: same seed produces same container") {
        constexpr i64 seed = 123456789LL;
        Container<27> c1, c2;
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c1, seed);
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c2, seed);

        REQUIRE(c1.slotCount() == c2.slotCount());
        for (size_t i = 0; i < c1.slotCount(); ++i) {
            CHECK(c1.getSlotAt(static_cast<i32>(i)).getID() ==
                  c2.getSlotAt(static_cast<i32>(i)).getID());
            CHECK(c1.getSlotAt(static_cast<i32>(i)).getCount() ==
                  c2.getSlotAt(static_cast<i32>(i)).getCount());
        }
    }

    TEST_CASE("different seeds produce different results") {
        Container<27> c1, c2;
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c1, 111LL);
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c2, 222LL);

        // At least one item should differ between the two containers
        bool anyDifference = (c1.slotCount() != c2.slotCount());
        if (!anyDifference) {
            for (size_t i = 0; i < c1.slotCount(); ++i) {
                if (c1.getSlotAt(static_cast<i32>(i)).getID() !=
                    c2.getSlotAt(static_cast<i32>(i)).getID() ||
                    c1.getSlotAt(static_cast<i32>(i)).getCount() !=
                    c2.getSlotAt(static_cast<i32>(i)).getCount()) {
                    anyDifference = true;
                    break;
                }
            }
        }
        CHECK(anyDifference);
    }

    TEST_CASE("container is non-empty after generation") {
        Container<27> c;
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, 42LL);
        CHECK(c.slotCount() > 0u);
    }

    TEST_CASE("all generated item counts are >= 1") {
        Container<27> c;
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, 999LL);
        for (size_t i = 0; i < c.slotCount(); ++i) {
            CHECK(c.getSlotAt(static_cast<i32>(i)).getCount() >= 1);
        }
    }

    TEST_CASE("container can be cleared and reused") {
        Container<27> c;
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, 1LL);
        CHECK(c.slotCount() > 0u);
        c.clear();
        CHECK(c.slotCount() == 0u);
        spawn_bonus_chest.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, 1LL);
        CHECK(c.slotCount() > 0u);
    }

    TEST_CASE("getLootFromSeed is deterministic across two identical RNGs") {
        RNG rng1(777ULL), rng2(777ULL);
        Container<27> c1, c2;
        spawn_bonus_chest.getLootFromSeed<GenMode::MOD_NO_SHUF>(c1, rng1);
        spawn_bonus_chest.getLootFromSeed<GenMode::MOD_NO_SHUF>(c2, rng2);

        REQUIRE(c1.slotCount() == c2.slotCount());
        for (size_t i = 0; i < c1.slotCount(); ++i) {
            CHECK(c1.getSlotAt(static_cast<i32>(i)).getID() ==
                  c2.getSlotAt(static_cast<i32>(i)).getID());
        }
    }

    TEST_CASE("getLootFromChunk is deterministic for same world seed + chunk") {
        Container<27> c1, c2;
        spawn_bonus_chest.getLootFromChunk<GenMode::MOD_NO_SHUF>(c1, 100LL, 0, 0);
        spawn_bonus_chest.getLootFromChunk<GenMode::MOD_NO_SHUF>(c2, 100LL, 0, 0);

        REQUIRE(c1.slotCount() == c2.slotCount());
        for (size_t i = 0; i < c1.slotCount(); ++i) {
            CHECK(c1.getSlotAt(static_cast<i32>(i)).getID() ==
                  c2.getSlotAt(static_cast<i32>(i)).getID());
        }
    }

    TEST_CASE("getLootFromChunk differs for different chunk positions") {
        Container<27> c1, c2;
        spawn_bonus_chest.getLootFromChunk<GenMode::MOD_NO_SHUF>(c1, 100LL, 0, 0);
        spawn_bonus_chest.getLootFromChunk<GenMode::MOD_NO_SHUF>(c2, 100LL, 5, 3);

        bool anyDiff = (c1.slotCount() != c2.slotCount());
        if (!anyDiff) {
            for (size_t i = 0; i < c1.slotCount(); ++i) {
                if (c1.getSlotAt(static_cast<i32>(i)).getID() !=
                    c2.getSlotAt(static_cast<i32>(i)).getID()) {
                    anyDiff = true; break;
                }
            }
        }
        CHECK(anyDiff);
    }
}

// ============================================================
//  buried_treasure (aquatic, no enchant functions)
// ============================================================
TEST_SUITE("LootTable_buried_treasure") {

    TEST_CASE("always contains Heart of the Sea (id check)") {
        // The buried_treasure table has a Roll(1,1) table with a single
        // HEART_OF_THE_SEA item, so every generation must include it.
        constexpr u16 heartID = HEART_OF_THE_SEA.getID();
        for (i64 seed = 0; seed < 20; ++seed) {
            Container<27> c;
            buried_treasure.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, seed);
            bool found = false;
            for (size_t i = 0; i < c.slotCount(); ++i) {
                if (c.getSlotAt(static_cast<i32>(i)).getID() == heartID) {
                    found = true;
                    break;
                }
            }
            CHECK(found);
        }
    }

    TEST_CASE("deterministic for same seed") {
        Container<27> c1, c2;
        buried_treasure.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c1, 55LL);
        buried_treasure.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c2, 55LL);

        REQUIRE(c1.slotCount() == c2.slotCount());
        for (size_t i = 0; i < c1.slotCount(); ++i) {
            CHECK(c1.getSlotAt(static_cast<i32>(i)).getID() ==
                  c2.getSlotAt(static_cast<i32>(i)).getID());
        }
    }

    TEST_CASE("slot count is within plausible bounds") {
        // buried_treasure has 2 tables: Roll(1,1) + Roll(5,12). Max items = 13.
        for (i64 seed = 1; seed <= 10; ++seed) {
            Container<27> c;
            buried_treasure.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, seed);
            CHECK(c.slotCount() >= 1u);
            CHECK(c.slotCount() <= 13u);
        }
    }

    TEST_CASE("all item counts are >= 1") {
        Container<27> c;
        buried_treasure.getLootFromLootTableSeed<GenMode::MOD_NO_SHUF>(c, 7LL);
        for (size_t i = 0; i < c.slotCount(); ++i) {
            CHECK(c.getSlotAt(static_cast<i32>(i)).getCount() >= 1);
        }
    }
}

