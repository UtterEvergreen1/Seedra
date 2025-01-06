#include <algorithm>
#include <cmath>

#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "village.hpp"


namespace generation {

    std::map<int8_t, std::string> Village::pieceTypeNames = {
            {PieceType::NONE, "NONE        "},    {PieceType::Start, "Start       "},
            {PieceType::Road, "Road        "},    {PieceType::House4Garden, "House4Garden"},
            {PieceType::Church, "Church      "},  {PieceType::House1, "House1      "},
            {PieceType::WoodHut, "WoodHut     "}, {PieceType::Hall, "Hall        "},
            {PieceType::Field1, "Double Field"},  {PieceType::Field2, "Single Field"},
            {PieceType::House2, "Blacksmith  "},  {PieceType::House3, "House3      "},
            {PieceType::Torch, "Torch       "},
    };


    c_int Village::VILLAGE_SIZE = 0;
    constexpr Village::PieceWeight Village::PIECE_WEIGHTS[9] = {
            {PieceType::House4Garden, 4, 2 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE * 2},
            {PieceType::Church, 20, 0 + Village::VILLAGE_SIZE, 1 + Village::VILLAGE_SIZE},
            {PieceType::House1, 20, 0 + Village::VILLAGE_SIZE, 2 + Village::VILLAGE_SIZE},
            {PieceType::WoodHut, 3, 2 + Village::VILLAGE_SIZE, 5 + Village::VILLAGE_SIZE * 3},
            {PieceType::Hall, 15, 0 + Village::VILLAGE_SIZE, 2 + Village::VILLAGE_SIZE},
            {PieceType::Field1, 3, 1 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE},
            {PieceType::Field2, 3, 2 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE * 2},
            {PieceType::House2, 15, 0, 1 + Village::VILLAGE_SIZE},
            {PieceType::House3, 8, 0 + Village::VILLAGE_SIZE, 3 + Village::VILLAGE_SIZE * 2}};

    Village::Village(const Generator* generator) { g = generator; }

    void Village::generate(c_int chunkX, c_int chunkZ) {
        rng = RNG::getLargeFeatureSeed(g->getWorldSeed(), chunkX, chunkZ);
        rng.advance();
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;
        setupPieces();
        rng.nextInt(4); // direction: this.setCoordBaseMode(EnumFacing.Plane.HORIZONTAL.random(rand));
        isZombieInfested = rng.nextInt(50) == 0; // zombie infested
        const BoundingBox well = createPieceBoundingBox(PieceType::Start, {startX, 64, startZ}, FACING::NORTH);

        // start piece
        c_auto startPiece = Piece(static_cast<i8>(PieceType::Start), 0, well, FACING::NORTH, 0);
        pieceArray[pieceArraySize++] = startPiece;

        buildComponentStart(startPiece);

        while (pendingRoadArraySize != 0) {
            if (generationStep == GenerationStep::VALID && pieceArraySize - numInvalidPieces > 2) {
                hasMoreThanTwoComponents = true;
                return;
            }
            c_int i = rng.nextInt(pendingRoadArraySize);
            const Piece& piece = pieceArray[pendingRoadArray[i]];

            pendingRoadArraySize--;

            // possible faster way to shift over
            // size_t bytesToShift = (pendingRoadArraySize - i) * sizeof(pendingRoadArray[0]);
            // memcpy(&pendingRoadArray[i], &pendingRoadArray[i + 1], bytesToShift);

            for (int j = i; j < pendingRoadArraySize; j++) { pendingRoadArray[j] = pendingRoadArray[j + 1]; }

            buildComponent(piece);

            // stop at blacksmith
            if (blackSmithPiece != nullptr && generationStep == GenerationStep::BLACKSMITH) return;
        }

        if (generationStep == GenerationStep::LAYOUT) return;

        int k = 0;
        for (int index = 0; index < pieceArraySize; index++) {
            if (const Piece& structureComponent = pieceArray[index];
                (structureComponent.type > static_cast<i8>(PieceType::Road)))
                k++;
        }

        structureBoundingBox = BoundingBox::EMPTY;
        for (int index = 0; index < pieceArraySize; index++) { structureBoundingBox.encompass(pieceArray[index]); }

        hasMoreThanTwoComponents = k > 2;
    }


    void Village::setupPieces() {

        currentVillagePW.clear();
        for (c_auto [pieceType, weight, PlaceCountMin, PlaceCountMax]: PIECE_WEIGHTS) {
            if (auto fPiece = FinalPieceWeight(pieceType, weight, rng.nextInt(PlaceCountMin, PlaceCountMax), 0);
                fPiece.maxPlaceCount > 0) {
                currentVillagePW.push_back(fPiece);
            }
        }

        hasMoreThanTwoComponents = false;
        previousPiece = PieceType::NONE;
        blackSmithPiece = nullptr;
        numInvalidPieces = 1;
        pieceArraySize = 0;
        pendingRoadArraySize = 0;
    }


    BoundingBox Village::createPieceBoundingBox(const PieceType pieceType, const Pos3D pos, const FACING direction) {
        switch (pieceType) {
            case PieceType::House4Garden:
                return BoundingBox::orientBox(pos, 5, 6, 5, direction);
            case PieceType::Church:
                return BoundingBox::orientBox(pos, 5, 12, 9, direction);
            case PieceType::House1:
                return BoundingBox::orientBox(pos, 9, 9, 6, direction);
            case PieceType::WoodHut:
                return BoundingBox::orientBox(pos, 4, 6, 5, direction);
            case PieceType::Hall:
                return BoundingBox::orientBox(pos, 9, 7, 11, direction);
            case PieceType::Field1:
                return BoundingBox::orientBox(pos, 13, 4, 9, direction);
            case PieceType::Field2:
                return BoundingBox::orientBox(pos, 7, 4, 9, direction);
            case PieceType::House2:
                return BoundingBox::orientBox(pos, 10, 6, 7, direction);
            case PieceType::House3:
                return BoundingBox::orientBox(pos, 9, 7, 12, direction);
            case PieceType::Start:
                return {pos.getX(), 64, pos.getZ(), pos.getX() + 6 - 1, 78, pos.getZ() + 6 - 1};
            case PieceType::Torch:
                return BoundingBox::orientBox(pos, 3, 4, 2, direction);
            default: // PieceType::NONE
                return BoundingBox::EMPTY;
        }
    }


    Piece Village::genAndAddRoadPiece(const Pos3D pos, const FACING facing) {
        if (abs(startX - pos.getX()) > 112 || abs(startZ - pos.getZ()) > 112) return {};

        if (const BoundingBox boundingBox = road(pos, facing); boundingBox.maxY != 0) {
            c_auto piece =
                    Piece(static_cast<i8>(PieceType::Road), 0, boundingBox, facing, boundingBox.getLength() + 1);
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


    void Village::additionalRngRolls(const Piece& p) {
        switch (static_cast<PieceType>(p.type)) {
            case PieceType::WoodHut:
                rng.nextBoolean(); // isTallHouse
                rng.nextInt(3);    // tablePosition
                return;
            case PieceType::House4Garden:
                rng.nextBoolean(); // isRoofAccessible
                return;
            case PieceType::Field2:
                rng.nextInt(10); // cropTypeA
                rng.nextInt(10); // cropTypeB
                return;
            case PieceType::Field1:
                rng.nextInt(10); // cropTypeA
                rng.nextInt(10); // cropTypeB
                rng.nextInt(10); // cropTypeC
                rng.nextInt(10); // cropTypeD
                return;
            case PieceType::House2:
            case PieceType::Church:
            case PieceType::House1:
            case PieceType::Hall:
            case PieceType::House3:
            case PieceType::Torch:
            case PieceType::Start:
            case PieceType::Road:
            case PieceType::NONE:
            default:;
        }
    }


    Piece Village::generateComponent(const Pos3D pos, const FACING facing, c_i8 depth) {
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
                                Piece(static_cast<i8>(pieceWeight.pieceType), depth,
                                      createPieceBoundingBox(pieceWeight.pieceType, pos, facing), facing, 0);
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
        c_auto torch = Piece(static_cast<i8>(PieceType::Torch), 0,
                                 BoundingBox(createPieceBoundingBox(PieceType::Torch, pos, facing)), facing, 0);
        if (hasCollisionPiece(torch)) return {};
        return torch;
    }


    Piece Village::genAndAddComponent(const Pos3D pos, const FACING facing, c_i8 depth) {
        if (depth > 50) return {};                                                       // don't do this for elytra???
        if (abs(startX - pos.getX()) > 112 || abs(startZ - pos.getZ()) > 112) return {}; // 48 for elytra???

        const Piece structureComponent = generateComponent(pos, facing, static_cast<i8>(depth + 1));
        if (structureComponent.type == static_cast<i8>(PieceType::NONE)) return {};

        if (c_int radius = structureComponent.getLength() / 2 + 4;
            g->areBiomesViable(structureComponent.getCenterX(), structureComponent.getCenterZ(), radius,
                               Placement::Village<false>::VALID_BIOMES)) {

            if (structureComponent.type == static_cast<i8>(PieceType::House2))
                blackSmithPiece = &pieceArray[pieceArraySize];

            pieceArray[pieceArraySize++] = structureComponent;
            return structureComponent;
        }

        return {}; // ------------------------ returning null piece
    }


    void Village::buildComponentStart(const Piece& piece) {
        genAndAddRoadPiece({piece.minX - 1, piece.maxY - 4, piece.minZ + 1}, FACING::WEST);
        genAndAddRoadPiece({piece.maxX + 1, piece.maxY - 4, piece.minZ + 1}, FACING::EAST);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.minZ - 1}, FACING::NORTH);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.maxZ + 1}, FACING::SOUTH);
    }


    void Village::buildComponent(Piece piece) {
        bool flag = false;

        for (int i = rng.nextInt(5); i < piece.additionalData - 8; i += 2 + rng.nextInt(5)) {
            Piece structureComponent;
            switch (piece.orientation) {
                case FACING::NORTH:
                case FACING::SOUTH:
                default:
                    structureComponent = genAndAddComponent({piece.minX - 1, piece.minY, piece.minZ + i},
                                                            FACING::WEST, piece.depth);
                    break;
                case FACING::WEST:
                case FACING::EAST:
                    structureComponent = genAndAddComponent({piece.minX + i, piece.minY, piece.minZ - 1},
                                                            FACING::NORTH, piece.depth);
                    break;
            }

            if (structureComponent.type != static_cast<i8>(PieceType::NONE)) {
                i += structureComponent.getLength() + 1;
                flag = true;
            }
        }

        for (int j = rng.nextInt(5); j < piece.additionalData - 8; j += 2 + rng.nextInt(5)) {
            Piece structureComponent1; // = getNextComponentPP(componentIn, rand, 0, j);
            switch (piece.orientation) {
                case FACING::NORTH:
                case FACING::SOUTH:
                default:
                    structureComponent1 = genAndAddComponent({piece.maxX + 1, piece.minY, piece.minZ + j},
                                                             FACING::EAST, piece.depth);
                    break;
                case FACING::EAST:
                case FACING::WEST:
                    structureComponent1 = genAndAddComponent({piece.minX + j, piece.minY, piece.maxZ + 1},
                                                             FACING::SOUTH, piece.depth);
                    break;
            }

            if (structureComponent1.type != static_cast<i8>(PieceType::NONE)) {
                j += structureComponent1.getLength() + 1;
                flag = true;
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (piece.orientation) {
                case FACING::NORTH:
                default:
                    genAndAddRoadPiece({piece.minX - 1, piece.minY, piece.minZ}, FACING::WEST);
                    break;
                case FACING::SOUTH:
                    genAndAddRoadPiece({piece.minX - 1, piece.minY, piece.maxZ - 2}, FACING::WEST);
                    break;
                case FACING::WEST:
                    genAndAddRoadPiece({piece.minX, piece.minY, piece.minZ - 1}, FACING::NORTH);
                    break;
                case FACING::EAST:
                    genAndAddRoadPiece({piece.maxX - 2, piece.minY, piece.minZ - 1}, FACING::NORTH);
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (piece.orientation) {
                case FACING::NORTH:
                default:
                    genAndAddRoadPiece({piece.maxX + 1, piece.minY, piece.minZ}, FACING::EAST);
                    break;
                case FACING::SOUTH:
                    genAndAddRoadPiece({piece.maxX + 1, piece.minY, piece.maxZ - 2}, FACING::EAST);
                    break;
                case FACING::WEST:
                    genAndAddRoadPiece({piece.minX, piece.minY, piece.maxZ + 1}, FACING::SOUTH);
                    break;
                case FACING::EAST:
                    genAndAddRoadPiece({piece.maxX - 2, piece.minY, piece.maxZ + 1}, FACING::SOUTH);
            }
        }
    }


    void Village::addPiece(const Piece& piece) {
        pendingRoadArray[pendingRoadArraySize++] = pieceArraySize;
        pieceArray[pieceArraySize++] = piece;
        if (piece.type == static_cast<i8>(PieceType::Road)) { numInvalidPieces++; }
    }


    bool Village::hasCollisionPiece(const BoundingBox& boundingBox) const {
        for (int i = 0; i < pieceArraySize; i++) {
            if (pieceArray[i].intersects(boundingBox)) { return true; }
        }
        return false;
    }

} // namespace generation