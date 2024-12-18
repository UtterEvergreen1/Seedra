
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include <fstream>

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/structures/rolls/mineshaft.hpp"


int main() {
    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::AQUATIC;
    Generator g(console, version, -101, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    int WIDTH = 3;
    int X_OFF = 58 / 16;
    int Z_OFF = 184 / 16;



    std::string filename = R"(D:\PycharmProjects\idk\chunks\chunk_data_)"
                           + std::to_string(WIDTH) + ".bin";

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return -1;
    }


    for (int cx = -WIDTH + X_OFF; cx < WIDTH + X_OFF + 1; cx++) {
        for (int cz = -WIDTH + Z_OFF; cz < WIDTH + Z_OFF + 1; cz++) {
            auto* chunkPrimer = Chunk::provideChunk<true, true, true, false>(g, cx, cz);

            RNG rng = RNG::getPopulationSeed(g.getWorldSeed(), cx, cz);
            Biome::registry[g.getBiomeAt(1, (cx >> 4) + 8, (cz >> 4) + 8)]
                    ->decorate(chunkPrimer, rng, {cx, cz});



            file.write(reinterpret_cast<const char*>(&cx), sizeof(cx));
            file.write(reinterpret_cast<const char*>(&cz), sizeof(cz));

            file.write(reinterpret_cast<const char*>(chunkPrimer->blocks),
                       sizeof(chunkPrimer->blocks));

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