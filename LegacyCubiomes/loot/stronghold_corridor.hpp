#pragma once
#include "LegacyCubiomes/loot/classes/EnchantWithLevels.hpp"
#include "LegacyCubiomes/loot/classes/StrongholdLoot.hpp"


namespace loot {
    template<bool isAquatic>
    class StrongholdCorridor : public StrongholdLoot<StrongholdCorridor<isAquatic>> {
    public:
        static void setup();

        template<bool checkCaves, bool shuffle, bool checkWaterCaves = false>
        ND static Container getAltarChestLoot(const Generator& g, const Piece& altarChestPiece,
                                              generation::Stronghold* strongholdGenerator);

        template<bool checkCaves, bool shuffle, bool checkWaterCaves = false>
        ND static std::vector<Container> getAllAltarChestLoot(const Generator& g,
                                                              generation::Stronghold* strongholdGenerator);
    };

    template<bool isAquatic>
    void StrongholdCorridor<isAquatic>::setup() {
        std::vector<ItemEntry> items;
        if constexpr (isAquatic) items.reserve(20);
        else
            items.reserve(19);

        items.emplace_back(&Items::ENDER_PEARL, 50);
        items.emplace_back(&Items::EMERALD, 15, 1, 3);
        if constexpr (isAquatic) items.emplace_back(&Items::DIAMOND, 15, 1, 3);
        items.emplace_back(&Items::IRON_INGOT, 50, 1, 5);
        items.emplace_back(&Items::GOLD_INGOT, 25, 1, 3);
        items.emplace_back(&Items::REDSTONE, 25, 4, 9);
        items.emplace_back(&Items::BREAD, 75, 1, 3);
        items.emplace_back(&Items::APPLE, 75, 1, 3);
        items.emplace_back(&Items::IRON_PICKAXE, 25);
        items.emplace_back(&Items::IRON_SWORD, 25);
        items.emplace_back(&Items::IRON_CHESTPLATE, 25);
        items.emplace_back(&Items::IRON_HELMET, 25);
        items.emplace_back(&Items::IRON_LEGGINGS, 25);
        items.emplace_back(&Items::IRON_BOOTS, 25);
        items.emplace_back(&Items::GOLDEN_APPLE, 5);
        items.emplace_back(&Items::SADDLE, 5);
        items.emplace_back(&Items::IRON_HORSE_ARMOR, 5);
        items.emplace_back(&Items::GOLDEN_HORSE_ARMOR, 5);
        items.emplace_back(&Items::DIAMOND_HORSE_ARMOR, 5);
        items.emplace_back(&Items::ENCHANTED_BOOK, new EnchantWithLevelsBook(30), 6);

        StrongholdCorridor::lootTables.emplace_back(items, 2, 3);

        StrongholdCorridor::maxItemsPossible = 3;
    }

    template<bool isAquatic>
    template<bool checkCaves, bool shuffle, bool checkWaterCaves>
    Container StrongholdCorridor<isAquatic>::getAltarChestLoot(const Generator& g, const Piece& altarChestPiece,
                                                               generation::Stronghold* strongholdGenerator) {
        RNG lootSeed = StrongholdLoot<StrongholdCorridor>::template getLootSeed<checkCaves, checkWaterCaves>(
                g, strongholdGenerator, altarChestPiece, altarChestPiece.getWorldX(3, 3) >> 4,
                altarChestPiece.getWorldZ(3, 3) >> 4);
        if (lootSeed == -1) return {};

        return Loot<StrongholdLoot<StrongholdCorridor>>::template getLootFromSeed<shuffle>(lootSeed);
    }

    template<bool isAquatic>
    template<bool checkCaves, bool shuffle, bool checkWaterCaves>
    std::vector<Container>
    StrongholdCorridor<isAquatic>::getAllAltarChestLoot(const Generator& g,
                                                        generation::Stronghold* strongholdGenerator) {
        std::vector<Container> altarChests(strongholdGenerator->altarChestArraySize);
        for (int altarChestIndex = 0; altarChestIndex < strongholdGenerator->altarChestArraySize; altarChestIndex++)
            altarChests[altarChestIndex] =
                    StrongholdCorridor::getAltarChestLoot<checkCaves, shuffle, checkWaterCaves>(
                            g, *strongholdGenerator->altarChestsArray[altarChestIndex], strongholdGenerator);

        return altarChests;
    }
}
