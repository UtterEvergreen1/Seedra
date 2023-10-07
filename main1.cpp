
#include <fstream>
// #include "LegacyCubiomes/structures/generation/stronghold/GenStronghold.hpp"
// #include "LegacyCubiomes/loot/stronghold_corridor.hpp"
#include <iostream>
#include <vector>
#include "LegacyCubiomes/structures/generation/mineshaft/GenMineshaft.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"



// using stronghold_generator::StrongholdGenerator;
using mineshaft_generator::MineshaftGenerator;


int main(int argc, char* argv[]) {
    // int64_t worldSeed = 48739; // 98238811 = gelly, 48739 = 12d
    Biome::registerBiomes();
    EnchantmentHelper::setup(CONSOLE::WIIU, LCEVERSION::AQUATIC);
    Generator g(LCEVERSION::AQUATIC, CONSOLE::WIIU, BIOMESCALE::SMALL, WORLDSIZE::CLASSIC);
    auto *gen = new MineshaftGenerator();
    g.applyWorldSeed(12498783792879328);
    gen->generate(g.getWorldSeed(), 0, 0);
    std::cout << gen->pieceArraySize << std::endl;
    delete gen;
}
