# Legacy Cubiomes

Modified Cubiomes library in C++ to generate chunks, check biomes, find structure positions, and find structure loot for
Minecraft Legacy Console Edition.

## Usage/Examples

Structure Positions:

```c++
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/structures/include.hpp"

int main(int argc, char* argv[]) {
    /* This will find mansions and villages of a world seed */
    int64_t worldSeed = 1;
    Generator generator(LCEVERSION::AQUATIC, CONSOLE::WIIU, worldSeed, BIOMESCALE::CLASSIC);
    std::cout << "Seed " << worldSeed << " has these buried treasures and features: " << std::endl;
    
    Placement::setWorldSize(worldSize); // sets world size of all structures
    Placement::Village::setWorldSize(WORLDSIZE::CLASSIC); // sets world size of one structure

    for (Pos2D& treasurePos : Placement::Treasure::getAllPositions(&generator)) {
        treasurePos.z++;
        std::cout << "Buried Treasure: " << treasurePos << std::endl;
    }

    for (const Placement::FeatureStructure& feature : Placement::Feature::getAllFeaturePositions(&generator))
        std::cout << feature << std::endl;
}
```

Structure loot:

```c++
#include "LegacyCubiomes/loot/include.hpp"

int main(int argc, char* argv[]) {
    loot::BuriedTreasure::setWorldSize();
    int64_t worldSeed = 1;
    std::cout << loot::BuriedTreasure::getLoot<true>(worldSeed, 0, 0) << std::endl;

    loot::Blacksmith::setWorldSize();
    std::cout << loot::Blacksmith::getLootFromChunk<true>(worldSeed, 0, 0) << std::endl;
}
```

## Used By

This project is used by the following personal apps:

- Legacy FSG

## License

[MIT](https://choosealicense.com/licenses/agpl-3.0/)
