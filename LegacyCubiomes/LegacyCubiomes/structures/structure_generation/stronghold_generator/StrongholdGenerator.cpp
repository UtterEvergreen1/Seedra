#include <cmath>
#include "StrongholdGenerator.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"
const DIRECTION HORIZONTAL[4] = {DIRECTION::NORTH, DIRECTION::EAST, DIRECTION::SOUTH, DIRECTION::WEST };

void stronghold_generator::StrongholdGenerator::generate(int64_t worldSeed, int chunkX, int chunkZ) {
    uint64_t random = getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
    random = (random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
    int x = (chunkX << 4) + 2;
    int z = (chunkZ << 4) + 2;
    this->startX = x;
    this->startZ = z;
    do {
        this->resetPieces();
        DIRECTION direction = HORIZONTAL[nextInt(&random, 4)];
        BoundingBox stairsBoundingBox = BasePiece::makeBoundingBox(x, 64, z, direction, 5, 11, 5);
        Piece startPiece = Piece(PieceType::STAIRS_DOWN, 0, stairsBoundingBox, direction, 1);
        this->pieces[this->piecesSize++] = startPiece;
        this->addChildren(startPiece, &random);
        while (this->pendingPiecesSize != 0) {
            int i = nextInt(&random, this->pendingPiecesSize);
            Piece &piece = this->pieces[this->pendingPieces[i]];
            this->pendingPiecesSize--;
            for (int j = i; j < this->pendingPiecesSize; j++) {
                this->pendingPieces[j] = this->pendingPieces[j + 1];
            }
            this->addChildren(piece, &random);
            if (this->portalRoomPiece != nullptr && this->generationStep == GenerationStep::PORTAL) {
                return;
            }
            if (this->portalRoomPiece != nullptr && this->generationStopped && this->generationStep == GenerationStep::LAYOUT) {
                return;
            }
        }

        if (this->portalRoomPiece != nullptr && this->generationStep == GenerationStep::LAYOUT) {
            return;
        }

        BoundingBox boundingBox = BoundingBox::EMPTY;
        for (int pieceIndex = 0; pieceIndex < this->piecesSize; pieceIndex++) {
            boundingBox.encompass(this->pieces[pieceIndex].boundingBox);
        }

        const int i = 53;//63 - 10
        int j = boundingBox.getSizeY() + 1;

        if (j < i) {
            j += nextInt(&random, i - j);
        }

        int k = j - boundingBox.maxY;
        for (int pieceIndex = 0; pieceIndex < this->piecesSize; pieceIndex++) {
            this->pieces[pieceIndex].boundingBox.offset(0, k, 0);
        }
    } while (this->portalRoomPiece == nullptr);
}

void stronghold_generator::StrongholdGenerator::generate(int64_t worldSeed, Pos2D chunkPos) {
    return generate(worldSeed, chunkPos.x, chunkPos.z);
}

stronghold_generator::StrongholdGenerator::StrongholdGenerator() {
    this->resetPieces();
}

void stronghold_generator::StrongholdGenerator::resetPieces() {
    for (int i = 0; i < 11; i++) {
        this->piecePlaceCounts[i] = PiecePlaceCount::DEFAULT[i];
    }
    this->numAltarChests = 0;
    this->piecePlaceCountsSize = 11;
    this->imposedPiece = PieceType::NONE;
    this->totalWeight = 145;
    this->previousPiece = PieceType::NONE;
    this->piecesSize = 0;
    this->pendingPiecesSize = 0;
    this->portalRoomPiece = nullptr;
    this->generationStopped = false;
}

void stronghold_generator::StrongholdGenerator::updatePieceWeight() {
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

void stronghold_generator::StrongholdGenerator::onWeightedPiecePlaced(int piecePlaceCountIndex) {
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

BoundingBox stronghold_generator::StrongholdGenerator::createPieceBoundingBox(PieceType pieceType, int x, int y, int z, DIRECTION direction) {
    switch (pieceType) {
        case PieceType::STRAIGHT: {
            return BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 7, direction);
        }
        case PieceType::PRISON_HALL: {
            return BoundingBox::orientBox(x, y, z, -1, -1, 0, 9, 5, 11, direction);
        }
        case PieceType::RIGHT_TURN:
        case PieceType::LEFT_TURN:
            return BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 5, direction);
        case PieceType::ROOM_CROSSING: {
            return BoundingBox::orientBox(x, y, z, -4, -1, 0, 11, 7, 11, direction);
        }
        case PieceType::STRAIGHT_STAIRS_DOWN: {
            return BoundingBox::orientBox(x, y, z, -1, -7, 0, 5, 11, 8, direction);
        }
        case PieceType::STAIRS_DOWN: {
            return BoundingBox::orientBox(x, y, z, -1, -7, 0, 5, 11, 5, direction);
        }
        case PieceType::FIVE_CROSSING: {
            return BoundingBox::orientBox(x, y, z, -4, -3, 0, 10, 9, 11, direction);
        }
        case PieceType::CHEST_CORRIDOR: {
            return BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 7, direction);
        }
        case PieceType::LIBRARY: {
            return BoundingBox::orientBox(x, y, z, -4, -1, 0, 14, 11, 15, direction);
        }
        case PieceType::PORTAL_ROOM: {
            return BoundingBox::orientBox(x, y, z, -4, -1, 0, 11, 8, 16, direction);
        }
        default: {
            return BoundingBox::EMPTY;
        }
    }
}

stronghold_generator::Piece stronghold_generator::StrongholdGenerator::createPiece(PieceType pieceType, uint64_t* random,
                                                                                   DIRECTION direction, int depth, BoundingBox boundingBox) {
    switch (pieceType) {
        case PieceType::STRAIGHT: {
            int additionalData = 0;
            //random.nextInt(5);//this is java
            additionalData |= (nextInt(random, 2) == 0) << 0;
            additionalData |= (nextInt(random, 2) == 0) << 1;
            nextInt(random, 5);
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::PRISON_HALL: {
            nextInt(random, 5);
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::LEFT_TURN: {
            nextInt(random, 5);
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::RIGHT_TURN: {
            nextInt(random, 5);
            int additionalData = 0;
            return {PieceType::LEFT_TURN, depth, boundingBox, direction, additionalData};
        }
        case PieceType::ROOM_CROSSING: {
            //random.nextInt(5);//this is java
            int additionalData = nextInt(random, 5);
            nextInt(random, 5);
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::STRAIGHT_STAIRS_DOWN: {
            nextInt(random, 5);
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::STAIRS_DOWN: {
            nextInt(random, 5);
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::FIVE_CROSSING: {
            nextInt(random, 5);
            int additionalData = 0;
            additionalData |= (nextBoolean(random)) << 0;
            additionalData |= (nextBoolean(random)) << 1;
            additionalData |= (nextBoolean(random)) << 2;
            additionalData |= (nextInt(random, 3) > 0) << 3;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::CHEST_CORRIDOR: {
            this->altarChests[this->numAltarChests++] = &this->pieces[this->piecesSize];
            nextInt(random, 5);
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::LIBRARY: {
            nextInt(random, 5);
            int additionalData = 0;
            if (boundingBox.maxY > 6) {
                additionalData = 1;
            }
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        case PieceType::PORTAL_ROOM: {
            this->portalRoomPiece = &this->pieces[this->piecesSize];
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
        default: {
            int additionalData = 0;
            return {pieceType, depth, boundingBox, direction, additionalData};
        }
    }
}

void stronghold_generator::StrongholdGenerator::addPiece(Piece piece) {
    // printf("Adding piece at %i with rotation %i and depth %i\n", piece.boundingBox.minX, piece.boundingBox.minZ, piece.orientation, piece.depth);
    this->pendingPieces[this->pendingPiecesSize++] = this->piecesSize;
    this->pieces[this->piecesSize++] = piece;
}

bool stronghold_generator::StrongholdGenerator::tryAddPieceFromType(PieceType pieceType, uint64_t* random, int x, int y, int z, DIRECTION direction, int depth) {
    BoundingBox boundingBox = this->createPieceBoundingBox(pieceType, x, y, z, direction);
    if (!StrongholdGenerator::isOkBox(boundingBox) || this->findCollisionPiece(boundingBox) != nullptr) {
        if (pieceType == PieceType::LIBRARY) {
            boundingBox = BoundingBox::orientBox(x, y, z, -4, -1, 0, 14, 6, 15, direction);
            if(!StrongholdGenerator::isOkBox(boundingBox) || this->findCollisionPiece(boundingBox) != nullptr) {
                return false;
            }
        } else {
            return false;
        }
    }

    Piece piece = this->createPiece(pieceType, random, direction, depth, boundingBox);
    this->addPiece(piece);
    if (pieceType == PieceType::PORTAL_ROOM && this->generationStep == GenerationStep::PORTAL) {
        // printf("Stopping at portal 0\n");
        this->generationStopped = true;
    }

    return true;
}

bool stronghold_generator::StrongholdGenerator::generatePieceFromSmallDoor(uint64_t* random, int x, int y, int z, DIRECTION direction, int depth) {
    // printf("Adding at %i %i %i\n", x, y, z);
    
    if (this->generationStopped) {
        return false;
    }

    if (this->imposedPiece != PieceType::NONE) {
        // printf("Adding forced piece\n");
        PieceType theImposedPiece = this->imposedPiece;
        this->imposedPiece = PieceType::NONE;
        if (this->tryAddPieceFromType(theImposedPiece, random, x, y, z, direction, depth)) {
            // printf("Added forced piece\n");
            return true;
        }
    }

    int maxWeight = this->totalWeight;

    for (int attempt = 0; attempt < 5; attempt++) {
        int selectedWeight = nextInt(random, maxWeight);
        // printf("Selected weight %i %i\n", totalWeight, selectedWeight);
        for (int piecePlaceCountIndex = 0; piecePlaceCountIndex < this->piecePlaceCountsSize; piecePlaceCountIndex++) {
            PiecePlaceCount &piecePlaceCount = this->piecePlaceCounts[piecePlaceCountIndex];
            PieceType pieceType = piecePlaceCount.pieceType;
            int weight = PieceWeight::PIECE_WEIGHTS[static_cast<int>(pieceType)].weight;

            if ((selectedWeight -= weight) >= 0) continue;
            // printf("Trying %llu %i\n", random.seed, this->piecesSize);
            if (!piecePlaceCount.canPlace(depth) || pieceType == this->previousPiece) {
                // printf("Can't add\n");
                break;
            }
            if (!this->tryAddPieceFromType(pieceType, random, x, y, z, direction, depth)) {
                // printf("Failed\n");
                continue;
            }
            // printf("After place %llu %i\n", random.seed, this->piecesSize);
            this->onWeightedPiecePlaced(piecePlaceCountIndex);
            return true;
        }
    }

    BoundingBox boundingBox = BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, 4, direction);
    Piece *collidingPiece = this->findCollisionPiece(boundingBox);
    if (collidingPiece == nullptr) {
        return false;
    }
    if (collidingPiece->boundingBox.minY == boundingBox.minY) {
        for (int i = 3; i >= 1; --i) {
            boundingBox = BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, i - 1, direction);
            if (collidingPiece->boundingBox.intersects(boundingBox)) continue;

            boundingBox = BoundingBox::orientBox(x, y, z, -1, -1, 0, 5, 5, i, direction);
            if (boundingBox.minY > 1) {
                this->addPiece(Piece(PieceType::FILLER_CORRIDOR, depth, boundingBox, direction, 0));
                return true;
            }

            return false;
        }
    }

    return false;
}

void stronghold_generator::StrongholdGenerator::generateAndAddPiece(uint64_t* random, int x, int y, int z, DIRECTION direction, int depth) {
    if (depth > 50) {
        return;
    }

    if (abs(x - this->startX) <= 48 && abs(z - this->startZ) <= 48) {
        this->generatePieceFromSmallDoor(random, x, y, z, direction, depth + 1);
    } else {
        for (int piecePlaceCountIndex = 0; piecePlaceCountIndex < this->piecePlaceCountsSize; piecePlaceCountIndex++) {
            PiecePlaceCount &piecePlaceCount = this->piecePlaceCounts[piecePlaceCountIndex];
            if (piecePlaceCount.pieceType == PieceType::PORTAL_ROOM) {
                BoundingBox boundingBox = BoundingBox::orientBox(x, y, z, -4, -1, 0, 11, 8, 16, direction);
                if (StrongholdGenerator::isOkBox(boundingBox) && this->findCollisionPiece(boundingBox) == nullptr) {
                    this->onWeightedPiecePlaced(piecePlaceCountIndex);
                }
                break;
            }
        }
    }
}

void stronghold_generator::StrongholdGenerator::generateSmallDoorChildForward(Piece &piece, uint64_t* random, int n, int n2) {
    DIRECTION direction = piece.orientation;
    switch (direction) {
        case DIRECTION::NORTH: {
            return this->generateAndAddPiece(random, piece.boundingBox.minX + n, piece.boundingBox.minY + n2, piece.boundingBox.minZ - 1, direction, piece.depth);
        }
        case DIRECTION::SOUTH: {
            return this->generateAndAddPiece(random, piece.boundingBox.minX + n, piece.boundingBox.minY + n2, piece.boundingBox.maxZ + 1, direction, piece.depth);
        }
        case DIRECTION::WEST: {
            return this->generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY + n2, piece.boundingBox.minZ + n, direction, piece.depth);
        }
        case DIRECTION::EAST: {
            return this->generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY + n2, piece.boundingBox.minZ + n, direction, piece.depth);
        }
    }
}

void stronghold_generator::StrongholdGenerator::generateSmallDoorChildLeft(Piece &piece, uint64_t* random, int n, int n2) {
    DIRECTION direction = piece.orientation;
    switch (direction) {
        case DIRECTION::SOUTH:
        case DIRECTION::NORTH: {
            return this->generateAndAddPiece(random, piece.boundingBox.minX - 1, piece.boundingBox.minY + n, piece.boundingBox.minZ + n2, DIRECTION::WEST, piece.depth);
        }
        case DIRECTION::EAST:
        case DIRECTION::WEST: {
            return this->generateAndAddPiece(random, piece.boundingBox.minX + n2, piece.boundingBox.minY + n, piece.boundingBox.minZ - 1, DIRECTION::NORTH, piece.depth);
        }
    }
}

void stronghold_generator::StrongholdGenerator::generateSmallDoorChildRight(Piece &piece, uint64_t* random, int n, int n2) {
    DIRECTION direction = piece.orientation;
    switch (direction) {
        case DIRECTION::SOUTH:
        case DIRECTION::NORTH: {
            return this->generateAndAddPiece(random, piece.boundingBox.maxX + 1, piece.boundingBox.minY + n, piece.boundingBox.minZ + n2, DIRECTION::EAST, piece.depth);
        }
        case DIRECTION::EAST:
        case DIRECTION::WEST: {
            return this->generateAndAddPiece(random, piece.boundingBox.minX + n2, piece.boundingBox.minY + n, piece.boundingBox.maxZ + 1, DIRECTION::SOUTH, piece.depth);
        }
    }
}

void stronghold_generator::StrongholdGenerator::addChildren(Piece &piece, uint64_t* random) {
    switch (piece.type) {
        case PieceType::STRAIGHT: {
            this->generateSmallDoorChildForward(piece, random, 1, 1);
            if ((piece.additionalData & 1) != 0) {
                this->generateSmallDoorChildLeft(piece, random, 1, 2);
            }
            if ((piece.additionalData & 2) != 0) {
                this->generateSmallDoorChildRight(piece, random, 1, 2);
            }
        } break;
        case PieceType::PRISON_HALL: {
            this->generateSmallDoorChildForward(piece, random, 1, 1);
        } break;
        case PieceType::LEFT_TURN: {
            DIRECTION direction = piece.orientation;
            if (direction == DIRECTION::NORTH || direction == DIRECTION::EAST) {
                this->generateSmallDoorChildLeft(piece, random, 1, 1);
            } else {
                this->generateSmallDoorChildRight(piece, random, 1, 1);
            }
        } break;
        case PieceType::RIGHT_TURN: {
            DIRECTION direction = piece.orientation;
            if (direction == DIRECTION::NORTH || direction == DIRECTION::EAST) {
                this->generateSmallDoorChildRight(piece, random, 1, 1);
            } else {
                this->generateSmallDoorChildLeft(piece, random, 1, 1);
            }
        } break;
        case PieceType::ROOM_CROSSING: {
            this->generateSmallDoorChildForward(piece, random, 4, 1);
            this->generateSmallDoorChildLeft(piece, random, 1, 4);
            this->generateSmallDoorChildRight(piece, random, 1, 4);
        } break;
        case PieceType::STRAIGHT_STAIRS_DOWN: {
            this->generateSmallDoorChildForward(piece, random, 1, 1);
        } break;
        case PieceType::STAIRS_DOWN: {
            if (piece.additionalData != 0) {
                this->imposedPiece = PieceType::FIVE_CROSSING;
            }
            this->generateSmallDoorChildForward(piece, random, 1, 1);
        } break;
        case PieceType::FIVE_CROSSING: {
            int n = 3;
            int n2 = 5;
            DIRECTION direction = piece.orientation;
            if (direction == DIRECTION::WEST || direction == DIRECTION::NORTH) {
                n = 8 - n;
                n2 = 8 - n2;
            }
            this->generateSmallDoorChildForward(piece, random, 5, 1);
            if ((piece.additionalData & 1) != 0) {
                this->generateSmallDoorChildLeft(piece, random, n, 1);
            }
            if ((piece.additionalData & 2) != 0) {
                this->generateSmallDoorChildLeft(piece, random, n2, 7);
            }
            if ((piece.additionalData & 4) != 0) {
                this->generateSmallDoorChildRight(piece, random, n, 1);
            }
            if ((piece.additionalData & 8) != 0) {
                this->generateSmallDoorChildRight(piece, random, n2, 7);
            }
        } break;
        case PieceType::CHEST_CORRIDOR: {
            this->generateSmallDoorChildForward(piece, random, 1, 1);
        } break;
        case PieceType::FILLER_CORRIDOR:
        case PieceType::PORTAL_ROOM:
        case PieceType::LIBRARY:
        case PieceType::NONE:
           break;
    }
}

stronghold_generator::Piece* stronghold_generator::StrongholdGenerator::findCollisionPiece(BoundingBox &boundingBox) {
    for (int i = 0; i < this->piecesSize; i++) {
        if (this->pieces[i].boundingBox.intersects(boundingBox)) {
            return &this->pieces[i];
        }
    }

    return nullptr;
}

bool stronghold_generator::StrongholdGenerator::isOkBox(BoundingBox &boundingBox) {
    return boundingBox.minY > 10;
}
