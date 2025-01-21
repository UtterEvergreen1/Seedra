#include <cmath>

#include "stronghold.hpp"

#include "support/rng.hpp"

#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/blockStates.hpp"

using namespace lce::blocks;
using namespace lce::blocks::states;


namespace rolls {

    template<bool stopStrongholdChest, bool stopPortal>
    [[gnu::noinline]] bool
    Stronghold::additionalStrongholdRolls(ChunkPrimer *chunk, const gen::Stronghold * sg, RNG &rng,
                                          c_int xChunk, c_int zChunk, const StructureComponent &pieceStop) {
        const BoundingBox chunkBoundingBox = BoundingBox::makeChunkBox(xChunk, zChunk);

        /*if constexpr (isStrongholdChest) {
            if (chunk && isLiquidInStructureBoundingBox(chunkBoundingBox, pieceStop, chunk)) return false;
        }*/

        for (int pieceIndex = 0; pieceIndex < sg->getPieceCount(); ++pieceIndex) {
            const StructureComponent& piece = sg->getPieceConst(pieceIndex);

            if (piece.type == PT_Stronghold_NONE) continue;

            if (!piece.intersects(chunkBoundingBox)) continue;

            if (chunk && piece.type != PT_Stronghold_PortalRoom &&
                StructureComponent::isLiquidInStructureBoundingBox(chunkBoundingBox, piece, chunk))
                continue;

            switch (piece.type) {
                case PT_Stronghold_NONE:
                    break;
                case PT_Stronghold_Straight:
                    // true means check for air
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                    rng.advance<4>();
                    break;
                case PT_Stronghold_PrisonHall:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 8, 4, 10, rng, chunk);
                    rng.advance<12>();
                    break;
                case PT_Stronghold_LeftTurn:
                case PT_Stronghold_RightTurn:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 4, rng, chunk);
                    break;
                case PT_Stronghold_RoomCrossing:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 10, 6, 10, rng, chunk);
                    if constexpr (stopStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    if (piece.data == 2) { // rolling for the chest seed if in chunk
                        StructureComponent::generateChest(chunkBoundingBox, piece, rng, 3, 4, 8);
                    }
                    break;
                case PT_Stronghold_StraightStairsDown:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 7, rng, chunk);
                    break;
                case PT_Stronghold_StairsDown:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 4, rng, chunk);
                    break;
                case PT_Stronghold_FiveCrossing:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 9, 8, 10, rng, chunk);
                    rng.advance<109>();
                    break;
                case PT_Stronghold_ChestCorridor:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                    if constexpr (stopStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    StructureComponent::generateChest(chunkBoundingBox, piece, rng, 3, 2, 3);
                    break;
                case PT_Stronghold_Library:
                    StructureComponent::fillWithRandomizedBlocksRNGOnly(chunkBoundingBox, piece, 0, 0, 0, 13,
                                                                        piece.data ? 10 : 5, 14, rng, chunk);
                    rng.advance<520>();
                    if constexpr (stopStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    StructureComponent::generateChest(chunkBoundingBox, piece, rng, 3, 3, 5);
                    if (piece.data == 1) { StructureComponent::generateChest(chunkBoundingBox, piece, rng, 12, 8, 1); }
                    break;
                case PT_Stronghold_PortalRoom:
                    if constexpr (stopPortal) {
                        rng.advance<760>();
                        return true;
                    } else {
                        rng.advance<772>(); // 760 rolls + 12 eye rolls = 772 rolls
                    }
                    break;
                case PT_Stronghold_FillerCorridor:
                    break;

            }
        }

        return false;
    }

    template<bool isStrongholdChest>
    [[gnu::noinline]] bool
    Stronghold::generateStructure(ChunkPrimer *chunk, gen::Stronghold * sg, RNG &rng,
                                  c_int xChunk,
                                  c_int zChunk, const StructureComponent &pieceStop) {


        return additionalStrongholdRolls<isStrongholdChest, false>(chunk, sg, rng, xChunk, zChunk, pieceStop);
    }

    void Stronghold::setEye(const BoundingBox &chunkBB, const StructureComponent *piece, c_int x, c_int z, RNG &random,
                            std::vector<bool> &portalRoomEyes, int &success, c_int index) {
        c_bool hasEye = random.nextFloat() > 0.9F;
        const Pos2D pos = piece->getWorldPos(x, z);
        if (pos.insideBounds(chunkBB) && pos.insideBounds(*piece)) {
            portalRoomEyes[index] = hasEye;
            success++;
        }
    }

    [[gnu::noinline]] std::vector<bool> Stronghold::getEyePlacements(World& worldIn, const gen::Stronghold * sg, const Generator &g) {
        std::vector<bool> eyes(12, false);
        const Pos2D portalRoomPos = sg->portalRoomPiece->getWorldPos(5, 10);
        const auto portalRoomBoundingBox = BoundingBox(portalRoomPos.x - 5, 0, portalRoomPos.z - 5,
                                                       portalRoomPos.x + 5, 255, portalRoomPos.z + 5);
        const Pos2D portalRoomChunkPos = portalRoomPos.toChunkPos();
        int eyesPlaced = 0;
        for (int x = -1; x <= 1; ++x) {
            for (int z = -1; z <= 1; ++z) {
                c_int checkChunkX = portalRoomChunkPos.x + x;
                c_int checkChunkZ = portalRoomChunkPos.z + z;
                const BoundingBox chunkBoundingBox = BoundingBox::makeChunkBox(checkChunkX, checkChunkZ);
                if (!portalRoomBoundingBox.intersects(chunkBoundingBox))
                    continue;

                ChunkPrimer *chunk = Chunk::provideChunk<true>(g, checkChunkX, checkChunkZ, true);
                RNG random = RNG::getPopulationSeed(g.getWorldSeed(), portalRoomChunkPos.x + x,
                                                    portalRoomChunkPos.z + z);
                additionalStrongholdRolls<false, true>(chunk, sg, random,
                                                       portalRoomChunkPos.x + x,
                                                       portalRoomChunkPos.z + z,
                                                       *sg->portalRoomPiece);
                delete chunk;
                setEye(chunkBoundingBox, sg->portalRoomPiece, 4, 8, random, eyes, eyesPlaced, 0);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 5, 8, random, eyes, eyesPlaced, 1);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 6, 8, random, eyes, eyesPlaced, 2);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 4, 12, random, eyes, eyesPlaced, 3);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 5, 12, random, eyes, eyesPlaced, 4);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 6, 12, random, eyes, eyesPlaced, 5);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 3, 9, random, eyes, eyesPlaced, 6);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 3, 10, random, eyes, eyesPlaced, 7);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 3, 11, random, eyes, eyesPlaced, 8);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 7, 9, random, eyes, eyesPlaced, 9);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 7, 10, random, eyes, eyesPlaced, 10);
                setEye(chunkBoundingBox, sg->portalRoomPiece, 7, 11, random, eyes, eyesPlaced, 11);
                if (eyesPlaced == 12) {
                    return eyes;
                }
            }
        }

        //unreachable
        return eyes;
    }

    MU int Stronghold::getEyesCount(World& worldIn, const gen::Stronghold * sg, const Generator &g) {
        std::vector<bool> eyes = getEyePlacements(worldIn, sg, g);
        int count = 0;
        for (c_bool eye : eyes) {
            if (eye) count++;
        }
        return count;
    }

    template bool
    Stronghold::additionalStrongholdRolls<false, false>
            (ChunkPrimer *, const gen::Stronghold *, RNG &, c_int, c_int, const StructureComponent &);

    template bool
    Stronghold::additionalStrongholdRolls<true, false>
            (ChunkPrimer *, const gen::Stronghold *, RNG &, c_int, c_int, const StructureComponent &);

    template bool
    Stronghold::additionalStrongholdRolls<false, true>
            (ChunkPrimer *, const gen::Stronghold *, RNG &, c_int, c_int, const StructureComponent &);

    template bool
    Stronghold::generateStructure<false>
            (ChunkPrimer *, gen::Stronghold *, RNG &, c_int, c_int, const StructureComponent &);

    template bool
    Stronghold::generateStructure<true>
            (ChunkPrimer *, gen::Stronghold *, RNG &, c_int, c_int, const StructureComponent &);

} // namespace rolls