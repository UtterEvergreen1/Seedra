#include <fstream>
#include <algorithm>

#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"


#include "structures/gen/village/village.hpp"

#include "common/timer.hpp"
#include "structures/placement/StaticStructures.hpp"
#include "terrain/biomes/biome.hpp"


int main() {

    Biome::registerBiomes();
    auto console = lce::CONSOLE::XBOX360;
    auto version = LCEVERSION::BOUNTIFUL;

    auto worldsize = lce::WORLDSIZE::CLASSIC;
    auto biomescale = lce::BIOMESCALE::SMALL;
    int required_blacksmiths = 5;
    int seeds_to_check = 10000;

    Generator g(console, version, worldsize, biomescale);

    std::vector<gen::Village> villages;

    std::cout << "Start...\n\n";

    for (int seed = 0; seed < seeds_to_check; seed++) {
        g.applyWorldSeed(seed);
        villages.clear();

        auto village_locations = Placement::Village<false>::getAllPositions(&g);


        for (auto& village_pos : village_locations) {
            auto village_gen = gen::Village(&g);
            village_gen.generate(village_pos.toChunkPos());
            if (village_gen.getBlackSmithPiece() != nullptr) {
                villages.emplace_back(village_gen);
            }
        }

        if (villages.size() >= required_blacksmiths) {
            std::cout << "Seed: " << seed << "\n";
            int count = 1;
            for (auto& village : villages) {
                auto* piece = village.getBlackSmithPiece();
                std::cout << count << ". At " << piece->getWorldXYZ(0, 0, 0) << "\n";
                count++;
            }
            std::cout << "\n";
        }

    }

    std::cout << "\nEnd.";





    return 0;
}









