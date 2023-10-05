#include <cmath>

#include "GenMineshaft.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"


namespace mineshaft_generator {

    void MineshaftGenerator::generate(int64_t worldSeed, int chunkX, int chunkZ) {
        uint64_t rng = getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        // 4 rolls (1 for skip, 3 for is feature chunk rolls (2 double, 1 int))
        rng = (rng * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF;
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;
        piecesSize = 0;

        // build the entire structure
        int boundingBoxXUpper = startX + nextInt(&rng, 6) + 7;
        int boundingBoxYUpper = 54 + nextInt(&rng, 6);
        int boundingBoxZUpper = startZ + nextInt(&rng, 6) + 7;
        BoundingBox roomBoundingBox(startX, 50, startZ, boundingBoxXUpper, boundingBoxYUpper, boundingBoxZUpper);
        addPiece(PieceType::ROOM, 0, roomBoundingBox, DIRECTION::NORTH, 1);
        buildComponent(pieces[piecesSize - 1], &rng);

        // get Y level
        structureBoundingBox = BoundingBox::EMPTY;
        for (int index = 0; index < piecesSize; index++) {
            structureBoundingBox.encompass(pieces[index]);
        }

        // specifically mesa
        if (mineShaftType == MineshaftType::MESA) {
            int i = 63 - structureBoundingBox.maxY + structureBoundingBox.getYSize() / 2 + 5;
            structureBoundingBox.offset(0, i, 0);
            for (int index = 0; index < piecesSize; index++) {
                pieces[index].offset(0, i, 0);
            }
            return;
        }

        // non-mesa
        const int i = 53; // 63 - 10
        int j = structureBoundingBox.getYSize() + 1;
        if (j < i) {
            j += nextInt(&rng, i - j);
        }
        int k = j - structureBoundingBox.maxY;
        for (int piece = 0; piece < piecesSize; piece++) {
            pieces[piece].offset(0, k, 0);
        }

    }


    void MineshaftGenerator::reset() {
        piecesSize = 0;
        collisionChecks = 0;
    }


    void MineshaftGenerator::addPiece(PieceType type, int depth, const BoundingBox& boundingBox,
                                      DIRECTION direction, int additionalData) {
        Piece piece = Piece(type, depth, boundingBox, direction, additionalData);
        pieces[piecesSize++] = piece;
    }


    bool MineshaftGenerator::createPiece(uint64_t *rng, Pos3D pos, DIRECTION direction, int depth) {
        int randomRoom = nextInt(rng, 100);

        if (randomRoom >= 80) {
            int additionalData = 0;
            BoundingBox crossingBoundingBox = BoundingBox::orientBox(pos, -1, 0, 0, 5, 3, 5, direction);

            if(nextInt(rng, 4) == 0) {
                crossingBoundingBox.maxY += 4;
                additionalData = 1;
            }

            Piece *collidingPiece = findCollisionPiece(crossingBoundingBox);
            if (collidingPiece != nullptr) return false;

            addPiece(PieceType::CROSSING, depth, crossingBoundingBox, direction, additionalData);

        } else if (randomRoom >= 70) {
            BoundingBox stairsBoundingBox = BoundingBox::orientBox(pos, 0, -5, 0, 3, 8, 9, direction);

            Piece *collidingPiece = findCollisionPiece(stairsBoundingBox);
            if (collidingPiece != nullptr) return false;

            addPiece(PieceType::STAIRS, depth, stairsBoundingBox, direction, 0);

        } else {
            BoundingBox corridorBoundingBox;
            int i;
            for (i = nextInt(rng, 3) + 2; i > 0; --i) {
                int j = i * 5;
                corridorBoundingBox = BoundingBox::orientBox(pos, 0, 0, 0, 3, 3, j, direction);

                Piece *collidingPiece = findCollisionPiece(corridorBoundingBox);
                if (collidingPiece == nullptr) {
                    break;
                }
            }
            if (i == 0) return false;
            bool hasRails = nextInt(rng, 3) == 0;
            bool hasSpiders = !hasRails && nextInt(rng, 23) == 0;
            int additionalData = 0;
            additionalData |= hasRails;
            additionalData |= (hasSpiders) << 1;
            addPiece(PieceType::CORRIDOR, depth, corridorBoundingBox, direction, additionalData);
        }
        return true;
    }


    void MineshaftGenerator::genAndAddPiece(uint64_t *rng, Pos3D pos, DIRECTION direction, int depth) {
        if (depth > 8) return;
        if (abs(pos.getX() - startX) > 80 || abs(pos.getZ() - startZ) > 80) return;

        bool validRoom = createPiece(rng, pos, direction, depth + 1);
        if (validRoom) buildComponent(pieces[piecesSize - 1], rng);
    }


    Piece* MineshaftGenerator::findCollisionPiece(BoundingBox &boundingBox) {
        // return collision.findCollisionPiece(boundingBox);

        for (int i = 0; i < piecesSize; i++) {
            collisionChecks++;
            if (pieces[i].intersects(boundingBox)) {
                return &pieces[i];
            }
        }
        return nullptr;

    }


    void MineshaftGenerator::buildComponent(Piece &piece, uint64_t *rng) {
        switch (piece.type) {

            case PieceType::ROOM: {
                int k;
                int j = piece.getYSize() - 4;
                if(j <= 0) j = 1;
                int roomXSize = piece.getXSize();
                int roomZSize = piece.getZSize();
                for (k = 0; k < roomXSize; k += 4) {
                    k += nextInt(rng, roomXSize);
                    if(k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {piece.minX + k, piece.minY + nextInt(rng, j) + 1,
                                   piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                }

                for (k = 0; k < roomXSize; k += 4) {
                    k += nextInt(rng, roomXSize);
                    if(k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {piece.minX + k,
                                   piece.minY + nextInt(rng, j) + 1,
                                   piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                }

                for (k = 0; k < roomZSize; k += 4) {
                    k += nextInt(rng, roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {piece.minX - 1,
                                   piece.minY + nextInt(rng, j) + 1,
                                   piece.minZ + k}, DIRECTION::WEST, piece.depth);
                }

                for (k = 0; k < roomZSize; k += 4) {
                    k += nextInt(rng, roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {piece.maxX + 1,
                                   piece.minY + nextInt(rng, j) + 1,
                                   piece.minZ + k}, DIRECTION::EAST, piece.depth);
                }
                return;
            }

            case PieceType::CORRIDOR: {
                int corridorType = nextInt(rng, 4);
                int yState = piece.minY + nextInt(rng, 3) - 1;
                switch(piece.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        switch(corridorType) {
                            case 0:
                            case 1:
                            default:
                                genAndAddPiece(rng, {piece.minX, yState, piece.minZ - 1}, piece.orientation, piece.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {piece.minX - 1, yState, piece.minZ}, DIRECTION::WEST, piece.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {piece.maxX + 1, yState, piece.minZ}, DIRECTION::EAST, piece.depth);
                                break;
                        }
                        break;

                    case DIRECTION::SOUTH:
                        switch(corridorType) {
                            case 0:
                            case 1:
                            default:
                                genAndAddPiece(rng, {piece.minX, yState, piece.maxZ + 1}, piece.orientation, piece.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {piece.minX - 1, yState, piece.maxZ - 3}, DIRECTION::WEST, piece.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {piece.maxX + 1, yState, piece.maxZ - 3}, DIRECTION::EAST, piece.depth);
                                break;
                        }
                        break;

                    case DIRECTION::WEST:
                        switch(corridorType) {
                            case 0:
                            case 1:
                            default:
                                genAndAddPiece(rng, {piece.minX - 1, yState, piece.minZ}, piece.orientation, piece.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {piece.minX, yState, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {piece.minX, yState, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                                break;
                        }
                        break;

                    case DIRECTION::EAST:
                        switch(corridorType) {
                            case 0:
                            case 1:
                            default:
                                genAndAddPiece(rng, {piece.maxX + 1, yState, piece.minZ}, piece.orientation, piece.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {piece.maxX - 3, yState, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {piece.maxX - 3, yState, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                                break;
                        }
                        break;
                }

                if (piece.depth >= 8) return;

                switch (piece.orientation) {
                    case DIRECTION::NORTH:
                    case DIRECTION::SOUTH:
                        for(int k = piece.minZ + 3; k + 3 <= piece.maxZ; k += 5) {
                            int l = nextInt(rng, 5);
                            if (l == 0) {
                                genAndAddPiece(rng, {piece.minX - 1, piece.minY, k}, DIRECTION::WEST, piece.depth + 1);
                            }
                            else if (l == 1) {
                                genAndAddPiece(rng, {piece.maxX + 1, piece.minY, k}, DIRECTION::EAST, piece.depth + 1);
                            }
                        }
                        return;
                    case DIRECTION::WEST:
                    case DIRECTION::EAST:
                        for (int k = piece.minX + 3; k + 3 <= piece.maxX; k += 5) {
                            int l = nextInt(rng, 5);
                            if (l == 0) {
                                genAndAddPiece(rng, {k, piece.minY, piece.minZ - 1}, DIRECTION::NORTH, piece.depth + 1);
                            }
                            else if (l == 1) {
                                genAndAddPiece(rng, {k, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth + 1);
                            }
                        }
                        return;
                }
                return;
            }

            case PieceType::CROSSING: {
                switch(piece.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                        genAndAddPiece(rng, {piece.minX - 1, piece.minY, piece.minZ + 1}, DIRECTION::WEST, piece.depth);
                        genAndAddPiece(rng, {piece.maxX + 1, piece.minY, piece.minZ + 1}, DIRECTION::EAST, piece.depth);
                        break;
                    case DIRECTION::SOUTH:
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                        genAndAddPiece(rng, {piece.minX - 1, piece.minY, piece.minZ + 1}, DIRECTION::WEST, piece.depth);
                        genAndAddPiece(rng, {piece.maxX + 1, piece.minY, piece.minZ + 1}, DIRECTION::EAST, piece.depth);
                        break;
                    case DIRECTION::WEST:
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                        genAndAddPiece(rng, {piece.minX - 1, piece.minY, piece.minZ + 1}, DIRECTION::WEST, piece.depth);
                        break;
                    case DIRECTION::EAST:
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                        genAndAddPiece(rng, {piece.maxX + 1, piece.minY, piece.minZ + 1}, DIRECTION::EAST, piece.depth);
                        break;
                }

                if (piece.additionalData) {
                    if(nextBoolean(rng))
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY + 4, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                    if(nextBoolean(rng))
                        genAndAddPiece(rng, {piece.minX - 1, piece.minY + 4, piece.minZ + 1}, DIRECTION::WEST, piece.depth);
                    if(nextBoolean(rng))
                        genAndAddPiece(rng, {piece.maxX + 1, piece.minY + 4, piece.minZ + 1}, DIRECTION::EAST, piece.depth);
                    if(nextBoolean(rng))
                        genAndAddPiece(rng, {piece.minX + 1, piece.minY + 4, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                }
                return;
            }

            case PieceType::STAIRS: {
                switch(piece.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        genAndAddPiece(rng, {piece.minX, piece.minY, piece.minZ - 1}, DIRECTION::NORTH, piece.depth);
                        return;
                    case DIRECTION::SOUTH:
                        genAndAddPiece(rng, {piece.minX, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH, piece.depth);
                        return;
                    case DIRECTION::WEST:
                        genAndAddPiece(rng, {piece.minX - 1, piece.minY, piece.minZ}, DIRECTION::WEST, piece.depth);
                        return;
                    case DIRECTION::EAST:
                        genAndAddPiece(rng, {piece.maxX + 1, piece.minY, piece.minZ}, DIRECTION::EAST, piece.depth);
                        return;
                }
            }
            default:
                break;
        }
    }


}