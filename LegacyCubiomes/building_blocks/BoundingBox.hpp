#pragma once

#include "LegacyCubiomes/utils/enums.hpp"
#include "LegacyCubiomes/cubiomes/processor.hpp"

struct BoundingBox {
    static const BoundingBox EMPTY;

    int minX{};
    int minY{};
    int minZ{};
    int maxX{};
    int maxY{};
    int maxZ{};

    BoundingBox();
    BoundingBox(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);

    ND bool intersects(const BoundingBox &other) const;

    MU ND bool contains(const BoundingBox &other) const;

    MU void encompass(const BoundingBox &other);

    MU void offset(int x, int y, int z);

    MU ND int getSizeY() const;

    static BoundingBox orientBox(int x, int y, int z, int offsetWidth, int offsetHeight, int offsetDepth,
                                 int width, int height, int depth, DIRECTION direction);

    static BoundingBox orientBox(int x, int y, int z, int width, int height, int depth, DIRECTION direction);

};
