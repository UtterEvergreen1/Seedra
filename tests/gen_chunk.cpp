#include "common/AreaRange.hpp"
#include "common/timer.hpp"
#include "common/worldPicture.hpp"
#include "loot/Tables.hpp"
#include "structures/placement/DynamicStructures.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/finders/LargeCaveFinder.hpp"
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
    c_i64 WORLD_SEED = 2143553818; //2026238685186108391; // 615831156172857837; // 7710511010199114;
    c_auto CONSOLE = lce::CONSOLE::WIIU;
    c_auto VERSION = LCEVERSION::ELYTRA;
    c_auto WORLD_SIZE = lce::WORLDSIZE::CLASSIC;
    c_auto BIOME_SCALE = lce::BIOMESCALE::MEDIUM;
    // -6651998285536156346
    Generator g(CONSOLE, VERSION, WORLD_SEED, WORLD_SIZE, BIOME_SCALE);





    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int RADIUS = 27;
    Pos2D WIDTH(2 * RADIUS, 2 * RADIUS);
    Pos2D CENTER(0, 0);

    AreaRange RANGE = {CENTER, RADIUS, false, false};

    std::cout << "Starting...\n";


    Timer start;
    Timer gen;

    auto world = World(&g);

    world.generateMineshafts();
    world.generateVillages();
    world.generateStrongholds();

    world.getOrCreateChunk(CENTER);

    world.createChunks(RANGE);
    std::cout << "Chunk Gen Time: " << gen.getSeconds() << "\n";
    gen.reset();
    world.decorateCaves(RANGE, true);
    std::cout << "Cave Gen Time: " << gen.getSeconds() << "\n";
    gen.reset();


    // for (auto [key, value] : sectionsInChunk) {
    //     std::cout << "key: " << key << " value: " << value << "\n";
    // }

    // const Pos2DTemplate<i64> seedMultiplierCave = CaveGenerator::getSeedMultiplier(&g);
    // for (int x = -27; x < 27; x++) {
    //     for (int z = -27; z < 27; z++) {
    //         CaveGenerator caveGen(world);
    //         caveGen.setupRNG(seedMultiplierCave, {x, z});
    //         ChunkPrimer* chunk = world.getChunk({x, z});
    //         caveGen.generate(chunk, {x, z}, true);
    //     }
    // }

    std::cout << "Decorate Gen Time: " << gen.getSeconds() << "\n";
    std::cout << "Total world Gen Time: " << start.getSeconds() << "\n";


    std::ofstream file(filePath.string(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filePath << std::endl;
        return -1;
    }

    int64_t worldSeed = g.getWorldSeed();

    fileWrite<i64>(file, worldSeed);
    fileWrite<int>(file, CENTER.x);
    fileWrite<int>(file, CENTER.z);
    fileWrite<int>(file, WIDTH.x);
    fileWrite<int>(file, WIDTH.z);

    int sx = (CENTER.x - WIDTH.x) * 16;
    int sz = (CENTER.z - WIDTH.z) * 16;
    int sw = WIDTH.x * 2 * 16;
    int sh = WIDTH.z * 2 * 16;

    std::cout << "Generating biomes at "
              << sx << ", "
              << sz << ", "
              << sx + sw <<  ", "
              << sz + sh << std::endl;

    biome_t* biomes = world.getGenerator()
        ->getBiomeRange(1, sx, sz, sw, sh);

    if (biomes == nullptr) {
        std::cerr << "Error loading biome map, exiting..." << std::endl;
        return -1;
    } else {
        std::cout << "Loaded Biome Map, writing chunks to file" << std::endl;
    }

    const Pos2D lower(world.worldBounds.minX, world.worldBounds.minZ);
    const Pos2D upper(world.worldBounds.maxX, world.worldBounds.maxZ);

    for (int cx = lower.x; cx < upper.x; cx++) {
        for (int cz = lower.z; cz < upper.z; cz++) {
            Pos2D chunkPos(cx, cz);
            auto* chunkPrimer = world.getChunk({cx, cz});

            if (chunkPrimer == nullptr) {
                std::cerr << "Error getting chunk: " << chunkPos << std::endl;
                continue;
            }

            u8 biomeChunk[256];

            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    int bx = (chunkPos.x + WIDTH.x) * 16 + x;
                    int bz = (chunkPos.z + WIDTH.z) * 16 + z;
                    biomeChunk[z * 16 + x] = (u8)biomes[bz * sw + bx];
                }
            }


            chunkPrimer->highestYBlock = -1;
            chunkPrimer->highestYBlock = std::min(chunkPrimer->getHighestYBlock() + 1, 255);
            i64 sizeToWrite = (i64) sizeof(chunkPrimer->blocks[0]) * 256 * 256;

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

    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::AQUATIC, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    //generator.generateCaches(4);

    Timer timer;
    constexpr int seedInterval = 1000;
    for (int64_t seed = 830750; seed <= 830750; ++seed) {
        if EXPECT_FALSE (seed % seedInterval == 0) {
            std::cout << "Seed: " << seed << std::endl;
            std::cout << "Seeds per second: " << seedInterval / timer.getSeconds() << std::endl;
            timer.reset();
        }
        generator.applyWorldSeed(seed);

        auto buriedTreasurePositions = Placement::BuriedTreasure::getAllPositions(&generator);
        if (buriedTreasurePositions.size() < 4) {
            continue;
        }

        std::cout << "Found " << buriedTreasurePositions.size() << " buried treasures in the world. Seed: " << seed << std::endl;

        // Step 4: Retrieve loot for each buried treasure
        constexpr auto Mode = loot::GenMode::MODERN;
        loot::Container<27> container;

        for (const auto& pos : buriedTreasurePositions) {
            loot::buried_treasure.getLootFromBlock<Mode>(container, seed, pos.x, pos.z);
            std::cout << "Loot for buried treasure at (" << pos.x << ", " << pos.z << "):" << std::endl;
            std::cout << container << std::endl;
            container.clear();
        }
    }

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
 */





