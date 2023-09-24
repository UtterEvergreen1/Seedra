#pragma once

#include "loot.hpp"
#include "loot_classes.hpp"

#include "LegacyCubiomes/structures/structure_rolls/stronghold_rolls.hpp"
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"

using stronghold_generator::StrongholdGenerator;

namespace loot_tables {
    template <typename T>
    class StrongholdLoot : public Loot<StrongholdLoot<T>> {
    public:

        /// loot seeding with stronghold stone rolls
        template<bool checkCaves>
        static uint64_t getLootSeed(const Generator& g, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator);

        /// Combine loot seeding and generation to get the stronghold loot.
        template<bool checkCaves, bool shuffle>
        ND static Container getLoot(const Generator& g, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator);
    };

}
