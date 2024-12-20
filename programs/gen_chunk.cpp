
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenLakes.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"


int main() {

    /*RNG rngSeed;
    int64_t testLongSeed = 3343649769771685376LL;
    for (int i = 0; i < 10000; i++) {
        rngSeed = RNG::getPopulationSeed(27184353441555, 10, 15);
        for (int x = 0; x < i; x++) {
            rngSeed.nextInt();
        }
        if (rngSeed.nextLong() == testLongSeed) {
            std::cout << i << " rolls" << std::endl;
        }
    }*/

    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::AQUATIC;
    Generator g(console, version, -101, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    int WIDTH = 20;
    int X_OFF = 5;//10 15 or 5 11
    int Z_OFF = 11;



    std::string filename = R"(D:/Documents/GitHub/legacyChunkViewer/chunks/chunk_data_)"
                           + std::to_string(WIDTH) + ".bin";

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }


    auto* world = new World(&g);
    world->decorateChunkAndNeighbors({X_OFF, Z_OFF}, WIDTH);
    for (int cx = -WIDTH + X_OFF; cx < WIDTH + X_OFF + 1; cx++) {
        for (int cz = -WIDTH + Z_OFF; cz < WIDTH + Z_OFF + 1; cz++) {
            auto* chunk = world->getChunk({cx, cz});
            if (!chunk) {
                std::cerr << "Error getting chunk: " << cx << ", " << cz << std::endl;
                continue;
            }

            //replace all stone with air
            /*for (int i = 0; i < 16 * 16 * 256; i++) {
                if ((chunk->getBlockAtIndex(i) >> 4) == 1 && (chunk->getBlockAtIndex(i) & 15) == 0) {
                    chunk->blocks[i] = 0;
                }
            }*/

            /*auto* chunkPrimer = new ChunkPrimer();
            //set all the blocks to all the blocks available
            lce::registry::BlockRegistry blocks;
            blocks.setup();
            int x = 0;
            int y = 0;
            int z = 0;
            for (auto block : blocks.REGISTRY.getAllValues()) {
                chunkPrimer->setBlockAndData(x, y, z, block->object->getID(), block->object->getDataTag());
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
            } else {
                std::cout << "Chunk (" << cx << ", " << cz << ") "
                        "written to file: " << filename << std::endl;
            }
        }
    }

    std::cout << "finished!\n";

    file.close();


}