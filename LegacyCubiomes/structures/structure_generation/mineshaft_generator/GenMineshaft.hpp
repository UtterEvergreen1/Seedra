#pragma once

#include "Enums.hpp"
#include "Piece.hpp"

namespace mineshaft_generator {
    class MineshaftGenerator {
    public:
        Piece pieces[1024];
        int piecesSize = 0;
        int startX = 0;
        int startZ = 0;
        MineshaftType mineShaftType;
        BoundingBox structureBoundingBox;

        void generate(int64_t worldSeed, int chunkX, int chunkZ);
        inline void generate(int64_t worldSeed, Pos2D chunkPos) {
            return generate(worldSeed, chunkPos.x, chunkPos.z);
        }
        bool createPiece(uint64_t *random, int x, int y, int z,
                                                             DIRECTION direction, int depth);
        void generateAndAddPiece(uint64_t* random, int x, int y, int z, DIRECTION direction, int depth);
        void buildComponent(Piece &piece, uint64_t* random);

        Piece* findCollisionPiece(BoundingBox &boundingBox);
    };
}
