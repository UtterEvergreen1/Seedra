#include "BoundingBox.hpp"


const BoundingBox BoundingBox::EMPTY = BoundingBox(SHRT_MAX, SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN, SHRT_MIN);


BoundingBox::BoundingBox() = default;


BoundingBox::BoundingBox(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ)
    : minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ) {}


bool BoundingBox::operator==(const BoundingBox& other) const {
    return minX == other.minX && maxX == other.maxX && minZ == other.minZ && maxZ == other.maxZ && minY == other.minY &&
           maxY == other.maxY;
}

bool BoundingBox::intersects(const BoundingBox& other) const {
    return maxX >= other.minX && minX <= other.maxX && maxZ >= other.minZ && minZ <= other.maxZ && maxY >= other.minY &&
           minY <= other.maxY;
}


bool BoundingBox::contains(const BoundingBox& other) const {
    return this->maxX >= other.maxX && this->minX <= other.minX && this->maxY >= other.maxY &&
           this->minY <= other.minY && this->maxZ >= other.maxZ && this->minZ <= other.minZ;
}


void BoundingBox::encompass(const BoundingBox& other) {
    if (other.minX < this->minX) this->minX = other.minX;
    if (other.minY < this->minY) this->minY = other.minY;
    if (other.minZ < this->minZ) this->minZ = other.minZ;
    if (other.maxX > this->maxX) this->maxX = other.maxX;
    if (other.maxY > this->maxY) this->maxY = other.maxY;
    if (other.maxZ > this->maxZ) this->maxZ = other.maxZ;
}


void BoundingBox::offset(c_int x, c_int y, c_int z) {
    minX += x;
    minY += y;
    minZ += z;
    maxX += x;
    maxY += y;
    maxZ += z;
}


void BoundingBox::offsetY(c_int y) {
    minY += y;
    maxY += y;
}


BoundingBox BoundingBox::makeChunkBox(int xChunk, int zChunk) {
    return {xChunk << 4, 0, zChunk << 4, (xChunk << 4) + 15, 255, (zChunk << 4) + 15};
}


BoundingBox BoundingBox::orientBox(int x, int y, int z,
                                   c_int width, c_int height,
                                   c_int depth, const DIRECTION direction) {
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


BoundingBox BoundingBox::orientBox(const Pos3D& xyz, c_int width, c_int height,
                                   c_int depth, const DIRECTION direction) {
    switch (direction) {
        case DIRECTION::NORTH:
            return {xyz.getX(), xyz.getY(), xyz.getZ() - depth + 1, xyz.getX() + width - 1, xyz.getY() + height - 1,
                    xyz.getZ()};
        case DIRECTION::SOUTH:
            return {xyz.getX(),
                    xyz.getY(),
                    xyz.getZ(),
                    xyz.getX() + width - 1,
                    xyz.getY() + height - 1,
                    xyz.getZ() + depth - 1};
        case DIRECTION::WEST:
            return {xyz.getX() - depth + 1, xyz.getY(), xyz.getZ(), xyz.getX(), xyz.getY() + height - 1,
                    xyz.getZ() + width - 1};
        default:
        case DIRECTION::EAST:
            return {xyz.getX(),
                    xyz.getY(),
                    xyz.getZ(),
                    xyz.getX() + depth - 1,
                    xyz.getY() + height - 1,
                    xyz.getZ() + width - 1};
    }
}


BoundingBox BoundingBox::orientBox(c_int x, c_int y, c_int z, c_int offsetWidth, c_int offsetHeight,
                                   c_int offsetDepth, c_int width,
                                   c_int height, c_int depth, const DIRECTION direction) {
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
            return {x - depth + 1 + offsetDepth,   y + offsetHeight,           z + offsetWidth, x + offsetDepth,
                    y + height - 1 + offsetHeight, z + width - 1 + offsetWidth};
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


BoundingBox BoundingBox::orientBox(const Pos3D& posXYZ, const Pos3D& posOffset, const Pos3D& size,
                                   const DIRECTION direction) {
    return orientBox(posXYZ.getX(), posXYZ.getY(), posXYZ.getZ(), posOffset.getX(), posOffset.getY(), posOffset.getZ(),
                     size.getX(), size.getY(), size.getZ(), direction);
}

BoundingBox BoundingBox::orientBox(const Pos3D& posXYZ, c_int offsetWidth, c_int offsetHeight,
    c_int offsetDepth, c_int width, c_int height, c_int depth, const DIRECTION direction) {
    return orientBox(posXYZ.getX(), posXYZ.getY(), posXYZ.getZ(), offsetWidth, offsetHeight, offsetDepth, width, height,
                     depth, direction);
}

std::string BoundingBox::toString() const {
    return "(" + std::to_string(minX) + ", " + std::to_string(minY) + ", " + std::to_string(minZ) + ") -> (" +
           std::to_string(maxX) + ", " + std::to_string(maxY) + ", " + std::to_string(maxZ) + ")";
}


std::ostream& operator<<(std::ostream& out, const BoundingBox& bBox) {
    out << bBox.toString();
    return out;
}
