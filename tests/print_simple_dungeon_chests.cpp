#include "lce/processor.hpp"
#include "lce/items/itemsInit.hpp"
#include "lce/registry/itemRegistry.hpp"

#include "loot/classes/Loot.hpp"
#include "loot/classes/LootFunctions.hpp"
#include "loot/classes/LootItem.hpp"
#include "loot/classes/LootTable.hpp"
#include "loot/Tables.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

struct ChestSeed {
    const char* dimension;
    int x;
    int y;
    int z;
    const char* lootTable;
    std::int64_t lootTableSeed;
};

static const std::vector<ChestSeed> CHESTS = {
        {"OVERWORLD", -415, 18, 140, "minecraft:chests/simple_dungeon", -787202823772998876LL},
        {"OVERWORLD", -412, 18, 142, "minecraft:chests/simple_dungeon", 5383723035258829234LL},
        {"OVERWORLD", -412, 27, -346, "minecraft:chests/simple_dungeon", 8734801992612811792LL},
        {"OVERWORLD", -408, 27, -343, "minecraft:chests/simple_dungeon", 4403471773590370749LL},
        {"OVERWORLD", -395, 100, 47, "minecraft:chests/simple_dungeon", 9114689459773317502LL},
        {"OVERWORLD", -390, 25, -379, "minecraft:chests/simple_dungeon", -4386455330954364660LL},
        {"OVERWORLD", -390, 25, -377, "minecraft:chests/simple_dungeon", 7707967950252240437LL},
        {"OVERWORLD", -338, 28, 123, "minecraft:chests/simple_dungeon", 6738119981541006885LL},
        {"OVERWORLD", -337, 28, 126, "minecraft:chests/simple_dungeon", -4174223181688814344LL},
        {"OVERWORLD", -333, 36, -169, "minecraft:chests/simple_dungeon", -19670392725111016LL},
        {"OVERWORLD", -326, 29, 120, "minecraft:chests/simple_dungeon", 6745267548890968730LL},
        {"OVERWORLD", -324, 29, 119, "minecraft:chests/simple_dungeon", 1596562307079261781LL},
        {"OVERWORLD", -310, 19, 192, "minecraft:chests/simple_dungeon", -8642333534006342875LL},
        {"OVERWORLD", -308, 19, 193, "minecraft:chests/simple_dungeon", 8073179674316482373LL},
        {"OVERWORLD", -206, 19, -168, "minecraft:chests/simple_dungeon", 81572943361382271LL},
        {"OVERWORLD", -137, 44, 42, "minecraft:chests/simple_dungeon", 2939523107852154726LL},
        {"OVERWORLD", -88, 30, 251, "minecraft:chests/simple_dungeon", 11874676839349112LL},
        {"OVERWORLD", -84, 30, 251, "minecraft:chests/simple_dungeon", -6383553660987430507LL},
        {"OVERWORLD", -81, 54, -18, "minecraft:chests/simple_dungeon", 9115560321864431359LL},
        {"OVERWORLD", 56, 28, 366, "minecraft:chests/simple_dungeon", 5324827931323087059LL},
        {"OVERWORLD", 148, 14, 331, "minecraft:chests/simple_dungeon", -8841180502048637250LL},
        {"OVERWORLD", 150, 14, 331, "minecraft:chests/simple_dungeon", -8775424921481197684LL},
        {"OVERWORLD", 164, 18, -23, "minecraft:chests/simple_dungeon", 2046127426335264181LL},
        {"OVERWORLD", 166, 18, -21, "minecraft:chests/simple_dungeon", 5565817828398283213LL},
        {"OVERWORLD", 203, 24, -104, "minecraft:chests/simple_dungeon", -4183160484505220727LL},
        {"OVERWORLD", 207, 24, -105, "minecraft:chests/simple_dungeon", 2226450788634168179LL},
        {"OVERWORLD", 239, 12, 117, "minecraft:chests/simple_dungeon", -6583337324687314397LL},
        {"OVERWORLD", 240, 12, 114, "minecraft:chests/simple_dungeon", 7959761525235854759LL},
        {"OVERWORLD", 284, 16, 155, "minecraft:chests/simple_dungeon", 3684013275954434536LL},
};

int main() {

    enchants::EnchantController::setup(
            lce::CONSOLE::PS3,
            LCEVERSION::AQUATIC
    );

    constexpr auto MODE = loot::GenMode::MODERN;

    loot::Buffer buffer;
    loot::Container<27> container;

    std::cout << "Console: PS3\n";
    std::cout << "Version: AQUATIC\n";
    std::cout << "Loot table: SimpleDungeon\n";
    std::cout << "Chest count: " << CHESTS.size() << "\n\n";

    for (std::size_t i = 0; i < CHESTS.size(); ++i) {
        const ChestSeed& chest = CHESTS[i];

        container.clear();

        const u64 seedBits = static_cast<u64>(chest.lootTableSeed);

        loot::simple_dungeon.getLootFromLootTableSeed<MODE>(
                container,
                seedBits,
                &buffer
        );

        std::cout << "============================================================\n";
        std::cout << "Chest #" << (i + 1) << "\n";
        std::cout << "Dimension: " << chest.dimension << "\n";
        std::cout << "Position: [" << chest.x << ", " << chest.y << ", " << chest.z << "]\n";
        std::cout << "LootTable: " << chest.lootTable << "\n";
        std::cout << "LootTableSeed signed: " << chest.lootTableSeed << "\n";
        std::cout << "LootTableSeed bits: " << seedBits << "\n";
        std::cout << "Generated slot count: " << container.slotCount() << "\n";
        std::cout << "Contents:\n";
        std::cout << container << "\n";
    }

    return 0;
}