#pragma once

#include <cmath>
#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/utils/rng.hpp"


namespace generation {

    class Mineshaft {
    public:

        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################

        enum PieceType {
            ROOM = 0,
            CORRIDOR = 1,
            CROSSING = 2,
            STAIRS = 3,
            NONE,
        };

        enum class MineshaftType {
            NORMAL,
            MESA
        };

        const std::map<PieceType, std::string> PieceTypeName = {
                { PieceType::ROOM, "ROOM" },
                { PieceType::CORRIDOR, "CORRIDOR" },
                { PieceType::CROSSING, "CROSSING" },
                { PieceType::STAIRS, "STAIRS" },
                { PieceType::NONE, "NONE -> something went wrong" }
        };

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################

        Piece pieceArray[1024];
        int collisionChecks = 0;

        int pieceArraySize = 0;
        int startX = 0;
        int startZ = 0;
        MineshaftType mineShaftType = MineshaftType::NORMAL;
        BoundingBox structureBoundingBox;

        void generate(int64_t worldSeed, int chunkX, int chunkZ);
        inline void generate(int64_t worldSeed, Pos2D chunkPos) { return generate(worldSeed, chunkPos.x, chunkPos.z);}
        void reset();

    private:
        Piece* findCollisionPiece(BoundingBox &boundingBox);
        void genAndAddPiece(RNG& rng, Pos3D pos, DIRECTION direction, int depth);
        void buildComponent(RNG& rng, int type, int depth, const BoundingBox& boundingBox, DIRECTION direction, int additionalData);
    };
}
