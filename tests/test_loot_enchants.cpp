

#include "lce/items/itemsInit.hpp"


#include "common/timer.hpp"
#include <fstream>
#include <iostream>


#include "loot/classes/Loot.hpp"
#include "loot/classes/LootFunctions.hpp"
#include "loot/classes/LootItem.hpp"
#include "loot/classes/LootTable.hpp"

#include "loot/Tables.hpp"


template<bool debug>
void updateData(
    std::ofstream& file, c_u64 lootTableSeed,
    size_t& count, loot::Container<27>& container,
    const std::string& header, const std::string& str) {
    count += container.slotCount();
    if (debug) {
        file << header << lootTableSeed << " " << str << container << "\n";
    }
    container.clear();
}


template<lce::CONSOLE THE_CONSOLE, LCEVERSION THE_VERSION, loot::GenMode Mode, bool debug>
int writeToFile(std::ofstream& file, const int amount, const int offset) {
    constexpr bool IS_AQUATIC = THE_VERSION == LCEVERSION::AQUATIC;

    enchants::EnchantController::setup(THE_CONSOLE, THE_VERSION);

    const std::string header = consoleToStr(THE_CONSOLE) + " " + LceVersionToString(THE_VERSION) + " ";

    loot::Buffer buffer;

    size_t count = 0;
    for (int i = offset; i < amount + offset; i++) {
        // if (i % 5000 == 0) std::cout << "At: " << i << "\n";

        u64 lootTableSeed = i;
        loot::Container<27> container;
        // loot::Container<9> container9;

        loot::abandoned_mineshaft.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "AbandonedMineshaft");

        loot::blacksmith.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "Blacksmith");

        loot::woodland_mansion.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "Mansion");

        loot::desert_temple.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "DesertTemple");

        loot::end_city_treasure.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "EndCityTreasure");

        loot::igloo.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "Igloo");

        loot::jungle_temple.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "JungleTemple");

        // loot::jungle_temple_dispenser.getLootFromLootTableSeed<Mode>(container9, lootTableSeed, &buffer);
        // updateData<debug>(file, lootTableSeed, count, container9, header, "JungleTempleDispenser");

        loot::nether_bridge.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "NetherBridge");

        loot::simple_dungeon.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "SimpleDungeon");

        loot::spawn_bonus_chest.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "SpawnBonusChest");

        loot::stronghold_crossing.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "StrongholdCrossing");

        loot::stronghold_library.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
        updateData<debug>(file, lootTableSeed, count, container, header, "StrongholdLibrary");

        if constexpr (IS_AQUATIC) {
            loot::stronghold_corridor_aquatic.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "StrongholdCorridor");
        } else {
            loot::stronghold_corridor_elytra.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "StrongholdCorridor");
        }

        if constexpr (IS_AQUATIC) {
            // loot::buried_treasure.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            // updateData<debug>(file, lootTableSeed, count, container, header, "BuriedTreasure");

            loot::underwater_ruin_big.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "UnderwaterRuinBig");

            loot::underwater_ruin_small.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "UnderwaterRuinSmall");

            loot::shipwreck_treasure.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "ShipwreckTreasure");

            loot::shipwreck_supply.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "ShipwreckSupply");

            loot::shipwreck_map.getLootFromLootTableSeed<Mode>(container, lootTableSeed, &buffer);
            updateData<debug>(file, lootTableSeed, count, container, header, "ShipwreckMap");
        }
    }

    return (int)count;
}

int unit_test() {

    enchants::EnchantController::setup(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC);

    // loot::Container<27> container;
    // constexpr auto tempMode = loot::GenerationMode::MOD_SHUF;
    // loot::stronghold_library.getLootFromLootTableSeed<tempMode>(container, 42);
    // std::cout << container;

    constexpr int aquaticCount = 10000;
    constexpr int elytraCount = 10000;
    constexpr bool DEBUG = true;
    constexpr auto MODE = loot::GenMode::MODERN;
    constexpr int totalCount = 1 * (aquaticCount + elytraCount);

    const std::string filename = "enchants_out" + std::to_string(totalCount) + ".txt";
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }
    // static constexpr std::size_t BUFFER_SIZE = 128 * 1024;
    // std::vector<char> buffer(BUFFER_SIZE);
    // file.rdbuf()->pubsetbuf(buffer.data(), &buffer.size());


    Timer start;

    int count = 0;
    int offset = 1;
    count += writeToFile<lce::CONSOLE::WIIU, LCEVERSION::ELYTRA, MODE, DEBUG>(file, elytraCount, offset);
    offset += elytraCount;
    // count += writeToFile<lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, MODE, DEBUG>(file, elytraCount, offset);
    // offset += elytraCount;
    count += writeToFile<lce::CONSOLE::WIIU, LCEVERSION::AQUATIC, MODE, DEBUG>(file, aquaticCount, offset);
    // offset += elytraCount;
    // count += writeToFile<lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, MODE, DEBUG>(file, aquaticCount, offset);

    const auto end = start.getSeconds();

    std::cout << "Time: " << end << "\n";

    std::cout << count << "\n";


    std::cout << "finished writing to " << filename << "\n";
    file.close();

    return 0;
}





int testMending() {
    enchants::EnchantController::setup(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC);
    lce::registry::ItemRegistry::setup();

    constexpr auto MODE = loot::GenMode::MODERN;

    loot::Container<27> loot;
    loot::stronghold_library.getLootFromLootTableSeed<MODE>(loot, 508808115, nullptr);
    std::cout << loot << "\n";

    return 0;
}



int main() {

    // return testMending();

    lce::registry::ItemRegistry::setup();
    enchants::EnchantController::setup(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC);
    unit_test();
    return 0;

    /*
    constexpr auto MODE = loot::GenerationMode::MOD_SHUF;
    loot::Container<27> container;
    loot::stronghold_library.getLootFromLootTableSeed<MODE>(container, 12, nullptr);
    std::cout << container;
    */
    /*
    Container<27> loot;
    int checksum = 0;

    loot.clear();
    blacksmith.getLootFromLootTableSeed<Mode>(loot, 6'524'555);
    std::cout << loot << "\n";
    checksum += loot.getSlot(0).myItemCount;

    loot.clear();
    blacksmith.getLootFromLootTableSeed<Mode>(loot, 6'524'556);
    std::cout << loot << "\n";
    checksum += loot.getSlot(0).myItemCount;

    return checksum;
    */
    /*
    // int off = 6'524'555;
    // int startttttt = off + 0;
    // int iterations = off + 2;
    Container<27> loot1;
    blacksmith.getLootFromLootTableSeed<Mode>(loot1, 6'524'556);
    std::cout << loot1 << "\n";
*/
    /*
    Container<27> test;
    EnchantData data(EnchantController::getRegistry()[0], 1, 0);
    test.push_back_enchant(data);
    lce::ItemState state = lce::items::DIAMOND.getState();
    state.myItemCount = 1;
    test.push_back_item(state);
    std::cout << test;
    return 0;
     */
    /*
    // 6'524'556
    u64 checksum = 0;
    Timer timer;
    loot::Container<27> loot;
    int iterations = 10'000'000;
    std::cout << "end_city_treasure\n";
    for (int i = 0; i < iterations; i++) {
        loot::end_city_treasure.getLootFromLootTableSeed<Mode>(loot, i);
        // std::cout << i << "\n" << loot << "\n";
        for (int n = 0; n < loot.getSlotCount(); n++) {
            checksum += loot.getSlot(n).myItemCount * loot.getSlot(n).getID();
        }

        loot.clear();
    }
    std::cout << "Time: " << timer.getSeconds() << "\n";
    std::cout << "Size: " << iterations << "\n";
    std::cout << "Checksum: " << checksum << "\n";
     */
    /*U
    Container<27> loot2;
    blacksmith.getLootFromLootTableSeed<Mode>(loot2, 6'524'556);
    std::cout << loot2 << "\n";

    */
    /*
    std::cout << container;
    return container.getSlot(0).myItemState.getID();
    RNG rng;
    rng.setRandomSeed();
    const auto lootResults = loot3.getLootFromLootTableSeed<Mode>(rng.getSeed());
    std::cout << lootResults << "\n";
    volatile int checksum = 0;
    for (int i = 0; i < lootResults.getSlotCount(); i++) {
        checksum += lootResults.getSlot(i).myItemState.getID();
    }
    checksum += lootResults.getSlot(1).myItemState.getId();
    std::cout << stack.myItemState.getId() << "\n";
    std::cout << flag;
    lootResults.printCombinedItems();
    for (auto item : lootResults) {
        std::cout << item << "\n";
    }
    std::cout << std::flush;
    return (int)reinterpret_cast<u64>(&func);
    */
}
