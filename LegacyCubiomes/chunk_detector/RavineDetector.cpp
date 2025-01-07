#include "RavineDetector.hpp"


#include "LegacyCubiomes/cubiomes/biomeID.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "LegacyCubiomes/utils/constants.hpp"


void RavineDetector::addTunnel(c_i64 randomSeed, const Pos2D chunk, DoublePos3D tunnel, c_float angle, float slope,
                               float curvature, int tunnelStartSegment, int tunnelEndSegment, c_double widthMultiplier,
                               bool accurate) {

    if (accurate && myG.getLCEVersion() == LCEVERSION::AQUATIC &&
        isOceanic(myG.getBiomeAt(1, (int) tunnel.x, (int) tunnel.z))) {
        return;
    }

    RNG rng;
    rng.setSeed(randomSeed);
    c_auto offsetX = (double) (chunk.x * 16 + 8);
    c_auto offsetZ = (double) (chunk.z * 16 + 8);
    float curvatureChangeRate = 0.0F;
    float slopeChangeRate = 0.0F;

    if (tunnelEndSegment < 1) {
        constexpr int rangeBoundary = RANGE * 16 - 16;
        constexpr int randomRange = rangeBoundary / 4;
        tunnelEndSegment = rangeBoundary - rng.nextInt(randomRange);
    }

    bool isSegmentAtCenter = false;

    if (tunnelStartSegment == -1) {
        tunnelStartSegment = tunnelEndSegment / 2;
        isSegmentAtCenter = true;
    }

    float tunnelRadiusMultiplier = 1.0F;

    for (int segment = 0; segment < 128; ++segment) {
        if (segment == 0 || rng.nextInt(3) == 0) { tunnelRadiusMultiplier = 1.0F + rng.nextFloat() * rng.nextFloat(); }

        rs[segment] = tunnelRadiusMultiplier * tunnelRadiusMultiplier;
    }

    for (; tunnelStartSegment < tunnelEndSegment; tunnelStartSegment++) {
        double adjustedWidth =
                1.5 +
                (double) (MathHelper::sin((float) tunnelStartSegment * PI_FLOAT / (float) tunnelEndSegment) * angle);
        double adjustedHeight = adjustedWidth * widthMultiplier;
        adjustedWidth = adjustedWidth * ((double) rng.nextFloat() * 0.25 + 0.75);
        adjustedHeight = adjustedHeight * ((double) rng.nextFloat() * 0.25 + 0.75);
        c_float cosCurvature = MathHelper::cos(curvature);
        c_float sinCurvature = MathHelper::sin(curvature);
        tunnel.x += (double) (MathHelper::cos(slope) * cosCurvature);
        tunnel.y += (double) sinCurvature;
        tunnel.z += (double) (MathHelper::sin(slope) * cosCurvature);
        curvature = curvature * 0.7F;
        curvature = curvature + slopeChangeRate * 0.05F;
        slope += curvatureChangeRate * 0.05F;
        slopeChangeRate = slopeChangeRate * 0.8F;
        curvatureChangeRate = curvatureChangeRate * 0.5F;
        slopeChangeRate = slopeChangeRate + (rng.nextFloat() - rng.nextFloat()) * rng.nextFloat() * 2.0F;
        curvatureChangeRate = curvatureChangeRate + (rng.nextFloat() - rng.nextFloat()) * rng.nextFloat() * 4.0F;

        if (isSegmentAtCenter || rng.nextInt(4) != 0) {
            c_double distanceX = tunnel.x - offsetX;
            c_double distanceZ = tunnel.z - offsetZ;
            c_auto remainingSegments = (double) (tunnelEndSegment - tunnelStartSegment);
            c_auto maxDistance = (double) (angle + 18.0F);

            if (distanceX * distanceX + distanceZ * distanceZ - remainingSegments * remainingSegments >
                maxDistance * maxDistance) {
                return;
            }

            if (tunnel.x >= offsetX - 16.0 - adjustedWidth * 2.0 && tunnel.z >= offsetZ - 16.0 - adjustedWidth * 2.0 &&
                tunnel.x <= offsetX + 16.0 + adjustedWidth * 2.0 && tunnel.z <= offsetZ + 16.0 + adjustedWidth * 2.0) {
                int startX = (int) floor(tunnel.x - adjustedWidth) - chunk.x * 16 - 1;
                int endX = (int) floor(tunnel.x + adjustedWidth) - chunk.x * 16 + 1;
                int startY = (int) floor(tunnel.y - adjustedHeight) - 1;
                int endY = (int) floor(tunnel.y + adjustedHeight) + 1;
                int startZ = (int) floor(tunnel.z - adjustedWidth) - chunk.z * 16 - 1;
                int endZ = (int) floor(tunnel.z + adjustedWidth) - chunk.z * 16 + 1;

                if (startX < 0) startX = 0;
                if (endX > 16) endX = 16;
                if (startY < 1) startY = 1;
                if (endY > 120) endY = 120;
                if (startZ < 0) startZ = 0;
                if (endZ > 16) endZ = 16;

                int offset = 1;
                for (int x = startX - offset; x < endX + offset; ++x) {
                    for (int z = startZ - offset; z < endZ + offset; ++z) {

                        int worldX = x + chunk.x * 16;
                        int worldZ = z + chunk.z * 16;

                        int cX = worldX >> 4;
                        int cZ = worldZ >> 4;

                        if (cX >= myMinChunkX && cX <= myMaxChunkX && cZ >= myMinChunkZ && cZ <= myMaxChunkZ) {
                            myRavineChunks.insert({cX, cZ});
                        }
                    }
                }

                if (isSegmentAtCenter) { break; }
            }
        }
    }
}


void RavineDetector::addFeature(c_int baseChunkX, c_int baseChunkZ, c_int currentChunkX, c_int currentChunkZ,
                                const bool accurate) {
    if EXPECT_FALSE (myRNG.nextInt(50) == 0) {
        auto tunnelStartX = (double) (baseChunkX * 16 + myRNG.nextInt(16));
        auto tunnelStartY = (double) (myRNG.nextInt(myRNG.nextInt(40) + 8) + 20);
        auto tunnelStartZ = (double) (baseChunkZ * 16 + myRNG.nextInt(16));
        c_float tunnelDirection = myRNG.nextFloat() * (PI_FLOAT * 2.0F);
        c_float tunnelSlope = (myRNG.nextFloat() - 0.5F) * 2.0F / 8.0F;
        c_float tunnelLengthMultiplier = (myRNG.nextFloat() * 2.0F + myRNG.nextFloat()) * 2.0F;
        addTunnel(myRNG.nextLongI(), {currentChunkX, currentChunkZ}, {tunnelStartX, tunnelStartY, tunnelStartZ},
                  tunnelLengthMultiplier, tunnelDirection, tunnelSlope, 0, 0, 3.0, accurate);
    }
}
