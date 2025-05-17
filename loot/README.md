
# Loot System

This project implements the Minecraft Legacy Console Edition loot generation system. 
It provides a set of pre-compiled loot table objects that you can use to generate loot 
using a few public functions while keeping the internal code hidden.

---

## Prerequisites

- **Initialization**:  
Before generating loot, call the following at runtime (for example, at application startup) 
to ensure that both items and enchantments are set up correctly:

```cpp
// initialize items
lce::registry::ItemRegistry::setup();
// initialize enchants
enchants::EnchantController::setup(lce::CONSOLE::YOUR_CONSOLE, LCEVERSION::YOUR_VERSION);
```
Replace `YOUR_CONSOLE` and `YOUR_VERSION` with the appropriate values.

- **Project Integration**:  
Include only the public API headers (for generating loot from seed, chunk, or block) in your project. 
Internal headers are not intended for public use.

---

## Available Loot Tables

The following loot tables are pre-compiled and available for generating loot.

| Loot Table                  | Uses Enchants |
|-----------------------------|---------------|
| abandoned_mineshaft         | ✔️            |
| blacksmith                  | ❌             |
| buried_treasure             | ❌             |
| desert_temple               | ✔️            |
| end_city_treasure           | ✔️            |
| igloo                       | ❌             |
| jungle_temple               | ✔️            |
| jungle_temple_dispenser     | ❌             |
| nether_bridge               | ❌             |
| simple_dungeon              | ❌             |
| shipwreck_map               | ❌             |
| shipwreck_supply            | ✔️            |
| shipwreck_treasure          | ❌             |
| spawn_bonus_chest           | ❌             |
| stronghold_corridor_elytra  | ✔️            |
| stronghold_corridor_aquatic | ✔️            |
| stronghold_crossing         | ✔️            |
| stronghold_library          | ✔️            |
| underwater_ruin_big         | ✔️            |
| underwater_ruin_small       | ✔️            |
| village_two_room_house      | ❌             |
| woodland_mansion            | ✔️            |

Each loot table is defined as a `static constexpr` object using the common `LootWrapper` and `TableWrapper` templates. 
These tables are ready to use out-of-the-box.

---

## Public API Functions

The main functions you will use to generate loot are defined on the loot table objects:

- **Generate Loot from Seed**
```cpp
template<GenMode Mode>
MU void getLootFromSeed(Container<ContainerSize>& container, RNG& seed) const;
```

- **Generate Loot from Chunk Coordinates**
```cpp
template<GenMode Mode, typename... Args>
MU void getLootFromChunk(Container<ContainerSize>& container, i64 worldSeed, Args&&... args) const;
```

- **Generate Loot from Block Coordinates**
```cpp
template<GenMode Mode, typename... Args>
MU void getLootFromBlock(Container<ContainerSize>& container, i64 worldSeed, Args&&... args) const;
```

- **Generate Loot from Loot Table Seed**
```cpp
template<GenMode Mode>
MU void getLootFromLootTableSeed(Container<ContainerSize>& container, u64 lootTableSeed, Buffer* buffer) const;
```
> **Note:** `Args` type can be either `Pos2D`, or `int, int`

> **Note:** Replace `Container<9/27>` with the appropriate container type (`Container<27>` or `Container<9>`) 
based on the loot table. Only `jungle_temple_dispenser` uses `Container<9>`.

### Flags

| Mode                     | Description                                                                                                               |
|--------------------------|---------------------------------------------------------------------------------------------------------------------------|
| **GenMode::LEGACY**      | Uses the legacy generation algorithm (TU31-).                                                                             |
| **GenMode::MODERN**      | Uses the modern generation algorithm (TU46+).                                                                             |
| **GenMode::MOD_NO_SHUF** | Uses the modern generation algorithm (TU46+), but does not shuffle items after loot generation (used for checking items). |

---

## Usage Example

Below is an example illustrating how to generate loot:

```cpp
#include "loot/tables/abandoned_mineshaft.hpp" // Include the desired loot table
#include "common/rng.hpp"                      // Include RNG

int main() {
    // Initialize the enchantment system (necessary for tables with enchantments):
    enchants::EnchantController::setup(lce::CONSOLE::YOUR_CONSOLE, LCEVERSION::YOUR_VERSION);

    // Select a loot table, e.g., abandoned_mineshaft:
    auto lootTable = loot::abandoned_mineshaft;
    auto mode = loot::GenMode::MODERN;

    // Create a container for the loot (ensure correct size: 27 or 9, as required):
    loot::Container<27> container;

    // Create and seed an RNG:
    RNG rng;
    rng.setSeed(123456789);  // Example seed

    // Generate loot from a seed:
    lootTable.getLootFromSeed<mode>(container, rng);

    // Process or display the container's loot as needed.
    
    // Clear the container if you plan to generate loot again:
    container.clear();

    return 0;
}
```

---

## Additional Information

- **Container Management:**  
The public API functions require a reference to a `loot::Container`. After generating loot, 
process the container's contents as needed. Always call `container.clear()` 
before generating new loot to avoid leftover items from previous generations.
It is important to note the only loot table with a container size of 9 is `jungle_temple_dispenser`.

- **Internal Details:**  
Internal implementation details (such as weighted random selection, binary search on cumulative weights, 
and enchantment processing) are abstracted away. Users only interact with the public functions.

- **Documentation:**  
Detailed internal documentation is provided through in-code Doxygen comments.

---

## Conclusion

This loot system is designed to replicate Minecraft's loot generation while providing optimizations through 
compile-time configuration. Use the pre-compiled loot table objects and 
public functions (`getLootFromSeed`, `getLootFromChunk`, `getLootFromBlock`, and `getLootFromLootTableSeed`) 
to generate loot. Ensure you initialize the enchantment system and manage your container correctly.
