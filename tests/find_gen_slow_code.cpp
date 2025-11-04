

#include "common/AreaRange.hpp"
#include "common/timer.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/carve/CaveGenerator.hpp"
#include "terrain/finders/LargeCaveFinder.hpp"
#include "terrain/generator.hpp"



// #define TEST_PER_WORLD
#define TEST_PER_CHUNK


int main(int argc, char* argv[]) {
    Biome::registerBiomes();

    // vars
    auto CONSOLE = lce::CONSOLE::WIIU;
    auto VERSION = LCEVERSION::ELYTRA;
    auto WORLDSIZE = lce::WORLDSIZE::CLASSIC;
    auto SEED_COUNT = 1;

    // space
    int RADIUS = 27;
    Pos2D CENTER(0, 0);
    AreaRange RANGE = {CENTER, RADIUS, false, false};

    // instantiate world object we will be using
    Generator g(CONSOLE, VERSION, 0, WORLDSIZE, lce::BIOMESCALE::SMALL);
    World world(&g);

    // test
#ifdef TEST_PER_WORLD
    for (int i = 1; i <= SEED_COUNT; i++) {
        world.getGenerator()->applyWorldSeed(i);
        world.getGenerator()->generateCachesUpTo(4);

        std::cout << "\nSeed " << i << ":\n";

        Timer gen;
        world.generateMineshafts();
        world.generateVillages();
        world.generateStrongholds();
        std::cout << "    Structure Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        world.getOrCreateChunk({0, 0});

        world.createChunks(RANGE);
        std::cout << "    Chunk Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        world.decorateCaves(RANGE, true);
        std::cout << "    Cave Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        world.decorateChunks(RANGE);
        std::cout << "    Decorate Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        world.deleteWorld();
    }
#endif


#ifdef TEST_PER_CHUNK
    for (int i = 1; i <= SEED_COUNT; i++) {
        world.getGenerator()->applyWorldSeed(i);
        world.getGenerator()->generateCachesUpTo(4);

        std::cout << "\nSeed " << i << ":\n";

        Timer gen;
        world.generateMineshafts();
        world.generateVillages();
        world.generateStrongholds();
        std::cout << "    Structure Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        for (int x = -27; x < 27; x++) {
            for (int z = -27; z < 27; z++) {
                world.getOrCreateChunk({x, z});
            }
        }
        std::cout << "    Chunk Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        for (int x = -27; x < 27; x++) {
            for (int z = -27; z < 27; z++) {
                Chunk::populateCaves(world, {x, z});
            }
        }
        std::cout << "    Cave Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        for (int x = -27; x < 27; x++) {
            for (int z = -27; z < 27; z++) {
                Chunk::populateChunk(world, {x, z});
            }
        }
        std::cout << "    Decorate Gen Time: " << gen.getSeconds() << "\n";
        gen.reset();

        world.deleteWorld();
    }
#endif

    return 0;
}
