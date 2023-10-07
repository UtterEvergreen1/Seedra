#include "Piece.hpp"



Pos2D Piece::getBoundingBoxCenter() const {
    Pos2D pos{};
    pos.x = minX + maxX / 2;
    pos.z = minZ + maxZ / 2;
    return pos;
}


int Piece::getWorldX(int offsetWidth, int offsetDepth) const {
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


int Piece::getWorldZ(int offsetWidth, int offsetDepth) const {
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


Pos2D Piece::getWorldPos(int offsetWidth, int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldZ(offsetWidth, offsetDepth)};
}


BoundingBox Piece::makeBoundingBox(int x, int y, int z,
                                   DIRECTION direction,
                                   int width, int height, int depth) {
    if (direction == DIRECTION::NORTH || direction == DIRECTION::SOUTH) {
        return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
    } else {
        return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};
    }
}