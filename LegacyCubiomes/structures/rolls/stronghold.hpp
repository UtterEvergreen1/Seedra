#pragma once

#include "LegacyCubiomes/structures/rolls/rolls_base.hpp"
#include "LegacyCubiomes/structures/generation/stronghold/stronghold.hpp"

namespace structure_rolls {
    class Stronghold : RollsBase {
    public:
        /** generate all stronghold rolls in the chunk */
        template<bool isStrongholdChest>
        MU static void generateStructure(ChunkPrimer *chunk, generation::Stronghold *strongholdGenerator,
                                         uint64_t *rng, int chestX, int chestY, int chestZ);
    };

    template<bool isStrongholdChest>
    void Stronghold::generateStructure(ChunkPrimer *chunk, generation::Stronghold *strongholdGenerator,
                                       uint64_t *rng, int chestX, int chestY, int chestZ) {
        int chunkX = chestX >> 4;
        int chunkZ = chestZ >> 4;
        for (int pieceIndex = 0; pieceIndex < strongholdGenerator->pieceArraySize; ++pieceIndex) {
            const Piece &piece = strongholdGenerator->pieceArray[pieceIndex];
            if (piece.type == generation::Stronghold::PieceType::NONE) continue;
            const BoundingBox chunkBoundingBox = BoundingBox((chunkX << 4), 0, (chunkZ << 4), (chunkX << 4) + 15, 255, (chunkZ << 4) + 15);
            const BoundingBox& pieceBoundingBox = piece;

            if (!pieceBoundingBox.intersects(chunkBoundingBox)) {
                continue;
            }

            if(chunk && piece.type != generation::Stronghold::PieceType::PORTAL_ROOM && isLiquidInStructureBoundingBox(chunkBoundingBox, pieceBoundingBox, chunk))
                continue;

            switch (piece.type) {
            case generation::Stronghold::PieceType::STRAIGHT:
                fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk); // true means check for air
                *rng = (*rng * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF; // 4 rolls
                break;
            case generation::Stronghold::PieceType::PRISON_HALL:
                fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 8, 4, 10, rng, chunk);
                *rng = (*rng * 0x199C3838D031 + 0xD4CF89E2CFCC) & 0xFFFFFFFFFFFF; // 12 rolls
                break;
            case generation::Stronghold::PieceType::LEFT_TURN:
            case generation::Stronghold::PieceType::RIGHT_TURN:
                fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 4, rng, chunk);
                break;
            case generation::Stronghold::PieceType::ROOM_CROSSING:
                fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 10, 6, 10, rng, chunk);
                if constexpr (isStrongholdChest){
                    if (chestX == piece.getWorldX(3, 8) && chestY == piece.getWorldY(4) && chestZ == piece.getWorldZ(3, 8)) {
                        return;
                    }
                }
                if (piece.additionalData == 2) { //rolling for the chest seed if in chunk
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
                *rng = (*rng * 0xE3DB7EC1825D + 0xF751DEF08DC7) & 0xFFFFFFFFFFFF; // 109 rolls
                break;
            case generation::Stronghold::PieceType::CHEST_CORRIDOR:
                fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, rng, chunk);
                if constexpr (isStrongholdChest) {
                    if (chestX == piece.getWorldX(3, 3) && chestY == piece.getWorldY(2) && chestZ == piece.getWorldZ(3, 3)) {
                        return;
                    }
                }
                generateChest(chunkBoundingBox, piece, rng, 3, 2, 3);
                break;
            case generation::Stronghold::PieceType::LIBRARY:
                fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 13,
                                         piece.additionalData ? 10 : 5, 14, rng, chunk);
                *rng = (*rng * 0x53E5A095E721 + 0xCACA74409848) & 0xFFFFFFFFFFFF; // 520 rolls
                if constexpr (isStrongholdChest) {
                    if (chestX == piece.getWorldX(3, 5) && chestY == piece.getWorldY(3) && chestZ == piece.getWorldZ(3, 5)) {
                        return;
                    }
                }
                generateChest(chunkBoundingBox, piece, rng, 3, 3, 5);
                if (piece.additionalData == 1) {
                    generateChest(chunkBoundingBox, piece, rng, 12, 8, 1);
                }
                break;
            case generation::Stronghold::PieceType::PORTAL_ROOM:
                // TODO: change this so the stronghold saves it's eye count...
                //*rng = (*rng * 0xE5CFDCCC10E1 + 0x2FC9E05B45B8) & 0xFFFFFFFFFFFF; // 760 rolls
                *rng = (*rng * 0x129FF9FE0B11 + 0x80152440A804) & 0xFFFFFFFFFFFF; // 760 rolls + 12 eye rolls = 772 rolls
                break;
            case generation::Stronghold::PieceType::FILLER_CORRIDOR:
            case generation::Stronghold::PieceType::NONE:
                break;
            }
        }
    }
}
