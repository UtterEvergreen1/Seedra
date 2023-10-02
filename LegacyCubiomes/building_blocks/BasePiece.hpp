#pragma once

#include "LegacyCubiomes/cubiomes/processor.hpp"

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"

class BasePiece : public BoundingBox {
public:
    DIRECTION orientation;

    BasePiece();
    BasePiece(BoundingBox boundingBox, DIRECTION orientation);
    BasePiece(int minX, int minY, int minZ, int maxX, int maxY, int maxZ, DIRECTION orientation);

    ND MU Pos2D getBoundingBoxCenter() const;
    ND    int getWorldX(int offsetWidth, int offsetDepth) const;
    ND    inline int getWorldY(int offsetHeight) const {return minY + offsetHeight;}
    ND    int getWorldZ(int offsetWidth, int offsetDepth) const;
    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;
       MU static BoundingBox makeBoundingBox(int x, int y, int z, DIRECTION direction, int width, int height, int depth);
};