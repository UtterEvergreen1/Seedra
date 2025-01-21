#include "mineshaft.hpp"

#include "components/StructureComponent.hpp"
#include "terrain/Chunk.hpp"
#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"


namespace gen {

    MU std::string Mineshaft::PIECE_TYPE_NAMES[5] = {
            "NONE",     // NONE
            "ROOM",     // Mineshaft_Room
            "CORRIDOR", // Mineshaft_Corridor
            "CROSSING", // Mineshaft_Crossing
            "STAIRS",   // Mineshaft_Stairs
    };

    MU std::string Mineshaft::getPieceName(const PieceType pieceType) {
        if (pieceType < 5) {
            return PIECE_TYPE_NAMES[pieceType];
        }
        return "";
    }

    Mineshaft::~Mineshaft() {
        this->reset();
    }


    MU void Mineshaft::reset() {
        pieceArraySize = 0;
        // collisionChecks = 0;
    }


    /**
     * \n
     * Overload function. Generates a mineshaft with the given seed and chunk coordinates.
     * @param worldSeed the seed
     * @param chunkPos coordinates of the chunk
     */
    void Mineshaft::generate(lce::CONSOLE console, c_i64 worldSeed, const Pos2D chunkPos) {
        return generate(console, worldSeed, chunkPos.x, chunkPos.z);
    }

    /**
     * \n
     * Generates a mineshaft with the given seed and chunk coordinates.
     * @param worldSeed the seed
     * @param chunkX x coordinate of the chunk
     * @param chunkZ z coordinate of the chunk
     */
    void Mineshaft::generate(lce::CONSOLE console, c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        // 4 rolls (1 for skip, 3 for is feature chunk rolls (2 double, 1 int))
        rng = RNG::ConstructWithoutSetSeed((rng.getSeed() * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF);
        startPos.x = (chunkX << 4) + 2;
        startPos.z = (chunkZ << 4) + 2;
        this->reset();


        int boundingBoxXUpper;
        int boundingBoxYUpper;
        int boundingBoxZUpper;
        // build the entire structure
        if (console == lce::CONSOLE::XBOX360 || console == lce::CONSOLE::XBOX1) {
            boundingBoxZUpper = startPos.z + rng.nextInt(6) + 7;
            boundingBoxYUpper = 54 + rng.nextInt(6);
            boundingBoxXUpper = startPos.x + rng.nextInt(6) + 7;
        } else {
            boundingBoxXUpper = startPos.x + rng.nextInt(6) + 7;
            boundingBoxYUpper = 54 + rng.nextInt(6);
            boundingBoxZUpper = startPos.z + rng.nextInt(6) + 7;
        }
        const BoundingBox roomBoundingBox(startPos.x, 50, startPos.z, boundingBoxXUpper, boundingBoxYUpper, boundingBoxZUpper);

        // recursive gen
        buildComponent(rng, {PieceType::PT_Mineshaft_Room, 0, roomBoundingBox, EnumFacing::NORTH, 1});

        // get Y level
        structureBB = BoundingBox::EMPTY;
        for (int index = 0; index < pieceArraySize; index++) {
            structureBB.encompass(pieceArray[index]);
        }

        // specifically mesa
        if (myVariant == StructureVariant::SV_Mineshaft_Mesa) {
            c_int i = 63 - structureBB.maxY + structureBB.getYSize() / 2 + 5;
            // update structure offset
            structureBB.offset(0, i, 0);
            // update pieces offset
            for (int index = 0; index < pieceArraySize; index++) {
                pieceArray[index].variant = StructureVariant::SV_Mineshaft_Mesa;
                pieceArray[index].offset(0, i, 0);
            }
            return;
        } else {
            // non-mesa
            constexpr int i = 63 - 10;
            int j = structureBB.getYSize() + 1;
            if (j < i) { j += rng.nextInt(i - j); }
            c_int k = j - structureBB.maxY;
            // update structure offset
            structureBB.offset(0, k, 0);
            // update pieces offset
            for (int piece = 0; piece < pieceArraySize; piece++) { pieceArray[piece].offset(0, k, 0); }
        }

    }


    StructureComponent* Mineshaft::findCollisionPiece(const BoundingBox& bbIn) {
        for (int i = 0; i < pieceArraySize; i++) {
            collisionChecks++;
            if (pieceArray[i].intersects(bbIn)) { return &pieceArray[i]; }
        }
        return nullptr;
    }


    bool Mineshaft::collides(const BoundingBox& bbIn) {
        const StructureComponent* collidingPiece = findCollisionPiece(bbIn);
        return collidingPiece != nullptr;
    }



    void Mineshaft::genAndAddPiece(RNG& rng, const Pos3D pos, const EnumFacing facing, c_int depth) {
        // step 1: return early
        if (depth > 8) return;
        if (abs(pos.getX() - startPos.x) > 80 || abs(pos.getZ() - startPos.z) > 80) return;

        // step 2: create the piece
        c_int randomRoom = rng.nextInt(100);
        BoundingBox boundingBox;
        int data = 0;

        // step 3: 3 different cases
        if (randomRoom >= 80) { // CASE CROSSING
            boundingBox = BoundingBox::orientBox(pos, -1, 0, 0, 5, 3, 5, facing);
            if (rng.nextInt(4) == 0) {
                boundingBox.maxY += 4;
                data = 1;
            }
            if (collides(boundingBox)) return;
            buildComponent(rng, {PT_Mineshaft_Crossing, static_cast<int8_t>(depth + 1), boundingBox, facing, data});

        } else if (randomRoom >= 70) { // CASE STAIRS
            boundingBox = BoundingBox::orientBox(pos, 0, -5, 0, 3, 8, 9, facing);
            if (collides(boundingBox)) return;
            buildComponent(rng, {PT_Mineshaft_Stairs, static_cast<int8_t>(depth + 1), boundingBox, facing, 0});

        } else { // CASE CORRIDOR
            int i;

            for (i = rng.nextInt(3) + 2; i > 0; --i) {
                c_int j = i * 5;
                boundingBox = BoundingBox::orientBox(pos, 0, 0, 0, 3, 3, j, facing);
                if (!collides(boundingBox)) { break; }
            }
            if (i == 0) { return; }
            c_bool hasRails = rng.nextInt(3) == 0;
            c_bool hasSpiders = !hasRails && rng.nextInt(23) == 0;
            data |= hasRails;
            data |= hasSpiders << 1;
            buildComponent(rng, {PT_Mineshaft_Corridor, static_cast<int8_t>(depth + 1), boundingBox, facing, data});
        }
    }


    void Mineshaft::buildComponent(RNG& rng, StructureComponent p) {
        pieceArray[pieceArraySize++] = p;

        switch (p.type) {
            default:
                break;

            case PT_Mineshaft_Room: {
                int k;
                int j = p.getYSize() - 4;
                if (j <= 0) j = 1;
                c_int roomXSize = p.getXSize();
                c_int roomZSize = p.getZSize();
                for (k = 0; k < roomXSize; k += 4) {
                    k += rng.nextInt(roomXSize);
                    if (k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {p.minX + k, p.minY + rng.nextInt(j) + 1, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                }
                for (k = 0; k < roomXSize; k += 4) {
                    k += rng.nextInt(roomXSize);
                    if (k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {p.minX + k, p.minY + rng.nextInt(j) + 1, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                }
                for (k = 0; k < roomZSize; k += 4) {
                    k += rng.nextInt(roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {p.minX - 1, p.minY + rng.nextInt(j) + 1, p.minZ + k}, EnumFacing::WEST, p.depth);
                }
                for (k = 0; k < roomZSize; k += 4) {
                    k += rng.nextInt(roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {p.maxX + 1, p.minY + rng.nextInt(j) + 1, p.minZ + k}, EnumFacing::EAST, p.depth);
                }
                return;
            }

            case PT_Mineshaft_Corridor: {
                c_int corridorType = rng.nextInt(4);
                c_int yState = p.minY + rng.nextInt(3) - 1;
                switch (p.facing) {
                    case EnumFacing::NORTH:
                    default:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.minX, yState, p.minZ - 1}, p.facing, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.minX - 1, yState, p.minZ}, EnumFacing::WEST, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.maxX + 1, yState, p.minZ}, EnumFacing::EAST, p.depth);
                        }
                        break;

                    case EnumFacing::SOUTH:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.minX, yState, p.maxZ + 1}, p.facing, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.minX - 1, yState, p.maxZ - 3}, EnumFacing::WEST, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.maxX + 1, yState, p.maxZ - 3}, EnumFacing::EAST, p.depth);
                        }
                        break;

                    case EnumFacing::WEST:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.minX - 1, yState, p.minZ}, p.facing, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.minX, yState, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.minX, yState, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                        }
                        break;

                    case EnumFacing::EAST:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.maxX + 1, yState, p.minZ}, p.facing, p.depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.maxX - 3, yState, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.maxX - 3, yState, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                        }
                        break;
                }

                if (p.depth >= 8) return;

                switch (p.facing) {
                    case EnumFacing::NORTH:
                    case EnumFacing::SOUTH:
                        for (int k = p.minZ + 3; k + 3 <= p.maxZ; k += 5) {
                            if (c_int l = rng.nextInt(5); l == 0)
                                genAndAddPiece(rng, {p.minX - 1, p.minY, k}, EnumFacing::WEST, p.depth + 1);
                            else if (l == 1)
                                genAndAddPiece(rng, {p.maxX + 1, p.minY, k}, EnumFacing::EAST, p.depth + 1);
                        }
                        return;
                    case EnumFacing::WEST:
                    case EnumFacing::EAST:
                        for (int k = p.minX + 3; k + 3 <= p.maxX; k += 5) {
                            if (c_int l = rng.nextInt(5); l == 0)
                                genAndAddPiece(rng, {k, p.minY, p.minZ - 1}, EnumFacing::NORTH, p.depth + 1);
                            else if (l == 1)
                                genAndAddPiece(rng, {k, p.minY, p.maxZ + 1}, EnumFacing::SOUTH, p.depth + 1);
                        }
                        return;
                    case EnumFacing::DOWN:
                    case EnumFacing::UP:
                        return;
                }
                return;
            }

            case PT_Mineshaft_Crossing: {
                switch (p.facing) {
                    case EnumFacing::NORTH:
                    default:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                        genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ + 1}, EnumFacing::WEST, p.depth);
                        genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ + 1}, EnumFacing::EAST, p.depth);
                        break;
                    case EnumFacing::SOUTH:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                        genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ + 1}, EnumFacing::WEST, p.depth);
                        genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ + 1}, EnumFacing::EAST, p.depth);
                        break;
                    case EnumFacing::WEST:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                        genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ + 1}, EnumFacing::WEST, p.depth);
                        break;
                    case EnumFacing::EAST:
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                        genAndAddPiece(rng, {p.minX + 1, p.minY, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                        genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ + 1}, EnumFacing::EAST, p.depth);
                        break;
                }

                if (p.data) {
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.minX + 1, p.minY + 4, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.minX - 1, p.minY + 4, p.minZ + 1}, EnumFacing::WEST, p.depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.maxX + 1, p.minY + 4, p.minZ + 1}, EnumFacing::EAST, p.depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.minX + 1, p.minY + 4, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                }
                return;
            }

            case PT_Mineshaft_Stairs: {
                switch (p.facing) {
                    default:
                    case EnumFacing::NORTH:
                        return genAndAddPiece(rng, {p.minX, p.minY, p.minZ - 1}, EnumFacing::NORTH, p.depth);
                    case EnumFacing::SOUTH:
                        return genAndAddPiece(rng, {p.minX, p.minY, p.maxZ + 1}, EnumFacing::SOUTH, p.depth);
                    case EnumFacing::WEST:
                        return genAndAddPiece(rng, {p.minX - 1, p.minY, p.minZ}, EnumFacing::WEST, p.depth);
                    case EnumFacing::EAST:
                        return genAndAddPiece(rng, {p.maxX + 1, p.minY, p.minZ}, EnumFacing::EAST, p.depth);
                }
            }
        }
    }

} // namespace gen


