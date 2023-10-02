#include <climits>

#include "BoundingBox.hpp"


const BoundingBox BoundingBox::EMPTY = BoundingBox(INT_MAX, INT_MAX, INT_MAX, INT_MIN, INT_MIN, INT_MIN);


BoundingBox::BoundingBox() = default;


BoundingBox::BoundingBox(int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
    : minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ) {}


bool BoundingBox::intersects(const BoundingBox &other) const {
    return maxX >= other.minX && minX <= other.maxX &&
           maxZ >= other.minZ && minZ <= other.maxZ &&
           maxY >= other.minY && minY <= other.maxY;
}


bool BoundingBox::contains(const BoundingBox &other) const {
    return this->maxX >= other.maxX && this->minX <= other.minX &&
           this->maxY >= other.maxY && this->minY <= other.minY &&
           this->maxZ >= other.maxZ && this->minZ <= other.minZ;
}


void BoundingBox::encompass(const BoundingBox &other) {
    if (other.minX < this->minX) this->minX = other.minX;
    if (other.minY < this->minY) this->minY = other.minY;
    if (other.minZ < this->minZ) this->minZ = other.minZ;
    if (other.maxX > this->maxX) this->maxX = other.maxX;
    if (other.maxY > this->maxY) this->maxY = other.maxY;
    if (other.maxZ > this->maxZ) this->maxZ = other.maxZ;
}


void BoundingBox::offset(int x, int y, int z) {
    this->minX += x;
    this->minY += y;
    this->minZ += z;
    this->maxX += x;
    this->maxY += y;
    this->maxZ += z;
}


BoundingBox BoundingBox::orientBox(int x, int y, int z, int width, int height, int depth, DIRECTION direction) {
    switch (direction) {
        case DIRECTION::NORTH:
            return {x, y, z - depth + 1, x + width - 1, y + height - 1, z};
        case DIRECTION::SOUTH:
            return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
        case DIRECTION::WEST:
            return {x - depth + 1, y, z, x, y + height - 1, z + width - 1};
        default:
        case DIRECTION::EAST:
            return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};
    }
}


BoundingBox BoundingBox::orientBox(int x, int y, int z,
                                   int offsetWidth, int offsetHeight, int offsetDepth,
                                   int width, int height, int depth, DIRECTION direction) {
    switch (direction) {
        case DIRECTION::NORTH:
            return {x + offsetWidth,
                    y + offsetHeight,
                    z - depth + 1 + offsetDepth,
                    x + width - 1 + offsetWidth,
                    y + height - 1 + offsetHeight,
                    z + offsetDepth};
        case DIRECTION::SOUTH:
            return {x + offsetWidth,
                    y + offsetHeight,
                    z + offsetDepth,
                    x + width - 1 + offsetWidth,
                    y + height - 1 + offsetHeight,
                    z + depth - 1 + offsetDepth};
        case DIRECTION::WEST:
            return {x - depth + 1 + offsetDepth,
                    y + offsetHeight,
                    z + offsetWidth,
                    x + offsetDepth,
                    y + height - 1 + offsetHeight,
                    z + width - 1 + offsetWidth};
        default:
        case DIRECTION::EAST:
            return {x + offsetDepth,
                    y + offsetHeight,
                    z + offsetWidth,
                    x + depth - 1 + offsetDepth,
                    y + height - 1 + offsetHeight,
                    z + width - 1 + offsetWidth};
    }
}


std::ostream& operator<<(std::ostream& out, const BoundingBox &bBox) {
    out << "(" << bBox.minX << ", " << bBox.minY << ", " << bBox.minZ << ") -> (" <<
        bBox.maxX << ", " << bBox.maxY << ", " << bBox.maxZ << ")";
    return out;
}

