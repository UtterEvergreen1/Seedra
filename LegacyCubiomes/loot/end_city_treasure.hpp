#pragma once

#include "base_classes/loot.hpp"

using namespace Items;

namespace loot_tables {
    class EndCityTreasure : public Loot<EndCityTreasure> {
    public:
        static void setup();
        template <bool isAquatic, bool shuffle>
        static Container getLootFromLootTableSeed(
                EnchantmentHelper<isAquatic>* helper, uint64_t* lootTableSeed);
    };

    void EndCityTreasure::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&DIAMOND,                5, 2, 7);
        items.emplace_back(&IRON_INGOT,             10, 4, 8);
        items.emplace_back(&GOLD_INGOT,             15, 2, 7);
        items.emplace_back(&EMERALD,                2, 2, 6);
        items.emplace_back(&BEETROOT_SEEDS,         5, 1, 10);
        items.emplace_back(&SADDLE,                 3);
        items.emplace_back(&IRON_HORSE_ARMOR,       1);
        items.emplace_back(&GOLDEN_HORSE_ARMOR,     1);
        items.emplace_back(&DIAMOND_HORSE_ARMOR,    1);

        items.emplace_back(&DIAMOND_SWORD,          3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&DIAMOND_BOOTS,          3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&DIAMOND_CHESTPLATE,     3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&DIAMOND_LEGGINGS,       3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&DIAMOND_HELMET,         3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&DIAMOND_PICKAXE,        3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&DIAMOND_SHOVEL,         3); // enchant with levels, treasure=true, levels=20-39

        items.emplace_back(&IRON_SWORD,             3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&IRON_BOOTS,             3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&IRON_CHESTPLATE,        3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&IRON_LEGGINGS,          3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&IRON_HELMET,            3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&IRON_PICKAXE,           3); // enchant with levels, treasure=true, levels=20-39
        items.emplace_back(&IRON_SHOVEL,            3); // enchant with levels, treasure=true, levels=20-39

        lootTables.emplace_back(items, 2, 6);

        maxItemsPossible = 6;
    }

    template <bool isAquatic, bool shuffle>
    Container EndCityTreasure::getLootFromLootTableSeed(EnchantmentHelper<isAquatic>* helper, uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for (const LootTable& table : lootTables) {
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                switch (result.item->getItemType()) {
                    case Items::ItemType::ItemTool:
                    case Items::ItemType::ItemSword:
                    case Items::ItemType::ItemArmor: {
                        helper->enchantWithLevelsItem.template apply<true>(lootTableSeed, &result, 20, 39);
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

