#pragma once

#include "lce/processor.hpp"


namespace finders {
    inline constexpr int CHUNK_RANGE = 8;

    MU inline constexpr float RESERVE_MULTIPLIER = 1.4F;

    struct SeedMul { i64 x, z; };

    inline SeedMul getSeedMultiplier(c_i64 worldSeed) {
        RNG r;
        r.setSeed(static_cast<u64>(worldSeed));
        return { r.nextLongI(), r.nextLongI() };
    }


    struct LargeCaveResult {
        DoublePos2D adjusted{};
        DoublePos3D start{};
        Pos3D range{};
        u32 m_empty;
    };


    template<bool IsXbox>
    class LargeCaveFinder final {
        static constexpr double CUT_OFF = 13.0;

    public:
        i64 m_worldSeed;
        RNG m_rng;
        BoundingBox m_genBounds = BoundingBox::EMPTY;


        explicit LargeCaveFinder(c_i64 _worldSeed) : m_worldSeed(_worldSeed) {}

        static std::vector<LargeCaveResult> findAllLargeCaves(c_i64 worldSeed, const AreaRange& range) {
            Pos2D lower = range.getLower();
            Pos2D upper = range.getUpper();

            BoundingBox bounds = BoundingBox::makeChunkBox(lower.x, lower.z);
            bounds.encompass(BoundingBox::makeChunkBox(upper.x, upper.z));

            std::vector<LargeCaveResult> results;

            const SeedMul seedMultiplierCave = getSeedMultiplier(worldSeed);
            lower = lower - CHUNK_RANGE;
            upper = upper + CHUNK_RANGE;

            LargeCaveFinder caveGen(worldSeed);
            for (int x = lower.x; x <= upper.x; x++) {
                c_auto adjustedX = static_cast<i64>(x) * seedMultiplierCave.x;
                for (int z = lower.z; z <= upper.z; z++) {
                    c_auto adjustedZ = static_cast<i64>(z) * seedMultiplierCave.z;

                    caveGen.m_rng.setSeed(static_cast<u64>(adjustedX ^ adjustedZ ^ worldSeed));
                    caveGen.m_genBounds = bounds;
                    caveGen.addFeature(Pos2D(x, z), results);
                }
            }

            return results;
        }


        void addFeature(const Pos2D baseChunk, std::vector<LargeCaveResult>& results) {
            c_int tunnelCount = m_rng.nextInt(m_rng.nextInt(m_rng.nextInt<40>() + 1) + 1);

            if EXPECT_TRUE (m_rng.nextInt<15>() != 0) { return; }

            for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
                DoublePos3D tStart;
                tStart.x = static_cast<double>(baseChunk.x * 16 + m_rng.nextInt<16>());
                tStart.y = static_cast<double>(m_rng.nextInt(m_rng.nextInt<120>() + 8));
                tStart.z = static_cast<double>(baseChunk.z * 16 + m_rng.nextInt<16>());

                int segmentCount = 1;

                if (m_rng.nextInt<4>() == 0) {
                    c_i64 seedModifier = m_rng.nextLongI();
                    addTunnel(results, seedModifier, baseChunk, tStart,
                              1.0F + m_rng.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
                    segmentCount = m_rng.nextInt<4>() + 1;
                }

                for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
                    c_float tYaw = m_rng.nextFloat() * (PI_FLOAT * 2.0F);
                    c_float tPitch = (m_rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
                    float tWidth = m_rng.nextFloat() * 2.0F + m_rng.nextFloat();

                    if (m_rng.nextInt(10) == 0) { tWidth *= m_rng.nextFloat() * m_rng.nextFloat() * 3.0F + 1.0F; }

                    addTunnel(results, m_rng.nextLongI(), baseChunk, tStart, tWidth, tYaw, tPitch, 0, 0, 1.0);
                }
            }
        }






        void addTunnel(std::vector<LargeCaveResult>& results,
                       c_i64 theSeedModifier, const Pos2D theCurrentChunk, DoublePos3D theStart,
                       c_float theWidth, float theTunnelYaw, float theTunnelPitch, int theCurrentSegment,
                       int theMaxSegment, c_double theHeightMultiplier) {

            const Pos2D currentChunkX16 = theCurrentChunk * 16;

            float yawModifier = 0.0F;
            float pitchModifier = 0.0F;
            RNG rng;
            rng.setSeed(static_cast<u64>(theSeedModifier));

            if (theMaxSegment <= 0) {
                constexpr int RANGE_BOUNDARY = CHUNK_RANGE * 16 - 16;
                constexpr int RANDOM_RANGE = RANGE_BOUNDARY / 4;
                theMaxSegment = RANGE_BOUNDARY - rng.nextInt(RANDOM_RANGE);
            }

            bool isMainTunnel = false;
            if (theCurrentSegment == -1) {
                theCurrentSegment = theMaxSegment / 2;
                isMainTunnel = true;
            }

            c_int splitPoint = rng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
            c_float theMaxSegmentFDivPI = PI_FLOAT / static_cast<float>(theMaxSegment);
            c_bool isTunnelWide = rng.nextInt<6>() == 0;

            for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {

                // setup tunnelWidth + tunnelHeight
                c_double adjustedWidth = 1.5 + static_cast<double>(MathHelper::sin(
                    static_cast<float>(theCurrentSegment) * theMaxSegmentFDivPI) * theWidth);
                c_double adjustedHeight = adjustedWidth * theHeightMultiplier;

                // setup tunnel start
                {
                    c_float directionCos = (MathHelper::cos(theTunnelPitch));
                    theStart.x += static_cast<double>(MathHelper::cos(theTunnelYaw) * directionCos);
                    theStart.y += static_cast<double>(MathHelper::sin(theTunnelPitch));
                    theStart.z += static_cast<double>(MathHelper::sin(theTunnelYaw) * directionCos);
                }

                // setup tunnel slope
                if (isTunnelWide) {
                    theTunnelPitch = theTunnelPitch * 0.92F;
                } else {
                    theTunnelPitch = theTunnelPitch * 0.7F;
                }
                theTunnelPitch = theTunnelPitch + pitchModifier * 0.1F;
                pitchModifier = pitchModifier * 0.9F;
                {
                    c_float f1_1 = rng.nextFloat();
                    c_float f1_2 = rng.nextFloat();
                    c_float f1_3 = rng.nextFloat();
                    pitchModifier = pitchModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
                }

                // setup tunnel direction
                theTunnelYaw += yawModifier * 0.1F;
                yawModifier = yawModifier * 0.75F;
                {
                    c_float f2_1 = rng.nextFloat();
                    c_float f2_2 = rng.nextFloat();
                    c_float f2_3 = rng.nextFloat();
                    yawModifier = yawModifier + (f2_1 - f2_2) * f2_3 * 4.0F;
                }

                // split the tunnel (if not main)
                if (!isMainTunnel && theCurrentSegment == splitPoint && theWidth > 1.0F && theMaxSegment > 0) {
                    float tunnelWidth1, tunnelWidth2;
                    i64 seedModifier1, seedModifier2;

                    if constexpr (IsXbox) {
                        tunnelWidth1 = rng.nextFloat();
                        seedModifier1 = rng.nextLongI();
                        tunnelWidth2 = rng.nextFloat();
                        seedModifier2 = rng.nextLongI();
                    } else {
                        seedModifier1 = rng.nextLongI();
                        tunnelWidth1 = rng.nextFloat();
                        seedModifier2 = rng.nextLongI();
                        tunnelWidth2 = rng.nextFloat();
                    }
                    addTunnel(results, seedModifier1, theCurrentChunk, theStart, tunnelWidth1 * 0.5F + 0.5F,
                              theTunnelYaw - HALF_PI_FLOAT, theTunnelPitch / 3.0F, theCurrentSegment, theMaxSegment, 1.0);
                    addTunnel(results, seedModifier2, theCurrentChunk, theStart, tunnelWidth2 * 0.5F + 0.5F,
                              theTunnelYaw + HALF_PI_FLOAT, theTunnelPitch / 3.0F, theCurrentSegment, theMaxSegment, 1.0);
                    return;
                }

                if (!isMainTunnel && rng.nextInt<4>() == 0) { continue; }

                if (adjustedWidth < CUT_OFF) {
                    if (isMainTunnel) { return; }
                    continue;
                }

                if (   theStart.x + adjustedWidth < m_genBounds.m_minX
                    || theStart.x - adjustedWidth > m_genBounds.m_maxX
                    || theStart.z + adjustedWidth < m_genBounds.m_minZ
                    || theStart.z - adjustedWidth > m_genBounds.m_maxZ) {
                    continue;
                    }

                Pos3D min;
                Pos3D max;
                min.x = static_cast<int>(floor(theStart.x - adjustedWidth)) - currentChunkX16.x - 1;
                max.x = static_cast<int>(floor(theStart.x + adjustedWidth)) - currentChunkX16.x + 1;
                min.y = static_cast<int>(floor(theStart.y - adjustedHeight)) - 1;
                max.y = static_cast<int>(floor(theStart.y + adjustedHeight)) + 1;
                min.z = static_cast<int>(floor(theStart.z - adjustedWidth)) - currentChunkX16.z - 1;
                max.z = static_cast<int>(floor(theStart.z + adjustedWidth)) - currentChunkX16.z + 1;

                // can be assumed after
                if (min.y < 1) min.y = 1;
                if (max.y > 120) max.y = 120;

                if (!m_genBounds.isVecInside(min + currentChunkX16) &&
                    !m_genBounds.isVecInside(max + currentChunkX16)) {
                    continue;
                    }

                if (adjustedWidth > CUT_OFF) {
                    results.emplace_back( DoublePos2D(adjustedWidth, adjustedHeight), theStart, max - min);
                }

                // place blocks
                if (isMainTunnel) { return; }
            }
        }


    };

}
