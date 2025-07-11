#pragma once

#include "common/Pos3DTemplate.hpp"

template <class WorldT, class Pred>
bool scanBoxHull(WorldT& world, const Pos3D& min, const Pos3D& max, Pred&& pred) {
    const int minX = min.x,     maxX = max.x;
    const int minY = min.y - 1, maxY = max.y + 1;
    const int minZ = min.z,     maxZ = max.z;

    auto visit = [&](const int x, const int y, const int z) -> bool {
        return std::forward<Pred>(pred)(Pos3D{x, y, z}, world);
    };

    auto scanFaceXZ = [&](const int y) -> bool { // entire X-Z plane at fixed Y
        for (int x = minX; x < maxX; ++x)
            for (int z = minZ; z < maxZ; ++z)
                if (visit(x, y, z)) return true;
        return false;
    };

    auto scanFaceXY = [&](const int z) -> bool { // entire X-Y plane at fixed Z
        for (int x = minX; x < maxX; ++x)
            for (int y = minY; y <= maxY; ++y)
                if (visit(x, y, z)) return true;
        return false;
    };

    auto scanFaceYZ = [&](const int x) -> bool { // entire Y-Z plane at fixed X
        for (int z = minZ; z < maxZ; ++z)
            for (int y = minY; y <= maxY; ++y)
                if (visit(x, y, z)) return true;
        return false;
    };

    if (scanFaceXZ(maxY))     return true; // 1) top      y = maxY
    if (scanFaceXZ(minY))     return true; // 2) bottom   y = minY
    if (scanFaceYZ(minX))     return true; // 3) west     x = minX
    if (scanFaceYZ(maxX - 1)) return true; // 4) east     x = maxX-1
    if (scanFaceXY(minZ))     return true; // 5) north    z = minZ
    if (scanFaceXY(maxZ - 1)) return true; // 6) south    z = maxZ-1

    return false;
}