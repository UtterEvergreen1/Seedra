
#include <iostream>
#include <vector>
#include <fstream>
#include "LegacyCubiomes/structures/generation/stronghold/GenStronghold.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/loot/stronghold_corridor.hpp"




using stronghold_generator::StrongholdGenerator;


int main(int argc, char* argv[]) {


    Biome::registerBiomes();
    EnchantmentHelper::setup(CONSOLE::WIIU, LCEVERSION::AQUATIC);
    //int64_t worldSeed = 48739;//98238811 = gelly, 48739 = 12d
    Generator g(LCEVERSION::AQUATIC, CONSOLE::WIIU, BIOMESCALE::SMALL, WORLDSIZE::CLASSIC);
    loot_tables::StrongholdCorridor<true>::setup();
    auto *gen = new StrongholdGenerator();

    gen->generate(g.getWorldSeed(), 0, 0);
    std::cout << gen->pieceArraySize << std::endl;
    delete gen;
}


