#pragma once

#include <vector>
#include <map>
#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "LegacyCubiomes/utils/Pos2D.hpp"
#include "LegacyCubiomes/utils/Pos3D.hpp"

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

        static std::map<int, std::string> pieceTypeNames;
        static const int VILLAGE_SIZE;
        static const PieceWeight PIECE_WEIGHTS[9];

    private:
        struct FinalPieceWeight {
            int pieceType;
            int weight;
            int maxPlaceCount;
            int amountPlaced;
            FinalPieceWeight(int pieceType, int weight, int maxPlaceCount, int amountPlaced)
                    : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {}
        };

        std::vector<FinalPieceWeight> currentVillagePW;
        int previousPiece{};
        int pendingRoadArray[512]{};
        int pendingRoadArraySize{};
        const Generator* g;

    public:
        bool isZombieInfested{};
        GenerationStep generationStep = GenerationStep::FULL;

        Piece pieceArray[512];
        int pieceArraySize{};

        Piece* blackSmithPiece{};

        int startX{};
        int startZ{};

        bool hasMoreThanTwoComponents{};

        explicit Village(const Generator* generator);

        /**
        * \n
        * Generates a mineshaft with the given chunk coordinates and stored generator.
        * @param chunkX x coordinate of the chunk
        * @param chunkZ z coordinate of the chunk
        */
        void generate(int chunkX, int chunkZ);

        /**
        * \n
        * Overload function. Generates a mineshaft with the given chunk coordinates and stored generator.
        * @param chunk coordinates of the chunk
        */
        void inline generate(Pos2D chunk) {
            generate(chunk.x, chunk.z);
        }

    private:
        void setupPieces(RNG& rng);
        int updatePieceWeight();
        static BoundingBox createPieceBoundingBox(int pieceType, Pos3D pos, DIRECTION direction);
        void buildComponentStart(Piece piece, RNG& rng);
        void buildComponent(Piece piece, RNG& rng);
        BoundingBox road(RNG& rng, Pos3D pos, DIRECTION facing);
        void additionalRngRolls(RNG& rng, const Piece& p);
        Piece generateComponent(RNG& rng, Pos3D pos, DIRECTION facing, int depth);
        Piece genAndAddRoadPiece(RNG& rng, Pos3D pos, DIRECTION facing);
        Piece genAndAddComponent(RNG& rand, Pos3D pos, DIRECTION facing, int depth);
        void addPiece(Piece& piece);
        bool hasCollisionPiece(const BoundingBox& boundingBox);
    };
}
