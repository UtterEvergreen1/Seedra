#pragma once

#include "base_rolls.hpp"
#include "LegacyCubiomes/structures/structure_generation/stronghold_generator/GenStronghold.hpp"

using namespace stronghold_generator;

namespace structure_rolls {
    class StrongholdRolls : public BaseRolls {
    public:
        /** generate all stronghold rolls in the chunk */
        template<bool isStrongholdChest>
        static void generateStructure(uint64_t *random, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator, ChunkPrimer *chunk);
    };

    template<bool isStrongholdChest>
    void StrongholdRolls::generateStructure(uint64_t *random, int chestX, int chestY, int chestZ, StrongholdGenerator* strongholdGenerator, ChunkPrimer *chunk) {
        int chunkX = chestX >> 4;
        int chunkZ = chestZ >> 4;
        for (Piece &piece: strongholdGenerator->pieces) {
            if (piece.type != PieceType::NONE) {
                const BoundingBox chunkBoundingBox = BoundingBox((chunkX << 4), 0, (chunkZ << 4), (chunkX << 4) + 15, 255, (chunkZ << 4) + 15);
                const BoundingBox& pieceBoundingBox = piece.boundingBox;
                if (pieceBoundingBox.intersects(chunkBoundingBox)) {
                    if(chunk){
                        if(isLiquidInStructureBoundingBox(chunkBoundingBox, pieceBoundingBox, chunk))
                            continue;
                    }

                    switch (piece.type) {
                    case PieceType::STRAIGHT:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, random, chunk); // true means check for air
                        *random = (*random * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF; // 4 rolls
                        break;
                    case PieceType::PRISON_HALL:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 8, 4, 10, random, chunk);
                        *random = (*random * 0x199C3838D031 + 0xD4CF89E2CFCC) & 0xFFFFFFFFFFFF; // 12 rolls
                        break;
                    case PieceType::LEFT_TURN:
                    case PieceType::RIGHT_TURN:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 4, random, chunk);
                        break;
                    case PieceType::ROOM_CROSSING:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 10, 6, 10, random, chunk);
                        if constexpr (isStrongholdChest){
                            if (chestX == piece.getWorldX(3, 8) && chestY == piece.getWorldY(4) && chestZ == piece.getWorldZ(3, 8)) {
                                return;
                            }
                        }
                        if (piece.additionalData == 2) { //rolling for the chest seed if in chunk
                            generateChest(chunkBoundingBox, piece, random, 3, 4, 8);
                        }
                        break;
                    case PieceType::STRAIGHT_STAIRS_DOWN:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 7, random, chunk);
                        break;
                    case PieceType::STAIRS_DOWN:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 10, 4, random, chunk);
                        break;
                    case PieceType::FIVE_CROSSING:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 9, 8, 10, random, chunk);
                        *random = (*random * 0xE3DB7EC1825D + 0xF751DEF08DC7) & 0xFFFFFFFFFFFF; // 109 rolls
                        break;
                    case PieceType::CHEST_CORRIDOR:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 4, 4, 6, random, chunk);
                        if constexpr (isStrongholdChest){
                            if (chestX == piece.getWorldX(3, 3) && chestY == piece.getWorldY(2) && chestZ == piece.getWorldZ(3, 3)) {
                                return;
                            }
                        }
                        generateChest(chunkBoundingBox, piece, random, 3, 2, 3);
                        break;
                    case PieceType::LIBRARY:
                        fillWithRandomizedBlocks(chunkBoundingBox, piece, 0, 0, 0, 13,
                                                 piece.additionalData ? 10 : 5, 14, random, chunk);
                        *random = (*random * 0x53E5A095E721 + 0xCACA74409848) & 0xFFFFFFFFFFFF; // 520 rolls
                        if constexpr (isStrongholdChest){
                            if (chestX == piece.getWorldX(3, 5) && chestY == piece.getWorldY(3) && chestZ == piece.getWorldZ(3, 5)) {
                                return;
                            }
                        }
                        generateChest(chunkBoundingBox, piece, random, 3, 3, 5);
                        if (piece.additionalData == 1) {
                            generateChest(chunkBoundingBox, piece, random, 12, 8, 1);
                        }
                        break;
                    case PieceType::PORTAL_ROOM:
                        //*random = (*random * 0xE5CFDCCC10E1 + 0x2FC9E05B45B8) & 0xFFFFFFFFFFFF; // 760 rolls
                        *random = (*random * 0x129FF9FE0B11 + 0x80152440A804) & 0xFFFFFFFFFFFF; // 760 rolls + 12 eye rolls = 772 rolls
                        break;
                    case PieceType::FILLER_CORRIDOR:
                    case PieceType::NONE:
                        break;
                    }
                }
            }
        }
    }
}
