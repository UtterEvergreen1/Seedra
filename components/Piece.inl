#pragma once

inline Pos2D Piece::getBoundingBoxCenter() const {
    return {minX + maxX / 2, minZ + maxZ / 2};
}

inline int Piece::getWorldX(c_int offsetWidth, c_int offsetDepth) const {
    switch (facing) {
        case EnumFacing::NORTH:
        case EnumFacing::SOUTH:
            return minX + offsetWidth;
        case EnumFacing::WEST:
            return maxX - offsetDepth;
        default:
        case EnumFacing::EAST:
            return minX + offsetDepth;
    }
}

inline int Piece::getWorldY(c_int offsetHeight) const {
    return minY + offsetHeight;
}

inline int Piece::getWorldZ(c_int offsetWidth, c_int offsetDepth) const {
    switch (facing) {
        case EnumFacing::NORTH:
            return maxZ - offsetDepth;
        case EnumFacing::SOUTH:
            return minZ + offsetDepth;
        default:
        case EnumFacing::WEST:
        case EnumFacing::EAST:
            return minZ + offsetWidth;
    }
}

inline  Pos3D Piece::getWorldXYZ(c_int offsetWidth, c_int offsetHeight, c_int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldY(offsetHeight), getWorldZ(offsetWidth, offsetDepth)};
}


inline Pos2D Piece::getWorldPos(c_int offsetWidth, c_int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldZ(offsetWidth, offsetDepth)};
}

inline BoundingBox Piece::makeBoundingBox(c_int x, c_int y, c_int z, const EnumFacing direction,
                                   c_int width, c_int height, c_int depth) {
    if (direction == EnumFacing::NORTH || direction == EnumFacing::SOUTH) {
        return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
    }
    return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};

}