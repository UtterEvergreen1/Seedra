#include <algorithm>
#include <cmath>

#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
#include "village.hpp"


namespace generation {

    std::map<int, std::string> Village::pieceTypeNames = {
            {NONE, "NONE        "},         {Start, "Start       "},  {Road, "Road        "},
            {House4Garden, "House4Garden"}, {Church, "Church      "}, {House1, "House1      "},
            {WoodHut, "WoodHut     "},      {Hall, "Hall        "},   {Field1, "Field1      "},
            {Field2, "Field2      "},       {House2, "House2      "}, {House3, "House3      "},
            {Torch, "Torch       "},
    };


    const int Village::VILLAGE_SIZE = 0;
    const Village::PieceWeight Village::PIECE_WEIGHTS[9] = {
            {Village::PieceType::House4Garden, 4, 2 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE * 2},
            {Village::PieceType::Church, 20, 0 + Village::VILLAGE_SIZE, 1 + Village::VILLAGE_SIZE},
            {Village::PieceType::House1, 20, 0 + Village::VILLAGE_SIZE, 2 + Village::VILLAGE_SIZE},
            {Village::PieceType::WoodHut, 3, 2 + Village::VILLAGE_SIZE, 5 + Village::VILLAGE_SIZE * 3},
            {Village::PieceType::Hall, 15, 0 + Village::VILLAGE_SIZE, 2 + Village::VILLAGE_SIZE},
            {Village::PieceType::Field1, 3, 1 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE},
            {Village::PieceType::Field2, 3, 2 + Village::VILLAGE_SIZE, 4 + Village::VILLAGE_SIZE * 2},
            {Village::PieceType::House2, 15, 0, 1 + Village::VILLAGE_SIZE},
            {Village::PieceType::House3, 8, 0 + Village::VILLAGE_SIZE, 3 + Village::VILLAGE_SIZE * 2}};

    Village::Village(const Generator* generator) { g = generator; }

    void Village::generate(int chunkX, int chunkZ) {
        rng = RNG::getLargeFeatureSeed(g->getWorldSeed(), chunkX, chunkZ);
        rng.advance();
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;
        setupPieces();
        rng.nextInt(4); // direction: this.setCoordBaseMode(EnumFacing.Plane.HORIZONTAL.random(rand));
        isZombieInfested = rng.nextInt(50) == 0; // zombie infested
        BoundingBox well = createPieceBoundingBox(PieceType::Start, {startX, 64, startZ}, DIRECTION::NORTH);

        // start piece
        Piece startPiece = Piece(PieceType::Start, 0, well, DIRECTION::NORTH, 0);
        pieceArray[pieceArraySize++] = startPiece;

        buildComponentStart(startPiece);

        while (pendingRoadArraySize != 0) {
            int i = rng.nextInt(pendingRoadArraySize);
            Piece& piece = pieceArray[pendingRoadArray[i]];

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
            const Piece& structureComponent = pieceArray[index];
            if ((structureComponent.type > PieceType::Road)) k++;
        }

        hasMoreThanTwoComponents = k > 2;
    }


    void Village::setupPieces() {

        currentVillagePW.clear();
        for (auto pieceWeightAt: PIECE_WEIGHTS) {
            FinalPieceWeight fPiece =
                    FinalPieceWeight(pieceWeightAt.pieceType, pieceWeightAt.weight,
                                     rng.nextInt(pieceWeightAt.PlaceCountMin, pieceWeightAt.PlaceCountMax),
                                     0); //random set max amount of houses for each house because it is not set
            if (fPiece.maxPlaceCount > 0) { currentVillagePW.push_back(fPiece); }
        }

        // hasMoreThanTwoComponents = false;
        previousPiece = PieceType::NONE;
        blackSmithPiece = nullptr;
        pieceArraySize = 0;
        pendingRoadArraySize = 0;
    }


    BoundingBox Village::createPieceBoundingBox(int pieceType, Pos3D pos, DIRECTION direction) {
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


    Piece Village::genAndAddRoadPiece(Pos3D pos, DIRECTION facing) {
        if (abs(startX - pos.getX()) > 112 || abs(startZ - pos.getZ()) > 112) return {};

        BoundingBox boundingBox = road(pos, facing);

        if (boundingBox.maxY != 0) {
            Piece piece = Piece(PieceType::Road, 0, boundingBox, facing, boundingBox.getLength() + 1);
            addPiece(piece);
            return piece;
        }

        return {};
    }


    int Village::updatePieceWeight() {
        bool flag = false;
        int i = 0;
        for (const FinalPieceWeight& pWeight: currentVillagePW) {
            if (pWeight.maxPlaceCount > 0 && pWeight.amountPlaced < pWeight.maxPlaceCount) { flag = true; }
            i += pWeight.weight;
        }
        return flag ? i : -1;
    }


    BoundingBox Village::road(Pos3D pos, DIRECTION facing) {
        for (int i = 7 * rng.nextInt(3, 5); i >= 7; i -= 7) {
            // bool flag = true;
            BoundingBox structure = BoundingBox::orientBox(pos, 3, 3, i, facing);
            if (!hasCollisionPiece(structure)) return structure;
        }
        return {};
    }


    void Village::additionalRngRolls(const Piece& p) {
        switch (p.type) {
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
                return;
        }
    }


    Piece Village::generateComponent(Pos3D pos, DIRECTION facing, int8_t depth) {
        int i = updatePieceWeight();
        if (i <= 0) return {};

        int j = 0;

        while (j < 5) {
            ++j;
            int k = rng.nextInt(i);

            int pieceWeightsSize = (int) currentVillagePW.size();
            for (int pieceTypeNum = 0; pieceTypeNum < pieceWeightsSize; pieceTypeNum++) {
                FinalPieceWeight& pieceWeight = currentVillagePW[pieceTypeNum];
                k -= pieceWeight.weight;

                if (k < 0) {
                    if (pieceWeight.amountPlaced >= pieceWeight.maxPlaceCount ||
                        pieceWeight.pieceType == previousPiece && currentVillagePW.size() > 1) {
                        break;
                    }

                    Piece structureVillagePiece =
                            Piece(pieceWeight.pieceType, depth,
                                  createPieceBoundingBox(pieceWeight.pieceType, pos, facing), facing, 0);
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
        Piece torch = Piece(PieceType::Torch, 0, BoundingBox(createPieceBoundingBox(PieceType::Torch, pos, facing)),
                            facing, 0);
        if (hasCollisionPiece(torch)) return {};
        return torch;
    }


    Piece Village::genAndAddComponent(Pos3D pos, DIRECTION facing, int8_t depth) {
        if (depth > 50) return {};                                                       // don't do this for elytra???
        if (abs(startX - pos.getX()) > 112 || abs(startZ - pos.getZ()) > 112) return {}; // 48 for elytra???

        Piece structureComponent = generateComponent(pos, facing, (int8_t) (depth + 1));
        if (structureComponent.type == PieceType::NONE) return {};

        int radius = (structureComponent.getLength() / 2) + 4;
        if (g->areBiomesViable(structureComponent.getCenterX(), structureComponent.getCenterZ(), radius,
                               Placement::Village<false>::VALID_BIOMES)) {

            if (structureComponent.type == PieceType::House2) blackSmithPiece = &pieceArray[pieceArraySize];

            pieceArray[pieceArraySize++] = structureComponent;
            return structureComponent;
        }

        return {}; // ------------------------ returning null piece
    }


    void Village::buildComponentStart(Piece piece) {
        genAndAddRoadPiece({piece.minX - 1, piece.maxY - 4, piece.minZ + 1}, DIRECTION::WEST);
        genAndAddRoadPiece({piece.maxX + 1, piece.maxY - 4, piece.minZ + 1}, DIRECTION::EAST);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.minZ - 1}, DIRECTION::NORTH);
        genAndAddRoadPiece({piece.minX + 1, piece.maxY - 4, piece.maxZ + 1}, DIRECTION::SOUTH);
    }


    void Village::buildComponent(Piece piece) {
        bool flag = false;

        for (int i = rng.nextInt(5); i < piece.additionalData - 8; i += 2 + rng.nextInt(5)) {
            Piece structureComponent;
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                case DIRECTION::SOUTH:
                default:
                    structureComponent = genAndAddComponent({piece.minX - 1, piece.minY, piece.minZ + i},
                                                            DIRECTION::WEST, piece.depth);
                    break;
                case DIRECTION::WEST:
                case DIRECTION::EAST:
                    structureComponent = genAndAddComponent({piece.minX + i, piece.minY, piece.minZ - 1},
                                                            DIRECTION::NORTH, piece.depth);
                    break;
            }

            if (structureComponent.type != PieceType::NONE) {
                i += structureComponent.getLength() + 1;
                flag = true;
            }
        }

        for (int j = rng.nextInt(5); j < piece.additionalData - 8; j += 2 + rng.nextInt(5)) {
            Piece structureComponent1; // = getNextComponentPP(componentIn, rand, 0, j);
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                case DIRECTION::SOUTH:
                default:
                    structureComponent1 = genAndAddComponent({piece.maxX + 1, piece.minY, piece.minZ + j},
                                                             DIRECTION::EAST, piece.depth);
                    break;
                case DIRECTION::EAST:
                case DIRECTION::WEST:
                    structureComponent1 = genAndAddComponent({piece.minX + j, piece.minY, piece.maxZ + 1},
                                                             DIRECTION::SOUTH, piece.depth);
                    break;
            }

            if (structureComponent1.type != PieceType::NONE) {
                j += structureComponent1.getLength() + 1;
                flag = true;
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                default:
                    genAndAddRoadPiece({piece.minX - 1, piece.minY, piece.minZ}, DIRECTION::WEST);
                    break;
                case DIRECTION::SOUTH:
                    genAndAddRoadPiece({piece.minX - 1, piece.minY, piece.maxZ - 2}, DIRECTION::WEST);
                    break;
                case DIRECTION::WEST:
                    genAndAddRoadPiece({piece.minX, piece.minY, piece.minZ - 1}, DIRECTION::NORTH);
                    break;
                case DIRECTION::EAST:
                    genAndAddRoadPiece({piece.maxX - 2, piece.minY, piece.minZ - 1}, DIRECTION::NORTH);
            }
        }

        if (flag && rng.nextInt(3) > 0) {
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                default:
                    genAndAddRoadPiece({piece.maxX + 1, piece.minY, piece.minZ}, DIRECTION::EAST);
                    break;
                case DIRECTION::SOUTH:
                    genAndAddRoadPiece({piece.maxX + 1, piece.minY, piece.maxZ - 2}, DIRECTION::EAST);
                    break;
                case DIRECTION::WEST:
                    genAndAddRoadPiece({piece.minX, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH);
                    break;
                case DIRECTION::EAST:
                    genAndAddRoadPiece({piece.maxX - 2, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH);
            }
        }
    }


    void Village::addPiece(Piece& piece) {
        pendingRoadArray[pendingRoadArraySize++] = pieceArraySize;
        pieceArray[pieceArraySize++] = piece;
    }


    bool Village::hasCollisionPiece(const BoundingBox& boundingBox) {
        for (int i = 0; i < pieceArraySize; i++) {
            if (pieceArray[i].intersects(boundingBox)) { return true; }
        }
        return false;
    }

} // namespace generation