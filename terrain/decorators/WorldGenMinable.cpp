// Seedra/terrain/decorators/WorldGenMinable.cpp
#include "WorldGenMinable.hpp"

#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "terrain/ChunkQuadAccessor.hpp"
#include "terrain/World.hpp"


#define USE_CHUNK_QUAD



WorldGenMinable::WorldGenMinable(const lce::BlockState block, const int blockCount)
    : oreBlock(block), blockCount(blockCount) {
}


#ifdef USE_CHUNK_QUAD
static inline void computeVeinBounds(const Pos3D& pos, int blockCount, float theta, int& minX, int& maxX, int& minZ,
                                     int& maxZ) {
    // Endpoints along the line
    const float halfSpan = static_cast<float>(blockCount) / 8.0f;
    const double x0 = static_cast<double>(pos.getX() + 8) + MathHelper::sin(theta) * halfSpan;
    const double x1 = static_cast<double>(pos.getX() + 8) - MathHelper::sin(theta) * halfSpan;
    const double z0 = static_cast<double>(pos.getZ() + 8) + MathHelper::cos(theta) * halfSpan;
    const double z1 = static_cast<double>(pos.getZ() + 8) - MathHelper::cos(theta) * halfSpan;

    // Max possible radius: when sin(πt)=+1 and randScale=max = blockCount/16
    // radius_max = ((2 * (B/16)) + 1) / 2 = B/16 + 0.5
    const double rMax = static_cast<double>(blockCount) / 16.0 + 0.5;

    const double minXD = std::min(x0, x1) - rMax;
    const double maxXD = std::max(x0, x1) + rMax;
    const double minZD = std::min(z0, z1) - rMax;
    const double maxZD = std::max(z0, z1) + rMax;

    minX = MathHelper::floor(minXD);
    maxX = MathHelper::floor(maxXD);
    minZ = MathHelper::floor(minZD);
    maxZ = MathHelper::floor(maxZD);
}
#endif




bool WorldGenMinable::generate(World* world, RNG& rng, const Pos3D& pos) const {
    // TODO: jerrin changed PI -> PI_FLOAT to silence warning
    c_float theta = rng.nextFloat() * PI_FLOAT;
    c_float halfSpan = static_cast<float>(this->blockCount) / 8.0F;
    c_auto x0 = (double) ((float) (pos.getX() + 8) + MathHelper::sin(theta) * halfSpan);
    c_auto x1 = (double) ((float) (pos.getX() + 8) - MathHelper::sin(theta) * halfSpan);
    c_auto z0 = (double) ((float) (pos.getZ() + 8) + MathHelper::cos(theta) * halfSpan);
    c_auto z1 = (double) ((float) (pos.getZ() + 8) - MathHelper::cos(theta) * halfSpan);
    c_auto y0 = (double) (pos.getY() + rng.nextInt(3) - 2);
    c_auto y1 = (double) (pos.getY() + rng.nextInt(3) - 2);


#ifdef USE_CHUNK_QUAD
    ChunkQuadAccessor quad;
    {
        int minX, maxX, minZ, maxZ;
        computeVeinBounds(pos, this->blockCount, theta, minX, maxX, minZ, maxZ);

        const int baseChunkX = (minX >> 4);
        const int baseChunkZ = (minZ >> 4);
        const bool needXPlus = ((maxX >> 4) > baseChunkX);
        const bool needZPlus = ((maxZ >> 4) > baseChunkZ);

        quad = {world, baseChunkX, baseChunkZ, needXPlus, needZPlus};
    }
#endif
    int h00 = 128, h01 = 128, h10 = 128, h11 = 128;
    if (auto* c00 = quad.pick(0, 0)) h00 = c00->getHighestYBlock();
    if (auto* c01 = quad.pick(0, 1)) h01 = c01->getHighestYBlock();
    if (auto* c10 = quad.pick(1, 0)) h10 = c10->getHighestYBlock();
    if (auto* c11 = quad.pick(1, 1)) h11 = c11->getHighestYBlock();
    int maxHeight = std::min(  std::max(std::max(h00, h01), std::max(h10, h11)), 128 ) + 5;


    for (int i = 0; i <= this->blockCount; ++i) {
        c_float t = (float) i / (float) this->blockCount;
        c_double centerX = x0 + (x1 - x0) * (double) t;
        c_double centerY = y0 + (y1 - y0) * (double) t;
        c_double centerZ = z0 + (z1 - z0) * (double) t;
        c_double randScale = rng.nextDouble() * (double) this->blockCount / 16.0;
        if (centerY >= maxHeight) continue;

        c_double radius = ((MathHelper::sin(PI_FLOAT * t) + 1.0F) * randScale + 1.0) / 2.0;
        c_int minX = MathHelper::floor(centerX - radius);
        c_int minY = MathHelper::floor(centerY - radius);
        c_int minZ = MathHelper::floor(centerZ - radius);
        c_int maxX = MathHelper::floor(centerX + radius);
        c_int maxY = MathHelper::floor(centerY + radius);
        c_int maxZ = MathHelper::floor(centerZ + radius);


        const double invR = (radius != 0.0) ? (1.0 / radius) : 0.0;
        const double dN = invR; // increment for nx/ny/nz when index++

        // start Y at minY, normalized ny for (minY + 0.5)
        double ny = ( (double)minY + 0.5 - centerY ) * invR;

        for (int voxelY = minY; voxelY <= maxY; ++voxelY, ny += dN) {
            const double ny2 = ny * ny;
            if (ny2 >= 1.0) continue;

            // exact X radius for this Y slice (circle cross-section)
            const double xRad = radius * std::sqrt(1.0 - ny2);

            // open interval on cell centers: |(x+0.5) - centerX| < xRad
            int xLo = MathHelper::floor((centerX - 0.5) - xRad) + 1; // ceil(open left)
            int xHi = MathHelper::ceil ((centerX - 0.5) + xRad) - 1; // floor(open right)

            // clamp to previous AABB
            if (xLo < minX) xLo = minX;
            if (xHi > maxX) xHi = maxX;
            if (xLo > xHi) continue;

            // normalized nx for xLo
            double nx = ( (double)xLo + 0.5 - centerX ) * invR;

            for (int voxelX = xLo; voxelX <= xHi; ++voxelX, nx += dN) {
                const double nx2   = nx * nx;
                const double ny2x2 = ny2 + nx2;
                if (ny2x2 >= 1.0) continue;

                // exact Z radius for this (Y,X) row
                const double zRad = radius * std::sqrt(1.0 - ny2x2);

                // open interval on cell centers: |(z+0.5) - centerZ| < zRad
                int zLo = MathHelper::floor((centerZ - 0.5) - zRad) + 1;
                int zHi = MathHelper::ceil ((centerZ - 0.5) + zRad) - 1;

                if (zLo < minZ) zLo = minZ;
                if (zHi > maxZ) zHi = maxZ;
                if (zLo > zHi) continue;

                // normalized nz for zLo
                double nz = ( (double)zLo + 0.5 - centerZ ) * invR;

                for (int voxelZ = zLo; voxelZ <= zHi; ++voxelZ, nz += dN) {
                    // keep the exact acceptance check for bit-identical behavior
                    const double nz2 = nz * nz;
                    if (ny2x2 + nz2 >= 1.0) continue;

#ifdef USE_CHUNK_QUAD
                    const int blockId = quad.getBlockId(voxelX, voxelY, voxelZ);
#else
                    Pos3D blockPos(voxelX, voxelY, voxelZ);
                    int blockId = world->getBlockId(blockPos);
#endif
                    if (blockId == lce::blocks::BlockID::STONE_ID /* &&
                            (block.getDataTag() == 0 || block.getDataTag() & 1)*/) {
#ifdef USE_CHUNK_QUAD
                        quad.setBlock(voxelX, voxelY, voxelZ, this->oreBlock);
#else
                        world->setBlock(blockPos, this->oreBlock);
#endif
                    }

                }
            }
        }

    }

    return true;
}

