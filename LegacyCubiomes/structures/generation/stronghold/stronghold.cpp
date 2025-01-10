#include <cmath>

#include "LegacyCubiomes/utils/rng.hpp"
#include "stronghold.hpp"


namespace gen {

    const std::map<PieceType, std::string> Stronghold::PieceTypeName = {
            {PieceType::Stronghold_NONE, "NONE -> something went wrong"},
            {PieceType::Stronghold_Straight, "STRAIGHT"},
            {PieceType::Stronghold_PrisonHall, "PRISON_HALL"},
            {PieceType::Stronghold_LeftTurn, "LEFT_TURN"},
            {PieceType::Stronghold_RightTurn, "RIGHT_TURN"},
            {PieceType::Stronghold_RoomCrossing, "ROOM_CROSSING"},
            {PieceType::Stronghold_StraightStairsDown, "STRAIGHT_STAIRS_DOWN"},
            {PieceType::Stronghold_StairsDown, "STAIRS_DOWN"},
            {PieceType::Stronghold_FiveCrossing, "FIVE_CROSSING"},
            {PieceType::Stronghold_ChestCorridor, "CHEST_CORRIDOR"},
            {PieceType::Stronghold_Library, "LIBRARY"},
            {PieceType::Stronghold_PortalRoom, "PORTAL_ROOM"},
            {PieceType::Stronghold_FillerCorridor, "FILLER_CORRIDOR"}};

    const PieceWeight Stronghold::PIECE_WEIGHTS[12] = {{0, 0, 0}, // none
                                                       {40, 0, 0}, {5, 5, 0},  {20, 0, 0}, {20, 0, 0},
                                                       {10, 6, 0}, {5, 5, 0},  {5, 5, 0},  {5, 4, 0},
                                                       {5, 4, 0},  {10, 2, 5}, {20, 1, 6}};

    const Stronghold::PiecePlaceCount Stronghold::PIECE_PLACE_COUNT_DEFAULT[11] = {
            {PieceType::Stronghold_Straight, 0},      {PieceType::Stronghold_PrisonHall, 0},
            {PieceType::Stronghold_LeftTurn, 0},      {PieceType::Stronghold_RightTurn, 0},
            {PieceType::Stronghold_RoomCrossing, 0},  {PieceType::Stronghold_StraightStairsDown, 0},
            {PieceType::Stronghold_StairsDown, 0},    {PieceType::Stronghold_FiveCrossing, 0},
            {PieceType::Stronghold_ChestCorridor, 0}, {PieceType::Stronghold_Library, 0},
            {PieceType::Stronghold_PortalRoom, 0}};

    Stronghold::Stronghold() { resetPieces(); }


    void Stronghold::generate(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        rng = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        rng.advance();
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;

        do {
            resetPieces();

            // creates starting staircase
            FACING direction = FACING_HORIZONTAL[rng.nextInt(4)];
            BoundingBox stairsBoundingBox = Piece::makeBoundingBox(startX, 64, startZ, direction, 5, 11, 5);

            pieceArray[pieceArraySize++] = {PieceType::Stronghold_StairsDown, 0, stairsBoundingBox, direction, 1};

            // this only adds the 5-crossing room
            addChildren(pieceArray[0]);

            while (pendingPiecesArraySize != 0) {
                c_int i = rng.nextInt(pendingPiecesArraySize);
                Piece& piece = pieceArray[pendingPieceArray[i]];
                pendingPiecesArraySize--;

                const size_t bytesToShift = (pendingPiecesArraySize - i) * sizeof(pendingPieceArray[0]);
                memmove(&pendingPieceArray[i], &pendingPieceArray[i + 1], bytesToShift);
                // for (int j = i; j < pendingPiecesArraySize; j++) {
                //     pendingPieceArray[j] = pendingPieceArray[j + 1];
                // }
                addChildren(piece);


                switch (generationStep) {
                    case GenerationStep::PORTAL:
                        if (portalRoomPiece != nullptr) return;
                    case GenerationStep::LAYOUT:
                        if (generationStopped && portalRoomPiece != nullptr) return;
                    default:
                        break;
                }
            }

            if (portalRoomPiece != nullptr && generationStep == GenerationStep::LAYOUT) { return; }

            structureBoundingBox = BoundingBox::EMPTY;
            for (int index = 0; index < pieceArraySize; index++) { structureBoundingBox.encompass(pieceArray[index]); }

            constexpr int i = 63 - 10;
            int j = structureBoundingBox.getYSize() + 1;
            if (j < i) { j += rng.nextInt(i - j); }

            c_int k = j - structureBoundingBox.maxY;
            structureBoundingBox.offsetY(k);
            for (int index = 0; index < pieceArraySize; index++) { pieceArray[index].offsetY(k); }
        } while (portalRoomPiece == nullptr);
    }


    void Stronghold::resetPieces() {
        /*memcpy(&piecePlaceCounts,
               &PIECE_PLACE_COUNT_DEFAULT,
               11 * sizeof(PIECE_PLACE_COUNT_DEFAULT));*/
        for (int i = 0; i < 11; i++) { piecePlaceCounts[i] = PIECE_PLACE_COUNT_DEFAULT[i]; }

        generationStopped = false;
        forcedPiece = PieceType::Stronghold_NONE;
        previousPiece = PieceType::Stronghold_NONE;
        pieceArraySize = 0;
        pendingPiecesArraySize = 0;
        portalRoomPiece = nullptr;
        altarChestArraySize = 0;
        piecePlaceCountsSize = 11;
        totalWeight = 145;
    }


    void Stronghold::onWeightedPiecePlaced(c_int piecePlaceCountIndex) {
        PiecePlaceCount& piecePlaceCount = piecePlaceCounts[piecePlaceCountIndex];
        piecePlaceCount.placeCount++;

        previousPiece = piecePlaceCount.pieceType;
        if (piecePlaceCount.isValid()) return;

        totalWeight -= PiecePlaceCount::get(piecePlaceCount.pieceType)->getWeight();

        piecePlaceCountsSize--;

        for (int i = piecePlaceCountIndex; i < piecePlaceCountsSize; i++) {
            piecePlaceCounts[i] = piecePlaceCounts[i + 1];
        }

        // function originally called "updatePieceWeight"
        for (int index = 0; index < piecePlaceCountsSize; index++) {
            c_auto [pieceType, placeCount] = piecePlaceCounts[index];

            if (c_auto [weight, maxPlaceCount, minDepth] = PIECE_WEIGHTS[pieceType];
                maxPlaceCount > 0 && placeCount < maxPlaceCount)
                return;
        }

        generationStopped = true;
    }


    BoundingBox Stronghold::createPieceBoundingBox(const PieceType pieceType, const Pos3D& pos,
                                                   const FACING facing) {
        switch (pieceType) {
            case PieceType::Stronghold_Straight:
                return BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 7, facing);
            case PieceType::Stronghold_PrisonHall:
                return BoundingBox::orientBox(pos, -1, -1, 0, 9, 5, 11, facing);
            case PieceType::Stronghold_RightTurn:
            case PieceType::Stronghold_LeftTurn:
                return BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 5, facing);
            case PieceType::Stronghold_RoomCrossing:
                return BoundingBox::orientBox(pos, -4, -1, 0, 11, 7, 11, facing);
            case PieceType::Stronghold_StraightStairsDown:
                return BoundingBox::orientBox(pos, -1, -7, 0, 5, 11, 8, facing);
            case PieceType::Stronghold_StairsDown:
                return BoundingBox::orientBox(pos, -1, -7, 0, 5, 11, 5, facing);
            case PieceType::Stronghold_FiveCrossing:
                return BoundingBox::orientBox(pos, -4, -3, 0, 10, 9, 11, facing);
            case PieceType::Stronghold_ChestCorridor:
                return BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 7, facing);
            case PieceType::Stronghold_Library:
                return BoundingBox::orientBox(pos, -4, -1, 0, 14, 11, 15, facing);
            case PieceType::Stronghold_PortalRoom:
                return BoundingBox::orientBox(pos, -4, -1, 0, 11, 8, 16, facing);
            default:
                return BoundingBox::EMPTY;
        }
    }


    void Stronghold::createPiece(PieceType pieceType, FACING direction, i8 depth, BoundingBox boundingBox) {
        int additionalData = 0;

        switch (pieceType) {
            case PieceType::Stronghold_RightTurn:
                rng.nextInt(5);
                pieceArray[pieceArraySize++] = {PieceType::Stronghold_LeftTurn,
                                                depth, boundingBox,
                                                direction, additionalData};
                return;
            case PieceType::Stronghold_Straight:
                // rng.nextInt(5); // this is java
                additionalData |= (rng.nextInt(2) == 0) << 0;
                additionalData |= (rng.nextInt(2) == 0) << 1;
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_PrisonHall:
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_LeftTurn:
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_RoomCrossing:
                // rng.nextInt(5); // this is java
                additionalData = rng.nextInt(5);
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_StraightStairsDown:
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_StairsDown:
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_FiveCrossing:
                rng.nextInt(5);
                additionalData |= (rng.nextBoolean()) << 0;
                additionalData |= (rng.nextBoolean()) << 1;
                additionalData |= (rng.nextBoolean()) << 2;
                additionalData |= (rng.nextInt(3) > 0) << 3;
                break;
            case PieceType::Stronghold_ChestCorridor:
                altarChestsArray[altarChestArraySize++] = &pieceArray[pieceArraySize];
                rng.nextInt(5);
                break;
            case PieceType::Stronghold_Library:
                rng.nextInt(5);
                additionalData = boundingBox.maxY > 6;
                break;
            case PieceType::Stronghold_PortalRoom:
                portalRoomPiece = &pieceArray[pieceArraySize];
                break;
            default:
                break;
        }

        pieceArray[pieceArraySize++] = {pieceType, depth, boundingBox, direction, additionalData};
    }


    bool Stronghold::tryAddPieceFromType(const PieceType pieceType, const Pos3D& pos, const FACING direction,
                                         c_i8 depth) {
        BoundingBox bBox = createPieceBoundingBox(pieceType, pos, direction);
        if (!isOkBox(bBox) || collidesWithPiece(bBox)) {
            if (pieceType == PieceType::Stronghold_Library) {
                bBox = BoundingBox::orientBox(pos, -4, -1, 0, 14, 6, 15, direction);
                if (!isOkBox(bBox) || collidesWithPiece(bBox)) return false;
            } else {
                return false;
            }
        }

        pendingPieceArray[pendingPiecesArraySize++] = pieceArraySize;
        createPiece(pieceType, direction, depth, bBox);

        if (pieceType == PieceType::Stronghold_PortalRoom && generationStep == GenerationStep::PORTAL) generationStopped = true;


        return true;
    }

    bool Stronghold::genPieceFromSmallDoor(const Pos3D& pos, FACING direction, i8 depth) {
        if EXPECT_FALSE (generationStopped) return false;

        if EXPECT_FALSE (forcedPiece != PieceType::Stronghold_NONE) {
            c_bool canAdd = tryAddPieceFromType(forcedPiece, pos, direction, depth);
            forcedPiece = PieceType::Stronghold_NONE;
            if (canAdd) return true;
        }

        c_int maxWeight = totalWeight;

        // TODO: can be rewritten to use a cumulative table, but would require
        // TODO: recalculating the table everytime an entry is removed.

        for (int attempt = 0; attempt < 5; attempt++) {
            int selectedWeight = rng.nextInt(maxWeight);
            // printf("Selected weight %i %i", totalWeight, selectedWeight);
            for (int index = 0; index < piecePlaceCountsSize; index++) {
                PiecePlaceCount& piecePlaceCount = piecePlaceCounts[index];
                const PieceType pieceType = piecePlaceCount.pieceType;
                c_int weight = PiecePlaceCount::get(pieceType)->getWeight();

                selectedWeight -= weight;
                if (selectedWeight >= 0) continue;

                if (pieceType == previousPiece) break;
                if (!piecePlaceCount.canPlace(depth)) break;

                if (!tryAddPieceFromType(pieceType, pos, direction, depth)) continue;

                onWeightedPiecePlaced(index);
                return true;
            }
        }

        BoundingBox boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 4, direction);
        const Piece* collidingPiece = findCollisionPiece(boundingBox);
        if (collidingPiece == nullptr) return false;

        // if (collidingPiece->minY != boundingBox.minY) return false;
        // doing it this way gets rid of an if statement

        for (int i = 3 * (collidingPiece->minY == boundingBox.minY); i >= 1; --i) {
            boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, i - 1, direction);
            if (collidingPiece->intersects(boundingBox)) continue;

            boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, i, direction);
            if (boundingBox.minY <= 1) return false;

            pendingPieceArray[pendingPiecesArraySize++] = pieceArraySize;
            pieceArray[pieceArraySize++] = {PieceType::Stronghold_FillerCorridor, depth, boundingBox, direction, 0};

            return true;
        }

        return false;
    }

    void Stronghold::genAndAddPiece(const Pos3D& pos, const FACING direction, c_i8 depth) {
        if (depth > 50) return;

        if (abs(pos.getX() - startX) <= 48 && abs(pos.getZ() - startZ) <= 48) {
            genPieceFromSmallDoor(pos, direction, static_cast<i8>(depth + 1));
            return;
        }

        for (int index = 0; index < piecePlaceCountsSize; index++) {
            if (c_auto& [pieceType, placeCount] = piecePlaceCounts[index]; pieceType == PieceType::Stronghold_PortalRoom) {
                if (const BoundingBox bBox = BoundingBox::orientBox(pos, -4, -1, 0, 11, 8, 16, direction);
                    isOkBox(bBox) && findCollisionPiece(bBox) == nullptr) {
                    onWeightedPiecePlaced(index);
                }
                break;
            }
        }
    }


    Piece* Stronghold::findCollisionPiece(const BoundingBox& boundingBox) {
        for (int i = 0; i < pieceArraySize; i++) {
            if (pieceArray[i].intersects(boundingBox)) { return &pieceArray[i]; }
        }
        return nullptr;
    }


    bool Stronghold::collidesWithPiece(const BoundingBox& boundingBox) {
        return findCollisionPiece(boundingBox) != nullptr;
    }


    bool inline Stronghold::isOkBox(const BoundingBox& boundingBox) { return boundingBox.minY > 10; }


    void Stronghold::genSmallDoorChildForward(const Piece& piece, c_int n, c_int n2) {
        switch (const FACING direction = piece.orientation) {
            case FACING::NORTH:
                return genAndAddPiece({piece.minX + n, piece.minY + n2, piece.minZ - 1}, direction, piece.depth);
            case FACING::SOUTH:
                return genAndAddPiece({piece.minX + n, piece.minY + n2, piece.maxZ + 1}, direction, piece.depth);
            case FACING::WEST:
                return genAndAddPiece({piece.minX - 1, piece.minY + n2, piece.minZ + n}, direction, piece.depth);
            case FACING::EAST:
                return genAndAddPiece({piece.maxX + 1, piece.minY + n2, piece.minZ + n}, direction, piece.depth);
            case FACING::NONE:
            case FACING::DOWN:
            case FACING::UP:
                break;
        }
    }


    void Stronghold::genSmallDoorChildLeft(const Piece& piece, c_int n, c_int n2) {
        switch (piece.orientation) {
            case FACING::SOUTH:
            case FACING::NORTH:
                genAndAddPiece({piece.minX - 1, piece.minY + n, piece.minZ + n2}, FACING::WEST, piece.depth);
                break;
            case FACING::EAST:
            case FACING::WEST:
                genAndAddPiece({piece.minX + n2, piece.minY + n, piece.minZ - 1}, FACING::NORTH, piece.depth);
                break;
            case FACING::NONE:
            case FACING::DOWN:
            case FACING::UP:
                break;
        }
    }

    void Stronghold::genSmallDoorChildRight(const Piece& piece, c_int n, c_int n2) {
        switch (piece.orientation) {
            case FACING::SOUTH:
            case FACING::NORTH:
                genAndAddPiece({piece.maxX + 1, piece.minY + n, piece.minZ + n2}, FACING::EAST, piece.depth);
                break;
            case FACING::EAST:
            case FACING::WEST:
                genAndAddPiece({piece.minX + n2, piece.minY + n, piece.maxZ + 1}, FACING::SOUTH, piece.depth);
                break;
            case FACING::NONE:
            case FACING::DOWN:
            case FACING::UP:
                break;
        }
    }


    void Stronghold::addChildren(const Piece& piece) {
        switch (piece.type) {
            case PieceType::Stronghold_Straight:
                genSmallDoorChildForward(piece, 1, 1);
                if ((piece.additionalData & 1) != 0) genSmallDoorChildLeft(piece, 1, 2);
                if ((piece.additionalData & 2) != 0) genSmallDoorChildRight(piece, 1, 2);
                break;
            case PieceType::Stronghold_PrisonHall:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PieceType::Stronghold_LeftTurn: {
                if (const FACING direction = piece.orientation;
                    direction == FACING::NORTH || direction == FACING::EAST) {
                    genSmallDoorChildLeft(piece, 1, 1);
                } else {
                    genSmallDoorChildRight(piece, 1, 1);
                }
                break;
            }
            case PieceType::Stronghold_RightTurn: {
                if (const FACING direction = piece.orientation;
                    direction == FACING::NORTH || direction == FACING::EAST) {
                    genSmallDoorChildRight(piece, 1, 1);
                } else {
                    genSmallDoorChildLeft(piece, 1, 1);
                }
                break;
            }
            case PieceType::Stronghold_RoomCrossing:
                genSmallDoorChildForward(piece, 4, 1);
                genSmallDoorChildLeft(piece, 1, 4);
                genSmallDoorChildRight(piece, 1, 4);
                break;
            case PieceType::Stronghold_StraightStairsDown:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PieceType::Stronghold_StairsDown: {
                if (piece.additionalData != 0) forcedPiece = PieceType::Stronghold_FiveCrossing;
                genSmallDoorChildForward(piece, 1, 1);
                break;
            }
            case PieceType::Stronghold_FiveCrossing: {
                const FACING o = piece.orientation;
                // 3 and 5 or 5 and 3
                c_int n = 5 - 2 * (o == FACING::EAST || o == FACING::SOUTH);
                c_int n2 = 8 - n;
                genSmallDoorChildForward(piece, 5, 1);
                if ((piece.additionalData & 1) != 0) genSmallDoorChildLeft(piece, n, 1);
                if ((piece.additionalData & 2) != 0) genSmallDoorChildLeft(piece, n2, 7);
                if ((piece.additionalData & 4) != 0) genSmallDoorChildRight(piece, n, 1);
                if ((piece.additionalData & 8) != 0) genSmallDoorChildRight(piece, n2, 7);
                break;
            }
            case PieceType::Stronghold_ChestCorridor:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PieceType::Stronghold_FillerCorridor:
            case PieceType::Stronghold_PortalRoom:
            case PieceType::Stronghold_Library:
            case PieceType::Stronghold_NONE:
                break;
        }
    }
} // namespace generation