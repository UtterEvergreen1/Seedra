#pragma once

#include <vector>
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/cubiomes/rng.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"
#include "LegacyCubiomes/utils/pos3D.hpp"

namespace generation {

    class Village {
    public:

        enum class GenerationStep {
            BLACKSMITH, // Generates the layout up to the blacksmith, hasMoreThanTwoComponents won't be calculated
            LAYOUT, // Generates full layout, hasMoreThanTwoComponents won't be calculated
            FULL,   // Generates full layout and calculates hasMoreThanTwoComponents
        };

        enum PieceType {
            NONE,
            House4Garden,
            Church,
            House1,
            WoodHut,
            Hall,
            Field1,
            Field2,
            House2,
            House3,
            Torch,
            Start,
            Road,
        };

        struct PieceWeight {
            int pieceType;
            int weight;
            int PlaceCountMin;
            int PlaceCountMax;
        };

        const int VILLAGE_SIZE = 0;
        const PieceWeight PIECE_WEIGHTS[9] = {
                {PieceType::House4Garden,    4, 2 + VILLAGE_SIZE,   4 + VILLAGE_SIZE * 2},
                {PieceType::Church,         20, 0 + VILLAGE_SIZE,   1 + VILLAGE_SIZE},
                {PieceType::House1,         20, 0 + VILLAGE_SIZE,   2 + VILLAGE_SIZE},
                {PieceType::WoodHut,         3, 2 + VILLAGE_SIZE,   5 + VILLAGE_SIZE * 3},
                {PieceType::Hall,           15, 0 + VILLAGE_SIZE,   2 + VILLAGE_SIZE},
                { PieceType::Field1,         3, 1 + VILLAGE_SIZE,   4 + VILLAGE_SIZE},
                { PieceType::Field2,         3, 2 + VILLAGE_SIZE,   4 + VILLAGE_SIZE * 2},
                { PieceType::House2,        15, 0,                  1 + VILLAGE_SIZE},
                { PieceType::House3,         8, 0 + VILLAGE_SIZE,   3 + VILLAGE_SIZE * 2}
        };

        struct FinalPieceWeight {
            int pieceType;
            int weight;
            int maxPlaceCount;
            int amountPlaced;
            FinalPieceWeight(int pieceType, int weight, int maxPlaceCount, int amountPlaced)
                    : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {}
        };

        bool isZombieInfested;

        std::vector<FinalPieceWeight> currentVillagePW;
        const Generator* g;
        GenerationStep generationStep = GenerationStep::FULL;

        int previousPiece;
        Piece pieces[1024];
        int piecesSize;

        int pendingRoads[1024];
        int pendingRoadsSize;

        Piece* blackSmithPiece{};

        int startX;
        int startZ;

        bool hasMoreThanTwoComponents;

        Village(const Generator* generator);

        void generate(int chunkX, int chunkZ);

        void inline generate(Pos2D chunk) {
            generate(chunk.x, chunk.z);
        }

        void setupPieces(uint64_t* rng);
        int updatePieceWeight();
        static BoundingBox createPieceBoundingBox(int pieceType, Pos3D pos, DIRECTION direction);
        void buildComponentStart(Piece piece, uint64_t* rng);
        void buildComponent(Piece piece, uint64_t* rand);
        BoundingBox road(uint64_t *rng, Pos3D pos, DIRECTION facing);
        void additionalRngRolls(uint64_t* rng, const Piece& p);
        Piece generateComponent(uint64_t* rng, Pos3D pos, DIRECTION facing);
        Piece generateAndAddRoadPiece(uint64_t *rng, Pos3D pos, DIRECTION facing);
        Piece generateAndAddComponent(uint64_t *rng, Pos3D pos, DIRECTION facing);
        void addPiece(Piece& piece);
        bool hasCollisionPiece(const BoundingBox& boundingBox);
    };
}
