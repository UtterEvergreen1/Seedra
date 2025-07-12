#include "structures/placement/stronghold.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome.hpp"

int main() {
    /*constexpr int64_t MAX_SEED = (1LL << 48); // Maximum seed value for 48-bit seeds
    // Generator g(lce::CONSOLE::XBOX1, LCEVERSION::ELYTRA, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    RNG rng;
    for (int64_t seed = 0; seed < MAX_SEED; seed++) {
        rng.setSeed(seed);
        for (int i = 0; i < 10; ++i) {
            c_int x = rng.nextInt(16) + 8;
            c_int z = rng.nextInt(16) + 8;
            c_int y = rng.nextInt(124);
            Pos3D pos = {x, y, z};
            for (int j = 10; j != 0; --j) {
                pos.z = rng.nextInt(8) - rng.nextInt(8);
                pos.y = rng.nextInt(4) - rng.nextInt(4);
                pos.x = rng.nextInt(8) - rng.nextInt(8);
            }
        }
    }*/
    Generator g(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 5023952512342200539 , lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::LARGE);
    // g.generateCache(4);
    Pos2D strongholdPos = Placement::Stronghold::getWorldPosition(g);
    std::cout << "Stronghold Position: " << strongholdPos << std::endl;
}
