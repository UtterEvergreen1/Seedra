#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class UnderwaterRuinBig : public Loot<UnderwaterRuinBig> {
    public:
        static void setup();

        // template<bool shuffle>
        // static Container getLootFromLootTableSeed(uint64_t *lootTableSeed);
    };

    void UnderwaterRuinBig::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&COAL, 10, 1, 4);
        items1.emplace_back(&GOLD_NUGGET, 10, 1, 3);
        items1.emplace_back(&EMERALD, 1);
        items1.emplace_back(&WHEAT, 10, 2, 3);
        lootTables.emplace_back(items1, 2, 8);

        items2.emplace_back(&GOLDEN_APPLE, 1);
        items2.emplace_back(&ENCHANTED_BOOK, 5); // enchant_randomly
        items2.emplace_back(&LEATHER_TUNIC, 1);
        items2.emplace_back(&GOLDEN_HELMET, 1);
        items2.emplace_back(&FISHING_ROD, 5); // enchant_randomly
        items2.emplace_back(&TREASURE_MAP, 10); // zoom=1
        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;

    }
    /*
    template<bool shuffle>
    Container UnderwaterRuinBig::getLootFromLootTableSeed(uint64_t *lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : loot_tables::SpawnBonusChest::lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if (result.item == &ENCHANTED_BOOK) {
                    EnchantmentHelper::EnchantRandomly::apply<true>(lootTableSeed, &result);
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
     */
}