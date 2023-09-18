#include <cmath>

#include "GenMineshaft.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"

const DIRECTION HORIZONTAL[4] = {DIRECTION::NORTH, DIRECTION::EAST, DIRECTION::SOUTH, DIRECTION::WEST };

namespace mineshaft_generator {

    void MineshaftGenerator::generate(int64_t worldSeed, int chunkX, int chunkZ) {
        uint64_t random = getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        random = (random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
        int x = (chunkX << 4) + 2;
        int z = (chunkZ << 4) + 2;
        this->startX = x;
        this->startZ = z;
    }

    void MineshaftGenerator::generate(int64_t worldSeed, Pos2D chunkPos) {
        return generate(worldSeed, chunkPos.x, chunkPos.z);
    }

    MineshaftGenerator::MineshaftGenerator() {
        this->resetPieces();
    }

    void MineshaftGenerator::resetPieces() {
        for (int i = 0; i < 4; i++) {
            this->piecePlaceCounts[i] = PiecePlaceCount::DEFAULT[i];
        }
        this->numMinecartChests = 0;
        this->piecePlaceCountsSize = 4;
        this->imposedPiece = PieceType::NONE;
        this->totalWeight = 145; // WRONG
        this->previousPiece = PieceType::NONE;
        this->piecesSize = 0;
        this->pendingPiecesSize = 0;
        this->generationStopped = false;
    }

    void MineshaftGenerator::updatePieceWeight() {
        for (int i = 0; i < this->piecePlaceCountsSize; i++) {
            PiecePlaceCount piecePlaceCount = this->piecePlaceCounts[i];
            PieceWeight pieceWeight = PieceWeight::PIECE_WEIGHTS[static_cast<int>(piecePlaceCount.pieceType)];
            if (pieceWeight.maxPlaceCount > 0 && piecePlaceCount.placeCount < pieceWeight.maxPlaceCount) {
                return;
            }
        }
        // printf("Stopping because all weights are used up\n");
        this->generationStopped = true;
    }

    void MineshaftGenerator::onWeightedPiecePlaced(int piecePlaceCountIndex) {
        PiecePlaceCount &piecePlaceCount = this->piecePlaceCounts[piecePlaceCountIndex];

        piecePlaceCount.placeCount++;
        this->previousPiece = piecePlaceCount.pieceType;
        if (!piecePlaceCount.isValid()) {
            // printf("Removed weight %i %i\n", piecePlaceCountIndex, piecePlaceCount.pieceType);
            this->totalWeight -= PieceWeight::PIECE_WEIGHTS[static_cast<int>(piecePlaceCount.pieceType)].weight;
            this->piecePlaceCountsSize--;
            for (int i = piecePlaceCountIndex; i < this->piecePlaceCountsSize; i++) {
                this->piecePlaceCounts[i] = this->piecePlaceCounts[i + 1];
            }
            this->updatePieceWeight();
        }
    }

    BoundingBox
    MineshaftGenerator::createPieceBoundingBox(PieceType pieceType, int x, int y, int z,
                                                DIRECTION direction) {
        switch (pieceType) {
            default: {
                return BoundingBox::EMPTY;
            }
        }
    }

    Piece
    MineshaftGenerator::createPiece(PieceType pieceType, uint64_t *random,
                                     DIRECTION direction, int depth, BoundingBox boundingBox) {
        switch (pieceType) {
            default: {
                int additionalData = 0;
                return {pieceType, depth, boundingBox, direction, additionalData};
            }
        }
    }

    void MineshaftGenerator::addPiece(Piece piece) {
        // printf("Adding piece at %i with rotation %i and depth %i\n", piece.boundingBox.minX, piece.boundingBox.minZ, piece.orientation, piece.depth);
        this->pendingPieces[this->pendingPiecesSize++] = this->piecesSize;
        this->pieces[this->piecesSize++] = piece;
    }

    bool
    MineshaftGenerator::tryAddPieceFromType(PieceType pieceType, uint64_t *random, int x, int y,
                                             int z, DIRECTION direction, int depth) {
        BoundingBox boundingBox = this->createPieceBoundingBox(pieceType, x, y, z, direction);
        return true;
    }

    void MineshaftGenerator::generateAndAddPiece(uint64_t *random, int x, int y, int z,
                                                  DIRECTION direction, int depth) {
    }

    void MineshaftGenerator::generateSmallDoorChildForward(Piece &piece, uint64_t *random, int n,
                                                            int n2) {
        DIRECTION direction = piece.orientation;
        switch (direction) {
            case DIRECTION::NORTH: {
                return this->generateAndAddPiece(random, piece.boundingBox.minX + n, piece.boundingBox.minY + n2,
                                                 piece.boundingBox.minZ - 1, direction, piece.depth);
            }
            case DIRECTION::SOUTH: {
                return this->generateAndAddPiece(random, piece.boundingBox.minX + n, piece.boundingBox.minY + n2,
                                                 piece.boundingBox.maxZ + 1, direction, piece.depth);
            }
            case DIRECTION::WEST: {
                return this->generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY + n2,
                                                 piece.boundingBox.minZ + n, direction, piece.depth);
            }
            case DIRECTION::EAST: {
                return this->generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY + n2,
                                                 piece.boundingBox.minZ + n, direction, piece.depth);
            }
        }
    }

    void MineshaftGenerator::generateSmallDoorChildLeft(Piece &piece, uint64_t *random, int n, int n2) {
    }

    void MineshaftGenerator::generateSmallDoorChildRight(Piece &piece, uint64_t *random, int n, int n2) {
        DIRECTION direction = piece.orientation;
        switch (direction) {
            case DIRECTION::SOUTH:
            case DIRECTION::NORTH: {
                return this->generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY + n,
                                                 piece.boundingBox.minZ + n2, DIRECTION::EAST, piece.depth);
            }
            case DIRECTION::EAST:
            case DIRECTION::WEST: {
                return this->generateAndAddPiece(random, piece.boundingBox.minX + n2, piece.boundingBox.minY + n,
                                                 piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
            }
        }
    }

    void MineshaftGenerator::addChildren(Piece &piece, uint64_t *random) {
        switch (piece.type) {
            default:
                break;
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

    bool MineshaftGenerator::isOkBox(BoundingBox &boundingBox) {
        return boundingBox.minY > 10;
    }

}