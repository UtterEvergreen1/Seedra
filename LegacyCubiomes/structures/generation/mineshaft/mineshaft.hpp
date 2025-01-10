#pragma once

#include <cmath>
#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "LegacyCubiomes/utils/rng.hpp"


namespace gen {

    class Mineshaft {
    public:
        // #######################################################
        //              sub-structs and sub-classes
        // #######################################################


        enum class MineshaftType { NORMAL, MESA };

        static std::map<PieceType, std::string> pieceTypeNames;

        // #######################################################
        //       class attributes, variables, functions
        // #######################################################

        StructureComponent pieceArray[1024];
        int collisionChecks = 0;

        int pieceArraySize = 0;
        int startX = 0;
        int startZ = 0;
        MineshaftType mineShaftType = MineshaftType::NORMAL;
        BoundingBox structureBB;


        /**
        * \n
        * Generates a mineshaft with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkX x coordinate of the chunk
        * @param chunkZ z coordinate of the chunk
        */
        void generate(lce::CONSOLE console, i64 worldSeed, int chunkX, int chunkZ);

        /**
        * \n
        * Overload function. Generates a mineshaft with the given seed and chunk coordinates.
        * @param worldSeed the seed
        * @param chunkPos coordinates of the chunk
        */
        void generate(lce::CONSOLE console, c_i64 worldSeed, const Pos2D chunkPos) {
            return generate(console, worldSeed, chunkPos.x, chunkPos.z);
        }

        MU void reset();

    private:
        StructureComponent* findCollisionPiece(const BoundingBox& boundingBox);
        void genAndAddPiece(RNG& rng, Pos3D pos, FACING direction, int depth);


        void buildComponent(RNG& rng, PieceType type, int depth,
                            const BoundingBox& boundingBox, FACING direction,
                            int additionalData);
    };
} // namespace generation
