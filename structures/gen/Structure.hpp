#pragma once

#include "lce/processor.hpp"

#include "components/StructureComponent.hpp"


template<int ARRAY_SIZE>
class Structure {
protected:
    StructureComponent pieceArray[ARRAY_SIZE]{};
    short pieceArraySize = 0;

    Pos2D startPos{};
    BoundingBox structureBB{};

    int collisionChecks = 0;

public:

    /// getter
    MU ND StructureComponent& getPiece(int index) { return pieceArray[index]; }
    /// getter
    MU ND const StructureComponent& getPieceConst(int index) const { return pieceArray[index]; }
    /// getter
    MU ND int getPieceCount() const { return pieceArraySize; }
    /// getter
    MU ND const BoundingBox& getStructureBB() const { return structureBB; }
    /// getter
    MU ND const Pos2D& getStartPos() const { return startPos; }




    /*
    MU ND bool hasCollisionPiece(const BoundingBox& bbIn);

    MU ND StructureComponent* findCollisionPiece(const BoundingBox& bbIn);
     */
};