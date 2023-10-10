#pragma once


#include <vector>

#include "Enums.hpp"
#include "PieceWeight.hpp"

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

        bool isZombieInfested;

        std::vector<FinalPieceWeight> currentVillagePW;
        Generator* g;
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
        //void genVillage(Generator* generator, int64_t worldSeed, int chunkX, int chunkZ);

        void generate(Generator* generator, int chunkX, int chunkZ);

        void inline generate(Generator* generator, Pos2D chunk) {
            generate(generator, chunk.x, chunk.z);
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
