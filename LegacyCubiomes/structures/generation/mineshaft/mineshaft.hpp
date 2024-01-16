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

        enum class PieceType {
            ROOM = 0,
            CORRIDOR = 1,
            CROSSING = 2,
            STAIRS = 3,
            NONE,
        };

        enum class MineshaftType { NORMAL, MESA };

        static const std::map<PieceType, std::string> PieceTypeName;

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


        /**
        * \n
        * Generates a mineshaft with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkX x coordinate of the chunk
        * @param chunkZ z coordinate of the chunk
        */
        void generate(int64_t worldSeed, int chunkX, int chunkZ);

        /**
        * \n
        * Overload function. Generates a mineshaft with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkPos coordinates of the chunk
        */
        void generate(const int64_t worldSeed, const Pos2D chunkPos) {
            return generate(worldSeed, chunkPos.x, chunkPos.z);
        }
        void reset();

    private:
        Piece* findCollisionPiece(const BoundingBox& boundingBox);
        void genAndAddPiece(RNG& rng, Pos3D pos, DIRECTION direction, int depth);

        /**
        */
        void buildComponent(RNG& rng, PieceType type, int depth, const BoundingBox& boundingBox, DIRECTION direction,
                            int additionalData);
    };
} // namespace generation
