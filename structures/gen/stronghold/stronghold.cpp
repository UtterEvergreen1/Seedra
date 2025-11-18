#include <cmath>

#include "stronghold.hpp"

#include "common/rng.hpp"

#include "terrain/Chunk.hpp"
#include "terrain/World.hpp"

#include "lce/blocks/blockStates.hpp"


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
            {0, PT_Stronghold_Straight},
            {0, PT_Stronghold_PrisonHall},
            {0, PT_Stronghold_LeftTurn},
            {0, PT_Stronghold_RightTurn},
            {0, PT_Stronghold_RoomCrossing},
            {0, PT_Stronghold_StraightStairsDown},
            {0, PT_Stronghold_StairsDown},
            {0, PT_Stronghold_FiveCrossing},
            {0, PT_Stronghold_ChestCorridor},
            {0, PT_Stronghold_Library},
            {0, PT_Stronghold_PortalRoom}};

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
        m_rng = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        m_rng.advance();
        m_startPos.x = (chunkX << 4) + 2;
        m_startPos.z = (chunkZ << 4) + 2;

        do {
            reset();

            // creates starting staircase
            EnumFacing direction = FACING_HORIZONTAL[m_rng.nextInt(4)];
            BoundingBox stairsBoundingBox = StructureComponent::makeBoundingBox(m_startPos.x, 64, m_startPos.z, direction, 5, 11, 5);

            m_pieceArray[m_pieceArraySize++] = StructureComponent(PT_Stronghold_StairsDown, 0, stairsBoundingBox, direction, 1);

            // this only adds the 5-crossing room
            addChildren(m_pieceArray[0]);

            while (m_pendingPiecesArraySize != 0) {
                c_int i = m_rng.nextInt(m_pendingPiecesArraySize);
                StructureComponent& piece = m_pieceArray[m_pendingPieceArray[i]];
                m_pendingPiecesArraySize--;

                // shift all the pieces over
                const size_t bytesToShift = (m_pendingPiecesArraySize - i) * sizeof(m_pendingPieceArray[0]);
                memmove(&m_pendingPieceArray[i], &m_pendingPieceArray[i + 1], bytesToShift);
                // for (int j = i; j < pendingPiecesArraySize; j++) {
                //     pendingPieceArray[j] = pendingPieceArray[j + 1];
                // }
                addChildren(piece);


                switch (m_generationStep) {
                    case GS_Stronghold_Portal:
                        if (m_portalRoomPiece != nullptr) { return; }
                        break;
                    case GS_Stronghold_Layout:
                        if (m_generationStopped && m_portalRoomPiece != nullptr) { return; }
                        break;
                    default:
                        break;
                }
            }

            if (m_generationStep == GS_Stronghold_Layout && m_portalRoomPiece != nullptr) { return; }

            // encompass structure bounding box
            m_structureBB = BoundingBox::EMPTY;
            for (int index = 0; index < m_pieceArraySize; index++) {
                m_structureBB.encompass(m_pieceArray[index]);
            }

            // find value to offset structure by "k"
            constexpr int i = 63 - 10;
            int j = m_structureBB.getYSize() + 1;
            if (j < i) { j += m_rng.nextInt(i - j); }
            c_int k = j - m_structureBB.m_maxY;

            // offset structure bounding box by "k"
            m_structureBB.offsetY(k);

            // offset all pieces by "k"
            for (int index = 0; index < m_pieceArraySize; index++) {
                m_pieceArray[index].offsetY(k);
            }


        } while (m_portalRoomPiece == nullptr);
    }


    void Stronghold::reset() {
        /*memcpy(&piecePlaceCounts,
               &PIECE_PLACE_COUNT_DEFAULT,
               11 * sizeof(PIECE_PLACE_COUNT_DEFAULT));*/
        for (int i = 0; i < 11; i++) { m_piecePlaceCounts[i] = PIECE_PLACE_COUNT_DEFAULT[i]; }
        this->deletePieces();

        m_generationStopped = false;
        m_forcedPiece = PT_Stronghold_NONE;
        m_previousPiece = PT_Stronghold_NONE;
        m_pieceArraySize = 0;
        m_pendingPiecesArraySize = 0;
        m_portalRoomPiece = nullptr;
        m_altarChestArraySize = 0;
        m_piecePlaceCountsSize = 11;
        m_totalWeight = 145;
    }


    void Stronghold::onWeightedPiecePlaced(c_int piecePlaceCountIndex) {
        PiecePlaceCount& piecePlaceCount = m_piecePlaceCounts[piecePlaceCountIndex];
        piecePlaceCount.m_placeCount++;

        m_previousPiece = piecePlaceCount.m_pieceType;
        if (piecePlaceCount.isValid()) return;

        m_totalWeight -= PiecePlaceCount::get(piecePlaceCount.m_pieceType)->getWeight();

        m_piecePlaceCountsSize--;

        for (int i = piecePlaceCountIndex; i < m_piecePlaceCountsSize; i++) {
            m_piecePlaceCounts[i] = m_piecePlaceCounts[i + 1];
        }
        
        // TODO: whoever inlined this function, we better hope they handled the return correctly
        // function originally called "updatePieceWeight"
        for (int index = 0; index < m_piecePlaceCountsSize; index++) {
            c_auto [placeCount, pieceType] = m_piecePlaceCounts[index];

            if (c_auto [weight, maxPlaceCount, minDepth] = PIECE_WEIGHTS[pieceType];
                maxPlaceCount > 0 && placeCount < maxPlaceCount)
                return;
        }

        m_generationStopped = true;
    }


    BoundingBox Stronghold::createPieceBoundingBox(const PieceType pieceType, const Pos3D& pos,
                                                   const EnumFacing facing) {
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


    void Stronghold::createPiece(PieceType pieceType, const EnumFacing facing, i8 depth, BoundingBox boundingBox) {
        int additionalData = 0;

        switch (pieceType) {
            case PT_Stronghold_Straight:
                // m_rng.nextInt(5); // this is java
                additionalData |= (m_rng.nextInt(2) == 0) << 0;
                additionalData |= (m_rng.nextInt(2) == 0) << 1;
                additionalData |= m_rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_PrisonHall:
                additionalData |= m_rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_LeftTurn:
                additionalData |= m_rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_RightTurn:
                additionalData |= m_rng.nextInt(5) << 16;
                m_pieceArray[m_pieceArraySize++] = StructureComponent(PT_Stronghold_LeftTurn, depth, boundingBox, facing, additionalData);
                return;
            case PT_Stronghold_RoomCrossing:
                // m_rng.nextInt(5); // this is java
                additionalData |= m_rng.nextInt(5);
                additionalData |= m_rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_StraightStairsDown:
                additionalData |= m_rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_StairsDown:
                additionalData |= m_rng.nextInt(5) << 16;
                break;
            case PT_Stronghold_FiveCrossing:
                additionalData |= m_rng.nextInt(5) << 16;
                additionalData |= m_rng.nextBoolean() << 0;
                additionalData |= m_rng.nextBoolean() << 1;
                additionalData |= m_rng.nextBoolean() << 2;
                additionalData |= (m_rng.nextInt(3) > 0) << 3;
                break;
            case PT_Stronghold_ChestCorridor:
                additionalData |= m_rng.nextInt(5) << 16;
                m_altarChestsArray[m_altarChestArraySize++] = &m_pieceArray[m_pieceArraySize];
                break;
            case PT_Stronghold_Library:
                additionalData |= m_rng.nextInt(5) << 16;
                additionalData |= (boundingBox.m_maxY - boundingBox.m_minY) > 6 ? 1 : 0;
                break;
            case PT_Stronghold_PortalRoom:
                m_portalRoomPiece = &m_pieceArray[m_pieceArraySize];
                break;
            default:
                break;
        }

        m_pieceArray[m_pieceArraySize++] = StructureComponent(pieceType, depth, boundingBox, facing, additionalData);
    }


    bool Stronghold::tryAddPieceFromType(const PieceType pieceType, const Pos3D& pos, const EnumFacing facing, c_i8 depth) {
        BoundingBox bBox = createPieceBoundingBox(pieceType, pos, facing);
        if (!isOkBox(bBox) || collidesWithPiece(bBox)) {
            if (pieceType == PT_Stronghold_Library) {
                bBox = BoundingBox::orientBox(pos, -4, -1, 0, 14, 6, 15, facing);
                if (!isOkBox(bBox) || collidesWithPiece(bBox)) return false;
            } else {
                return false;
            }
        }

        m_pendingPieceArray[m_pendingPiecesArraySize++] = m_pieceArraySize;
        createPiece(pieceType, facing, depth, bBox);

        if (m_generationStep == GS_Stronghold_Portal &&
            pieceType == PT_Stronghold_PortalRoom) m_generationStopped = true;


        return true;
    }

    bool Stronghold::genPieceFromSmallDoor(const Pos3D& pos, const EnumFacing facing, const i8 depth) {
        if EXPECT_FALSE (m_generationStopped) return false;

        if EXPECT_FALSE (m_forcedPiece != PT_Stronghold_NONE) {
            c_bool canAdd = tryAddPieceFromType(m_forcedPiece, pos, facing, depth);
            m_forcedPiece = PT_Stronghold_NONE;
            if (canAdd) { return true; }
        }

        c_int maxWeight = m_totalWeight;

        // TODO: can be rewritten to use a cumulative table, but would require
        // TODO: recalculating the table everytime an entry is removed.

        for (int attempt = 0; attempt < 5; attempt++) {
            int selectedWeight = m_rng.nextInt(maxWeight);
            // printf("Selected weight %i %i", totalWeight, selectedWeight);
            for (int index = 0; index < m_piecePlaceCountsSize; index++) {
                PiecePlaceCount& piecePlaceCount = m_piecePlaceCounts[index];
                const PieceType pieceType = piecePlaceCount.m_pieceType;
                c_int weight = PiecePlaceCount::get(pieceType)->getWeight();

                selectedWeight -= weight;
                if (selectedWeight >= 0) continue;

                if (pieceType == m_previousPiece) { break; }
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

        for (int i = 3 * (collidingPiece->m_minY == boundingBox.m_minY); i >= 1; --i) {
            boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, i - 1, facing);
            if (collidingPiece->intersects(boundingBox)) { continue; }

            boundingBox = BoundingBox::orientBox(pos, -1, -1, 0, 5, 5, i, facing);
            if (boundingBox.m_minY <= 1) { return false; }

            m_pendingPieceArray[m_pendingPiecesArraySize++] = m_pieceArraySize;
            m_pieceArray[m_pieceArraySize++] = StructureComponent(PT_Stronghold_FillerCorridor, depth, boundingBox, facing, 0);

            return true;
        }

        return false;
    }

    void Stronghold::genAndAddPiece(const Pos3D& pos, const EnumFacing facing, const int16_t depth) {
        if (depth > 50) return;

        if (abs(pos.getX() - m_startPos.x) <= 48 && abs(pos.getZ() - m_startPos.z) <= 48) {
            genPieceFromSmallDoor(pos, facing, static_cast<i8>(depth + 1));
            return;
        }

        for (int index = 0; index < m_piecePlaceCountsSize; index++) {
            if (c_auto& [placeCount, pieceType] = m_piecePlaceCounts[index];
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
        for (int i = 0; i < m_pieceArraySize; i++) {
            if (m_pieceArray[i].intersects(boundingBox)) { return &m_pieceArray[i]; }
        }
        return nullptr;
    }


    bool Stronghold::collidesWithPiece(const BoundingBox& boundingBox) {
        return findCollisionPiece(boundingBox) != nullptr;
    }


    bool inline Stronghold::isOkBox(const BoundingBox& boundingBox) { return boundingBox.m_minY > 10; }


    void Stronghold::genSmallDoorChildForward(const StructureComponent& piece, c_int n, c_int n2) {
        switch (piece.m_facing) {
            case EnumFacing::DOWN:
            case EnumFacing::UP:
                break;
            case EnumFacing::NORTH:
                return genAndAddPiece({piece.m_minX + n, piece.m_minY + n2, piece.m_minZ - 1}, EnumFacing::NORTH,
                                      piece.m_depth);
            case EnumFacing::SOUTH:
                return genAndAddPiece({piece.m_minX + n, piece.m_minY + n2, piece.m_maxZ + 1}, EnumFacing::SOUTH,
                                      piece.m_depth);
            case EnumFacing::WEST:
                return genAndAddPiece({piece.m_minX - 1, piece.m_minY + n2, piece.m_minZ + n}, EnumFacing::WEST,
                                      piece.m_depth);
            case EnumFacing::EAST:
                return genAndAddPiece({piece.m_maxX + 1, piece.m_minY + n2, piece.m_minZ + n}, EnumFacing::EAST,
                                      piece.m_depth);
        }
    }


    void Stronghold::genSmallDoorChildLeft(const StructureComponent& piece, c_int n, c_int n2) {
        switch (piece.m_facing) {
            case EnumFacing::DOWN:
            case EnumFacing::UP:
                break;
            case EnumFacing::SOUTH:
            case EnumFacing::NORTH:
                genAndAddPiece({piece.m_minX - 1, piece.m_minY + n, piece.m_minZ + n2}, EnumFacing::WEST,
                               piece.m_depth);
                break;
            case EnumFacing::EAST:
            case EnumFacing::WEST:
                genAndAddPiece({piece.m_minX + n2, piece.m_minY + n, piece.m_minZ - 1}, EnumFacing::NORTH,
                               piece.m_depth);
                break;
        }
    }

    void Stronghold::genSmallDoorChildRight(const StructureComponent& piece, c_int n, c_int n2) {
        switch (piece.m_facing) {
            case EnumFacing::DOWN:
            case EnumFacing::UP:
                break;
            case EnumFacing::SOUTH:
            case EnumFacing::NORTH:
                genAndAddPiece({piece.m_maxX + 1, piece.m_minY + n, piece.m_minZ + n2}, EnumFacing::EAST,
                               piece.m_depth);
                break;
            case EnumFacing::EAST:
            case EnumFacing::WEST:
                genAndAddPiece({piece.m_minX + n2, piece.m_minY + n, piece.m_maxZ + 1}, EnumFacing::SOUTH,
                               piece.m_depth);
                break;
            default:
                break;
        }
    }


    void Stronghold::addChildren(const StructureComponent& piece) {
        switch (piece.m_type) {
            case PT_Stronghold_NONE:
                break;
            case PT_Stronghold_Straight:
                genSmallDoorChildForward(piece, 1, 1);
                if ((piece.m_data & 1) != 0) genSmallDoorChildLeft(piece, 1, 2);
                if ((piece.m_data & 2) != 0) genSmallDoorChildRight(piece, 1, 2);
                break;
            case PT_Stronghold_PrisonHall:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PT_Stronghold_LeftTurn: {
                if (const EnumFacing direction = piece.m_facing;
                    direction == EnumFacing::NORTH || direction == EnumFacing::EAST) {
                    genSmallDoorChildLeft(piece, 1, 1);
                } else {
                    genSmallDoorChildRight(piece, 1, 1);
                }
                break;
            }
            case PT_Stronghold_RightTurn: {
                if (const EnumFacing direction = piece.m_facing;
                    direction == EnumFacing::NORTH || direction == EnumFacing::EAST) {
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
                if ((piece.m_data & 7) != 0) { m_forcedPiece = PT_Stronghold_FiveCrossing; }
                genSmallDoorChildForward(piece, 1, 1);
                break;
            }
            case PT_Stronghold_FiveCrossing: {
                const EnumFacing o = piece.m_facing;
                // 3 and 5 or 5 and 3
                c_int n = 5 - 2 * (o == EnumFacing::EAST || o == EnumFacing::SOUTH);
                c_int n2 = 8 - n;
                genSmallDoorChildForward(piece, 5, 1);
                if ((piece.m_data & 1) != 0) { genSmallDoorChildLeft(piece, n, 1); }
                if ((piece.m_data & 2) != 0) { genSmallDoorChildLeft(piece, n2, 7); }
                if ((piece.m_data & 4) != 0) { genSmallDoorChildRight(piece, n, 1); }
                if ((piece.m_data & 8) != 0) { genSmallDoorChildRight(piece, n2, 7); }
                break;
            }
            case PT_Stronghold_ChestCorridor:
                genSmallDoorChildForward(piece, 1, 1);
                break;
            case PT_Stronghold_FillerCorridor:
            case PT_Stronghold_PortalRoom:
            case PT_Stronghold_Library:
            default:
                break;
        }
    }
} // namespace gen

