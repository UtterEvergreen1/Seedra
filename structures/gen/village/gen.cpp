#include "village.hpp"

#include "structures/placement/StaticStructures.hpp"

#include "components/StructureComponent.hpp"
#include "terrain/World.hpp"
#include "terrain/biomes/biome_t.hpp"
#include "terrain/generator.hpp"

namespace gen {

    MU std::string Village::PIECE_TYPE_NAMES[13] = {
            "NONE        ", // PieceType::Village_NONE
            "Start       ", // PieceType::Village_Start
            "Road        ", // PieceType::Village_Road
            "House4Garden", // PieceType::Village_House4Garden
            "Church      ", // PieceType::Village_Church
            "House1      ", // PieceType::Village_House1
            "WoodHut     ", // PieceType::Village_WoodHut
            "Hall        ", // PieceType::Village_Hall
            "Double Field", // PieceType::Village_Field1
            "Single Field", // PieceType::Village_Field2
            "Blacksmith  ", // PieceType::Village_House2
            "House3      ", // PieceType::Village_House3
            "Torch       ", // PieceType::Village_Torch
    };


    MU std::string Village::getPieceName(const PieceType pieceType) {
        if (pieceType < 12) {
            return PIECE_TYPE_NAMES[pieceType];
        }
        return "";
    }


    c_int Village::VILLAGE_SIZE = 0;

    constexpr Village::PieceWeight Village::PIECE_WEIGHTS[9] = {
            {4, 2 + VILLAGE_SIZE, 4 + VILLAGE_SIZE * 2, PieceType::PT_Village_House4Garden},
            {20, 0 + VILLAGE_SIZE, 1 + VILLAGE_SIZE, PieceType::PT_Village_Church},
            {20, 0 + VILLAGE_SIZE, 2 + VILLAGE_SIZE, PieceType::PT_Village_House1},
            {3, 2 + VILLAGE_SIZE, 5 + VILLAGE_SIZE * 3, PieceType::PT_Village_WoodHut},
            {15, 0 + VILLAGE_SIZE, 2 + VILLAGE_SIZE, PieceType::PT_Village_Hall},
            {3, 1 + VILLAGE_SIZE, 4 + VILLAGE_SIZE, PieceType::PT_Village_Field1},
            {3, 2 + VILLAGE_SIZE, 4 + VILLAGE_SIZE * 2, PieceType::PT_Village_Field2},
            {15, 0, 1 + VILLAGE_SIZE, PieceType::PT_Village_House2},
            {8, 0 + VILLAGE_SIZE, 3 + VILLAGE_SIZE * 2, PieceType::PT_Village_House3}};



    Village::Village(const Generator* generator, c_bool useBiomes) { m_g = generator; this->m_useBiomes = useBiomes; }

    Village::~Village() {
        this->reset();
    }

    void Village::reset() {
        // for (int i = 0; i < m_pieceArraySize; i++) {
        //     delete pieceArray[i];
        // }
        m_previousPiece = PieceType::PT_Village_NONE;
        m_myBlackSmithPieceIndex = -1;
        m_numInvalidPieces = 1;
        m_pieceArraySize = 0;
        m_pendingRoadArraySize = 0;
    }

    /**
      * \n
      * Overload function. Generates a mineshaft with the given chunk coordinates and stored generator.
      * @param chunk coordinates of the chunk
      */
    void Village::generate(const Pos2D chunk) { generate(chunk.x, chunk.z); }

    /**
     * \n
     * Generates a mineshaft with the given chunk coordinates and stored generator.
     * @param chunkX x coordinate of the chunk
     * @param chunkZ z coordinate of the chunk
     */
    void Village::generate(c_int chunkX, c_int chunkZ) {
        m_rng = RNG::getLargeFeatureSeed(m_g->getWorldSeed(), chunkX, chunkZ);
        m_rng.advance();
        m_startPos.x = (chunkX << 4) + 2;
        m_startPos.z = (chunkZ << 4) + 2;

        this->reset();
        setVillageBiomeType();

        setupPieces();
        m_rng.nextInt(4); // direction: this.setCoordBaseMode(EnumFacing.Plane.HORIZONTAL.random(rand));
        m_isZombieInfested = m_rng.nextInt(50) == 0;

        c_auto startPiece = StructureComponent(PieceType::PT_Village_Start, 0,
            createPieceBB(PieceType::PT_Village_Start, {m_startPos.x, 64, m_startPos.z}, EnumFacing::NORTH),
                EnumFacing::NORTH, 0);
        m_pieceArray[m_pieceArraySize++] = startPiece;

        buildComponentStart(startPiece);

        while (m_pendingRoadArraySize != 0) {
            if (m_generationStep == GS_Village_Valid && hasMoreThanTwoComponents()) {
                return;
            }

            c_int i = m_rng.nextInt(m_pendingRoadArraySize);
            const StructureComponent& structureComponent = m_pieceArray[m_pendingRoadArray[i]];
            m_pendingRoadArraySize--;

            // possible faster way to shift over
            // size_t bytesToShift = (m_pendingRoadArraySize - i) * sizeof(pendingRoadArray[0]);
            // memcpy(&pendingRoadArray[i], &pendingRoadArray[i + 1], bytesToShift);

            for (int j = i; j < m_pendingRoadArraySize; j++) {
                m_pendingRoadArray[j] = m_pendingRoadArray[j + 1];
            }

            buildComponent(structureComponent);

            // stop at blacksmith
            if (m_generationStep == GS_Village_Blacksmith && m_myBlackSmithPieceIndex != -1)
                return;
        }

        if (m_generationStep == GS_Village_Layout) return;

        // encompass structure bounding box
        m_structureBB = BoundingBox::EMPTY;
        for (int index = 0; index < m_pieceArraySize; index++) {
            m_structureBB.encompass(m_pieceArray[index]);
            m_pieceArray[index].variant = m_myVariant;
        }

    }


    void Village::setupPieces() {
        m_currentVillagePW.clear();
        for (c_auto [weight, PlaceCountMin, PlaceCountMax, pieceType]: PIECE_WEIGHTS) {
            if (auto fPiece = FinalPieceWeight(weight,  m_rng.nextInt(PlaceCountMin, PlaceCountMax), 0, pieceType);
                fPiece.m_maxPlaceCount > 0) {
                m_currentVillagePW.push_back(fPiece);
            }
        }
    }


    void Village::buildComponentStart(const StructureComponent& piece) {
        genAndAddRoadPiece({piece.m_minX - 1, piece.m_maxY - 4, piece.m_minZ + 1}, EnumFacing::WEST);
        genAndAddRoadPiece({piece.m_maxX + 1, piece.m_maxY - 4, piece.m_minZ + 1}, EnumFacing::EAST);
        genAndAddRoadPiece({piece.m_minX + 1, piece.m_maxY - 4, piece.m_minZ - 1}, EnumFacing::NORTH);
        genAndAddRoadPiece({piece.m_minX + 1, piece.m_maxY - 4, piece.m_maxZ + 1}, EnumFacing::SOUTH);
    }


    BoundingBox Village::createPieceBB(const PieceType pieceType, const Pos3D pos, const EnumFacing direction) {
        switch (pieceType) {
            case PieceType::PT_Village_Road:
                return BoundingBox::EMPTY;
            case PieceType::PT_Village_Start:
                return {
                    static_cast<bbType_t>(pos.getX()),
                    static_cast<bbType_t>(64),
                    static_cast<bbType_t>(pos.getZ()),
                    static_cast<bbType_t>(pos.getX() + 6 - 1),
                    static_cast<bbType_t>(78),
                    static_cast<bbType_t>(pos.getZ() + 6 - 1)
                };
            case PieceType::PT_Village_House4Garden:
                return BoundingBox::orientBox(pos, 5, 6, 5, direction);
            case PieceType::PT_Village_Church:
                return BoundingBox::orientBox(pos, 5, 12, 9, direction);
            case PieceType::PT_Village_House1:
                return BoundingBox::orientBox(pos, 9, 9, 6, direction);
            case PieceType::PT_Village_WoodHut:
                return BoundingBox::orientBox(pos, 4, 6, 5, direction);
            case PieceType::PT_Village_Hall:
                return BoundingBox::orientBox(pos, 9, 7, 11, direction);
            case PieceType::PT_Village_Field1:
                return BoundingBox::orientBox(pos, 13, 4, 9, direction);
            case PieceType::PT_Village_Field2:
                return BoundingBox::orientBox(pos, 7, 4, 9, direction);
            case PieceType::PT_Village_House2:
                return BoundingBox::orientBox(pos, 10, 6, 7, direction);
            case PieceType::PT_Village_House3:
                return BoundingBox::orientBox(pos, 9, 7, 12, direction);
            case PieceType::PT_Village_Torch:
                return BoundingBox::orientBox(pos, 3, 4, 2, direction);
            case PieceType::PT_Village_NONE:
            default:
                return BoundingBox::EMPTY;
        }
    }


    void Village::genAndAddRoadPiece(const Pos3D pos, const EnumFacing facing) {
        if (abs(m_startPos.x - pos.getX()) > 112 || abs(m_startPos.z - pos.getZ()) > 112) return;

        if (const BoundingBox boundingBox = road(pos, facing); boundingBox.m_maxY != 0) {
            auto sc = StructureComponent(PieceType::PT_Village_Road, 0, boundingBox, facing, boundingBox.getLength() + 1);
            addPiece(sc);
        }
    }


    int Village::updatePieceWeight() const {
        bool flag = false;
        int i = 0;
        for (const FinalPieceWeight& pWeight: m_currentVillagePW) {
            if (pWeight.m_maxPlaceCount > 0 && pWeight.m_amountPlaced < pWeight.m_maxPlaceCount) { flag = true; }
            i += pWeight.m_weight;
        }
        return flag ? i : -1;
    }


    BoundingBox Village::road(const Pos3D pos, const EnumFacing facing) {
        for (int i = 7 * m_rng.nextInt(3, 5); i >= 7; i -= 7) {
            if (BoundingBox structure = BoundingBox::orientBox(pos, 3, 3, i, facing);
                !hasCollisionPiece(structure))
                return structure;
        }
        return {};
    }


    void Village::additionalRngRolls(StructureComponent& p) {
        switch (p.m_type) {
            case PieceType::PT_Village_WoodHut: {
                c_u8 isTallHouse = m_rng.nextBoolean() ? 1 : 0;
                c_u8 tablePosition = static_cast<u8>(m_rng.nextInt(3));
                p.m_data = isTallHouse << 8 | tablePosition;
                return;
            }
            case PieceType::PT_Village_House4Garden: {
                c_i32 isRoofAccessible = m_rng.nextBoolean() ? 1 : 0;
                p.m_data = isRoofAccessible;
                return;
            }
            case PieceType::PT_Village_Field2: {
                c_u8 cropTypeA = static_cast<u8>(m_rng.nextInt(10));
                c_u8 cropTypeB = static_cast<u8>(m_rng.nextInt(10));
                p.m_data = cropTypeA << 4 | cropTypeB;
                return;
            }
            case PieceType::PT_Village_Field1: {
                c_u8 cropTypeA = static_cast<u8>(m_rng.nextInt(10));
                c_u8 cropTypeB = static_cast<u8>(m_rng.nextInt(10));
                c_u8 cropTypeC = static_cast<u8>(m_rng.nextInt(10));
                c_u8 cropTypeD = static_cast<u8>(m_rng.nextInt(10));
                p.m_data = cropTypeA << 12 | cropTypeB << 8 | cropTypeC << 4 | cropTypeD;
                return;
            }
            case PieceType::PT_Village_House2:
            case PieceType::PT_Village_Church:
            case PieceType::PT_Village_House1:
            case PieceType::PT_Village_Hall:
            case PieceType::PT_Village_House3:
            case PieceType::PT_Village_Torch:
            case PieceType::PT_Village_Start:
            case PieceType::PT_Village_Road:
            case PieceType::PT_Village_NONE:
            default:;
        }
    }


    StructureComponent Village::generateComponent(const Pos3D pos, const EnumFacing facing, c_i8 depth) {
        c_int i = updatePieceWeight();
        if (i <= 0) return {};

        int j = 0;

        while (j < 5) {
            ++j;
            int k = m_rng.nextInt(i);

            c_int pieceWeightsSize = static_cast<int>(m_currentVillagePW.size());
            for (size_t pieceTypeNum = 0; pieceTypeNum < pieceWeightsSize; pieceTypeNum++) {
                FinalPieceWeight& pieceWeight = m_currentVillagePW[pieceTypeNum];
                k -= pieceWeight.m_weight;

                if (k < 0) {
                    if ((pieceWeight.m_amountPlaced >= pieceWeight.m_maxPlaceCount
                        || pieceWeight.m_pieceType == m_previousPiece)
                        && m_currentVillagePW.size() > 1) {
                        break;
                    }
                    auto structureVillagePiece = StructureComponent(pieceWeight.m_pieceType, depth, createPieceBB(pieceWeight.m_pieceType, pos, facing), facing, 0);
                    if (!hasCollisionPiece(structureVillagePiece)) {
                        additionalRngRolls(structureVillagePiece);
                        pieceWeight.m_amountPlaced++;
                        m_previousPiece = pieceWeight.m_pieceType;
                        if (pieceWeight.m_amountPlaced >= pieceWeight.m_maxPlaceCount) {
                            m_currentVillagePW.erase(m_currentVillagePW.begin() + pieceTypeNum);
                        }
                        return structureVillagePiece;
                    }
                }
            }
        }
        c_auto torch = StructureComponent(PieceType::PT_Village_Torch, 0,
                                          BoundingBox(createPieceBB(PieceType::PT_Village_Torch, pos, facing)), facing, 0);
        if (hasCollisionPiece(torch)) {
            return {};
        }
        return torch;
    }


    StructureComponent Village::genAndAddComponent(const Pos3D pos, const EnumFacing facing, c_i8 depth) {
        // don't do this for elytra???
        if (depth > 50) return {};
        // 48 for elytra???
        if (abs(m_startPos.x - pos.getX()) > 112 || abs(m_startPos.z - pos.getZ()) > 112) { return {}; }

        StructureComponent structureComponent = generateComponent(pos, facing, static_cast<i8>(depth + 1));
        if (structureComponent.m_type == PieceType::PT_Village_NONE) {
            return {};
        }

        c_int radius = structureComponent.getLength() / 2 + 4;
        if (!this->m_useBiomes || m_g->areBiomesViable(structureComponent.getCenterX(),
                               structureComponent.getCenterZ(),
                               radius,
                               Placement::Village<false>::VALID_BIOMES)) {

            if (structureComponent.m_type == PieceType::PT_Village_House2) {
                m_myBlackSmithPieceIndex = m_pieceArraySize;
            }

            m_pieceArray[m_pieceArraySize++] = structureComponent;
            return structureComponent;
        }

        return {};
    }


    void Village::buildComponent(const StructureComponent& scIn) {
        bool flag = false;

        for (int i = m_rng.nextInt(5); i < scIn.m_data - 8; i += 2 + m_rng.nextInt(5)) {
            StructureComponent sc;
            switch (scIn.m_facing) {
                case EnumFacing::NORTH:
                case EnumFacing::SOUTH:
                default:
                    sc = genAndAddComponent({scIn.m_minX - 1, scIn.m_minY, scIn.m_minZ + i}, EnumFacing::WEST, scIn.m_depth);
                    break;
                case EnumFacing::WEST:
                case EnumFacing::EAST:
                    sc = genAndAddComponent({scIn.m_minX + i, scIn.m_minY, scIn.m_minZ - 1}, EnumFacing::NORTH, scIn.m_depth);
                    break;
            }

            if (sc.m_type != PieceType::PT_Village_NONE) {
                i += sc.getLength() + 1;
                flag = true;
            }
        }

        for (int j = m_rng.nextInt(5); j < scIn.m_data - 8; j += 2 + m_rng.nextInt(5)) {
            StructureComponent sc1;
            switch (scIn.m_facing) {
                case EnumFacing::NORTH:
                case EnumFacing::SOUTH:
                default:
                    sc1 = genAndAddComponent({scIn.m_maxX + 1, scIn.m_minY, scIn.m_minZ + j}, EnumFacing::EAST, scIn.m_depth);
                    break;
                case EnumFacing::EAST:
                case EnumFacing::WEST:
                    sc1 = genAndAddComponent({scIn.m_minX + j, scIn.m_minY, scIn.m_maxZ + 1}, EnumFacing::SOUTH, scIn.m_depth);
                    break;
            }

            if (sc1.m_type != PieceType::PT_Village_NONE) {
                j += sc1.getLength() + 1;
                flag = true;
            }
        }

        if (flag && m_rng.nextInt(3) > 0) {
            switch (scIn.m_facing) {
                case EnumFacing::NORTH:
                default:
                    genAndAddRoadPiece({scIn.m_minX - 1, scIn.m_minY, scIn.m_minZ}, EnumFacing::WEST);
                    break;
                case EnumFacing::SOUTH:
                    genAndAddRoadPiece({scIn.m_minX - 1, scIn.m_minY, scIn.m_maxZ - 2}, EnumFacing::WEST);
                    break;
                case EnumFacing::WEST:
                    genAndAddRoadPiece({scIn.m_minX, scIn.m_minY, scIn.m_minZ - 1}, EnumFacing::NORTH);
                    break;
                case EnumFacing::EAST:
                    genAndAddRoadPiece({scIn.m_maxX - 2, scIn.m_minY, scIn.m_minZ - 1}, EnumFacing::NORTH);
            }
        }

        if (flag && m_rng.nextInt(3) > 0) {
            switch (scIn.m_facing) {
                case EnumFacing::NORTH:
                default:
                    genAndAddRoadPiece({scIn.m_maxX + 1, scIn.m_minY, scIn.m_minZ}, EnumFacing::EAST);
                    break;
                case EnumFacing::SOUTH:
                    genAndAddRoadPiece({scIn.m_maxX + 1, scIn.m_minY, scIn.m_maxZ - 2}, EnumFacing::EAST);
                    break;
                case EnumFacing::WEST:
                    genAndAddRoadPiece({scIn.m_minX, scIn.m_minY, scIn.m_maxZ + 1}, EnumFacing::SOUTH);
                    break;
                case EnumFacing::EAST:
                    genAndAddRoadPiece({scIn.m_maxX - 2, scIn.m_minY, scIn.m_maxZ + 1}, EnumFacing::SOUTH);
            }
        }
    }


    void Village::addPiece(StructureComponent& piece) {
        m_pendingRoadArray[m_pendingRoadArraySize++] = m_pieceArraySize;
        m_pieceArray[m_pieceArraySize++] = piece;
        if (piece.m_type == PieceType::PT_Village_Road) { m_numInvalidPieces++; }
    }


    bool Village::hasCollisionPiece(const BoundingBox& boundingBox) const {
        for (int i = 0; i < m_pieceArraySize; i++) {
            if (m_pieceArray[i].intersects(boundingBox)) { return true; }
        }
        return false;
    }





    MU ND const StructureComponent *Village::getBlackSmithPiece() {
        if (m_myBlackSmithPieceIndex == -1) {
            return nullptr;
        }
        return &m_pieceArray[m_myBlackSmithPieceIndex];

    }


    MU void Village::setVillageBiomeType() {
        const biome_t chosen = m_g->getBiomeIdAt(1, m_startPos);
        switch (chosen) {
            case biome_t::plains:
            default:
                m_myVariant = StructureVariant::SV_Village_Plains;
                break;
            case biome_t::desert:
            case biome_t::desert_hills:
                m_myVariant = StructureVariant::SV_Village_Desert;
                break;
            case biome_t::savanna:
                m_myVariant = StructureVariant::SV_Village_Savanna;
                break;
            case biome_t::taiga:
            case biome_t::ice_plains:
            case biome_t::cold_taiga:
                m_myVariant = StructureVariant::SV_Village_Taiga;
                break;
        }
    }





} // namespace gen

