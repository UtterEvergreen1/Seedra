
#include <fstream>
#include <iostream>

#include "LegacyCubiomes/structures/include.hpp"
#include "LegacyCubiomes/enchants/include.hpp"

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"



int main(int argc, char* argv[]) {
    // setup
    Biome::registerBiomes();
    EnchantmentHelper::setup(CONSOLE::WIIU, LCEVERSION::AQUATIC);
    Generator g(CONSOLE::WIIU, LCEVERSION::AQUATIC, 12498783792879328, BIOMESCALE::SMALL, WORLDSIZE::CLASSIC);

    // code
    generation::Mineshaft gen{};
    gen.generate(g.getWorldSeed(), 0, 0);
    std::cout << gen.pieceArraySize << std::endl;
}
