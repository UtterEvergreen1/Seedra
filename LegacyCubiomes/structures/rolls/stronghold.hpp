#pragma once

#include "LegacyCubiomes/structures/generation/stronghold/stronghold.hpp"
#include "LegacyCubiomes/structures/rolls/RollsBase.hpp"

namespace structure_rolls {
    class Stronghold : public RollsBase {
    public:

        /// generate certain stronghold rolls in the chunk
        template<bool stopStrongholdChest, bool stopPortal>
        static bool additionalStrongholdRolls(ChunkPrimer *chunk, generation::Stronghold *strongholdGenerator, RNG &rng,
                                              c_int xChunk, c_int zChunk, const Piece &pieceStop);

        template<bool isStrongholdChest>
        MU static bool
        generateStructure(ChunkPrimer *chunk, generation::Stronghold *strongholdGenerator, RNG &rng, c_int xChunk,
                          c_int zChunk, const Piece &pieceStop);

        static void setEye(const BoundingBox &chunkBB, const Piece &piece, int x, int z, RNG &random,
                           std::vector<bool> &portalRoomEyes, int &success, int index);

        static std::vector<bool> getEyePlacements(generation::Stronghold *strongholdGenerator, const Generator &g);

        /// get the number of eyes in the portal room
        MU static int getEyesCount(generation::Stronghold *strongholdGenerator, const Generator &g);
    };
} // namespace structure_rolls
