
#include "terrain/biomes/biome.hpp"
#include "terrain/carve/ChunkGenerator.hpp"


int main() {
    Biome::registerBiomes();
    auto console = lce::CONSOLE::WIIU;
    auto version = LCEVERSION::ELYTRA;
    // -6651998285536156346
    auto seed = 12345;
    auto worldsize = lce::WORLDSIZE::CLASSIC;
    auto biomescale = lce::BIOMESCALE::SMALL;;


    Generator g(console, version, seed, worldsize, biomescale);

    ChunkGeneratorOverWorld chunkGen(g);

    chunkGen.generateHeightmap3(10, 15, 10);

    return 0;

}