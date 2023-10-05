#pragma once

#include "Enums.hpp"
#include "Piece.hpp"

namespace mineshaft_generator {

    class MineshaftGenerator {
    public:
        Piece pieces[1024];
        int collisionChecks = 0;

        int piecesSize = 0;
        int startX = 0;
        int startZ = 0;
        MineshaftType mineShaftType = MineshaftType::NORMAL;
        BoundingBox structureBoundingBox;

        void generate(int64_t worldSeed, int chunkX, int chunkZ);
        inline void generate(int64_t worldSeed, Pos2D chunkPos) { return generate(worldSeed, chunkPos.x, chunkPos.z);}
        void reset();

    private:
        void addPiece(PieceType type, int depth, const BoundingBox& boundingBox, DIRECTION direction, int additionalData);
        bool createPiece(uint64_t *rng, Pos3D pos, DIRECTION direction, int depth);
        void genAndAddPiece(uint64_t* rng, Pos3D pos, DIRECTION direction, int depth);
        Piece* findCollisionPiece(BoundingBox &boundingBox);
        void buildComponent(Piece &piece, uint64_t* rng);
    };
}
