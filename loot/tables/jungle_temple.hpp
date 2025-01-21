#pragma once

#include "loot/classes/EnchantWithLevels.hpp"
#include "loot/classes/Loot.hpp"


namespace loot {
    class JungleTemple : public Loot<JungleTemple> {
    public:
        static void setup();
    };

    inline void JungleTemple::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&lce::items::EMERALD, 15, 1, 3);
        items.emplace_back(&lce::items::IRON_INGOT, 50, 1, 5);
        items.emplace_back(&lce::items::GOLD_INGOT, 75, 2, 7);
        items.emplace_back(&lce::items::EMERALD, 10, 1, 3);
        items.emplace_back(&lce::items::BONE, 100, 4, 6);
        items.emplace_back(&lce::items::ROTTEN_FLESH, 80, 3, 7);
        items.emplace_back(&lce::items::SADDLE, 15);
        items.emplace_back(&lce::items::IRON_HORSE_ARMOR, 5);
        items.emplace_back(&lce::items::GOLDEN_HORSE_ARMOR, 5);
        items.emplace_back(&lce::items::DIAMOND_HORSE_ARMOR, 5);
        items.emplace_back(&lce::items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30), 6);

        lootTables.emplace_back(items, 2, 6);

        maxItemsPossible = 6;
    }
}
