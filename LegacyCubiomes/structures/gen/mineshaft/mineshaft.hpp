#pragma once

#include <cmath>
#include <map>
#include <string>

#include "LegacyCubiomes/building_blocks/Piece.hpp"
#include "LegacyCubiomes/building_blocks/StructureComponent.hpp"
#include "LegacyCubiomes/utils/rng.hpp"


#include "lce/enums.hpp"

namespace gen {

    enum class MineshaftType { NORMAL, MESA };

    class Mineshaft {
        static constexpr u32 ARRAY_SIZE = 1024;
        MU static std::string PIECE_TYPE_NAMES[5];


    public:
        StructureComponent pieceArray[ARRAY_SIZE];
        MU int pieceArraySize = 0;
        Pos2D startPos;
        MineshaftType mineShaftType = MineshaftType::NORMAL;
        BoundingBox structureBB;

        /// debugging purposes
        int collisionChecks = 0;





        void generate(lce::CONSOLE console, i64 worldSeed, int chunkX, int chunkZ);
        void generate(lce::CONSOLE console, i64 worldSeed, Pos2D chunkPos);
        MU void reset();
        MU static std::string getPieceName(PieceType pieceType);

    private:
        bool collides(const BoundingBox& bbIn);
        StructureComponent* findCollisionPiece(const BoundingBox& bbIn);
        void genAndAddPiece(RNG& rng, Pos3D pos, FACING facing, int depth);
        void buildComponent(RNG& rng, StructureComponent p);
    };

} // namespace gen
