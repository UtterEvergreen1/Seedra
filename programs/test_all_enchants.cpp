
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/loot/tables/include.hpp"

int main(int argc, char* argv[]) {

    EnchantmentHelper::setup(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC);
    loot::setup<true>();

    Container loot;
    uint64_t lootTableSeed = 1;

    loot = loot::AbandonedMineshaft::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "AbandonedMineshaft:" << loot << std::endl;

    loot = loot::Blacksmith::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "Blacksmith:" << loot << std::endl;

    loot = loot::Mansion::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "Mansion:" << loot << std::endl;

    loot = loot::DesertTemple::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "DesertTemple:" << loot << std::endl;

    loot = loot::EndCityTreasure::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "EndCityTreasure:" << loot << std::endl;

    loot = loot::Igloo::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "Igloo:" << loot << std::endl;

    loot = loot::JungleTemple::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "JungleTemple:" << loot << std::endl;

    loot = loot::JungleTempleDispenser::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "JungleTempleDispenser:" << loot << std::endl;

    loot = loot::NetherBridge::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "NetherBridge:" << loot << std::endl;

    loot = loot::SimpleDungeon::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "SimpleDungeon:" << loot << std::endl;

    loot = loot::SpawnBonusChest::getLootFromLootTableSeed<true>(lootTableSeed);
    std::cout << "SpawnBonusChest:" << loot << std::endl;

    loot = loot::StrongholdCrossing::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "StrongholdCrossing:" << loot << std::endl;

    loot = loot::StrongholdLibrary::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "StrongholdLibrary:" << loot << std::endl;

    loot = loot::StrongholdCorridor<true>::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "StrongholdCorridor:" << loot << std::endl;

    loot = loot::BuriedTreasure::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "BuriedTreasure:" << loot << std::endl;

    loot = loot::UnderwaterRuinBig::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "UnderwaterRuinBig:" << loot << std::endl;

    loot = loot::UnderwaterRuinSmall::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "UnderwaterRuinSmall:" << loot << std::endl;

    loot = loot::ShipwreckTreasure::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "ShipwreckTreasure:" << loot << std::endl;

    loot = loot::ShipwreckSupply::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "ShipwreckSupply:" << loot << std::endl;

    loot = loot::ShipwreckMap::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << "ShipwreckMap:" << loot << std::endl;

    return 0;
}