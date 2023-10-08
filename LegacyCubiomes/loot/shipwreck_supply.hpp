#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"

namespace loot {
    class ShipwreckSupply : public Loot<ShipwreckSupply> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);
    };

    void ShipwreckSupply::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::PAPER,              8, 1, 12);
        items.emplace_back(&Items::POTATO,             7, 2, 6);
        items.emplace_back(&Items::POISONOUS_POTATO,   7, 2, 6);
        items.emplace_back(&Items::CARROT,             7, 4, 8);
        items.emplace_back(&Items::WHEAT,              7, 8, 21);
        items.emplace_back(&Items::COAL,               6, 2, 8);
        items.emplace_back(&Items::ROTTEN_FLESH,       5, 5, 24);
        items.emplace_back(&Items::PUMPKIN,            2, 1, 3);
        items.emplace_back(&Items::GUNPOWDER,          3, 1, 5);
        items.emplace_back(&Items::TNT,                1, 1, 2);
        items.emplace_back(&Items::LEATHER_HELMET, 3); // enchant randomly
        items.emplace_back(&Items::LEATHER_TUNIC,  3); // enchant randomly
        items.emplace_back(&Items::LEATHER_PANTS,  3); // enchant randomly
        items.emplace_back(&Items::LEATHER_BOOTS,  3); // enchant randomly

        lootTables.emplace_back(items, 3, 10);
        maxItemsPossible = 10;
    }


    template <bool shuffle>
    Container ShipwreckSupply::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        setSeed(lootTableSeed, *lootTableSeed);

        for (const LootTable& table : lootTables) {
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if (result.item->getItemType() == Items::ItemType::ItemArmor) {
                    EnchantmentHelper::EnchantRandomlyItem::apply(lootTableSeed, &result);
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
