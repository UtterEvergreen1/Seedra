#pragma once

#include "base_rolls.hpp"
#include "LegacyCubiomes/structures/structure_generation/mineshaft_generator/GenMineshaft.hpp"

using namespace mineshaft_generator;

namespace structure_rolls {
    class MineshaftRolls : public BaseRolls {
    public:
        std::vector<std::pair<Pos3D, int64_t>> mineshaftChests;
        void generateStructure(MineshaftGenerator* mineshaftGenerator, ChunkPrimer *chunk, uint64_t *random, int chunkX, int chunkZ);
        /// Generate all chests for a given mineshaft
        MU void generateAllChests(MineshaftGenerator* mineshaftGenerator, const Generator& g, bool generateFullChunk);
        void generateChest(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const BasePiece &piece, uint64_t *random, int x, int y, int z);
        static void placeCobWeb(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const BasePiece &piece, uint64_t *random, int x, int z);
    };

    void MineshaftRolls::generateStructure(MineshaftGenerator* mineshaftGenerator, ChunkPrimer *chunk, uint64_t *random, int chunkX, int chunkZ) {
        for (int pieceIndex = 0; pieceIndex < mineshaftGenerator->piecesSize; ++pieceIndex) {
            const mineshaft_generator::Piece &piece = mineshaftGenerator->pieces[pieceIndex];
            if (piece.type != mineshaft_generator::PieceType::NONE) {
                const BoundingBox chunkBoundingBox = BoundingBox((chunkX << 4), 0, (chunkZ << 4), (chunkX << 4) + 15, 255, (chunkZ << 4) + 15);
                const BoundingBox& pieceBoundingBox = piece.boundingBox;
                if (pieceBoundingBox.intersects(chunkBoundingBox)) {
                    if(piece.type == mineshaft_generator::PieceType::CORRIDOR) {
                        if(chunk && isLiquidInStructureBoundingBox(chunkBoundingBox, pieceBoundingBox, chunk))
                            continue;

                        int sectionCount =
                                ((piece.orientation == DIRECTION::NORTH ||
                                piece.orientation == DIRECTION::SOUTH) ?
                                piece.boundingBox.getZSize() :
                                piece.boundingBox.getXSize()) / 5;
                        int depth = sectionCount * 5;

                        skipNextN(random, 3 * depth);
                        if(piece.additionalData & 2)
                            skipNextN(random, 6 * depth);


                        for(int i = 0; i < sectionCount; ++i) {
                            int currentDepth = 2 + i * 5;
                            // place torches
                            bool shouldPlaceTorch = (chunk == nullptr);
                            if(!shouldPlaceTorch) {
                                int worldY = piece.getWorldY(3);
                                for (int x = 0; x <= 2; ++x) {
                                    int worldX = piece.getWorldX(x, currentDepth);
                                    int worldZ = piece.getWorldZ(x, currentDepth);
                                    if (!intersectsWithBlock(chunkBoundingBox, worldX, worldY, worldZ) ||
                                        !chunk->getBlock(worldX & 15, worldY, worldZ & 15)) {
                                        shouldPlaceTorch = false;
                                    }
                                }
                            }
                            if(shouldPlaceTorch) {
                                if(nextInt(random, 4) != 0) {
                                    *random = (*random * 205749139540585 + 277363943098) & 0xffffffffffff; // 2 rolls
                                }
                            }

                            // place cobwebs
                            if(chunk == nullptr) {
                                *random = (*random * 128954768138017 + 137139456763464) & 0xffffffffffff; // 8 rolls
                            }
                            else {
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 0, currentDepth - 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 2, currentDepth - 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 0, currentDepth + 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 2, currentDepth + 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 0, currentDepth - 2);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 2, currentDepth - 2);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 0, currentDepth + 2);
                                placeCobWeb(chunk, chunkBoundingBox, piece, random, 2, currentDepth + 2);
                            }
                            if(chunkX == 1 && chunkZ == -18) {
                                std::cout << nextLong(random) << std::endl;
                            }
                            if(nextInt(random, 100) == 0) {
                                generateChest(chunk, chunkBoundingBox, piece, random, 2, 0, currentDepth - 1);
                            }

                            if(nextInt(random, 100) == 0) {
                                generateChest(chunk, chunkBoundingBox, piece, random, 0, 0, currentDepth + 1);
                            }
                            //if it has spawner
                            if (piece.additionalData & 2)
                            {
                                *random = (*random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng for placement on the depth axis
                            }

                            // if it has rails
                            if(piece.additionalData & 1) {
                                for(int railPos = 0; railPos < depth; ++railPos) {
                                    int xPos = piece.getWorldX(1, railPos);
                                    int yPos = piece.getWorldY(-1);
                                    int zPos = piece.getWorldZ(1, railPos);
                                    if (intersectsWithBlock(chunkBoundingBox, xPos, yPos, zPos) && (chunk == nullptr || chunk->getBlock(xPos & 15, yPos - 1, zPos & 15) != 0)) {
                                        *random = (*random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng for rail placement
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void MineshaftRolls::generateAllChests(MineshaftGenerator* mineshaftGenerator, const Generator& g, bool generateFullChunk) {
        int xEnd = (mineshaftGenerator->startX >> 4) + 6;
        int zEnd = (mineshaftGenerator->startZ >> 4) + 6;
        uint64_t random;
        setSeed(&random, g.getWorldSeed());
        uint64_t xModifier = nextLong(&random);
        uint64_t zModifier = nextLong(&random);
        xModifier = (int64_t)(((xModifier / 2) * 2) + 1);
        zModifier = (int64_t)(((zModifier / 2) * 2) + 1);
        for(int xChunk = (mineshaftGenerator->startX >> 4) - 6; xChunk < xEnd; ++xChunk) {
            uint64_t aix = xChunk * xModifier;
            for(int zChunk = (mineshaftGenerator->startZ >> 4) - 6; zChunk < zEnd; ++zChunk) {
                setSeed(&random, (aix + zChunk * zModifier) ^ g.getWorldSeed());
                random = (random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
                ChunkPrimer *chunk = nullptr;
                if(generateFullChunk) {
                    chunk = Chunk::provideChunk<true, true, false>(g, xChunk, zChunk);
                }
                generateStructure(mineshaftGenerator, chunk, &random, xChunk, zChunk);
                if(generateFullChunk) {
                    delete chunk;
                }
            }
        }
    }

    // TODO: generate legacy chest where the loot is generated with the seed and doesn't use the loot table seed
    void MineshaftRolls::generateChest(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const BasePiece &piece, uint64_t *random, int x, int y, int z) {
        int xPos = piece.getWorldX(x, z);
        int yPos = piece.getWorldY(y);
        int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBoundingBox, xPos, yPos, zPos) && (chunk == nullptr || chunk->getBlock(xPos & 15, yPos - 1, zPos & 15) != 0)) {
            *random = (*random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng for next boolean roll for rail shape
            mineshaftChests.emplace_back(Pos3D(xPos, yPos, zPos), nextLong(random));
        }
    }

    void MineshaftRolls::placeCobWeb(ChunkPrimer* chunk, const BoundingBox &chunkBoundingBox, const BasePiece &piece, uint64_t *random, int x, int z) {
        int xPos = piece.getWorldX(x, z);
        int yPos = piece.getWorldY(2);
        int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBoundingBox, xPos, yPos, zPos) && chunk->getSkyLight(xPos, yPos, zPos) < 8) {
            *random = (*random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
        }
    }
}
