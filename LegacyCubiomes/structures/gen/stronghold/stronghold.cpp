#include <cmath>

#include "LegacyCubiomes/utils/rng.hpp"
#include "stronghold.hpp"

#include "LegacyCubiomes/chunk_generator/Chunk.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/include.hpp"
#include "lce/blocks/block_states.hpp"


using namespace lce::blocks;
using namespace lce::blocks::states;


namespace gen {

    MU std::string Stronghold::PIECE_TYPE_NAMES[13] = {
            "NONE",                 // Stronghold_NONE
            "STRAIGHT",             // Stronghold_Straight
            "PRISON_HALL",          // Stronghold_PrisonHall
            "LEFT_TURN",            // Stronghold_LeftTurn
            "RIGHT_TURN",           // Stronghold_RightTurn
            "ROOM_CROSSING",        // Stronghold_RoomCrossing
            "STRAIGHT_STAIRS_DOWN", // Stronghold_StraightStairsDown
            "STAIRS_DOWN",          // Stronghold_StairsDown
            "FIVE_CROSSING",        // Stronghold_FiveCrossing
            "CHEST_CORRIDOR",       // Stronghold_ChestCorridor
            "LIBRARY",              // Stronghold_Library
            "PORTAL_ROOM",          // Stronghold_PortalRoom
            "FILLER_CORRIDOR",      // Stronghold_FillerCorridor
    };


    MU std::string Stronghold::getPieceName(const PieceType pieceType) {
        if (pieceType < 13) {
            return PIECE_TYPE_NAMES[pieceType];
        }
        return "";
    }


    const PieceWeight Stronghold::PIECE_WEIGHTS[12] = {{0, 0, 0}, // none
                                                       {40, 0, 0}, {5, 5, 0},  {20, 0, 0}, {20, 0, 0},
                                                       {10, 6, 0}, {5, 5, 0},  {5, 5, 0},  {5, 4, 0},
                                                       {5, 4, 0},  {10, 2, 5}, {20, 1, 6}};

    const Stronghold::PiecePlaceCount Stronghold::PIECE_PLACE_COUNT_DEFAULT[11] = {
            {PT_Stronghold_Straight, 0},
            {PT_Stronghold_PrisonHall, 0},
            {PT_Stronghold_LeftTurn, 0},
            {PT_Stronghold_RightTurn, 0},
            {PT_Stronghold_RoomCrossing, 0},
            {PT_Stronghold_StraightStairsDown, 0},
            {PT_Stronghold_StairsDown, 0},
            {PT_Stronghold_FiveCrossing, 0},
            {PT_Stronghold_ChestCorridor, 0},
            {PT_Stronghold_Library, 0},
            {PT_Stronghold_PortalRoom, 0}};

    Stronghold::Stronghold() { reset(); }

    Stronghold::~Stronghold() {
        this->deletePieces();
    }

    void Stronghold::deletePieces() {
        // for (int i = 0; i < pieceArraySize; i++) {
        //     delete pieceArray[i];
        // }
    }


    /**
      * \n
      * Overload function. Generates a stronghold with the given seed and chunk coordinates.
      * @param worldSeed the seed
      * @param chunkPos coordinates of the chunk
      */
    void Stronghold::generate(c_i64 worldSeed, const Pos2D chunkPos) {
        return generate(worldSeed, chunkPos.x, chunkPos.z);
    }


    /**
     * \n
     * Generates a stronghold with the given seed and chunk coordinates.
     * @param worldSeed the seed
     * @param chunkX x coordinate of the chunk
     * @param chunkZ z coordinate of the chunk
     */
    void Stronghold::generate(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        rng = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        rng.advance();
        startPos.x = (chunkX << 4) + 2;
        startPos.z = (chunkZ << 4) + 2;

        do {
            reset();

            // creates starting staircase
            enumFacing direction = FACING_HORIZONTAL[rng.nextInt(4)];
            BoundingBox stairsBoundingBox = StructureComponent::makeBoundingBox(startPos.x, 64, startPos.z, direction, 5, 11, 5);

            pieceArray[pieceArraySize++] = StructureComponent(PT_Stronghold_StairsDown, 0, stairsBoundingBox, direction, 1);

            // this only adds the 5-crossing room
            addChildren(pieceArray[0]);

            while (pendingPiecesArraySize != 0) {
                c_int i = rng.nextInt(pendingPiecesArraySize);
                StructureComponent& piece = pieceArray[pendingPieceArray[i]];
                pendingPiecesArraySize--;

                // shift all the pieces over
                const size_t bytesToShift = (pendingPiecesArraySize - i) * sizeof(pendingPieceArray[0]);
                memmove(&pendingPieceArray[i], &pendingPieceArray[i + 1], bytesToShift);
                // for (int j = i; j < pendingPiecesArraySize; j++) {
                //     pendingPieceArray[j] = pendingPieceArray[j + 1];
                // }
                addChildren(piece);


                switch (generationStep) {
                    case GS_Stronghold_Portal:
                        if (portalRoomPiece != nullptr) return;
                    case GS_Stronghold_Layout:
                        if (generationStopped && portalRoomPiece != nullptr) return;
                    default:
                        break;
                }
            }

            if (generationStep == GS_Stronghold_Layout && portalRoomPiece != nullptr) { return; }

            // encompass structure bounding box
            structureBB = BoundingBox::EMPTY;
            for (int index = 0; index < pieceArraySize; index++) {
                structureBB.encompass(pieceArray[index]);
            }

            // find value to offset structure by "k"
            constexpr int i = 63 - 10;
            int j = structureBB.getYSize() + 1;
            if (j < i) { j += rng.nextInt(i - j); }
            c_int k = j - structureBB.maxY;

            // offset structure bounding box by "k"
            structureBB.offsetY(k);

            // offset all pieces by "k"
            for (int index = 0; index < pieceArraySize; index++) {
                pieceArray[index].offsetY(k);
            }


        } while (portalRoomPiece == nullptr);
    }


    void Stronghold::reset() {
        /*memcpy(&piecePlaceCounts,
               &PIECE_PLACE_COUNT_DEFAULT,
               11 * sizeof(PIECE_PLACE_COUNT_DEFAULT));*/
        for (int i = 0; i < 11; i++) { piecePlaceCounts[i] = PIECE_PLACE_COUNT_DEFAULT[i]; }
        this->deletePieces();

        generationStopped = false;
        forcedPiece = PT_Stronghold_NONE;
        previousPiece = PT_Stronghold_NONE;
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
        
        // TODO: whoever inlined this function, we better hope they handled the return correctly
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
                                                   const enumFacing facing) {
        switch (pieceType) {
            case PT_Stronghold_NONE:
                return BoundingBox::EMPTY;
            case PT_Stronghold_Straight:
                return BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 7, facing);
            case PT_Stronghold_PrisonHall:
                return BoundingBox::orientBox(pos, -1, -1, 0, 9, 5, 11, facing);
            case PT_Stronghold_LeftTurn:
            case PT_Stronghold_RightTurn:
                return BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 5, facing);
            case PT_Stronghold_RoomCrossing:
                return BoundingBox::orientBox(pos, -4, -1, 0, 11, 7, 11, facing);
            case PT_Stronghold_StraightStairsDown:
                return BoundingBox::orientBox(pos, -1, -7, 0, 5, 11, 8, facing);
            case PT_Stronghold_StairsDown:
                return BoundingBox::orientBox(pos, -1, -7, 0, 5, 11, 5, facing);
            case PT_Stronghold_FiveCrossing:
                return BoundingBox::orientBox(pos, -4, -3, 0, 10, 9, 11, facing);
            case PT_Stronghold_ChestCorridor:
                return BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 7, facing);
            case PT_Stronghold_Library:
                return BoundingBox::orientBox(pos, -4, -1, 0, 14, 11, 15, facing);
            case PT_Stronghold_PortalRoom:
                return BoundingBox::orientBox(pos, -4, -1, 0, 11, 8, 16, facing);
            default:
                return BoundingBox::EMPTY;
        }
    }


    void Stronghold::createPiece(PieceType pieceType, const enumFacing facing, i8 depth, BoundingBox boundingBox) {
        int additionalData = 0;

        switch (pieceType) {
            case PT_Stronghold_Straight:
                // rng.nextInt(5); // this is java
                additionalData |= (rng.nextInt(2) == 0) << 0;
                additionalData |= (rng.nextInt(2) == 0) << 1;
                additionalData |= rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_PrisonHall:
                additionalData |= rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_LeftTurn:
                additionalData |= rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_RightTurn:
                additionalData |= rng.nextInt(5) << 16;
                pieceArray[pieceArraySize++] = StructureComponent(PT_Stronghold_LeftTurn, depth, boundingBox, facing, additionalData);
                return;
            case PT_Stronghold_RoomCrossing:
                // rng.nextInt(5); // this is java
                additionalData |= rng.nextInt(5);
                additionalData |= rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_StraightStairsDown:
                additionalData |= rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_StairsDown:
                additionalData |= rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_FiveCrossing:
                additionalData |= rng.nextInt(5) << 16;
                additionalData |= rng.nextBoolean() << 0;
                additionalData |= rng.nextBoolean() << 1;
                additionalData |= rng.nextBoolean() << 2;
                additionalData |= (rng.nextInt(3) > 0) << 3;
                break;
            case PT_Stronghold_ChestCorridor:
                additionalData |= rng.nextInt(5) << 16;
                altarChestsArray[altarChestArraySize++] = &pieceArray[pieceArraySize];
                break;
            case PT_Stronghold_Library:
                additionalData |= rng.nextInt(5) << 16;
                additionalData |= boundingBox.maxY > 6 ? 1 : 0;
                break;
            case PT_Stronghold_PortalRoom:
                portalRoomPiece = &pieceArray[pieceArraySize];
                break;
            default:
                break;
        }

        pieceArray[pieceArraySize++] = StructureComponent(pieceType, depth, boundingBox, facing, additionalData);
    }


    bool Stronghold::tryAddPieceFromType(const PieceType pieceType, const Pos3D& pos, const enumFacing facing, c_i8 depth) {
        BoundingBox bBox = createPieceBoundingBox(pieceType, pos, facing);
        if (!isOkBox(bBox) || collidesWithPiece(bBox)) {
            if (pieceType == PT_Stronghold_Library) {
                bBox = BoundingBox::orientBox(pos, -4, -1, 0, 14, 6, 15, facing);
                if (!isOkBox(bBox) || collidesWithPiece(bBox)) return false;
            } else {
                return false;
            }
        }

        pendingPieceArray[pendingPiecesArraySize++] = pieceArraySize;
        createPiece(pieceType, facing, depth, bBox);

        if (generationStep == GS_Stronghold_Portal &&
            pieceType == PT_Stronghold_PortalRoom) generationStopped = true;


        return true;
    }

    bool Stronghold::genPieceFromSmallDoor(const Pos3D& pos, const enumFacing facing, const i8 depth) {
        if EXPECT_FALSE (generationStopped) return false;

        if EXPECT_FALSE (forcedPiece != PT_Stronghold_NONE) {
            c_bool canAdd = tryAddPieceFromType(forcedPiece, pos, facing, depth);
            forcedPiece = PT_Stronghold_NONE;
            if (canAdd) { return true; }
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

                if (pieceType == previousPiece) { break; }
                if (!piecePlaceCount.canPlace(depth)) { break; }

                if (!tryAddPieceFromType(pieceType, pos, facing, depth)) { continue; }

                onWeightedPiecePlaced(index);
                return true;
            }
        }

        BoundingBox boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, 4, facing);
        const Piece* collidingPiece = findCollisionPiece(boundingBox);
        if (collidingPiece == nullptr) { return false; }

        // if (collidingPiece->minY != boundingBox.minY) return false;
        // doing it this way gets rid of an if statement

        for (int i = 3 * (collidingPiece->minY == boundingBox.minY); i >= 1; --i) {
            boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, i - 1, facing);
            if (collidingPiece->intersects(boundingBox)) { continue; }

            boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, i, facing);
            if (boundingBox.minY <= 1) { return false; }

            pendingPieceArray[pendingPiecesArraySize++] = pieceArraySize;
            pieceArray[pieceArraySize++] = StructureComponent(PT_Stronghold_FillerCorridor, depth, boundingBox, facing, 0);

            return true;
        }

        return false;
    }

    void Stronghold::genAndAddPiece(const Pos3D& pos, const enumFacing facing, c_i8 depth) {
        if (depth > 50) return;

        if (abs(pos.getX() - startPos.x) <= 48 && abs(pos.getZ() - startPos.z) <= 48) {
            genPieceFromSmallDoor(pos, facing, static_cast<i8>(depth + 1));
            return;
        }

        for (int index = 0; index < piecePlaceCountsSize; index++) {
            if (c_auto& [pieceType, placeCount] = piecePlaceCounts[index];
                pieceType == PT_Stronghold_PortalRoom) {
                if (const BoundingBox bBox = BoundingBox::orientBox(pos, -4, -1, 0, 11, 8, 16, facing);
                    isOkBox(bBox) && findCollisionPiece(bBox) == nullptr) {
                    onWeightedPiecePlaced(index);
                }
                break;
            }
        }
    }


    StructureComponent* Stronghold::findCollisionPiece(const BoundingBox& boundingBox) {
        for (int i = 0; i < pieceArraySize; i++) {
            if (pieceArray[i].intersects(boundingBox)) { return &pieceArray[i]; }
        }
        return nullptr;
    }


    bool Stronghold::collidesWithPiece(const BoundingBox& boundingBox) {
        return findCollisionPiece(boundingBox) != nullptr;
    }


    bool inline Stronghold::isOkBox(const BoundingBox& boundingBox) { return boundingBox.minY > 10; }


    void Stronghold::genSmallDoorChildForward(const StructureComponent& piece, c_int n, c_int n2) {
        switch (piece.facing) {
            case enumFacing::DOWN:
            case enumFacing::UP:
                break;
            case enumFacing::NORTH:
                return genAndAddPiece({piece.minX + n, piece.minY + n2, piece.minZ - 1}, enumFacing::NORTH, piece.depth);
            case enumFacing::SOUTH:
                return genAndAddPiece({piece.minX + n, piece.minY + n2, piece.maxZ + 1}, enumFacing::SOUTH, piece.depth);
            case enumFacing::WEST:
                return genAndAddPiece({piece.minX - 1, piece.minY + n2, piece.minZ + n}, enumFacing::WEST, piece.depth);
            case enumFacing::EAST:
                return genAndAddPiece({piece.maxX + 1, piece.minY + n2, piece.minZ + n}, enumFacing::EAST, piece.depth);
        }
    }


    void Stronghold::genSmallDoorChildLeft(const StructureComponent& piece, c_int n, c_int n2) {
        switch (piece.facing) {
            case enumFacing::DOWN:
            case enumFacing::UP:
                break;
            case enumFacing::SOUTH:
            case enumFacing::NORTH:
                genAndAddPiece({piece.minX - 1, piece.minY + n, piece.minZ + n2}, enumFacing::WEST, piece.depth);
                break;
            case enumFacing::EAST:
            case enumFacing::WEST:
                genAndAddPiece({piece.minX + n2, piece.minY + n, piece.minZ - 1}, enumFacing::NORTH, piece.depth);
                break;
        }
    }

    void Stronghold::genSmallDoorChildRight(const StructureComponent& piece, c_int n, c_int n2) {
        switch (piece.facing) {
            case enumFacing::DOWN:
            case enumFacing::UP:
                break;
            case enumFacing::SOUTH:
            case enumFacing::NORTH:
                genAndAddPiece({piece.maxX + 1, piece.minY + n, piece.minZ + n2}, enumFacing::EAST, piece.depth);
                break;
            case enumFacing::EAST:
            case enumFacing::WEST:
                genAndAddPiece({piece.minX + n2, piece.minY + n, piece.maxZ + 1}, enumFacing::SOUTH, piece.depth);
                break;
        }
    }


    void Stronghold::addChildren(const StructureComponent& piece) {
        switch (piece.type) {
            case PT_Stronghold_NONE:
                break;
            case PT_Stronghold_Straight:
                genSmallDoorChildForward(piece, 1, 1);
                if ((piece.data & 1) != 0) genSmallDoorChildLeft(piece, 1, 2);
                if ((piece.data & 2) != 0) genSmallDoorChildRight(piece, 1, 2);
                break;
            case PT_Stronghold_PrisonHall:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PT_Stronghold_LeftTurn: {
                if (const enumFacing direction = piece.facing;
                    direction == enumFacing::NORTH || direction == enumFacing::EAST) {
                    genSmallDoorChildLeft(piece, 1, 1);
                } else {
                    genSmallDoorChildRight(piece, 1, 1);
                }
                break;
            }
            case PT_Stronghold_RightTurn: {
                if (const enumFacing direction = piece.facing;
                    direction == enumFacing::NORTH || direction == enumFacing::EAST) {
                    genSmallDoorChildRight(piece, 1, 1);
                } else {
                    genSmallDoorChildLeft(piece, 1, 1);
                }
                break;
            }
            case PT_Stronghold_RoomCrossing:
                genSmallDoorChildForward(piece, 4, 1);
                genSmallDoorChildLeft(piece, 1, 4);
                genSmallDoorChildRight(piece, 1, 4);
                break;
            case PT_Stronghold_StraightStairsDown:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PT_Stronghold_StairsDown: {
                if ((piece.data & 7) != 0) { forcedPiece = PT_Stronghold_FiveCrossing; }
                genSmallDoorChildForward(piece, 1, 1);
                break;
            }
            case PT_Stronghold_FiveCrossing: {
                const enumFacing o = piece.facing;
                // 3 and 5 or 5 and 3
                c_int n = 5 - 2 * (o == enumFacing::EAST || o == enumFacing::SOUTH);
                c_int n2 = 8 - n;
                genSmallDoorChildForward(piece, 5, 1);
                if ((piece.data & 1) != 0) { genSmallDoorChildLeft(piece, n, 1); }
                if ((piece.data & 2) != 0) { genSmallDoorChildLeft(piece, n2, 7); }
                if ((piece.data & 4) != 0) { genSmallDoorChildRight(piece, n, 1); }
                if ((piece.data & 8) != 0) { genSmallDoorChildRight(piece, n2, 7); }
                break;
            }
            case PT_Stronghold_ChestCorridor:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PT_Stronghold_FillerCorridor:
            case PT_Stronghold_PortalRoom:
            case PT_Stronghold_Library:
                break;
        }
    }
} // namespace gen

