#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"

#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"

namespace fs = std::filesystem;

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


    Biome::registerBiomes();
    auto console = lce::CONSOLE::XBOX360;
    auto version = LCEVERSION::ELYTRA;
    // -6651998285536156346
    Generator g(console, version, 27184353441555, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // 3 13 for seed -101, 8 15 for seed 1, 11 16 or 15 5 for seed 27184353441555
    int X_WIDTH = 27;
    int Z_WIDTH = 27;
    int X_CENTER = 0;
    int Z_CENTER = 0;

    auto world = World(&g);
    world.generateMineshafts();
    world.generateVillages();
    world.generateStrongholds();
    world.getOrCreateChunk({X_CENTER, Z_CENTER});
    world.decorateChunks({X_CENTER, Z_CENTER}, X_WIDTH);

    // const lce::blocks::Block* block = world.getBlock({-215, 69, 118});
    // std::cout << block->getID() << " " << block->getDataTag() << "\n";


    std::ofstream file(filePath.string(), std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filePath << std::endl;
        return -1;
    }

    int64_t worldSeed = g.getWorldSeed();

    // file.write(reinterpret_cast<const char*>(&worldSeed), sizeof(worldSeed));
    file.write(reinterpret_cast<const char*>(&X_CENTER), sizeof(X_CENTER));
    file.write(reinterpret_cast<const char*>(&Z_CENTER), sizeof(Z_CENTER));
    file.write(reinterpret_cast<const char*>(&X_WIDTH), sizeof(X_WIDTH));
    file.write(reinterpret_cast<const char*>(&Z_WIDTH), sizeof(Z_WIDTH));


    for (int cx = -(X_WIDTH) + X_CENTER; cx < X_WIDTH + X_CENTER - 1; cx++) {
        for (int cz = -(Z_WIDTH) + Z_CENTER; cz < Z_WIDTH + Z_CENTER - 1; cz++) {
            auto* chunk = world.getChunk({cx, cz});
            if (!chunk) {
                std::cerr << "Error getting chunk: " << cx << ", " << cz << std::endl;
                continue;
            }

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
            }*/


            file.write(reinterpret_cast<const char*>(&cx), sizeof(cx));
            file.write(reinterpret_cast<const char*>(&cz), sizeof(cz));

            file.write(reinterpret_cast<const char*>(chunk->blocks), sizeof(chunk->blocks));

            if (!file) {
                std::cerr << "Error writing to file: " << filePath << std::endl;
            }
        }
    }

    return 0;
}









