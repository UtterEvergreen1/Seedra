#pragma once

#include "lce/blocks/__include.hpp"
#include "terrain/biomes/biome.hpp"
#include "terrain/biomes/biomeID.hpp"
#include "terrain/generator.hpp"

class World;

class WorldGenLakes final : public AbstractWorldGenerator {
    Generator const *g;
    lce::Block const *block;

public:
    MU explicit WorldGenLakes(const Generator *g, lce::Block const *blockIn)
        : g(g), block(blockIn) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;
};

class FeaturePositions {
public:
    /**
     * @brief Generates a water lake in the chunk.
     * @param g The generator.
     * @param chunkX The chunk X.
     * @param chunkZ The chunk Z.
     * @return The position of the water lake.
     */
    MU ND static Pos3D waterLake(const Generator *g, const int chunkX, const int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        return waterLake(g, rng, chunkX, chunkZ);
    }

    /**
     * @brief Generates a water lake in the chunk using the given random number generator.
     * @param g The generator.
     * @param rng The random number generator.
     * @param chunkX The chunk X.
     * @param chunkZ The chunk Z.
     * @return The position of the water lake.
     */
    ND static Pos3D waterLake(const Generator *g, RNG &rng, const int chunkX, const int chunkZ) {
        const int biomeAt = g->getBiomeAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if (biomeAt == desert || biomeAt == desert_hills || rng.nextInt(4) != 0) return {};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        return {xPos, yPos, zPos};
    }

    /**
     * @brief Generates a lava lake in the chunk.
     * @param g The generator.
     * @param chunkX The chunk X.
     * @param chunkZ The chunk Z.
     * @return The position of the lava lake.
     */
    MU ND static Pos3D lavaLake(const Generator *g, const int chunkX, const int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        if (!waterLake(g, rng, chunkX, chunkZ).isNull()) return {};

        return lavaLake(rng, chunkX, chunkZ);
    }

    /**
     * @brief Generates a lava lake in the chunk using the given random number generator.
     * @param rng The random number generator.
     * @param chunkX The chunk X.
     * @param chunkZ The chunk Z.
     * @return The position of the lava lake.
     */
    ND static Pos3D lavaLake(RNG &rng, const int chunkX, const int chunkZ) {
        if (rng.nextInt(8) != 0) // no lava lake in chunk
            return {};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(rng.nextInt(120) + 8);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        if (yPos >= 63 && rng.nextInt(10) != 0) // no lava lake above sea level
            return {};

        return {xPos, yPos, zPos};
    }

    /**
     * @brief Generates a dungeon in the chunk.
     * @param g The generator.
     * @param chunkX The chunk X.
     * @param chunkZ The chunk Z.
     * @return The position of the dungeon.
     */
    MU ND static Pos3D dungeon(const Generator *g, const int chunkX, const int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        if (!waterLake(g, rng, chunkX, chunkZ).isNull()) return {};
        if (!lavaLake(rng, chunkX, chunkZ).isNull()) return {};

        return dungeon(rng, chunkX, chunkZ);
    }

    /**
     * @brief Generates a dungeon in the chunk using the given random number generator.
     * @param rng The random number generator.
     * @param chunkX The chunk X.
     * @param chunkZ The chunk Z.
     * @return The position of the dungeon.
     */
    ND static Pos3D dungeon(RNG &rng, const int chunkX, const int chunkZ) {
        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        return {xPos, yPos, zPos};
    }

    MU static void decorations(RNG &rng, MU c_int chunkX, MU c_int chunkZ) {
        // generate ores
        // skip rng for now
        rng.advance<7950>();
    }
};
