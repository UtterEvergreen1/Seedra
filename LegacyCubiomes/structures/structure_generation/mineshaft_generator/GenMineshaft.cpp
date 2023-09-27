#include <cmath>

#include "GenMineshaft.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"

namespace mineshaft_generator {

    void MineshaftGenerator::generate(int64_t worldSeed, int chunkX, int chunkZ) {
        uint64_t random = getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        random = (random * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF; // 4 rolls (1 for skip, 3 for is feature chunk rolls (2 double, 1 int))
        int x = (chunkX << 4) + 2;
        int z = (chunkZ << 4) + 2;
        this->startX = x;
        this->startZ = z;

        this->piecesSize = 0;

        int boundingBoxXUpper = x + nextInt(&random, 6) + 7;
        int boundingBoxYUpper = 54 + nextInt(&random, 6);
        int boundingBoxZUpper = z + nextInt(&random, 6) + 7;
        BoundingBox roomBoundingBox(x, 50, z,
                                    boundingBoxXUpper,
                                    boundingBoxYUpper,
                                    boundingBoxZUpper);
        Piece startPiece = Piece(PieceType::ROOM, 0, roomBoundingBox, DIRECTION::NORTH, 1);
        this->pieces[this->piecesSize++] = startPiece;
        this->buildComponent(startPiece, &random);

        // get Y level
        this->structureBoundingBox = BoundingBox::EMPTY;
        for (int pieceIndex = 0; pieceIndex < this->piecesSize; pieceIndex++) {
            this->structureBoundingBox.encompass(this->pieces[pieceIndex].boundingBox);
        }

        if(this->mineShaftType == MineshaftType::MESA) {
            int i = 63 - this->structureBoundingBox.maxY + this->structureBoundingBox.getYSize() / 2 + 5;
            this->structureBoundingBox.offset(0, i, 0);
            for (int pieceIndex = 0; pieceIndex < this->piecesSize; pieceIndex++) {
                this->pieces[pieceIndex].boundingBox.offset(0, i, 0);
            }
        }
        else {
            const int i = 53;//63 - 10
            int j = this->structureBoundingBox.getYSize() + 1;

            if (j < i) {
                j += nextInt(&random, i - j);
            }

            int k = j - this->structureBoundingBox.maxY;
            for (int pieceIndex = 0; pieceIndex < this->piecesSize; pieceIndex++) {
                this->pieces[pieceIndex].boundingBox.offset(0, k, 0);
            }
        }
    }

    bool MineshaftGenerator::createPiece(uint64_t *random, int x, int y, int z, DIRECTION direction, int depth) {
        int randomRoom = nextInt(random, 100);

        if(randomRoom >= 80) {
            int additionalData = 0;
            BoundingBox crossingBoundingBox = BoundingBox::orientBox(x, y, z, -1, 0, 0, 5, 3, 5, direction);

            if(nextInt(random, 4) == 0) {
                crossingBoundingBox.maxY += 4;
                additionalData = 1;
            }

            Piece *collidingPiece = this->findCollisionPiece(crossingBoundingBox);
            if (collidingPiece != nullptr) {
                return false;
            }
            this->pieces[this->piecesSize++] = {PieceType::CROSSING, depth, crossingBoundingBox, direction, additionalData};
        }
        else if(randomRoom >= 70) {
            BoundingBox stairsBoundingBox = BoundingBox::orientBox(x, y, z, 0, -5, 0, 3, 8, 9, direction);

            Piece *collidingPiece = this->findCollisionPiece(stairsBoundingBox);
            if (collidingPiece != nullptr) {
                return false;
            }
            this->pieces[this->piecesSize++] = {PieceType::STAIRS, depth, stairsBoundingBox, direction, 0};
        }
        else {
            BoundingBox corridorBoundingBox;
            int i;
            for(i = nextInt(random, 3) + 2; i > 0; --i) {
                int j = i * 5;
                corridorBoundingBox = BoundingBox::orientBox(x, y, z, 0, 0, 0, 3, 3, j, direction);

                Piece *collidingPiece = this->findCollisionPiece(corridorBoundingBox);
                if (collidingPiece == nullptr) {
                    break;
                }
            }
            if(i == 0) return false;
            bool hasRails = nextInt(random, 3) == 0;
            bool hasSpiders = !hasRails && nextInt(random, 23) == 0;
            int additionalData = 0;
            additionalData |= hasRails;
            additionalData |= (hasSpiders) << 1;
            this->pieces[this->piecesSize++] = {PieceType::CORRIDOR, depth, corridorBoundingBox, direction, additionalData};
        }
        return true;
    }

    void MineshaftGenerator::generateAndAddPiece(uint64_t *random, int x, int y, int z,
                                                  DIRECTION direction, int depth) {
        if (depth > 8) {
            return;
        }

        if (abs(x - this->startX) > 80 || abs(z - this->startZ) > 80) {
            return;
        }

        bool validRoom = this->createPiece(random, x, y, z, direction, depth + 1);
        if(validRoom) {
            buildComponent(this->pieces[this->piecesSize-1], random);
        }
    }

    void MineshaftGenerator::buildComponent(Piece &piece, uint64_t *random) {
        switch (piece.type) {
            case PieceType::ROOM: {
                int k;
                int j = piece.boundingBox.getYSize() - 4;
                if(j <= 0) j = 1;
                int roomXSize = piece.boundingBox.getXSize();
                int roomZSize = piece.boundingBox.getZSize();
                for(k = 0; k < roomXSize; k += 4) {
                    k += nextInt(random, roomXSize);
                    if(k + 3 > roomXSize) { break; }
                    generateAndAddPiece(random, piece.boundingBox.minX + k,
                                        piece.boundingBox.minY + nextInt(random, j) + 1,
                                        piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                }

                for(k = 0; k < roomXSize; k += 4) {
                    k += nextInt(random, roomXSize);
                    if(k + 3 > roomXSize) { break; }
                    generateAndAddPiece(random, piece.boundingBox.minX + k,
                                        piece.boundingBox.minY + nextInt(random, j) + 1,
                                        piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                }

                for(k = 0; k < roomZSize; k += 4) {
                    k += nextInt(random, roomZSize);
                    if(k + 3 > roomZSize) { break; }
                    generateAndAddPiece(random, piece.boundingBox.minX - 1,
                                        piece.boundingBox.minY + nextInt(random, j) + 1,
                                        piece.boundingBox.minZ + k, DIRECTION::WEST, piece.depth);
                }

                for(k = 0; k < roomZSize; k += 4) {
                    k += nextInt(random, roomZSize);
                    if(k + 3 > roomZSize) { break; }
                    generateAndAddPiece(random, piece.boundingBox.maxX + 1,
                                        piece.boundingBox.minY + nextInt(random, j) + 1,
                                        piece.boundingBox.minZ + k, DIRECTION::EAST, piece.depth);
                }
                return;
            }
            case PieceType::CORRIDOR: {
                int j = nextInt(random, 4);
                switch(piece.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        switch(j){
                            case 0:
                            case 1:
                            default:
                                generateAndAddPiece(random, piece.boundingBox.minX,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ - 1, piece.orientation, piece.depth);
                                break;
                            case 2:
                                generateAndAddPiece(random, piece.boundingBox.minX - 1,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ, DIRECTION::WEST, piece.depth);
                                break;
                            case 3:
                                generateAndAddPiece(random, piece.boundingBox.maxX + 1,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ, DIRECTION::EAST, piece.depth);
                                break;
                        }
                        break;
                    case DIRECTION::SOUTH:
                        switch(j){
                            case 0:
                            case 1:
                            default:
                                generateAndAddPiece(random, piece.boundingBox.minX,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.maxZ + 1, piece.orientation, piece.depth);
                                break;
                            case 2:
                                generateAndAddPiece(random, piece.boundingBox.minX - 1,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.maxZ - 3, DIRECTION::WEST, piece.depth);
                                break;
                            case 3:
                                generateAndAddPiece(random, piece.boundingBox.maxX + 1,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.maxZ - 3, DIRECTION::EAST, piece.depth);
                                break;
                        }
                        break;
                    case DIRECTION::WEST:
                        switch(j){
                            case 0:
                            case 1:
                            default:
                                generateAndAddPiece(random, piece.boundingBox.minX - 1,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ, piece.orientation, piece.depth);
                                break;
                            case 2:
                                generateAndAddPiece(random, piece.boundingBox.minX,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                                break;
                            case 3:
                                generateAndAddPiece(random, piece.boundingBox.minX,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                                break;
                        }
                        break;
                    case DIRECTION::EAST:
                        switch(j){
                            case 0:
                            case 1:
                            default:
                                generateAndAddPiece(random, piece.boundingBox.maxX + 1,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ, piece.orientation, piece.depth);
                                break;
                            case 2:
                                generateAndAddPiece(random, piece.boundingBox.maxX - 3,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                                break;
                            case 3:
                                generateAndAddPiece(random, piece.boundingBox.maxX - 3,
                                                    piece.boundingBox.minY + nextInt(random, 3) - 1,
                                                    piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                                break;
                        }
                        break;
                }
                if (piece.depth >= 8) return;
                switch(piece.orientation) {
                    case DIRECTION::NORTH:
                    case DIRECTION::SOUTH:
                        for(int k = piece.boundingBox.minZ + 3; k + 3 <= piece.boundingBox.maxZ; k += 5) {
                            int l = nextInt(random, 5);
                            if(l == 0) {
                                generateAndAddPiece(random, piece.boundingBox.minX - 1,
                                                    piece.boundingBox.minY, k, DIRECTION::WEST, piece.depth + 1);
                            }
                            else if(l == 1) {
                                generateAndAddPiece(random, piece.boundingBox.maxX + 1,
                                                    piece.boundingBox.minY, k, DIRECTION::EAST, piece.depth + 1);
                            }
                        }
                        return;
                    case DIRECTION::WEST:
                    case DIRECTION::EAST:
                        for(int k = piece.boundingBox.minX + 3; k + 3 <= piece.boundingBox.maxX; k += 5) {
                            int l = nextInt(random, 5);
                            if(l == 0) {
                                generateAndAddPiece(random, k, piece.boundingBox.minY,
                                                    piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth + 1);
                            }
                            else if(l == 1) {
                                generateAndAddPiece(random, k, piece.boundingBox.minY,
                                                    piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth + 1);
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
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY, piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY, piece.boundingBox.minZ + 1, DIRECTION::WEST, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY, piece.boundingBox.minZ + 1, DIRECTION::EAST, piece.depth);
                        break;
                    case DIRECTION::SOUTH:
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY, piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY, piece.boundingBox.minZ + 1, DIRECTION::WEST, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY, piece.boundingBox.minZ + 1, DIRECTION::EAST, piece.depth);
                        break;
                    case DIRECTION::WEST:
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY, piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY, piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY, piece.boundingBox.minZ + 1, DIRECTION::WEST, piece.depth);
                        break;
                    case DIRECTION::EAST:
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY, piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY, piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                        generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY, piece.boundingBox.minZ + 1, DIRECTION::EAST, piece.depth);
                        break;
                }

                if(piece.additionalData) {
                    if(nextBoolean(random)) {
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY + 4, piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                    }

                    if(nextBoolean(random)) {
                        generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY + 4, piece.boundingBox.minZ + 1, DIRECTION::WEST, piece.depth);
                    }

                    if(nextBoolean(random)) {
                        generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY + 4, piece.boundingBox.minZ + 1, DIRECTION::EAST, piece.depth);
                    }

                    if(nextBoolean(random)) {
                        generateAndAddPiece(random, piece.boundingBox.minX + 1, piece.boundingBox.minY + 4, piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                    }
                }
                return;
            }
            case PieceType::STAIRS: {
                switch(piece.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        generateAndAddPiece(random, piece.boundingBox.minX, piece.boundingBox.minY, piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
                        return;
                    case DIRECTION::SOUTH:
                        generateAndAddPiece(random, piece.boundingBox.minX, piece.boundingBox.minY, piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                        return;
                    case DIRECTION::WEST:
                        generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY, piece.boundingBox.minZ, DIRECTION::WEST, piece.depth);
                        return;
                    case DIRECTION::EAST:
                        generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY, piece.boundingBox.minZ, DIRECTION::EAST, piece.depth);
                        return;
                }
            }
        }
    }

    Piece *
    MineshaftGenerator::findCollisionPiece(BoundingBox &boundingBox) {
        for (int i = 0; i < this->piecesSize; i++) {
            if (this->pieces[i].boundingBox.intersects(boundingBox)) {
                return &this->pieces[i];
            }
        }

        return nullptr;
    }
}