
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"


void writeBlocksToFile(const ChunkPrimer* chunkPrimer, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Write the blocks array to the file
    int size = sizeof(chunkPrimer->blocks);
    file.write(reinterpret_cast<const char*>(chunkPrimer->blocks), size);
    if (!file) {
        std::cerr << "Error writing to file: " << filename << std::endl;
    } else {
        std::cout << "Blocks successfully written to file: " << filename << std::endl;
    }

    file.close();
}







int main() {
    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::AQUATIC;
    Generator g(console, version, 72, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    int WIDTH = 12;
    int X_OFF = 58 / 16;
    int Z_OFF = 184 / 16;

    // generation::Mineshaft mineshaft;
    // mineshaft.generate(-101, X_OFF, Z_OFF);
    // structure_rolls::Mineshaft mineshaft1;
    // RNG rng;

    for (int cx = -WIDTH + X_OFF; cx < WIDTH + X_OFF + 1; cx++) {
        for (int cz = -WIDTH + Z_OFF; cz < WIDTH + Z_OFF + 1; cz++) {
            auto* chunkPrimer = Chunk::provideChunk
                    <true, true, true, false>(g, cx, cz);
            // mineshaft1.generateStructure(&mineshaft, chunkPrimer, rng, cx, cz);


            std::string filename = R"(D:\PycharmProjects\idk\chunks\_)"
                                   + std::to_string(cx) + "_"
                                   + std::to_string(cz) + "_.bin";
            writeBlocksToFile(chunkPrimer, filename);
        }
    }


}