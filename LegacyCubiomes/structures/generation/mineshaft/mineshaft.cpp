#include "mineshaft.hpp"


namespace generation {

    const std::map<int8_t, std::string> Mineshaft::PieceTypeName = {
            {PieceType::ROOM, "ROOM"},
            {PieceType::CORRIDOR, "CORRIDOR"},
            {PieceType::CROSSING, "CROSSING"},
            {PieceType::STAIRS, "STAIRS"},
            {PieceType::NONE, "NONE -> something went wrong"}};

    void Mineshaft::reset() {
        pieceArraySize = 0;
        collisionChecks = 0;
    }

    void Mineshaft::generate(c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        // 4 rolls (1 for skip, 3 for is feature chunk rolls (2 double, 1 int))
        rng = RNG((rng.getSeed() * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF);
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;
        pieceArraySize = 0;

        // build the entire structure
        c_int boundingBoxXUpper = startX + rng.nextInt(6) + 7;
        c_int boundingBoxYUpper = 54 + rng.nextInt(6);
        c_int boundingBoxZUpper = startZ + rng.nextInt(6) + 7;
        const BoundingBox roomBoundingBox(startX, 50, startZ, boundingBoxXUpper, boundingBoxYUpper, boundingBoxZUpper);

        // recursive gen
        buildComponent(rng, PieceType::ROOM, 0, roomBoundingBox, DIRECTION::NORTH, 1);

        // get Y level
        structureBoundingBox = BoundingBox::EMPTY;
        for (int index = 0; index < pieceArraySize; index++) { structureBoundingBox.encompass(pieceArray[index]); }

        // specifically mesa
        if (mineShaftType == MineshaftType::MESA) {
            c_int i = 63 - structureBoundingBox.maxY + structureBoundingBox.getYSize() / 2 + 5;
            structureBoundingBox.offset(0, i, 0);
            for (int index = 0; index < pieceArraySize; index++) { pieceArray[index].offset(0, i, 0); }
            return;
        }

        // non-mesa
        constexpr int i = 63 - 10;
        int j = structureBoundingBox.getYSize() + 1;
        if (j < i) { j += rng.nextInt(i - j); }
        c_int k = j - structureBoundingBox.maxY;
        for (int piece = 0; piece < pieceArraySize; piece++) { pieceArray[piece].offset(0, k, 0); }
    }


    Piece* Mineshaft::findCollisionPiece(const BoundingBox& boundingBox) {
        for (int i = 0; i < pieceArraySize; i++) {
            collisionChecks++;
            if (pieceArray[i].intersects(boundingBox)) { return &pieceArray[i]; }
        }
        return nullptr;
    }


    void Mineshaft::genAndAddPiece(RNG& rng, const Pos3D pos, const DIRECTION direction, c_int depth) {
        // step 1: return early
        if (depth > 8)
            return;
        if (abs(pos.getX() - startX) > 80 || abs(pos.getZ() - startZ) > 80)
            return;

        // step 2: create the piece
        c_int randomRoom = rng.nextInt(100);
        BoundingBox boundingBox;
        int additionalData = 0;

        // step 3: 3 different cases
        if (randomRoom >= 80) { // CASE CROSSING
            boundingBox = BoundingBox::orientBox(pos, -1, 0, 0, 5, 3, 5, direction);
            if (rng.nextInt(4) == 0) {
                boundingBox.maxY += 4;
                additionalData = 1;
            }
            if (const Piece* collidingPiece = findCollisionPiece(boundingBox); collidingPiece != nullptr) return;
            buildComponent(rng, PieceType::CROSSING, depth + 1, boundingBox, direction, additionalData);

        } else if (randomRoom >= 70) { // CASE STAIRS
            boundingBox = BoundingBox::orientBox(pos, 0, -5, 0, 3, 8, 9, direction);
            if (const Piece* collidingPiece = findCollisionPiece(boundingBox); collidingPiece != nullptr) return;
            buildComponent(rng, PieceType::STAIRS, depth + 1, boundingBox, direction, 0);

        } else {
            int i; // CASE CORRIDOR
            for (i = rng.nextInt(3) + 2; i > 0; --i) {
                c_int j = i * 5;
                boundingBox = BoundingBox::orientBox(pos, 0, 0, 0, 3, 3, j, direction);
                if (const Piece* collidingPiece = findCollisionPiece(boundingBox); collidingPiece == nullptr) break;
            }
            if (i == 0) return;
            c_bool hasRails = rng.nextInt(3) == 0;
            c_bool hasSpiders = !hasRails && rng.nextInt(23) == 0;
            additionalData |= hasRails;
            additionalData |= hasSpiders << 1;
            buildComponent(rng, PieceType::CORRIDOR, depth + 1, boundingBox, direction, additionalData);
        }
    }


    void Mineshaft::buildComponent(RNG& rng, const PieceType type, c_int depth, const BoundingBox& boundingBox,
                                   const DIRECTION direction, c_int additionalData) {
        auto p = Piece(static_cast<i8>(type), static_cast<i8>(depth), boundingBox, direction, additionalData);
        pieceArray[pieceArraySize++] = p;

        switch (static_cast<PieceType>(p.type)) {
            default:
                break;

            case PieceType::ROOM: {
                int k;
                int j = p.getYSize() - 4;
                if (j <= 0) j = 1;
                c_int roomXSize = p.getXSize();
                c_int roomZSize = p.getZSize();
                for (k = 0; k < roomXSize; k += 4) {
                    k += rng.nextInt(roomXSize);
                    if (k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {p.minX + k, p.minY + rng.nextInt(j) + 1, p.minZ - 1}, DIRECTION::NORTH,
                                   p.depth);
                }
                for (k = 0; k < roomXSize; k += 4) {
                    k += rng.nextInt(roomXSize);
                    if (k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {p.minX + k, p.minY + rng.nextInt(j) + 1, p.maxZ + 1}, DIRECTION::SOUTH,
                                   p.depth);
                }
                for (k = 0; k < roomZSize; k += 4) {
                    k += rng.nextInt(roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {p.minX - 1, p.minY + rng.nextInt(j) + 1, p.minZ + k}, DIRECTION::WEST,
                                   p.depth);
                }
                for (k = 0; k < roomZSize; k += 4) {
                    k += rng.nextInt(roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {p.maxX + 1, p.minY + rng.nextInt(j) + 1, p.minZ + k}, DIRECTION::EAST,
                                   p.depth);
                }
                return;
            }

            case PieceType::CORRIDOR: {
                c_int corridorType = rng.nextInt(4);
                c_int yState = p.minY + rng.nextInt(3) - 1;
                switch (p.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.minX, yState, p.minZ - 1}, p.orientation, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.minX - 1, yState, p.minZ}, DIRECTION::WEST, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.maxX + 1, yState, p.minZ}, DIRECTION::EAST, p.depth);
                        }
                        break;

                    case DIRECTION::SOUTH:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.minX, yState, p.maxZ + 1}, p.orientation, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.minX - 1, yState, p.maxZ - 3}, DIRECTION::WEST, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.maxX + 1, yState, p.maxZ - 3}, DIRECTION::EAST, p.depth);
                        }
                        break;

                    case DIRECTION::WEST:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.minX - 1, yState, p.minZ}, p.orientation, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.minX, yState, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.minX, yState, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                        }
                        break;

                    case DIRECTION::EAST:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.maxX + 1, yState, p.minZ}, p.orientation, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.maxX - 3, yState, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.maxX - 3, yState, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                        }
                        break;
                }

                if (p.depth >= 8) return;

                switch (p.orientation) {
                    case DIRECTION::NORTH:
                    case DIRECTION::SOUTH:
                        for (int k = p.minZ + 3; k + 3 <= p.maxZ; k += 5) {
                            if (c_int l = rng.nextInt(5); l == 0)
                                genAndAddPiece(rng, {p.minX - 1, p.minY, k}, DIRECTION::WEST, p.depth + 1);
                            else if (l == 1)
                                genAndAddPiece(rng, {p.maxX + 1, p.minY, k}, DIRECTION::EAST, p.depth + 1);
                        }
                        return;
                    case DIRECTION::WEST:
                    case DIRECTION::EAST:
                        for (int k = p.minX + 3; k + 3 <= p.maxX; k += 5) {
                            if (c_int l = rng.nextInt(5); l == 0)
                                genAndAddPiece(rng, {k, p.minY, p.minZ - 1}, DIRECTION::NORTH, p.depth + 1);
                            else if (l == 1)
                                genAndAddPiece(rng, {k, p.minY, p.maxZ + 1}, DIRECTION::SOUTH, p.depth + 1);
                        }
                        return;
                }
                return;
            }

            case PieceType::CROSSING: {
                switch (p.orientation) {
                    case DIRECTION::NORTH:
                    default:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                        genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ + 1}, DIRECTION::WEST, p.depth);
                        genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ + 1}, DIRECTION::EAST, p.depth);
                        break;
                    case DIRECTION::SOUTH:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                        genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ + 1}, DIRECTION::WEST, p.depth);
                        genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ + 1}, DIRECTION::EAST, p.depth);
                        break;
                    case DIRECTION::WEST:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                        genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ + 1}, DIRECTION::WEST, p.depth);
                        break;
                    case DIRECTION::EAST:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                        genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ + 1}, DIRECTION::EAST, p.depth);
                    break;
                }

                if (p.additionalData) {
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.minX + 1, p.minY + 4, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.minX - 1, p.minY + 4, p.minZ + 1}, DIRECTION::WEST, p.depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.maxX + 1, p.minY + 4, p.minZ + 1}, DIRECTION::EAST, p.depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.minX + 1, p.minY + 4, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                }
                return;
            }

            case PieceType::STAIRS: {
                switch (p.orientation) {
                    default:
                    case DIRECTION::NORTH:
                        return genAndAddPiece(rng, {p.minX, p.minY, p.minZ - 1}, DIRECTION::NORTH, p.depth);
                    case DIRECTION::SOUTH:
                        return genAndAddPiece(rng, {p.minX, p.minY, p.maxZ + 1}, DIRECTION::SOUTH, p.depth);
                    case DIRECTION::WEST:
                        return genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ}, DIRECTION::WEST, p.depth);
                    case DIRECTION::EAST:
                        return genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ}, DIRECTION::EAST, p.depth);
                }
            }
        }
    }


} // namespace generation