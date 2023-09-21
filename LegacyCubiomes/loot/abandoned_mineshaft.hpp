#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    //TODO: make base mineshaft class that generates the rolls in the chunk like stronghold
    class AbandonedMineshaft : public Loot<AbandonedMineshaft> {
    public:
        static void setup();

        template <bool isAquatic, bool shuffle>
        static Container getLootFromLootTableSeed(
                EnchantmentHelper<isAquatic>* helper, uint64_t* lootTableSeed);
    };

    void AbandonedMineshaft::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&GOLDEN_APPLE,              20);
        items1.emplace_back(&ENCHANTED_GOLDEN_APPLE,     1);
        items1.emplace_back(&NAME_TAG,                  30);
        items1.emplace_back(&ENCHANTED_BOOK,            10);
        // function=enchant_randomly
        items1.emplace_back(&IRON_PICKAXE,               5);
        items1.emplace_back(&AIR,                        5);
        lootTables.emplace_back(items1,                  1); // 71

        // table 2
        items2.emplace_back(&IRON_INGOT,                10, 1, 5);
        items2.emplace_back(&GOLD_INGOT,                 5, 1, 3);
        items2.emplace_back(&REDSTONE,                   5, 4, 9);
        items2.emplace_back(&LAPIS_LAZULI,               5, 4, 9);
        items2.emplace_back(&DIAMOND,                    3, 1, 2);
        items2.emplace_back(&COAL,                      10, 3, 8);
        items2.emplace_back(&BREAD,                     15, 1, 3);
        items2.emplace_back(&MELON_SEEDS,               10, 2, 4);
        items2.emplace_back(&PUMPKIN_SEEDS,             10, 2, 4);
        items2.emplace_back(&BEETROOT_SEEDS,            10, 2, 4);
        lootTables.emplace_back(items2,                     2, 4); // 83

        // table 3
        items3.emplace_back(&RAIL,                      20, 4,  8);
        items3.emplace_back(&POWERED_RAIL,               5, 1,  4);
        items3.emplace_back(&DETECTOR_RAIL,              5, 1,  4);
        items3.emplace_back(&ACTIVATOR_RAIL,             5, 1,  4);
        items3.emplace_back(&TORCH,                     15, 1, 16);
        lootTables.emplace_back(items3,                  3); // 50

        maxItemsPossible = 8;
    }


    template <bool isAquatic, bool shuffle>
    Container AbandonedMineshaft::getLootFromLootTableSeed(
            EnchantmentHelper<isAquatic>* helper, uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::ENCHANTED_BOOK_ID) {
                    helper->enchantRandomlyBook.apply(lootTableSeed, &result);
                    // helper->EnchantWithLevelsBook.apply<true>(lootTableSeed, &result, 30);
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

