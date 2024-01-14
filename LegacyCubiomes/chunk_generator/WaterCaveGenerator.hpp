#pragma once

#include "LegacyCubiomes/chunk_generator/MapGenBase.hpp"
#include "LegacyCubiomes/mc/blocks.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "WaterCaveGeneratorBase.hpp"
#include <cmath>

class WaterCaveGenerator : public WaterCaveGeneratorBase {
public:
    explicit WaterCaveGenerator(const Generator& generator) : WaterCaveGeneratorBase(generator) {}

    WaterCaveGenerator(CONSOLE console, LCEVERSION version, int64_t seed, WORLDSIZE size, BIOMESCALE scale)
        : WaterCaveGeneratorBase(console, version, seed, size, scale) {}

    static bool canReplaceBlock(uint16_t blockAt);

    void addTunnel(int64_t seedModifier, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D start, float tunnelWidth,
                   float tunnelDirection, float tunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   double tunnelHeightMultiplier);

    void addRoom(int64_t seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, DoublePos3D roomStart, RNG& rng);

    void addFeature(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) final;
};
