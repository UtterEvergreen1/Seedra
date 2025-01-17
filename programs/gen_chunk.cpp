#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/structures/build/mineshaft.hpp"
#include "LegacyCubiomes/structures/build/stronghold.hpp"
#include "LegacyCubiomes/structures/build/village.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/mineshaft.hpp"
#include "LegacyCubiomes/structures/placement/stronghold.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"
#include "LegacyCubiomes/structures/rolls/stronghold.hpp"


int main() {


    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    // -6651998285536156346
    Generator g(console, version, -872420751, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

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

    const lce::blocks::Block* block = world.getBlock({-215, 69, 118});
    std::cout << block->getID() << " " << block->getDataTag() << "\n";

    std::string DIR_NAME = R"(C:/Users/Jerrin/CLionProjects/LegacyChunkViewer/)";
    std::string filename = DIR_NAME + R"(build/chunks/chunkdata.bin)";

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }

    file.write(reinterpret_cast<const char*>(&X_CENTER), sizeof(X_CENTER));
    file.write(reinterpret_cast<const char*>(&Z_CENTER), sizeof(Z_CENTER));
    file.write(reinterpret_cast<const char*>(&X_WIDTH), sizeof(X_WIDTH));
    file.write(reinterpret_cast<const char*>(&Z_WIDTH), sizeof(Z_WIDTH));


    for (int cx = -X_WIDTH + X_CENTER; cx < X_WIDTH + X_CENTER - 1; cx++) {
        for (int cz = -Z_WIDTH + Z_CENTER; cz < Z_WIDTH + Z_CENTER - 1; cz++) {
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

            file.write(reinterpret_cast<const char*>(chunk->blocks),
                       sizeof(chunk->blocks));

            if (!file) {
                std::cerr << "Error writing to file: " << filename << std::endl;
            }
        }
    }

    return 0;
}










