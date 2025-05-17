# Legacy Cubiomes

Modified Cubiomes library in C++ to generate chunks, check biomes, find structure positions, and find structure loot for
Minecraft Legacy Console Edition.

## Usages

### Chunk Generation
```c++
#include "terrain/Chunk.hpp"
#include "terrain/generator.hpp"

int main() {
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    Pos2D chunkPos(0, 0);
    ChunkPrimer* chunk = Chunk::provideNewChunk(generator, chunkPos);

    // Use the chunk (e.g., print the highest Y block)
    int highestY = chunk->getHighestYBlock();
    std::cout << "Highest Y block in chunk: " << highestY << std::endl;

    delete chunk;
    return 0;
}
```

### Saving a World's Biomes to a png
```c++
#include "common/worldPicture.hpp"
#include "terrain/generator.hpp"

int main() {
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);
    WorldPicture worldPicture(&generator);

    // Draw and save the biomes
    worldPicture.drawBiomes();
    worldPicture.save("output_directory/");

    return 0;
}
```

### Finding Structures
```c++
#include "structures/placement/DynamicStructures.hpp"
#include "terrain/generator.hpp"

int main() {
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // Example: Get all positions of Mansions replace mansion with other structures to get their positions
    auto mansionPositions = Placement::Mansion::getAllPositions(&generator);
    for (const auto& pos : mansionPositions) {
        std::cout << "Mansion at: (" << pos.x << ", " << pos.z << ")" << std::endl;
    }

    return 0;
}
```

### Finding Spawn
```c++
#include "terrain/generator.hpp"

int main() {
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // Get the spawn position
    Pos2D spawnPos = generator.getSpawnBlock();
    std::cout << "Estimated spawn position: (" << spawnPos.x << ", " << spawnPos.z << ")" << std::endl;

    return 0;
}
```

### Resuse a generator object with a different seed
```c++
#include "terrain/generator.hpp"

int main() {
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // Get the spawn position
    for (int seed = 0; seed < 10; ++seed) {
        generator.applyWorldSeed(seed);
        Pos2D spawnPos = generator.getSpawnBlock();
        std::cout << "Spawn position with seed " << seed << ": (" << spawnPos.x << ", " << spawnPos.z << ")" << std::endl;
    }
    return 0;
}
```

### Loot Usage
```c++
#include "LegacyCubiomes/loot/Tables.hpp"

int main(int argc, char* argv[]) {
    // Set up the items
    lce::registry::ItemRegistry::setup();
    
    constexpr auto Mode = loot::GenMode::MODERN;
    int64_t worldSeed = 789;
    loot::Container<27> container;
    
    // Get loot for blacksmith at chunk 0, 0
    loot::blacksmith.getLootFromChunk<Mode>(container, worldSeed, 0, 0);
    std::cout << container << std::endl;
    return 0;
}
```

## Example Usage 
```c++
#include "terrain/generator.hpp"
#include "common/worldPicture.hpp"
#include "structures/placement/DynamicStructures.hpp"
#include "loot/Tables.hpp"
#include "terrain/biomes/biome.hpp"

int main() {
    // Initialize the generator
    Biome::registerBiomes();
    Generator generator(lce::CONSOLE::XBOX360, LCEVERSION::ELYTRA, 12345, lce::WORLDSIZE::CLASSIC, lce::BIOMESCALE::SMALL);

    // Step 1: Generate a chunk
    Pos2D chunkPos(0, 0);
    ChunkPrimer* chunk = Chunk::provideNewChunk(generator, chunkPos);
    std::cout << "Generated chunk at position: (" << chunkPos.x << ", " << chunkPos.z << ")" << std::endl;

    // Step 2: Save the world's biomes to a PNG
    WorldPicture worldPicture(&generator);
    worldPicture.drawBiomes();
    worldPicture.save("output_directory/");
    std::cout << "Saved biomes to PNG in 'output_directory/'" << std::endl;

    // Step 3: Find all buried treasure positions
    auto buriedTreasurePositions = Placement::BuriedTreasure::getAllPositions(&generator);
    std::cout << "Found " << buriedTreasurePositions.size() << " buried treasures in the world." << std::endl;

    // Step 4: Retrieve loot for each buried treasure
    constexpr auto Mode = loot::GenMode::MODERN;
    loot::Container<27> container;

    for (const auto& pos : buriedTreasurePositions) {
        loot::buried_treasure.getLootFromChunk<Mode>(container, generator.getWorldSeed(), pos.x, pos.z);
        std::cout << "Loot for buried treasure at (" << pos.x << ", " << pos.z << "):" << std::endl;
        std::cout << container << std::endl;
    }

    // Clean up
    delete chunk;
    return 0;
}
```

Refer to the `tests/` directory to see many other ways the code can be used.

## Submodules

This project uses a [separate project](https://github.com/zugebot/lce.git).
Set this up by doing:

```bash
git init
git submodule add https://github.com/lce-resources/lceLIB.git
git submodule update --init
```

To use this as a submodule in another project, you can do

```bash
git submodule add https://github.com/UtterEvergreen1/LegacyCubiomes.git
git submodule update --init
```

## License

[Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0)
