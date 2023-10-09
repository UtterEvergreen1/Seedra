
#include <iostream>
#include <vector>
#include <fstream>



#include "LegacyCubiomes/structures/include.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/cubiomes/picture.hpp"


int main(int argc, char* argv[]) {
    std::string dir = R"(C:\Users\Jerrin\CLionProjects\LegacyCubiomes\)";

    auto console = CONSOLE::PS3;
    auto version = LCEVERSION::AQUATIC;
    auto size = WORLDSIZE::CLASSIC;
    auto scale = BIOMESCALE::LARGE;
    auto seed = 1185706985;

    Biome::registerBiomes();
    EnchantmentHelper::setup(console, version);
    Generator g(console, version, size, scale);
    g.applyWorldSeed(seed);

    writeBiomesToImage(&g, dir);

    return 0;
}




/*
   uint64_t rng;
   int passes = 1;
   Pos2D pos = {0, 0};
   auto biome = ocean;

   Pos2D biomePos = g.locateBiome(
       pos,
       4,
       biome,
       &rng,
       &passes
       );

   std::cout << biomePos << std::endl;


   while (true) {
       // g.incrementSeed();
       // Pos2DVec_t pos2DVec = Placement::BuriedTreasure::getAllPositions(&g);
       // if (pos2DVec.empty()) continue;
       // for (Pos2D pos : pos2DVec) {}

       break;
   }*/
