#pragma once

#include "base_classes/stronghold_loot.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"


using namespace Items;
using stronghold_generator::StrongholdGenerator;

namespace loot_tables {
    template<bool isAquatic>
    class StrongholdCorridor : public StrongholdLoot<StrongholdCorridor<isAquatic>> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t lootTableSeed);

        template<bool checkCaves, bool shuffle>
        [[nodiscard]] static Container getAltarChestLoot(int64_t seed, BIOMESCALE biomeSize, BasePiece* alterChestPiece,
                                                         StrongholdGenerator* strongholdGenerator);
        template<bool checkCaves, bool shuffle>
        [[nodiscard]] static std::vector<Container> getAllAlterChestLoot(
                int64_t worldSeed, BIOMESCALE biomeSize, StrongholdGenerator* strongholdGenerator);
    };

    template<bool isAquatic>
    void StrongholdCorridor<isAquatic>::setup() {
        std::vector<ItemEntry> items;

        items.emplace_back(&ENDER_PEARL,         50);
        items.emplace_back(&EMERALD,             15, 1, 3);
        if constexpr (isAquatic)
            items.emplace_back(&DIAMOND,         15, 1, 3);
        items.emplace_back(&IRON_INGOT,          50, 1, 5);
        items.emplace_back(&GOLD_INGOT,          25, 1, 3);
        items.emplace_back(&REDSTONE,            25, 4, 9);
        items.emplace_back(&BREAD,               75, 1, 3);
        items.emplace_back(&APPLE,               75, 1, 3);
        items.emplace_back(&IRON_PICKAXE,        25);
        items.emplace_back(&IRON_SWORD,          25);
        items.emplace_back(&IRON_CHESTPLATE,     25);
        items.emplace_back(&IRON_HELMET,         25);
        items.emplace_back(&IRON_LEGGINGS,       25);
        items.emplace_back(&IRON_BOOTS,          25);
        items.emplace_back(&GOLDEN_APPLE,        5);
        items.emplace_back(&SADDLE,              5);
        items.emplace_back(&IRON_HORSE_ARMOR,    5);
        items.emplace_back(&GOLDEN_HORSE_ARMOR,  5);
        items.emplace_back(&DIAMOND_HORSE_ARMOR, 5);
        items.emplace_back(&ENCHANTED_BOOK,      6); // function=enchant_with_levels, levels=30, treasure=true
        if constexpr (isAquatic) {
            StrongholdCorridor<isAquatic>::lootTables.emplace_back(items, 2, 3, 511);
        } else {
            StrongholdCorridor<isAquatic>::lootTables.emplace_back(items, 2, 3, 496);
        }
        StrongholdCorridor<isAquatic>::maxItemsPossible = 3;
    }

    template<bool isAquatic>
    template <bool shuffle>
    Container StrongholdCorridor<isAquatic>::getLootFromLootTableSeed(uint64_t lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        setSeed(&lootTableSeed, lootTableSeed);

        // generate loot
        for(const LootTable& table : loot_tables::StrongholdCorridor<isAquatic>::lootTables){
            rollCount = LootTable::getInt<false>(&lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(&lootTableSeed);

                if EXPECT_FALSE(result.item == &Items::ENCHANTED_BOOK) {
                    EnchantmentHelper::EnchantWithLevels::apply(&lootTableSeed, &result, 30, true, true);
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container(27);
            container.shuffleIntoContainer(chestContents, lootTableSeed);
            return container;
        }
        else
            return  {27, chestContents};
    }

    template<bool isAquatic>
    template<bool checkCaves, bool shuffle>
    Container StrongholdCorridor<isAquatic>::getAltarChestLoot(int64_t seed, BIOMESCALE biomeSize,
                                                               BasePiece* alterChestPiece,
                                                               StrongholdGenerator* strongholdGenerator) {
        uint64_t lootSeed = Loot<StrongholdCorridor<isAquatic>>::getLootSeed<checkCaves>(seed, biomeSize,
                                                alterChestPiece->getWorldX(3, 3),
                                                alterChestPiece->getWorldY(2),
                                                alterChestPiece->getWorldZ(3, 3),
                                                strongholdGenerator);
        return Loot<StrongholdCorridor<isAquatic>>::getLootFromSeed<shuffle>(&lootSeed);
    }

    template<bool isAquatic>
    template<bool checkCaves, bool shuffle>
    std::vector<Container> StrongholdCorridor<isAquatic>::getAllAlterChestLoot(int64_t worldSeed, BIOMESCALE biomeSize,
                                                                               StrongholdGenerator* strongholdGenerator) {
        std::vector<Container> alterChests(strongholdGenerator->numAltarChests);
        for(int alterChestIndex = 0; alterChestIndex < strongholdGenerator->numAltarChests; alterChestIndex++) {
            alterChests[alterChestIndex] = getAltarChestLoot<checkCaves, shuffle>(
                    worldSeed, biomeSize, strongholdGenerator->altarChests[alterChestIndex], strongholdGenerator);
        }
        return alterChests;
    }
}

