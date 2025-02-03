#include "BoundingBox.hpp"


const BoundingBox BoundingBox::EMPTY = BoundingBox(SHRT_MAX, SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN, SHRT_MIN);


BoundingBox::BoundingBox() = default;


BoundingBox::BoundingBox(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ)
    : minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ) {}


bool BoundingBox::operator==(const BoundingBox& other) const {
    return minX == other.minX && maxX == other.maxX && minZ == other.minZ && maxZ == other.maxZ && minY == other.minY &&
           maxY == other.maxY;
}

BoundingBox BoundingBox::operator<<(int shiftAmount) const {
    return {minX << shiftAmount, minY << shiftAmount, minZ << shiftAmount, maxX << shiftAmount, maxY << shiftAmount,
            maxZ << shiftAmount};
}

bool BoundingBox::intersects(const BoundingBox& other) const {
    return maxX >= other.minX
        && minX <= other.maxX
        && maxZ >= other.minZ
        && minZ <= other.maxZ
        && maxY >= other.minY
        && minY <= other.maxY;
}


bool BoundingBox::contains(const BoundingBox& other) const {
    return maxX >= other.maxX && minX <= other.minX && maxY >= other.maxY &&
           minY <= other.minY && maxZ >= other.maxZ && minZ <= other.minZ;
}


/// makes "*this" expand such that it contains "other".
void BoundingBox::encompass(const BoundingBox& other) {
    if (other.minX < minX) minX = other.minX;
    if (other.minY < minY) minY = other.minY;
    if (other.minZ < minZ) minZ = other.minZ;
    if (other.maxX > maxX) maxX = other.maxX;
    if (other.maxY > maxY) maxY = other.maxY;
    if (other.maxZ > maxZ) maxZ = other.maxZ;
}


/// makes "*this" expand such that it contains "other". Only y-coords.
MU void BoundingBox::encompassY(const BoundingBox& other) {
    if (other.minY < minY) minY = other.minY;
    if (other.maxY > maxY) maxY = other.maxY;
}


/// makes "*this" shrink such that "other" contains it.
MU void BoundingBox::shrinkToFit(const BoundingBox& other) {
    if (minX < other.minX) minX = other.minX;
    if (minY < other.minY) minY = other.minY;
    if (minZ < other.minZ) minZ = other.minZ;
    if (maxX < other.maxX) maxX = other.maxX;
    if (maxY < other.maxY) maxY = other.maxY;
    if (maxZ < other.maxZ) maxZ = other.maxZ;
}


bool BoundingBox::isVecInside(const Pos3D pos) const {
    return this->maxX >= pos.x && this->minX <= pos.x &&
           this->maxY >= pos.y && this->minY <= pos.y &&
           this->maxZ >= pos.z && this->minZ <= pos.z;
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


BoundingBox BoundingBox::makeChunkBox(c_int xChunk, c_int zChunk) {
    return {xChunk << 4, 0, zChunk << 4, (xChunk << 4) + 15, 255, (zChunk << 4) + 15};
}


BoundingBox BoundingBox::orientBox(c_int x, c_int y, c_int z,
                                   c_int width, c_int height,
                                   c_int depth, const EnumFacing direction) {
    switch (direction) {
        case EnumFacing::NORTH:
            return {x, y, z - depth + 1, x + width - 1, y + height - 1, z};
        case EnumFacing::SOUTH:
            return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
        case EnumFacing::WEST:
            return {x - depth + 1, y, z, x, y + height - 1, z + width - 1};
        default:
        case EnumFacing::EAST:
            return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};
    }
}


BoundingBox BoundingBox::orientBox(const Pos3D& xyz, c_int width, c_int height, c_int depth, const EnumFacing direction) {
    switch (direction) {
        case EnumFacing::NORTH:
            return {xyz.getX(),
                    xyz.getY(),
                    xyz.getZ() - depth + 1,
                    xyz.getX() + width - 1,
                    xyz.getY() + height - 1,
                    xyz.getZ()};
        case EnumFacing::SOUTH:
            return {xyz.getX(),
                    xyz.getY(),
                    xyz.getZ(),
                    xyz.getX() + width - 1,
                    xyz.getY() + height - 1,
                    xyz.getZ() + depth - 1};
        case EnumFacing::WEST:
            return {xyz.getX() - depth + 1,
                    xyz.getY(),
                    xyz.getZ(),
                    xyz.getX(),
                    xyz.getY() + height - 1,
                    xyz.getZ() + width - 1};
        default:
        case EnumFacing::EAST:
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
                                   c_int height, c_int depth, const EnumFacing direction) {
    switch (direction) {
        case EnumFacing::NORTH:
            return {x + offsetWidth,
                    y + offsetHeight,
                    z - depth + 1 + offsetDepth,
                    x + width - 1 + offsetWidth,
                    y + height - 1 + offsetHeight,
                    z + offsetDepth};
        case EnumFacing::SOUTH:
            return {x + offsetWidth,
                    y + offsetHeight,
                    z + offsetDepth,
                    x + width - 1 + offsetWidth,
                    y + height - 1 + offsetHeight,
                    z + depth - 1 + offsetDepth};
        case EnumFacing::WEST:
            return {x - depth + 1 + offsetDepth,
                    y + offsetHeight,
                    z + offsetWidth,
                    x + offsetDepth,
                    y + height - 1 + offsetHeight,
                    z + width - 1 + offsetWidth};
        default:
        case EnumFacing::EAST:
            return {x + offsetDepth,
                    y + offsetHeight,
                    z + offsetWidth,
                    x + depth - 1 + offsetDepth,
                    y + height - 1 + offsetHeight,
                    z + width - 1 + offsetWidth};
    }
}


BoundingBox BoundingBox::orientBox(const Pos3D& posXYZ, const Pos3D& posOffset, const Pos3D& size,
                                   const EnumFacing direction) {
    return orientBox(posXYZ.getX(), posXYZ.getY(), posXYZ.getZ(), posOffset.getX(), posOffset.getY(), posOffset.getZ(),
                     size.getX(), size.getY(), size.getZ(), direction);
}

BoundingBox BoundingBox::orientBox(const Pos3D& posXYZ, c_int offsetWidth, c_int offsetHeight,
    c_int offsetDepth, c_int width, c_int height, c_int depth, const EnumFacing direction) {
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
