#pragma once

#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"
#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"


namespace loot {
    class StrongholdLibrary : public StrongholdLoot<StrongholdLibrary> {
    public:
        static void setup();
    };

    inline void StrongholdLibrary::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&Items::BOOK, 100, 1, 3);
        items.emplace_back(&Items::PAPER, 100, 2, 7);
        items.emplace_back(&Items::MAP, 5);
        items.emplace_back(&Items::COMPASS, 5);
        items.emplace_back(&Items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30), 60);

        lootTables.emplace_back(items, 2, 10);

        maxItemsPossible = 10;
    }
}
