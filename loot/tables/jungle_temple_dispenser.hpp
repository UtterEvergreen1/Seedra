#pragma once

#include "loot/classes/Loot.hpp"


namespace loot {
    class JungleTempleDispenser : public Loot<JungleTempleDispenser> {
    public:
        static void setup();

        template<bool shuffle>
        static Container getLoot(i64 worldSeed, int chunkX, int chunkZ);
        static Container getLootLegacy(i64 worldSeed, int chunkX, int chunkZ);
    };

    inline void JungleTempleDispenser::setup() {
        std::vector<ItemEntry> items = {{&lce::items::ARROW, 30, 2, 7}};
        lootTables.emplace_back(items, 1, 2);
        maxItemsPossible = 2;
    }

    template<bool shuffle>
    Container JungleTempleDispenser::getLoot(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        std::vector<ItemStack> chestContents;

        RNG lootTableSeed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);
        lootTableSeed = RNG(lootTableSeed.nextLong());

        //generate loot
        for (const LootTable& table: lootTables) {
            c_int rollCount = lootTableSeed.nextInt(table.getMin(), table.getMax());
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);
                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle) {
            Container container = Container(9);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        } else
            return {9, chestContents};
    }

    inline Container JungleTempleDispenser::getLootLegacy(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        auto chestContents = Container(9);

        RNG lootTableSeed = RNG::getPopulationSeed(worldSeed, chunkX, chunkZ);

        // generate loot
        for (const LootTable& table: lootTables) {
            c_int rollCount = lootTableSeed.nextIntLegacy(table.getMin(), table.getMax());
            for (int rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<true>(lootTableSeed);
                chestContents.setInventorySlotContents(lootTableSeed.nextInt(9), std::move(result));
            }
        }
        return chestContents;
    }
}
