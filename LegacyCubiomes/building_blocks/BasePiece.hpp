#pragma once

#include "LegacyCubiomes/cubiomes/processor.hpp"

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"

class BasePiece {
public:
    BoundingBox boundingBox;
    DIRECTION orientation;

    BasePiece();
    BasePiece(BoundingBox boundingBox, DIRECTION orientation);

    ND Pos2D getBoundingBoxCenter() const;
    ND int getWorldX(int offsetWidth, int offsetDepth) const;
    ND int getWorldY(int offsetHeight) const;
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;
    ND Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;

    MU static BoundingBox makeBoundingBox(int x, int y, int z, DIRECTION direction,
                                                        int width, int height, int depth);
};
