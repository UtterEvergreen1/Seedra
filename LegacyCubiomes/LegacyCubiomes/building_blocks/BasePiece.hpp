#pragma once

#include "LegacyCubiomes/building_blocks/BoundingBox.hpp"
#include "LegacyCubiomes/utils/pos2D.hpp"

class BasePiece {
public:
    BoundingBox boundingBox;
    DIRECTION orientation;

    BasePiece();
    BasePiece(BoundingBox boundingBox, DIRECTION orientation);

    [[nodiscard]] Pos2D getBoundingBoxCenter() const;
    [[nodiscard]] int getWorldX(int offsetWidth, int offsetDepth) const;
    [[nodiscard]] int getWorldY(int offsetHeight) const;
    [[nodiscard]] int getWorldZ(int offsetWidth, int offsetDepth) const;
    [[nodiscard]] Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;

    [[maybe_unused]] static BoundingBox makeBoundingBox(int x, int y, int z, DIRECTION direction,
                                                        int width, int height, int depth);
};
