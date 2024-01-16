#pragma once

#include "LegacyCubiomes/structures/generation/stronghold/stronghold.hpp"
#include "LegacyCubiomes/structures/rolls/rolls_base.hpp"

namespace structure_rolls {
    class Stronghold : RollsBase {
    public:
        /// generate all stronghold rolls in the chunk
        template<bool isStrongholdChest>
        MU static bool generateStructure(ChunkPrimer* chunk, generation::Stronghold* strongholdGenerator, RNG& rng,
                                         const Piece& pieceStop, int chestXChunk, int chestZChunk);
    };

    template<bool isStrongholdChest>
    bool Stronghold::generateStructure(ChunkPrimer* chunk, generation::Stronghold* strongholdGenerator, RNG& rng,
                                       const Piece& pieceStop, const int chestXChunk, const int chestZChunk) {
        const BoundingBox chunkBoundingBox = BoundingBox((chestXChunk << 4), 0, (chestZChunk << 4),
                                                         (chestXChunk << 4) + 15, 255, (chestZChunk << 4) + 15);
        if constexpr (isStrongholdChest) {
            if (chunk && isLiquidInStructureBoundingBox(chunkBoundingBox, pieceStop, chunk)) return false;
        }

        for (int pieceIndex = 0; pieceIndex < strongholdGenerator->pieceArraySize; ++pieceIndex) {
            const Piece& piece = strongholdGenerator->pieceArray[pieceIndex];

            if (piece.type == static_cast<int8_t>(generation::Stronghold::PieceType::NONE)) continue;

            if (!piece.intersects(chunkBoundingBox)) continue;

            if (chunk && piece.type != static_cast<int8_t>(generation::Stronghold::PieceType::PORTAL_ROOM) &&
                isLiquidInStructureBoundingBox(chunkBoundingBox, piece, chunk))
                continue;

            switch (piece.type) {
                case static_cast<int8_t>(generation::Stronghold::PieceType::STRAIGHT):
                    // true means check for air
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                    rng.advance4();
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::PRISON_HALL):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 8, 4, 10, rng, chunk);
                    rng.advance12();
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::LEFT_TURN):
                case static_cast<int8_t>(generation::Stronghold::PieceType::RIGHT_TURN):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 4, rng, chunk);
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::ROOM_CROSSING):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 10, 6, 10, rng, chunk);
                    if constexpr (isStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    if (piece.additionalData == 2) { // rolling for the chest seed if in chunk
                        generateChest(chunkBoundingBox, piece, rng, 3, 4, 8);
                    }
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::STRAIGHT_STAIRS_DOWN):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 7, rng, chunk);
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::STAIRS_DOWN):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 4, rng, chunk);
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::FIVE_CROSSING):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 9, 8, 10, rng, chunk);
                    rng.advance109();
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::CHEST_CORRIDOR):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                    if constexpr (isStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    generateChest(chunkBoundingBox, piece, rng, 3, 2, 3);
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::LIBRARY):
                    fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 13, piece.additionalData ? 10 : 5, 14,
                                             rng, chunk);
                    rng.advance520();
                    if constexpr (isStrongholdChest) {
                        if (piece == pieceStop) return true;
                    }
                    generateChest(chunkBoundingBox, piece, rng, 3, 3, 5);
                    if (piece.additionalData == 1) { generateChest(chunkBoundingBox, piece, rng, 12, 8, 1); }
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::PORTAL_ROOM):
                    rng.advance772(); // 760 rolls + 12 eye rolls = 772 rolls
                    break;
                case static_cast<int8_t>(generation::Stronghold::PieceType::FILLER_CORRIDOR):
                case static_cast<int8_t>(generation::Stronghold::PieceType::NONE):
                    break;
            }
        }

        // it should never reach here
        return false;
    }
} // namespace structure_rolls
