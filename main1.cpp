
#include <fstream>
#include <iostream>

#include "LegacyCubiomes/structures/include.hpp"
#include "LegacyCubiomes/enchants/include.hpp"
#include "LegacyCubiomes/loot/include.hpp"
#include "LegacyCubiomes/utils/include.hpp"

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"


#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

int main(int argc, char* argv[]) {
    Biome::registerBiomes();

    auto console = CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    EnchantmentHelper::setup(console, version);
    Generator g(console, version, 117, WORLDSIZE::CLASSIC, BIOMESCALE::SMALL);

    loot::StrongholdLibrary::setup();
    RNG state = RNG(216765366);
    Container con = loot::StrongholdLibrary::getLootFromLootTableSeed<false>(state);
    std::cout << con << std::endl;

    g.applyWorldSeed(5887763652726489932);
    auto pos = Placement::Stronghold::getWorldPosition(g);
    std::cout << pos << std::endl;
}

/*
    // Placement::BuriedTreasure::setWorldSize(WORLDSIZE::CLASSIC);
    // writeBiomesToImage(&g, R"(C:\Users\jerrin\CLionProjects\LegacyCubiomes\)");
    // Pos2D pos = Placement::BuriedTreasure::getPosition(&g, 0, 0);
    // std::cout << g.getWorldSeed() << std::endl;
    // std::cout << pos << std::endl;
    // auto lol2 = Pos2D(-55 - 0, 389 - 16).toChunkPos();
    // Pos2D chunkPos = pos.toChunkPos();
    // auto* chunkPrimer = Chunk::provideChunk<true, false, false>(g, 0, 0);
     */

