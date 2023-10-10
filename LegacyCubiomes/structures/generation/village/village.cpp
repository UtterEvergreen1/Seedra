#include <iostream>
#include <cmath>
#include <algorithm>

#include "village.hpp"
#include "LegacyCubiomes/structures/placement/StaticStructures.hpp"
namespace generation {
    Village::Village(const Generator* generator) {
        g = generator;
    }

    void Village::generate(int chunkX, int chunkZ) {
        uint64_t random = getLargeFeatureSeed(g->getWorldSeed(), chunkX, chunkZ);
        random = (random * 0x5deece66d + 0xb) & 0xFFFFFFFFFFFF; // advance rng
        startX = (chunkX << 4) + 2;
        startZ = (chunkZ << 4) + 2;
        setupPieces(&random);
        nextInt(&random, 4); // direction: this.setCoordBaseMode(EnumFacing.Plane.HORIZONTAL.random(rand));
        isZombieInfested = nextInt(&random, 50) == 0; // zombie infested
        BoundingBox well = createPieceBoundingBox(PieceType::Start, {startX, 64, startZ}, DIRECTION::NORTH);

        // start piece
        Piece startPiece = Piece(PieceType::Start, 0, well, DIRECTION::NORTH, 0);
        pieces[piecesSize++] = startPiece;

        buildComponentStart(startPiece, &random);

        while (pendingRoadsSize != 0) {
            int i = nextInt(&random, pendingRoadsSize);
            Piece &piece = pieces[pendingRoads[i]];
            pendingRoadsSize--;
            for (int j = i; j < pendingRoadsSize; j++) {
                pendingRoads[j] = pendingRoads[j + 1];
            }
            buildComponent(piece, &random);

            if (blackSmithPiece != nullptr && this->generationStep == GenerationStep::BLACKSMITH) return; // stop at blacksmith
        }

        if(this->generationStep == GenerationStep::LAYOUT)
            return;

        int k = 0;
        for (int pieceIndex = 0; pieceIndex < this->piecesSize; pieceIndex++) {
            const Piece& structurecomponent = this->pieces[pieceIndex];
            if ((structurecomponent.type != PieceType::NONE && structurecomponent.type != PieceType::Road && structurecomponent.type != PieceType::Start)) {
                k++;
            }
        }
        this->hasMoreThanTwoComponents = k > 2;
    }


    void Village::setupPieces(uint64_t *rng) {

        currentVillagePW.clear();
        for (auto pieceWeightAt: PIECE_WEIGHTS) {
            FinalPieceWeight fPiece = FinalPieceWeight(pieceWeightAt.pieceType, pieceWeightAt.weight,
                                                       nextInt(rng, pieceWeightAt.PlaceCountMin, pieceWeightAt.PlaceCountMax),
                                                       0); //random set max amount of houses for each house because it is not set
            if (fPiece.maxPlaceCount > 0) {
                currentVillagePW.push_back(fPiece);
            }
        }

        // hasMoreThanTwoComponents = false;
        previousPiece = PieceType::NONE;
        blackSmithPiece = nullptr;
        piecesSize = 0;
        pendingRoadsSize = 0;
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
            default:
                return BoundingBox::EMPTY;
        }
    }


    Piece Village::generateAndAddRoadPiece(uint64_t *rand, Pos3D pos, DIRECTION facing) {
        if (abs(startX - pos.getX()) <= 48 && abs(startZ - pos.getZ()) <= 48) return {};

        BoundingBox boundingBox = road(rand, pos, facing);

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
        for (const FinalPieceWeight &pWeight: currentVillagePW) {
            if (pWeight.maxPlaceCount > 0 && pWeight.amountPlaced < pWeight.maxPlaceCount) {
                flag = true;
            }

            i += pWeight.weight;
        }

        return flag ? i : -1;
    }


    BoundingBox Village::road(uint64_t* rng, Pos3D pos, DIRECTION facing) {
        for (int i = 7 * nextInt(rng, 3, 5); i >= 7; i -= 7) {
            // bool flag = true;
            BoundingBox structure = BoundingBox::orientBox(pos, 3, 3, i, facing);
            if (!hasCollisionPiece(structure)) return structure;
        }

        return {0, 0, 0, 0, 0, 0};
    }


    void Village::additionalRngRolls(uint64_t *rng, const Piece &p) {
        switch (p.type) {
            case PieceType::WoodHut:
                nextBoolean(rng); // isTallHouse
                nextInt(rng, 3); // tablePosition
                return;
            case PieceType::House4Garden:
                nextBoolean(rng); // isRoofAccessible
                return;
            case PieceType::Field2:
                nextInt(rng, 10); // cropTypeA
                nextInt(rng, 10); // cropTypeB
                return;
            case PieceType::Field1:
                nextInt(rng, 10); // cropTypeA
                nextInt(rng, 10); // cropTypeB
                nextInt(rng, 10); // cropTypeC
                nextInt(rng, 10); // cropTypeD
                return;
            case PieceType::House2:
                blackSmithPiece = &pieces[piecesSize];
                return;
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


    Piece Village::generateComponent(uint64_t *rng, Pos3D pos, DIRECTION facing) {
        int i = updatePieceWeight();
        if (i <= 0) return {};

        int j = 0;

        while (j < 5) {
            // bool flag = true; // assigned but never accessed
            ++j;
            int k = nextInt(rng, i);

            int pieceWeightsSize = (int) currentVillagePW.size();
            for (int pieceTypeNum = 0; pieceTypeNum < pieceWeightsSize; pieceTypeNum++) {
                FinalPieceWeight &pieceWeight = currentVillagePW[pieceTypeNum];
                k -= pieceWeight.weight;

                if (k < 0) {
                    if ((pieceWeight.amountPlaced >= pieceWeight.maxPlaceCount) ||
                        (pieceWeight.pieceType == previousPiece && currentVillagePW.size() > 1)) {
                        break;
                    }
                    // flag = true;
                    Piece structureVillagePiece = Piece(pieceWeight.pieceType, 0,
                                                        createPieceBoundingBox(pieceWeight.pieceType, pos, facing),
                                                        facing, 0);
                    if (!hasCollisionPiece(structureVillagePiece)) {
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
        Piece torch = Piece(PieceType::Torch, 0, BoundingBox(createPieceBoundingBox(PieceType::Torch, pos, facing)), facing, 0);
        if (hasCollisionPiece(torch)) return {};
        return torch;

    }


    Piece Village::generateAndAddComponent(uint64_t *rand, Pos3D pos, DIRECTION facing) {
        if (abs(startX - pos.getX()) <= 48 && abs(startZ - pos.getZ()) <= 48) {
            Piece structureComponent = generateComponent(rand, pos, facing);
            if (structureComponent.type != PieceType::NONE) {
                int radius = (structureComponent.getLength() / 2) + 4;

                if (g->areBiomesViable(structureComponent.getCenterX(),
                                       structureComponent.getCenterZ(),
                                       radius,
                                       Placement::Village<false>::VALID_BIOMES))
                {
                    additionalRngRolls(rand, structureComponent);
                    pieces[piecesSize++] = structureComponent;
                    return structureComponent;
                }
            }
        }
        return {}; // ------------------------ returning null piece
    }


    void Village::buildComponentStart(Piece piece, uint64_t *rand) {
        generateAndAddRoadPiece(rand, {piece.minX - 1, piece.maxY - 4, piece.minZ + 1}, DIRECTION::WEST);
        generateAndAddRoadPiece(rand, {piece.maxX + 1, piece.maxY - 4, piece.minZ + 1}, DIRECTION::EAST);
        generateAndAddRoadPiece(rand, {piece.minX + 1, piece.maxY - 4, piece.minZ - 1}, DIRECTION::NORTH);
        generateAndAddRoadPiece(rand, {piece.minX + 1, piece.maxY - 4, piece.maxZ + 1}, DIRECTION::SOUTH);
    }


    void Village::buildComponent(Piece piece, uint64_t *rand) {
        bool flag = false;

        for (int i = nextInt(rand, 5); i < piece.additionalData - 8; i += 2 + nextInt(rand, 5)) {
            Piece structureComponent;
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                case DIRECTION::SOUTH:
                default:
                    structureComponent = generateAndAddComponent(rand, {piece.minX - 1, piece.minY, piece.minZ + i}, DIRECTION::WEST);
                    break;
                case DIRECTION::WEST:
                case DIRECTION::EAST:
                    structureComponent = generateAndAddComponent(rand, {piece.minX + i, piece.minY, piece.minZ - 1}, DIRECTION::NORTH);
                    break;
            }

            if (structureComponent.type != PieceType::NONE) {
                i += structureComponent.getLength() + 1;
                flag = true;
            }
        }

        for (int j = nextInt(rand, 5); j < piece.additionalData - 8; j += 2 + nextInt(rand, 5)) {
            Piece structureComponent1; // = getNextComponentPP(componentIn, rand, 0, j);
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                case DIRECTION::SOUTH:
                default:
                    structureComponent1 = generateAndAddComponent(rand, {piece.maxX + 1, piece.minY, piece.minZ + j}, DIRECTION::EAST);
                    break;
                case DIRECTION::EAST:
                case DIRECTION::WEST:
                    structureComponent1 = generateAndAddComponent(rand, {piece.minX + j, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH);
                    break;
            }

            if (structureComponent1.type != PieceType::NONE) {
                j += structureComponent1.getLength() + 1;
                flag = true;
            }
        }

        if (flag && nextInt(rand, 3) > 0) {
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                default:
                    generateAndAddRoadPiece(rand, {piece.minX - 1, piece.minY, piece.minZ}, DIRECTION::WEST);
                    break;
                case DIRECTION::SOUTH:
                    generateAndAddRoadPiece(rand, {piece.minX - 1, piece.minY, piece.maxZ - 2}, DIRECTION::WEST);
                    break;
                case DIRECTION::WEST:
                    generateAndAddRoadPiece(rand, {piece.minX, piece.minY, piece.minZ - 1}, DIRECTION::NORTH);
                    break;
                case DIRECTION::EAST:
                    generateAndAddRoadPiece(rand, {piece.maxX - 2, piece.minY, piece.minZ - 1}, DIRECTION::NORTH);
            }
        }

        if (flag && nextInt(rand, 3) > 0) {
            switch (piece.orientation) {
                case DIRECTION::NORTH:
                default:
                    generateAndAddRoadPiece(rand, {piece.maxX + 1, piece.minY, piece.minZ}, DIRECTION::EAST);
                    break;
                case DIRECTION::SOUTH:
                    generateAndAddRoadPiece(rand, {piece.maxX + 1, piece.minY, piece.maxZ - 2}, DIRECTION::EAST);
                    break;
                case DIRECTION::WEST:
                    generateAndAddRoadPiece(rand, {piece.minX, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH);
                    break;
                case DIRECTION::EAST:
                    generateAndAddRoadPiece(rand, {piece.maxX - 2, piece.minY, piece.maxZ + 1}, DIRECTION::SOUTH);
            }
        }
    }


    void Village::addPiece(Piece &piece) {
        pendingRoads[pendingRoadsSize++] = piecesSize;
        pieces[piecesSize++] = piece;
    }


    bool Village::hasCollisionPiece(const BoundingBox &boundingBox) {
        for (int i = 0; i < piecesSize; i++) {
            if (pieces[i].intersects(boundingBox)) {
                return true;
            }
        }
        return false;
    }

}