#include "Piece.hpp"


Pos2D Piece::getBoundingBoxCenter() const {
    Pos2D pos{};
    pos.x = minX + maxX / 2;
    pos.z = minZ + maxZ / 2;
    return pos;
}


int Piece::getWorldX(c_int offsetWidth, c_int offsetDepth) const {
    switch (orientation) {
        case DIRECTION::NORTH:
        case DIRECTION::SOUTH:
            return minX + offsetWidth;
        case DIRECTION::WEST:
            return maxX - offsetDepth;
        default:
        case DIRECTION::EAST:
            return minX + offsetDepth;
    }
}


int Piece::getWorldZ(c_int offsetWidth, c_int offsetDepth) const {
    switch (orientation) {
        case DIRECTION::NORTH:
            return maxZ - offsetDepth;
        case DIRECTION::SOUTH:
            return minZ + offsetDepth;
        default:
        case DIRECTION::WEST:
        case DIRECTION::EAST:
            return minZ + offsetWidth;
    }
}


Pos2D Piece::getWorldPos(c_int offsetWidth, c_int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldZ(offsetWidth, offsetDepth)};
}


BoundingBox Piece::makeBoundingBox(int x, int y, int z, const DIRECTION direction,
                                   c_int width, c_int height, c_int depth) {
    if (direction == DIRECTION::NORTH || direction == DIRECTION::SOUTH) {
        return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
    }
    return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};

}