#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"

namespace loot_tables {
    class UnderwaterRuinBig : public Loot<UnderwaterRuinBig> {
    public:
        static void setup();

        template<bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t *lootTableSeed);
    };

    void UnderwaterRuinBig::setup() {
        std::vector<ItemEntry> items1;
        std::vector<ItemEntry> items2;

        items1.emplace_back(&Items::COAL, 10, 1, 4);
        items1.emplace_back(&Items::GOLD_NUGGET, 10, 1, 3);
        items1.emplace_back(&Items::EMERALD, 1);
        items1.emplace_back(&Items::WHEAT, 10, 2, 3);
        lootTables.emplace_back(items1, 2, 8);

        items2.emplace_back(&Items::GOLDEN_APPLE, 1);
        items2.emplace_back(&Items::ENCHANTED_BOOK, 5); // enchant_randomly
        items2.emplace_back(&Items::LEATHER_TUNIC, 1);
        items2.emplace_back(&Items::GOLDEN_HELMET, 1);
        items2.emplace_back(&Items::FISHING_ROD, 5); // enchant_randomly
        items2.emplace_back(&Items::TREASURE_MAP, 10); // IDK the weight
        items2.emplace_back(&Items::POISONOUS_POTATO, 10); // zoom=1

        lootTables.emplace_back(items2, 1);

        maxItemsPossible = 9;

    }


    template<bool shuffle>
    Container UnderwaterRuinBig::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        setSeed(lootTableSeed, *lootTableSeed);

        for(const LootTable& table : lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                switch (result.item->getID()) {
                    case (Items::ENCHANTED_BOOK_ID): {
                        EnchantmentHelper::EnchantRandomlyBook::apply(lootTableSeed, &result);
                        break;
                    }
                    case (Items::FISHING_ROD_ID): {
                        EnchantmentHelper::EnchantRandomlyItem::apply(lootTableSeed, &result);
                        break;
                    }
                    default:
                        break;


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