#pragma once

#include "LegacyCubiomes/utils/enums.hpp"

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

    [[nodiscard]] bool intersects(const BoundingBox &other) const;

    [[maybe_unused]] [[nodiscard]] bool contains(const BoundingBox &other) const;

    [[maybe_unused]] void encompass(const BoundingBox &other);

    [[maybe_unused]] void offset(int x, int y, int z);

    [[maybe_unused]] [[nodiscard]] int getSizeY() const;

    static BoundingBox orientBox(int x, int y, int z, int offsetWidth, int offsetHeight, int offsetDepth,
                                 int width, int height, int depth, DIRECTION direction);

    static BoundingBox orientBox(int x, int y, int z, int width, int height, int depth, DIRECTION direction);

};
