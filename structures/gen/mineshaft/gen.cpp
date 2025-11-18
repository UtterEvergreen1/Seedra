#include "mineshaft.hpp"

#include "components/StructureComponent.hpp"
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
        m_pieceArraySize = 0;
        // collisionChecks = 0;
    }


    /**
     * \n
     * Overload function. Generates a mineshaft with the given seed and chunk coordinates.
     *
     * @param console the console
     * @param worldSeed the seed
     * @param chunkPos coordinates of the chunk
     */
    void Mineshaft::generate(lce::CONSOLE console, c_i64 worldSeed, const Pos2D chunkPos) {
        return generate(console, worldSeed, chunkPos.x, chunkPos.z);
    }

    /**
     * \n
     * Generates a mineshaft with the given seed and chunk coordinates.
     * @param console the console
     * @param worldSeed the seed
     * @param chunkX x coordinate of the chunk
     * @param chunkZ z coordinate of the chunk
     */
    void Mineshaft::generate(const lce::CONSOLE console, c_i64 worldSeed, c_int chunkX, c_int chunkZ) {
        RNG rng = RNG::getLargeFeatureSeed(worldSeed, chunkX, chunkZ);
        // 4 rolls (1 for skip, 3 for is feature chunk rolls (2 double, 1 int))
        rng = RNG::ConstructWithoutSetSeed((rng.getSeed() * 0x32EB772C5F11 + 0x2D3873C4CD04) & 0xFFFFFFFFFFFF);
        m_startPos.x = (chunkX << 4) + 2;
        m_startPos.z = (chunkZ << 4) + 2;
        this->reset();


        int boundingBoxXUpper;
        int boundingBoxYUpper;
        int boundingBoxZUpper;
        // build the entire structure
        if (console == lce::CONSOLE::XBOX360 || console == lce::CONSOLE::XBOX1) {
            boundingBoxZUpper = m_startPos.z + rng.nextInt(6) + 7;
            boundingBoxYUpper = 54 + rng.nextInt(6);
            boundingBoxXUpper = m_startPos.x + rng.nextInt(6) + 7;
        } else {
            boundingBoxXUpper = m_startPos.x + rng.nextInt(6) + 7;
            boundingBoxYUpper = 54 + rng.nextInt(6);
            boundingBoxZUpper = m_startPos.z + rng.nextInt(6) + 7;
        }
        const BoundingBox roomBoundingBox(m_startPos.x, 50, m_startPos.z, boundingBoxXUpper, boundingBoxYUpper, boundingBoxZUpper);

        // recursive gen
        buildComponent(rng, {PT_Mineshaft_Room, 0, roomBoundingBox, EnumFacing::NORTH, 1});

        // get Y level
        m_structureBB = BoundingBox::EMPTY;
        for (int index = 0; index < m_pieceArraySize; index++) {
            m_structureBB.encompass(m_pieceArray[index]);
        }

        // specifically mesa
        if (m_variant == StructureVariant::SV_Mineshaft_Mesa) {
            c_int i = 63 - m_structureBB.m_maxY + m_structureBB.getYSize() / 2 + 5;
            // update structure offset
            m_structureBB.offset(0, i, 0);
            // update pieces offset
            for (int index = 0; index < m_pieceArraySize; index++) {
                m_pieceArray[index].variant = StructureVariant::SV_Mineshaft_Mesa;
                m_pieceArray[index].offset(0, i, 0);
            }
            return;
        }

        // non-mesa
        constexpr int i = 63 - 10;
        int j = m_structureBB.getYSize() + 1;
        if (j < i) { j += rng.nextInt(i - j); }
        c_int k = j - m_structureBB.m_maxY;
        // update structure offset
        m_structureBB.offset(0, k, 0);
        // update pieces offset
        for (int piece = 0; piece < m_pieceArraySize; piece++) { m_pieceArray[piece].offset(0, k, 0); }


    }


    StructureComponent* Mineshaft::findCollisionPiece(const BoundingBox& bbIn) {
        for (int i = 0; i < m_pieceArraySize; i++) {
            m_collisionChecks++;
            if (m_pieceArray[i].intersects(bbIn)) { return &m_pieceArray[i]; }
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
        if (abs(pos.getX() - m_startPos.x) > 80 || abs(pos.getZ() - m_startPos.z) > 80) return;

        // step 2: create the piece
        c_int randomRoom = rng.nextInt(100);
        BoundingBox boundingBox;
        int data = 0;

        // step 3: 3 different cases
        if (randomRoom >= 80) { // CASE CROSSING
            boundingBox = BoundingBox::orientBox(pos, -1, 0, 0, 5, 3, 5, facing);
            if (rng.nextInt(4) == 0) {
                boundingBox.m_maxY += 4;
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
            data |= hasRails ? 1 : 0;
            data |= hasSpiders << 1;
            buildComponent(rng, {PT_Mineshaft_Corridor, static_cast<int8_t>(depth + 1), boundingBox, facing, data});
        }
    }


    void Mineshaft::buildComponent(RNG& rng, const StructureComponent& p) {
        m_pieceArray[m_pieceArraySize++] = p;

        switch (p.m_type) {
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
                    genAndAddPiece(rng, {p.m_minX + k, p.m_minY + rng.nextInt(j) + 1, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                }
                for (k = 0; k < roomXSize; k += 4) {
                    k += rng.nextInt(roomXSize);
                    if (k + 3 > roomXSize) break;
                    genAndAddPiece(rng, {p.m_minX + k, p.m_minY + rng.nextInt(j) + 1, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                }
                for (k = 0; k < roomZSize; k += 4) {
                    k += rng.nextInt(roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {p.m_minX - 1, p.m_minY + rng.nextInt(j) + 1, p.m_minZ + k}, EnumFacing::WEST, p.m_depth);
                }
                for (k = 0; k < roomZSize; k += 4) {
                    k += rng.nextInt(roomZSize);
                    if (k + 3 > roomZSize) break;
                    genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY + rng.nextInt(j) + 1, p.m_minZ + k}, EnumFacing::EAST, p.m_depth);
                }
                return;
            }

            case PT_Mineshaft_Corridor: {
                c_int corridorType = rng.nextInt(4);
                c_int yState = p.m_minY + rng.nextInt(3) - 1;
                switch (p.m_facing) {
                    case EnumFacing::NORTH:
                    default:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.m_minX, yState, p.m_minZ - 1}, p.m_facing, p.m_depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.m_minX - 1, yState, p.m_minZ}, EnumFacing::WEST, p.m_depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.m_maxX + 1, yState, p.m_minZ}, EnumFacing::EAST, p.m_depth);
                        }
                        break;

                    case EnumFacing::SOUTH:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.m_minX, yState, p.m_maxZ + 1}, p.m_facing, p.m_depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.m_minX - 1, yState, p.m_maxZ - 3}, EnumFacing::WEST, p.m_depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.m_maxX + 1, yState, p.m_maxZ - 3}, EnumFacing::EAST, p.m_depth);
                        }
                        break;

                    case EnumFacing::WEST:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.m_minX - 1, yState, p.m_minZ}, p.m_facing, p.m_depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.m_minX, yState, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.m_minX, yState, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                        }
                        break;

                    case EnumFacing::EAST:
                        switch (corridorType) {
                            default:
                                genAndAddPiece(rng, {p.m_maxX + 1, yState, p.m_minZ}, p.m_facing, p.m_depth);
                                break;
                            case 2:
                                genAndAddPiece(rng, {p.m_maxX - 3, yState, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                                break;
                            case 3:
                                genAndAddPiece(rng, {p.m_maxX - 3, yState, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                        }
                        break;
                }

                if (p.m_depth >= 8) return;

                switch (p.m_facing) {
                    case EnumFacing::NORTH:
                    case EnumFacing::SOUTH:
                        for (int k = p.m_minZ + 3; k + 3 <= p.m_maxZ; k += 5) {
                            if (c_int l = rng.nextInt(5); l == 0)
                                genAndAddPiece(rng, {p.m_minX - 1, p.m_minY, k}, EnumFacing::WEST, p.m_depth + 1);
                            else if (l == 1)
                                genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY, k}, EnumFacing::EAST, p.m_depth + 1);
                        }
                        return;
                    case EnumFacing::WEST:
                    case EnumFacing::EAST:
                        for (int k = p.m_minX + 3; k + 3 <= p.m_maxX; k += 5) {
                            if (c_int l = rng.nextInt(5); l == 0)
                                genAndAddPiece(rng, {k, p.m_minY, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth + 1);
                            else if (l == 1)
                                genAndAddPiece(rng, {k, p.m_minY, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth + 1);
                        }
                        return;
                    case EnumFacing::DOWN:
                    case EnumFacing::UP:
                        return;
                }
                return;
            }

            case PT_Mineshaft_Crossing: {
                switch (p.m_facing) {
                    case EnumFacing::NORTH:
                    default:
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                        genAndAddPiece(rng, {p.m_minX - 1, p.m_minY, p.m_minZ + 1}, EnumFacing::WEST, p.m_depth);
                        genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY, p.m_minZ + 1}, EnumFacing::EAST, p.m_depth);
                        break;
                    case EnumFacing::SOUTH:
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                        genAndAddPiece(rng, {p.m_minX - 1, p.m_minY, p.m_minZ + 1}, EnumFacing::WEST, p.m_depth);
                        genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY, p.m_minZ + 1}, EnumFacing::EAST, p.m_depth);
                        break;
                    case EnumFacing::WEST:
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                        genAndAddPiece(rng, {p.m_minX - 1, p.m_minY, p.m_minZ + 1}, EnumFacing::WEST, p.m_depth);
                        break;
                    case EnumFacing::EAST:
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                        genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY, p.m_minZ + 1}, EnumFacing::EAST, p.m_depth);
                        break;
                }

                if (p.m_data) {
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY + 4, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.m_minX - 1, p.m_minY + 4, p.m_minZ + 1}, EnumFacing::WEST, p.m_depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY + 4, p.m_minZ + 1}, EnumFacing::EAST, p.m_depth);
                    if (rng.nextBoolean())
                        genAndAddPiece(rng, {p.m_minX + 1, p.m_minY + 4, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                }
                return;
            }

            case PT_Mineshaft_Stairs: {
                switch (p.m_facing) {
                    default:
                    case EnumFacing::NORTH:
                        return genAndAddPiece(rng, {p.m_minX, p.m_minY, p.m_minZ - 1}, EnumFacing::NORTH, p.m_depth);
                    case EnumFacing::SOUTH:
                        return genAndAddPiece(rng, {p.m_minX, p.m_minY, p.m_maxZ + 1}, EnumFacing::SOUTH, p.m_depth);
                    case EnumFacing::WEST:
                        return genAndAddPiece(rng, {p.m_minX - 1, p.m_minY, p.m_minZ}, EnumFacing::WEST, p.m_depth);
                    case EnumFacing::EAST:
                        return genAndAddPiece(rng, {p.m_maxX + 1, p.m_minY, p.m_minZ}, EnumFacing::EAST, p.m_depth);
                }
            }
        }
    }

} // namespace gen


