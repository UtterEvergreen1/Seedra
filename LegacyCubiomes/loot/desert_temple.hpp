#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"

namespace loot {
    class DesertTemple : public Loot<DesertTemple> {
    public:
        static void setup();
        template<bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t *lootTableSeed);
    };

    void DesertTemple::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        // table 1
        items1.emplace_back(&Items::DIAMOND,                  5, 1, 3);
        items1.emplace_back(&Items::IRON_INGOT,              15, 1, 5);
        items1.emplace_back(&Items::GOLD_INGOT,              15, 2, 7);
        items1.emplace_back(&Items::EMERALD,                 15, 1, 3);
        items1.emplace_back(&Items::BONE,                    25, 4, 6);
        items1.emplace_back(&Items::SPIDER_EYE,              25, 1, 3);
        items1.emplace_back(&Items::ROTTEN_FLESH,            25, 3, 7);
        items1.emplace_back(&Items::SADDLE,                  20);
        items1.emplace_back(&Items::IRON_HORSE_ARMOR,        15);
        items1.emplace_back(&Items::GOLDEN_HORSE_ARMOR,      10);
        items1.emplace_back(&Items::DIAMOND_HORSE_ARMOR,     5);
        items1.emplace_back(&Items::ENCHANTED_BOOK,          20); // function=enchant_randomly
        items1.emplace_back(&Items::GOLDEN_APPLE,            20);
        items1.emplace_back(&Items::ENCHANTED_GOLDEN_APPLE,  2);
        items1.emplace_back(&Items::AIR,                     15);
        lootTables.emplace_back(items1,              2, 4);

        // table 2
        items2.emplace_back(&Items::BONE,                    10, 1, 8);
        items2.emplace_back(&Items::GUNPOWDER,               10, 1, 8);
        items2.emplace_back(&Items::ROTTEN_FLESH,            10, 1, 8);
        items2.emplace_back(&Items::STRING,                  10, 1, 8);
        items2.emplace_back(&Items::SAND,                    10, 1, 8);
        lootTables.emplace_back(items2,              4);

        maxItemsPossible = 8;
    }

    template<bool shuffle>
    Container DesertTemple::getLootFromLootTableSeed(uint64_t *lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for (const LootTable& table : lootTables) {
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);
                switch (result.item->getID()) {
                    case (Items::AIR_ID): {
                        continue;
                    }
                    case (Items::ENCHANTED_BOOK_ID): {
                        EnchantmentHelper::EnchantRandomlyBook::apply(lootTableSeed, &result);
                        break;
                    }
                    default:
                        break;
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle) {
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, *lootTableSeed);
            return container;
        }
        else
            return  { 27, chestContents };
    }
}

