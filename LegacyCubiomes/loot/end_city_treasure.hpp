#pragma once

#include "LegacyCubiomes/loot/classes/Loot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"

namespace loot {
    class EndCityTreasure : public Loot<EndCityTreasure> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(RNG& lootTableSeed);
    };

    void EndCityTreasure::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::DIAMOND,                5, 2, 7);
        items.emplace_back(&Items::IRON_INGOT,             10, 4, 8);
        items.emplace_back(&Items::GOLD_INGOT,             15, 2, 7);
        items.emplace_back(&Items::EMERALD,                2, 2, 6);
        items.emplace_back(&Items::BEETROOT_SEEDS,         5, 1, 10);
        items.emplace_back(&Items::SADDLE,                 3);
        items.emplace_back(&Items::IRON_HORSE_ARMOR,       1);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR,     1);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR,    1);

        items.emplace_back(&Items::DIAMOND_SWORD,          3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::DIAMOND_BOOTS,          3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::DIAMOND_CHESTPLATE,     3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::DIAMOND_LEGGINGS,       3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::DIAMOND_HELMET,         3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::DIAMOND_PICKAXE,        3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::DIAMOND_SHOVEL,         3); // enchant with levels, treasure=true, levels=20-39

        items.emplace_back(&Items::IRON_SWORD,             3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::IRON_BOOTS,             3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::IRON_CHESTPLATE,        3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::IRON_LEGGINGS,          3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::IRON_HELMET,            3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::IRON_PICKAXE,           3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&Items::IRON_SHOVEL,            3); // enchant with levels, treasure=true, levels=20-39

        lootTables.emplace_back(items, 2, 6);

        maxItemsPossible = 6;
    }

    template <bool shuffle>
    Container EndCityTreasure::getLootFromLootTableSeed(RNG& lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(maxItemsPossible);
        lootTableSeed.setSeed(lootTableSeed.getSeed());

        // generate loot
        for (const LootTable& table : lootTables) {
            rollCount = LootTable::getInt<false>(lootTableSeed, table.getMin(), table.getMax());
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                switch (result.item->getItemType()) {
                    case Items::ItemType::ItemTool:
                    case Items::ItemType::ItemSword:
                    case Items::ItemType::ItemArmor: {
                        EnchantmentHelper::EnchantWithLevelsItem::apply(lootTableSeed, &result, 20, 39);
                    }
                    default:
                        break;
                }


                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle) {
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return  { 27, chestContents };
    }
}

