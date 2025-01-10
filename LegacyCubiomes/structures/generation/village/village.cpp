#include <algorithm>
#include <cmath>

#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "village.hpp"


namespace gen {

    std::map<PieceType, std::string> Village::pieceTypeNames = {
            {PieceType::Village_NONE, "NONE        "},    {PieceType::Village_Start, "Start       "},
            {PieceType::Village_Road, "Road        "},    {PieceType::Village_House4Garden, "House4Garden"},
            {PieceType::Village_Church, "Church      "},  {PieceType::Village_House1, "House1      "},
            {PieceType::Village_WoodHut, "WoodHut     "}, {PieceType::Village_Hall, "Hall        "},
            {PieceType::Village_Field1, "Double Field"},  {PieceType::Village_Field2, "Single Field"},
            {PieceType::Village_House2, "Blacksmith  "},  {PieceType::Village_House3, "House3      "},
            {PieceType::Village_Torch, "Torch       "},
    };


    c_int Village::VILLAGE_SIZE = 0;
    constexpr Village::PieceWeight Village::PIECE_WEIGHTS[9] = {
            {PieceType::Village_House4Garden, 4, 2 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE * 2},
            {PieceType::Village_Church, 20, 0 + Village::VILLAGE_SIZE, 1 + Village::VILLAGE_SIZE},
            {PieceType::Village_House1, 20, 0 + Village::VILLAGE_SIZE, 2 + Village::VILLAGE_SIZE},
            {PieceType::Village_WoodHut, 3, 2 + Village::VILLAGE_SIZE, 5 + Village::VILLAGE_SIZE * 3},
            {PieceType::Village_Hall, 15, 0 + Village::VILLAGE_SIZE, 2 + Village::VILLAGE_SIZE},
            {PieceType::Village_Field1, 3, 1 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE},
            {PieceType::Village_Field2, 3, 2 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE * 2},
            {PieceType::Village_House2, 15, 0, 1 + Village::VILLAGE_SIZE},
            {PieceType::Village_House3, 8, 0 + Village::VILLAGE_SIZE, 3 + Village::VILLAGE_SIZE * 2}};

    Village::Village(const Generator* generator) { g = generator; }

    void Village::generate(c_int chunkX, c_int chunkZ) {
        rng = RNG::getLargeFeatureSeed(g->getWorldSeed(), chunkX, chunkZ);
        rng.advance();
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;

        setupPieces();
        rng.nextInt(4); // direction: this.setCoordBaseMode(EnumFacing.Plane.HORIZONTAL.random(rand));
        isZombieInfested = rng.nextInt(50) == 0;

        c_auto startPiece = StructureComponent(PieceType::Village_Start, 0,
            createPieceBB(PieceType::Village_Start, {startX, 64, startZ}, FACING::NORTH), FACING::NORTH, 0);
        pieceArray[pieceArraySize++] = startPiece;

        buildComponentStart(startPiece);

        while (pendingRoadArraySize != 0) {
            if (generationStep == GenerationStep::VALID && pieceArraySize - numInvalidPieces > 2) {
                myHasMoreThanTwoComponents = true;
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
            if (generationStep == GenerationStep::BLACKSMITH && myBlackSmithPiece != nullptr) return;
        }

        if (generationStep == GenerationStep::LAYOUT) return;

        int k = 0;
        for (int index = 0; index < pieceArraySize; index++) {
            if (const Piece& sc = pieceArray[index]; sc.type > static_cast<i8>(PieceType::Village_Road)) {
                k++;
            }
        }

        structureBB = BoundingBox::EMPTY;
        for (int index = 0; index < pieceArraySize; index++) { structureBB.encompass(pieceArray[index]);
        }

        myHasMoreThanTwoComponents = k > 2;
    }


    void Village::setupPieces() {

        currentVillagePW.clear();
        for (c_auto [pieceType, weight, PlaceCountMin, PlaceCountMax]: PIECE_WEIGHTS) {
            if (auto fPiece = FinalPieceWeight(pieceType, weight, rng.nextInt(PlaceCountMin, PlaceCountMax), 0);
                fPiece.maxPlaceCount > 0) {
                currentVillagePW.push_back(fPiece);
            }
        }

        myHasMoreThanTwoComponents = false;
        previousPiece = NONE;
        myBlackSmithPiece = nullptr;
        numInvalidPieces = 1;
        pieceArraySize = 0;
        pendingRoadArraySize = 0;
    }


    void Village::buildComponentStart(const StructureComponent& piece) {
        genAndAddRoadPiece({piece.minX - 1, piece.maxY - 4, piece.minZ + 1}, FACING::WEST);
        genAndAddRoadPiece({piece.maxX + 1, piece.maxY - 4, piece.minZ + 1}, FACING::EAST);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.minZ - 1}, FACING::NORTH);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.maxZ + 1}, FACING::SOUTH);
    }


    BoundingBox Village::createPieceBB(const PieceType pieceType, const Pos3D pos, const FACING direction) {
        switch (pieceType) {
            case PieceType::Village_Road:
                return BoundingBox::EMPTY;
            case PieceType::Village_Start:
                return {pos.getX(), 64, pos.getZ(), pos.getX() + 6 - 1, 78, pos.getZ() + 6 - 1};
            case PieceType::Village_House4Garden:
                return BoundingBox::orientBox(pos, 5, 6, 5, direction);
            case PieceType::Village_Church:
                return BoundingBox::orientBox(pos, 5, 12, 9, direction);
            case PieceType::Village_House1:
                return BoundingBox::orientBox(pos, 9, 9, 6, direction);
            case PieceType::Village_WoodHut:
                return BoundingBox::orientBox(pos, 4, 6, 5, direction);
            case PieceType::Village_Hall:
                return BoundingBox::orientBox(pos, 9, 7, 11, direction);
            case PieceType::Village_Field1:
                return BoundingBox::orientBox(pos, 13, 4, 9, direction);
            case PieceType::Village_Field2:
                return BoundingBox::orientBox(pos, 7, 4, 9, direction);
            case PieceType::Village_House2:
                return BoundingBox::orientBox(pos, 10, 6, 7, direction);
            case PieceType::Village_House3:
                return BoundingBox::orientBox(pos, 9, 7, 12, direction);
            case PieceType::Village_Torch:
                return BoundingBox::orientBox(pos, 3, 4, 2, direction);
            case NONE:
            default:
                return BoundingBox::EMPTY;
        }
    }


    StructureComponent Village::genAndAddRoadPiece(const Pos3D pos, const FACING facing) {
        if (abs(startX - pos.getX()) > 112 || abs(startZ - pos.getZ()) > 112) return {};

        if (const BoundingBox boundingBox = road(pos, facing); boundingBox.maxY != 0) {
            c_auto piece = StructureComponent(PieceType::Village_Road, 0, boundingBox, facing, boundingBox.getLength() + 1);
            addPiece(piece);
            return piece;
        }
        return {};
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


    BoundingBox Village::road(const Pos3D pos, const FACING facing) {
        for (int i = 7 * rng.nextInt(3, 5); i >= 7; i -= 7) {
            if (BoundingBox structure = BoundingBox::orientBox(pos, 3, 3, i, facing); !hasCollisionPiece(structure))
                return structure;
        }
        return {};
    }


    void Village::additionalRngRolls(Piece& p) {
        switch (static_cast<PieceType>(p.type)) {
            case PieceType::Village_WoodHut: {
                c_u8 isTallHouse = rng.nextBoolean() ? 1 : 0;
                c_u8 tablePosition = rng.nextInt(3);
                p.additionalData = isTallHouse << 8 | tablePosition;
                return;
            }
            case PieceType::Village_House4Garden: {
                c_i32 isRoofAccessible = rng.nextBoolean() ? 1 : 0;
                p.additionalData = isRoofAccessible;
                return;
            }
            case PieceType::Village_Field2: {
                c_u8 cropTypeA = rng.nextInt(10);
                c_u8 cropTypeB = rng.nextInt(10);
                p.additionalData = cropTypeA << 5 | cropTypeB;
                return;
            }
            case PieceType::Village_Field1: {
                c_u8 cropTypeA = rng.nextInt(10);
                c_u8 cropTypeB = rng.nextInt(10);
                c_u8 cropTypeC = rng.nextInt(10);
                c_u8 cropTypeD = rng.nextInt(10);
                p.additionalData = cropTypeA << 15 | cropTypeB << 10 | cropTypeC << 5 | cropTypeD;
                return;
            }
            case PieceType::Village_House2:
            case PieceType::Village_Church:
            case PieceType::Village_House1:
            case PieceType::Village_Hall:
            case PieceType::Village_House3:
            case PieceType::Village_Torch:
            case PieceType::Village_Start:
            case PieceType::Village_Road:
            case PieceType::Village_NONE:
            default:;
        }
    }


    StructureComponent Village::generateComponent(const Pos3D pos, const FACING facing, c_i8 depth) {
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

                    if (auto structureVillagePiece =
                                StructureComponent(pieceWeight.pieceType, depth,
                                      createPieceBB(pieceWeight.pieceType, pos, facing), facing, 0);
                        !hasCollisionPiece(structureVillagePiece)) {
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
        c_auto torch = StructureComponent(PieceType::Village_Torch, 0, BoundingBox(createPieceBB(PieceType::Village_Torch, pos, facing)), facing, 0);
        if (hasCollisionPiece(torch)) return {};
        return torch;
    }


    StructureComponent Village::genAndAddComponent(const Pos3D pos, const FACING facing, c_i8 depth) {
        if (depth > 50) return {};                                                       // don't do this for elytra???
        if (abs(startX - pos.getX()) > 112 || abs(startZ - pos.getZ()) > 112) return {}; // 48 for elytra???

        const StructureComponent structureComponent = generateComponent(pos, facing, static_cast<i8>(depth + 1));
        if (structureComponent.type == static_cast<i8>(PieceType::NONE)) return {};

        if (c_int radius = structureComponent.getLength() / 2 + 4;
            g->areBiomesViable(structureComponent.getCenterX(),
                               structureComponent.getCenterZ(),
                               radius,
                               Placement::Village<false>::VALID_BIOMES)) {

            if (structureComponent.type == static_cast<i8>(PieceType::Village_House2))
                myBlackSmithPiece = &pieceArray[pieceArraySize];

            pieceArray[pieceArraySize++] = structureComponent;
            return structureComponent;
        }

        return {};
    }


    void Village::buildComponent(const StructureComponent& scIn) {
        bool flag = false;

        for (int i = rng.nextInt(5); i < scIn.additionalData - 8; i += 2 + rng.nextInt(5)) {
            StructureComponent sc;
            switch (scIn.orientation) {
                case FACING::NORTH:
                case FACING::SOUTH:
                default:
                    sc = genAndAddComponent({scIn.minX - 1, scIn.minY, scIn.minZ + i}, FACING::WEST, scIn.depth);
                    break;
                case FACING::WEST:
                case FACING::EAST:
                    sc = genAndAddComponent({scIn.minX + i, scIn.minY, scIn.minZ - 1}, FACING::NORTH, scIn.depth);
                    break;
            }

            if (sc.type != static_cast<i8>(NONE)) {
                i += sc.getLength() + 1;
                flag = true;
            }
        }

        for (int j = rng.nextInt(5); j < scIn.additionalData - 8; j += 2 + rng.nextInt(5)) {
            StructureComponent sc1;
            switch (scIn.orientation) {
                case FACING::NORTH:
                case FACING::SOUTH:
                default:
                    sc1 = genAndAddComponent({scIn.maxX + 1, scIn.minY, scIn.minZ + j}, FACING::EAST, scIn.depth);
                    break;
                case FACING::EAST:
                case FACING::WEST:
                    sc1 = genAndAddComponent({scIn.minX + j, scIn.minY, scIn.maxZ + 1}, FACING::SOUTH, scIn.depth);
                    break;
            }

            if (sc1.type != static_cast<i8>(NONE)) {
                j += sc1.getLength() + 1;
                flag = true;
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (scIn.orientation) {
                case FACING::NORTH:
                default:
                    genAndAddRoadPiece({scIn.minX - 1, scIn.minY, scIn.minZ}, FACING::WEST);
                    break;
                case FACING::SOUTH:
                    genAndAddRoadPiece({scIn.minX - 1, scIn.minY, scIn.maxZ - 2}, FACING::WEST);
                    break;
                case FACING::WEST:
                    genAndAddRoadPiece({scIn.minX, scIn.minY, scIn.minZ - 1}, FACING::NORTH);
                    break;
                case FACING::EAST:
                    genAndAddRoadPiece({scIn.maxX - 2, scIn.minY, scIn.minZ - 1}, FACING::NORTH);
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (scIn.orientation) {
                case FACING::NORTH:
                default:
                    genAndAddRoadPiece({scIn.maxX + 1, scIn.minY, scIn.minZ}, FACING::EAST);
                    break;
                case FACING::SOUTH:
                    genAndAddRoadPiece({scIn.maxX + 1, scIn.minY, scIn.maxZ - 2}, FACING::EAST);
                    break;
                case FACING::WEST:
                    genAndAddRoadPiece({scIn.minX, scIn.minY, scIn.maxZ + 1}, FACING::SOUTH);
                    break;
                case FACING::EAST:
                    genAndAddRoadPiece({scIn.maxX - 2, scIn.minY, scIn.maxZ + 1}, FACING::SOUTH);
            }
        }
    }


    void Village::addPiece(const StructureComponent& piece) {
        pendingRoadArray[pendingRoadArraySize++] = pieceArraySize;
        pieceArray[pieceArraySize++] = piece;
        if (piece.type == static_cast<i8>(PieceType::Village_Road)) { numInvalidPieces++; }
    }


    bool Village::hasCollisionPiece(const BoundingBox& boundingBox) const {
        for (int i = 0; i < pieceArraySize; i++) {
            if (pieceArray[i].intersects(boundingBox)) { return true; }
        }
        return false;
    }

} // namespace generation