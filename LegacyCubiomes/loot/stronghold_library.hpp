#pragma once

#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"
#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"

namespace loot {
    class StrongholdLibrary : public StrongholdLoot<StrongholdLibrary> {
    public:
        static void setup();
    };

    void StrongholdLibrary::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::BOOK, 100, 1, 3);
        items.emplace_back(&Items::PAPER, 100, 2, 7);
        items.emplace_back(&Items::MAP, 5);
        items.emplace_back(&Items::COMPASS, 5);
        items.emplace_back(&Items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30), 60);
        // function=enchant_with_levels, levels=30, treasure=true

        lootTables.emplace_back(items, 2, 10);

        maxItemsPossible = 10;
    }
} // namespace loot
