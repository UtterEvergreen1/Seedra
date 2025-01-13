#include "Piece.hpp"


MU Pos2D Piece::getBoundingBoxCenter() const {
    Pos2D pos{};
    pos.x = minX + maxX / 2;
    pos.z = minZ + maxZ / 2;
    return pos;
}


int Piece::getWorldX(c_int offsetWidth, c_int offsetDepth) const {
    switch (facing) {
        case FACING::NORTH:
        case FACING::SOUTH:
            return minX + offsetWidth;
        case FACING::WEST:
            return maxX - offsetDepth;
        default:
        case FACING::EAST:
            return minX + offsetDepth;
    }
}


int Piece::getWorldZ(c_int offsetWidth, c_int offsetDepth) const {
    switch (facing) {
        case FACING::NORTH:
            return maxZ - offsetDepth;
        case FACING::SOUTH:
            return minZ + offsetDepth;
        default:
        case FACING::WEST:
        case FACING::EAST:
            return minZ + offsetWidth;
    }
}


MU ND Pos3D Piece::getWorldXYZ(int offsetWidth, int offsetHeight, int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldY(offsetHeight), getWorldZ(offsetWidth, offsetDepth)};
}


Pos2D Piece::getWorldPos(c_int offsetWidth, c_int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldZ(offsetWidth, offsetDepth)};
}


BoundingBox Piece::makeBoundingBox(int x, int y, int z, const FACING direction,
                                   c_int width, c_int height, c_int depth) {
    if (direction == FACING::NORTH || direction == FACING::SOUTH) {
        return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
    }
    return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};

}