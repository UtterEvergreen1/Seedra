#include "village.hpp"

#include <algorithm>

#include "LegacyCubiomes/cubiomes/biomeID.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"


#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/block_states.hpp"
#include "lce/blocks/blocks.hpp"


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
            {PieceType::PT_Village_House4Garden, 4, 2 + VILLAGE_SIZE, 4 + VILLAGE_SIZE * 2},
            {PieceType::PT_Village_Church, 20, 0 + VILLAGE_SIZE, 1 + VILLAGE_SIZE},
            {PieceType::PT_Village_House1, 20, 0 + VILLAGE_SIZE, 2 + VILLAGE_SIZE},
            {PieceType::PT_Village_WoodHut, 3, 2 + VILLAGE_SIZE, 5 + VILLAGE_SIZE * 3},
            {PieceType::PT_Village_Hall, 15, 0 + VILLAGE_SIZE, 2 + VILLAGE_SIZE},
            {PieceType::PT_Village_Field1, 3, 1 + VILLAGE_SIZE, 4 + VILLAGE_SIZE},
            {PieceType::PT_Village_Field2, 3, 2 + VILLAGE_SIZE, 4 + VILLAGE_SIZE * 2},
            {PieceType::PT_Village_House2, 15, 0, 1 + VILLAGE_SIZE},
            {PieceType::PT_Village_House3, 8, 0 + VILLAGE_SIZE, 3 + VILLAGE_SIZE * 2}};



    Village::Village(const Generator* generator) { g = generator; }

    Village::~Village() {
        this->reset();
    }

    void Village::reset() {
        // for (int i = 0; i < pieceArraySize; i++) {
        //     delete pieceArray[i];
        // }
        previousPiece = PieceType::PT_Village_NONE;
        myBlackSmithPieceIndex = -1;
        numInvalidPieces = 1;
        pieceArraySize = 0;
        pendingRoadArraySize = 0;
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
        rng = RNG::getLargeFeatureSeed(g->getWorldSeed(), chunkX, chunkZ);
        rng.advance();
        startPos.x = (chunkX << 4) + 2;
        startPos.z = (chunkZ << 4) + 2;

        this->reset();
        setVillageBiomeType();

        setupPieces();
        rng.nextInt(4); // direction: this.setCoordBaseMode(EnumFacing.Plane.HORIZONTAL.random(rand));
        isZombieInfested = rng.nextInt(50) == 0;

        c_auto startPiece = StructureComponent(PieceType::PT_Village_Start, 0,
            createPieceBB(PieceType::PT_Village_Start, {startPos.x, 64, startPos.z}, enumFacing::NORTH), enumFacing::NORTH, 0);
        pieceArray[pieceArraySize++] = startPiece;

        buildComponentStart(startPiece);

        while (pendingRoadArraySize != 0) {
            if (generationStep == GS_Village_Valid && hasMoreThanTwoComponents()) {
                return;
            }

            c_int i = rng.nextInt(pendingRoadArraySize);
            const StructureComponent& structureComponent = pieceArray[pendingRoadArray[i]];
            pendingRoadArraySize--;

            // possible faster way to shift over
            // size_t bytesToShift = (pendingRoadArraySize - i) * sizeof(pendingRoadArray[0]);
            // memcpy(&pendingRoadArray[i], &pendingRoadArray[i + 1], bytesToShift);

            for (int j = i; j < pendingRoadArraySize; j++) {
                pendingRoadArray[j] = pendingRoadArray[j + 1];
            }

            buildComponent(structureComponent);

            // stop at blacksmith
            if (generationStep == GS_Village_Blacksmith && myBlackSmithPieceIndex != -1)
                return;
        }

        if (generationStep == GS_Village_Layout) return;

        // encompass structure bounding box
        structureBB = BoundingBox::EMPTY;
        for (int index = 0; index < pieceArraySize; index++) {
            structureBB.encompass(pieceArray[index]);
            pieceArray[index].structureType = biomeType;
        }

    }


    void Village::setupPieces() {
        currentVillagePW.clear();
        for (c_auto [pieceType, weight, PlaceCountMin, PlaceCountMax]: PIECE_WEIGHTS) {
            if (auto fPiece = FinalPieceWeight(pieceType, weight,
                                               rng.nextInt(PlaceCountMin, PlaceCountMax), 0);
                fPiece.maxPlaceCount > 0) {
                currentVillagePW.push_back(fPiece);
            }
        }
    }


    void Village::buildComponentStart(const StructureComponent& piece) {
        genAndAddRoadPiece({piece.minX - 1, piece.maxY - 4, piece.minZ + 1}, enumFacing::WEST);
        genAndAddRoadPiece({piece.maxX + 1, piece.maxY - 4, piece.minZ + 1}, enumFacing::EAST);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.minZ - 1}, enumFacing::NORTH);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.maxZ + 1}, enumFacing::SOUTH);
    }


    BoundingBox Village::createPieceBB(const PieceType pieceType, const Pos3D pos, const enumFacing direction) {
        switch (pieceType) {
            case PieceType::PT_Village_Road:
                return BoundingBox::EMPTY;
            case PieceType::PT_Village_Start:
                return {pos.getX(), 64, pos.getZ(), pos.getX() + 6 - 1, 78, pos.getZ() + 6 - 1};
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


    void Village::genAndAddRoadPiece(const Pos3D pos, const enumFacing facing) {
        if (abs(startPos.x - pos.getX()) > 112 || abs(startPos.z - pos.getZ()) > 112) return;

        if (const BoundingBox boundingBox = road(pos, facing); boundingBox.maxY != 0) {
            auto sc = StructureComponent(PieceType::PT_Village_Road, 0, boundingBox, facing, boundingBox.getLength() + 1);
            addPiece(sc);
        }
    }


    int Village::updatePieceWeight() const {
        bool flag = false;
        int i = 0;
        for (const FinalPieceWeight& pWeight: currentVillagePW) {
            if (pWeight.maxPlaceCount > 0 && pWeight.amountPlaced < pWeight.maxPlaceCount) { flag = true; }
            i += pWeight.weight;
        }
        return flag ? i : -1;
    }


    BoundingBox Village::road(const Pos3D pos, const enumFacing facing) {
        for (int i = 7 * rng.nextInt(3, 5); i >= 7; i -= 7) {
            if (BoundingBox structure = BoundingBox::orientBox(pos, 3, 3, i, facing);
                !hasCollisionPiece(structure))
                return structure;
        }
        return {};
    }


    void Village::additionalRngRolls(Piece& p) {
        switch (static_cast<PieceType>(p.type)) {
            case PieceType::PT_Village_WoodHut: {
                c_u8 isTallHouse = rng.nextBoolean() ? 1 : 0;
                c_u8 tablePosition = rng.nextInt(3);
                p.data = isTallHouse << 8 | tablePosition;
                return;
            }
            case PieceType::PT_Village_House4Garden: {
                c_i32 isRoofAccessible = rng.nextBoolean() ? 1 : 0;
                p.data = isRoofAccessible;
                return;
            }
            case PieceType::PT_Village_Field2: {
                c_u8 cropTypeA = rng.nextInt(10);
                c_u8 cropTypeB = rng.nextInt(10);
                p.data = cropTypeA << 4 | cropTypeB;
                return;
            }
            case PieceType::PT_Village_Field1: {
                c_u8 cropTypeA = rng.nextInt(10);
                c_u8 cropTypeB = rng.nextInt(10);
                c_u8 cropTypeC = rng.nextInt(10);
                c_u8 cropTypeD = rng.nextInt(10);
                p.data = cropTypeA << 12 | cropTypeB << 8 | cropTypeC << 4 | cropTypeD;
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


    StructureComponent Village::generateComponent(const Pos3D pos, const enumFacing facing, c_i8 depth) {
        c_int i = updatePieceWeight();
        if (i <= 0) return {};

        int j = 0;

        while (j < 5) {
            ++j;
            int k = rng.nextInt(i);

            c_int pieceWeightsSize = static_cast<int>(currentVillagePW.size());
            for (int pieceTypeNum = 0; pieceTypeNum < pieceWeightsSize; pieceTypeNum++) {
                FinalPieceWeight& pieceWeight = currentVillagePW[pieceTypeNum];
                k -= pieceWeight.weight;

                if (k < 0) {
                    if (pieceWeight.amountPlaced >= pieceWeight.maxPlaceCount ||
                        pieceWeight.pieceType == previousPiece && currentVillagePW.size() > 1) {
                        break;
                    }
                    auto structureVillagePiece = StructureComponent(pieceWeight.pieceType, depth, createPieceBB(pieceWeight.pieceType, pos, facing), facing, 0);
                    if (!hasCollisionPiece(structureVillagePiece)) {
                        additionalRngRolls(structureVillagePiece);
                        pieceWeight.amountPlaced++;
                        previousPiece = pieceWeight.pieceType;
                        if (pieceWeight.amountPlaced >= pieceWeight.maxPlaceCount) {
                            currentVillagePW.erase(currentVillagePW.begin() + pieceTypeNum);
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


    StructureComponent Village::genAndAddComponent(const Pos3D pos, const enumFacing facing, c_i8 depth) {
        // don't do this for elytra???
        if (depth > 50) return {};
        // 48 for elytra???
        if (abs(startPos.x - pos.getX()) > 112 || abs(startPos.z - pos.getZ()) > 112) { return {}; }

        StructureComponent structureComponent = generateComponent(pos, facing, static_cast<i8>(depth + 1));
        if (structureComponent.type == PieceType::PT_Village_NONE) {
            return {};
        }

        if (c_int radius = structureComponent.getLength() / 2 + 4;
            g->areBiomesViable(structureComponent.getCenterX(),
                               structureComponent.getCenterZ(),
                               radius,
                               Placement::Village<false>::VALID_BIOMES)) {

            if (structureComponent.type == PieceType::PT_Village_House2) {
                myBlackSmithPieceIndex = pieceArraySize;
            }

            pieceArray[pieceArraySize++] = structureComponent;
            return structureComponent;
        }

        return {};
    }


    void Village::buildComponent(const StructureComponent& scIn) {
        bool flag = false;

        for (int i = rng.nextInt(5); i < scIn.data - 8; i += 2 + rng.nextInt(5)) {
            StructureComponent sc;
            switch (scIn.facing) {
                case enumFacing::NORTH:
                case enumFacing::SOUTH:
                default:
                    sc = genAndAddComponent({scIn.minX - 1, scIn.minY, scIn.minZ + i}, enumFacing::WEST, scIn.depth);
                    break;
                case enumFacing::WEST:
                case enumFacing::EAST:
                    sc = genAndAddComponent({scIn.minX + i, scIn.minY, scIn.minZ - 1}, enumFacing::NORTH, scIn.depth);
                    break;
            }

            if (sc.type != PieceType::PT_Village_NONE) {
                i += sc.getLength() + 1;
                flag = true;
            }
        }

        for (int j = rng.nextInt(5); j < scIn.data - 8; j += 2 + rng.nextInt(5)) {
            StructureComponent sc1;
            switch (scIn.facing) {
                case enumFacing::NORTH:
                case enumFacing::SOUTH:
                default:
                    sc1 = genAndAddComponent({scIn.maxX + 1, scIn.minY, scIn.minZ + j}, enumFacing::EAST, scIn.depth);
                    break;
                case enumFacing::EAST:
                case enumFacing::WEST:
                    sc1 = genAndAddComponent({scIn.minX + j, scIn.minY, scIn.maxZ + 1}, enumFacing::SOUTH, scIn.depth);
                    break;
            }

            if (sc1.type != PieceType::PT_Village_NONE) {
                j += sc1.getLength() + 1;
                flag = true;
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (scIn.facing) {
                case enumFacing::NORTH:
                default:
                    genAndAddRoadPiece({scIn.minX - 1, scIn.minY, scIn.minZ}, enumFacing::WEST);
                    break;
                case enumFacing::SOUTH:
                    genAndAddRoadPiece({scIn.minX - 1, scIn.minY, scIn.maxZ - 2}, enumFacing::WEST);
                    break;
                case enumFacing::WEST:
                    genAndAddRoadPiece({scIn.minX, scIn.minY, scIn.minZ - 1}, enumFacing::NORTH);
                    break;
                case enumFacing::EAST:
                    genAndAddRoadPiece({scIn.maxX - 2, scIn.minY, scIn.minZ - 1}, enumFacing::NORTH);
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (scIn.facing) {
                case enumFacing::NORTH:
                default:
                    genAndAddRoadPiece({scIn.maxX + 1, scIn.minY, scIn.minZ}, enumFacing::EAST);
                    break;
                case enumFacing::SOUTH:
                    genAndAddRoadPiece({scIn.maxX + 1, scIn.minY, scIn.maxZ - 2}, enumFacing::EAST);
                    break;
                case enumFacing::WEST:
                    genAndAddRoadPiece({scIn.minX, scIn.minY, scIn.maxZ + 1}, enumFacing::SOUTH);
                    break;
                case enumFacing::EAST:
                    genAndAddRoadPiece({scIn.maxX - 2, scIn.minY, scIn.maxZ + 1}, enumFacing::SOUTH);
            }
        }
    }


    void Village::addPiece(StructureComponent& piece) {
        pendingRoadArray[pendingRoadArraySize++] = pieceArraySize;
        pieceArray[pieceArraySize++] = piece;
        if (piece.type == PieceType::PT_Village_Road) { numInvalidPieces++; }
    }


    bool Village::hasCollisionPiece(const BoundingBox& boundingBox) const {
        for (int i = 0; i < pieceArraySize; i++) {
            if (pieceArray[i].intersects(boundingBox)) { return true; }
        }
        return false;
    }





    MU ND const StructureComponent *Village::getBlackSmithPiece() {
        if (myBlackSmithPieceIndex == -1) {
            return nullptr;
        }
        return &pieceArray[myBlackSmithPieceIndex];

    }


    MU void Village::setVillageBiomeType() {
        switch (g->getBiomeAt(1, startPos)) {
            case BiomeID::plains:
            default:
                biomeType = 0;
                break;
            case BiomeID::desert:
                biomeType = 1;
                break;
            case BiomeID::savanna:
                biomeType = 2;
                break;
            case BiomeID::taiga:
            case BiomeID::ice_plains:
            case BiomeID::cold_taiga:
                biomeType = 3;
                break;
        }
    }





} // namespace gen

