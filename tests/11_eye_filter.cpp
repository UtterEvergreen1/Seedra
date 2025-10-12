#include "structures/placement/stronghold.hpp"

int main() {
    // Used for testing stronghold placement on nex gen consoles
    Generator g(lce::CONSOLE::XBOX1, LCEVERSION::ELYTRA, 9, lce::WORLDSIZE::LARGE, lce::BIOMESCALE::LARGE);
    Placement::Stronghold::setWorldSize(lce::WORLDSIZE::LARGE);
    // g.setWorldGenerator(WORLDGENERATOR::FLAT);
    // g.setFixedBiome(BiomeID::ocean);
    // g.setStrongholdCount(3);
    // g.generateCache(4);
    auto strongholdPositions = Placement::Stronghold::getWorldPositions(g);
    for (auto& strongholdPos : strongholdPositions)
        std::cout << "Stronghold Position: " << strongholdPos << std::endl;

}
