#include "WorldGenMinable.hpp"

#include "common/MathHelper.hpp"
#include "common/constants.hpp"
#include "lce/blocks/__include.hpp"
#include "terrain/ChunkQuadAccessor.hpp"
#include "terrain/World.hpp"


#define USE_CHUNK_QUAD


#ifdef USE_CHUNK_QUAD
static inline void computeVeinBounds(const Pos3D& pos, int blockCount, float theta, int& minX, int& maxX, int& minZ,
                                     int& maxZ) {
    // Endpoints along the line (same math as the generator):
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

        const int baseChunkX = (minX >> 4); // left chunk
        const int baseChunkZ = (minZ >> 4); // top chunk (depending on coordinate system)
        const bool needXPlus = ((maxX >> 4) > baseChunkX);
        const bool needZPlus = ((maxZ >> 4) > baseChunkZ);

        quad = {world, baseChunkX, baseChunkZ, needXPlus, needZPlus};
    }
#endif


    for (int i = 0; i <= this->blockCount; ++i) {
        c_float t = (float) i / (float) this->blockCount;
        c_double centerX = x0 + (x1 - x0) * (double) t;
        c_double centerY = y0 + (y1 - y0) * (double) t;
        c_double centerZ = z0 + (z1 - z0) * (double) t;
        c_double randScale = rng.nextDouble() * (double) this->blockCount / 16.0;
        c_double radius = ((MathHelper::sin(PI_FLOAT * t) + 1.0F) * randScale + 1.0) / 2.0;
        c_int minX = MathHelper::floor(centerX - radius);
        c_int minY = MathHelper::floor(centerY - radius);
        c_int minZ = MathHelper::floor(centerZ - radius);
        c_int maxX = MathHelper::floor(centerX + radius);
        c_int maxY = MathHelper::floor(centerY + radius);
        c_int maxZ = MathHelper::floor(centerZ + radius);


        const double invR = (radius != 0.0) ? (1.0 / radius) : 0.0;

        // start X at minX, normalized nx for (minX + 0.5)
        double nx  = ( (double)minX + 0.5 - centerX ) * invR;
        const double dN = invR;                 // increment for nx/ny/nz when index++

        for (int voxelX = minX; voxelX <= maxX; ++voxelX, nx += dN) {
            const double nx2 = nx * nx;
            if (nx2 >= 1.0) continue;

            // exact Y radius for this X slice (circle cross-section)
            const double yRad = radius * std::sqrt(1.0 - nx2);

            // open interval on cell centers: |(y+0.5) - centerY| < yRad
            int yLo = MathHelper::floor((centerY - 0.5) - yRad) + 1; // ceil(open left)
            int yHi = MathHelper::ceil ((centerY - 0.5) + yRad) - 1; // floor(open right)

            // clamp to previous AABB + world
            if (yLo < minY) yLo = minY;
            if (yHi > maxY) yHi = maxY;
            if (yLo > yHi) continue;

            // normalized ny for yLo
            double ny = ( (double)yLo + 0.5 - centerY ) * invR;

            for (int voxelY = yLo; voxelY <= yHi; ++voxelY, ny += dN) {
                const double ny2   = ny * ny;
                const double nx2y2 = nx2 + ny2;
                if (nx2y2 >= 1.0) continue;

                // exact Z radius for this (X,Y) row
                const double zRad = radius * std::sqrt(1.0 - nx2y2);

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
                    if (nx2y2 + nz2 >= 1.0) continue;

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
