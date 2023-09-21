#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class ShipwreckSupply : public Loot<ShipwreckSupply> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void ShipwreckSupply::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&PAPER, 8, 1, 12);
        items.emplace_back(&POTATO, 7, 2, 6);
        items.emplace_back(&POISONOUS_POTATO, 7, 2, 6);
        items.emplace_back(&CARROT, 7, 4, 8);
        items.emplace_back(&WHEAT, 7, 8, 21);
        items.emplace_back(&COAL, 6, 2, 8);
        items.emplace_back(&ROTTEN_FLESH, 5, 5, 24);
        items.emplace_back(&PUMPKIN, 2, 1, 3);
        items.emplace_back(&GUNPOWDER, 3, 1, 5);
        items.emplace_back(&TNT, 1, 1, 2);
        items.emplace_back(&LEATHER_HELMET, 3); // enchant randomly
        items.emplace_back(&LEATHER_TUNIC, 3); // enchant randomly
        items.emplace_back(&LEATHER_PANTS, 3); // enchant randomly
        items.emplace_back(&LEATHER_BOOTS, 3); // enchant randomly


        lootTables.emplace_back(items, 3, 10);
        maxItemsPossible = 10;
    }


    template <bool shuffle>
    Container ShipwreckSupply::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        for (const LootTable& table : lootTables) {
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if (result.item->getItemType() == Items::ItemType::ItemArmor) {
                    EnchantmentHelper::EnchantRandomly::apply<false>(lootTableSeed, &result);
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
