#pragma once

#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/cubiomes/layers.hpp"
#include "LegacyCubiomes/utils/Pos2D.hpp"
#include "LegacyCubiomes/utils/Pos3D.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include <map>
#include <vector>

namespace generation {

    class Village {
    public:
        enum class GenerationStep : int8_t {
            BLACKSMITH, // Generates the layout up to the blacksmith, hasMoreThanTwoComponents won't be calculated
            LAYOUT,     // Generates full layout, hasMoreThanTwoComponents won't be calculated
            FULL,       // Generates full layout and calculates hasMoreThanTwoComponents
        };

        enum PieceType : int8_t {
            NONE,
            Start,
            Road,
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
        };

        struct PieceWeight {
            int8_t pieceType;
            int weight;
            int PlaceCountMin;
            int PlaceCountMax;
        };

        static std::map<int, std::string> pieceTypeNames;
        static const int VILLAGE_SIZE;
        static const PieceWeight PIECE_WEIGHTS[9];

    private:
        struct FinalPieceWeight {
            int8_t pieceType;
            int weight;
            int maxPlaceCount;
            int amountPlaced;
            FinalPieceWeight(int8_t pieceType, int weight, int maxPlaceCount, int amountPlaced)
                : pieceType(pieceType), weight(weight), maxPlaceCount(maxPlaceCount), amountPlaced(amountPlaced) {}
        };

        std::vector<FinalPieceWeight> currentVillagePW;
        int8_t previousPiece{};
        int pendingRoadArray[512]{};
        int pendingRoadArraySize{};
        const Generator* g;
        RNG rng;

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
        void inline generate(Pos2D chunk) { generate(chunk.x, chunk.z); }

    private:
        void setupPieces();
        int updatePieceWeight();
        static BoundingBox createPieceBoundingBox(int pieceType, Pos3D pos, DIRECTION direction);
        void buildComponentStart(Piece piece);
        void buildComponent(Piece piece);
        BoundingBox road(Pos3D pos, DIRECTION facing);
        void additionalRngRolls(const Piece& p);
        Piece generateComponent(Pos3D pos, DIRECTION facing, int8_t depth);
        Piece genAndAddRoadPiece(Pos3D pos, DIRECTION facing);
        Piece genAndAddComponent(Pos3D pos, DIRECTION facing, int8_t depth);
        void addPiece(Piece& piece);
        bool hasCollisionPiece(const BoundingBox& boundingBox);
    };
} // namespace generation
