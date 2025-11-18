#pragma once

#include "lce/processor.hpp"

#include "components/StructureComponent.hpp"


template<int ARRAY_SIZE>
class Structure {
protected:
    StructureComponent m_pieceArray[ARRAY_SIZE]{};
    short m_pieceArraySize = 0;

    Pos2D m_startPos{};
    BoundingBox m_structureBB{};

    int m_collisionChecks = 0;

public:
    Structure() = default;

    /// getter
    MU ND StructureComponent& getPiece(int index) { return m_pieceArray[index]; }
    /// getter
    MU ND const StructureComponent& getPieceConst(int index) const { return m_pieceArray[index]; }
    MU ND StructureComponent* getPieceArray() { return m_pieceArray; }
    /// getter
    MU ND int getPieceCount() const { return m_pieceArraySize; }
    /// getter
    MU ND const BoundingBox& getStructureBB() const { return m_structureBB; }
    /// getter
    MU ND const Pos2D& getStartPos() const { return m_startPos; }




    /*
    MU ND bool hasCollisionPiece(const BoundingBox& bbIn);

    MU ND StructureComponent* findCollisionPiece(const BoundingBox& bbIn);
     */
};
