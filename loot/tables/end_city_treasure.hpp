#pragma once

#include "loot/classes/EnchantWithLevels.hpp"
#include "loot/classes/Loot.hpp"

namespace loot {
    class EndCityTreasure : public Loot<EndCityTreasure> {
    public:
        static void setup();
    };

    inline void EndCityTreasure::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::DIAMOND, 5, 2, 7);
        items.emplace_back(&lce::items::IRON_INGOT, 10, 4, 8);
        items.emplace_back(&lce::items::GOLD_INGOT, 15, 2, 7);
        items.emplace_back(&lce::items::EMERALD, 2, 2, 6);
        items.emplace_back(&lce::items::BEETROOT_SEEDS, 5, 1, 10);
        items.emplace_back(&lce::items::SADDLE, 3);
        items.emplace_back(&lce::items::IRON_HORSE_ARMOR, 1);
        items.emplace_back(&lce::items::GOLDEN_HORSE_ARMOR, 1);
        items.emplace_back(&lce::items::DIAMOND_HORSE_ARMOR, 1);

        auto* enchantFunction = new EnchantWithLevelsItem({20, 39});
        items.emplace_back(&lce::items::DIAMOND_SWORD, enchantFunction, 3);
        items.emplace_back(&lce::items::DIAMOND_BOOTS, enchantFunction, 3);
        items.emplace_back(&lce::items::DIAMOND_CHESTPLATE, enchantFunction, 3);
        items.emplace_back(&lce::items::DIAMOND_LEGGINGS, enchantFunction, 3);
        items.emplace_back(&lce::items::DIAMOND_HELMET, enchantFunction, 3);
        items.emplace_back(&lce::items::DIAMOND_PICKAXE, enchantFunction, 3);
        items.emplace_back(&lce::items::DIAMOND_SHOVEL, enchantFunction, 3);

        items.emplace_back(&lce::items::IRON_SWORD, enchantFunction, 3);
        items.emplace_back(&lce::items::IRON_BOOTS, enchantFunction, 3);
        items.emplace_back(&lce::items::IRON_CHESTPLATE, enchantFunction, 3);
        items.emplace_back(&lce::items::IRON_LEGGINGS, enchantFunction, 3);
        items.emplace_back(&lce::items::IRON_HELMET, enchantFunction, 3);
        items.emplace_back(&lce::items::IRON_PICKAXE, enchantFunction, 3);
        items.emplace_back(&lce::items::IRON_SHOVEL, enchantFunction, 3);

        lootTables.emplace_back(items, 2, 6);

        maxItemsPossible = 6;
    }
} // namespace loot
