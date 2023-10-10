#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"

#include "LegacyCubiomes/structures/generation/mineshaft/mineshaft.hpp"

namespace loot {
    // TODO: make base mineshaft class that generates the rolls in the chunk like stronghold
    class AbandonedMineshaft : public Loot<AbandonedMineshaft> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLootFromLootTableSeed(RNG& lootTableSeed);
    };

    void AbandonedMineshaft::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;
        std::vector<ItemEntry> items3;

        // table 1
        items1.emplace_back(&Items::GOLDEN_APPLE,              20);
        items1.emplace_back(&Items::ENCHANTED_GOLDEN_APPLE,     1);
        items1.emplace_back(&Items::NAME_TAG,                  30);
        items1.emplace_back(&Items::ENCHANTED_BOOK,            10); // enchant_randomly
        items1.emplace_back(&Items::IRON_PICKAXE,               5);
        items1.emplace_back(&Items::AIR,                        5);
        lootTables.emplace_back(items1,                  1);

        // table 2
        items2.emplace_back(&Items::IRON_INGOT,                10, 1, 5);
        items2.emplace_back(&Items::GOLD_INGOT,                 5, 1, 3);
        items2.emplace_back(&Items::REDSTONE,                   5, 4, 9);
        items2.emplace_back(&Items::LAPIS_LAZULI,               5, 4, 9);
        items2.emplace_back(&Items::DIAMOND,                    3, 1, 2);
        items2.emplace_back(&Items::COAL,                      10, 3, 8);
        items2.emplace_back(&Items::BREAD,                     15, 1, 3);
        items2.emplace_back(&Items::MELON_SEEDS,               10, 2, 4);
        items2.emplace_back(&Items::PUMPKIN_SEEDS,             10, 2, 4);
        items2.emplace_back(&Items::BEETROOT_SEEDS,            10, 2, 4);
        lootTables.emplace_back(items2,                     2, 4);

        // table 3
        items3.emplace_back(&Items::RAIL,                      20, 4,  8);
        items3.emplace_back(&Items::POWERED_RAIL,               5, 1,  4);
        items3.emplace_back(&Items::DETECTOR_RAIL,              5, 1,  4);
        items3.emplace_back(&Items::ACTIVATOR_RAIL,             5, 1,  4);
        items3.emplace_back(&Items::TORCH,                     15, 1, 16);
        lootTables.emplace_back(items3,                  3);

        maxItemsPossible = 8;
    }


    template <bool shuffle>
    Container AbandonedMineshaft::getLootFromLootTableSeed(RNG& lootTableSeed) {

        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        lootTableSeed.setSeed(lootTableSeed.getSeed());

        // generate loot
        int rollCount;
        int rollIndex;
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed,
                                                 table.getMin(), table.getMax());
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::ENCHANTED_BOOK_ID) {
                    EnchantmentHelper::EnchantRandomlyBook::apply(lootTableSeed, &result);
                }

                chestContents.push_back(result);
            }
        }

        if constexpr (shuffle){
            Container container = Container();
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return  {std::move(chestContents)};
    }
}

