#pragma once
#include <iostream>
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

    MU ND inline int getXSize() const {
        return this->maxX - this->minX + 1;
    }

    MU ND inline int getYSize() const {
        return this->maxY - this->minY + 1;
    }

    MU ND inline int getZSize() const {
        return this->maxZ - this->minZ + 1;
    }

    static BoundingBox orientBox(int x, int y, int z, int offsetWidth, int offsetHeight, int offsetDepth,
                                 int width, int height, int depth, DIRECTION direction);

    static BoundingBox orientBox(int x, int y, int z, int width, int height, int depth, DIRECTION direction);

    friend std::ostream &operator<<(std::ostream &out, const BoundingBox &boundingBox) {
        out << "(" << boundingBox.minX << ", " << boundingBox.minY << ", " << boundingBox.minZ << ") -> (" <<
                boundingBox.maxX << ", " << boundingBox.maxY << ", " << boundingBox.maxZ << ")";
        return out;
    }

};
