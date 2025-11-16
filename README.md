# Seedra

Library in C++ to find structure positions, biomes, generate chunks, structures, and even find loot for
Minecraft Legacy Console Edition. Based on works of [Cubiomes](https://github.com/Cubitect/cubiomes).

# Disclaimer

This project is an independent reimplementation of Minecraft: Legacy Console Edition's
world generation behavior. It is not affiliated with, endorsed by, or approved by Mojang, Microsoft, or 4J Studios.

# Contributing to Seedra

By following this short guide, you will help keep the project organized,
maintainable, and safe. Thank you for considering contributing!

## Table of Contents

- [How to Contribute](#how-to-contribute)
- [Pull Request Workflow](#pull-request-workflow)
- [Coding Standards](#coding-standards)
- [License for Contributions](#license-for-contributions)

---

## How to Contribute

Clone the repository:
   ```bash
   # Clone the main repo
   git clone https://github.com/UtterEvergreen1/Seedra.git
   cd Seedra

   # Initialize and pull all submodules
   git submodule update --init --recursive
   ```

All contributions are welcome, including:

- Bug fixes
- Documentation improvements
- New features
- Tests or examples

Please make sure your changes are relevant to the project goals
(world generation, seed-finding, biomes, structures, loot, etc.).

---

## Pull Request Workflow

All code changes must be made in a separate branch and submitted via a pull request:

1. **Create a new branch** for your feature or bugfix:
   ```bash
   git checkout -b feature/my-feature
   ```

2. Make your changes.

3. Commit with clear, descriptive messages:

   ```bash
   git commit -m "Added a file that searches for big boi cacti."
   ```

4. Push your branch to the repository:

   ```bash
   git push origin feature/my-feature
   ```
   
5. Open a pull request against the main branch.

6. Wait for review and approval from a maintainer/admin. PRs cannot be merged without approval.

---

## Coding Standards

- Follow consistent format (ex. 4 spaces NO TABS). See [`.clang-format`](.clang-format).

- Use clear variable and function names.

- Comment complex algorithms.

- Keep commits small and logically grouped if possible.

---

## License for Contributions

By contributing to this project, you agree that your contributions will be licensed under the same terms
as the project (Apache License 2.0). See [`LICENSE`](LICENSE) for full details.

---

# Usages

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
#include "Seedra/loot/Tables.hpp"

int main(int argc, char* argv[]) {
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
        container.clear();
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

## Using as a Submodule

To use this as a submodule in another project, you can do:

```bash
git submodule add https://github.com/UtterEvergreen1/Seedra.git
git submodule update --init --recursive
```
