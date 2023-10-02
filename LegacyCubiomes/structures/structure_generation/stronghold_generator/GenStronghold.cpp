#include <cmath>

#include "GenStronghold.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"


const DIRECTION HORIZONTAL[4] = {DIRECTION::NORTH, DIRECTION::EAST, DIRECTION::SOUTH, DIRECTION::WEST };


namespace stronghold_generator {


    StrongholdGenerator::StrongholdGenerator() {
        resetPieces();
    }


    void StrongholdGenerator::generate(int64_t worldSeed, int chunkX, int chunkZ) {
        uint64_t random = getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        random = (random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;
        do {
            resetPieces();

            DIRECTION direction = HORIZONTAL[nextInt(&random, 4)];
            BoundingBox stairsBoundingBox = BasePiece::makeBoundingBox(startX, 64, startZ, direction, 5, 11, 5);
            Piece startPiece = Piece(PieceType::STAIRS_DOWN, 0, stairsBoundingBox, direction, 1);
            pieces[piecesSize++] = startPiece;
            addChildren(startPiece, &random);

            while (pendingPiecesSize != 0) {
                int i = nextInt(&random, pendingPiecesSize);
                Piece &piece = pieces[pendingPieces[i]];
                pendingPiecesSize--;
                for (int j = i; j < pendingPiecesSize; j++) {
                    pendingPieces[j] = pendingPieces[j + 1];
                }
                addChildren(piece, &random);
                if (portalRoomPiece != nullptr && generationStep == GenerationStep::PORTAL) {
                    return;
                }
                if (portalRoomPiece != nullptr && generationStopped && generationStep == GenerationStep::LAYOUT) {
                    return;
                }
            }

            if (portalRoomPiece != nullptr && generationStep == GenerationStep::LAYOUT) {
                return;
            }

            structureBoundingBox = BoundingBox::EMPTY;
            for (int pieceIndex = 0; pieceIndex < piecesSize; pieceIndex++) {
                structureBoundingBox.encompass(pieces[pieceIndex]);
            }

            const int i = 53; // 63 - 10
            int j = structureBoundingBox.getYSize() + 1;
            if (j < i) {
                j += nextInt(&random, i - j);
            }

            int k = j - structureBoundingBox.maxY;
            structureBoundingBox.offset(0, k, 0);
            for (int pieceIndex = 0; pieceIndex < piecesSize; pieceIndex++) {
                pieces[pieceIndex].offset(0, k, 0);
            }
        } while (portalRoomPiece == nullptr);
    }


    void StrongholdGenerator::resetPieces() {
        for (int i = 0; i < 11; i++) {
            piecePlaceCounts[i] = PiecePlaceCount::DEFAULT[i];
        }
        numAltarChests = 0;
        piecePlaceCountsSize = 11;
        imposedPiece = PieceType::NONE;
        totalWeight = 145;
        previousPiece = PieceType::NONE;
        piecesSize = 0;
        pendingPiecesSize = 0;
        portalRoomPiece = nullptr;
        generationStopped = false;
    }


    void StrongholdGenerator::addPiece(Piece piece) {
        // printf("Adding piece at %i with rotation %i and depth %i\n", piece.minX, piece.minZ, piece.orientation, piece.depth);
        pendingPieces[pendingPiecesSize++] = piecesSize;
        pieces[piecesSize++] = piece;
    }


    void StrongholdGenerator::updatePieceWeight() {
        for (int i = 0; i < piecePlaceCountsSize; i++) {
            PiecePlaceCount piecePlaceCount = piecePlaceCounts[i];
            PieceWeight pieceWeight = PieceWeight::PIECE_WEIGHTS[static_cast<int>(piecePlaceCount.pieceType)];
            if (pieceWeight.maxPlaceCount > 0 && piecePlaceCount.placeCount < pieceWeight.maxPlaceCount) {
                return;
            }
        }
        // printf("Stopping because all weights are used up\n");
        generationStopped = true;
    }


    void StrongholdGenerator::onWeightedPiecePlaced(int piecePlaceCountIndex) {
        PiecePlaceCount &piecePlaceCount = piecePlaceCounts[piecePlaceCountIndex];

        piecePlaceCount.placeCount++;
        previousPiece = piecePlaceCount.pieceType;
        if (!piecePlaceCount.isValid()) {
            // printf("Removed weight %i %i\n", piecePlaceCountIndex, piecePlaceCount.pieceType);
            totalWeight -= PieceWeight::PIECE_WEIGHTS[static_cast<int>(piecePlaceCount.pieceType)].weight;
            piecePlaceCountsSize--;
            for (int i = piecePlaceCountIndex; i < piecePlaceCountsSize; i++) {
                piecePlaceCounts[i] = piecePlaceCounts[i + 1];
            }
            updatePieceWeight();
        }
    }


    BoundingBox
    StrongholdGenerator::createPieceBoundingBox(PieceType pieceType, int x, int y, int z, DIRECTION direction) {
        switch (pieceType) {
            case PieceType::STRAIGHT:
                return BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 7, direction);
            case PieceType::PRISON_HALL:
                return BoundingBox::orientBox(x, y, z, -1, -1, 0, 9, 5, 11, direction);
            case PieceType::RIGHT_TURN:
            case PieceType::LEFT_TURN:
                return BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 5, direction);
            case PieceType::ROOM_CROSSING:
                return BoundingBox::orientBox(x, y, z, -4, -1, 0, 11, 7, 11, direction);
            case PieceType::STRAIGHT_STAIRS_DOWN:
                return BoundingBox::orientBox(x, y, z, -1, -7, 0, 5, 11, 8, direction);
            case PieceType::STAIRS_DOWN:
                return BoundingBox::orientBox(x, y, z, -1, -7, 0, 5, 11, 5, direction);
            case PieceType::FIVE_CROSSING:
                return BoundingBox::orientBox(x, y, z, -4, -3, 0, 10, 9, 11, direction);
            case PieceType::CHEST_CORRIDOR:
                return BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 7, direction);
            case PieceType::LIBRARY:
                return BoundingBox::orientBox(x, y, z, -4, -1, 0, 14, 11, 15, direction);
            case PieceType::PORTAL_ROOM:
                return BoundingBox::orientBox(x, y, z, -4, -1, 0, 11, 8, 16, direction);
            default:
                return BoundingBox::EMPTY;
        }
    }


    Piece StrongholdGenerator::createPiece(PieceType pieceType, uint64_t *random,
                                                           DIRECTION direction, int depth, BoundingBox boundingBox) {
        int additionalData = 0;
        switch (pieceType) {
            case PieceType::STRAIGHT:
                // random.nextInt(5); //this is java
                additionalData |= (nextInt(random, 2) == 0) << 0;
                additionalData |= (nextInt(random, 2) == 0) << 1;
                nextInt(random, 5);
                break;
            case PieceType::PRISON_HALL:
                nextInt(random, 5);
                break;
            case PieceType::LEFT_TURN:
                nextInt(random, 5);
                break;
            case PieceType::RIGHT_TURN:
                nextInt(random, 5);
                return {PieceType::LEFT_TURN, depth, boundingBox, direction, additionalData};
            case PieceType::ROOM_CROSSING:
                // random.nextInt(5); // this is java
                additionalData = nextInt(random, 5);
                nextInt(random, 5);
                break;
            case PieceType::STRAIGHT_STAIRS_DOWN:
                nextInt(random, 5);
                break;
            case PieceType::STAIRS_DOWN:
                nextInt(random, 5);
                break;
            case PieceType::FIVE_CROSSING:
                nextInt(random, 5);
                additionalData |= (nextBoolean(random)) << 0;
                additionalData |= (nextBoolean(random)) << 1;
                additionalData |= (nextBoolean(random)) << 2;
                additionalData |= (nextInt(random, 3) > 0) << 3;
                break;
            case PieceType::CHEST_CORRIDOR:
                altarChests[numAltarChests++] = &pieces[piecesSize];
                nextInt(random, 5);
                break;
            case PieceType::LIBRARY:
                nextInt(random, 5);
                if (boundingBox.maxY > 6) additionalData = 1;
                break;
            case PieceType::PORTAL_ROOM:
                portalRoomPiece = &pieces[piecesSize];
                break;
            default:
                break;
        }
        return {pieceType, depth, boundingBox, direction, additionalData};
    }


    bool
    StrongholdGenerator::tryAddPieceFromType(PieceType pieceType, uint64_t *random, int x, int y,
                                                                   int z, DIRECTION direction, int depth) {
        BoundingBox boundingBox = createPieceBoundingBox(pieceType, x, y, z, direction);
        if (!isOkBox(boundingBox) || findCollisionPiece(boundingBox) != nullptr) {
            if (pieceType == PieceType::LIBRARY) {
                boundingBox = BoundingBox::orientBox(x, y, z, -4, -1, 0, 14, 6, 15, direction);
                if (!isOkBox(boundingBox) || findCollisionPiece(boundingBox) != nullptr) {
                    return false;
                }
            } else {
                return false;
            }
        }

        Piece piece = createPiece(pieceType, random, direction, depth, boundingBox);
        addPiece(piece);
        if (pieceType == PieceType::PORTAL_ROOM && generationStep == GenerationStep::PORTAL) {
            // printf("Stopping at portal 0\n");
            generationStopped = true;
        }

        return true;
    }

    bool StrongholdGenerator::genPieceFromSmallDoor(uint64_t *rng, int x, int y, int z, DIRECTION direction, int depth) {
        // printf("Adding at %i %i %i\n", x, y, z);

        if (generationStopped) return false;

        if (imposedPiece != PieceType::NONE) {
            // printf("Adding forced piece\n");
            PieceType theImposedPiece = imposedPiece;
            imposedPiece = PieceType::NONE;
            if (tryAddPieceFromType(theImposedPiece, rng, x, y, z, direction, depth)) {
                // printf("Added forced piece\n");
                return true;
            }
        }

        int maxWeight = totalWeight;

        for (int attempt = 0; attempt < 5; attempt++) {
            int selectedWeight = nextInt(rng, maxWeight);
            // printf("Selected weight %i %i\n", totalWeight, selectedWeight);

            for (int piecePlaceCountIndex = 0; piecePlaceCountIndex < piecePlaceCountsSize; piecePlaceCountIndex++) {
                PiecePlaceCount &piecePlaceCount = piecePlaceCounts[piecePlaceCountIndex];
                PieceType pieceType = piecePlaceCount.pieceType;
                int weight = PieceWeight::PIECE_WEIGHTS[static_cast<int>(pieceType)].weight;

                if ((selectedWeight -= weight) >= 0) continue;

                // printf("Trying %llu %i\n", rng.seed, piecesSize);
                if (!piecePlaceCount.canPlace(depth) || pieceType == previousPiece) {
                    // printf("Can't add\n");
                    break;
                }
                if (!tryAddPieceFromType(pieceType, rng, x, y, z, direction, depth)) {
                    // printf("Failed\n");
                    continue;
                }
                // printf("After place %llu %i\n", rng.seed, piecesSize);
                onWeightedPiecePlaced(piecePlaceCountIndex);
                return true;
            }
        }

        BoundingBox boundingBox = BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 4, direction);
        Piece *collidingPiece = findCollisionPiece(boundingBox);
        if (collidingPiece == nullptr) return false;

        if (collidingPiece->minY == boundingBox.minY) {
            for (int i = 3; i >= 1; --i) {
                boundingBox = BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, i - 1, direction);
                if (collidingPiece->intersects(boundingBox)) continue;

                boundingBox = BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, i, direction);
                if (boundingBox.minY > 1) {
                    addPiece(Piece(PieceType::FILLER_CORRIDOR, depth, boundingBox, direction, 0));
                    return true;
                }

                return false;
            }
        }

        return false;
    }

    void StrongholdGenerator::genAndAddPiece(uint64_t *rng, int x, int y, int z, DIRECTION direction, int depth) {
        if (depth > 50) return;

        if (abs(x - startX) <= 48 && abs(z - startZ) <= 48) {
            genPieceFromSmallDoor(rng, x, y, z, direction, depth + 1);
            return;
        }

        for (int piecePlaceCountIndex = 0; piecePlaceCountIndex < piecePlaceCountsSize; piecePlaceCountIndex++) {
            PiecePlaceCount &piecePlaceCount = piecePlaceCounts[piecePlaceCountIndex];
            if (piecePlaceCount.pieceType == PieceType::PORTAL_ROOM) {
                BoundingBox boundingBox = BoundingBox::orientBox(x, y, z, -4, -1, 0, 11, 8, 16, direction);
                if (isOkBox(boundingBox) && findCollisionPiece(boundingBox) == nullptr) {
                    onWeightedPiecePlaced(piecePlaceCountIndex);
                }
                break;
            }
        }

    }


    Piece* StrongholdGenerator::findCollisionPiece(BoundingBox &boundingBox) {
        for (int i = 0; i < piecesSize; i++) {
            if (pieces[i].intersects(boundingBox)) {
                return &pieces[i];
            }
        }
        return nullptr;
    }


    bool StrongholdGenerator::isOkBox(BoundingBox &boundingBox) {
        return boundingBox.minY > 10;
    }


    void StrongholdGenerator::genSmallDoorChildForward(Piece &piece, uint64_t *rng, int n, int n2) {
        DIRECTION direction = piece.orientation;
        switch (direction) {
            case DIRECTION::NORTH:
                return genAndAddPiece(rng, piece.minX + n, piece.minY + n2, piece.minZ - 1, direction, piece.depth);
            case DIRECTION::SOUTH:
                return genAndAddPiece(rng, piece.minX + n, piece.minY + n2, piece.maxZ + 1, direction, piece.depth);
            case DIRECTION::WEST:
                return genAndAddPiece(rng, piece.minX - 1, piece.minY + n2, piece.minZ + n, direction, piece.depth);
            case DIRECTION::EAST:
                return genAndAddPiece(rng, piece.maxX + 1, piece.minY + n2, piece.minZ + n, direction, piece.depth);
        }
    }

    void StrongholdGenerator::genSmallDoorChildLeft(Piece &piece, uint64_t *rng, int n, int n2) {
        switch (piece.orientation) {
            case DIRECTION::SOUTH:
            case DIRECTION::NORTH:
                genAndAddPiece(rng, piece.minX - 1, piece.minY + n, piece.minZ + n2, DIRECTION::WEST, piece.depth);
                break;
            case DIRECTION::EAST:
            case DIRECTION::WEST:
                genAndAddPiece(rng, piece.minX + n2, piece.minY + n, piece.minZ - 1, DIRECTION::NORTH, piece.depth);
                break;
        }
    }

    void StrongholdGenerator::genSmallDoorChildRight(Piece &piece, uint64_t *rng, int n, int n2) {
        switch (piece.orientation) {
            case DIRECTION::SOUTH:
            case DIRECTION::NORTH:
                genAndAddPiece(rng, piece.maxX + 1, piece.minY + n, piece.minZ + n2, DIRECTION::EAST, piece.depth);
                break;
            case DIRECTION::EAST:
            case DIRECTION::WEST:
                genAndAddPiece(rng, piece.minX + n2, piece.minY + n, piece.maxZ + 1, DIRECTION::SOUTH, piece.depth);
                break;
        }
    }


    void StrongholdGenerator::addChildren(Piece &piece, uint64_t *rng) {
        switch (piece.type) {
            case PieceType::STRAIGHT:
                genSmallDoorChildForward(piece, rng, 1, 1);
                if ((piece.additionalData & 1) != 0)
                    genSmallDoorChildLeft(piece, rng, 1, 2);
                if ((piece.additionalData & 2) != 0)
                    genSmallDoorChildRight(piece, rng, 1, 2);
                break;
            case PieceType::PRISON_HALL:
                genSmallDoorChildForward(piece, rng, 1, 1);
                break;
            case PieceType::LEFT_TURN: {
                DIRECTION direction = piece.orientation;
                if (direction == DIRECTION::NORTH || direction == DIRECTION::EAST) {
                    genSmallDoorChildLeft(piece, rng, 1, 1);
                } else {
                    genSmallDoorChildRight(piece, rng, 1, 1);
                }
                break;
            }
            case PieceType::RIGHT_TURN: {
                DIRECTION direction = piece.orientation;
                if (direction == DIRECTION::NORTH || direction == DIRECTION::EAST) {
                    genSmallDoorChildRight(piece, rng, 1, 1);
                } else {
                    genSmallDoorChildLeft(piece, rng, 1, 1);
                }
                break;
            }
            case PieceType::ROOM_CROSSING:
                genSmallDoorChildForward(piece, rng, 4, 1);
                genSmallDoorChildLeft(piece, rng, 1, 4);
                genSmallDoorChildRight(piece, rng, 1, 4);
                break;
            case PieceType::STRAIGHT_STAIRS_DOWN:
                genSmallDoorChildForward(piece, rng, 1, 1);
                break;
            case PieceType::STAIRS_DOWN: {
                if (piece.additionalData != 0)
                    imposedPiece = PieceType::FIVE_CROSSING;
                genSmallDoorChildForward(piece, rng, 1, 1);
                break;
            }
            case PieceType::FIVE_CROSSING: {
                int n = 3;
                int n2 = 5;
                DIRECTION direction = piece.orientation;
                if (direction == DIRECTION::WEST || direction == DIRECTION::NORTH) {
                    n = 8 - n;
                    n2 = 8 - n2;
                }
                genSmallDoorChildForward(piece, rng, 5, 1);
                if ((piece.additionalData & 1) != 0)
                    genSmallDoorChildLeft(piece, rng, n, 1);
                if ((piece.additionalData & 2) != 0)
                    genSmallDoorChildLeft(piece, rng, n2, 7);
                if ((piece.additionalData & 4) != 0)
                    genSmallDoorChildRight(piece, rng, n, 1);
                if ((piece.additionalData & 8) != 0)
                    genSmallDoorChildRight(piece, rng, n2, 7);
                break;
            }
            case PieceType::CHEST_CORRIDOR:
                genSmallDoorChildForward(piece, rng, 1, 1);
                break;
            case PieceType::FILLER_CORRIDOR:
            case PieceType::PORTAL_ROOM:
            case PieceType::LIBRARY:
            case PieceType::NONE:
                break;
        }
    }

}