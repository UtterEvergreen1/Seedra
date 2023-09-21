#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class UnderwaterRuinSmall : public Loot<UnderwaterRuinSmall> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void UnderwaterRuinSmall::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&COAL,          10, 1, 4);
        items1.emplace_back(&STONE_AXE,      2);
        items1.emplace_back(&ROTTEN_FLESH,   5);
        items1.emplace_back(&EMERALD,        1);
        items1.emplace_back(&WHEAT,         10, 2, 3);
        lootTables.emplace_back(items1,  2, 8);

        items2.emplace_back(&LEATHER_TUNIC,  1);
        items2.emplace_back(&GOLDEN_HELMET,  1);
        items2.emplace_back(&FISHING_ROD,    5); // enchant_randomly
        items2.emplace_back(&NAUTILUS_CORE,  5); // replaced treasure map

        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;
    }

    template <bool shuffle>
    Container UnderwaterRuinSmall::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::FISHING_ROD_ID) {
                    EnchantmentHelper::EnchantRandomly::apply<false>(lootTableSeed, &result);
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, *lootTableSeed);
            return container;
        }
        else
            return  {27, chestContents};
    }
}