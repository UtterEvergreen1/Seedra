#include "mineshaft.hpp"


namespace structure_rolls {
    ///TODO: MAKE SURE IT WORKS!!!
    void Mineshaft::generateStructure(const generation::Mineshaft* mineshaftGenerator, ChunkPrimer* chunk, RNG& rng,
                                      const int chunkX, const int chunkZ) {
        for (int pieceIndex = 0; pieceIndex < mineshaftGenerator->pieceArraySize; ++pieceIndex) {
            const Piece& piece = mineshaftGenerator->pieceArray[pieceIndex];
            if (piece.type != static_cast<int8_t>(generation::Mineshaft::PieceType::NONE)) {
                const auto chunkBoundingBox =
                        BoundingBox(chunkX << 4, 0, chunkZ << 4, (chunkX << 4) + 15, 255, (chunkZ << 4) + 15);
                if (const BoundingBox& pieceBoundingBox = piece; pieceBoundingBox.intersects(chunkBoundingBox)) {
                    if (piece.type == static_cast<int8_t>(generation::Mineshaft::PieceType::CORRIDOR)) {
                        if (chunk && isLiquidInStructureBoundingBox(chunkBoundingBox, pieceBoundingBox, chunk))
                            continue;

                        const int sectionCount =
                                ((piece.orientation == DIRECTION::NORTH || piece.orientation == DIRECTION::SOUTH)
                                         ? piece.getZSize()
                                         : piece.getXSize()) /
                                5;
                        const int depth = sectionCount * 5;

                        rng.skipNextN(3 * depth);
                        if (piece.additionalData & 2) rng.skipNextN(6 * depth);


                        for (int i = 0; i < sectionCount; ++i) {
                            const int currentDepth = 2 + i * 5;
                            // place torches
                            bool shouldPlaceTorch = (chunk == nullptr);
                            if (!shouldPlaceTorch) {
                                const int worldY = piece.getWorldY(3);
                                for (int x = 0; x <= 2; ++x) {
                                    const int worldX = piece.getWorldX(x, currentDepth);
                                    const int worldZ = piece.getWorldZ(x, currentDepth);
                                    if (!intersectsWithBlock(chunkBoundingBox, worldX, worldY, worldZ) ||
                                        !chunk->getBlockId(worldX & 15, worldY, worldZ & 15)) {
                                        shouldPlaceTorch = false;
                                    }
                                }
                            }
                            if (shouldPlaceTorch) {
                                if (rng.nextInt(4) != 0) {
                                    rng.advance2(); // 2 rolls
                                }
                            }

                            // place cobwebs
                            if (chunk == nullptr) {
                                rng.advance8(); // 8 rolls
                            } else {
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 0, currentDepth - 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 2, currentDepth - 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 0, currentDepth + 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 2, currentDepth + 1);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 0, currentDepth - 2);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 2, currentDepth - 2);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 0, currentDepth + 2);
                                placeCobWeb(chunk, chunkBoundingBox, piece, rng, 2, currentDepth + 2);
                            }
                            /*if (chunkX == 1 && chunkZ == -18) {
                                std::cout << rng.nextLong() << std::endl;
                            }*/
                            if (rng.nextInt(100) == 0) {
                                generateChest(chunk, chunkBoundingBox, piece, rng, 2, 0, currentDepth - 1);
                            }

                            if (rng.nextInt(100) == 0) {
                                generateChest(chunk, chunkBoundingBox, piece, rng, 0, 0, currentDepth + 1);
                            }
                            //if it has spawner
                            if (piece.additionalData & 2) {
                                // advance rng for placement on the depth axis
                                rng.advance();
                            }

                            // if it has rails
                            if (piece.additionalData & 1) {
                                for (int railPos = 0; railPos < depth; ++railPos) {
                                    const int xPos = piece.getWorldX(1, railPos);
                                    const int yPos = piece.getWorldY(-1);
                                    const int zPos = piece.getWorldZ(1, railPos);
                                    if (intersectsWithBlock(chunkBoundingBox, xPos, yPos, zPos) &&
                                        (chunk == nullptr || chunk->getBlockId(xPos & 15, yPos - 1, zPos & 15) != 0)) {
                                        rng.advance(); // advance rng for rail placement
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    void Mineshaft::generateAllChests(generation::Mineshaft* mineshaftGenerator, const Generator& g,
                                      const bool generateFullChunk) {
        const int xEnd = (mineshaftGenerator->startX >> 4) + 6;
        const int zEnd = (mineshaftGenerator->startZ >> 4) + 6;
        RNG rng;
        rng.setSeed(g.getWorldSeed());
        uint64_t xModifier = rng.nextLong();
        uint64_t zModifier = rng.nextLong();
        xModifier = (int64_t) (((xModifier / 2) * 2) + 1);
        zModifier = (int64_t) (((zModifier / 2) * 2) + 1);
        for (int xChunk = (mineshaftGenerator->startX >> 4) - 6; xChunk < xEnd; ++xChunk) {
            const uint64_t aix = xChunk * xModifier;
            for (int zChunk = (mineshaftGenerator->startZ >> 4) - 6; zChunk < zEnd; ++zChunk) {
                rng.setSeed((aix + zChunk * zModifier) ^ g.getWorldSeed());
                rng.advance(); // advance rng
                ChunkPrimer* chunk = nullptr;
                if (generateFullChunk) { chunk = Chunk::provideChunk(g, xChunk, zChunk); }
                generateStructure(mineshaftGenerator, chunk, rng, xChunk, zChunk);
                if (generateFullChunk) { delete chunk; }
            }
        }
    }


    // TODO: generate legacy chest where the loot is generated with the seed and doesn't use the loot table seed
    void Mineshaft::generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB, const Piece& piece, RNG& rng,
                                  const int x, const int y, const int z) {
        const int xPos = piece.getWorldX(x, z);
        const int yPos = piece.getWorldY(y);
        const int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBB, xPos, yPos, zPos) &&
            (chunk == nullptr || chunk->getBlockId(xPos & 15, yPos - 1, zPos & 15) != 0)) {
            rng.advance(); // advance rng for next boolean roll for rail shape
            mineshaftChests.emplace_back(Pos3D(xPos, yPos, zPos), rng.nextLong());
        }
    }


    void Mineshaft::placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB, const Piece& piece, RNG& rng,
                                const int x, const int z) {
        const int xPos = piece.getWorldX(x, z);
        const int yPos = piece.getWorldY(2);
        const int zPos = piece.getWorldZ(x, z);
        if (intersectsWithBlock(chunkBB, xPos, yPos, zPos) && chunk->getSkyLight(xPos, yPos, zPos) < 8) {
            rng.advance(); // advance rng
        }
    }


} // namespace structure_rolls