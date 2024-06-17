# Legacy Cubiomes

Modified Cubiomes library in C++ to generate chunks, check biomes, find structure positions, and find structure loot for
Minecraft Legacy Console Edition.

## Usage/Examples

Structure Positions:

```c++
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/structures/placement/stronghold.hpp"
#include "LegacyCubiomes/structures/placement/DynamicStructures.hpp"

int main(int argc, char* argv[]) {
    int64_t worldSeed = 789;
    // Create a generator
    Generator g(lce::CONSOLE::WIIU, LCEVERSION::AQUATIC, worldSeed, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // Get all feature positions
    std::vector<Placement::FeatureStructurePair> features = Placement::Feature::getAllFeaturePositions(&g);
    // Print all features
    for (const auto& feature : features) {
        std::cout << feature << std::endl;
    }

    // Get stronghold position
    Pos2D strongholdPos = Placement::Stronghold::getWorldPosition(g);
    // Print stronghold position
    std::cout << "Stronghold at: " << strongholdPos << std::endl;

    // Get spawn block
    Pos2D spawnBlock = g.getSpawnBlock();
    // Print spawn block
    std::cout << "Spawn block at: " << spawnBlock << std::endl;

    // Get all monument positions in the world
    std::vector<Pos2D> monumentPositions = Placement::Monument::getAllPositions(&g);
    // Print monument positions
    for (const auto& monument : monumentPositions) {
        std::cout << "Monument at: " << monument << std::endl;
    }
    return 0;
}
```

Structure loot:

```c++
#include "LegacyCubiomes/loot/tables/buried_treasure.hpp"
#include "LegacyCubiomes/loot/tables/blacksmith.hpp"

int main(int argc, char* argv[]) {
    int64_t worldSeed = 789;
    // Get loot for buried treasure at chunk 0, 0
    loot::BuriedTreasure::setup();
    std::cout << loot::BuriedTreasure::getLoot<true>(worldSeed, 0, 0) << std::endl;

    // Get loot for blacksmith at chunk 0, 0
    loot::Blacksmith::setup();
    std::cout << loot::Blacksmith::getLootFromChunk<true, false>(worldSeed, 0, 0) << std::endl;
}
```

## Submodules
This project uses a [separate project](https://github.com/zugebot/lce.git).
Set this up by doing:
```bash
git submodule add https://github.com/zugebot/lce.git
git submodule update --init
```
To use this as a submodule in another project, you can do
```bash
git submodule add https://github.com/UtterEvergreen1/LegacyCubiomes.git
git submodule update --init
```

## License

[MIT](https://choosealicense.com/licenses/agpl-3.0/)
