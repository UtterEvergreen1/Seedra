#pragma once

#include "LegacyCubiomes/structures/gen/stronghold/stronghold.hpp"


namespace rolls {
    class Stronghold {
    public:
        /// generate certain stronghold rolls in the chunk
        template<bool stopStrongholdChest, bool stopPortal>
        static bool additionalStrongholdRolls(ChunkPrimer* chunk, const gen::Stronghold* sg, RNG& rng, c_int xChunk,
                                              c_int zChunk, const StructureComponent& pieceStop);

        template<bool isStrongholdChest>
        MU static bool generateStructure(ChunkPrimer* chunk, gen::Stronghold* sg, RNG& rng, c_int xChunk, c_int zChunk,
                                         const StructureComponent& pieceStop);

        static void setEye(const BoundingBox& chunkBB, const StructureComponent& piece, int x, int z, RNG& random,
                           std::vector<bool>& portalRoomEyes, int& success, int index);

        static std::vector<bool> getEyePlacements(World& worldIn, gen::Stronghold* sg, const Generator& g);

        /// get the number of eyes in the portal room
        MU static int getEyesCount(World& worldIn, gen::Stronghold* sg, const Generator& g);
    };

} // namespace rolls
