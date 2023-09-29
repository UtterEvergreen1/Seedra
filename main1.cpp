
#include <iostream>
#include <vector>
#include <fstream>
#include "LegacyCubiomes/structures/structure_generation/stronghold_generator/GenStronghold.hpp"
//#include "LegacyCubiomes/loot/base_classes/loot_classes.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"

//#include "LegacyCubiomes/loot/setup.hpp"


// #include "LegacyCubiomes/loot/buried_treasure.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
// #include "LegacyCubiomes/loot/stronghold_corridor.hpp"
//#include "LegacyCubiomes/structures/structure_placement/DynamicStructures.hpp"
//#include "LegacyCubiomes/chunk_generator/RavineGenerator.hpp"
//#include "LegacyCubiomes/chunk_generator/ChunkGenerator.hpp"
//#include "LegacyCubiomes/structures/structure_rolls/mineshaft_rolls.hpp"
//#include "LegacyCubiomes/structures/structure_placement/MineshaftStructure.hpp"
#include "LegacyCubiomes/structures/structure_placement/StrongholdStructure.hpp"
#include "LegacyCubiomes/loot/base_classes/loot.hpp"
#include "LegacyCubiomes/loot/stronghold_corridor.hpp"
//#include "LegacyCubiomes/chunk_generator/Chunk.hpp"



using stronghold_generator::StrongholdGenerator;
/*int findItemFromLootTableSeed(uint64_t* lootTableSeed, int itemID) {
    int rollCount;
    int rollIndex;
    std::vector<ItemStack> chestContents;
    setSeed(lootTableSeed, *lootTableSeed);
    int amount = 0;
    int total = 0;

    // generate loot
    for(const LootTable& table : loot_tables::StrongholdLibrary::lootTables){
        rollCount = LootTable::getInt<false>(lootTableSeed, table.min, table.max);
        for (rollIndex = 0; rollIndex < rollCount; rollIndex++) {
            ItemStack result = table.createLootRoll<false>(lootTableSeed);

            if (result.item->getID() == ENCHANTED_BOOK_ID) {
                EnchantmentHelper::EnchantWithLevelsBook::apply(lootTableSeed, &result, 30);
            }

            if (result.item->getID() == itemID) {
                amount = (int)result.enchantments.size();
                if (amount > total) {
                    total = amount;
                }
            }

            chestContents.push_back(result);
        }
    }
    return total;
}


void findSeedWithItem(int itemID, int total, int count) {
    uint64_t start = 216000000;
    uint64_t seed = 0;
    int amount;

    bool found;
    while (true) {
        amount = findItemFromLootTableSeed(&seed, itemID);
        found = amount >= total;
        if (found) {
            std::cout << start << ", ";
            count--;
            if (count <= 0) {
                break;
            }
        }
        start++;
        seed = start;
    }
}*/

int main(int argc, char* argv[]) {
    /*int64_t worldSeed = 1;
    EnchantmentHelper::setup(CONSOLE::WIIU, LCEVERSION::AQUATIC);
    Biome::registerBiomes();
    loot_tables::AbandonedMineshaft::setup();
    Generator g(LCEVERSION::AQUATIC, CONSOLE::WIIU, BIOMESCALE::SMALL, WORLDSIZE::CLASSIC);
    g.applyWorldSeed(worldSeed);
    MineshaftGenerator* mineshaftGenerator = new MineshaftGenerator();
    structure_rolls::MineshaftRolls* mineshaftRolls = new structure_rolls::MineshaftRolls();
    std::vector<Pos2D> mineshaftPoses = Structure::MineshaftStructure::getPositions(worldSeed, 0, 0, 27);
    for(const Pos2D& pos: mineshaftPoses) {
        std::cout << ((pos << 4) + 8) << std::endl;
        mineshaftGenerator->generate(worldSeed, pos);
        mineshaftRolls->generateAllChests(mineshaftGenerator, g, true);
        if(pos.x == 1) {
            for (int pieceIndex = 0; pieceIndex < mineshaftGenerator->piecesSize; ++pieceIndex) {
                const mineshaft_generator::Piece &piece = mineshaftGenerator->pieces[pieceIndex];
                std::cout << mineshaft_generator::PieceTypeName.find(piece.type)->second << ": " << piece.boundingBox << " type: "<< piece.additionalData << std::endl;
            }
        }
    }
    Container loot;
    for(std::pair<Pos3D, int64_t> chest : mineshaftRolls->mineshaftChests) {
        std::cout << std::endl << "Pos: " << chest.first << std::endl;
        loot = loot_tables::AbandonedMineshaft::getLootFromLootTableSeed<true>(reinterpret_cast<uint64_t *>(&chest.second));
        loot.printCombinedItems();
    }
    delete mineshaftGenerator;
    delete mineshaftRolls;
    return 0;*/

    Biome::registerBiomes();
    EnchantmentHelper::setup(CONSOLE::WIIU, LCEVERSION::AQUATIC);
    //int64_t worldSeed = 48739;//98238811 = gelly, 48739 = 12d
    Generator g(LCEVERSION::AQUATIC, CONSOLE::WIIU, BIOMESCALE::SMALL, WORLDSIZE::CLASSIC);
    loot_tables::StrongholdCorridor<true>::setup();
    StrongholdGenerator* stronghold_gen = new StrongholdGenerator();
    std::ifstream inputFile;
    std::string inputFileString = "small_wiiu.txt";
    inputFile.open(inputFileString);
    if(!inputFile.is_open()) {
        std::cout << "Could not open in file \"" << inputFileString << "\"" << std::endl;
        return 0;
    }

    std::ofstream outputFile;
    std::string outputFileString = "small_wiiu_out.txt";
    outputFile.open(outputFileString);
    if(!outputFile.is_open()) {
        std::cout << "Could not open out file \"" << outputFileString << "\"" << std::endl;
        return 0;
    }
    Container loot;
    bool hasSword;
    bool hasPearl;
    Pos2D spawnBlock;
    int64_t worldSeed;
    int portalXChunk;
    int portalZChunk;
    int portalBlockX;
    int portalBlockY;
    int portalBlockZ;

    int previousPortalBlockX = 0;
    int previousPortalBlockY = 0;
    int previousPortalBlockZ = 0;
    bool sisterSeedHasValidAltarChest = false;
    bool skipSisterSeeds = false;
    stronghold_generator::Piece *altarChest;

    bool isSisterSeed;
    while(inputFile >> worldSeed >> portalXChunk >> portalZChunk >> portalBlockX >> portalBlockY >> portalBlockZ) {
        isSisterSeed = portalBlockX == previousPortalBlockX && portalBlockY == previousPortalBlockY && portalBlockZ == previousPortalBlockZ;
        if(isSisterSeed && skipSisterSeeds) continue;
        g.applyWorldSeed(worldSeed);
        spawnBlock = g.getSpawnBlock();
        if(abs(spawnBlock.x - portalBlockX) > 50 || abs(spawnBlock.z - portalBlockZ) > 50) continue;
        if(!isSisterSeed) {
            sisterSeedHasValidAltarChest = false;
            skipSisterSeeds = false;
            previousPortalBlockX = portalBlockX;
            previousPortalBlockY = portalBlockY;
            previousPortalBlockZ = portalBlockZ;
            stronghold_gen->generate(worldSeed, Structure::StrongholdStructure::getWorldPosition(g) >> 4);
        }
        for(int i = 0; i < stronghold_gen->numAltarChests && !sisterSeedHasValidAltarChest; ++i) {
            altarChest = stronghold_gen->altarChests[i];
            if(abs(altarChest->getWorldX(3, 3) - portalBlockX) > 50 ||
            altarChest->getWorldY(2) < portalBlockY - 3 ||
            abs(altarChest->getWorldZ(3, 3) - portalBlockZ) > 50) continue;
            loot = loot_tables::StrongholdCorridor<true>::getAltarChestLoot<true, false>(g, altarChest, stronghold_gen);
            hasPearl = false;
            hasSword = false;
            for(const ItemStack& item : loot.inventorySlots) {
                if (item.stackSize == 0) break; // end of items
                if (item.item->getID() == Items::ENDER_PEARL_ID) {
                    hasPearl = true;
                }
                else if (item.item->getID() == Items::IRON_SWORD_ID) {
                    hasSword = true;
                }
            }
            if(!(hasPearl && hasSword)) continue;
            sisterSeedHasValidAltarChest = true;
        }
        if(sisterSeedHasValidAltarChest) {
            outputFile << std::endl << worldSeed << " -> (" <<
                       portalBlockX << ", " <<
                       portalBlockY << ", " <<
                       portalBlockZ << ") (" <<
                       altarChest->getWorldX(3, 3) << ", " <<
                       altarChest->getWorldY(2) << ", " <<
                       altarChest->getWorldZ(3, 3) << ") " <<
                       spawnBlock << loot << std::endl;
        }
        else {
            skipSisterSeeds = true;
        }
    }
    outputFile.close();
    delete stronghold_gen;
    return 0;
   /* stronghold_gen->generate(worldSeed, Structure::StrongholdStructure::getWorldPosition(g) >> 4);
    for(int i = 0; i < stronghold_gen->numAltarChests; ++i) {
        stronghold_generator::Piece *altarChest = stronghold_gen->altarChests[i];
        Pos3D altarChestPos(altarChest->getWorldX(3, 3),
                            altarChest->getWorldY(2),
                            altarChest->getWorldZ(3, 3));
        std::cout << std::endl << altarChestPos << loot_tables::StrongholdCorridor<true>::getAltarChestLoot<true, true>(g, altarChest, stronghold_gen) << std::endl;
    }*/
    //ChunkPrimer* chunk = Chunk::provideChunk<true, true, false>(g, 6, 14);
    //std::cout << *chunk << std::endl;
    //delete chunk;

    /*Pos2D netherFortressPos;
    const int ROLLS = INT32_MAX;
    uint64_t start = getMilliseconds();
    for(int i = INT32_MIN; i < ROLLS; i++){
        netherFortressPos = Structure::NetherFortressStructure::getWorldPosition(i);
        if(netherFortressPos == 7) {
            std::cout << "Seed " << i << " has fortress at chunk (7, 7)" << std::endl;
            break;
        }
    }
    uint64_t end = getMilliseconds();

    uint64_t diff = end - start;
    std::cout << "rolls: " << ROLLS << " | time: " << diff << "ms" << std::endl;*/

    /*EnchantmentHelper::setup(CONSOLE::XBOX, LCEVERSION::ADVENTURE);

    loot_tables::DesertTemple::setup();
    std::vector<uint64_t> lootTablesSeedsToTest = {
            -8395339864670790781ULL,
            6262159008576967325ULL,
            -5945485444657331499ULL,
    };
    Container loot;
    for(uint64_t& lootTableSeed : lootTablesSeedsToTest) {
        loot = loot_tables::DesertTemple::getLootFromLootTableSeed<true>(&lootTableSeed);
        std::cout << std::endl << "Loot:" << loot << std::endl << std::endl;
    }
    return 0;*/
    /*loot_tables::StrongholdLibrary::setup();

    uint64_t lootTableSeed = 216765366;
    const int ROLLS = 1000000;
    Container loot;
    auto start = getMilliseconds();
    for (int i = 0; i < ROLLS; i++) {
        setSeed(&lootTableSeed, lootTableSeed);
        loot = loot_tables::StrongholdLibrary::getLootFromLootTableSeed<false>(&lootTableSeed);
    }

    auto end = getMilliseconds();

    auto diff = end - start;
    std::cout << "rolls: " << ROLLS <<
              " | time: " << diff << "ms" << std::endl;*/


    /*
    // find specific item in loot table
    findSeedWithItem(Items::ENCHANTED_BOOK_ID, 10, 1); int x; std::cin >> x;


    uint64_t lootTableSeed = 216765366;

    Container loot = loot_tables::StrongholdLibrary::getLootFromLootTableSeed<true>(&lootTableSeed);

    // print details
    std::cout << std::endl << "Loot:" << loot << std::endl << std::endl;
    std::cout << "Combined Items:" << std::endl;
    loot.printCombinedItems();
    */



    /*
    uint64_t lootTableSeed = 0;
    uint64_t start = 12358;
    bool found;
    while (true) {
        found = false;
        lootTableSeed = start;
        start++;

        Container loot = loot_tables::StrongholdCorridor<isAquatic>
        ::getLootFromLootTableSeed<true>(&lootTableSeed);

        std::map<const Items::Item*, int> itemCount;

        for (const auto& itemStack : loot.inventorySlots) {
            if (itemStack.stackSize > 0) {
                if (itemCount.find(itemStack.item) != itemCount.end()) {
                    itemCount[itemStack.item] += itemStack.stackSize;
                } else {
                    itemCount[itemStack.item] = itemStack.stackSize;
                }
            }
        }

        for (const auto& pair : itemCount) {
            ItemStack itemStack = ItemStack(pair.first, pair.second);
            if (itemStack.item->getID() == ENCHANTED_BOOK_ID) {
                std::cout << lootTableSeed << std::endl;
                found = true;
                break;
            }
        }
        if (found)
            break;
    }
        // std::cout << std::endl << "Loot:" << loot << std::endl << std::endl;

        // std::cout << "Combined Items:" << std::endl;
        // loot.printCombinedItems();

        // std::cout << "\n\n\n" << std::endl;
    //}
*/



    /*
    // Biome::registerBiomes();
    // Generator g = Generator(LCEVERSION::WIIU_LATEST, BIOMESCALE::SMALL);
    // g.applySeed(DIMENSIONS::OVERWORLD, 12349);

    // std::cout << Pos2D(1, 148).toChunkPos() << std::endl;
     -7254631889086558805 / (12348)(wiiu) two enchanted books
     -5989332256310069151 / (12348)(ps3)

     -3093593048950619536 / (12350)(ps3) (1, 28, 148)
     2540137978986425799  / (12350)(ps3) (10, 33, 152)

     -7139668156223608697 / books and paper / correct
     -4210146869381317490 / https://media.discordapp.net/attachments/753070027397398610/1149803061506752614/image.png


    // loot_tables::StrongholdCorridor<false>::setup();
    // uint64_t lootTableSeed = 1068107887565861472;
    // Container loot = loot_tables::StrongholdCorridor<false>::getLootFromLootTableSeed<false>(lootTableSeed);
    // std::cout << std::endl << "Loot:" << loot << std::endl;




    const int ROLLS = 1000000;
    auto start = getMilliseconds();
    for (int i = 0; i < ROLLS; i++) {
        setSeed(&lootTableSeed, lootTableSeed);
        loot = getLootFromLootTableSeed<false>(&lootTableSeed);
    }

    auto end = getMilliseconds();

    auto diff = end - start;
    std::cout << "rolls: " << ROLLS << " | time: " << diff << "ms" << std::endl;
    */

    // wiiu
    // loot_tables::StrongholdCorridor<false>::setup();
    // loot_tables::StrongholdLibrary::setup();
    // uint64_t  lootTableSeed = -5989332256310069151;
    // Container loot = loot_tables::StrongholdLibrary
    //         ::getLootFromLootTableSeed<false>(&lootTableSeed);
    // std::cout << std::endl << "Loot:" << loot << std::endl;

    // ps3
    // loot_tables::StrongholdLibrary::setup();
    // uint64_t  lootTableSeed = 3076128080566098038;
    // Container loot = loot_tables::StrongholdLibrary
    // ::getLootFromLootTableSeed<true>(&lootTableSeed);
    // std::cout << std::endl << "Loot:" << loot << std::endl;

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
            switch(itemStack.itemEntry.getID()){
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

