
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/structures/structure_generation/stronghold_generator/StrongholdGenerator.hpp"
#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"
// #include "LegacyCubiomes/loot/buried_treasure.hpp"

// #include "LegacyCubiomes/loot/stronghold_corridor.hpp"

#include "LegacyCubiomes/structures/structure_placement/DynamicStructures.hpp"
#include "LegacyCubiomes/chunk_generator/RavineGenerator.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkGenerator.hpp"

#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"


#include <iostream>
#include <vector>

using stronghold_generator::StrongholdGenerator;


/*
template <bool shuffle>
Container getLootFromLootTableSeed(uint64_t* lootTableSeed) {
    int rollCount;
    int rollIndex;
    std::vector<ItemStack> chestContents;
    setSeed(lootTableSeed, *lootTableSeed);

    // generate loot
    for(const LootTable& table : loot_tables::SpawnBonusChest::lootTables){
        rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
        for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
            ItemStack result = table.createLootRoll<false>(lootTableSeed);

            if EXPECT_FALSE(result.item == &Items::ENCHANTED_BOOK) {
                EnchantmentHelper::EnchantWithLevels::apply(lootTableSeed, &result, 30, true, true);
            }

            chestContents.push_back(result);
        }
    }
    if constexpr (shuffle){
        Container container = Container(27);
        container.shuffleIntoContainer(chestContents, *lootTableSeed);
        return container;
    }
    else
        return  {27, chestContents};
}
*/

#include "LegacyCubiomes/loot/spawn_bonus_chest.hpp"
template <bool shuffle>
Container getLootFromLootTableSeed(uint64_t lootTableSeed) {
    int rollCount;
    int rollIndex;
    std::vector<ItemStack> chestContents;
    setSeed(&lootTableSeed, lootTableSeed);

    // generate loot
    for(const LootTable& table : loot_tables::SpawnBonusChest::lootTables){
        rollCount = LootTable::getInt<false>(&lootTableSeed, table.min, table.max);
        for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
            ItemStack result = table.createLootRoll<false>(&lootTableSeed);

            if EXPECT_FALSE(result.item == &Items::ACACIA_WOOD) {
                nextInt(&lootTableSeed, 0, 1);
            }
            else if EXPECT_FALSE(result.item == &Items::OAK_WOOD) {
                nextInt(&lootTableSeed, 0, 3);
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



#include "LegacyCubiomes/enchants/enchantment.hpp"


int main(int argc, char* argv[]) {
    // Biome::registerBiomes();
    // Enchantment::registerEnchantments();

    // Generator g = Generator(LCEVERSION::WIIU_LATEST, BIOMESCALE::SMALL);
    // g.applySeed(DIMENSIONS::OVERWORLD, 12349);

    loot_tables::SpawnBonusChest::setup();


    // std::cout << Pos2D(-108, 254).toChunkPos() << std::endl;
    /*
     -7254631889086558805 / (12348)(wiiu) two enchanted books
     -5989332256310069151 / (12348)(ps3)

     -3093593048950619536 / (12350)(ps3) (1, 28, 148)
     2540137978986425799  / (12350)(ps3) (10, 33, 152)

     -7139668156223608697 / books and paper / correct
     -4210146869381317490 / https://media.discordapp.net/attachments/753070027397398610/1149803061506752614/image.png
     */
    uint64_t lootTableSeed = -3532906795931795829;
    Container loot = getLootFromLootTableSeed<true>(lootTableSeed);

    /*
    std::cout << "Enchant List" << std::endl;
    int count;
    for (auto ench = Enchantment::REGISTRY.rbegin(); ench != Enchantment::REGISTRY.rend(); ++ench) {
        count += ench->second->rarity->getWeight();
        std::cout << count << " " << ench->second->name << std::endl;
    }
     */

    // std::cout << chunkPos.toBlockPos() << std::endl;
    // Pos2D chest = Pos2D(16, 232).toChunkPos();
    // std::cout << chest << std::endl;

    std::cout << std::endl << "Loot:" << loot << std::endl;

    /*
     1.
     (id 1, lvl 3) Fire Protection III

     2.
     (id 35, lvl 2) Sharpness III
     (id 16, lvl 3) Efficiency IV
     */
}

// Pos2D chunkPos = g.getStronghold();
// auto stronghold = StrongholdGenerator();
// stronghold.generate(g.seed, chunkPos);

// std::cout << Pos2D(21, 199).toChunkPos() << std::endl;
// std::cout << chunkPos.toBlockPos() << std::endl;
/* for (int i = 0; i < 4; i++) {
    auto alterChestPiece = stronghold.altarChests[i];
    std::cout << alterChestPiece->getWorldX(3, 3) << " "
              << alterChestPiece->getWorldY(2) << " "
              << alterChestPiece->getWorldZ(3, 3) << std::endl;

} */
/*
                // this is called by EnchantWithLevels.apply()
                int levels = nextInt(lootTableSeed, 30);

                // this is called by EnchantmentHelper::buildEnchantmentList() part 1
                // this computes the enchantment level
                nextInt(lootTableSeed);
                nextInt(lootTableSeed);
                nextFloat(lootTableSeed);
                nextFloat(lootTableSeed);

                // this is called by EnchantmentHelper::buildEnchantmentList() part 1
                // grabs a random enchantment out of the list
                nextInt(lootTableSeed);

                std::cout << levels << std::endl;
                */
/*
int main(int argc, char* argv[]) {
    Biome::registerBiomes();
    StrongholdLibrary::setup();

    Generator g = Generator(LCEVERSION::WIIU_LATEST, BIOMESCALE::SMALL);
    g.applySeed(DIMENSIONS::OVERWORLD, 12346);

    // std::cout << Pos2D(-145, 171).toChunkPos() << std::endl;
    // uint64_t lootTableSeed = -4210146869381317490;

    // Pos3D chest = {-153, 30, 167};
    uint64_t lootTableSeed = -7139668156223608697;

    // Container loot = getLootLegacyFromSeed(&lootTableSeed);
    Container loot = getLootFromSeed<false>(&lootTableSeed);





    std::map<const Item*, int> itemCount;

    for (auto itemStack : loot.inventorySlots) {
        if (itemStack.stackSize > 0) {
            // Check if the item is already in the map
            if (itemCount.find(itemStack.item) != itemCount.end()) {
                // Add the stackSize to the existing count
                itemCount[itemStack.item] += itemStack.stackSize;
            } else {
                // Insert a new entry into the map
                itemCount[itemStack.item] = itemStack.stackSize;
            }
        }
    }

    std::cout << "\ncounts:" << std::endl;
    for (const auto& pair : itemCount) {
        const Item* item = pair.first;
        int count = pair.second;
        ItemStack itemStack = ItemStack(item, count);

        std::cout << itemStack << std::endl;
    }






}
 */
/*
 * Pos2D chunkPos = g.getStronghold();
    auto stronghold = StrongholdGenerator();
    stronghold.generate(g.seed, chunkPos);
 */
/*
std::vector<RavineData> ravineInChunk(const Generator &g, ChunkPrimer* chunkPrimer, Pos2D chunk) {
    RavineGenerator gen = RavineGenerator(g.seed, g.biomeSize);
    std::vector<RavineData> ravines = gen.getTunnelsFromChunk(g.seed, chunkPrimer, chunk.x, chunk.z);
    return ravines;
}*/
/*
    loot_tables::StrongholdCorridor<false>::setup();
    std::vector<Container> containers =
            loot_tables::StrongholdCorridor<false>::
                    getAllAlterChestLoot<false, false>(g.seed, g.biomeSize, &stronghold);
    */
/*
int main(int argc, char* argv[]) {
    Generator g = Generator(LCEVERSION::WIIU_LATEST, BIOMESCALE::SMALL);
    g.applySeed(DIMENSIONS::OVERWORLD, -6883608035857155058);

    Pos2D chunkPos = Pos2D(-219, 215).toChunkPos();

    Biome::registerBiomes();

    ChunkGeneratorOverWorld chunk = ChunkGeneratorOverWorld(g.seed, g.biomeSize);
    ChunkPrimer* chunkPrimer = chunk.provideChunk(chunkPos.x, chunkPos.z);

    RavineGenerator gen = RavineGenerator(g.seed, g.biomeSize);
    std::vector<RavineData> ravines = gen.getTunnelsFromChunk(
            g.seed, chunkPrimer, chunkPos.x + 1, chunkPos.z + 1);

    for (RavineData ravine : ravines) {
        std::cout
        << " start: " << ravine.tunnelStart
        << " minH: " << ravine.minHeight
        << " maxH: " << ravine.maxHeight
        << " dir: " << ravine.tunnelDirection
        << " slope: " << ravine.tunnelSlope
        << " length_mult: " << ravine.tunnelLengthMultiplier
        << std::endl;
    }
    // ravineGenerator.generate(chunkX, chunkZ, chunkPrimer);
}
*/
/*
int main(int argc, char* argv[]) {
    Generator g = Generator(LCEVERSION::WIIU_LATEST, BIOMESCALE::SMALL);
    g.applySeed(DIMENSIONS::OVERWORLD, 2220913686226291287);

    loot_tables::BuriedTreasure::setup();
    Structure::Treasure::setup(WORLDSIZE::CLASSIC);

    for (int x = -1; x < 2; x++) {
        for (int z = -1; z < 2; z++) {
            Pos2D pos = Structure::Treasure::getPosition(g, x, z);
            Container loot = loot_tables::BuriedTreasure::getLoot<true>(2220913686226291287, pos.x >> 4, pos.z >> 4);
            std::cout << x << " " << z  << " " << pos << std::endl;
        }
    }

}*/
/*
int main(int argc, char* argv[]) {

    // Container loot;
    // loot = loot_tables::BuriedTreasure::getLoot<true>(839683, -11, -24);
    // std::cout << "Chest:" << loot << std::endl;
    // std::cout << "Amount: " << amount << std::endl;

    loot_tables::BuriedTreasure::setup();

    int64_t rollCount = power2_32;

    // auto start = getNanoSeconds();

    int amount;
    for (int64_t i = 2624658160; i < rollCount; i++) {
        amount = loot_tables::BuriedTreasure::checkFor5Cakes(i);
        if EXPECT_FALSE(amount > 3) {
            std::cout << "Seed: " << i << " Amount: " << amount << std::endl;
        }
    }

    std::cout << "Checked all 2^32 seeds..." << std::endl;
    std::cout << "enter a number to continue." << std::endl;
    int x;
    std::cin >> x;
    // auto end = getNanoSeconds();
    // auto diff = end - start;

    // double total_per_second = 10000 * (1000000000 / (double)diff);
    // double time_to_check_all = 4294967296 / total_per_second;

    // std::cout << "Time for " << rollCount << " rolls: " << diff << " nanoseconds." << std::endl;
    // std::cout << "Rolls per second: " << total_per_second << std::endl;
    // std::cout << "Time for checking all 2^32 seeds: " << time_to_check_all << std::endl;

    return 0;
}
*/
/*
int main(int argc, char* argv[]) {
    Generator g = Generator(LCEVERSION::WIIU_LATEST, BIOMESCALE::SMALL);
    g.applySeed(1405922471972256859);
    stronghold_generator::StrongholdGenerator strongholdGen = stronghold_generator::StrongholdGenerator();

    Pos2D strongholdChunk = g.getStronghold();
    Pos2D strongholdPos = strongholdChunk.toBlockPos();
    strongholdGen.generate(g.seed, strongholdChunk);

    int numIron;
    bool foundIron = false;
    bool foundEnderPearls = false;
    int numPearls;
    std::vector<Container> alterChestsLoot = loot_tables::StrongholdCorridor<true>::getAllAlterChestLoot<true, false>(g.seed, g.biomeSize, &strongholdGen);
    for(int alterChestIndex = 0; alterChestIndex < alterChestsLoot.size() && (!foundEnderPearls || !foundIron); alterChestIndex++){
        numPearls = 0;
        numIron = 0;
        const Container& container = alterChestsLoot[alterChestIndex];
        for(int itemIndex = 0; itemIndex < container.inventorySlots.size() && (!foundEnderPearls || !foundIron); itemIndex++){
            const ItemStack& itemStack = container.inventorySlots[itemIndex];
            switch(itemStack.itemEntry.id){
                case Items::ItemID::ENDER_PEARL_ID:
                    numPearls++;
                    if(numPearls == 2){
                        foundEnderPearls = true;
                    }
                    break;
                case Items::ItemID::IRON_INGOT_ID:
                    numIron++;
                    if(numIron == 2){
                        foundIron = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if(!foundEnderPearls || !foundIron)
        return 1;
    for (int containerIndex = 0; containerIndex < alterChestsLoot.size(); containerIndex++) {
        const Container& container = alterChestsLoot[containerIndex];
        std::cout << "CHEST" << containerIndex << container << "\n";
    }
    return 0;



}
*/




















