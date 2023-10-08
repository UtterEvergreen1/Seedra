#pragma once

#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"
#include "LegacyCubiomes/enchants/include.hpp"




namespace loot {
    template<bool isAquatic>
    class StrongholdCorridor : public StrongholdLoot<StrongholdCorridor<isAquatic>> {
    public:
        static void setup();
        template <bool shuffle>
        static Container getLootFromLootTableSeed(uint64_t* lootTableSeed);

        template<bool checkCaves, bool shuffle>
        ND static Container getAltarChestLoot(const Generator& g, Piece* altarChestPiece, generation::Stronghold* strongholdGenerator);

        template<bool checkCaves, bool shuffle>
        ND static std::vector<Container> getAllAltarChestLoot(const Generator& g, generation::Stronghold* strongholdGenerator);
    };

    template<bool isAquatic>
    void StrongholdCorridor<isAquatic>::setup() {
        std::vector<ItemEntry> items;
        items.reserve(21);

        items.emplace_back(&Items::ENDER_PEARL,         50);
        items.emplace_back(&Items::EMERALD,             15, 1, 3);
        if constexpr (isAquatic)
            items.emplace_back(&Items::DIAMOND,         15, 1, 3);
        items.emplace_back(&Items::IRON_INGOT,          50, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT,          25, 1, 3);
        items.emplace_back(&Items::REDSTONE,            25, 4, 9);
        items.emplace_back(&Items::BREAD,               75, 1, 3);
        items.emplace_back(&Items::APPLE,               75, 1, 3);
        items.emplace_back(&Items::IRON_PICKAXE,        25);
        items.emplace_back(&Items::IRON_SWORD,          25);
        items.emplace_back(&Items::IRON_CHESTPLATE,     25);
        items.emplace_back(&Items::IRON_HELMET,         25);
        items.emplace_back(&Items::IRON_LEGGINGS,       25);
        items.emplace_back(&Items::IRON_BOOTS,          25);
        items.emplace_back(&Items::GOLDEN_APPLE,        5);
        items.emplace_back(&Items::SADDLE,              5);
        items.emplace_back(&Items::IRON_HORSE_ARMOR,    5);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR,  5);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR, 5);
        items.emplace_back(&Items::ENCHANTED_BOOK,      6);
        // function=enchant_with_levels, levels=30, treasure=true

        StrongholdCorridor<isAquatic>::lootTables.emplace_back(items, 2, 3);
        StrongholdCorridor<isAquatic>::maxItemsPossible = 3;
    }

    template<bool isAquatic>
    template <bool shuffle>
    Container StrongholdCorridor<isAquatic>::getLootFromLootTableSeed(uint64_t* lootTableSeed) {
        int rollCount;
        int rollIndex;
        std::vector<ItemStack> chestContents;
        chestContents.reserve(StrongholdCorridor<isAquatic>::maxItemsPossible);
        setSeed(lootTableSeed, *lootTableSeed);

        // generate loot
        for(const LootTable& table : StrongholdCorridor<isAquatic>::lootTables){
            rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
            for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
                ItemStack result = table.createLootRoll<false>(lootTableSeed);

                if EXPECT_FALSE(result.item->getID() == Items::ENCHANTED_BOOK_ID) {
                    EnchantmentHelper::EnchantWithLevelsBook::apply(lootTableSeed, &result, 30);
                }

                chestContents.push_back(result);
            }
        }
        if constexpr (shuffle){
            Container container = Container();
            container.shuffleIntoContainer(chestContents, *lootTableSeed);
            return container;
        }
        else
            return  {std::move(chestContents)};
    }


    template<bool isAquatic>
    template<bool checkCaves, bool shuffle>
    Container StrongholdCorridor<isAquatic>::getAltarChestLoot(const Generator& g,
                                                               Piece* altarChestPiece,
                                                               generation::Stronghold* strongholdGenerator) {
        uint64_t lootSeed = StrongholdLoot<StrongholdCorridor<isAquatic>>::template getLootSeed<checkCaves>(g,
                                                                                                            altarChestPiece->getWorldX(3, 3),
                                                                                                            altarChestPiece->getWorldY(2),
                                                                                                            altarChestPiece->getWorldZ(3, 3),
                                                                                                            strongholdGenerator);
        return Loot<StrongholdLoot<StrongholdCorridor<isAquatic>>>::template getLootFromSeed<shuffle>(&lootSeed);
    }


    template<bool isAquatic>
    template<bool checkCaves, bool shuffle>
    std::vector<Container> StrongholdCorridor<isAquatic>::getAllAltarChestLoot(const Generator& g,
                                                                               generation::Stronghold* strongholdGenerator) {
        std::vector<Container> altarChests(strongholdGenerator->altarChestArraySize);
        for (int altarChestIndex = 0; altarChestIndex < strongholdGenerator->altarChestArraySize; altarChestIndex++)
            altarChests[altarChestIndex] = StrongholdCorridor<isAquatic>
                                           ::template getAltarChestLoot<checkCaves, shuffle>(g,
                                                                                             strongholdGenerator->altarChestsArray[altarChestIndex],
                                                                                             strongholdGenerator);

        return altarChests;
    }
}

