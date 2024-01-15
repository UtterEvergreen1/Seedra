#pragma once

#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"
#include "LegacyCubiomes/loot/classes/Loot.hpp"

namespace loot {
    class EndCityTreasure : public Loot<EndCityTreasure> {
    public:
        static void setup();
    };

    inline void EndCityTreasure::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::DIAMOND, 5, 2, 7);
        items.emplace_back(&Items::IRON_INGOT, 10, 4, 8);
        items.emplace_back(&Items::GOLD_INGOT, 15, 2, 7);
        items.emplace_back(&Items::EMERALD, 2, 2, 6);
        items.emplace_back(&Items::BEETROOT_SEEDS, 5, 1, 10);
        items.emplace_back(&Items::SADDLE, 3);
        items.emplace_back(&Items::IRON_HORSE_ARMOR, 1);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR, 1);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR, 1);

        auto* enchantFunction = new EnchantWithLevelsItem({20, 39});
        items.emplace_back(&Items::DIAMOND_SWORD, enchantFunction, 3);
        items.emplace_back(&Items::DIAMOND_BOOTS, enchantFunction, 3);
        items.emplace_back(&Items::DIAMOND_CHESTPLATE, enchantFunction, 3);
        items.emplace_back(&Items::DIAMOND_LEGGINGS, enchantFunction, 3);
        items.emplace_back(&Items::DIAMOND_HELMET, enchantFunction, 3);
        items.emplace_back(&Items::DIAMOND_PICKAXE, enchantFunction, 3);
        items.emplace_back(&Items::DIAMOND_SHOVEL, enchantFunction, 3);

        items.emplace_back(&Items::IRON_SWORD, enchantFunction, 3);
        items.emplace_back(&Items::IRON_BOOTS, enchantFunction, 3);
        items.emplace_back(&Items::IRON_CHESTPLATE, enchantFunction, 3);
        items.emplace_back(&Items::IRON_LEGGINGS, enchantFunction, 3);
        items.emplace_back(&Items::IRON_HELMET, enchantFunction, 3);
        items.emplace_back(&Items::IRON_PICKAXE, enchantFunction, 3);
        items.emplace_back(&Items::IRON_SHOVEL, enchantFunction, 3);

        lootTables.emplace_back(items, 2, 6);

        maxItemsPossible = 6;
    }
} // namespace loot
