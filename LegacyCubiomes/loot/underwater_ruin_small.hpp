#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"

namespace loot {
    class UnderwaterRuinSmall : public Loot<UnderwaterRuinSmall> {
    public:
        static void setup();

        template <bool shuffle>
        static Container getLootFromLootTableSeed(RNG& lootTableSeed);
    };

    void UnderwaterRuinSmall::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&Items::COAL,          10, 1, 4);
        items1.emplace_back(&Items::STONE_AXE,      2);
        items1.emplace_back(&Items::ROTTEN_FLESH,   5);
        items1.emplace_back(&Items::EMERALD,        1);
        items1.emplace_back(&Items::WHEAT,         10, 2, 3);
        lootTables.emplace_back(items1,  2, 8);

        items2.emplace_back(&Items::LEATHER_TUNIC,  1);
        items2.emplace_back(&Items::GOLDEN_HELMET,  1);
        items2.emplace_back(&Items::FISHING_ROD,    5); // enchant_randomly
        items2.emplace_back(&Items::NAUTILUS_CORE,  5); // replaced treasure map

        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;
    }

    template <bool shuffle>
    Container UnderwaterRuinSmall::getLootFromLootTableSeed(RNG& lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        lootTableSeed.setSeed(lootTableSeed.getSeed());

        // generate loot
        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.getMin(), table.getMax());
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::FISHING_ROD_ID) {
                    EnchantmentHelper::EnchantRandomlyItem::apply(lootTableSeed, &result);
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return  {27, chestContents};
    }
}