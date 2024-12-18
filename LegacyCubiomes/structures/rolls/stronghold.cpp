#include "stronghold.hpp"
#include "LegacyCubiomes/chunk_generator/Chunk.hpp"

namespace structure_rolls {

    template<bool stopStrongholdChest, bool stopPortal>
    [[gnu::noinline]] bool
    Stronghold::additionalStrongholdRolls(ChunkPrimer *chunk, generation::Stronghold *strongholdGenerator, RNG &rng,
                                          c_int xChunk, c_int zChunk, const Piece &pieceStop) {
        const BoundingBox chunkBoundingBox = BoundingBox::makeChunkBox(xChunk, zChunk);

        /*if constexpr (isStrongholdChest) {
            if (chunk && isLiquidInStructureBoundingBox(chunkBoundingBox, pieceStop, chunk)) return false;
        }*/

        for (int pieceIndex = 0; pieceIndex < strongholdGenerator->pieceArraySize; ++pieceIndex) {
            const Piece &piece = strongholdGenerator->pieceArray[pieceIndex];

            if (piece.type == static_cast<i8>(generation::Stronghold::PieceType::NONE)) continue;

            if (!piece.intersects(chunkBoundingBox)) continue;

            if (chunk && piece.type != static_cast<i8>(generation::Stronghold::PieceType::PORTAL_ROOM) &&
                isLiquidInStructureBoundingBox(chunkBoundingBox, piece, chunk))
                continue;

            switch (static_cast<generation::Stronghold::PieceType>(piece.type)) {
                case generation::Stronghold::PieceType::STRAIGHT:
                    // true means check for air
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                    rng.advance<4>();
                    break;
                case generation::Stronghold::PieceType::PRISON_HALL:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 8, 4, 10, rng, chunk);
                    rng.advance<12>();
                    break;
                case generation::Stronghold::PieceType::LEFT_TURN:
                case generation::Stronghold::PieceType::RIGHT_TURN:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 4, rng, chunk);
                    break;
                case generation::Stronghold::PieceType::ROOM_CROSSING:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 10, 6, 10, rng, chunk);
                    if constexpr (stopStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    if (piece.additionalData == 2) { // rolling for the chest seed if in chunk
                        generateChest(chunkBoundingBox, piece, rng, 3, 4, 8);
                    }
                    break;
                case generation::Stronghold::PieceType::STRAIGHT_STAIRS_DOWN:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 7, rng, chunk);
                    break;
                case generation::Stronghold::PieceType::STAIRS_DOWN:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 4, rng, chunk);
                    break;
                case generation::Stronghold::PieceType::FIVE_CROSSING:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 9, 8, 10, rng, chunk);
                    rng.advance<109>();
                    break;
                case generation::Stronghold::PieceType::CHEST_CORRIDOR:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                    if constexpr (stopStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    generateChest(chunkBoundingBox, piece, rng, 3, 2, 3);
                    break;
                case generation::Stronghold::PieceType::LIBRARY:
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 13,
                                             piece.additionalData ? 10 : 5, 14, rng, chunk);
                    rng.advance<520>();
                    if constexpr (stopStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    generateChest(chunkBoundingBox, piece, rng, 3, 3, 5);
                    if (piece.additionalData == 1) { generateChest(chunkBoundingBox, piece, rng, 12, 8, 1); }
                    break;
                case generation::Stronghold::PieceType::PORTAL_ROOM:
                    if constexpr (stopPortal) {
                        rng.advance<760>();
                        return true;
                    } else {
                        rng.advance<772>(); // 760 rolls + 12 eye rolls = 772 rolls
                    }
                    break;
                case generation::Stronghold::PieceType::FILLER_CORRIDOR:
                case generation::Stronghold::PieceType::NONE:
                    break;
            }
        }

        return false;
    }

    template<bool isStrongholdChest>
    [[gnu::noinline]] bool
    Stronghold::generateStructure(ChunkPrimer *chunk, generation::Stronghold *strongholdGenerator, RNG &rng,
                                  c_int xChunk,
                                  c_int zChunk, const Piece &pieceStop) {


        return additionalStrongholdRolls<isStrongholdChest, false>(chunk, strongholdGenerator, rng, xChunk, zChunk,
                                                                   pieceStop);
    }

    void Stronghold::setEye(const BoundingBox &chunkBB, const Piece &piece, int x, int z, RNG &random,
                            std::vector<bool> &portalRoomEyes, int &success, int index) {
        bool hasEye = random.nextFloat() > 0.9F;
        Pos2D pos = piece.getWorldPos(x, z);
        if (pos.insideBounds(chunkBB) && pos.insideBounds(piece)) {
            portalRoomEyes[index] = hasEye;
            success++;
        }
    }

    [[gnu::noinline]] std::vector<bool> Stronghold::getEyePlacements(generation::Stronghold *strongholdGenerator, const Generator &g) {
        std::vector<bool> eyes(12, false);
        const Pos2D portalRoomPos = strongholdGenerator->portalRoomPiece->getWorldPos(5, 10);
        const BoundingBox portalRoomBoundingBox = BoundingBox(portalRoomPos.x - 5, 0, portalRoomPos.z - 5,
                                                              portalRoomPos.x + 5, 255, portalRoomPos.z + 5);
        Pos2D portalRoomChunkPos = portalRoomPos.toChunkPos();
        int eyesPlaced = 0;
        for (int x = -1; x <= 1; ++x) {
            for (int z = -1; z <= 1; ++z) {
                int checkChunkX = portalRoomChunkPos.x + x;
                int checkChunkZ = portalRoomChunkPos.z + z;
                const BoundingBox chunkBoundingBox = BoundingBox::makeChunkBox(checkChunkX, checkChunkZ);
                if (!portalRoomBoundingBox.intersects(chunkBoundingBox))
                    continue;

                ChunkPrimer *chunk = Chunk::provideChunk<true>(g, checkChunkX, checkChunkZ, true);
                RNG random = RNG::getPopulationSeed(g.getWorldSeed(), portalRoomChunkPos.x + x,
                                                    portalRoomChunkPos.z + z);
                additionalStrongholdRolls<false, true>(chunk, strongholdGenerator, random,
                                                       portalRoomChunkPos.x + x,
                                                       portalRoomChunkPos.z + z,
                                                       *strongholdGenerator->portalRoomPiece);
                delete chunk;
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 4, 8, random, eyes, eyesPlaced, 0);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 5, 8, random, eyes, eyesPlaced, 1);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 6, 8, random, eyes, eyesPlaced, 2);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 4, 12, random, eyes, eyesPlaced, 3);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 5, 12, random, eyes, eyesPlaced, 4);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 6, 12, random, eyes, eyesPlaced, 5);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 3, 9, random, eyes, eyesPlaced, 6);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 3, 10, random, eyes, eyesPlaced, 7);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 3, 11, random, eyes, eyesPlaced, 8);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 7, 9, random, eyes, eyesPlaced, 9);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 7, 10, random, eyes, eyesPlaced, 10);
                setEye(chunkBoundingBox, *strongholdGenerator->portalRoomPiece, 7, 11, random, eyes, eyesPlaced, 11);
                if (eyesPlaced == 12) {
                    return eyes;
                }
            }
        }

        //unreachable
        return eyes;
    }

    int Stronghold::getEyesCount(generation::Stronghold *strongholdGenerator, const Generator &g) {
        std::vector<bool> eyes = getEyePlacements(strongholdGenerator, g);
        int count = 0;
        for (bool eye : eyes) {
            if (eye) count++;
        }
        return count;
    }

    template bool
    Stronghold::additionalStrongholdRolls<false, false>(ChunkPrimer *, generation::Stronghold *,
                                                        RNG &, c_int, c_int, const Piece &);

    template bool
    Stronghold::additionalStrongholdRolls<true, false>(ChunkPrimer *, generation::Stronghold *,
                                                       RNG &, c_int, c_int, const Piece &);

    template bool
    Stronghold::additionalStrongholdRolls<false, true>(ChunkPrimer *, generation::Stronghold *,
                                                       RNG &, c_int, c_int, const Piece &);

    template bool
    Stronghold::generateStructure<false>(ChunkPrimer *, generation::Stronghold *,
                                         RNG &, c_int, c_int, const Piece &);

    template bool
    Stronghold::generateStructure<true>(ChunkPrimer *, generation::Stronghold *,
                                        RNG &, c_int, c_int, const Piece &);
} // namespace structure_rolls