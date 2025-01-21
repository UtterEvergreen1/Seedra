#include "mineshaft.hpp"

#include "components/StructureComponent.hpp"
#include "enchants/__include.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"


namespace rolls {

    ///TODO: MAKE SURE IT WORKS!!!
    void Mineshaft::generateStructure(World& worldIn, const ChunkPrimer* chunk, const gen::Mineshaft* mg,
                                      RNG& rng, c_int chunkX, c_int chunkZ) {
        for (int pieceIndex = 0; pieceIndex < mg->getPieceCount(); ++pieceIndex) {
            const StructureComponent& piece = mg->getPieceConst(pieceIndex);

            if (piece.type == PT_Mineshaft_NONE) continue;

            BoundingBox chunkBoundingBox(chunkX << 4, 0, chunkZ << 4, (chunkX << 4) + 15, 255, (chunkZ << 4) + 15);

            if (!piece.intersects(chunkBoundingBox)) continue;


            if (piece.type == PT_Mineshaft_Corridor) {
                if (chunk && StructureComponent::isLiquidInStructureBoundingBox(chunkBoundingBox, piece, chunk))
                    continue;

                c_int sectionCount =
                        (piece.facing == EnumFacing::NORTH ||
                                         piece.facing == EnumFacing::SOUTH
                                 ? piece.getZSize()
                                 : piece.getXSize()) / 5;
                c_int depth = sectionCount * 5;

                rng.skipNextN(3 * depth);
                if (piece.data & 2) rng.skipNextN(6 * depth);


                for (int i = 0; i < sectionCount; ++i) {
                    c_int currentDepth = 2 + i * 5;

                    // place torches
                    bool shouldPlaceTorch = (chunk == nullptr);
                    if (!shouldPlaceTorch) {
                        c_int worldY = piece.getWorldY(3);
                        for (int x = 0; x <= 2; ++x) {
                            c_int worldX = piece.getWorldX(x, currentDepth);
                            c_int worldZ = piece.getWorldZ(x, currentDepth);
                            if (!StructureComponent::intersectsWithBlock(chunkBoundingBox, worldX, worldY, worldZ) ||
                                !chunk->getBlockId(worldX & 15, worldY, worldZ & 15)) {
                                shouldPlaceTorch = false;
                            }
                        }
                    }

                    if (shouldPlaceTorch) {
                        if (rng.nextInt(4) != 0) {
                            rng.advance<2>(); // 2 rolls
                        }
                    }

                    // place cobwebs
                    if (chunk == nullptr) {
                        rng.advance<8>(); // 8 rolls
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
                        generateChest(chunk, chunkBoundingBox, &piece, rng, 2, 0, currentDepth - 1);
                    }

                    if (rng.nextInt(100) == 0) {
                        generateChest(chunk, chunkBoundingBox, &piece, rng, 0, 0, currentDepth + 1);
                    }
                    //if it has spawner
                    if (piece.data & 2) {
                        // advance rng for placement on the depth axis
                        rng.advance();
                    }

                    // if it has rails
                    if (piece.data & 1) {
                        for (int railPos = 0; railPos < depth; ++railPos) {
                            c_int xPos = piece.getWorldX(1, railPos);
                            c_int yPos = piece.getWorldY(-1);
                            c_int zPos = piece.getWorldZ(1, railPos);
                            if (StructureComponent::intersectsWithBlock(chunkBoundingBox, xPos, yPos, zPos) &&
                                (chunk == nullptr || chunk->getBlockId(xPos & 15, yPos - 1, zPos & 15) != 0)) {
                                rng.advance(); // advance rng for rail placement
                            }
                        }
                    }
                }
            }


        }
    }


    MU void Mineshaft::generateAllChests(World& worldIn, const gen::Mineshaft* mg,
                                         const Generator& g, c_bool generateFullChunk) {
        Pos2D start = (mg->getStartPos() >> 4) - 6;
        Pos2D end = (mg->getStartPos() >> 4) + 6;


        RNG rng;
        rng.setSeed(g.getWorldSeed());
        u64 xModifier = rng.nextLong();
        u64 zModifier = rng.nextLong();
        xModifier = (i64) (((xModifier / 2) * 2) + 1);
        zModifier = (i64) (((zModifier / 2) * 2) + 1);
        for (int xChunk = start.x; xChunk < end.x; ++xChunk) {
            c_u64 aix = xChunk * xModifier;
            for (int zChunk = start.x; zChunk < end.z; ++zChunk) {
                rng.setSeed((aix + zChunk * zModifier) ^ g.getWorldSeed());
                rng.advance(); // advance rng
                const ChunkPrimer* chunk = nullptr;
                if (generateFullChunk) { chunk = Chunk::provideChunk(g, xChunk, zChunk); }
                generateStructure(worldIn, chunk, mg, rng, xChunk, zChunk);
                if (generateFullChunk) { delete chunk; }
            }
        }
    }


    // TODO: generate legacy chest where the loot is generated with the seed and doesn't use the loot table seed
    void Mineshaft::generateChest(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                  const StructureComponent *piece, RNG& rng, c_int x, c_int y, c_int z) {
        c_int xPos = piece->getWorldX(x, z);
        c_int yPos = piece->getWorldY(y);
        c_int zPos = piece->getWorldZ(x, z);
        if (StructureComponent::intersectsWithBlock(chunkBB, xPos, yPos, zPos) &&
            (chunk == nullptr || chunk->getBlockId(xPos & 15, yPos - 1, zPos & 15) != 0)) {
            rng.advance(); // advance rng for next boolean roll for rail shape
            mineshaftChests.emplace_back(Pos3D(xPos, yPos, zPos), rng.nextLong());
        }
    }


    void Mineshaft::placeCobWeb(const ChunkPrimer* chunk, const BoundingBox& chunkBB,
                                const StructureComponent& piece, RNG& rng, c_int x, c_int z) {
        c_int xPos = piece.getWorldX(x, z);
        c_int yPos = piece.getWorldY(2);
        c_int zPos = piece.getWorldZ(x, z);
        if (StructureComponent::intersectsWithBlock(chunkBB, xPos, yPos, zPos) &&
            chunk->getSkyLight(xPos, yPos, zPos) < 8) {
            rng.advance(); // advance rng
        }
    }


}