#pragma once

#include "lce/processor.hpp"
#include "terrain/World.hpp"
#include "terrain/carve/AbstractMapGen.hpp"


#pragma once

#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"



class MU AbstractGenFinder {
protected:

    static constexpr int CHUNK_RANGE = 8;

    MU static constexpr float RESERVE_MULTIPLIER = 1.4F;

public:

    const Generator *g;

    RNG rng;
    
    BoundingBox genBounds = BoundingBox::EMPTY;

    explicit AbstractGenFinder(const Generator* g) : g(g) {}

    virtual ~AbstractGenFinder() = default;

    MU ND static Pos2DTemplate<i64> getSeedMultiplier(const Generator* g) {
        RNG _rng;
        _rng.setSeed(g->getWorldSeed());
        Pos2DTemplate<i64> seedMultiplier;
        seedMultiplier.x = _rng.nextLongI();
        seedMultiplier.z = _rng.nextLongI();
        
        return seedMultiplier;
    }

    MU void setupRNG(Pos2DTemplate<i64> seedMultiplier, Pos2D chunkPos) {
        c_auto adjustedX = (i64) chunkPos.x * seedMultiplier.x;
        c_auto adjustedZ = (i64) chunkPos.z * seedMultiplier.z;
        rng.setSeed(adjustedX ^ adjustedZ ^ g->getWorldSeed());
    }

};










class LargeCaveFinder : public AbstractGenFinder {

    static constexpr double CUT_OFF = 13.0;

public:

    struct Results {
        Pos3D range;
        DoublePos3D start;
        DoublePos2D adjusted;
    };
    
    explicit LargeCaveFinder(const Generator* g) : AbstractGenFinder(g) {}
  
    ~LargeCaveFinder() override = default;

    static std::vector<Results> findAllLargeCaves(const Generator* g, AreaRange& range) {
        Pos2D lower = range.getLower();
        Pos2D upper = range.getUpper();

        BoundingBox bounds = BoundingBox::makeChunkBox(lower.x, lower.z);
        bounds.encompass(BoundingBox::makeChunkBox(upper.x, upper.z));

        std::vector<Results> results;

        const Pos2DTemplate<i64> seedMultiplierCave = getSeedMultiplier(g);

        lower = lower - CHUNK_RANGE;
        upper = upper + CHUNK_RANGE;

        LargeCaveFinder caveGen(g);
        for (int x = lower.x; x <= upper.x; x++) {
            c_auto adjustedX = (i64) x * seedMultiplierCave.x;
            for (int z = lower.z; z <= upper.z; z++) {
                c_auto adjustedZ = (i64) z * seedMultiplierCave.z;

                caveGen.rng.setSeed(adjustedX ^ adjustedZ ^ g->getWorldSeed());
                caveGen.genBounds = bounds;
                caveGen.addFeature(Pos2D(x, z), results, true);
            }
        }

        return results;
    }


    void addFeature(Pos2D baseChunk, std::vector<Results>& results, bool accurate) {
        c_int tunnelCount = rng.nextInt(rng.nextInt(rng.nextInt<40>() + 1) + 1);

        if EXPECT_TRUE (rng.nextInt<15>() != 0) { return; }

        for (int currentTunnel = 0; currentTunnel < tunnelCount; ++currentTunnel) {
            DoublePos3D tunnelStart;
            tunnelStart.x = (double) (baseChunk.x * 16 + rng.nextInt<16>());
            tunnelStart.y = (double) (rng.nextInt(rng.nextInt<120>() + 8));
            tunnelStart.z = (double) (baseChunk.z * 16 + rng.nextInt<16>());

            int segmentCount = 1;

            if (rng.nextInt<4>() == 0) {
                i64 seedModifier = rng.nextLongI();
                addTunnel(results, seedModifier, baseChunk, tunnelStart,
                          1.0F + rng.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5, accurate);
                segmentCount = rng.nextInt<4>() + 1;
            }

            for (int currentSegment = 0; currentSegment < segmentCount; ++currentSegment) {
                c_float yaw = rng.nextFloat() * (PI_FLOAT * 2.0F);
                c_float pitch = (rng.nextFloat() - 0.5F) * 2.0F / 8.0F;
                float tunnelLength = rng.nextFloat() * 2.0F + rng.nextFloat();

                if (rng.nextInt(10) == 0) { tunnelLength *= rng.nextFloat() * rng.nextFloat() * 3.0F + 1.0F; }

                addTunnel(results, rng.nextLongI(), baseChunk, tunnelStart, tunnelLength, yaw, pitch, 0, 0, 1.0, accurate);
            }
        }
    }






    void addTunnel(std::vector<Results>& results,
                   i64 theSeedModifier, Pos2D currentChunk, DoublePos3D startPos,
                   float theWidth, float theDirection, float theSlope, int theCurrentSegment,
                   int theMaxSegment, double theHeightMultiplier, bool accurate) {

        Pos2D currentChunkX16 = currentChunk * 16;

        float directionModifier = 0.0F;
        float slopeModifier = 0.0F;
        RNG rng;
        rng.setSeed(theSeedModifier);

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

        int splitPoint = rng.nextInt(theMaxSegment / 2) + theMaxSegment / 4;
        float theMaxSegmentFDivPI = PI_FLOAT / (float)(theMaxSegment);
        bool isTunnelWide = rng.nextInt<6>() == 0;

        for (; theCurrentSegment < theMaxSegment; ++theCurrentSegment) {

            // setup tunnelWidth + tunnelHeight
            c_double adjustedWidth = 1.5 + (double) (MathHelper::sin(
                (float) theCurrentSegment * theMaxSegmentFDivPI) * theWidth);
            c_double adjustedHeight = adjustedWidth * theHeightMultiplier;

            // setup tunnel start
            {
                c_float directionCos = (MathHelper::cos(theSlope));
                startPos.x += (double) (MathHelper::cos(theDirection) * directionCos);
                startPos.y += (double) (MathHelper::sin(theSlope));
                startPos.z += (double) (MathHelper::sin(theDirection) * directionCos);
            }

            // setup tunnel slope
            if (isTunnelWide) {
                theSlope = theSlope * 0.92F;
            } else {
                theSlope = theSlope * 0.7F;
            }
            theSlope = theSlope + slopeModifier * 0.1F;
            slopeModifier = slopeModifier * 0.9F;
            {
                c_float f1_1 = rng.nextFloat();
                c_float f1_2 = rng.nextFloat();
                c_float f1_3 = rng.nextFloat();
                slopeModifier = slopeModifier + (f1_1 - f1_2) * f1_3 * 2.0F;
            }

            // setup tunnel direction
            theDirection += directionModifier * 0.1F;
            directionModifier = directionModifier * 0.75F;
            {
                c_float f2_1 = rng.nextFloat();
                c_float f2_2 = rng.nextFloat();
                c_float f2_3 = rng.nextFloat();
                directionModifier = directionModifier + (f2_1 - f2_2) * f2_3 * 4.0F;
            }

            // split the tunnel (if not main)
            if (!isMainTunnel && theCurrentSegment == splitPoint && theWidth > 1.0F && theMaxSegment > 0) {
                float tunnelWidth1, tunnelWidth2;
                i64 seedModifier1, seedModifier2;

                if (lce::isXbox(g->getConsole())) {
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
                addTunnel(results, seedModifier1, currentChunk, startPos, tunnelWidth1 * 0.5F + 0.5F,
                          theDirection - HALF_PI, theSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0, accurate);
                addTunnel(results, seedModifier2, currentChunk, startPos, tunnelWidth2 * 0.5F + 0.5F,
                          theDirection + HALF_PI, theSlope / 3.0F, theCurrentSegment, theMaxSegment, 1.0, accurate);
                return;
            }

            if (!isMainTunnel && rng.nextInt<4>() == 0) { continue; }

            if (adjustedWidth < CUT_OFF) {
                if (isMainTunnel) { return; }
                continue;
            }

            if (   startPos.x + adjustedWidth < genBounds.minX
                || startPos.x - adjustedWidth > genBounds.maxX
                || startPos.z + adjustedWidth < genBounds.minZ
                || startPos.z - adjustedWidth > genBounds.maxZ) {
                continue;
            }

            Pos3D min;
            Pos3D max;
            min.x = static_cast<int>(floor(startPos.x - adjustedWidth)) - currentChunkX16.x - 1;
            max.x = static_cast<int>(floor(startPos.x + adjustedWidth)) - currentChunkX16.x + 1;
            min.y = static_cast<int>(floor(startPos.y - adjustedHeight)) - 1;
            max.y = static_cast<int>(floor(startPos.y + adjustedHeight)) + 1;
            min.z = static_cast<int>(floor(startPos.z - adjustedWidth)) - currentChunkX16.z - 1;
            max.z = static_cast<int>(floor(startPos.z + adjustedWidth)) - currentChunkX16.z + 1;

            if (min.y < 1) min.y = 1;
            if (max.y > 120) max.y = 120;

            if (!genBounds.isVecInside(min + currentChunkX16) &&
                !genBounds.isVecInside(max + currentChunkX16)) {
                continue;
            }

            if (adjustedWidth > CUT_OFF) {
                results.emplace_back(max - min, startPos, DoublePos2D(adjustedWidth, adjustedHeight));
            }

            // place blocks
            if (isMainTunnel) { return; }
        }
    }


};

