#pragma once

#include <climits>
#include <iostream>

#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/enums.hpp"
#include "lce/processor.hpp"


struct BoundingBox {
    static const BoundingBox EMPTY;
    short minX{}, minY{}, minZ{}, maxX{}, maxY{}, maxZ{};

    BoundingBox();
    BoundingBox(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);

    bool operator==(const BoundingBox& other) const;

    ND bool intersects(const BoundingBox& other) const;
    ND bool contains(const BoundingBox& other) const;
    MU void encompass(const BoundingBox& other);

    MU bool isVecInside(Pos3D pos) const;

    MU void offset(int x, int y, int z);
    MU void offsetY(int y);

    MU ND int getXSize() const { return maxX - minX + 1; }
    MU ND int getYSize() const { return maxY - minY + 1; }
    MU ND int getZSize() const { return maxZ - minZ + 1; }
    MU ND int getLength() const { return std::max(maxX - minX, maxZ - minZ); }
    MU ND int getCenterX() const { return (minX + maxX) / 2; }
    MU ND int getCenterZ() const { return (minZ + maxZ) / 2; }

    static BoundingBox makeChunkBox(int xChunk, int zChunk);
    static BoundingBox orientBox(int x, int y, int z, int offsetWidth, int offsetHeight, int offsetDepth, int width,
                                 int height, int depth, FACING direction);

    static BoundingBox orientBox(int x, int y, int z, int width, int height, int depth, FACING direction);
    static BoundingBox orientBox(const Pos3D& xyz, int width, int height, int depth, FACING direction);
    static BoundingBox orientBox(const Pos3D& posXYZ, const Pos3D& posOffset, const Pos3D& size, FACING direction);
    static BoundingBox orientBox(const Pos3D& posXYZ, int offsetWidth, int offsetHeight, int offsetDepth, int width,
                                 int height, int depth, FACING direction);

    MU ND std::string toString() const;

    friend std::ostream& operator<<(std::ostream& out, const BoundingBox& boundingBox);
};
