#include "common/timer.hpp"
#include "common/worldPicture.hpp"
#include "loot/Tables.hpp"
#include "structures/placement/DynamicStructures.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/generator.hpp"

#include <filesystem>

namespace fs = std::filesystem;


template<typename T>
void fileWrite(std::ofstream& file, T obj) {
    file.write(reinterpret_cast<const char*>(&obj), sizeof(T));
}


int main() {
    const char* userProfile = std::getenv("USERPROFILE");
    if (!userProfile) {
        std::cerr << "USERPROFILE environment variable not set!\n";
        return 1;
    }
    fs::path dirPath = fs::path(userProfile)
                       / "CLionProjects"
                       / "LegacyChunkViewer";
    std::string dirName = dirPath.string();
    std::cout << "Using \"" + dirName + "\"...\n\n";
    fs::path filePath = dirPath / "build" / "chunks" / "chunkdata.bin";


    // auto* primer = new ChunkPrimer;
    // primer->setBlock(0, 0, 0, &lce::BlocksInit::SPRUCE_WOOD_STAIRS);
    // bool canSnowAt = primer->canSnowAt({0, 1, 0});
    // std::cout << canSnowAt << "\n";



    // -1204924307554348042;
    Biome::registerBiomes();
    c_i64 WORLD_SEED = -6651998285536156346LL; // 615831156172857837; // 7710511010199114;
    c_auto CONSOLE = lce::CONSOLE::WIIU;
    c_auto VERSION = LCEVERSION::ELYTRA;
    c_auto WORLD_SIZE = lce::WORLDSIZE::CLASSIC;
    c_auto BIOME_SCALE = lce::BIOMESCALE::SMALL;
    // -6651998285536156346
    Generator g(CONSOLE, VERSION, WORLD_SEED, WORLD_SIZE, BIOME_SCALE);

    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int X_WIDTH  = 54; // 10;
    int Z_WIDTH  = 54; // 10;
    int X_CENTER = 0; // 9;
    int Z_CENTER = 0; // 210 / 16;

    Timer start;
    Timer gen;

    auto world = World(&g);

    world.generateMineshafts();
    world.generateVillages();
    world.generateStrongholds();

    world.getOrCreateChunk({X_CENTER, Z_CENTER});

    world.createChunks({X_CENTER, Z_CENTER}, X_WIDTH);
    std::cout << "Chunk Gen Time: " << gen.getSeconds() << "\n";
    gen.reset();
    world.decorateCaves({X_CENTER, Z_CENTER}, X_WIDTH, VERSION == LCEVERSION::AQUATIC);
    std::cout << "Cave Gen Time: " << gen.getSeconds() << "\n";
    gen.reset();

    world.decorateChunks({X_CENTER, Z_CENTER}, X_WIDTH);

    std::cout << "Decorate Gen Time: " << gen.getSeconds() << "\n";
    std::cout << "Total world Gen Time: " << start.getSeconds() << "\n";


    std::ofstream file(filePath.string(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filePath << std::endl;
        return -1;
    }

    int64_t worldSeed = g.getWorldSeed();

    fileWrite<i64>(file, worldSeed);
    fileWrite<int>(file, X_CENTER);
    fileWrite<int>(file, Z_CENTER);
    fileWrite<int>(file, X_WIDTH);
    fileWrite<int>(file, Z_WIDTH);

    int sx = (X_CENTER - X_WIDTH) * 16;
    int sz = (Z_CENTER - Z_WIDTH) * 16;
    int sw = X_WIDTH * 2 * 16;
    int sh = Z_WIDTH * 2 * 16;

    std::cout << "Generating biomes at "
              << sx << ", "
              << sz << ", "
              << sx + sw <<  ", "
              << sz + sh << std::endl;

    int* biomes = world.getGenerator()
        ->getBiomeRange(1, sx, sz, sw, sh);

    if (biomes == nullptr) {
        std::cerr << "Error loading biome map, exiting..." << std::endl;
        return -1;
    } else {
        std::cout << "Loaded Biome Map, writing chunks to file" << std::endl;
    }

    for (int cx = -(X_WIDTH) + X_CENTER; cx < X_WIDTH + X_CENTER - 1; cx++) {
        for (int cz = -(Z_WIDTH) + Z_CENTER; cz < Z_WIDTH + Z_CENTER - 1; cz++) {
            Pos2D chunkPos(cx, cz);
            auto* chunkPrimer = world.getChunk({cx, cz});

            if (chunkPrimer == nullptr) {
                std::cerr << "Error getting chunk: " << chunkPos << std::endl;
                continue;
            }

            u8 biomeChunk[256];

            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    int bx = (chunkPos.x + X_WIDTH) * 16 + x;
                    int bz = (chunkPos.z + Z_WIDTH) * 16 + z;
                    biomeChunk[z * 16 + x] = (u8)biomes[bz * sw + bx];
                }
            }


            chunkPrimer->highestYBlock = -1;
            chunkPrimer->highestYBlock = std::min(chunkPrimer->getHighestYBlock() + 1, 255);
            i64 sizeToWrite = (i64) sizeof(chunkPrimer->blocks[0]) * 256 * chunkPrimer->highestYBlock;

            fileWrite<int>(file, chunkPos.x);
            fileWrite<int>(file, chunkPos.z);
            fileWrite<int>(file, chunkPrimer->highestYBlock);
            file.write(reinterpret_cast<const char*>(&biomeChunk[0]), 256);
            file.write(reinterpret_cast<const char*>(&chunkPrimer->blocks[0]), sizeToWrite);


            if (!file) { std::cerr << "Error writing to file: " << filePath << std::endl; }
        }
    }

    delete biomes;

    return 0;
}


/*
int main() {
    // Initialize the generator
    Biome::registerBiomes();
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // Step 1: Generate a chunk
    Pos2D chunkPos(0, 0);
    ChunkPrimer* chunk = Chunk::provideNewChunk(generator, chunkPos);
    std::cout << "Generated chunk at position: (" << chunkPos.x << ", " << chunkPos.z << ")" << std::endl;

    // Step 2: Save the world's biomes to a PNG
    WorldPicture worldPicture(&generator);
    worldPicture.drawBiomes();
    worldPicture.save("output_directory/");
    std::cout << "Saved biomes to PNG in 'output_directory/'" << std::endl;

    // Step 3: Find all buried treasure positions
    auto buriedTreasurePositions = Placement::BuriedTreasure::getAllPositions(&generator);
    std::cout << "Found " << buriedTreasurePositions.size() << " buried treasures in the world." << std::endl;

    // Step 4: Retrieve loot for each buried treasure
    constexpr auto Mode = loot::GenMode::MODERN;
    loot::Container<27> container;

    for (const auto& pos : buriedTreasurePositions) {
        loot::buried_treasure.getLootFromChunk<Mode>(container, generator.getWorldSeed(), pos.x, pos.z);
        std::cout << "Loot for buried treasure at (" << pos.x << ", " << pos.z << "):" << std::endl;
        std::cout << container << std::endl;
    }

    // Clean up
    delete chunk;
    return 0;
}*/


    /*
int main() {
    // Initialize the generator
    Biome::registerBiomes();
    lce::registry::ItemRegistry::setup();

    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::MEDIUM);
    generator.generateCache(4);

    int numRight = 0;
    RNG rng = RNG::initializeWithRandomSeed();
    //Timer timer;
    constexpr int maxSeed = 1000000;
    constexpr int seedInterval = 1000;
    for (int64_t counter = 1; counter <= maxSeed; ++counter) {
        /*if EXPECT_FALSE (seed % seedInterval == 0) {
            std::cout << "Seed: " << seed << std::endl;
            std::cout << "Seeds per second: " << seedInterval / timer.getSeconds() << std::endl;
            timer.reset();
        }*/
        int64_t seed = rng.nextLongI();
        generator.applyWorldSeed(seed);

        auto buriedTreasurePositions = Placement::BuriedTreasure::getAllPositions(&generator);
        if (buriedTreasurePositions.size() > 0) {
            continue;
        }

        ++numRight;

        //std::cout << "Found " << buriedTreasurePositions.size() << " buried treasures in the world. Seed: " << seed << std::endl;

        // Step 4: Retrieve loot for each buried treasure
        /*constexpr auto Mode = loot::GenMode::MODERN;
        loot::Container<27> container;

        for (const auto& pos : buriedTreasurePositions) {
            loot::buried_treasure.getLootFromBlock<Mode>(container, seed, pos.x, pos.z);
            std::cout << "Loot for buried treasure at (" << pos.x << ", " << pos.z << "):" << std::endl;
            std::cout << container << std::endl;
            container.clear();
        }*/
    }

    std::cout << "Total seeds with no buried treasure: " << numRight << std::endl;
    std::cout << "Percentage of seeds with no buried treasure: " << (static_cast<double>(numRight) / maxSeed) * 100 << "%" << std::endl;

    // Clean up
    return 0;
}
*/


/*int main() {
    Pos2DVec_t positions = {
    {-1, 7},
        {-11, -24},
        {12, 33},
        {30, -11},
        {-34, 18},
        {7, 18}
        };
    RNG rng;

    int amountRight = 0;
    for (uint64_t worldSeed = 22873568274; worldSeed < 281474976710656; ++worldSeed) {
        amountRight = 0;
        rng.setSeed(worldSeed);
        c_u64 xModifier = rng.nextLong();
        c_u64 zModifier = rng.nextLong();

        for (const Pos2D& pos : positions) {
            c_u64 aix = pos.x * xModifier ^ worldSeed;
            rng.setSeed(aix ^ pos.z * zModifier);
            rng.advance();
            if EXPECT_FALSE (rng.nextDouble() < 0.004) {
                int distance = pos.x;
                if (-pos.x > distance) distance = -pos.x;
                if (+pos.z > distance) distance = +pos.z;
                if (-pos.z > distance) distance = -pos.z;
                if (distance >= 80 || rng.nextInt(80) < distance) {
                    ++amountRight;
                    if (amountRight > 4) {
                        std::cout << "Position: (" << pos.x << ", " << pos.z << ") on seed: " << worldSeed << std::endl;
                    }
                }
            }
        }
    }
}*/

// replace all stone with air
// for (int i = 0; i < 16 * 16 * 256; i++) {
//     if ((chunk->getBlockAtIndex(i) >> 4) == 1 && (chunk->getBlockAtIndex(i) & 15) == 0) {
//         chunk->blocks[i] = 0;
//     }
// }

// useful for set certain blocks in the world
/*
//set all the blocks to all the blocks available
lce::registry::BlockRegistry blocks;
blocks.setup();
int x = 0;
int y = 0;
int z = 0;
for (auto block : blocks.REGISTRY.getAllValues()) {
    world->setBlock(x, y, z, block->object->getID(), block->object->getDataTag());
    x+=2;
    if (x == 16) {
        x = 0;
        z+=2;
        if (z == 16) {
            z = 0;
            y+=2;
        }
    }
}





